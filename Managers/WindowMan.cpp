#include "WindowMan.h"
#include "SettingsMan.h"
#include "FrameMan.h"
#include "UInputMan.h"
#include "ConsoleMan.h"

#include "SDL.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_query.hpp"

namespace RTE {

	void SDLWindowDeleter::operator()(SDL_Window *window) const { SDL_DestroyWindow(window); }
	void SDLRendererDeleter::operator()(SDL_Renderer *renderer) const { SDL_DestroyRenderer(renderer); }
	void SDLTextureDeleter::operator()(SDL_Texture *texture) const { SDL_DestroyTexture(texture); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::Clear() {
		m_PrimaryWindow.reset();
		m_PrimaryRenderer.reset();
		m_PrimaryTexture.reset();
		ClearMultiDisplayData();

		m_AnyWindowHasFocus = false;
		m_FocusLostDueToMovingBetweenGameWindows = false;
		m_ResolutionChanged = false;

		m_NumDisplays = 0;
		m_MaxResX = 0;
		m_MaxResY = 0;
		m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.clear();
		m_CanMultiDisplayFullscreen = false;
		m_DisplayArrangmentLeftMostDisplayIndex = -1;
		m_DisplayArrangementLeftMostOffset = 0;
		m_DisplayArrangementLeftMostOffset = 0;

		m_DisplayIndexPrimaryWindowIsAt = 0;
		m_DisplayWidthPrimaryWindowIsAt = 0;
		m_DisplayHeightPrimaryWindowIsAt = 0;
		m_ResX = c_DefaultResX;
		m_ResY = c_DefaultResY;
		m_ResMultiplier = 1;
		m_EnableVSync = true;
		m_IgnoreMultiDisplays = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ClearMultiDisplayData() {
		m_MultiDisplayTextureOffsets.clear();
		m_MultiDisplayTextures.clear();
		m_MultiDisplayRenderers.clear();
		m_MultiDisplayWindows.clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	WindowMan::WindowMan() {
		Clear();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::Initialize() {
		m_NumDisplays = SDL_GetNumVideoDisplays();

		SDL_Rect currentDisplayBounds;
		SDL_GetDisplayBounds(m_DisplayIndexPrimaryWindowIsAt, &currentDisplayBounds);

		m_DisplayWidthPrimaryWindowIsAt = currentDisplayBounds.w;
		m_DisplayHeightPrimaryWindowIsAt = currentDisplayBounds.h;

		MapDisplays(false);

		ValidateResolution(m_ResX, m_ResY, m_ResMultiplier);

		CreatePrimaryWindow();
		CreatePrimaryRenderer();
		CreatePrimaryTexture();

		m_DisplayIndexPrimaryWindowIsAt = SDL_GetWindowDisplayIndex(m_PrimaryWindow.get());

		if (FullyCoversAllDisplays()) {
			ChangeResolutionToMultiDisplayFullscreen(m_ResMultiplier);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CreatePrimaryWindow() {
		const char *windowTitle = "Cortex Command Community Project";
		int windowPosX = (m_ResX * m_ResMultiplier <= m_DisplayWidthPrimaryWindowIsAt) ? SDL_WINDOWPOS_CENTERED : (m_MaxResX - (m_ResX * m_ResMultiplier)) / 2;
		int windowPosY = SDL_WINDOWPOS_CENTERED;
		int windowFlags = SDL_WINDOW_SHOWN;

		if (FullyCoversDisplayWindowIsAtOnly()) {
			windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		m_PrimaryWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow(windowTitle, windowPosX, windowPosY, m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier, windowFlags), SDLWindowDeleter());
		if (!m_PrimaryWindow) {
			ShowMessageBox("Unable to create window because:\n" + std::string(SDL_GetError()) + "!\n\nTrying to revert to defaults!");

			m_ResX = c_DefaultResX;
			m_ResY = c_DefaultResY;
			m_ResMultiplier = 1;
			g_SettingsMan.SetSettingsNeedOverwrite();

			m_PrimaryWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier, SDL_WINDOW_SHOWN), SDLWindowDeleter());
			if (!m_PrimaryWindow) {
				RTEAbort("Failed to create window because:\n" + std::string(SDL_GetError()));
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CreatePrimaryRenderer() {
		int renderFlags = SDL_RENDERER_ACCELERATED;
		if (m_EnableVSync) {
			renderFlags |= SDL_RENDERER_PRESENTVSYNC;
		}

		m_PrimaryRenderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(m_PrimaryWindow.get(), -1, renderFlags), SDLRendererDeleter());
		if (!m_PrimaryRenderer) {
			ShowMessageBox("Unable to create hardware accelerated renderer because:\n" + std::string(SDL_GetError()) + "!\n\nTrying to revert to software rendering!");
			m_PrimaryRenderer = std::shared_ptr<SDL_Renderer>(SDL_CreateRenderer(m_PrimaryWindow.get(), -1, SDL_RENDERER_SOFTWARE), SDLRendererDeleter());

			if (!m_PrimaryRenderer) {
				RTEAbort("Failed to initialize renderer!\nAre you sure this is a computer?");
			}
		}

		SDL_RenderSetIntegerScale(m_PrimaryRenderer.get(), SDL_TRUE);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CreatePrimaryTexture() {
		m_PrimaryTexture = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTexture(m_PrimaryRenderer.get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_ResX, m_ResY));
		if (!m_PrimaryTexture) {
			RTEAbort("Failed to create texture because:\n" + std::string(SDL_GetError()));
		}
		SDL_RenderSetLogicalSize(m_PrimaryRenderer.get(), m_ResX, m_ResY);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CreateMultiDisplayTextures() {
		m_MultiDisplayTextures.resize(m_MultiDisplayTextureOffsets.size());
		for (size_t i = 0; i < m_MultiDisplayTextures.size(); ++i) {
			m_MultiDisplayTextures[i] = std::unique_ptr<SDL_Texture, SDLTextureDeleter>(SDL_CreateTexture(m_MultiDisplayRenderers[i].get(), SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_MultiDisplayTextureOffsets[i].w, m_MultiDisplayTextureOffsets[i].h));
			if (!m_MultiDisplayTextures[i]) {
				RTEAbort("Failed to create texture for multi-display because:\n" + std::string(SDL_GetError()));
			}
			SDL_RenderSetLogicalSize(m_MultiDisplayRenderers[i].get(), m_MultiDisplayTextureOffsets[i].w, m_MultiDisplayTextureOffsets[i].h);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::SetVSyncEnabled(bool enable) {
		m_EnableVSync = enable;

		int sdlEnable = m_EnableVSync ? SDL_TRUE : SDL_FALSE;
		int result = -1;

#if	SDL_VERSION_ATLEAST(2, 0, 18)
		// Setting VSync per renderer is introduced in 2.0.18.
		if (!m_MultiDisplayRenderers.empty()) {
			for (const auto &renderer : m_MultiDisplayRenderers) {
				result = SDL_RenderSetVSync(renderer.get(), sdlEnable);

				if (result != 0) {
					break;
				}
			}
		} else {
			result = SDL_RenderSetVSync(m_PrimaryRenderer.get(), sdlEnable);
		}
#endif

		if (result != 0) {
			g_ConsoleMan.PrintString("ERROR: Unable to change VSync mode at runtime! The change will be applied after restarting!");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::UpdateInfoOfDisplayPrimaryWindowIsAt() {
		m_DisplayIndexPrimaryWindowIsAt = SDL_GetWindowDisplayIndex(m_PrimaryWindow.get());

		SDL_Rect currentDisplayBounds;
		SDL_GetDisplayBounds(m_DisplayIndexPrimaryWindowIsAt, &currentDisplayBounds);

		m_DisplayWidthPrimaryWindowIsAt = currentDisplayBounds.w;
		m_DisplayHeightPrimaryWindowIsAt = currentDisplayBounds.h;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::MapDisplays(bool updateInfoOfDisplayPrimaryWindowIsAt) {
		if (updateInfoOfDisplayPrimaryWindowIsAt) {
			UpdateInfoOfDisplayPrimaryWindowIsAt();
		}

		m_NumDisplays = SDL_GetNumVideoDisplays();

		bool mappingErrorOrOnlyOneDisplay = false;
		std::string errorMsg = "";

		if (!m_IgnoreMultiDisplays) {
			m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.clear();

			int leftMostOffset = 0;
			int topMostOffset = std::numeric_limits<int>::max();
			int maxHeight = std::numeric_limits<int>::min();
			int maxUsableHeight = std::numeric_limits<int>::max();
			int minHeightDisplayIndex = -1;
			int totalWidth = 0;

			for (int displayIndex = 0; displayIndex < m_NumDisplays; ++displayIndex) {
				SDL_Rect displayBounds;
				if (SDL_GetDisplayBounds(displayIndex, &displayBounds) == 0) {
					m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.emplace_back(displayIndex, displayBounds);

					leftMostOffset = std::min(leftMostOffset, displayBounds.x);
					topMostOffset = std::min(topMostOffset, displayBounds.y);
					maxHeight = std::max(maxHeight, displayBounds.h);

					int prevMinHeight = maxUsableHeight;
					maxUsableHeight = std::min(maxUsableHeight, displayBounds.h);

					if (maxUsableHeight < prevMinHeight) {
						minHeightDisplayIndex = displayIndex;
					}

					totalWidth += displayBounds.w;
				} else {
					errorMsg = "Failed to get resolution of display " + std::to_string(displayIndex);
					mappingErrorOrOnlyOneDisplay = true;
				}
			}

			if (m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.size() > 1) {
				std::stable_sort(m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.begin(), m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.end(),
					[](auto left, auto right) {
						return left.second.x < right.second.x;
					}
				);

				for (const auto &[displayIndex, displayBounds] : m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen) {
#if	SDL_VERSION_ATLEAST(2, 24, 0)
					m_DisplayArrangmentLeftMostDisplayIndex = SDL_GetRectDisplayIndex(&displayBounds);
					if (m_DisplayArrangmentLeftMostDisplayIndex >= 0) {
#else
					// This doesn't return the nearest display index to the point but should still be reliable enough for reasonable display arrangements.
					SDL_Point testPoint = { leftMostOffset + 1, topMostOffset + 1 };
					if (SDL_PointInRect(&testPoint, &displayBounds) == SDL_TRUE) {
#endif
						m_DisplayArrangmentLeftMostDisplayIndex = displayIndex;
						break;
					}
				}
				if (m_DisplayArrangmentLeftMostDisplayIndex >= 0) {
					m_MaxResX = totalWidth;
					m_MaxResY = maxHeight;
					m_DisplayArrangementLeftMostOffset = leftMostOffset;
					m_DisplayArrangementTopMostOffset = topMostOffset;
					m_CanMultiDisplayFullscreen = true;
				} else {
					mappingErrorOrOnlyOneDisplay = true;
				}
			} else {
				mappingErrorOrOnlyOneDisplay = true;
			}
		}

		if (m_IgnoreMultiDisplays || mappingErrorOrOnlyOneDisplay) {
			m_MaxResX = m_DisplayWidthPrimaryWindowIsAt;
			m_MaxResY = m_DisplayHeightPrimaryWindowIsAt;
			m_NumDisplays = 1;
			m_DisplayArrangementLeftMostOffset = -1;
			m_DisplayArrangementTopMostOffset = -1;
			m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen.clear();
			m_CanMultiDisplayFullscreen = false;
			if (!m_IgnoreMultiDisplays) {
				ShowMessageBox("Failed to map displays for multi-display fullscreen" + (!errorMsg.empty() ? (" because:\n\n" + errorMsg) : "") + "!\n\nFullscreen will be limited to the display the window is positioned at!");
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ValidateResolution(int &resX, int &resY, int &resMultiplier) const {
		bool settingsNeedOverwrite = false;

		if (resX * resMultiplier > m_MaxResX || resY * resMultiplier > m_MaxResY) {
			settingsNeedOverwrite = true;
			resX = m_MaxResX / resMultiplier;
			resY = m_MaxResY / resMultiplier;
			ShowMessageBox("Resolution too high to fit display, overriding to fit!");
		}
		if (settingsNeedOverwrite) {
			g_SettingsMan.SetSettingsNeedOverwrite();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::AttemptToRevertToPreviousResolution(bool revertToDefaults) {
		SDL_SetWindowSize(m_PrimaryWindow.get(), m_ResX * m_ResMultiplier, m_ResY * m_ResMultiplier);

		int result = SDL_SetWindowFullscreen(m_PrimaryWindow.get(), FullyCoversDisplayWindowIsAtOnly() ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) != 0;

		if (!revertToDefaults && result != 0) {
			ShowMessageBox("Failed to switch to new resolution, reverted back to previous setting! Attempting to revert to defaults!");

			m_ResX = c_DefaultResX;
			m_ResY = c_DefaultResY;
			m_ResMultiplier = 1;
			g_SettingsMan.UpdateSettingsFile();

			AttemptToRevertToPreviousResolution(true);
		} else if (result != 0) {
			RTEAbort("Unable to revert to previous resolution or defaults because : \n" + std::string(SDL_GetError()) + "!");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ChangeResolution(int newResX, int newResY, bool upscaled, bool displaysAlreadyMapped) {
		int newResMultiplier = upscaled ? 2 : 1;

		if (m_ResX == newResX && m_ResY == newResY && m_ResMultiplier == newResMultiplier) {
			return;
		}

		bool onlyResMultiplierChange = (m_ResX == newResX) && (m_ResY == newResY) && (m_ResMultiplier != newResMultiplier);

		ClearMultiDisplayData();

		if (!displaysAlreadyMapped) {
			MapDisplays();
		}
		ValidateResolution(newResX, newResY, newResMultiplier);

		bool newResFullyCoversDisplayPrimaryWindowIsAtOnly = (newResX * newResMultiplier == m_DisplayWidthPrimaryWindowIsAt) && (newResY * newResMultiplier == m_DisplayHeightPrimaryWindowIsAt);
		bool newResFullyCoversAllDisplays = m_CanMultiDisplayFullscreen && (m_NumDisplays > 1) && (newResX * newResMultiplier == m_MaxResX) && (newResY * newResMultiplier == m_MaxResY);

		if ((newResFullyCoversAllDisplays && !ChangeResolutionToMultiDisplayFullscreen(newResMultiplier)) || (newResFullyCoversDisplayPrimaryWindowIsAtOnly && SDL_SetWindowFullscreen(m_PrimaryWindow.get(), SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)) {
			AttemptToRevertToPreviousResolution();
		} else if (!newResFullyCoversDisplayPrimaryWindowIsAtOnly && !newResFullyCoversAllDisplays) {
			SDL_SetWindowFullscreen(m_PrimaryWindow.get(), 0);
			SDL_SetWindowSize(m_PrimaryWindow.get(), newResX * newResMultiplier, newResY * newResMultiplier);
			SDL_RestoreWindow(m_PrimaryWindow.get());
			SDL_SetWindowBordered(m_PrimaryWindow.get(), SDL_TRUE);
			SDL_SetWindowPosition(m_PrimaryWindow.get(), SDL_WINDOWPOS_CENTERED_DISPLAY(m_DisplayIndexPrimaryWindowIsAt), SDL_WINDOWPOS_CENTERED_DISPLAY(m_DisplayIndexPrimaryWindowIsAt));
		}

		m_ResX = newResX;
		m_ResY = newResY;
		m_ResMultiplier = newResMultiplier;

		g_SettingsMan.UpdateSettingsFile();

		if (onlyResMultiplierChange) {
			if (!newResFullyCoversAllDisplays) {
				CreatePrimaryTexture();
			}
			g_ConsoleMan.PrintString("SYSTEM: Switched to different windowed mode multiplier.");
		} else {
			m_ResolutionChanged = true;
			g_FrameMan.CreateBackBuffers();

			if (newResFullyCoversAllDisplays) {
				CreateMultiDisplayTextures();
			} else {
				CreatePrimaryTexture();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ChangeResolutionMultiplier() {
		int newResMultiplier = (m_ResMultiplier == 1) ? 2 : 1;

		MapDisplays();

		if (m_MaxResX < m_ResX * newResMultiplier || m_MaxResY < m_ResY * newResMultiplier ) {
			ShowMessageBox("Requested resolution multiplier will result in game window exceeding display bounds!\nNo change will be made!\n\nNOTE: To toggle fullscreen, use the button in the Options & Controls Menu!");
			return;
		}
		ChangeResolution(m_ResX, m_ResY, newResMultiplier > 1, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool WindowMan::ChangeResolutionToMultiDisplayFullscreen(int resMultiplier) {
		if (!m_CanMultiDisplayFullscreen) {
			return false;
		}

		int windowPrevPositionX = 0;
		int windowPrevPositionY = 0;
		SDL_GetWindowPosition(m_PrimaryWindow.get(), &windowPrevPositionX, &windowPrevPositionY);

		// Move the window to the detected leftmost display to avoid all the headaches.
		if (m_DisplayIndexPrimaryWindowIsAt != m_DisplayArrangmentLeftMostDisplayIndex) {
			SDL_SetWindowPosition(m_PrimaryWindow.get(), SDL_WINDOWPOS_CENTERED_DISPLAY(m_DisplayArrangmentLeftMostDisplayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(m_DisplayArrangmentLeftMostDisplayIndex));
			m_DisplayIndexPrimaryWindowIsAt = m_DisplayArrangmentLeftMostDisplayIndex;
		}

		bool errorSettingFullscreen = false;

		for (const auto &[displayIndex, displayBounds] : m_ValidDisplayIndicesAndBoundsForMultiDisplayFullscreen) {
			int displayOffsetX = displayBounds.x;
			int displayOffsetY = displayBounds.y;
			int displayWidth = displayBounds.w;
			int displayHeight = displayBounds.h;

			if (displayIndex == m_DisplayIndexPrimaryWindowIsAt) {
				m_MultiDisplayWindows.emplace_back(m_PrimaryWindow);
				m_MultiDisplayRenderers.emplace_back(m_PrimaryRenderer);
			} else {
				m_MultiDisplayWindows.emplace_back(SDL_CreateWindow(nullptr, displayOffsetX, displayOffsetY, displayWidth, displayHeight, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SKIP_TASKBAR), SDLWindowDeleter());
				if (m_MultiDisplayWindows.back()) {
					m_MultiDisplayRenderers.emplace_back(SDL_CreateRenderer(m_MultiDisplayWindows.back().get(), -1, SDL_RENDERER_ACCELERATED), SDLRendererDeleter());
					if (!m_MultiDisplayRenderers.back()) {
						errorSettingFullscreen = true;
					}
				} else {
					errorSettingFullscreen = true;
				}
				if (errorSettingFullscreen) {
					break;
				}
			}

			int textureOffsetX = (displayOffsetX - m_DisplayArrangementLeftMostOffset);
			int textureOffsetY = (displayOffsetY - m_DisplayArrangementTopMostOffset);

			m_MultiDisplayTextureOffsets.emplace_back(SDL_Rect{
				textureOffsetX / resMultiplier,
				// Sometimes an odd Y offset implodes all the things, depending on the stupidity of the arrangement and what display is primary.
				// Sometimes it needs to be in multiples of 4 for reasons unknown to man, so we're just gonna go with this and hope for the best, for now at least.
				RoundToNearestMultiple(textureOffsetY, 2) / resMultiplier,
				displayWidth / resMultiplier,
				displayHeight / resMultiplier
			});
		}

		if (errorSettingFullscreen) {
			ClearMultiDisplayData();
			SDL_SetWindowPosition(m_PrimaryWindow.get(), windowPrevPositionX, windowPrevPositionY);
			return false;
		}

		CreateMultiDisplayTextures();
		SDL_SetWindowFullscreen(m_PrimaryWindow.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::CompleteResolutionChange() {
		m_ResolutionChanged = false;
		g_ConsoleMan.PrintString("SYSTEM: Switched to different resolution.");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::DisplaySwitchIn() const {
		g_UInputMan.DisableMouseMoving(false);
		if (!m_MultiDisplayWindows.empty()) {
			SDL_RaiseWindow(m_PrimaryWindow.get());
			for (auto &window : m_MultiDisplayWindows) {
				SDL_RaiseWindow(window.get());
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::DisplaySwitchOut() const {
		g_UInputMan.DisableMouseMoving(true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::HandleWindowEvent(const SDL_Event &windowEvent) {
		switch (windowEvent.window.event) {
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				if (!m_FocusLostDueToMovingBetweenGameWindows) {
					DisplaySwitchIn();
				}
				m_AnyWindowHasFocus = true;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				m_AnyWindowHasFocus = false;
				m_FocusLostDueToMovingBetweenGameWindows = true;
				DisplaySwitchOut();
				break;
			case SDL_WINDOWEVENT_ENTER:
				if (m_AnyWindowHasFocus && FullyCoversAllDisplays()) {
					SDL_RaiseWindow(SDL_GetWindowFromID(windowEvent.window.windowID));
					SDL_SetWindowInputFocus(SDL_GetWindowFromID(windowEvent.window.windowID));
					m_AnyWindowHasFocus = true;
				}
				break;
			default:
				break;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::Update() {
		m_FocusLostDueToMovingBetweenGameWindows = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::ClearRenderer() const {
		if (m_MultiDisplayRenderers.empty()) {
			SDL_RenderClear(m_PrimaryRenderer.get());
		} else {
			for (const auto &renderer : m_MultiDisplayRenderers) {
				SDL_RenderClear(renderer.get());
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void WindowMan::UploadFrame() const {
		const BITMAP *backbuffer = g_FrameMan.GetBackBuffer32();

		if (m_MultiDisplayTextureOffsets.empty()) {
			SDL_UpdateTexture(m_PrimaryTexture.get(), nullptr, backbuffer->line[0], backbuffer->w * 4);
			SDL_RenderCopy(m_PrimaryRenderer.get(), m_PrimaryTexture.get(), nullptr, nullptr);
			SDL_RenderPresent(m_PrimaryRenderer.get());
		} else {
			for (size_t i = 0; i < m_MultiDisplayTextureOffsets.size(); ++i) {
				int displayOffsetX = m_MultiDisplayTextureOffsets[i].x * 4;
				SDL_UpdateTexture(m_MultiDisplayTextures[i].get(), nullptr, backbuffer->line[m_MultiDisplayTextureOffsets[i].y] + displayOffsetX, backbuffer->w * 4);
				SDL_RenderCopy(m_MultiDisplayRenderers[i].get(), m_MultiDisplayTextures[i].get(), nullptr, nullptr);
				SDL_RenderPresent(m_MultiDisplayRenderers[i].get());
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glm::vec4 WindowMan::GetViewportLetterbox(int resX, int resY, int windowW, int windowH) {
		float aspectRatio = resX / static_cast<float>(resY);
		int width = windowW;
		int height = width / aspectRatio + 0.5F;

		if (height > windowH) {
			height = windowH;
			width = height * aspectRatio + 0.5F;
		}

		int offsetX = (windowW / 2) - (width / 2);
		int offsetY = (windowH / 2) - (height / 2);
		return glm::vec4(offsetX, windowH - offsetY - height, width, height);
	}
}
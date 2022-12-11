#ifndef _RTELUAADAPTERDEFINITIONS_
#define _RTELUAADAPTERDEFINITIONS_

#include "LuabindDefinitions.h"

#include "ActivityMan.h"
#include "AudioMan.h"
#include "ConsoleMan.h"
#include "FrameMan.h"
#include "MetaMan.h"
#include "MovableMan.h"
#include "PostProcessMan.h"
#include "PresetMan.h"
#include "PrimitiveMan.h"
#include "SceneMan.h"
#include "SettingsMan.h"
#include "TimerMan.h"
#include "UInputMan.h"

#include "Box.h"
#include "Controller.h"
#include "DataModule.h"

#include "GUIBanner.h"
#include "BuyMenuGUI.h"
#include "SceneEditorGUI.h"

#include "GameActivity.h"
#include "GAScripted.h"
#include "ActorEditor.h"

#include "Entity.h"
#include "ACDropShip.h"
#include "ACrab.h"
#include "ACraft.h"
#include "ACRocket.h"
#include "Actor.h"
#include "ADoor.h"
#include "AEmitter.h"
#include "AHuman.h"
#include "Arm.h"
#include "AtomGroup.h"
#include "Attachable.h"
#include "Deployment.h"
#include "Emission.h"
#include "Gib.h"
#include "GlobalScript.h"
#include "HDFirearm.h"
#include "HeldDevice.h"
#include "Leg.h"
#include "LimbPath.h"
#include "Magazine.h"
#include "Material.h"
#include "MetaPlayer.h"
#include "MOSParticle.h"
#include "MOPixel.h"
#include "MOSprite.h"
#include "MOSRotating.h"
#include "MovableObject.h"
#include "PEmitter.h"
#include "Round.h"
#include "Scene.h"
#include "SceneObject.h"
#include "SLBackground.h"
#include "SLTerrain.h"
#include "SoundContainer.h"
#include "TerrainObject.h"
#include "TDExplosive.h"
#include "ThrownDevice.h"
#include "Turret.h"
#include "PieMenu.h"
#include "PieSlice.h"

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

struct lua_State;

namespace RTE {

	class LuabindObjectWrapper;

#pragma region Entity Lua Adapter Macros
	/// <summary>
	/// Convenience macro to generate preset clone-create adapter functions that will return the exact pre-cast types, so we don't have to do: myNewActor = ToActor(PresetMan:GetPreset("AHuman", "Soldier Light", "All")):Clone()
	/// But can instead do: myNewActor = CreateActor("Soldier Light", "All");
	/// Or even: myNewActor = CreateActor("Soldier Light");
	/// Or for a randomly selected Preset within a group: myNewActor = RandomActor("Light Troops");
	/// </summary>
	#define LuaEntityCreateFunctionsDeclarationsForType(TYPE)								\
		static TYPE * Create##TYPE(std::string preseName, std::string moduleName);			\
		static TYPE * Create##TYPE(std::string preset);										\
		static TYPE * Random##TYPE(std::string groupName, int moduleSpaceID);				\
		static TYPE * Random##TYPE(std::string groupName, std::string dataModuleName);		\
		static TYPE * Random##TYPE(std::string groupName)

	struct LuaAdaptersEntityCreate {
		LuaEntityCreateFunctionsDeclarationsForType(SoundContainer);
		LuaEntityCreateFunctionsDeclarationsForType(Attachable);
		LuaEntityCreateFunctionsDeclarationsForType(Arm);
		LuaEntityCreateFunctionsDeclarationsForType(Leg);
		LuaEntityCreateFunctionsDeclarationsForType(AEmitter);
		LuaEntityCreateFunctionsDeclarationsForType(Turret);
		LuaEntityCreateFunctionsDeclarationsForType(Actor);
		LuaEntityCreateFunctionsDeclarationsForType(ADoor);
		LuaEntityCreateFunctionsDeclarationsForType(AHuman);
		LuaEntityCreateFunctionsDeclarationsForType(ACrab);
		LuaEntityCreateFunctionsDeclarationsForType(ACraft);
		LuaEntityCreateFunctionsDeclarationsForType(ACDropShip);
		LuaEntityCreateFunctionsDeclarationsForType(ACRocket);
		LuaEntityCreateFunctionsDeclarationsForType(MOSParticle);
		LuaEntityCreateFunctionsDeclarationsForType(MOSRotating);
		LuaEntityCreateFunctionsDeclarationsForType(MOPixel);
		LuaEntityCreateFunctionsDeclarationsForType(Scene);
		LuaEntityCreateFunctionsDeclarationsForType(HeldDevice);
		LuaEntityCreateFunctionsDeclarationsForType(Round);
		LuaEntityCreateFunctionsDeclarationsForType(Magazine);
		LuaEntityCreateFunctionsDeclarationsForType(HDFirearm);
		LuaEntityCreateFunctionsDeclarationsForType(ThrownDevice);
		LuaEntityCreateFunctionsDeclarationsForType(TDExplosive);
		LuaEntityCreateFunctionsDeclarationsForType(TerrainObject);
		LuaEntityCreateFunctionsDeclarationsForType(PEmitter);
		LuaEntityCreateFunctionsDeclarationsForType(PieSlice);
		LuaEntityCreateFunctionsDeclarationsForType(PieMenu);
	};

	/// <summary>
	/// Convenience macro to generate a preset clone adapter function for a type.
	/// </summary>
	#define LuaEntityCloneFunctionDeclarationForType(TYPE) \
		static TYPE * Clone##TYPE(const TYPE *thisEntity)

	struct LuaAdaptersEntityClone {
		LuaEntityCloneFunctionDeclarationForType(Entity);
		LuaEntityCloneFunctionDeclarationForType(SoundContainer);
		LuaEntityCloneFunctionDeclarationForType(SceneObject);
		LuaEntityCloneFunctionDeclarationForType(MovableObject);
		LuaEntityCloneFunctionDeclarationForType(Attachable);
		LuaEntityCloneFunctionDeclarationForType(Arm);
		LuaEntityCloneFunctionDeclarationForType(Leg);
		LuaEntityCloneFunctionDeclarationForType(Emission);
		LuaEntityCloneFunctionDeclarationForType(AEmitter);
		LuaEntityCloneFunctionDeclarationForType(Turret);
		LuaEntityCloneFunctionDeclarationForType(Actor);
		LuaEntityCloneFunctionDeclarationForType(ADoor);
		LuaEntityCloneFunctionDeclarationForType(AHuman);
		LuaEntityCloneFunctionDeclarationForType(ACrab);
		LuaEntityCloneFunctionDeclarationForType(ACraft);
		LuaEntityCloneFunctionDeclarationForType(ACDropShip);
		LuaEntityCloneFunctionDeclarationForType(ACRocket);
		LuaEntityCloneFunctionDeclarationForType(MOSParticle);
		LuaEntityCloneFunctionDeclarationForType(MOSRotating);
		LuaEntityCloneFunctionDeclarationForType(MOPixel);
		LuaEntityCloneFunctionDeclarationForType(Scene);
		LuaEntityCloneFunctionDeclarationForType(HeldDevice);
		LuaEntityCloneFunctionDeclarationForType(Round);
		LuaEntityCloneFunctionDeclarationForType(Magazine);
		LuaEntityCloneFunctionDeclarationForType(HDFirearm);
		LuaEntityCloneFunctionDeclarationForType(ThrownDevice);
		LuaEntityCloneFunctionDeclarationForType(TDExplosive);
		LuaEntityCloneFunctionDeclarationForType(TerrainObject);
		LuaEntityCloneFunctionDeclarationForType(PEmitter);
		LuaEntityCloneFunctionDeclarationForType(PieSlice);
		LuaEntityCloneFunctionDeclarationForType(PieMenu);
	};

	/// <summary>
	/// Convenience macro to generate type casting adapter functions for a type.
	/// </summary>
	#define LuaEntityCastFunctionsDeclarationsForType(TYPE)													\
		static TYPE * To##TYPE(Entity *entity);																\
		static const TYPE * ToConst##TYPE(const Entity *entity);											\
		static bool Is##TYPE(Entity *entity);																\
		static LuabindObjectWrapper * ToLuabindObject##TYPE(Entity *entity, lua_State *luaState)

	struct LuaAdaptersEntityCast {
		static std::unordered_map<std::string, std::function<LuabindObjectWrapper * (Entity *, lua_State *)>> s_EntityToLuabindObjectCastFunctions; //!< Map of preset names to casting methods for ensuring objects are downcast properly when passed into Lua.

		LuaEntityCastFunctionsDeclarationsForType(Entity);
		LuaEntityCastFunctionsDeclarationsForType(SoundContainer);
		LuaEntityCastFunctionsDeclarationsForType(SceneObject);
		LuaEntityCastFunctionsDeclarationsForType(MovableObject);
		LuaEntityCastFunctionsDeclarationsForType(Attachable);
		LuaEntityCastFunctionsDeclarationsForType(Arm);
		LuaEntityCastFunctionsDeclarationsForType(Leg);
		LuaEntityCastFunctionsDeclarationsForType(Emission);
		LuaEntityCastFunctionsDeclarationsForType(AEmitter);
		LuaEntityCastFunctionsDeclarationsForType(Turret);
		LuaEntityCastFunctionsDeclarationsForType(Actor);
		LuaEntityCastFunctionsDeclarationsForType(ADoor);
		LuaEntityCastFunctionsDeclarationsForType(AHuman);
		LuaEntityCastFunctionsDeclarationsForType(ACrab);
		LuaEntityCastFunctionsDeclarationsForType(ACraft);
		LuaEntityCastFunctionsDeclarationsForType(ACDropShip);
		LuaEntityCastFunctionsDeclarationsForType(ACRocket);
		LuaEntityCastFunctionsDeclarationsForType(MOSParticle);
		LuaEntityCastFunctionsDeclarationsForType(MOSRotating);
		LuaEntityCastFunctionsDeclarationsForType(MOPixel);
		LuaEntityCastFunctionsDeclarationsForType(MOSprite);
		LuaEntityCastFunctionsDeclarationsForType(Scene);
		LuaEntityCastFunctionsDeclarationsForType(Deployment);
		LuaEntityCastFunctionsDeclarationsForType(GameActivity);
		LuaEntityCastFunctionsDeclarationsForType(GlobalScript);
		LuaEntityCastFunctionsDeclarationsForType(GAScripted);
		LuaEntityCastFunctionsDeclarationsForType(HeldDevice);
		LuaEntityCastFunctionsDeclarationsForType(Round);
		LuaEntityCastFunctionsDeclarationsForType(Magazine);
		LuaEntityCastFunctionsDeclarationsForType(HDFirearm);
		LuaEntityCastFunctionsDeclarationsForType(ThrownDevice);
		LuaEntityCastFunctionsDeclarationsForType(TDExplosive);
		LuaEntityCastFunctionsDeclarationsForType(TerrainObject);
		LuaEntityCastFunctionsDeclarationsForType(PEmitter);
		LuaEntityCastFunctionsDeclarationsForType(PieSlice);
		LuaEntityCastFunctionsDeclarationsForType(PieMenu);
	};

	/// <summary>
	/// Special handling for passing ownership through properties. If you try to pass null to this normally, LuaJIT crashes.
	/// This handling avoids that, and is a bit safer since there's no actual ownership transfer from Lua to C++.
	/// </summary>
	#define LuaPropertyOwnershipSafetyFakerFunctionDeclaration(OBJECTTYPE, PROPERTYTYPE, SETTERFUNCTION) \
		static void OBJECTTYPE##SETTERFUNCTION(OBJECTTYPE *luaSelfObject, PROPERTYTYPE *objectToSet);

	struct LuaAdaptersPropertyOwnershipSafetyFaker {
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(MOSRotating, SoundContainer, SetGibSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Attachable, AEmitter, SetBreakWound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Attachable, AEmitter, SetParentBreakWound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AEmitter, Attachable, SetFlash);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AEmitter, SoundContainer, SetEmissionSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AEmitter, SoundContainer, SetBurstSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AEmitter, SoundContainer, SetEndSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ADoor, Attachable, SetDoor);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Leg, Attachable, SetFoot);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, PieMenu, SetPieMenu);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, SoundContainer, SetBodyHitSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, SoundContainer, SetAlarmSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, SoundContainer, SetPainSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, SoundContainer, SetDeathSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Actor, SoundContainer, SetDeviceSwitchSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ADoor, SoundContainer, SetDoorMoveStartSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ADoor, SoundContainer, SetDoorMoveSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ADoor, SoundContainer, SetDoorDirectionChangeSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ADoor, SoundContainer, SetDoorMoveEndSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Attachable, SetHead);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, AEmitter, SetJetpack);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Arm, SetFGArm);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Arm, SetBGArm);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Leg, SetFGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Leg, SetBGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Attachable, SetFGFoot);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, Attachable, SetBGFoot);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(AHuman, SoundContainer, SetStrideSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, Turret, SetTurret);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, AEmitter, SetJetpack);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, Leg, SetLeftFGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, Leg, SetLeftBGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, Leg, SetRightFGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, Leg, SetRightBGLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACrab, SoundContainer, SetStrideSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(Turret, HeldDevice, SetFirstMountedDevice);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACraft, SoundContainer, SetHatchOpenSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACraft, SoundContainer, SetHatchCloseSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACraft, SoundContainer, SetCrashSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, AEmitter, SetRightThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, AEmitter, SetLeftThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, AEmitter, SetURightThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, AEmitter, SetULeftThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, Attachable, SetRightHatch);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACDropShip, Attachable, SetLeftHatch);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, Leg, SetRightLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, Leg, SetLeftLeg);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, AEmitter, SetMainThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, AEmitter, SetLeftThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, AEmitter, SetRightThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, AEmitter, SetULeftThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(ACRocket, AEmitter, SetURightThruster);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, Magazine, SetMagazine);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, Attachable, SetFlash);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetPreFireSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetFireSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetFireEchoSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetActiveSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetDeactivationSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetEmptySound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetReloadStartSound);
		LuaPropertyOwnershipSafetyFakerFunctionDeclaration(HDFirearm, SoundContainer, SetReloadEndSound);
	};
#pragma endregion

#pragma region Entity Lua Adapters
	struct LuaAdaptersEntity {
		// TODO this is a temporary fix for lua PresetName setting causing scripts to have to rerun. It should be replaced with a DisplayName property someday.
		static void SetPresetName(Entity *luaSelfObject, const std::string &presetName);
	};
#pragma endregion

#pragma region Attachable Lua Adapters
	struct LuaAdaptersAttachable {
		static Attachable * RemoveFromParent1(Attachable *luaSelfObject);
		static Attachable * RemoveFromParent2(Attachable *luaSelfObject, bool addToMovableMan, bool addBreakWounds);
	};
#pragma endregion

#pragma region GlobalScript Lua Adapters
	struct LuaAdaptersGlobalScript {
		static void Deactivate(GlobalScript *luaSelfObject);
	};
#pragma endregion

#pragma region MovableObject Lua Adapters
	struct LuaAdaptersMovableObject {
		static bool AddScript(MovableObject *luaSelfObject, const std::string &scriptPath);
		static bool EnableScript(MovableObject *luaSelfObject, const std::string &scriptPath);
		static bool DisableScript(MovableObject *luaSelfObject, const std::string &scriptPath);
	};
#pragma endregion

#pragma region MOSRotating Lua Adapters
	struct LuaAdaptersMOSRotating {
		static void GibThis(MOSRotating *luaSelfObject);
	};
#pragma endregion

#pragma region PieMenu Lua Adapters
	struct LuaAdaptersPieMenu {
		static bool AddPieSlice(PieMenu *luaSelfObject, PieSlice *pieSliceToAdd, const Entity *pieSliceOriginalSource);
		static bool AddPieSliceIfPresetNameIsUnique1(PieMenu *luaSelfObject, PieSlice *pieSliceToAdd, const Entity *pieSliceOriginalSource);
		static bool AddPieSliceIfPresetNameIsUnique2(PieMenu *luaSelfObject, PieSlice *pieSliceToAdd, const Entity *pieSliceOriginalSource, bool onlyCheckPieSlicesWithSameOriginalSource);
	};
#pragma endregion

#pragma region SceneObject Lua Adapters
	struct LuaAdaptersSceneObject {
		static float GetTotalValue(const SceneObject *luaSelfObject, int nativeModule, float foreignMult);
	};
#pragma endregion

#pragma region Turret Lua Adapters
	struct LuaAdaptersTurret {
		static void AddMountedFirearm(Turret *luaSelfObject, HDFirearm *newMountedDevice);
	};
#pragma endregion

#pragma region MovableMan Lua Adapters
	struct LuaAdaptersMovableMan {
		/// <summary>
		/// Adds the given MovableObject to MovableMan if it doesn't already exist in there, or prints an error if it does.
		/// </summary>
		/// <param name="movableMan">A reference to MovableMan, provided by Lua.</param>
		/// <param name="movableObject">A pointer to the MovableObject to be added.</param>
		static void AddMO(MovableMan &movableMan, MovableObject *movableObject);

		/// <summary>
		/// Adds the given Actor to MovableMan if it doesn't already exist in there, or prints an error if it does.
		/// </summary>
		/// <param name="movableMan">A reference to MovableMan, provided by Lua.</param>
		/// <param name="actor">A pointer to the Actor to be added.</param>
		static void AddActor(MovableMan &movableMan, Actor *actor);

		/// <summary>
		/// Adds the given item MovableObject (generally a HeldDevice) to MovableMan if it doesn't already exist in there, or prints an error if it does.
		/// </summary>
		/// <param name="movableMan">A reference to MovableMan, provided by Lua.</param>
		/// <param name="item">A pointer to the item to be added.</param>
		static void AddItem(MovableMan &movableMan, HeldDevice *item);

		/// <summary>
		/// Adds the given particle MovableObject to MovableMan if it doesn't already exist in there, or prints an error if it does.
		/// </summary>
		/// <param name="movableMan">A reference to MovableMan, provided by Lua.</param>
		/// <param name="particle">A pointer to the particle to be added.</param>
		static void AddParticle(MovableMan &movableMan, MovableObject *particle);
	};
#pragma endregion

#pragma region TimerMan Lua Adapters
	struct LuaAdaptersTimerMan {
		/// <summary>
		/// Gets the current number of ticks that the simulation should be updating with. Lua can't handle int64 (or long long apparently) so we'll expose this specialized function.
		/// </summary>
		/// <returns>The current fixed delta time that the simulation should be updating with, in ticks.</returns>
		static double GetDeltaTimeTicks(const TimerMan &timerMan);

		/// <summary>
		/// Gets the number of ticks per second. Lua can't handle int64 (or long long apparently) so we'll expose this specialized function.
		/// </summary>
		/// <returns>The number of ticks per second.</returns>
		static double GetTicksPerSecond(const TimerMan &timerMan);
	};
#pragma endregion

#pragma region UInputMan Lua Adapters
	struct LuaAdaptersUInputMan {
		/// <summary>
		/// Gets whether a mouse button is being held down right now.
		/// </summary>
		/// <param name="whichButton">Which button to check for.</param>
		/// <returns>Whether the mouse button is held or not.</returns>
		static bool MouseButtonHeld(const UInputMan &uinputMan, int whichButton);

		/// <summary>
		/// Gets whether a mouse button was pressed between the last update and the one previous to it.
		/// </summary>
		/// <param name="whichButton">Which button to check for.</param>
		/// <returns>Whether the mouse button is pressed or not.</returns>
		static bool MouseButtonPressed(const UInputMan &uinputMan, int whichButton);

		/// <summary>
		/// Gets whether a mouse button was released between the last update and the one previous to it.
		/// </summary>
		/// <param name="whichButton">Which button to check for.</param>
		/// <returns>Whether the mouse button is released or not.</returns>
		static bool MouseButtonReleased(const UInputMan &uinputMan, int whichButton);
	};
#pragma endregion

#pragma region PresetMan Lua Adapters
	struct LuaAdaptersPresetMan {
		/// <summary>
		/// Reloads the specified Entity preset in PresetMan.
		/// </summary>
		/// <param name="presetName">The preset name of the Entity to reload.</param>
		/// <param name="className">The class name of the Entity to reload.</param>
		/// <param name="moduleName">The module name of the Entity to reload.</param>
		/// <returns>Whether or not the Entity was reloaded.</returns>
		static bool ReloadEntityPreset1(PresetMan &presetMan, const std::string &presetName, const std::string &className, const std::string &moduleName);

		/// <summary>
		/// Reloads the specified Entity preset in PresetMan.
		/// </summary>
		/// <param name="presetName">The preset name of the Entity to reload.</param>
		/// <param name="className">The class name of the Entity to reload.</param>
		/// <returns>Whether or not the Entity was reloaded.</returns>
		static bool ReloadEntityPreset2(PresetMan &presetMan, const std::string &presetName, const std::string &className);
	};
#pragma endregion

#pragma region Utility Lua Adapters
	struct LuaAdaptersUtility {
		/// <summary>
		/// Gets a random number between -1 and 1.
		/// </summary>
		/// <returns>A random number between -1 and 1.</returns>
		static double NormalRand();

		/// <summary>
		/// Gets a random number between 0 and 1.
		/// </summary>
		/// <returns>A random number between 0 and 1.</returns>
		static double PosRand();

		/// <summary>
		/// Gets a random number between 1 and the passed in upper limit number. The limits are included in the available random range.
		/// </summary>
		/// <param name="upperLimitInclusive">The upper limit for the random number.</param>
		/// <returns>A random number between 1 and the passed in number.</returns>
		static double LuaRand(double upperLimitInclusive);

		/// <summary>
		/// Explicit deletion of any Entity instance that Lua owns. It will probably be handled by the GC, but this makes it instantaneous.
		/// </summary>
		/// <param name="entityToDelete">The Entity to delete.</param>
		static void DeleteEntity(Entity *entityToDelete);
	};
#pragma endregion
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

#endif
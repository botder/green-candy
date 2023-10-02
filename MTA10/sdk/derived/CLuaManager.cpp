/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CLuaManager.cpp
*  PURPOSE:     Derived lua hyperstructure management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CLuaManager::CLuaManager() : LuaManager( *g_pClientGame->GetEvents(), m_debug ), m_debug( *this ), m_commands( *this )
{
}

CLuaManager::~CLuaManager()
{
}

void CLuaManager::GarbageCollect( lua_State *L )
{
    // Mark classes which should be preserved
    // Possible scenarios: background activity, system weak referencing while usage in engine code (RwObjects)
    g_pClientGame->GetResourceManager()->GarbageCollect( L );

    // Notify the general manager
    LuaManager::GarbageCollect( L );
}

bool CLuaManager::OnLuaClassDeallocationFail( lua_State *L, ILuaClass *j )
{
    union
    {
        CClientGUIElement *guiElem;
    };

    if ( j->GetTransmit( LUACLASS_GUIELEMENT, (void*&)guiElem ) )
    {
        // HACK: try to remove the activity lock of this GUI element because the
        // runtime really wants us to. Otherwise we would crash due to a fatal
        // breakpoint exception. I hope CEGUI is clever enough to deal with the
        // destruction of runtime elements properly.
        return g_pClientGame->GetGUIManager()->RemoveActivityLock( guiElem );
    }

    return LuaManager::OnLuaClassDeallocationFail( L, j );
}

static void LoadCFunctions( lua_State *L )
{
    using namespace CLuaFunctionDefs;

    // Resource functions
    LUA_REGISTER( L, call );
    LUA_REGISTER( L, getThisResource );
    LUA_REGISTER( L, getResourceConfig );
    LUA_REGISTER( L, getResourceName );
    LUA_REGISTER( L, getResourceFromName );
    LUA_REGISTER( L, getResourceRootElement );
    LUA_REGISTER( L, getResourceGUIElement );
    LUA_REGISTER( L, getResourceDynamicElementRoot );
    LUA_REGISTER( L, getResourceExportedFunctions );
    LUA_REGISTER( L, fileCreateTranslator );
    lua_register( L, "fileOpenArchive", luafsys_createArchiveTranslator );    // lib redirect
    lua_register( L, "fileCreateZIP", luafsys_createZIPArchive );

    // Event functions
    LUA_REGISTER( L, addEvent );
    LUA_REGISTER( L, addEventHandler );
    LUA_REGISTER( L, removeEventHandler );
    LUA_REGISTER( L, triggerEvent );
    LUA_REGISTER( L, triggerServerEvent );
    LUA_REGISTER( L, cancelEvent );
    LUA_REGISTER( L, wasEventCancelled );

    // Sound effects and synth functions
    LUA_REGISTER( L, playSound );
    LUA_REGISTER( L, playSound3D );
    LUA_REGISTER( L, stopSound );
    LUA_REGISTER( L, setSoundPosition );
    LUA_REGISTER( L, getSoundPosition );
    LUA_REGISTER( L, getSoundLength );
    LUA_REGISTER( L, setSoundPaused );
    LUA_REGISTER( L, isSoundPaused );
    LUA_REGISTER( L, setSoundVolume );
    LUA_REGISTER( L, getSoundVolume );
    LUA_REGISTER( L, setSoundSpeed );
    LUA_REGISTER( L, getSoundSpeed );
    LUA_REGISTER( L, setSoundMinDistance );
    LUA_REGISTER( L, getSoundMinDistance );
    LUA_REGISTER( L, setSoundMaxDistance );
    LUA_REGISTER( L, getSoundMaxDistance );
    LUA_REGISTER( L, getSoundMetaTags );
    LUA_REGISTER( L, setSoundEffectEnabled );
    LUA_REGISTER( L, getSoundEffects );

    // Output functions
    LUA_REGISTER( L, outputConsole );
    LUA_REGISTER( L, outputChatBox );
    LUA_REGISTER( L, enableChat );
    LUA_REGISTER( L, isChatEnabled );
    LUA_REGISTER( L, showChat );
    LUA_REGISTER( L, setClipboard );
    LUA_REGISTER( L, getClipboard );

    // Element get functions
    LUA_REGISTER( L, getRootElement );
    LUA_REGISTER( L, isElement );
    LUA_REGISTER( L, getElementChildren );
    LUA_REGISTER( L, getElementChild );
    LUA_REGISTER( L, getElementChildrenCount );
    LUA_REGISTER( L, getElementID );
    LUA_REGISTER( L, getElementByID );
    LUA_REGISTER( L, getElementByIndex );
    LUA_REGISTER( L, getElementData );
    LUA_REGISTER( L, getElementParent );
    LUA_REGISTER( L, getElementMatrix );
    LUA_REGISTER( L, getElementPosition );
    LUA_REGISTER( L, getElementRotation );
    LUA_REGISTER( L, getElementVelocity );
    LUA_REGISTER( L, getElementType );
    LUA_REGISTER( L, getElementsByType );
    LUA_REGISTER( L, getElementInterior );
    LUA_REGISTER( L, isElementWithinColShape );
    LUA_REGISTER( L, isElementWithinMarker );
    LUA_REGISTER( L, getElementsWithinColShape );
    LUA_REGISTER( L, getElementDimension );
    LUA_REGISTER( L, getElementBoundingBox );
    LUA_REGISTER( L, getElementRadius );
    LUA_REGISTER( L, isElementAttached );
    LUA_REGISTER( L, getElementAttachedTo );
    LUA_REGISTER( L, getAttachedElements );
    LUA_REGISTER( L, getElementDistanceFromCentreOfMassToBaseOfModel );
    LUA_REGISTER( L, isElementLocal );
    LUA_REGISTER( L, getElementAttachedOffsets );
    LUA_REGISTER( L, getElementAlpha );
    LUA_REGISTER( L, isElementOnScreen );
    LUA_REGISTER( L, getElementHealth );
    LUA_REGISTER( L, isElementStreamedIn );
    LUA_REGISTER( L, isElementStreamable );
    LUA_REGISTER( L, getElementModel );
    LUA_REGISTER( L, getElementColShape );
    LUA_REGISTER( L, isElementInWater );
    LUA_REGISTER( L, isElementSyncer );
    LUA_REGISTER( L, isElementCollidableWith );
    LUA_REGISTER( L, isElementDoubleSided );
    LUA_REGISTER( L, getElementCollisionsEnabled );
    LUA_REGISTER( L, isElementFrozen );

    // Element set funcs
    LUA_REGISTER( L, createElement );
    LUA_REGISTER( L, destroyElement );
    LUA_REGISTER( L, setElementID );
    LUA_REGISTER( L, setElementData );
    LUA_REGISTER( L, removeElementData );
    LUA_REGISTER( L, setElementPosition );
    LUA_REGISTER( L, setElementRotation );
    LUA_REGISTER( L, setElementVelocity );
    LUA_REGISTER( L, setElementParent );  
    LUA_REGISTER( L, setElementInterior );
    LUA_REGISTER( L, setElementDimension );
    LUA_REGISTER( L, attachElements );
    LUA_REGISTER( L, detachElements );
    LUA_REGISTER( L, setElementAttachedOffsets );
    LUA_REGISTER( L, setElementCollisionsEnabled );
    LUA_REGISTER( L, setElementAlpha );
    LUA_REGISTER( L, setElementHealth );
    LUA_REGISTER( L, setElementStreamable );
    LUA_REGISTER( L, setElementModel );
    LUA_REGISTER( L, setElementCollidableWith );
    LUA_REGISTER( L, setElementDoubleSided );
    LUA_REGISTER( L, setElementFrozen );

    // Radio functions
    LUA_REGISTER( L, setRadioChannel );
    LUA_REGISTER( L, getRadioChannel );
    LUA_REGISTER( L, getRadioChannelName );

    // Player get functions
    LUA_REGISTER( L, getLocalPlayer );
    LUA_REGISTER( L, getPlayerName );
    LUA_REGISTER( L, getPlayerFromName );
    LUA_REGISTER( L, getPlayerNametagText );
    LUA_REGISTER( L, getPlayerNametagColor );
    LUA_REGISTER( L, isPlayerNametagShowing );
    LUA_REGISTER( L, getPlayerPing );
    LUA_REGISTER( L, getPlayerTeam );
    LUA_REGISTER( L, isPlayerDead );
    LUA_REGISTER( L, getPlayerMoney );
    LUA_REGISTER( L, getPlayerWantedLevel );
    
    // Player set funcs
    LUA_REGISTER( L, showPlayerHudComponent );
    LUA_REGISTER( L, setPlayerMoney );
    LUA_REGISTER( L, givePlayerMoney );
    LUA_REGISTER( L, takePlayerMoney );
    LUA_REGISTER( L, setPlayerNametagText );
    LUA_REGISTER( L, setPlayerNametagColor );
    LUA_REGISTER( L, setPlayerNametagShowing );

    // Ped funcs
    LUA_REGISTER( L, createPed );

    LUA_REGISTER( L, detonateSatchels );

    LUA_REGISTER( L, getPedVoice );
    LUA_REGISTER( L, setPedVoice );
    LUA_REGISTER( L, getPedTarget );
    LUA_REGISTER( L, getPedTargetStart );
    LUA_REGISTER( L, getPedTargetEnd );
    LUA_REGISTER( L, getPedTargetRange );
    LUA_REGISTER( L, getPedTargetCollision );
    LUA_REGISTER( L, getPedWeaponSlot );
    LUA_REGISTER( L, getPedWeapon );
    LUA_REGISTER( L, getPedAmmoInClip );
    LUA_REGISTER( L, getPedTotalAmmo );
    LUA_REGISTER( L, getPedWeaponMuzzlePosition );
    LUA_REGISTER( L, getPedStat );
    LUA_REGISTER( L, getPedOccupiedVehicle );
    LUA_REGISTER( L, getPedArmor );
    LUA_REGISTER( L, isPedChoking );
    LUA_REGISTER( L, isPedDucked );
    LUA_REGISTER( L, isPedInVehicle );
    LUA_REGISTER( L, doesPedHaveJetPack );
    LUA_REGISTER( L, isPedOnGround );
    LUA_REGISTER( L, getPedTask );
    LUA_REGISTER( L, getPedSimplestTask );
    LUA_REGISTER( L, isPedDoingTask );
    LUA_REGISTER( L, getPedContactElement );
    LUA_REGISTER( L, getPedRotation );
    LUA_REGISTER( L, canPedBeKnockedOffBike );
    LUA_REGISTER( L, getPedBonePosition );
    LUA_REGISTER( L, getPedClothes );
    LUA_REGISTER( L, getPedControlState );
    LUA_REGISTER( L, isPedDoingGangDriveby );
    LUA_REGISTER( L, getPedAnimation );
    LUA_REGISTER( L, getPedMoveState );
    LUA_REGISTER( L, getPedMoveAnim );
    LUA_REGISTER( L, isPedHeadless );
    LUA_REGISTER( L, isPedFrozen );
    LUA_REGISTER( L, isPedFootBloodEnabled );
    LUA_REGISTER( L, getPedCameraRotation );
    LUA_REGISTER( L, isPedOnFire );

    LUA_REGISTER( L, setPedRotation );
    LUA_REGISTER( L, setPedCanBeKnockedOffBike );
    LUA_REGISTER( L, setPedAnimation );
    LUA_REGISTER( L, setPedAnimationProgress );
    LUA_REGISTER( L, setPedMoveAnim );
    LUA_REGISTER( L, setPedWeaponSlot );
    LUA_REGISTER( L, addPedClothes );
    LUA_REGISTER( L, removePedClothes );
    LUA_REGISTER( L, setPedControlState );
    LUA_REGISTER( L, setPedDoingGangDriveby );
    LUA_REGISTER( L, setPedLookAt );
    LUA_REGISTER( L, setPedHeadless );
    LUA_REGISTER( L, setPedFrozen );
    LUA_REGISTER( L, setPedFootBloodEnabled );
    LUA_REGISTER( L, setPedCameraRotation );
    LUA_REGISTER( L, setPedAimTarget );
    LUA_REGISTER( L, setPedOnFire );

    // Civilian ped functions.
    LUA_REGISTER( L, createCivilian );

    // Clothes and body functions
    LUA_REGISTER( L, getBodyPartName );
    LUA_REGISTER( L, getClothesByTypeIndex );
    LUA_REGISTER( L, getTypeIndexFromClothes );
    LUA_REGISTER( L, getClothesTypeName );

    // Vehicle get functions
    LUA_REGISTER( L, createVehicle );

    LUA_REGISTER( L, getVehicleType );
    LUA_REGISTER( L, getVehicleColor );
    LUA_REGISTER( L, getVehicleModelFromName );
    LUA_REGISTER( L, getVehicleLandingGearDown );
    LUA_REGISTER( L, getVehicleMaxPassengers );
    LUA_REGISTER( L, getVehicleOccupant );
    LUA_REGISTER( L, getVehicleOccupants );
    LUA_REGISTER( L, getVehicleController );
    LUA_REGISTER( L, getVehicleSirensOn );
    LUA_REGISTER( L, getVehicleTurnVelocity );
    LUA_REGISTER( L, getVehicleTurretPosition );
    LUA_REGISTER( L, isVehicleLocked );
    LUA_REGISTER( L, getVehicleUpgradeOnSlot );
    LUA_REGISTER( L, getVehicleUpgrades );
    LUA_REGISTER( L, getVehicleUpgradeSlotName );
    LUA_REGISTER( L, getVehicleCompatibleUpgrades );
    LUA_REGISTER( L, getVehicleWheelStates );
    LUA_REGISTER( L, getVehicleDoorState );
    LUA_REGISTER( L, getVehicleLightState );
    LUA_REGISTER( L, getVehiclePanelState );
    LUA_REGISTER( L, getVehicleOverrideLights );
    LUA_REGISTER( L, getVehicleTowedByVehicle );
    LUA_REGISTER( L, getVehicleTowingVehicle );
    LUA_REGISTER( L, getVehiclePaintjob );
    LUA_REGISTER( L, getVehiclePlateText );
    LUA_REGISTER( L, isVehicleDamageProof );
    LUA_REGISTER( L, isVehicleFuelTankExplodable );
    LUA_REGISTER( L, isVehicleFrozen );
    LUA_REGISTER( L, isVehicleOnGround );
    LUA_REGISTER( L, getVehicleName );
    LUA_REGISTER( L, getVehicleNameFromModel );
    LUA_REGISTER( L, getVehicleAdjustableProperty );
    LUA_REGISTER( L, getHelicopterRotorSpeed );
    LUA_REGISTER( L, getVehicleEngineState );
    LUA_REGISTER( L, isTrainDerailed );
    LUA_REGISTER( L, isTrainDerailable );
    LUA_REGISTER( L, getTrainDirection );
    LUA_REGISTER( L, getTrainSpeed );
    LUA_REGISTER( L, getVehicleGravity );
    LUA_REGISTER( L, isVehicleBlown );
    LUA_REGISTER( L, getVehicleHeadLightColor );
    LUA_REGISTER( L, getVehicleCurrentGear );
    LUA_REGISTER( L, getVehicleHandling );
    LUA_REGISTER( L, getOriginalHandling );
    LUA_REGISTER( L, getVehicleDoorOpenRatio );

    // Vehicle set functions
    LUA_REGISTER( L, fixVehicle );
    LUA_REGISTER( L, blowVehicle );
    LUA_REGISTER( L, setVehicleTurnVelocity );
    LUA_REGISTER( L, setVehicleColor );
    LUA_REGISTER( L, setVehicleLandingGearDown );
    LUA_REGISTER( L, setVehicleLocked );
    LUA_REGISTER( L, setVehicleDoorsUndamageable );
    LUA_REGISTER( L, setVehicleSirensOn );
    LUA_REGISTER( L, addVehicleUpgrade );
    LUA_REGISTER( L, removeVehicleUpgrade );
    LUA_REGISTER( L, setVehicleDoorState );
    LUA_REGISTER( L, setVehicleWheelStates );
    LUA_REGISTER( L, setVehicleLightState );
    LUA_REGISTER( L, setVehiclePanelState );
    LUA_REGISTER( L, setVehicleOverrideLights );
    LUA_REGISTER( L, setVehicleTaxiLightOn );
    LUA_REGISTER( L, isVehicleTaxiLightOn );
    LUA_REGISTER( L, attachTrailerToVehicle );
    LUA_REGISTER( L, detachTrailerFromVehicle );
    LUA_REGISTER( L, setVehicleEngineState );
    LUA_REGISTER( L, setVehicleDirtLevel );
    LUA_REGISTER( L, setVehicleDamageProof );
    LUA_REGISTER( L, setVehiclePaintjob );
    LUA_REGISTER( L, setVehicleFuelTankExplodable );
    LUA_REGISTER( L, setVehicleFrozen );
    LUA_REGISTER( L, setVehicleAdjustableProperty );
    LUA_REGISTER( L, setHelicopterRotorSpeed );
    LUA_REGISTER( L, setTrainDerailed );
    LUA_REGISTER( L, setTrainDerailable );
    LUA_REGISTER( L, setTrainDirection );
    LUA_REGISTER( L, setTrainSpeed );
    LUA_REGISTER( L, setVehicleGravity );
    LUA_REGISTER( L, setVehicleHeadLightColor );
    LUA_REGISTER( L, setVehicleTurretPosition );
    LUA_REGISTER( L, setVehicleDoorOpenRatio );

    // Object create funcs
    LUA_REGISTER( L, createObject );
    
    // Object get funcs
    LUA_REGISTER( L, getObjectScale );

    // Object set funcs
    LUA_REGISTER( L, moveObject );
    LUA_REGISTER( L, stopObject );
    LUA_REGISTER( L, setObjectScale );

    // Explosion functions
    LUA_REGISTER( L, createExplosion );

    // Fire functions
    LUA_REGISTER( L, createFire );

    // Audio funcs
    LUA_REGISTER( L, playMissionAudio );
    LUA_REGISTER( L, playSoundFrontEnd );
    LUA_REGISTER( L, preloadMissionAudio );
    LUA_REGISTER( L, setAmbientSoundEnabled );
    LUA_REGISTER( L, isAmbientSoundEnabled );
    LUA_REGISTER( L, resetAmbientSounds );

    // Blip funcs
    LUA_REGISTER( L, createBlip );
    LUA_REGISTER( L, createBlipAttachedTo );
    LUA_REGISTER( L, getBlipIcon );
    LUA_REGISTER( L, getBlipSize );
    LUA_REGISTER( L, getBlipColor );
    LUA_REGISTER( L, getBlipOrdering );
    LUA_REGISTER( L, getBlipVisibleDistance );

    LUA_REGISTER( L, setBlipIcon );
    LUA_REGISTER( L, setBlipSize );
    LUA_REGISTER( L, setBlipColor );
    LUA_REGISTER( L, setBlipOrdering );
    LUA_REGISTER( L, setBlipVisibleDistance );

    // Marker funcs
    LUA_REGISTER( L, createMarker );

    // Marker get functions
    LUA_REGISTER( L, getMarkerCount );
    LUA_REGISTER( L, getMarkerType );
    LUA_REGISTER( L, getMarkerSize );
    LUA_REGISTER( L, getMarkerColor );
    LUA_REGISTER( L, getMarkerTarget );
    LUA_REGISTER( L, getMarkerIcon );

    // Marker set functions
    LUA_REGISTER( L, setMarkerType );
    LUA_REGISTER( L, setMarkerSize );
    LUA_REGISTER( L, setMarkerColor );
    LUA_REGISTER( L, setMarkerTarget );
    LUA_REGISTER( L, setMarkerIcon );

    // Radar-area funcs
    LUA_REGISTER( L, createRadarArea );
    LUA_REGISTER( L, getRadarAreaColor );
    LUA_REGISTER( L, getRadarAreaSize );
    LUA_REGISTER( L, isRadarAreaFlashing );
    LUA_REGISTER( L, setRadarAreaColor );
    LUA_REGISTER( L, setRadarAreaFlashing );
    LUA_REGISTER( L, setRadarAreaSize );
    LUA_REGISTER( L, isInsideRadarArea );

    // Pickup create/destroy funcs
    LUA_REGISTER( L, createPickup );

    // Pickup get funcs
    LUA_REGISTER( L, getPickupType );
    LUA_REGISTER( L, getPickupWeapon );
    LUA_REGISTER( L, getPickupAmount );
    LUA_REGISTER( L, getPickupAmmo );

    // Pickup get funcs
    LUA_REGISTER( L, setPickupType );

    // Cam get funcs
    LUA_REGISTER( L, getCameraViewMode );
    LUA_REGISTER( L, getCameraMatrix );
    LUA_REGISTER( L, getCameraTarget );
    LUA_REGISTER( L, getCameraInterior );
    LUA_REGISTER( L, getCameraGoggleEffect );

    // Cam set funcs
    LUA_REGISTER( L, setCameraMatrix );
    LUA_REGISTER( L, setCameraTarget );
    LUA_REGISTER( L, setCameraInterior );
    LUA_REGISTER( L, fadeCamera );
    LUA_REGISTER( L, setCameraClip );
    LUA_REGISTER( L, setCameraViewMode );
    LUA_REGISTER( L, setCameraGoggleEffect );

    // Cursor funcs
    LUA_REGISTER( L, getCursorPosition );
    LUA_REGISTER( L, setCursorPosition );
    LUA_REGISTER( L, isCursorShowing );
    
    LUA_REGISTER( L, showCursor );

    // Engine (RenderWare abtract) functions
    LUA_REGISTER( L, engineLoadDFF );
    LUA_REGISTER( L, engineLoadTXD );
    LUA_REGISTER( L, engineLoadCOL );
    LUA_REGISTER( L, engineGetGameTextures );
    LUA_REGISTER( L, engineModelInfoHasLoaded );
    LUA_REGISTER( L, engineModelInfoCloneObject );
    LUA_REGISTER( L, engineCreateLight );
    LUA_REGISTER( L, engineCreateFrame );
    LUA_REGISTER( L, engineCreateCamera );
    LUA_REGISTER( L, engineCreateClump );
    LUA_REGISTER( L, engineIsRendering );
    LUA_REGISTER( L, engineImportTXD );
    LUA_REGISTER( L, engineReplaceCOL );
    LUA_REGISTER( L, engineRestoreCOL );
    LUA_REGISTER( L, engineReplaceModel );
    LUA_REGISTER( L, engineRestoreModel );
    LUA_REGISTER( L, engineSetModelLODDistance );
    LUA_REGISTER( L, engineSetEnvMapRenderingEnabled );
    LUA_REGISTER( L, engineIsEnvMapRenderingEnabled );
    LUA_REGISTER( L, engineSetAsynchronousLoading );
    LUA_REGISTER( L, engineApplyShaderToWorldTexture );
    LUA_REGISTER( L, engineRemoveShaderFromWorldTexture );
    LUA_REGISTER( L, engineGetModelNameFromID );
    LUA_REGISTER( L, engineGetModelIDFromName );
    LUA_REGISTER( L, engineGetModelTextureNames );
    LUA_REGISTER( L, engineGetVisibleTextureNames );
    LUA_REGISTER( L, engineSetInfiniteStreamingEnabled );
    LUA_REGISTER( L, engineIsInfiniteStreamingEnabled );
    LUA_REGISTER( L, engineSetStrictStreamingNodeDistributionEnabled );
    LUA_REGISTER( L, engineIsStrictStreamingNodeDistributionEnabled );
    LUA_REGISTER( L, engineSetStreamingGCOnDemandEnabled );
    LUA_REGISTER( L, engineIsStreamingGCOnDemandEnabled );
    LUA_REGISTER( L, engineAllowStreamingNodeStealing );
    LUA_REGISTER( L, engineIsStreamingNodeStealingAllowed );
    LUA_REGISTER( L, engineGetActiveStreamingEntityCount );
    LUA_REGISTER( L, engineGetActiveStreamingFreeSlotCount );
    LUA_REGISTER( L, engineGetActiveStreamingEntities );
    LUA_REGISTER( L, engineGetStreamingFocus );
    LUA_REGISTER( L, engineStreamingIsElementManaged );
    LUA_REGISTER( L, engineGetModelRefCount );
    LUA_REGISTER( L, engineSetWorldStreamingEnabled );
    LUA_REGISTER( L, engineIsWorldStreamingEnabled );
    LUA_REGISTER( L, engineGetEntitiesInRenderQueue );
    LUA_REGISTER( L, engineGetGamePoolLimits );
    LUA_REGISTER( L, engineGetStreamingInfo );
    LUA_REGISTER( L, engineStreamingEnableFiberedLoading );
    LUA_REGISTER( L, engineStreamingIsFiberedLoadingEnabled );
    LUA_REGISTER( L, engineStreamingSetFiberedPerfMultiplier );
    LUA_REGISTER( L, engineStreamingGetFiberedPerfMultiplier );
    LUA_REGISTER( L, engineSetGlobalLightingAlwaysEnabled );
    LUA_REGISTER( L, engineIsGlobalLightingAlwaysEnabled );
    LUA_REGISTER( L, engineSetLocalLightingAlwaysEnabled );
    LUA_REGISTER( L, engineIsLocalLightingAlwaysEnabled );
    LUA_REGISTER( L, engineSetWorldRenderMode );
    LUA_REGISTER( L, engineGetWorldRenderMode );
    LUA_REGISTER( L, engineIsModelBeingUsed );
    LUA_REGISTER( L, engineGetRenderBucketStatistics );

    // Drawing funcs (low-level)
    LUA_REGISTER( L, dxDrawLine );
    LUA_REGISTER( L, dxDrawLine3D );
    LUA_REGISTER( L, dxDrawText );
    LUA_REGISTER( L, dxDrawRectangle );
    LUA_REGISTER( L, dxDrawImage );
    LUA_REGISTER( L, dxDrawImageSection );
    LUA_REGISTER( L, dxGetTextWidth );
    LUA_REGISTER( L, dxGetFontHeight );
    LUA_REGISTER( L, dxCreateFont );
    LUA_REGISTER( L, dxCreateTexture );
    LUA_REGISTER( L, dxCreateShader );
    LUA_REGISTER( L, dxCreateRenderTarget );
    LUA_REGISTER( L, dxCreateScreenSource );
    LUA_REGISTER( L, dxGetMaterialSize );
    LUA_REGISTER( L, dxSetShaderValue );
    LUA_REGISTER( L, dxSetRenderTarget );
    LUA_REGISTER( L, dxUpdateScreenSource );
    LUA_REGISTER( L, dxGetStatus );
    LUA_REGISTER( L, dxSetBlendMode );
    LUA_REGISTER( L, dxGetBlendMode );
    LUA_REGISTER( L, dxSetTestMode );

    // Pfft utils
    LUA_REGISTER( L, tocolor );
    LUA_REGISTER( L, getColorFromString );
    LUA_REGISTER( L, gettok );
    LUA_REGISTER( L, split );

    // gui funcs
    LUA_REGISTER( L, guiGetInputEnabled );
    LUA_REGISTER( L, guiGetInputMode );
    LUA_REGISTER( L, guiSetInputEnabled );
    LUA_REGISTER( L, guiSetInputMode );
    LUA_REGISTER( L, isChatBoxInputActive );
    LUA_REGISTER( L, isConsoleActive );
    LUA_REGISTER( L, isDebugViewActive );
    LUA_REGISTER( L, isMainMenuActive );
    LUA_REGISTER( L, isMTAWindowActive );
    LUA_REGISTER( L, isTransferBoxActive );
    LUA_REGISTER( L, guiCreateWindow );
    LUA_REGISTER( L, guiCreateLabel );
    LUA_REGISTER( L, guiCreateButton );
    LUA_REGISTER( L, guiCreateEdit );
    LUA_REGISTER( L, guiCreateMemo );
    LUA_REGISTER( L, guiCreateGridList );
    LUA_REGISTER( L, guiCreateScrollBar );
    LUA_REGISTER( L, guiCreateScrollPane );
    LUA_REGISTER( L, guiCreateStaticImage );
    LUA_REGISTER( L, guiCreateTabPanel );
    LUA_REGISTER( L, guiCreateTab );
    LUA_REGISTER( L, guiCreateProgressBar );
    LUA_REGISTER( L, guiCreateCheckBox );
    LUA_REGISTER( L, guiCreateRadioButton );
    LUA_REGISTER( L, guiCreateFont );
    LUA_REGISTER( L, guiGetSelectedTab );
    LUA_REGISTER( L, guiSetSelectedTab );
    LUA_REGISTER( L, guiDeleteTab );
    LUA_REGISTER( L, guiScrollBarSetScrollPosition );
    LUA_REGISTER( L, guiScrollBarGetScrollPosition );
    LUA_REGISTER( L, guiGridListSetSortingEnabled );
    LUA_REGISTER( L, guiGridListAddColumn );
    LUA_REGISTER( L, guiGridListRemoveColumn );
    LUA_REGISTER( L, guiGridListSetColumnWidth );
    LUA_REGISTER( L, guiGridListSetScrollBars );
    LUA_REGISTER( L, guiGridListGetRowCount );
    LUA_REGISTER( L, guiGridListGetColumnCount );
    LUA_REGISTER( L, guiGridListAddRow );
    LUA_REGISTER( L, guiGridListInsertRowAfter );
    LUA_REGISTER( L, guiGridListRemoveRow );
    LUA_REGISTER( L, guiGridListAutoSizeColumn );
    LUA_REGISTER( L, guiGridListClear );
    LUA_REGISTER( L, guiGridListSetItemText );
    LUA_REGISTER( L, guiGridListGetItemText );
    LUA_REGISTER( L, guiGridListSetItemData );
    LUA_REGISTER( L, guiGridListGetItemData );
    LUA_REGISTER( L, guiGridListSetItemColor );
    LUA_REGISTER( L, guiGridListGetItemColor );
    LUA_REGISTER( L, guiGridListSetSelectionMode );
    LUA_REGISTER( L, guiGridListGetSelectedItem );
    LUA_REGISTER( L, guiGridListGetSelectedItems );
    LUA_REGISTER( L, guiGridListGetSelectedCount );
    LUA_REGISTER( L, guiGridListSetSelectedItem );
    LUA_REGISTER( L, guiScrollPaneSetScrollBars );
    LUA_REGISTER( L, guiScrollPaneSetHorizontalScrollPosition );
    LUA_REGISTER( L, guiScrollPaneGetHorizontalScrollPosition );
    LUA_REGISTER( L, guiScrollPaneSetVerticalScrollPosition );
    LUA_REGISTER( L, guiScrollPaneGetVerticalScrollPosition );
    LUA_REGISTER( L, guiStaticImageLoadImage );
    LUA_REGISTER( L, guiSetEnabled );
    LUA_REGISTER( L, guiSetText );
    LUA_REGISTER( L, guiSetFont );
    LUA_REGISTER( L, guiSetSize );
    LUA_REGISTER( L, guiSetPosition );
    LUA_REGISTER( L, guiSetVisible );
    LUA_REGISTER( L, guiSetAlpha );
    LUA_REGISTER( L, guiSetProperty );
    LUA_REGISTER( L, guiBringToFront );
    LUA_REGISTER( L, guiMoveToBack );
    LUA_REGISTER( L, guiCheckBoxSetSelected );
    LUA_REGISTER( L, guiRadioButtonSetSelected );    
    LUA_REGISTER( L, guiGetEnabled );
    LUA_REGISTER( L, guiGetText );
    LUA_REGISTER( L, guiGetFont );
    LUA_REGISTER( L, guiGetSize );
    LUA_REGISTER( L, guiGetPosition );
    LUA_REGISTER( L, guiGetVisible );
    LUA_REGISTER( L, guiGetAlpha );
    LUA_REGISTER( L, guiGetProperty );
    LUA_REGISTER( L, guiGetProperties );
    LUA_REGISTER( L, guiCheckBoxGetSelected );
    LUA_REGISTER( L, guiRadioButtonGetSelected );
    LUA_REGISTER( L, guiGetScreenSize );
    LUA_REGISTER( L, guiProgressBarSetProgress );
    LUA_REGISTER( L, guiProgressBarGetProgress );
    LUA_REGISTER( L, guiEditSetReadOnly );
    LUA_REGISTER( L, guiEditSetMasked );
    LUA_REGISTER( L, guiEditSetMaxLength );
    LUA_REGISTER( L, guiEditSetCaretIndex );
    LUA_REGISTER( L, guiMemoSetReadOnly );
    LUA_REGISTER( L, guiMemoSetCaratIndex );
    LUA_REGISTER( L, guiWindowSetMovable );
    LUA_REGISTER( L, guiWindowSetSizable );
    LUA_REGISTER( L, guiLabelSetColor );
    LUA_REGISTER( L, guiLabelSetVerticalAlign );
    LUA_REGISTER( L, guiLabelSetHorizontalAlign );
    LUA_REGISTER( L, guiLabelGetTextExtent );
    LUA_REGISTER( L, guiLabelGetFontHeight );
    LUA_REGISTER( L, guiGetChatboxLayout );
    LUA_REGISTER( L, guiCreateComboBox );
    LUA_REGISTER( L, guiComboBoxAddItem );
    LUA_REGISTER( L, guiComboBoxRemoveItem );
    LUA_REGISTER( L, guiComboBoxClear );
    LUA_REGISTER( L, guiComboBoxGetSelected );
    LUA_REGISTER( L, guiComboBoxSetSelected );
    LUA_REGISTER( L, guiComboBoxGetItemText );
    LUA_REGISTER( L, guiComboBoxSetItemText );

    // World functions
    LUA_REGISTER( L, getTime );
    LUA_REGISTER( L, getGroundPosition );
    LUA_REGISTER( L, processLineOfSight );
    LUA_REGISTER( L, isLineOfSightClear );
    LUA_REGISTER( L, testLineAgainstWater );
    LUA_REGISTER( L, createWater );
    LUA_REGISTER( L, getWaterLevel );
    LUA_REGISTER( L, getWaterVertexPosition );
    LUA_REGISTER( L, getWorldFromScreenPosition );
    LUA_REGISTER( L, getScreenFromWorldPosition  );
    LUA_REGISTER( L, getWeather );
    LUA_REGISTER( L, getZoneName );
    LUA_REGISTER( L, getGravity );
    LUA_REGISTER( L, getGameSpeed );
    LUA_REGISTER( L, getMinuteDuration );
    LUA_REGISTER( L, getWaveHeight );
    LUA_REGISTER( L, isGarageOpen );
    LUA_REGISTER( L, getGaragePosition );
    LUA_REGISTER( L, getGarageSize );
    LUA_REGISTER( L, getGarageBoundingBox );
    LUA_REGISTER( L, isWorldSpecialPropertyEnabled );
    LUA_REGISTER( L, getBlurLevel );
    LUA_REGISTER( L, getTrafficLightState );
    LUA_REGISTER( L, areTrafficLightsLocked );
    LUA_REGISTER( L, getJetpackMaxHeight );
    LUA_REGISTER( L, getAircraftMaxHeight );

    LUA_REGISTER( L, setTime );
    LUA_REGISTER( L, getSkyGradient );
    LUA_REGISTER( L, setSkyGradient );
    LUA_REGISTER( L, resetSkyGradient );
    LUA_REGISTER( L, getHeatHaze );
    LUA_REGISTER( L, setHeatHaze );
    LUA_REGISTER( L, resetHeatHaze );
    LUA_REGISTER( L, getWaterColor );
    LUA_REGISTER( L, setWaterColor );
    LUA_REGISTER( L, resetWaterColor );
    LUA_REGISTER( L, setWeather );
    LUA_REGISTER( L, setWeatherBlended );
    LUA_REGISTER( L, setGravity );
    LUA_REGISTER( L, setGameSpeed );
    LUA_REGISTER( L, setMinuteDuration );
    LUA_REGISTER( L, setWaterLevel );
    LUA_REGISTER( L, setWaterVertexPosition );
    LUA_REGISTER( L, setWaveHeight );
    LUA_REGISTER( L, setGarageOpen );
    LUA_REGISTER( L, setWorldSpecialPropertyEnabled );
    LUA_REGISTER( L, setBlurLevel );
    LUA_REGISTER( L, setJetpackMaxHeight );
    LUA_REGISTER( L, setCloudsEnabled );
    LUA_REGISTER( L, getCloudsEnabled );
    LUA_REGISTER( L, setTrafficLightState );
    LUA_REGISTER( L, setTrafficLightsLocked );
    LUA_REGISTER( L, getWindVelocity );
    LUA_REGISTER( L, setWindVelocity );
    LUA_REGISTER( L, resetWindVelocity );
    LUA_REGISTER( L, getInteriorSoundsEnabled );
    LUA_REGISTER( L, setInteriorSoundsEnabled );
    LUA_REGISTER( L, getRainLevel );
    LUA_REGISTER( L, setRainLevel );
    LUA_REGISTER( L, resetRainLevel );
    LUA_REGISTER( L, getFarClipDistance );
    LUA_REGISTER( L, setFarClipDistance );
    LUA_REGISTER( L, resetFarClipDistance );
    LUA_REGISTER( L, getFogDistance );
    LUA_REGISTER( L, setFogDistance );
    LUA_REGISTER( L, resetFogDistance );
    LUA_REGISTER( L, getSunColor );
    LUA_REGISTER( L, setSunColor );
    LUA_REGISTER( L, resetSunColor );
    LUA_REGISTER( L, getSunSize );
    LUA_REGISTER( L, setSunSize );
    LUA_REGISTER( L, resetSunSize );
    LUA_REGISTER( L, setAircraftMaxHeight );

    // Input functions
    LUA_REGISTER( L, bindKey );
    LUA_REGISTER( L, unbindKey );
    LUA_REGISTER( L, getKeyState );
    LUA_REGISTER( L, getControlState );
    LUA_REGISTER( L, getAnalogControlState );
    LUA_REGISTER( L, isControlEnabled );
    LUA_REGISTER( L, getBoundKeys );
    LUA_REGISTER( L, getFunctionsBoundToKey );
    LUA_REGISTER( L, getKeyBoundToFunction );
    LUA_REGISTER( L, getCommandsBoundToKey );
    LUA_REGISTER( L, getKeyBoundToCommand );

    LUA_REGISTER( L, setControlState );
    LUA_REGISTER( L, toggleControl );
    LUA_REGISTER( L, toggleAllControls );

    // Projectile funcs
    LUA_REGISTER( L, createProjectile );
    LUA_REGISTER( L, getProjectileType );
	LUA_REGISTER( L, getProjectileTarget );
	LUA_REGISTER( L, getProjectileCreator );
	LUA_REGISTER( L, getProjectileForce );

    // Shape create funcs
    LUA_REGISTER( L, createColCircle );
    LUA_REGISTER( L, createColCuboid );
    LUA_REGISTER( L, createColSphere );
    LUA_REGISTER( L, createColRectangle );
    LUA_REGISTER( L, createColPolygon );
    LUA_REGISTER( L, createColTube );

    // Team get funcs  
    LUA_REGISTER( L, getTeamFromName );
    LUA_REGISTER( L, getTeamName );
    LUA_REGISTER( L, getTeamColor );
    LUA_REGISTER( L, getTeamFriendlyFire );
    LUA_REGISTER( L, getPlayersInTeam );
    LUA_REGISTER( L, countPlayersInTeam );

    // Weapon funcs
    LUA_REGISTER( L, getWeaponNameFromID );
    LUA_REGISTER( L, getWeaponIDFromName );
    LUA_REGISTER( L, getSlotFromWeapon );

    // Community funcs
    LUA_REGISTER( L, getPlayerUserName );
    LUA_REGISTER( L, getPlayerSerial );

    // Map funcs
    LUA_REGISTER( L, isPlayerMapForced );
    LUA_REGISTER( L, isPlayerMapVisible );
    LUA_REGISTER( L, getPlayerMapBoundingBox );

    // Commands
    LUA_REGISTER( L, addCommandHandler );
    LUA_REGISTER( L, getCommandHandler );
    LUA_REGISTER( L, executeCommandHandler );
    LUA_REGISTER( L, removeCommandHandler );

    // Utility
    LUA_REGISTER( L, getNetworkUsageData );
    LUA_REGISTER( L, getNetworkStats );
    LUA_REGISTER( L, getPerformanceStats );
    LUA_REGISTER( L, getPerformanceTimer );

    LUA_REGISTER( L, getVersion );

    // Backwards compatibility
    lua_register( L, "isPlayerInVehicle", isPedInVehicle );
    lua_register( L, "getPlayerOccupiedVehicle", getPedOccupiedVehicle );
}

CLuaMain* CLuaManager::Create( const std::string& name, CFileTranslator& fileRoot )
{
    CLuaMain& main = *new CLuaMain( *this, fileRoot );
    main.SetName( name );

    Init( &main );
    LoadCFunctions( *main );

    m_structures.push_back( &main );
    return &main;
}

bool CLuaManager::Remove( LuaMain *lua )
{
    m_commands.CleanUp( lua );
    return LuaManager::Remove( lua );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.h
*  PURPOSE:     Lua function definitions class header
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Florian Busse <flobu@gmx.net>
*
*****************************************************************************/

#ifndef __CLUAFUNCTIONDEFINITIONS_H
#define __CLUAFUNCTIONDEFINITIONS_H

#include "CLuaCommonUtils.h"
#include "CLuaMain.h"
#include "CLuaTimerManager.h"

namespace CLuaFunctionDefs
{
    void Initialize( class CLuaManager *manager, class CScriptDebugging *debug, class CClientGame *game );

    // Resource functions
    LUA_DECLARE( call );
    LUA_DECLARE( getThisResource );
    LUA_DECLARE( getResourceConfig );
    LUA_DECLARE( getResourceName );
    LUA_DECLARE( getResourceFromName );
    LUA_DECLARE( getResourceRootElement );
    LUA_DECLARE( getResourceGUIElement );
    LUA_DECLARE( getResourceDynamicElementRoot );
    LUA_DECLARE( getResourceExportedFunctions );
    LUA_DECLARE( fileCreateTranslator );

    // Event functions
    LUA_DECLARE( addEvent );
    LUA_DECLARE( addEventHandler );
    LUA_DECLARE( removeEventHandler );
    LUA_DECLARE( triggerEvent );
    LUA_DECLARE( triggerServerEvent );
    LUA_DECLARE( cancelEvent );
    LUA_DECLARE( wasEventCancelled );

    // Sound effects and synth functions
    LUA_DECLARE( playSound );
    LUA_DECLARE( playSound3D );
    LUA_DECLARE( stopSound );
    LUA_DECLARE( setSoundPosition );
    LUA_DECLARE( getSoundPosition );
    LUA_DECLARE( getSoundLength );
    LUA_DECLARE( setSoundPaused );
    LUA_DECLARE( isSoundPaused );
    LUA_DECLARE( setSoundVolume );
    LUA_DECLARE( getSoundVolume );
    LUA_DECLARE( setSoundSpeed );
    LUA_DECLARE( getSoundSpeed );
    LUA_DECLARE( setSoundMinDistance );
    LUA_DECLARE( getSoundMinDistance );
    LUA_DECLARE( setSoundMaxDistance );
    LUA_DECLARE( getSoundMaxDistance );
    LUA_DECLARE( getSoundMetaTags );
    LUA_DECLARE( setSoundEffectEnabled );
    LUA_DECLARE( getSoundEffects );

    // Output functions
    LUA_DECLARE( outputConsole );
    LUA_DECLARE( outputChatBox );
    LUA_DECLARE( enableChat );
    LUA_DECLARE( isChatEnabled );
    LUA_DECLARE( showChat );
    LUA_DECLARE( setClipboard );
    LUA_DECLARE( getClipboard );

    // Element get functions
    LUA_DECLARE( getRootElement );
    LUA_DECLARE( isElement );
    LUA_DECLARE( getElementChildren );
    LUA_DECLARE( getElementChild );
    LUA_DECLARE( getElementChildrenCount );
    LUA_DECLARE( getElementID );
    LUA_DECLARE( getElementByID );
    LUA_DECLARE( getElementByIndex );
    LUA_DECLARE( getElementData );
    LUA_DECLARE( getElementParent );
    LUA_DECLARE( getElementMatrix );
    LUA_DECLARE( getElementPosition );
    LUA_DECLARE( getElementRotation );
    LUA_DECLARE( getElementVelocity );
    LUA_DECLARE( getElementType );
    LUA_DECLARE( getElementsByType );
    LUA_DECLARE( getElementInterior );
    LUA_DECLARE( isElementWithinColShape );
    LUA_DECLARE( isElementWithinMarker );
    LUA_DECLARE( getElementsWithinColShape );
    LUA_DECLARE( getElementDimension );
    LUA_DECLARE( getElementBoundingBox );
    LUA_DECLARE( getElementRadius );
    LUA_DECLARE( isElementAttached );
    LUA_DECLARE( getElementAttachedTo );
    LUA_DECLARE( getAttachedElements );
    LUA_DECLARE( getElementDistanceFromCentreOfMassToBaseOfModel );
    LUA_DECLARE( isElementLocal );
    LUA_DECLARE( getElementAttachedOffsets );
    LUA_DECLARE( getElementAlpha );
    LUA_DECLARE( isElementOnScreen );
    LUA_DECLARE( getElementHealth );
    LUA_DECLARE( isElementStreamedIn );
    LUA_DECLARE( isElementStreamable );
    LUA_DECLARE( getElementModel );
    LUA_DECLARE( getElementColShape );
    LUA_DECLARE( isElementInWater );
    LUA_DECLARE( isElementSyncer );
    LUA_DECLARE( isElementCollidableWith );
    LUA_DECLARE( isElementDoubleSided );
    LUA_DECLARE( getElementCollisionsEnabled );
    LUA_DECLARE( isElementFrozen );

    // Element set funcs
    LUA_DECLARE( createElement );
    LUA_DECLARE( destroyElement );
    LUA_DECLARE( setElementID );
    LUA_DECLARE( setElementData );
    LUA_DECLARE( removeElementData );
    LUA_DECLARE( setElementPosition );
    LUA_DECLARE( setElementRotation );
    LUA_DECLARE( setElementVelocity );
    LUA_DECLARE( setElementParent );  
    LUA_DECLARE( setElementInterior );
    LUA_DECLARE( setElementDimension );
    LUA_DECLARE( attachElements );
    LUA_DECLARE( detachElements );
    LUA_DECLARE( setElementAttachedOffsets );
    LUA_DECLARE( setElementCollisionsEnabled );
    LUA_DECLARE( setElementAlpha );
    LUA_DECLARE( setElementHealth );
    LUA_DECLARE( setElementStreamable );
    LUA_DECLARE( setElementModel );
    LUA_DECLARE( setElementCollidableWith );
    LUA_DECLARE( setElementDoubleSided );
    LUA_DECLARE( setElementFrozen );

    // Radio functions
    LUA_DECLARE( setRadioChannel );
    LUA_DECLARE( getRadioChannel );
    LUA_DECLARE( getRadioChannelName );

    // Player get functions
    LUA_DECLARE( getLocalPlayer );
    LUA_DECLARE( getPlayerName );
    LUA_DECLARE( getPlayerFromName );
    LUA_DECLARE( getPlayerNametagText );
    LUA_DECLARE( getPlayerNametagColor );
    LUA_DECLARE( isPlayerNametagShowing );
    LUA_DECLARE( getPlayerPing );
    LUA_DECLARE( getPlayerTeam );
    LUA_DECLARE( isPlayerDead );
    LUA_DECLARE( getPlayerMoney );
    LUA_DECLARE( getPlayerWantedLevel );
    
    // Player set funcs
    LUA_DECLARE( showPlayerHudComponent );
    LUA_DECLARE( setPlayerMoney );
    LUA_DECLARE( givePlayerMoney );
    LUA_DECLARE( takePlayerMoney );
    LUA_DECLARE( setPlayerNametagText );
    LUA_DECLARE( setPlayerNametagColor );
    LUA_DECLARE( setPlayerNametagShowing );

    // Ped funcs
    LUA_DECLARE( createPed );

    LUA_DECLARE( detonateSatchels );

    LUA_DECLARE( getPedVoice );
    LUA_DECLARE( setPedVoice );
    LUA_DECLARE( getPedTarget );
    LUA_DECLARE( getPedTargetStart );
    LUA_DECLARE( getPedTargetEnd );
    LUA_DECLARE( getPedTargetRange );
    LUA_DECLARE( getPedTargetCollision );
    LUA_DECLARE( getPedWeaponSlot );
    LUA_DECLARE( getPedWeapon );
    LUA_DECLARE( getPedAmmoInClip );
    LUA_DECLARE( getPedTotalAmmo );
    LUA_DECLARE( getPedWeaponMuzzlePosition );
    LUA_DECLARE( getPedStat );
    LUA_DECLARE( getPedOccupiedVehicle );
    LUA_DECLARE( getPedArmor );
    LUA_DECLARE( isPedChoking );
    LUA_DECLARE( isPedDucked );
    LUA_DECLARE( isPedInVehicle );
    LUA_DECLARE( doesPedHaveJetPack );
    LUA_DECLARE( isPedOnGround );
    LUA_DECLARE( getPedTask );
    LUA_DECLARE( getPedSimplestTask );
    LUA_DECLARE( isPedDoingTask );
    LUA_DECLARE( getPedContactElement );
    LUA_DECLARE( getPedRotation );
    LUA_DECLARE( canPedBeKnockedOffBike );
    LUA_DECLARE( getPedBonePosition );
    LUA_DECLARE( getPedClothes );
    LUA_DECLARE( getPedControlState );
    LUA_DECLARE( isPedDoingGangDriveby );
    LUA_DECLARE( getPedAnimation );
    LUA_DECLARE( getPedMoveState );
    LUA_DECLARE( getPedMoveAnim );
    LUA_DECLARE( isPedHeadless );
    LUA_DECLARE( isPedFrozen );
    LUA_DECLARE( isPedFootBloodEnabled );
    LUA_DECLARE( getPedCameraRotation );
    LUA_DECLARE( isPedOnFire );

    LUA_DECLARE( setPedRotation );
    LUA_DECLARE( setPedCanBeKnockedOffBike );
    LUA_DECLARE( setPedAnimation );
    LUA_DECLARE( setPedAnimationProgress );
    LUA_DECLARE( setPedMoveAnim );
    LUA_DECLARE( setPedWeaponSlot );
    LUA_DECLARE( addPedClothes );
    LUA_DECLARE( removePedClothes );
    LUA_DECLARE( setPedControlState );
    LUA_DECLARE( setPedDoingGangDriveby );
    LUA_DECLARE( setPedLookAt );
    LUA_DECLARE( setPedHeadless );
    LUA_DECLARE( setPedFrozen );
    LUA_DECLARE( setPedFootBloodEnabled );
    LUA_DECLARE( setPedCameraRotation );
    LUA_DECLARE( setPedAimTarget );
    LUA_DECLARE( setPedOnFire );

    // Civilian ped functions
    LUA_DECLARE( createCivilian );

    // Clothes and body functions
    LUA_DECLARE( getBodyPartName );
    LUA_DECLARE( getClothesByTypeIndex );
    LUA_DECLARE( getTypeIndexFromClothes );
    LUA_DECLARE( getClothesTypeName );

    // Vehicle get functions
    LUA_DECLARE( createVehicle );

    LUA_DECLARE( getVehicleType );
    LUA_DECLARE( getVehicleColor );
    LUA_DECLARE( getVehicleModelFromName );
    LUA_DECLARE( getVehicleLandingGearDown );
    LUA_DECLARE( getVehicleMaxPassengers );
    LUA_DECLARE( getVehicleOccupant );
    LUA_DECLARE( getVehicleOccupants );
    LUA_DECLARE( getVehicleController );
    LUA_DECLARE( getVehicleSirensOn );
    LUA_DECLARE( getVehicleTurnVelocity );
    LUA_DECLARE( getVehicleTurretPosition );
    LUA_DECLARE( isVehicleLocked );
    LUA_DECLARE( getVehicleUpgradeOnSlot );
    LUA_DECLARE( getVehicleUpgrades );
    LUA_DECLARE( getVehicleUpgradeSlotName );
    LUA_DECLARE( getVehicleCompatibleUpgrades );
    LUA_DECLARE( getVehicleWheelStates );
    LUA_DECLARE( getVehicleDoorState );
    LUA_DECLARE( getVehicleLightState );
    LUA_DECLARE( getVehiclePanelState );
    LUA_DECLARE( getVehicleOverrideLights );
    LUA_DECLARE( getVehicleTowedByVehicle );
    LUA_DECLARE( getVehicleTowingVehicle );
    LUA_DECLARE( getVehiclePaintjob );
    LUA_DECLARE( getVehiclePlateText );
    LUA_DECLARE( isVehicleDamageProof );
    LUA_DECLARE( isVehicleFuelTankExplodable );
    LUA_DECLARE( isVehicleFrozen );
    LUA_DECLARE( isVehicleOnGround );
    LUA_DECLARE( getVehicleName );
    LUA_DECLARE( getVehicleNameFromModel );
    LUA_DECLARE( getVehicleAdjustableProperty );
    LUA_DECLARE( getHelicopterRotorSpeed );
    LUA_DECLARE( getVehicleEngineState );
    LUA_DECLARE( isTrainDerailed );
    LUA_DECLARE( isTrainDerailable );
    LUA_DECLARE( getTrainDirection );
    LUA_DECLARE( getTrainSpeed );
    LUA_DECLARE( getVehicleGravity );
    LUA_DECLARE( isVehicleBlown );
    LUA_DECLARE( getVehicleHeadLightColor );
    LUA_DECLARE( getVehicleCurrentGear );
    LUA_DECLARE( getVehicleHandling );
    LUA_DECLARE( getOriginalHandling );
    LUA_DECLARE( getVehicleDoorOpenRatio );

    // Vehicle set functions
    LUA_DECLARE( fixVehicle );
    LUA_DECLARE( blowVehicle );
    LUA_DECLARE( setVehicleTurnVelocity );
    LUA_DECLARE( setVehicleColor );
    LUA_DECLARE( setVehicleLandingGearDown );
    LUA_DECLARE( setVehicleLocked );
    LUA_DECLARE( setVehicleDoorsUndamageable );
    LUA_DECLARE( setVehicleSirensOn );
    LUA_DECLARE( addVehicleUpgrade );
    LUA_DECLARE( removeVehicleUpgrade );
    LUA_DECLARE( setVehicleDoorState );
    LUA_DECLARE( setVehicleWheelStates );
    LUA_DECLARE( setVehicleLightState );
    LUA_DECLARE( setVehiclePanelState );
    LUA_DECLARE( setVehicleOverrideLights );
    LUA_DECLARE( setVehicleTaxiLightOn );
    LUA_DECLARE( isVehicleTaxiLightOn );
    LUA_DECLARE( attachTrailerToVehicle );
    LUA_DECLARE( detachTrailerFromVehicle );
    LUA_DECLARE( setVehicleEngineState );
    LUA_DECLARE( setVehicleDirtLevel );
    LUA_DECLARE( setVehicleDamageProof );
    LUA_DECLARE( setVehiclePaintjob );
    LUA_DECLARE( setVehicleFuelTankExplodable );
    LUA_DECLARE( setVehicleFrozen );
    LUA_DECLARE( setVehicleAdjustableProperty );
    LUA_DECLARE( setHelicopterRotorSpeed );
    LUA_DECLARE( setTrainDerailed );
    LUA_DECLARE( setTrainDerailable );
    LUA_DECLARE( setTrainDirection );
    LUA_DECLARE( setTrainSpeed );
    LUA_DECLARE( setVehicleGravity );
    LUA_DECLARE( setVehicleHeadLightColor );
    LUA_DECLARE( setVehicleTurretPosition );
    LUA_DECLARE( setVehicleDoorOpenRatio );

    // Object create funcs
    LUA_DECLARE( createObject );
    
    // Object get funcs
    LUA_DECLARE( getObjectScale );

    // Object set funcs
    LUA_DECLARE( moveObject );
    LUA_DECLARE( stopObject );
    LUA_DECLARE( setObjectScale );

    // Explosion functions
    LUA_DECLARE( createExplosion );

    // Fire functions
    LUA_DECLARE( createFire );

    // Audio funcs
    LUA_DECLARE( playMissionAudio );
    LUA_DECLARE( playSoundFrontEnd );
    LUA_DECLARE( preloadMissionAudio );
    LUA_DECLARE( setAmbientSoundEnabled );
    LUA_DECLARE( isAmbientSoundEnabled );
    LUA_DECLARE( resetAmbientSounds );

    // Blip funcs
    LUA_DECLARE( createBlip );
    LUA_DECLARE( createBlipAttachedTo );
    LUA_DECLARE( getBlipIcon );
    LUA_DECLARE( getBlipSize );
    LUA_DECLARE( getBlipColor );
    LUA_DECLARE( getBlipOrdering );
    LUA_DECLARE( getBlipVisibleDistance );

    LUA_DECLARE( setBlipIcon );
    LUA_DECLARE( setBlipSize );
    LUA_DECLARE( setBlipColor );
    LUA_DECLARE( setBlipOrdering );
    LUA_DECLARE( setBlipVisibleDistance );

    // Marker funcs
    LUA_DECLARE( createMarker );

    // Marker get functions
    LUA_DECLARE( getMarkerCount );
    LUA_DECLARE( getMarkerType );
    LUA_DECLARE( getMarkerSize );
    LUA_DECLARE( getMarkerColor );
    LUA_DECLARE( getMarkerTarget );
    LUA_DECLARE( getMarkerIcon );

    // Marker set functions
    LUA_DECLARE( setMarkerType );
    LUA_DECLARE( setMarkerSize );
    LUA_DECLARE( setMarkerColor );
    LUA_DECLARE( setMarkerTarget );
    LUA_DECLARE( setMarkerIcon );

    // Radar-area funcs
    LUA_DECLARE( createRadarArea );
    LUA_DECLARE( getRadarAreaColor );
    LUA_DECLARE( getRadarAreaSize );
    LUA_DECLARE( isRadarAreaFlashing );
    LUA_DECLARE( setRadarAreaColor );
    LUA_DECLARE( setRadarAreaFlashing );
    LUA_DECLARE( setRadarAreaSize );
    LUA_DECLARE( isInsideRadarArea );

    // Pickup create/destroy funcs
    LUA_DECLARE( createPickup );

    // Pickup get funcs
    LUA_DECLARE( getPickupType );
    LUA_DECLARE( getPickupWeapon );
    LUA_DECLARE( getPickupAmount );
    LUA_DECLARE( getPickupAmmo );

    // Pickup get funcs
    LUA_DECLARE( setPickupType );

    // Cam get funcs
    LUA_DECLARE( getCameraViewMode );
    LUA_DECLARE( getCameraMatrix );
    LUA_DECLARE( getCameraTarget );
    LUA_DECLARE( getCameraInterior );
    LUA_DECLARE( getCameraGoggleEffect );

    // Cam set funcs
    LUA_DECLARE( setCameraMatrix );
    LUA_DECLARE( setCameraTarget );
    LUA_DECLARE( setCameraInterior );
    LUA_DECLARE( fadeCamera );
    LUA_DECLARE( setCameraClip );
    LUA_DECLARE( setCameraViewMode );
    LUA_DECLARE( setCameraGoggleEffect );

    // Cursor funcs
    LUA_DECLARE( getCursorPosition );
    LUA_DECLARE( setCursorPosition );
    LUA_DECLARE( isCursorShowing );
    
    LUA_DECLARE( showCursor );

    // Engine (RenderWare abtract) functions
    LUA_DECLARE( engineLoadDFF );
    LUA_DECLARE( engineLoadTXD );
    LUA_DECLARE( engineLoadCOL );
    LUA_DECLARE( engineGetGameTextures );
    LUA_DECLARE( engineModelInfoHasLoaded );
    LUA_DECLARE( engineModelInfoCloneObject );
    LUA_DECLARE( engineCreateLight );
    LUA_DECLARE( engineCreateFrame );
    LUA_DECLARE( engineCreateCamera );
    LUA_DECLARE( engineCreateClump );
    LUA_DECLARE( engineIsRendering );
    LUA_DECLARE( engineImportTXD );
    LUA_DECLARE( engineReplaceCOL );
    LUA_DECLARE( engineRestoreCOL );
    LUA_DECLARE( engineReplaceModel );
    LUA_DECLARE( engineRestoreModel );
    LUA_DECLARE( engineSetModelLODDistance );
    LUA_DECLARE( engineSetEnvMapRenderingEnabled );
    LUA_DECLARE( engineIsEnvMapRenderingEnabled );
    LUA_DECLARE( engineSetAsynchronousLoading );
    LUA_DECLARE( engineApplyShaderToWorldTexture );
    LUA_DECLARE( engineRemoveShaderFromWorldTexture );
    LUA_DECLARE( engineGetModelNameFromID );
    LUA_DECLARE( engineGetModelIDFromName );
    LUA_DECLARE( engineGetModelTextureNames );
    LUA_DECLARE( engineGetVisibleTextureNames );
    LUA_DECLARE( engineSetInfiniteStreamingEnabled );
    LUA_DECLARE( engineIsInfiniteStreamingEnabled );
    LUA_DECLARE( engineSetStrictStreamingNodeDistributionEnabled );
    LUA_DECLARE( engineIsStrictStreamingNodeDistributionEnabled );
    LUA_DECLARE( engineSetStreamingGCOnDemandEnabled );
    LUA_DECLARE( engineIsStreamingGCOnDemandEnabled );
    LUA_DECLARE( engineAllowStreamingNodeStealing );
    LUA_DECLARE( engineIsStreamingNodeStealingAllowed );
    LUA_DECLARE( engineGetActiveStreamingEntityCount );
    LUA_DECLARE( engineGetActiveStreamingFreeSlotCount );
    LUA_DECLARE( engineGetActiveStreamingEntities );
    LUA_DECLARE( engineGetStreamingFocus );
    LUA_DECLARE( engineStreamingIsElementManaged );
    LUA_DECLARE( engineGetModelRefCount );
    LUA_DECLARE( engineSetWorldStreamingEnabled );
    LUA_DECLARE( engineIsWorldStreamingEnabled );
    LUA_DECLARE( engineGetEntitiesInRenderQueue );
    LUA_DECLARE( engineGetGamePoolLimits );
    LUA_DECLARE( engineGetStreamingInfo );
    LUA_DECLARE( engineStreamingEnableFiberedLoading );
    LUA_DECLARE( engineStreamingIsFiberedLoadingEnabled );
    LUA_DECLARE( engineStreamingSetFiberedPerfMultiplier );
    LUA_DECLARE( engineStreamingGetFiberedPerfMultiplier );
    LUA_DECLARE( engineSetGlobalLightingAlwaysEnabled );
    LUA_DECLARE( engineIsGlobalLightingAlwaysEnabled );
    LUA_DECLARE( engineSetLocalLightingAlwaysEnabled );
    LUA_DECLARE( engineIsLocalLightingAlwaysEnabled );
    LUA_DECLARE( engineSetWorldRenderMode );
    LUA_DECLARE( engineGetWorldRenderMode );
    LUA_DECLARE( engineIsModelBeingUsed );
    LUA_DECLARE( engineGetRenderBucketStatistics );

    // Drawing funcs (low-level)
    LUA_DECLARE( dxDrawLine );
    LUA_DECLARE( dxDrawLine3D );
    LUA_DECLARE( dxDrawText );
    LUA_DECLARE( dxDrawRectangle );
    LUA_DECLARE( dxDrawImage );
    LUA_DECLARE( dxDrawImageSection );
    LUA_DECLARE( dxGetTextWidth );
    LUA_DECLARE( dxGetFontHeight );
    LUA_DECLARE( dxCreateFont );
    LUA_DECLARE( dxCreateTexture );
    LUA_DECLARE( dxCreateShader );
    LUA_DECLARE( dxCreateRenderTarget );
    LUA_DECLARE( dxCreateScreenSource );
    LUA_DECLARE( dxGetMaterialSize );
    LUA_DECLARE( dxSetShaderValue );
    LUA_DECLARE( dxSetRenderTarget );
    LUA_DECLARE( dxUpdateScreenSource );
    LUA_DECLARE( dxGetStatus );
    LUA_DECLARE( dxSetBlendMode );
    LUA_DECLARE( dxGetBlendMode );
    LUA_DECLARE( dxSetTestMode );

    // Pfft utils
    LUA_DECLARE( tocolor );
    LUA_DECLARE( getColorFromString );
    LUA_DECLARE( gettok );
    LUA_DECLARE( split );
    
    // gui funcs
    LUA_DECLARE( guiGetInputEnabled );
    LUA_DECLARE( guiGetInputMode );
    LUA_DECLARE( guiSetInputEnabled );
    LUA_DECLARE( guiSetInputMode );
    LUA_DECLARE( isChatBoxInputActive );
    LUA_DECLARE( isConsoleActive );
    LUA_DECLARE( isDebugViewActive );
    LUA_DECLARE( isMainMenuActive );
    LUA_DECLARE( isMTAWindowActive );
    LUA_DECLARE( isTransferBoxActive );
    LUA_DECLARE( guiCreateWindow );
    LUA_DECLARE( guiCreateLabel );
    LUA_DECLARE( guiCreateButton );
    LUA_DECLARE( guiCreateEdit );
    LUA_DECLARE( guiCreateMemo );
    LUA_DECLARE( guiCreateGridList );
    LUA_DECLARE( guiCreateScrollBar );
    LUA_DECLARE( guiCreateScrollPane );
    LUA_DECLARE( guiCreateStaticImage );
    LUA_DECLARE( guiCreateTabPanel );
    LUA_DECLARE( guiCreateTab );
    LUA_DECLARE( guiCreateProgressBar );
    LUA_DECLARE( guiCreateCheckBox );
    LUA_DECLARE( guiCreateRadioButton );
    LUA_DECLARE( guiCreateFont );
    LUA_DECLARE( guiGetSelectedTab );
    LUA_DECLARE( guiSetSelectedTab );
    LUA_DECLARE( guiDeleteTab );
    LUA_DECLARE( guiScrollBarSetScrollPosition );
    LUA_DECLARE( guiScrollBarGetScrollPosition );
    LUA_DECLARE( guiGridListSetSortingEnabled );
    LUA_DECLARE( guiGridListAddColumn );
    LUA_DECLARE( guiGridListRemoveColumn );
    LUA_DECLARE( guiGridListSetColumnWidth );
    LUA_DECLARE( guiGridListSetScrollBars );
    LUA_DECLARE( guiGridListGetRowCount );
    LUA_DECLARE( guiGridListGetColumnCount );
    LUA_DECLARE( guiGridListAddRow );
    LUA_DECLARE( guiGridListInsertRowAfter );
    LUA_DECLARE( guiGridListRemoveRow );
    LUA_DECLARE( guiGridListAutoSizeColumn );
    LUA_DECLARE( guiGridListClear );
    LUA_DECLARE( guiGridListSetItemText );
    LUA_DECLARE( guiGridListGetItemText );
    LUA_DECLARE( guiGridListSetItemData );
    LUA_DECLARE( guiGridListGetItemData );
    LUA_DECLARE( guiGridListSetItemColor );
    LUA_DECLARE( guiGridListGetItemColor );
    LUA_DECLARE( guiGridListSetSelectionMode );
    LUA_DECLARE( guiGridListGetSelectedItem );
    LUA_DECLARE( guiGridListGetSelectedItems );
    LUA_DECLARE( guiGridListGetSelectedCount );
    LUA_DECLARE( guiGridListSetSelectedItem );
    LUA_DECLARE( guiScrollPaneSetScrollBars );
    LUA_DECLARE( guiScrollPaneSetHorizontalScrollPosition );
    LUA_DECLARE( guiScrollPaneGetHorizontalScrollPosition );
    LUA_DECLARE( guiScrollPaneSetVerticalScrollPosition );
    LUA_DECLARE( guiScrollPaneGetVerticalScrollPosition );
    LUA_DECLARE( guiStaticImageLoadImage );
    LUA_DECLARE( guiSetEnabled );
    LUA_DECLARE( guiSetText );
    LUA_DECLARE( guiSetFont );
    LUA_DECLARE( guiSetSize );
    LUA_DECLARE( guiSetPosition );
    LUA_DECLARE( guiSetVisible );
    LUA_DECLARE( guiSetAlpha );
    LUA_DECLARE( guiSetProperty );
    LUA_DECLARE( guiBringToFront );
    LUA_DECLARE( guiMoveToBack );
    LUA_DECLARE( guiCheckBoxSetSelected );
    LUA_DECLARE( guiRadioButtonSetSelected );    
    LUA_DECLARE( guiGetEnabled );
    LUA_DECLARE( guiGetText );
    LUA_DECLARE( guiGetFont );
    LUA_DECLARE( guiGetSize );
    LUA_DECLARE( guiGetPosition );
    LUA_DECLARE( guiGetVisible );
    LUA_DECLARE( guiGetAlpha );
    LUA_DECLARE( guiGetProperty );
    LUA_DECLARE( guiGetProperties );
    LUA_DECLARE( guiCheckBoxGetSelected );
    LUA_DECLARE( guiRadioButtonGetSelected );
    LUA_DECLARE( guiGetScreenSize );
    LUA_DECLARE( guiProgressBarSetProgress );
    LUA_DECLARE( guiProgressBarGetProgress );
    LUA_DECLARE( guiEditSetReadOnly );
    LUA_DECLARE( guiEditSetMasked );
    LUA_DECLARE( guiEditSetMaxLength );
    LUA_DECLARE( guiEditSetCaretIndex );
    LUA_DECLARE( guiMemoSetReadOnly );
    LUA_DECLARE( guiMemoSetCaratIndex );
    LUA_DECLARE( guiWindowSetMovable );
    LUA_DECLARE( guiWindowSetSizable );
    LUA_DECLARE( guiLabelSetColor );
    LUA_DECLARE( guiLabelSetVerticalAlign );
    LUA_DECLARE( guiLabelSetHorizontalAlign );
    LUA_DECLARE( guiLabelGetTextExtent );
    LUA_DECLARE( guiLabelGetFontHeight );
    LUA_DECLARE( guiGetChatboxLayout );
    LUA_DECLARE( guiCreateComboBox );
    LUA_DECLARE( guiComboBoxAddItem );
    LUA_DECLARE( guiComboBoxRemoveItem );
    LUA_DECLARE( guiComboBoxClear );
    LUA_DECLARE( guiComboBoxGetSelected );
    LUA_DECLARE( guiComboBoxSetSelected );
    LUA_DECLARE( guiComboBoxGetItemText );
    LUA_DECLARE( guiComboBoxSetItemText );

    // World functions
    LUA_DECLARE( getTime );
    LUA_DECLARE( getGroundPosition );
    LUA_DECLARE( processLineOfSight );
    LUA_DECLARE( isLineOfSightClear );
    LUA_DECLARE( testLineAgainstWater );
    LUA_DECLARE( createWater );
    LUA_DECLARE( getWaterLevel );
    LUA_DECLARE( getWaterVertexPosition );
    LUA_DECLARE( getWorldFromScreenPosition );
    LUA_DECLARE( getScreenFromWorldPosition  );
    LUA_DECLARE( getWeather );
    LUA_DECLARE( getZoneName );
    LUA_DECLARE( getGravity );
    LUA_DECLARE( getGameSpeed );
    LUA_DECLARE( getMinuteDuration );
    LUA_DECLARE( getWaveHeight );
    LUA_DECLARE( isGarageOpen );
    LUA_DECLARE( getGaragePosition );
    LUA_DECLARE( getGarageSize );
    LUA_DECLARE( getGarageBoundingBox );
    LUA_DECLARE( isWorldSpecialPropertyEnabled );
    LUA_DECLARE( getBlurLevel );
    LUA_DECLARE( getTrafficLightState );
    LUA_DECLARE( areTrafficLightsLocked );
    LUA_DECLARE( getJetpackMaxHeight );
    LUA_DECLARE( getAircraftMaxHeight );

    LUA_DECLARE( setTime );
    LUA_DECLARE( getSkyGradient );
    LUA_DECLARE( setSkyGradient );
    LUA_DECLARE( resetSkyGradient );
    LUA_DECLARE( getHeatHaze );
    LUA_DECLARE( setHeatHaze );
    LUA_DECLARE( resetHeatHaze );
    LUA_DECLARE( getWaterColor );
    LUA_DECLARE( setWaterColor );
    LUA_DECLARE( resetWaterColor );
    LUA_DECLARE( setWeather );
    LUA_DECLARE( setWeatherBlended );
    LUA_DECLARE( setGravity );
    LUA_DECLARE( setGameSpeed );
    LUA_DECLARE( setMinuteDuration );
    LUA_DECLARE( setWaterLevel );
    LUA_DECLARE( setWaterVertexPosition );
    LUA_DECLARE( setWaveHeight );
    LUA_DECLARE( setGarageOpen );
    LUA_DECLARE( setWorldSpecialPropertyEnabled );
    LUA_DECLARE( setBlurLevel );
    LUA_DECLARE( setJetpackMaxHeight );
    LUA_DECLARE( setCloudsEnabled );
    LUA_DECLARE( getCloudsEnabled );
    LUA_DECLARE( setTrafficLightState );
    LUA_DECLARE( setTrafficLightsLocked );
    LUA_DECLARE( getWindVelocity );
    LUA_DECLARE( setWindVelocity );
    LUA_DECLARE( resetWindVelocity );
    LUA_DECLARE( getInteriorSoundsEnabled );
    LUA_DECLARE( setInteriorSoundsEnabled );
    LUA_DECLARE( getRainLevel );
    LUA_DECLARE( setRainLevel );
    LUA_DECLARE( resetRainLevel );
    LUA_DECLARE( getFarClipDistance );
    LUA_DECLARE( setFarClipDistance );
    LUA_DECLARE( resetFarClipDistance );
    LUA_DECLARE( getFogDistance );
    LUA_DECLARE( setFogDistance );
    LUA_DECLARE( resetFogDistance );
    LUA_DECLARE( getSunColor );
    LUA_DECLARE( setSunColor );
    LUA_DECLARE( resetSunColor );
    LUA_DECLARE( getSunSize );
    LUA_DECLARE( setSunSize );
    LUA_DECLARE( resetSunSize );
    LUA_DECLARE( setAircraftMaxHeight );

    // Input functions
    LUA_DECLARE( bindKey );
    LUA_DECLARE( unbindKey );
    LUA_DECLARE( getKeyState );
    LUA_DECLARE( getControlState );
    LUA_DECLARE( getAnalogControlState );
    LUA_DECLARE( isControlEnabled );
    LUA_DECLARE( getBoundKeys );
    LUA_DECLARE( getFunctionsBoundToKey );
    LUA_DECLARE( getKeyBoundToFunction );
    LUA_DECLARE( getCommandsBoundToKey );
    LUA_DECLARE( getKeyBoundToCommand );

    LUA_DECLARE( setControlState );
    LUA_DECLARE( toggleControl );
    LUA_DECLARE( toggleAllControls );

    // Projectile funcs
    LUA_DECLARE( createProjectile );
    LUA_DECLARE( getProjectileType );
	LUA_DECLARE( getProjectileTarget );
	LUA_DECLARE( getProjectileCreator );
	LUA_DECLARE( getProjectileForce );

    // Shape create funcs
    LUA_DECLARE( createColCircle );
    LUA_DECLARE( createColCuboid );
    LUA_DECLARE( createColSphere );
    LUA_DECLARE( createColRectangle );
    LUA_DECLARE( createColPolygon );
    LUA_DECLARE( createColTube );

    // Team get funcs  
    LUA_DECLARE( getTeamFromName );
    LUA_DECLARE( getTeamName );
    LUA_DECLARE( getTeamColor );
    LUA_DECLARE( getTeamFriendlyFire );
    LUA_DECLARE( getPlayersInTeam );
    LUA_DECLARE( countPlayersInTeam );

    // Weapon funcs
    LUA_DECLARE( getWeaponNameFromID );
    LUA_DECLARE( getWeaponIDFromName );
    LUA_DECLARE( getSlotFromWeapon );

    // Community funcs
    LUA_DECLARE( getPlayerUserName );
    LUA_DECLARE( getPlayerSerial );

    // Map funcs
    LUA_DECLARE( isPlayerMapForced );
    LUA_DECLARE( isPlayerMapVisible );
    LUA_DECLARE( getPlayerMapBoundingBox );

    // Commands
    LUA_DECLARE( addCommandHandler );
    LUA_DECLARE( getCommandHandler );
    LUA_DECLARE( executeCommandHandler );
    LUA_DECLARE( removeCommandHandler );

    // Utility
    LUA_DECLARE( getNetworkUsageData );
    LUA_DECLARE( getNetworkStats );
    LUA_DECLARE( getPerformanceStats );
    LUA_DECLARE( getPerformanceTimer );

    LUA_DECLARE( getVersion );

    // Object references
    extern CLuaManager*                 m_pLuaManager;
    extern CScriptDebugging*            m_pScriptDebugging;
    extern CClientGame*                 m_pClientGame;
    extern CClientManager*              m_pManager;
    extern CClientEntity*               m_pRootEntity;
    extern CClientGUIManager*           m_pguiManager;
    extern CClientPedManager*           m_pPedManager;
    extern CClientPlayerManager*        m_pPlayerManager;
    extern CClientRadarMarkerManager*   m_pRadarMarkerManager;
    extern CResourceManager*            m_pResourceManager;
    extern CClientVehicleManager*       m_pVehicleManager;
    extern CClientColManager*           m_pColManager;
    extern CClientObjectManager*        m_pObjectManager;
    extern CClientTeamManager*          m_pTeamManager;
    extern CRenderWare*                 m_pRenderWare;
    extern CClientMarkerManager*        m_pMarkerManager;
    extern CClientPickupManager*        m_pPickupManager;
    extern CRegisteredCommands*         m_pRegisteredCommands;
};

#endif

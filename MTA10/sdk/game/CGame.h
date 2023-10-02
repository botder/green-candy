/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CGame.h
*  PURPOSE:     Game base interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME
#define __CGAME

// use this to check if you're using SA or VC headers
#define GTA_SA

typedef void ( InRenderer ) ( void );

#include <windows.h>
#include "Common.h"

#include "CRenderWare.h"
#include "C3DMarkers.h"
#include "CAERadioTrackManager.h"
#include "CAnimBlendAssociation.h"
#include "CAnimBlock.h"
#include "CAnimManager.h"
#include "CAudio.h"
#include "CCam.h"
#include "CCamera.h"
#include "CCarEnterExit.h"
#include "CClock.h"
#include "CCheckpoints.h"
#include "CControllerConfigManager.h"
#include "CCoronas.h"
#include "CEventDamage.h"
#include "CEventGunShot.h"
#include "CEventList.h"
#include "CExplosionManager.h"
#include "CFireManager.h"
#include "CFont.h"
#include "CFx.h"
#include "CGarages.h"
#include "CHandlingManager.h"
#include "CHud.h"
#include "CKeyGen.h"
#include "CMenuManager.h"
#include "CStreaming.h"
#include "CModelInfo.h"
#include "CPad.h"
#include "CPadManager.h"
#include "CPathFind.h"
#include "CPedDamageResponse.h"
#include "CPedModelInfo.h"
#include "CPickups.h"
#include "CPlayerInfo.h"
#include "CPopulation.h"
#include "CVehicle.h"
#include "CAutomobile.h"
#include "CPlane.h"
#include "CHeli.h"
#include "CAutomobileTrailer.h"
#include "CQuadBike.h"
#include "CMonsterTruck.h"
#include "CBoat.h"
#include "CTrain.h"
#include "CBike.h"
#include "CBicycle.h"
#include "CPed.h"
#include "CPlayerPed.h"
#include "CProjectile.h"
#include "CProjectileInfo.h"
#include "CRadar.h"
#include "CRestart.h"
#include "CRopes.h"
#include "CSettings.h"
#include "CStats.h"
#include "CTaskManagementSystem.h"
#include "CTasks.h"
#include "CText.h"
#include "CTheCarGenerators.h"
#include "RenderWare/include.h"
#include "CModelManager.h"
#include "CTextureManager.h"
#include "CVisibilityPlugins.h"
#include "CWaterManager.h"
#include "CWeather.h"
#include "CWeaponInfo.h"
#include "CWorld.h"
#include "CPools.h"
#include "TaskCarAccessories.h"

enum eGameVersion 
{
    VERSION_ALL = 0,
    VERSION_EU_10 = 5,
    VERSION_US_10 = 11,
    VERSION_11 = 15,
    VERSION_20 = 20,
    VERSION_UNKNOWN = 0xFF,
};

class __declspec(novtable) CGame 
{
public:
    typedef std::list <CEntity*> entityList_t;

    virtual                             ~CGame()    {};

    virtual void                        RegisterMultiplayer( CMultiplayer *mp ) = 0;
    virtual void                        UnregisterMultiplayer( CMultiplayer *mp ) = 0;

    virtual CPools*                     GetPools() = 0;
    virtual CPlayerInfo*                GetPlayerInfo() = 0;
    virtual CProjectileInfo*            GetProjectileInfo() = 0;
    virtual CRadar*                     GetRadar() = 0;
    virtual CRestart*                   GetRestart() = 0;
    virtual CClock*                     GetClock() = 0;
    virtual CCheckpoints*               GetCheckpoints() = 0;
    virtual CCoronas*                   GetCoronas() = 0;
    virtual CEventList*                 GetEventList() = 0;
    virtual CFireManager*               GetFireManager() = 0;
    virtual CExplosionManager*          GetExplosionManager() = 0;
    virtual CGarages*                   GetGarages() = 0;
    virtual CHud*                       GetHud() = 0;
    virtual CWeather*                   GetWeather() = 0;
    virtual CWorld*                     GetWorld() = 0;
    virtual CCamera*                    GetCamera() = 0;
    virtual CPickups*                   GetPickups() = 0;
    virtual C3DMarkers*                 Get3DMarkers() = 0;
    virtual CPad*                       GetPad() = 0;
    virtual CPadManager*                GetPadManager() = 0;
    virtual CAERadioTrackManager*       GetAERadioTrackManager() = 0;
    virtual CAudio*                     GetAudio() = 0;
    virtual CMenuManager*               GetMenuManager() = 0;
    virtual CText*                      GetText() = 0;
    virtual CStats*                     GetStats() = 0;
    virtual CTasks*                     GetTasks() = 0;
    virtual CFont*                      GetFont() = 0;
    virtual CPathFind*                  GetPathFind() = 0;
    virtual CPopulation*                GetPopulation() = 0;
    virtual CGameSettings*              GetSettings() = 0;
    virtual CCarEnterExit*              GetCarEnterExit() = 0;
    virtual CControllerConfigManager*   GetControllerConfigManager() = 0;
    virtual CRenderWare*                GetRenderWare() = 0;
    virtual CModelManager*              GetModelManager() = 0;
    virtual CTextureManager*            GetTextureManager() = 0;
    virtual CHandlingManager*           GetHandlingManager() = 0;
    virtual CAnimManager*               GetAnimManager() = 0;
    virtual CStreaming*                 GetStreaming() = 0;
    virtual CVisibilityPlugins*         GetVisibilityPlugins() = 0;
    virtual CKeyGen*                    GetKeyGen() = 0;
    virtual CRopes*                     GetRopes() = 0;
    virtual CFx*                        GetFx() = 0;
    virtual CWaterManager*              GetWaterManager() = 0;
    
    virtual CWeaponInfo*                GetWeaponInfo( eWeaponType weapon, eWeaponSkill skill = WEAPONSKILL_STD ) = 0;
    virtual CModelInfo*                 GetModelInfo( modelId_t id ) = 0;

    virtual unsigned long       GetSystemTime() = 0;
    virtual bool                IsAtMenu() = 0;
    virtual bool                IsGameLoaded() = 0;
    virtual void                StartGame() = 0;
    virtual void                SetSystemState( eSystemState State ) = 0;
    virtual eSystemState        GetSystemState() = 0;
    virtual void                Pause( bool bPaused ) = 0;
    virtual bool                IsPaused() = 0;
    virtual bool                IsInForeground() = 0;
    virtual void                DisableRenderer( bool bDisabled ) = 0;
    virtual void                SetRenderHook( InRenderer* pInRenderer ) = 0;
    virtual void                TakeScreenshot( char * szFileName ) = 0;
    virtual void                SetTimeScale( float fTimeScale ) = 0;
    virtual float               GetFPS() = 0;
    virtual float               GetTimeStep() = 0;
    virtual float               GetOldTimeStep() = 0;
    virtual float               GetTimeScale() = 0;

    virtual void                Initialize() = 0;
    virtual void                Reset() = 0;
    virtual void                OnPreFrame() = 0;
    virtual void                OnFrame() = 0;

    virtual void                SetEntityReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef );

    virtual void                SetEntityPreRenderCallback( gameEntityPreRenderCallback_t callback ) = 0;
    virtual void                SetEntityRenderCallback( gameEntityRenderCallback_t callback ) = 0;
    virtual void                SetEntityRenderUnderwaterCallback( gameEntityRenderUnderwaterCallback_t callback ) = 0;
    virtual void                SetEntityRenderPostProcessCallback( gameEntityPostProcessCallback_t callback ) = 0;

    virtual entityList_t        GetEntitiesInRenderQueue( void );

    virtual void                ResetShaders() = 0;

    virtual bool                InitLocalPlayer() = 0;

    virtual float               GetGravity() = 0;
    virtual void                SetGravity( float fGravity ) = 0;

    virtual float               GetGameSpeed() = 0;
    virtual void                SetGameSpeed( float fSpeed ) = 0;

    virtual unsigned long       GetMinuteDuration() = 0;
    virtual void                SetMinuteDuration( unsigned long ulDelay ) = 0;

    virtual void                HideRadar( bool hide ) = 0;
    virtual bool                IsRadarHidden( void ) = 0;

    virtual void                SetCenterOfWorld( CEntity *streamingEntity, const CVector *pos, float heading );

    virtual unsigned char       GetBlurLevel() = 0;
    virtual void                SetBlurLevel( unsigned char ucLevel ) = 0;

    virtual eGameVersion        GetGameVersion() = 0;

    virtual bool                IsCheatEnabled( const char* szCheatName ) = 0;
    virtual bool                SetCheatEnabled( const char* szCheatName, bool bEnable ) = 0;
    virtual void                ResetCheats() = 0;

    virtual bool                VerifySADataFileNames() = 0;
    virtual bool                PerformChecks() = 0;
    virtual int&                GetCheckStatus() = 0;

    virtual void                SetAsyncLoadingFromSettings( bool bSettingsDontUse, bool bSettingsEnabled ) = 0;
    virtual void                SetAsyncLoadingFromScript( bool bScriptEnabled, bool bScriptForced ) = 0;
    virtual void                SuspendASyncLoading( bool bSuspend ) = 0;
    virtual bool                IsASyncLoadingEnabled( bool bIgnoreSuspend = false ) = 0;

    virtual bool                HasCreditScreenFadedOut() = 0;
    virtual void                FlushPendingRestreamIPL() = 0;
    virtual void                DisableVSync() = 0;
};

#endif

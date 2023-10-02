/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CGameSA.h
*  PURPOSE:     Header file for base game logic handling class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA
#define __CGAMESA

#include <game/CGame.h>

#define     MAX_MEMORY_OFFSET_1_0           0xCAF008

#define     CLASS_CCamera                   0xB6F028    // ##SA##
#define     CLASS_CPad                      0xB73458    // ##SA##
#define     CLASS_CGarages                  0x96C048    // ##SA##
#define     CLASS_CFx                       0xa9ae00    // ##SA##
#define     CLASS_CMenuManager              0xBA6748    // ##SA##

#define     CLASS_RwCamera                  0xB6F97C

#define     ARRAY_WeaponInfo                0xC8AAB8    // ##SA##
#define     CLASSSIZE_WeaponInfo            112         // ##SA##
#define     NUM_WeaponInfosStdSkill         WEAPONTYPE_LAST_WEAPONTYPE
#define     NUM_WeaponInfosOtherSkill       11
#define     NUM_WeaponInfosTotal            (NUM_WeaponInfosStdSkill + (3*NUM_WeaponInfosOtherSkill)) // std, (poor, pro, special)

#define     MODELINFO_LAST_PLAYER_ID        288         // ??

#define     FUNC_GetLevelFromPosition       0x4DD300

#define     FUNC_CDebug_DebugDisplayTextBuffer      0x532260
#define     FUNC_JPegCompressScreenToFile   0x5D0820

#define     VAR_FlyingCarsEnabled           0x969160 // ##SA##
#define     VAR_ExtraBunnyhopEnabled        0x969161 // ##SA##
#define     VAR_HoveringCarsEnabled         0x969152 // ##SA##
#define     VAR_ExtraJumpEnabled            0x96916C // ##SA##
#define     VAR_TankModeEnabled             0x969164 // ##SA##
#define     VAR_NoReloadEnabled             0x969178 // ##SA##
#define     VAR_PerfectHandling             0x96914C // ##SA##
#define     VAR_AllCarsHaveNitro            0x969165 // ##SA##
#define     VAR_BoatsCanFly                 0x969153 // ##SA##
#define     VAR_InfiniteOxygen              0x96916E // ##SA##
#define     VAR_FasterClock                 0x96913B // ##SA##
#define     VAR_FasterGameplay              0x96913C // ##SA##
#define     VAR_SlowerGameplay              0x96913D // ##SA##
#define     VAR_AlwaysMidnight              0x969167 // ##SA##
#define     VAR_FullWeaponAiming            0x969179 // ##SA##
#define     VAR_InfiniteHealth              0x96916D // ##SA##
#define     VAR_NeverWanted                 0x969171 // ##SA##
#define     VAR_HealthArmorMoney            0x969133 // ##SA##
#define     VAR_WalkUnderwater              0x6C2759

#define CHEAT_HOVERINGCARS          "hovercars"
#define CHEAT_FLYINGCARS            "aircars"
#define CHEAT_EXTRABUNNYHOP         "extrabunny"
#define CHEAT_EXTRAJUMP             "extrajump"
#define CHEAT_TANKMODE              "tankmode"
#define CHEAT_NORELOAD              "noreload"
#define CHEAT_PERFECTHANDLING       "perfecthandling"
#define CHEAT_ALLCARSHAVENITRO      "allcarshavenitro"
#define CHEAT_BOATSCANFLY           "airboats"
#define CHEAT_INFINITEOXYGEN        "infiniteoxygen"
#define CHEAT_WALKUNDERWATER        "walkunderwater"
#define CHEAT_FASTERCLOCK           "fasterclock"
#define CHEAT_FASTERGAMEPLAY        "fastergameplay"
#define CHEAT_SLOWERGAMEPLAY        "slowergameplay"
#define CHEAT_ALWAYSMIDNIGHT        "alwaysmidnight"
#define CHEAT_FULLWEAPONAIMING      "fullweaponaiming"
#define CHEAT_INFINITEHEALTH        "infinitehealth"
#define CHEAT_NEVERWANTED           "neverwanted"
#define CHEAT_HEALTARMORMONEY       "healtharmormoney"

struct SCheatSA {
    BYTE*   m_byAddress; //Cheat Address
    bool    m_bEnabled; //Cheat State
    bool    m_bCanBeSet; //Cheat can be set with setWorldSpecialPropertyEnabled
    SCheatSA(BYTE* Address, bool bCanBeSet = true) {
        m_byAddress = Address;
        m_bCanBeSet = bCanBeSet;
    }
};
class CGameSA : public CGame
{
    friend class COffsets;

private:
    CWeaponInfoSA*      WeaponInfos[NUM_WeaponInfosTotal];
    CModelInfoSA        ModelInfo[MAX_RESOURCES];
public:
                                        CGameSA(); // constructor
                                        ~CGameSA();

    void                                RegisterMultiplayer( CMultiplayer *mp );
    void                                UnregisterMultiplayer( CMultiplayer *mp );

    inline CPoolsSA*                    GetPools()                      { return m_pPools; };
    inline CPlayerInfoSA*               GetPlayerInfo()                 { return m_pPlayerInfo; };
    inline CProjectileInfoSA*           GetProjectileInfo()             { return m_pProjectileInfo; };
    inline CRadarSA*                    GetRadar()                      { return m_pRadar; };
    inline CRestartSA*                  GetRestart()                    { return m_pRestart; };
    inline CClockSA*                    GetClock()                      { return m_pClock; };
    inline CCoronasSA*                  GetCoronas()                    { return m_pCoronas; };
    inline CCheckpointsSA*              GetCheckpoints()                { return m_pCheckpoints; };
    inline CEventListSA*                GetEventList()                  { return m_pEventList; };
    inline CFireManagerSA*              GetFireManager()                { return m_pFireManager; };
    inline CExplosionManagerSA*         GetExplosionManager()           { return m_pExplosionManager; };
    inline CGaragesSA*                  GetGarages()                    { return m_pGarages; };
    inline CHudSA*                      GetHud()                        { return m_pHud; };
    inline CWeatherSA*                  GetWeather()                    { return m_pWeather; };
    inline CWorldSA*                    GetWorld()                      { return m_pWorld; };
    inline CCameraSA*                   GetCamera()                     { return m_pCamera; };
    inline CPickupsSA*                  GetPickups()                    { return m_pPickups; };
    inline C3DMarkersSA*                Get3DMarkers()                  { return m_p3DMarkers; };
    inline CPadSA*                      GetPad()                        { return m_pPad; };
    inline CPadManagerSA*               GetPadManager()                 { return m_pPadManager; };
    inline CTheCarGeneratorsSA*         GetTheCarGenerators()           { return m_pTheCarGenerators; };
    inline CAERadioTrackManagerSA*      GetAERadioTrackManager()        { return m_pCAERadioTrackManager; };
    inline CAudioSA*                    GetAudio()                      { return m_pAudio; };
    inline CMenuManagerSA*              GetMenuManager()                { return m_pMenuManager; };
    inline CTextSA*                     GetText()                       { return m_pText; };
    inline CStatsSA*                    GetStats()                      { return m_pStats; };
    inline CFontSA*                     GetFont()                       { return m_pFont; };
    inline CPathFindSA*                 GetPathFind()                   { return m_pPathFind; };
    inline CPopulationSA*               GetPopulation()                 { return m_pPopulation; };
    inline CTaskManagementSystemSA*     GetTaskManagementSystem()       { return m_pTaskManagementSystem; };
    inline CTasksSA*                    GetTasks()                      { return m_pTasks; };
    inline CSettingsSA*                 GetSettings()                   { return m_pSettings; };
    inline CCarEnterExitSA*             GetCarEnterExit()               { return m_pCarEnterExit; };
    inline CControllerConfigManagerSA*  GetControllerConfigManager()    { return m_pControllerConfigManager; };
    inline CRenderWareSA*               GetRenderWare()                 { return m_pRenderWare; };
    inline CRwExtensionManagerSA*       GetRwExtensionManager()         { return m_pRwExtensionManager; };
    inline CModelManagerSA*             GetModelManager()               { return m_pModelManager; }
    inline CTextureManagerSA*           GetTextureManager()             { return m_pTextureManager; };
    inline CHandlingManagerSA*          GetHandlingManager()            { return m_pHandlingManager; };
    inline CAnimManagerSA*              GetAnimManager()                { return m_pAnimManager; }
    inline CRecordingsSA*               GetRecordings()                 { return m_recordings; }
    inline CStreamingSA*                GetStreaming()                  { return m_pStreaming; }
    inline CVisibilityPluginsSA*        GetVisibilityPlugins()          { return m_pVisibilityPlugins; }
    inline CKeyGenSA*                   GetKeyGen()                     { return m_pKeyGen; }
    inline CRopesSA*                    GetRopes()                      { return m_pRopes; }
    inline CParticleSystemSA*           GetParticleSystem()             { return m_pParticleSystem; }
    inline CFxSA*                       GetFx()                         { return m_pFx; }
    inline CWaterManagerSA*             GetWaterManager()               { return m_pWaterManager; }
    inline NativeExecutive::CExecutiveManager*  GetExecutiveManager()   { return m_executiveManager; }

    CWeaponInfoSA*          GetWeaponInfo( eWeaponType weapon, eWeaponSkill skill = WEAPONSKILL_STD );
    CModelInfoSA*           GetModelInfo( modelId_t id );

    inline unsigned long    GetSystemTime()                     { return *VAR_SystemTime; };
    inline bool             IsAtMenu()                          { return *VAR_IsAtMenu == 1; };
    inline bool             IsGameLoaded()                      { return *VAR_IsGameLoaded == 1; };
    void                    StartGame();
    void                    SetSystemState( eSystemState State );
    eSystemState            GetSystemState();
    inline bool             IsNastyGame()                       { return *VAR_IsNastyGame; };
    inline void             SetNastyGame( bool IsNasty )        { *VAR_IsNastyGame = IsNasty; };
    void                    Pause( bool bPaused );
    bool                    IsPaused();
    bool                    IsInForeground();
    void                    DisableRenderer( bool bDisabled );
    void                    TakeScreenshot( char * szFileName );

    void                    SetRenderHook( InRenderer* pInRenderer );

    void                    Initialize();
    void                    Reset();
    void                    Shutdown();
    void                    OnPreFrame();
    void                    OnFrame();

    void                    SetEntityReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef );

    void                    SetEntityPreRenderCallback( gameEntityPreRenderCallback_t callback );
    void                    SetEntityRenderCallback( gameEntityRenderCallback_t callback );
    void                    SetEntityRenderUnderwaterCallback( gameEntityRenderUnderwaterCallback_t callback );
    void                    SetEntityRenderPostProcessCallback( gameEntityPostProcessCallback_t callback );

    entityList_t            GetEntitiesInRenderQueue( void );

    void                    ResetShaders();

    eGameVersion            GetGameVersion();
    eGameVersion            FindGameVersion();

    float                   GetFPS();
    float                   GetTimeStep();
    float                   GetOldTimeStep();
    float                   GetTimeScale();
    void                    SetTimeScale( float fTimeScale );

    bool                    InitLocalPlayer();

    float                   GetGravity();
    void                    SetGravity( float fGravity );

    float                   GetGameSpeed();
    void                    SetGameSpeed( float fSpeed );

    unsigned char           GetBlurLevel();
    void                    SetBlurLevel( unsigned char ucLevel );

    unsigned long           GetMinuteDuration();
    void                    SetMinuteDuration( unsigned long ulTime );

    void                    HideRadar( bool hide );
    bool                    IsRadarHidden( void );

    void                    SetCenterOfWorld( CEntity *streamingEntity, const CVector *pos, float heading );

    bool                    IsCheatEnabled( const char* szCheatName );
    bool                    SetCheatEnabled( const char* szCheatName, bool bEnable );
    void                    ResetCheats();

    bool                    VerifySADataFileNames ();
    bool                    PerformChecks();
    int&                    GetCheckStatus()                    { return m_iCheckStatus; }

    bool                    ProcessCollisionHandler( CEntitySAInterface *caller, CEntitySAInterface *colld );

    void                    AddAnimationHandler( RpClump *clump, AssocGroupId animGroup, AnimationId animID );
    void                    BlendAnimationHandler( RpClump *clump, AssocGroupId animGroup, AnimationId animID, float blendDelta );

    void                    SetAsyncLoadingFromSettings( bool bSettingsDontUse, bool bSettingsEnabled );
    void                    SetAsyncLoadingFromScript( bool bScriptEnabled, bool bScriptForced );
    void                    SuspendASyncLoading( bool bSuspend );
    bool                    IsASyncLoadingEnabled( bool bIgnoreSuspend = false );

    bool                    HasCreditScreenFadedOut();

    void                    SetupSpecialCharacters();

    void                    FlushPendingRestreamIPL();
    void                    DisableVSync();

private:
    CPoolsSA*                   m_pPools;
    CPlayerInfoSA*              m_pPlayerInfo;
    CProjectileInfoSA*          m_pProjectileInfo;
    CRadarSA*                   m_pRadar;
    CRestartSA*                 m_pRestart;
    CClockSA*                   m_pClock;
    CCoronasSA*                 m_pCoronas;
    CCheckpointsSA*             m_pCheckpoints;
    CEventListSA*               m_pEventList;
    CFireManagerSA*             m_pFireManager;
    CGaragesSA*                 m_pGarages;
    CHudSA*                     m_pHud;
    CWantedSA*                  m_pWanted;
    CWeatherSA*                 m_pWeather;
    CWorldSA*                   m_pWorld;
    CCameraSA*                  m_pCamera;
    CModelInfoSA*               m_pModelInfo; 
    CPickupsSA*                 m_pPickups;
    CWeaponInfoSA*              m_pWeaponInfo;
    CExplosionManagerSA*        m_pExplosionManager;
    C3DMarkersSA*               m_p3DMarkers;
    CRenderWareSA*              m_pRenderWare;
    CRwExtensionManagerSA*      m_pRwExtensionManager;
    CModelManagerSA*            m_pModelManager;
    CTextureManagerSA*          m_pTextureManager;
    CHandlingManagerSA*         m_pHandlingManager;
    CAnimManagerSA*             m_pAnimManager;
    CRecordingsSA*              m_recordings;
    CStreamingSA*               m_pStreaming;
    CVisibilityPluginsSA*       m_pVisibilityPlugins;
    CKeyGenSA*                  m_pKeyGen;
    CRopesSA*                   m_pRopes;
    CParticleSystemSA*          m_pParticleSystem;
    CFxSA*                      m_pFx;
    CWaterManagerSA*            m_pWaterManager;
    NativeExecutive::CExecutiveManager* m_executiveManager;

    CPadSA*                     m_pPad;
    CPadManagerSA*              m_pPadManager;
    CTheCarGeneratorsSA*        m_pTheCarGenerators;
    CAERadioTrackManagerSA*     m_pCAERadioTrackManager;
    CAudioSA*                   m_pAudio;
    CMenuManagerSA*             m_pMenuManager;
    CTextSA*                    m_pText;
    CStatsSA*                   m_pStats;
    CFontSA*                    m_pFont;
    CPathFindSA*                m_pPathFind;
    CPopulationSA*              m_pPopulation;
    CTaskManagementSystemSA*    m_pTaskManagementSystem;
    CTasksSA*                   m_pTasks;
    CSettingsSA*                m_pSettings;
    CCarEnterExitSA*            m_pCarEnterExit;
    CControllerConfigManagerSA* m_pControllerConfigManager;

    // Cache for speeding up collision processing
    typedef std::map <CEntitySAInterface*, CEntitySA*> cachedColl_t;

    bool                        m_didCacheColl;
    cachedColl_t                m_cachedColl;

public:
    typedef std::map <CEntitySA*, bool> disabledColl_t; 
    disabledColl_t              m_disabledColl;

private:

    eGameVersion            m_eGameVersion;
    bool                    m_bAsyncSettingsDontUse;
    bool                    m_bAsyncSettingsEnabled;
    bool                    m_bAsyncScriptEnabled;
    bool                    m_bAsyncScriptForced;
    bool                    m_bASyncLoadingSuspended;
    int                     m_iCheckStatus;

    static unsigned long*   VAR_SystemTime;
    static unsigned long*   VAR_IsAtMenu;
    static unsigned long*   VAR_IsGameLoaded;
    static bool*            VAR_GamePaused;
    static bool*            VAR_IsForegroundWindow;;
    static unsigned long*   VAR_SystemState;
    static void*            VAR_StartGame;
    static bool*            VAR_IsNastyGame;
    static float*           VAR_TimeScale;
    static float*           VAR_FPS;
    static float*           VAR_OldTimeStep;
    static float*           VAR_TimeStep;
    static unsigned long*   VAR_Framelimiter;

    std::map <std::string, SCheatSA*> m_Cheats;
};

extern CFileTranslator *gameFileRoot;

// Utility functions
CFile*  OpenGlobalStream( const char *filename, const char *mode );

float __cdecl NormalizeRadians( float radians );

#endif //__CGAMESA
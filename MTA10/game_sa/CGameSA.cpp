/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CGameSA.cpp
*  PURPOSE:     Base game logic handling
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

#include "StdInc.h"
#include "SharedUtil.hpp"

unsigned long* CGameSA::VAR_SystemTime;
unsigned long* CGameSA::VAR_IsAtMenu;
unsigned long* CGameSA::VAR_IsGameLoaded;
bool* CGameSA::VAR_GamePaused;
bool* CGameSA::VAR_IsForegroundWindow;
unsigned long* CGameSA::VAR_SystemState;
void* CGameSA::VAR_StartGame;
bool* CGameSA::VAR_IsNastyGame;
float* CGameSA::VAR_TimeScale;
float* CGameSA::VAR_FPS;
float* CGameSA::VAR_OldTimeStep;
float* CGameSA::VAR_TimeStep;
unsigned long* CGameSA::VAR_Framelimiter;

static CMultiplayer* multiplayer;

CFileTranslator *gameFileRoot;

static bool ProcessCollisions( CEntitySAInterface *caller, CEntitySAInterface *colld )
{
    return pGame->ProcessCollisionHandler( caller, colld );
}

static void AddAnimation( RpClump *clump, AssocGroupId animGroup, AnimationId animId )
{
    pGame->AddAnimationHandler( clump, animGroup, animId );
}

static void BlendAnimation( RpClump *clump, AssocGroupId animGroup, AnimationId animId, float blendDelta )
{
    pGame->BlendAnimationHandler( clump, animGroup, animId, blendDelta );
}

CGameSA::CGameSA( void )
{
    pGame = this;
    
    // Setup the global game file root, which has to be our current directory
    gameFileRoot = core->GetFileSystem()->CreateTranslator( "/" );

    assert( gameFileRoot != NULL );
    
    m_bAsyncSettingsDontUse = false;
    m_bAsyncSettingsEnabled = false;
    m_bAsyncScriptEnabled = false;
    m_bAsyncScriptForced = false;
    m_bASyncLoadingSuspended = false;
    m_iCheckStatus = 0;

    // Unprotect all of the GTASA code at once and leave it that way
    DWORD oldProt;
    VirtualProtect((LPVOID)0x401000, 0x4A3000, PAGE_EXECUTE_READWRITE, &oldProt);

    // Initialize the offsets
    eGameVersion version = FindGameVersion ();
    switch ( version )
    {
    case VERSION_EU_10: COffsets::Initialize10EU (); break;
    case VERSION_US_10: COffsets::Initialize10US (); break;
    case VERSION_11:    COffsets::Initialize11 (); break;
    case VERSION_20:    COffsets::Initialize20 (); break;
    }

    // Set the model ids for all the CModelInfoSA instances
    for ( unsigned int i = 0; i < MAX_RESOURCES; i++ )
    {
        ModelInfo [i].SetModelID ( i );
    }

    // Monitor GTA:SA file system activity.
    FileMgr::InitHooks();

    IMG_Initialize();

    DEBUG_TRACE("CGameSA::CGameSA()");
    m_executiveManager          = NativeExecutive::CExecutiveManager::Create();
    m_pStreaming                = new CStreamingSA;
    m_pAudio                    = new CAudioSA();
    m_pWorld                    = new CWorldSA();
    m_pPools                    = new CPoolsSA();
    m_pClock                    = new CClockSA();
    m_pRadar                    = new CRadarSA();
    m_pCamera                   = new CCameraSA((CCameraSAInterface *)CLASS_CCamera);
    m_pCoronas                  = new CCoronasSA();
    m_pCheckpoints              = new CCheckpointsSA();
    m_pPickups                  = new CPickupsSA();
    m_pExplosionManager         = new CExplosionManagerSA();
    m_pHud                      = new CHudSA();
    m_pFireManager              = new CFireManagerSA();
    m_p3DMarkers                = new C3DMarkersSA();
    m_pPad                      = new CPadSA((CPadSAInterface *)CLASS_CPad);
    m_pPadManager               = new CPadManagerSA();
    m_pTheCarGenerators         = new CTheCarGeneratorsSA();
    m_pCAERadioTrackManager     = new CAERadioTrackManagerSA();
    m_pWeather                  = new CWeatherSA();
    m_pMenuManager              = new CMenuManagerSA();
    m_pText                     = new CTextSA();
    m_pStats                    = new CStatsSA();
    m_pFont                     = new CFontSA();
    m_pPathFind                 = new CPathFindSA();
    m_pPopulation               = new CPopulationSA();
    m_pTaskManagementSystem     = new CTaskManagementSystemSA();
    m_pSettings                 = new CSettingsSA();
    m_pCarEnterExit             = new CCarEnterExitSA();
    m_pControllerConfigManager  = new CControllerConfigManagerSA();
    m_pProjectileInfo           = new CProjectileInfoSA();
    m_pRenderWare               = new CRenderWareSA( version );
    m_pRwExtensionManager       = new CRwExtensionManagerSA();
    m_pModelManager             = new CModelManagerSA();
    m_pTextureManager           = new CTextureManagerSA();
    m_pHandlingManager          = new CHandlingManagerSA ();
    m_pEventList                = new CEventListSA();
    m_pGarages                  = new CGaragesSA( (CGaragesSAInterface *)CLASS_CGarages );
    m_pTasks                    = new CTasksSA( (CTaskManagementSystemSA*)m_pTaskManagementSystem );
    m_pAnimManager              = new CAnimManagerSA;
    m_recordings                = new CRecordingsSA;
    m_pVisibilityPlugins        = new CVisibilityPluginsSA;
    m_pKeyGen                   = new CKeyGenSA;
    m_pRopes                    = new CRopesSA;
    m_pParticleSystem           = new CParticleSystemSA();
    m_pFx                       = new CFxSA( (CFxSAInterface*)CLASS_CFx );
    m_pWaterManager             = new CWaterManagerSA();

    // :D
    RenderWareAPI_Init();
    RenderWarePipeline_Init();
    Transformation_Init();
    QuadTree_Init();
    Cache_Init();
    Placeable_Init();
    Camera_Init();
    ColModel_Init();
    Entity_Init();
    Physical_Init();
    Objects_Init();
    Ped_Init();
    ObjectRender_Init();
    CarGroups_Init();
    Streamer_Init();
    ModelInfo_Init();
    VehicleModels_Init();
    VehicleRender_Init();
    PlayerInfo_Init();
    HUD_Init();

    // Normal weapon types (WEAPONSKILL_STD)
    for ( int i = 0; i < NUM_WeaponInfosStdSkill; i++)
        WeaponInfos[i] = new CWeaponInfoSA((CWeaponInfoSAInterface *)(ARRAY_WeaponInfo + i*CLASSSIZE_WeaponInfo), (eWeaponType)(WEAPONTYPE_PISTOL + i));

    // Extra weapon types for skills (WEAPONSKILL_POOR,WEAPONSKILL_PRO,WEAPONSKILL_SPECIAL)
    int index;
    for ( int skill = 0; skill < 3 ; skill++ )
    {
        for ( int i = 0; i < NUM_WeaponInfosOtherSkill; i++ )
        {
            index = NUM_WeaponInfosStdSkill + skill*NUM_WeaponInfosOtherSkill + i;
            WeaponInfos[index] = new CWeaponInfoSA((CWeaponInfoSAInterface *)(ARRAY_WeaponInfo + index*CLASSSIZE_WeaponInfo), (eWeaponType)(WEAPONTYPE_PISTOL + i));
        }
    }

    m_pPlayerInfo = new CPlayerInfoSA ( (CPlayerInfoSAInterface *)CLASS_CPlayerInfo );

    // Init cheat name => address map
    m_Cheats [ CHEAT_HOVERINGCARS     ] = new SCheatSA((BYTE *)VAR_HoveringCarsEnabled);
    m_Cheats [ CHEAT_FLYINGCARS       ] = new SCheatSA((BYTE *)VAR_FlyingCarsEnabled);
    m_Cheats [ CHEAT_EXTRABUNNYHOP    ] = new SCheatSA((BYTE *)VAR_ExtraBunnyhopEnabled);
    m_Cheats [ CHEAT_EXTRAJUMP        ] = new SCheatSA((BYTE *)VAR_ExtraJumpEnabled);

    // New cheats for Anticheat
    m_Cheats [ CHEAT_TANKMODE         ] = new SCheatSA((BYTE *)VAR_TankModeEnabled, false);
    m_Cheats [ CHEAT_NORELOAD         ] = new SCheatSA((BYTE *)VAR_NoReloadEnabled, false);
    m_Cheats [ CHEAT_PERFECTHANDLING  ] = new SCheatSA((BYTE *)VAR_PerfectHandling, false);
    m_Cheats [ CHEAT_ALLCARSHAVENITRO ] = new SCheatSA((BYTE *)VAR_AllCarsHaveNitro, false);
    m_Cheats [ CHEAT_BOATSCANFLY      ] = new SCheatSA((BYTE *)VAR_BoatsCanFly, false);
    m_Cheats [ CHEAT_INFINITEOXYGEN   ] = new SCheatSA((BYTE *)VAR_InfiniteOxygen, false);
    m_Cheats [ CHEAT_WALKUNDERWATER   ] = new SCheatSA((BYTE *)VAR_WalkUnderwater, false);
    m_Cheats [ CHEAT_FASTERCLOCK      ] = new SCheatSA((BYTE *)VAR_FasterClock, false);
    m_Cheats [ CHEAT_FASTERGAMEPLAY   ] = new SCheatSA((BYTE *)VAR_FasterGameplay, false);
    m_Cheats [ CHEAT_SLOWERGAMEPLAY   ] = new SCheatSA((BYTE *)VAR_SlowerGameplay, false);
    m_Cheats [ CHEAT_ALWAYSMIDNIGHT   ] = new SCheatSA((BYTE *)VAR_AlwaysMidnight, false);
    m_Cheats [ CHEAT_FULLWEAPONAIMING ] = new SCheatSA((BYTE *)VAR_FullWeaponAiming, false);
    m_Cheats [ CHEAT_INFINITEHEALTH   ] = new SCheatSA((BYTE *)VAR_InfiniteHealth, false);
    m_Cheats [ CHEAT_NEVERWANTED      ] = new SCheatSA((BYTE *)VAR_NeverWanted, false);
    m_Cheats [ CHEAT_HEALTARMORMONEY  ] = new SCheatSA((BYTE *)VAR_HealthArmorMoney, false);
}

CGameSA::~CGameSA( void )
{
    // Destroy the player
    delete m_pPlayerInfo;
    
    for ( int i = 0; i < NUM_WeaponInfosTotal; i++ )
    {
        delete reinterpret_cast < CWeaponInfoSA* > ( WeaponInfos [i] );
    }

    Shutdown();

    HUD_Shutdown();
    PlayerInfo_Shutdown();
    VehicleRender_Shutdown();
    VehicleModels_Shutdown();
    ModelInfo_Shutdown();
    Streamer_Shutdown();
    CarGroups_Shutdown();
    ObjectRender_Shutdown();
    Ped_Shutdown();
    Objects_Shutdown();
    Physical_Shutdown();
    Entity_Shutdown();
    ColModel_Shutdown();
    Camera_Shutdown();
    Placeable_Shutdown();
    Cache_Shutdown();
    QuadTree_Shutdown();
    Transformation_Shutdown();
    RenderWarePipeline_Shutdown();
    RenderWareAPI_Shutdown();

    delete m_pPools;    // has to be first to delete using entities
    delete m_pFx;
    delete m_pParticleSystem;
    delete m_pRopes;
    delete m_pKeyGen;
    delete m_pVisibilityPlugins;
    delete m_pAnimManager;
    delete m_recordings;
    delete m_pTasks;
    delete m_pTextureManager;
    delete m_pModelManager;
    delete m_pRenderWare;
    delete m_pTaskManagementSystem;
    delete m_pHandlingManager;
    delete m_pPopulation;
    delete m_pPathFind;
    delete m_pFont;
    delete m_pStats;
    delete m_pText;
    delete m_pMenuManager;
    delete m_pWeather;
    delete m_pCAERadioTrackManager;
    delete m_pTheCarGenerators;
    delete m_pPadManager;
    delete m_pPad;
    delete m_p3DMarkers;
    delete m_pFireManager;
    delete m_pHud;
    delete m_pExplosionManager;
    delete m_pPickups;
    delete m_pCheckpoints;
    delete m_pCoronas;
    delete m_pCamera;
    delete m_pRadar;
    delete m_pClock;
    delete m_pWorld;
    delete m_pAudio;
    delete m_pStreaming;
    NativeExecutive::CExecutiveManager::Delete( m_executiveManager );

    IMG_Shutdown();

    // Shutdown file monitoring.
    FileMgr::ShutdownHooks();

    // Dump any memory leaks if DETECT_LEAKS is defined
#ifdef DETECT_LEAKS    
    DumpUnfreed();
#endif
}

void CGameSA::RegisterMultiplayer( CMultiplayer *mp )
{
    // Extend multiplayer with internal extensions
    multiplayer = mp;
    multiplayer->SetProcessCollisionHandler( ProcessCollisions );
}

void CGameSA::UnregisterMultiplayer( CMultiplayer *mp )
{
    assert( mp == multiplayer );

    multiplayer->SetProcessCollisionHandler( NULL );
}

CWeaponInfoSA* CGameSA::GetWeaponInfo( eWeaponType weapon, eWeaponSkill skill )
{ 
    DEBUG_TRACE("CWeaponInfoSA* CGameSA::GetWeaponInfo( eWeaponType weapon, eWeaponSkill skill )");
    
    if ( (skill == WEAPONSKILL_STD && weapon >= WEAPONTYPE_UNARMED && weapon < WEAPONTYPE_LAST_WEAPONTYPE) ||
         (skill != WEAPONSKILL_STD && weapon >= WEAPONTYPE_PISTOL && weapon <= WEAPONTYPE_TEC9) )
    {
        int offset = 0;

        switch ( skill )
        {
        case WEAPONSKILL_STD:
            offset = 0;
            break;
        case WEAPONSKILL_POOR:
            offset = NUM_WeaponInfosStdSkill - WEAPONTYPE_PISTOL;
            break;
        case WEAPONSKILL_PRO:
            offset = NUM_WeaponInfosStdSkill + NUM_WeaponInfosOtherSkill - WEAPONTYPE_PISTOL;
            break;
        case WEAPONSKILL_SPECIAL:
            offset = NUM_WeaponInfosStdSkill + 2*NUM_WeaponInfosOtherSkill - WEAPONTYPE_PISTOL;
            break;
        default:
            break;
        }
        return WeaponInfos[offset + weapon]; 
    }
    return NULL; 
}

void CGameSA::Pause( bool bPaused )
{
    *VAR_GamePaused = bPaused;
}

bool CGameSA::IsPaused()
{
    return *VAR_GamePaused;
}

bool CGameSA::IsInForeground()
{
    return *VAR_IsForegroundWindow;
}

CModelInfoSA* CGameSA::GetModelInfo( modelId_t model )
{ 
    DEBUG_TRACE("CModelInfo* CGameSA::GetModelInfo( modelId_t model )");

    if ( model > MAX_MODELS - 1 )
        return NULL;

    if ( !ModelInfo[model].IsValid() )
        return NULL;

    return &ModelInfo[model];
}

/**
 * Starts the game
 * \todo make addresses into constants
 */
void CGameSA::StartGame()
{
    DEBUG_TRACE("void CGameSA::StartGame()");

    SetSystemState( GS_INIT_PLAYING_GAME );
    *(unsigned char*)0xB7CB49 = 0;
    *(unsigned char*)0xBA67A4 = 0;
}

/**
 * Sets the part of the game loading process the game is in.
 * @param dwState DWORD containing a valid state 0 - 9
 */
void CGameSA::SetSystemState( eSystemState State )
{
    DEBUG_TRACE("void CGameSA::SetSystemState( eSystemState State )");
    *VAR_SystemState = (DWORD)State;
}

eSystemState CGameSA::GetSystemState()
{
    DEBUG_TRACE("eSystemState CGameSA::GetSystemState()");
    return (eSystemState)*VAR_SystemState;
}

bool CGameSA::InitLocalPlayer()
{
    DEBUG_TRACE("bool CGameSA::InitLocalPlayer()");

    if ( mtaPeds[0] )
        return true;

    new CPlayerPedSA( (CPlayerPedSAInterface*)(*ppPedPool)->Get( 0 ), 0, true );

    return true;
}

float CGameSA::GetGravity()
{
    return *(float*)0x863984;
}

void CGameSA::SetGravity( float fGravity )
{
   *(float*)0x863984 = fGravity;
}

float CGameSA::GetGameSpeed()
{
    return *(float*)0xB7CB64;
}

void CGameSA::SetGameSpeed( float fSpeed )
{
    *(float*)0xB7CB64 = fSpeed;
}

// this prevents some crashes (respawning mainly)
void CGameSA::DisableRenderer( bool bDisabled )
{
    // ENABLED:
    // 0053DF40   D915 2C13C800    FST DWORD PTR DS:[C8132C]
    // DISABLED:
    // 0053DF40   C3               RETN

    if ( bDisabled )
        *(unsigned char*)0x53DF40 = 0xC3;
    else
        *(unsigned char*)0x53DF40 = 0xD9;
}

void CGameSA::SetRenderHook( InRenderer* pInRenderer )
{
    if ( pInRenderer )
        HookInstall ( (DWORD)FUNC_CDebug_DebugDisplayTextBuffer, (DWORD)pInRenderer, 6 );
    else
        *(unsigned char*)FUNC_CDebug_DebugDisplayTextBuffer = 0xC3;
}

void CGameSA::TakeScreenshot( char *szFileName )
{
    DWORD dwFunc = FUNC_JPegCompressScreenToFile;
    _asm
    {
        mov     eax, CLASS_RwCamera
        mov     eax, [eax]
        push    szFileName
        push    eax
        call    dwFunc
        add     esp,8
    }
}

void CGameSA::Reset( void )
{
    // Things to do if the game was loaded
    if ( GetSystemState() == GS_PLAYING_GAME )
    {
        // Extinguish all fires
        m_pFireManager->ExtinguishAllFires();

        // Restore camera stuff
        m_pCamera->Restore();
        m_pCamera->SetFadeColor( 0, 0, 0 );
        m_pCamera->Fade( 0, FADE_OUT );

        Pause( false );        // We don't have to pause as the fadeout will stop the sound. Pausing it will make the fadein next start ugly
        m_pHud->Disable( false );

        DisableRenderer( false );

        // Restore the HUD
        m_pHud->Disable( false );
        m_pHud->DisableAll( false );
    }

    // Notify modules which want it.
    HUD_OnReset();

    // Reset ubiqitous managers.
    Streaming::Reset();

    // Reset rendering systems.
    RenderCallbacks_Reset();
    RenderWareLighting_Reset();
}

void CGameSA::Initialize()
{
    // Initialize garages
    m_pGarages->Initialize();
    SetupSpecialCharacters();

    // *Sebas* Hide the GTA:SA Main menu.
    MemPutFast < BYTE > ( CLASS_CMenuManager+0x5C, 0 );

    VehicleModelInfoRender_SetupDevice();
    RenderWareLighting_InitShaders();
}

void CGameSA::Shutdown()
{
}

void CGameSA::OnPreFrame()
{
    m_didCacheColl = false;

    // Update the executive manager.
    m_executiveManager->DoPulse();

    switch( GetSystemState() )
    {
    case GS_PLAYING_GAME:
        // Update control with current pad
        //m_pPad->SetState( m_pPad->GetState() );
        break;
    }
}

void CGameSA::OnFrame()
{
    switch( GetSystemState() )
    {
    case GS_PLAYING_GAME:
        if ( CPlayerPedSA *player = m_pPlayerInfo->GetPlayerPed() )
        {
            // Update local player's control
            GetPadManager()->UpdateLocalJoypad( *player );

            // Frame the player's activity
            player->OnFrame();
        }

        // Pulse the peds
        for ( unsigned int n = 0; n < MAX_PEDS; n++ )
        {
            CPedSAInterface *ped = (*ppPedPool)->Get( n );

            if ( !ped )
                continue;

            ped->OnFrame();
        }

        break;
    }

    core->GetKeyBinds()->DoPostGameFramePulse();
}

void CGameSA::SetEntityReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef )
{
    Entity::SetReferenceCallbacks( addRef, delRef );
}

void CGameSA::SetEntityPreRenderCallback( gameEntityPreRenderCallback_t callback )
{
    Entity::SetPreRenderCallback( callback );
}

void CGameSA::SetEntityRenderCallback( gameEntityRenderCallback_t callback )
{
    Entity::SetRenderCallback( callback );
}

void CGameSA::SetEntityRenderUnderwaterCallback( gameEntityRenderCallback_t callback )
{
    Entity::SetRenderUnderwaterCallback( callback );
}

void CGameSA::SetEntityRenderPostProcessCallback( gameEntityPostProcessCallback_t callback )
{
    Entity::SetRenderPostProcessCallback( callback );
}

CGame::entityList_t CGameSA::GetEntitiesInRenderQueue( void )
{
    return Entity::GetEntitiesInRenderQueue();
}

void CGameSA::ResetShaders()
{
    VehicleModelInfoRender_Reset();
    RenderWareLighting_ResetShaders();
}

eGameVersion CGameSA::GetGameVersion()
{
    return m_eGameVersion;
}

eGameVersion CGameSA::FindGameVersion()
{
    unsigned char ucA = *(unsigned char*)0x748ADD;
    unsigned char ucB = *(unsigned char*)0x748ADE;

    if ( ucA == 0xFF && ucB == 0x53 )
        m_eGameVersion = VERSION_US_10;
    else if ( ucA == 0x0F && ucB == 0x84 )
        m_eGameVersion = VERSION_EU_10;
    else if ( ucA == 0xFE && ucB == 0x10 )
        m_eGameVersion = VERSION_11;
    else
        m_eGameVersion = VERSION_UNKNOWN;

    return m_eGameVersion;
}

float CGameSA::GetFPS()
{
    return *VAR_FPS;
}

float CGameSA::GetTimeStep()
{
    return *VAR_TimeStep;
}

float CGameSA::GetOldTimeStep()
{
    return *VAR_OldTimeStep;
}

float CGameSA::GetTimeScale()
{
    return *VAR_TimeScale;
}

void CGameSA::SetTimeScale( float fTimeScale )
{
    *VAR_TimeScale = fTimeScale;
}

unsigned char CGameSA::GetBlurLevel()
{
    return *(unsigned char*)0x8D5104;
}

void CGameSA::SetBlurLevel( unsigned char ucLevel )
{
    *(unsigned char*)0x8D5104 = ucLevel;
}

unsigned long CGameSA::GetMinuteDuration()
{
    return *(unsigned long*)0xB7015C;
}

void CGameSA::SetMinuteDuration( unsigned long ulTime )
{
    *(unsigned long*)0xB7015C = ulTime;
}

void CGameSA::HideRadar( bool hide )
{
    return HUD::HideRadar( hide );
}

bool CGameSA::IsRadarHidden( void )
{
    return HUD::IsRadarHidden();
}

void CGameSA::SetCenterOfWorld( CEntity *streamingEntity, const CVector *pos, float heading )
{
    CEntitySA *entity = dynamic_cast <CEntitySA*> ( streamingEntity );

    World::SetCenterOfWorld( entity ? entity->GetInterface() : NULL, pos, heading );
}

bool CGameSA::IsCheatEnabled( const char* szCheatName )
{
    std::map <std::string, SCheatSA*>::iterator it = m_Cheats.find ( szCheatName );

    if ( it == m_Cheats.end () )
        return false;

    return *(it->second->m_byAddress) != 0;
}

bool CGameSA::SetCheatEnabled( const char* szCheatName, bool bEnable )
{
    std::map < std::string, SCheatSA* >::iterator it = m_Cheats.find ( szCheatName );

    if ( it == m_Cheats.end () )
        return false;
    if ( !it->second->m_bCanBeSet )
        return false;

    MemPutFast < BYTE > ( it->second->m_byAddress, bEnable );
    it->second->m_bEnabled = bEnable;
    return true;
}

void CGameSA::ResetCheats()
{
    std::map < std::string, SCheatSA* >::iterator it;
    for ( it = m_Cheats.begin (); it != m_Cheats.end (); it++ ) {
        if ( it->second->m_byAddress > (BYTE*)0x8A4000 )
            MemPutFast < BYTE > ( it->second->m_byAddress, 0 );
        else
            MemPut < BYTE > ( it->second->m_byAddress, 0 );
        it->second->m_bEnabled = false;
    }
}
bool CGameSA::PerformChecks()
{
    std::map < std::string, SCheatSA* >::iterator it;
    for ( it = m_Cheats.begin (); it != m_Cheats.end (); it++ ) {
        if (*(it->second->m_byAddress) != BYTE(it->second->m_bEnabled))
            return false;
    }
    return true;
}

bool CGameSA::ProcessCollisionHandler( CEntitySAInterface *caller, CEntitySAInterface *colld )
{
    if ( caller == colld )
        return true;

    if ( !m_didCacheColl )
    {
        // Build a map of CPhysicalSAInterface*/CClientEntity*'s that have collision disabled
        m_didCacheColl = true;
        m_cachedColl.clear();

        disabledColl_t::iterator iter = m_disabledColl.begin();

        for ( ; iter != m_disabledColl.end(); ++iter )
            m_cachedColl[iter->first->GetInterface()] = iter->first;
    }

    // Check both elements appear in the cached map before doing extra processing
    cachedColl_t::iterator iter1 = m_cachedColl.find( caller );

    if ( iter1 == m_cachedColl.end() )
        return true;

    cachedColl_t::iterator iter2 = m_cachedColl.find( colld );

    if ( iter2 == m_cachedColl.end() )
        return true;

    if ( !iter1->second->IsCollidableWith( iter2->second ) )
        return false;

    return true;
}

void CGameSA::AddAnimationHandler( RpClump *clump, AssocGroupId animGroup, AnimationId animID )
{

}

void CGameSA::BlendAnimationHandler( RpClump *clump, AssocGroupId animGroup, AnimationId animId, float blendDelta )
{

}

bool CGameSA::VerifySADataFileNames()
{
    return !strcmp( *(char**)0x5B65AE, "DATA\\CARMODS.DAT" ) &&
           !strcmp( *(char**)0x5BD839, "DATA" ) &&
           !strcmp( *(char**)0x5BD84C, "HANDLING.CFG" ) &&
           !strcmp( *(char**)0x5BEEE8, "DATA\\melee.dat" ) &&
           !strcmp( *(char**)0x5B925B, "DATA\\OBJECT.DAT" ) &&
           !strcmp( *(char**)0x55D0FC, "data\\surface.dat" ) &&
           !strcmp( *(char**)0x55F2BB, "data\\surfaud.dat" ) &&
           !strcmp( *(char**)0x55EB9E, "data\\surfinfo.dat" ) &&
           !strcmp( *(char**)0x6EAEF8, "DATA\\water.dat" ) &&
           !strcmp( *(char**)0x6EAEC3, "DATA\\water1.dat" ) &&
           !strcmp( *(char**)0x5BE686, "DATA\\WEAPON.DAT" );
}

void CGameSA::SetAsyncLoadingFromSettings ( bool bSettingsDontUse, bool bSettingsEnabled )
{
    m_bAsyncSettingsDontUse = bSettingsDontUse;
    m_bAsyncSettingsEnabled = bSettingsEnabled;
}

void CGameSA::SetAsyncLoadingFromScript ( bool bScriptEnabled, bool bScriptForced )
{
    m_bAsyncScriptEnabled = bScriptEnabled;
    m_bAsyncScriptForced = bScriptForced;
}

void CGameSA::SuspendASyncLoading ( bool bSuspend )
{
    m_bASyncLoadingSuspended = bSuspend;
}

bool CGameSA::IsASyncLoadingEnabled ( bool bIgnoreSuspend )
{
    if ( m_bASyncLoadingSuspended && !bIgnoreSuspend )
        return false;

    if ( m_bAsyncScriptForced || m_bAsyncSettingsDontUse )
        return m_bAsyncScriptEnabled;

    return m_bAsyncSettingsEnabled;
}

void CGameSA::SetupSpecialCharacters ( void )
{
    ModelInfo[1].MakePedModel ( "TRUTH" );
    ModelInfo[2].MakePedModel ( "MACCER" );
    //ModelInfo[190].MakePedModel ( "BARBARA" );
    //ModelInfo[191].MakePedModel ( "HELENA" );
    //ModelInfo[192].MakePedModel ( "MICHELLE" );
    //ModelInfo[193].MakePedModel ( "KATIE" );
    //ModelInfo[194].MakePedModel ( "MILLIE" );
    //ModelInfo[195].MakePedModel ( "DENISE" );
    ModelInfo[265].MakePedModel ( "TENPEN" );   
    ModelInfo[266].MakePedModel ( "PULASKI" );
    ModelInfo[267].MakePedModel ( "HERN" );
    ModelInfo[268].MakePedModel ( "DWAYNE" );
    ModelInfo[269].MakePedModel ( "SMOKE" );
    ModelInfo[270].MakePedModel ( "SWEET" );
    ModelInfo[271].MakePedModel ( "RYDER" );
    ModelInfo[272].MakePedModel ( "FORELLI" );
    ModelInfo[290].MakePedModel ( "ROSE" );
    ModelInfo[291].MakePedModel ( "PAUL" );
    ModelInfo[292].MakePedModel ( "CESAR" );
    ModelInfo[293].MakePedModel ( "OGLOC" );
    ModelInfo[294].MakePedModel ( "WUZIMU" );
    ModelInfo[295].MakePedModel ( "TORINO" );
    ModelInfo[296].MakePedModel ( "JIZZY" );
    ModelInfo[297].MakePedModel ( "MADDOGG" );
    ModelInfo[298].MakePedModel ( "CAT" );
    ModelInfo[299].MakePedModel ( "CLAUDE" );
    ModelInfo[300].MakePedModel ( "RYDER2" );
    ModelInfo[301].MakePedModel ( "RYDER3" );
    ModelInfo[302].MakePedModel ( "EMMET" );
    ModelInfo[303].MakePedModel ( "ANDRE" );
    ModelInfo[304].MakePedModel ( "KENDL" );
    ModelInfo[305].MakePedModel ( "JETHRO" );
    ModelInfo[306].MakePedModel ( "ZERO" );
    ModelInfo[307].MakePedModel ( "TBONE" );
    ModelInfo[308].MakePedModel ( "SINDACO" );
    ModelInfo[309].MakePedModel ( "JANITOR" );
    ModelInfo[310].MakePedModel ( "BBTHIN" );
    ModelInfo[311].MakePedModel ( "SMOKEV" );
    ModelInfo[312].MakePedModel ( "PSYCHO" );
    /* Hot-coffee only models
    ModelInfo[313].MakePedModel ( "GANGRL2" );
    ModelInfo[314].MakePedModel ( "MECGRL2" );
    ModelInfo[315].MakePedModel ( "GUNGRL2" );
    ModelInfo[316].MakePedModel ( "COPGRL2" );
    ModelInfo[317].MakePedModel ( "NURGRL2" );
    */
}

// Well, has it?
bool CGameSA::HasCreditScreenFadedOut()
{
    return ( GetSystemState() >= GS_FRONTEND ) && ( *(BYTE*)0xBAB320 < 6 );
}

// Ensure replaced/restored textures for models in the GTA map are correct
void CGameSA::FlushPendingRestreamIPL()
{
    CModelInfoSA::StaticFlushPendingRestreamIPL();
}

// Disable VSync by forcing what normally happends at the end of the loading screens
// Note #1: This causes the D3D device to be reset after the next frame
// Note #2: Some players do not need this to disable VSync. (Possibly because their video card driver settings override it somewhere)
void CGameSA::DisableVSync()
{
   *(unsigned char*)0xBAB318 = 0;
}

/*=========================================================
    OpenGlobalStream

    Arguments:
        filename - MTA filepath descriptor
        mode - ANSI C opening mode ("w", "rb+", "a", ...)
    Purpose:
        Opens a MTA stream handle by dispatching the filename.
        It currently tries to access...
            the mod (mods/deathmatch) root,
            the MTA (MTA San Andreas 1.x/MTA/) root and
            the game (Rockstar Games/GTA San Andreas/)...
        in the order: Files from the deathmatch root will
        override files from the MTA root and the game root.
        Returns the MTA stream handle if successful (filepath valid,
        contextual [points inside one of the roots], not filesystem
        locked).
=========================================================*/
CFile* OpenGlobalStream( const char *filename, const char *mode )
{
    CFile *file;

    // Attempt to access the deathmatch directory
    if ( CFileTranslator *modRoot = core->GetModRoot() )
    {
        if ( file = modRoot->Open( filename, mode ) )
            return file;
    }

    // Attempt to access the MTA directory
    if ( file = core->GetMTARoot()->Open( filename, mode ) )
        return file;

    // Attempt to access the GTA:SA current directory.
    if ( file = FileMgr::GetCurDirTranslator()->Open( filename, mode ) )
        return file;

    // (off-topic) TODO: accept read-only access to the game directory
    // MTA team has voiced their concern about game directory access; TOD (topic of discussion)
    // I see this feature as optional anyway ;)
    return NULL;
}

/*=========================================================
    NormalizeRadians

    Arguments:
        radians - angle value
    Purpose:
        Normalizes the radians to a comparable result and
        returns them.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0053CB50
=========================================================*/
float __cdecl NormalizeRadians( float radians )
{
    radians = Clamp( -25.0f, radians, 25.0f );

    while ( radians >= M_PI )
        radians -= (float)( M_PI * 2 );

    while ( radians < -M_PI )
        radians += (float)( M_PI * 2 );

    return radians;
}
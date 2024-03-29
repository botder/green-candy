/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CClientGame.h
*  PURPOSE:     Header for client game class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Derek Abdine <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTGAME_H
#define __CCLIENTGAME_H

#include "CPacketHandler.h"
#include <CClientCommon.h>
#include <CClientManager.h>
#include "Config.h"
#include "CBlendedWeather.h"
#include "CMovingObjectsManager.h"
#include "CNametags.h"
#include "CNetAPI.h"
#include "CNetworkStats.h"
#include "CSyncDebug.h"
#include "CTransferBox.h"
#include "rpc/CRPCFunctions.h"
#include "CUnoccupiedVehicleSync.h"
#include "CPedSync.h"
#include "CObjectSync.h"
#include "CRadarMap.h"
#include "CClientTeamManager.h"
#include "CClientPedManager.h"
#include "../../shared_logic/CZoneNames.h"
#include "CEvents.h"
#include "CResourceManager.h"
#include "CScriptKeyBinds.h"
#include "CFoo.h"
#include "../../shared_logic/CClientGUIElement.h"
#include "CVariableBuffer.h"
#include "CLocalServer.h"
#include "CVoiceRecorder.h"

class CClientGame
{
    friend class CPacketHandler;

public:
    enum eStatus
    {
        STATUS_CONNECTING,
        STATUS_JOINING,
        STATUS_JOINED,
        STATUS_TRANSFER,
        STATUS_OFFLINE
    };

    enum
    {
        WASTED_DIED,
        WASTED_DROWNED,
        WASTED_KILLED,
    };

    enum
    {
        DUMMY,
        PLAYER,
        VEHICLE,
        OBJECT,
        MARKER,
        BLIP,
        PICKUP,
        RADAR_AREA,
        SPAWNPOINT_DEPRECATED,
        REMOTECLIENT,
        CONSOLE,
        PATH_NODE,
        WORLD_MESH,
        TEAM,
        PED,
        COLSHAPE,
        SCRIPTFILE,
        WATER,
        UNKNOWN,
    };

    enum
    {
        MARKER_CHECKPOINT,
        MARKER_RING,
        MARKER_CYLINDER,
        MARKER_ARROW,
        MARKER_CORONA,
        MARKER_INVALID = 0xFF,
    };

    enum
    {
        PICKUP_HEALTH,
        PICKUP_ARMOR,
        PICKUP_WEAPON,
        PICKUP_CUSTOM,
        PICKUP_LAST,
        PICKUP_INVALID = 0xFF,
    };

    enum
    {
        VEHICLE_REQUEST_IN,
        VEHICLE_NOTIFY_IN,
        VEHICLE_NOTIFY_IN_ABORT,
        VEHICLE_REQUEST_OUT,
        VEHICLE_NOTIFY_OUT,
        VEHICLE_NOTIFY_OUT_ABORT,
        VEHICLE_NOTIFY_JACK,
        VEHICLE_NOTIFY_JACK_ABORT,
        VEHICLE_NOTIFY_FELL_OFF,
    };

    enum
    {
        VEHICLE_REQUEST_IN_CONFIRMED,
        VEHICLE_NOTIFY_IN_RETURN,
        VEHICLE_NOTIFY_IN_ABORT_RETURN,
        VEHICLE_REQUEST_OUT_CONFIRMED,
        VEHICLE_NOTIFY_OUT_RETURN,
        VEHICLE_NOTIFY_OUT_ABORT_RETURN,
        VEHICLE_NOTIFY_FELL_OFF_RETURN,
        VEHICLE_REQUEST_JACK_CONFIRMED,
        VEHICLE_NOTIFY_JACK_RETURN,
        VEHICLE_ATTEMPT_FAILED,
    };

    enum eQuitReason
    {
        QUIT_QUIT,
        QUIT_KICK,
        QUIT_BAN,
        QUIT_CONNECTION_DESYNC,
        QUIT_TIMEOUT,
    };   
    enum 
    {
        GLITCH_QUICKRELOAD,
        GLITCH_FASTFIRE,
        GLITCH_FASTMOVE,
        GLITCH_CROUCHBUG,
        NUM_GLITCHES
    };
    class CStoredWeaponSlot
    {
    public:
        inline          CStoredWeaponSlot   ( void ) { ucType = 0; usAmmo = 0; usAmmoInClip = 0; }
        unsigned char   ucType;
        unsigned short  usAmmo;
        unsigned short  usAmmoInClip;
    };

public:
                                        CClientGame                     ( bool bLocalPlay = false );
                                        ~CClientGame                    ( void );

    bool                                StartGame                       ( const char* szNick, const char* szPassword );
    bool                                StartLocalGame                  ( const char* szConfig, const char* szPassword = NULL );
    void                                SetupLocalGame                  ( const char* szConfig );
    //bool                                StartGame                       ( void );
    inline bool                         IsLocalGame                     ( ) const { return m_bLocalPlay; }

    void                                DoPulsePreFrame                 ( void );
    void                                DoPulsePreHUDRender             ( bool bDidUnminimize, bool bDidRecreateRenderTargets );
    void                                DoPulsePostFrame                ( void );
    void                                DoPulses                        ( void );

    DWORD                               GetFrameTimeSlice               ( void ) { return m_dwFrameTimeSlice; }

    void                                HandleException                 ( CExceptionInformation* pExceptionInformation );
    static void                         HandleRadioNext                 ( CControlFunctionBind* );
    static void                         HandleRadioPrevious             ( CControlFunctionBind* );

    bool                                IsNickValid                     ( const char* szNick );
    bool                                IsNametagValid                  ( const char* szNick );

    bool                                IsOfflineGame                   ( void ) { return m_Status == STATUS_OFFLINE; };

    inline bool                         IsGettingIntoVehicle            ( void ) { return m_bIsGettingIntoVehicle; };
    
    void                                StartPlayback                   ( void );
    void                                EnablePacketRecorder            ( const char* szFilename );
    void                                InitVoice                       ( bool bEnabled, unsigned int uiServerSampleRate, unsigned char ucQuality, unsigned int uiBitrate );

    // Accessors
    inline CVoiceRecorder*              GetVoiceRecorder                ( void )        { return m_pVoiceRecorder; };
    inline CClientManager*              GetManager                      ( void )        { return m_pManager; };
    inline CClientObjectManager*        GetObjectManager                ( void )        { return m_pObjectManager; };
    inline CClientPickupManager*        GetPickupManager                ( void )        { return m_pPickupManager; };
    inline CClientPlayerManager*        GetPlayerManager                ( void )        { return m_pPlayerManager; };
    inline CClientRadarMarkerManager*   GetRadarMarkerManager           ( void )        { return m_pRadarMarkerManager; };
    inline CClientVehicleManager*       GetVehicleManager               ( void )        { return m_pVehicleManager; };
    inline CClientTeamManager*          GetTeamManager                  ( void )        { return m_pTeamManager; };
    inline CClientPedManager*           GetPedManager                   ( void )        { return m_pPedManager; };
    inline CClientGUIManager*           GetGUIManager                   ( void )        { return m_pGUIManager; };
    inline CResourceManager*            GetResourceManager              ( void )        { return m_pResourceManager; }
    inline CLuaManager*                 GetLuaManager                   ( void )        { return m_pLuaManager; }
    inline CScriptKeyBinds*             GetScriptKeyBinds               ( void )        { return m_pScriptKeyBinds; }
    inline CScriptDebugging*            GetScriptDebugging              ( void )        { return m_pScriptDebugging; }
    inline CRegisteredCommands*         GetRegisteredCommands           ( void )        { return m_RegisteredCommands; }
    inline CZoneNames*                  GetZoneNames                    ( void )        { return m_pZoneNames; };
    inline CNametags*                   GetNametags                     ( void )        { return m_pNametags; }
    inline CSyncDebug*                  GetSyncDebug                    ( void )        { return m_pSyncDebug; };
    inline CRPCFunctions*               GetRPCFunctions                 ( void )        { return m_pRPCFunctions; }

    inline CClientEntity*               GetRootEntity                   ( void )        { return m_pRootEntity; }
    inline CEvents*                     GetEvents                       ( void )        { return &m_Events; }

    inline CBlendedWeather*             GetBlendedWeather               ( void )        { return m_pBlendedWeather; };
    inline CNetAPI*                     GetNetAPI                       ( void )        { return m_pNetAPI; };
    inline CClientPlayer*               GetLocalPlayer                  ( void )        { return m_pLocalPlayer; };
    inline CRadarMap*                   GetRadarMap                     ( void )        { return m_pRadarMap; };
    inline CMovingObjectsManager*       GetMovingObjectsManager         ( void )        { return m_pMovingObjectsManager; }

    inline CUnoccupiedVehicleSync*      GetUnoccupiedVehicleSync        ( void )        { return m_pUnoccupiedVehicleSync; }
    inline CPedSync*                    GetPedSync                      ( void )        { return m_pPedSync; }
    inline CObjectSync*                 GetObjectSync                   ( void )        { return m_pObjectSync; }

    // Status toggles
    void                                ShowNetstat                     ( bool bShow );
    void                                ShowEaeg                        ( bool bShow );
    inline void                         ShowFPS                         ( bool bShow )  { m_bShowFPS = bShow; };

    #if defined (MTA_DEBUG) || defined (MTA_BETA)
    inline void                         ShowSyncingInfo                 ( bool bShow )  { m_bShowSyncingInfo = bShow; };
    #endif

#ifdef MTA_WEPSYNCDBG
    void                                ShowWepdata                     ( const char* szNick );
#endif

    #ifdef MTA_DEBUG
    void                                ShowWepdata                     ( const char* szNick );
    void                                ShowTasks                       ( const char* szNick );
    void                                ShowPlayer                      ( const char* szNick );
    void                                SetMimic                        ( unsigned int uiMimicCount );
    inline void                         SetMimicLag                     ( bool bMimicLag )  { m_bMimicLag = bMimicLag; };
    inline void                         SetDoPaintballs                 ( bool bDoPaintballs )  { m_bDoPaintballs = bDoPaintballs; }
    void                                ShowInterpolation               ( bool bShow ) { m_bShowInterpolation = bShow; }
    bool                                IsShowingInterpolation          ( ) const { return m_bShowInterpolation; }
    #endif

    inline CEntity*                     GetTargetedGameEntity           ( void )    { return m_pTargetedGameEntity; }
    inline CClientEntity*               GetTargetedEntity               ( void )    { return m_pTargetedEntity; }
    
    inline CClientPlayer*               GetTargetedPlayer               ( void )    { return ( m_TargetedPlayerID != INVALID_ELEMENT_ID ) ? m_pPlayerManager->Get ( m_TargetedPlayerID ) : NULL; }

    inline CClientEntity*               GetDamageEntity                 ( void )    { return m_pDamageEntity; }
    inline ElementID                    GetDamagerID                    ( void )    { return m_DamagerID; }
    inline unsigned char                GetDamageWeapon                 ( void )    { return m_ucDamageWeapon; }
    inline unsigned char                GetDamageBodyPiece              ( void )    { return m_ucDamageBodyPiece; }
    inline bool                         GetDamageSent                   ( void )    { return m_bDamageSent; }
    inline void                         SetDamageSent                   ( bool b )  { m_bDamageSent = b; }

    void                                ProcessVehicleInOutKey          ( bool bPassenger );

    void                                ResetVehicleInOut               ( void );

    void                                SetAllDimensions                ( unsigned short usDimension );

    static void                         StaticKeyStrokeHandler          ( const SBindableKey * pKey, bool bState );
    void                                KeyStrokeHandler                ( const SBindableKey * pKey, bool bState );
    static bool                         StaticCharacterKeyHandler       ( WPARAM wChar );
    bool                                CharacterKeyHandler             ( WPARAM wChar );

    static void                         StaticProcessClientKeyBind      ( CKeyFunctionBind* pBind );
    void                                ProcessClientKeyBind            ( CKeyFunctionBind* pBind );
    static void                         StaticProcessClientControlBind  ( CControlFunctionBind* pBind );
    void                                ProcessClientControlBind        ( CControlFunctionBind* pBind );
  

    static void                         StaticProcessServerKeyBind      ( CKeyFunctionBind* pBind );
    void                                ProcessServerKeyBind            ( CKeyFunctionBind* pBind );
    static void                         StaticProcessServerControlBind  ( CControlFunctionBind* pBind );
    void                                ProcessServerControlBind        ( CControlFunctionBind* pBind );
    
    bool                                ProcessMessageForCursorEvents   ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    CClientPlayer *                     GetClosestRemotePlayer          ( const CVector & vecTemp, float fMaxDistance );

    void                                AddBuiltInEvents                ( void );

    inline const char*                  GetModRoot                      ( void )                        { return m_strModRoot; };

    void                                SetGameSpeed                    ( float fSpeed );
    void                                SetMinuteDuration               ( unsigned long ulDelay );
    inline long                         GetMoney                        ( void )                        { return m_lMoney; }
    void                                SetMoney                        ( long lMoney );
    void                                SetWanted                       ( DWORD dwWanted );

    void                                ResetAmmoInClip                 ( void ) { memset(m_wasWeaponAmmoInClip,0,sizeof(m_wasWeaponAmmoInClip)); }

    void                                ResetMapInfo                    ( void );

    void                                DoWastedCheck                   ( ElementID damagerID = INVALID_ELEMENT_ID, unsigned char ucWeapon = 0xFF, unsigned char ucBodyPiece = 0xFF, AssocGroupId animGroup = 0, AnimationId animId = 15 );
    void                                SendPedWastedPacket                       ( CClientPed* Ped, ElementID damagerID = INVALID_ELEMENT_ID, unsigned char ucWeapon = 0xFF, unsigned char ucBodyPiece = 0xFF, AssocGroupId animGroup = 0, AnimationId animID = 15 );

    void                                SetMarkerBounce                 ( float fMarkerBounce ) { m_fMarkerBounce = fMarkerBounce; }
    float                               GetMarkerBounce                 ( void ) { return m_fMarkerBounce; }

    inline CClientGUIElement*           GetClickedGUIElement            ( void )                        { return m_pClickedGUIElement; }
    inline void                         SetClickedGUIElement            ( CClientGUIElement* pElement ) { m_pClickedGUIElement = NULL; }

    inline void                         SetInitiallyFadedOut            ( bool bFadedOut )              { m_bInitiallyFadedOut = bFadedOut; };

    inline bool                         GetHudAreaNameDisabled          ( void )                        { return m_bHudAreaNameDisabled; };
    inline void                         SetHudAreaNameDisabled          ( bool bDisabled )              { m_bHudAreaNameDisabled = bDisabled; };

    bool                                SetGlitchEnabled                ( unsigned char cGlitch, bool bEnabled );
    bool                                IsGlitchEnabled                 ( unsigned char cGlitch );

    bool                                SetCloudsEnabled                ( bool bEnabled );
    bool                                GetCloudsEnabled                ( void );

    inline CTransferBox*                GetTransferBox                  ( void )                        { return m_pTransferBox; };

    void                                ChangeVehicleWeapon             ( bool bNext );
    void                                NotifyBigPacketProgress         ( unsigned long ulBytesReceived, unsigned long ulTotalSize );
    bool                                IsDownloadingBigPacket          ( ) const                       { return m_bReceivingBigPacket; }
    bool                                IsBeingDeleted                  ( void )                        { return m_bBeingDeleted; }

private:

    // CGUI Callbacks
    bool                                OnKeyDown                       ( CGUIKeyEventArgs Args );
    bool                                OnMouseClick                    ( CGUIMouseEventArgs Args );
    bool                                OnMouseDoubleClick              ( CGUIMouseEventArgs Args );
    bool                                OnMouseButtonDown               ( CGUIMouseEventArgs Args );
    bool                                OnMouseButtonUp                 ( CGUIMouseEventArgs Args );
    bool                                OnMouseMove                     ( CGUIMouseEventArgs Args );
    bool                                OnMouseEnter                    ( CGUIMouseEventArgs Args );
    bool                                OnMouseLeave                    ( CGUIMouseEventArgs Args );
    bool                                OnMouseWheel                    ( CGUIMouseEventArgs Args );
    bool                                OnMove                          ( CGUIElement * pElement );
    bool                                OnSize                          ( CGUIElement * pElement );
    bool                                OnFocusGain                     ( CGUIFocusEventArgs Args );
    bool                                OnFocusLoss                     ( CGUIFocusEventArgs Args );

    float                               m_fMarkerBounce;
    // Network update functions
    void                                DoVehicleInKeyCheck             ( void );
    void                                UpdateVehicleInOut              ( void );
    void                                UpdatePlayerTarget              ( void );
    void                                UpdatePlayerWeapons             ( void );
    void                                UpdateTrailers                  ( void );
    void                                UpdateFireKey                   ( void );
    void                                UpdateStunts                    ( void );

    // Internal key-bind callbacks
    static void                         StaticUpdateFireKey             ( CControlFunctionBind * pBind );

    void                                DrawFPS                         ( void );
    
#ifdef MTA_DEBUG
    void                                DrawTasks                       ( CClientPlayer* pPlayer );
    void                                DrawPlayerDetails               ( CClientPlayer* pPlayer );
    void                                UpdateMimics                    ( void );
    void                                DoPaintballs                    ( void );
    void                                DrawWeaponsyncData              ( CClientPlayer* pPlayer );
#endif

#ifdef MTA_WEPSYNCDBG
    void                                DrawWeaponsyncData              ( CClientPlayer* pPlayer );
#endif


    void                                DownloadFiles                   ( void );

    void                                QuitPlayer                      ( CClientPlayer* pPlayer, eQuitReason Reason );

    void                                Event_OnIngame                  ( void );
    void                                Event_OnIngameAndReady          ( void );
    void                                Event_OnIngameAndConnected      ( void );
    void                                Event_OnTransferComplete        ( void );

    static bool                         StaticDamageHandler             ( CPed* pDamagePed, CEventDamage * pEvent );
    static void                         StaticFireHandler               ( CFire* pFire );
    static bool                         StaticBreakTowLinkHandler       ( CVehicle* pTowedVehicle );
    static void                         StaticDrawRadarAreasHandler     ( void );
    static void                         StaticProjectileInitiateHandler ( CClientProjectile * pProjectile );
    static void                         StaticRender3DStuffHandler      ( void );
    static bool                         StaticChokingHandler            ( unsigned char ucWeaponType );
    static void                         StaticPreWorldProcessHandler    ( void );
    static void                         StaticPostWorldProcessHandler   ( void );
    static void                         StaticIdleHandler               ( void );
 
    bool                                DamageHandler                   ( CPed* pDamagePed, CEventDamage * pEvent );
    void                                FireHandler                     ( CFire* pFire );
    bool                                BreakTowLinkHandler             ( CVehicle* pTowedVehicle );
    void                                DrawRadarAreasHandler           ( void );
    void                                ProjectileInitiateHandler       ( CClientProjectile * pProjectile );
    void                                Render3DStuffHandler            ( void );
    bool                                ChokingHandler                  ( unsigned char ucWeaponType );
    void                                PreWorldProcessHandler          ( void );
    void                                PostWorldProcessHandler         ( void );
    void                                IdleHandler                     ( void );
   
    static bool                         StaticProcessMessage            ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    bool                                ProcessMessage                  ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

    static void                         PreWeaponFire                   ( CPlayerPed* pPlayerPed );
    static void                         PostWeaponFire                  ( void );
    static void                         BulletImpact                    ( CPed* pInitiator, CEntity* pVictim, const CVector* pStartPosition, const CVector* pEndPosition );
    static bool                         StaticProcessPacket             ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );

    bool                                VerifySADataFiles               ( int iEnableClientChecks = 0 );

public:
    void                                SendExplosionSync               ( const CVector& vecPosition, eExplosionType Type, CClientEntity * pOrigin = NULL );
    void                                SendFireSync                    ( CFire* pFire );
    void                                SendProjectileSync              ( CClientProjectile * pProjectile );

private:
    eStatus                             m_Status;
    unsigned long                       m_ulTimeStart;
    unsigned long                       m_ulVerifyTimeStart;
    unsigned long                       m_ulLastClickTick;
    CVector2D                           m_vecLastCursorPosition;
    bool                                m_bWaitingForLocalConnect;
    bool                                m_bErrorStartingLocal;
    int                                 m_iLocalConnectAttempts;

    bool                                m_bIsPlayingBack;
    bool                                m_bFirstPlaybackFrame;

    CClientManager*                     m_pManager;
    CClientCamera*                      m_pCamera;
    CClientGUIManager*                  m_pGUIManager;
    CClientMarkerManager*               m_pMarkerManager;
    CClientObjectManager*               m_pObjectManager;
    CClientPickupManager*               m_pPickupManager;
    CClientPlayerManager*               m_pPlayerManager;
    CClientDisplayManager*              m_pDisplayManager;
    CClientVehicleManager*              m_pVehicleManager;
    CClientRadarAreaManager*            m_pRadarAreaManager;
    CClientRadarMarkerManager*          m_pRadarMarkerManager;
    CClientPathManager*                 m_pPathManager;
    CClientTeamManager*                 m_pTeamManager;
    CClientPedManager*                  m_pPedManager;
    CClientProjectileManager*           m_pProjectileManager;
    CRPCFunctions*                      m_pRPCFunctions;
    CUnoccupiedVehicleSync*             m_pUnoccupiedVehicleSync;
    CPedSync*                           m_pPedSync;
    CObjectSync*                        m_pObjectSync;
    CMovingObjectsManager*              m_pMovingObjectsManager;
    CNametags*                          m_pNametags;
    CNetAPI*                            m_pNetAPI;
    CNetworkStats*                      m_pNetworkStats;
    CSyncDebug*                         m_pSyncDebug;
    //CScreenshot*                        m_pScreenshot;
    CRadarMap*                          m_pRadarMap;
    CTransferBox*                       m_pTransferBox;
    CResourceManager*                   m_pResourceManager;
    CScriptKeyBinds*                    m_pScriptKeyBinds;
    CZoneNames*                         m_pZoneNames;
    CPacketHandler*                     m_pPacketHandler;
    CLocalServer*                       m_pLocalServer;
    bool                                m_bInitiallyFadedOut;
    bool                                m_bHudAreaNameDisabled;

    std::string                         m_localServerPassword;

    // Revised facilities
    CVoiceRecorder*                     m_pVoiceRecorder;

    CClientPlayer*                      m_pLocalPlayer;
    ElementID                           m_LocalID;
    char                                m_szLocalNick [MAX_PLAYER_NICK_LENGTH + 1];

    CClientEntity*                      m_pRootEntity;
    CLuaManager*                        m_pLuaManager;
    CScriptDebugging*                   m_pScriptDebugging;
    CRegisteredCommands*                m_RegisteredCommands;

    // Map statuses
    char                                m_szCurrentMapName[MAX_MAPNAME_LENGTH + 1];
    SString                             m_strModRoot;

    CBlendedWeather*                    m_pBlendedWeather;
    bool                                m_bShowNametags;
    bool                                m_bShowRadar;

    // Status booleans
    bool                                m_bGameLoaded;
    bool                                m_bTriggeredIngameAndConnected;
    bool                                m_bGracefulDisconnect;

    // Network update vars
    unsigned long                       m_ulLastVehicleInOutTime;
    bool                                m_bIsGettingOutOfVehicle;
    bool                                m_bIsGettingIntoVehicle;
    bool                                m_bIsJackingVehicle;
    bool                                m_bIsGettingJacked;
    ElementID                           m_VehicleInOutID;
    unsigned char                       m_ucVehicleInOutSeat;
    bool                                m_bNoNewVehicleTask;
    CClientPlayer*                      m_pGettingJackedBy;

    CEntity*                            m_pTargetedGameEntity;
    CClientEntity*                      m_pTargetedEntity;
    bool                                m_bTargetSent;
    ElementID                           m_TargetedPlayerID;

    CClientEntity*                      m_pDamageEntity;
    ElementID                           m_DamagerID;
    unsigned char                       m_ucDamageWeapon;
    unsigned char                       m_ucDamageBodyPiece;
    unsigned long                       m_ulDamageTime;
    bool                                m_bDamageSent;

    DWORD                               m_dwTransferStarted;
    bool                                m_bTransferReset;

    eWeaponSlot                         m_lastWeaponSlot;
    DWORD                               m_wasWeaponAmmoInClip[WEAPONSLOT_MAX + 1];

    bool                                m_bLocalPlay;

    bool                                m_bShowNetstat;
    bool                                m_bShowFPS;

    bool                                m_bTransferResource;
    bool                                m_bTransferInitiated;

    float                               m_fGameSpeed;
    long                                m_lMoney;
    DWORD                               m_dwWanted;

    bool                                m_Glitches[NUM_GLITCHES];

    //Clouds Enabled
    bool                                m_bCloudsEnabled;

    unsigned long                       m_ulMinuteDuration;

    CClientGUIElement*                  m_pClickedGUIElement;

    eHTTPDownloadType                   m_ucHTTPDownloadType;
    unsigned short                      m_usHTTPDownloadPort;
    SString                             m_strHTTPDownloadURL;

    bool                                m_bReceivingBigPacket;
    unsigned long                       m_ulBigPacketSize;
    unsigned long                       m_ulBigPacketBytesReceivedBase;
    CTransferBox*                       m_pBigPacketTransferBox;

    DWORD                               m_dwFrameTimeSlice;     // how long it took (in ms) to process the current frame
    DWORD                               m_dwLastFrameTick;      // time at which the previous frame was processed

    long long                           m_llLastTransgressionTime;
    SString                             m_strLastDiagnosticStatus;

    bool                                m_bBeingDeleted;        // To enable speedy disconnect

    bool                                m_bWasMinimized;

#if defined (MTA_DEBUG) || defined (MTA_BETA)
    bool                                m_bShowSyncingInfo;
#endif

#ifdef MTA_DEBUG
    CClientPlayer*                      m_pShowPlayerTasks;
    CClientPlayer*                      m_pShowPlayer;
    std::list < CClientPlayer* >        m_Mimics;
    bool                                m_bMimicLag;
    unsigned long                       m_ulLastMimicLag;
    CVector                             m_vecLastMimicPos;
    CVector                             m_vecLastMimicMove;
    CVector                             m_vecLastMimicTurn;
    CVector                             m_vecLastMimicRot;
    bool                                m_bDoPaintballs;
    bool                                m_bShowInterpolation;
#endif

    // Debug class. Empty in release.
public:
    CFoo                                m_Foo;

private:

    CEvents                             m_Events;
};

extern CClientGame* g_pClientGame;

#endif

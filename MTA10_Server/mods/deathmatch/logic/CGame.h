/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CGame.h
*  PURPOSE:     Server game class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Peter Beverloo <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CGame;

#ifndef __CGAME_H
#define __CGAME_H

#include <net/CNetServer.h>
#include "CClient.h"
#include "CCommandLineParser.h"
#include "CConnectHistory.h"
#include "CElementDeleter.h"
#include "CWhoWas.h"
#include "CSerialManager.h"

#include "packets/CCommandPacket.h"
#include "packets/CExplosionSyncPacket.h"
#include "packets/CProjectileSyncPacket.h"
#include "packets/CPedWastedPacket.h"
#include "packets/CPlayerJoinDataPacket.h"
#include "packets/CPlayerQuitPacket.h"
#include "packets/CPlayerTimeoutPacket.h"
#include "packets/CPlayerPuresyncPacket.h"
#include "packets/CVehiclePuresyncPacket.h"
#include "packets/CKeysyncPacket.h"
#include "packets/CVehicleInOutPacket.h"
#include "packets/CVehicleDamageSyncPacket.h"
#include "packets/CVehicleTrailerPacket.h"
#include "packets/CVoiceDataPacket.h"
#include "packets/CLuaEventPacket.h"
#include "packets/CDetonateSatchelsPacket.h"
#include "packets/CCustomDataPacket.h"
#include "packets/CCameraSyncPacket.h"
#include "packets/CPlayerTransgressionPacket.h"
#include "packets/CPlayerDiagnosticPacket.h"
#include "packets/CPlayerModInfoPacket.h"

#include "CRPCFunctions.h"

#include "lua/CLuaManager.h"

// Forward declarations
class ASE;
class CAccessControlListManager;
class CAccountManager;
class CBanManager;
class CBlipManager;
class CClock;
class CColManager;
class CConsole;
class CConsoleClient;
class CElementDeleter;
class CGroups;
class CHandlingManager;
class CHTTPD;
class CMainConfig;
class CMapManager;
class CMarkerManager;
class CObjectManager;
class CPacket;
class CPacketTranslator;
class CPedManager;
class CPickupManager;
class CPlayer;
class CPlayerManager;
class CRadarAreaManager;
class CRegisteredCommands;
class CRegistryManager;
class CRegistry;
class CRemoteCalls;
class CResourceDownloader;
class CResourceManager;
class CScriptDebugging;
class CSettings;
class CTeamManager;
class CUnoccupiedVehicleSync;
class CPedSync;
class CObjectSync;
class CVehicleManager;
class CZoneNames;
class CLanBroadcast;
class CWaterManager;

// Packet forward declarations
class CCommandPacket;
class CCustomDataPacket;
class CDetonateSatchelsPacket;
class CExplosionSyncPacket;
class CKeysyncPacket;
class CLuaEventPacket;
class CPlayerJoinDataPacket;
class CPlayerPuresyncPacket;
class CPlayerQuitPacket;
class CPlayerTimeoutPacket;
class CPlayerWastedPacket;
class CProjectileSyncPacket;
class CVehicleDamageSyncPacket;
class CVehicleInOutPacket;
class CVehiclePuresyncPacket;
class CVehicleTrailerPacket;
class CVoiceDataPacket;
class CWeaponDamageCheckPacket;

class CGame
{
public:
    ZERO_ON_NEW

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
    enum eGlitchType
    {
        GLITCH_QUICKRELOAD,
        GLITCH_FASTFIRE,
        GLITCH_FASTMOVE,
        GLITCH_CROUCHBUG,
    };
public:
                                CGame                       ();
                                ~CGame                      ();

    void                        GetTag                      ( char* szInfoTag, int iInfoTag );
    void                        HandleInput                 ( char* szCommand );

    void                        DoPulse                     ();

    bool                        Start                       ( int iArgumentCount, char* szArguments [] );
    void                        Stop                        ();

    static bool                 StaticProcessPacket         ( unsigned char ucPacketID, NetServerPlayerID& Socket, NetBitStreamInterface& BitStream );
    bool                        ProcessPacket               ( CPacket& Packet );

    inline void                 SetIsFinished               ( bool bFinished )  { m_bIsFinished = bFinished; };
    inline bool                 IsFinished                  ()            { return m_bIsFinished; };

    inline CMainConfig*             GetConfig                   ()        { return m_pMainConfig; }
    inline CHandlingManager*        GetHandlingManager          ()        { return m_pHandlingManager; }
    inline CMapManager*             GetMapManager               ()        { return m_pMapManager; }
    inline CPlayerManager*          GetPlayerManager            ()        { return m_pPlayerManager; }
    inline CObjectManager*          GetObjectManager            ()        { return m_pObjectManager; }
    inline CVehicleManager*         GetVehicleManager           ()        { return m_pVehicleManager; }
    inline CTeamManager*            GetTeamManager              ()        { return m_pTeamManager; }
    inline CUnoccupiedVehicleSync*  GetUnoccupiedVehicleSync    ()        { return m_pUnoccupiedVehicleSync; }
    inline CPedSync*                GetPedSync                  ()        { return m_pPedSync; }
    inline CObjectSync*             GetObjectSync               ()        { return m_pObjectSync; }
    inline CConsole*                GetConsole                  ()        { return m_pConsole; }
    inline CRegistryManager*        GetRegistryManager          ()        { return m_pRegistryManager; }
    inline CRegistry*               GetRegistry                 ()        { return m_pRegistry; }
    inline CAccountManager*         GetAccountManager           ()        { return m_pAccountManager; }
    inline CScriptDebugging*        GetScriptDebugging          ()        { return m_pScriptDebugging; }
    inline CEvents*                 GetEvents                   ()        { return &m_Events; }
    inline CColManager*             GetColManager               ()        { return m_pColManager; }
    inline CPedManager*             GetPedManager               ()        { return m_pPedManager; }
    inline CResourceManager*        GetResourceManager          ()        { return m_pResourceManager; }
    inline CMarkerManager*          GetMarkerManager            ()        { return m_pMarkerManager; }
    inline CBlipManager*            GetBlipManager              ()        { return m_pBlipManager; }
    inline CPickupManager*          GetPickupManager            ()        { return m_pPickupManager; }
    inline CRadarAreaManager*       GetRadarAreaManager         ()        { return m_pRadarAreaManager; }
    inline CGroups*                 GetGroups                   ()        { return m_pGroups; }
    inline CElementDeleter*         GetElementDeleter           ()        { return &m_ElementDeleter; }
    inline CHTTPD*                  GetHTTPD                    ()        { return m_pHTTPD; }
    inline CSettings*               GetSettings                 ()        { return m_pSettings; }
    inline CAccessControlListManager* GetACLManager             ()        { return m_pACLManager; }
    inline CBanManager*             GetBanManager               ()        { return m_pBanManager; }
    inline CRemoteCalls*            GetRemoteCalls              ()        { return m_pRemoteCalls; }
    inline CResourceDownloader*     GetResourceDownloader       ()        { return m_pResourceDownloader; }
    inline CZoneNames*              GetZoneNames                ()        { return m_pZoneNames; }
    inline CClock*                  GetClock                    ()        { return m_pClock; }
    inline CSerialManager*          GetSerialManager            ()        { return &m_SerialManager; }
    inline CWaterManager*           GetWaterManager             ()        { return m_pWaterManager; }

    void                        JoinPlayer                  ( CPlayer& Player );
    void                        InitialDataStream           ( CPlayer& Player );
    void                        QuitPlayer                  ( CPlayer& Player, CClient::eQuitReasons Reason = CClient::QUIT_QUIT, bool bSayInConsole = true, const char* szKickReason = "None", const char* szResponsiblePlayer = "None" );

    inline class CLuaManager*   GetLuaManager               ()        { return m_pLuaManager; };

    inline float                GetGravity                  ()        { return m_fGravity; }
    inline void                 SetGravity                  ( float fGravity )  { m_fGravity = fGravity; }

    inline unsigned char        GetTrafficLightState        ()        { return m_ucTrafficLightState; }
    inline void                 SetTrafficLightState        ( unsigned char ucState ) { m_ucTrafficLightState = ucState; }

    inline bool                 GetTrafficLightsLocked      ()        { return m_bTrafficLightsLocked; }
    inline void                 SetTrafficLightsLocked      ( bool bLocked ) { m_bTrafficLightsLocked = bLocked; }

    inline float                GetJetpackMaxHeight         () { return m_fJetpackMaxHeight; }
    inline void                 SetJetpackMaxHeight         ( float fMaxHeight ) { m_fJetpackMaxHeight = fMaxHeight; }

    inline float                GetGameSpeed                ()        { return m_fGameSpeed; }
    inline void                 SetGameSpeed                ( float fGameSpeed )  { m_fGameSpeed = fGameSpeed; }

    inline bool                 HasSkyGradient              ()        { return m_bHasSkyGradient; }
    inline void                 SetHasSkyGradient           ( bool bHasSkyGradient ) { m_bHasSkyGradient = bHasSkyGradient; }

    inline void                 GetSkyGradient              ( unsigned char& ucTR, unsigned char& ucTG, unsigned char& ucTB, unsigned char& ucBR, unsigned char& ucBG, unsigned char& ucBB ) { ucTR = m_ucSkyGradientTR; ucTG = m_ucSkyGradientTG; ucTB = m_ucSkyGradientTB; ucBR = m_ucSkyGradientBR; ucBG = m_ucSkyGradientBG; ucBB = m_ucSkyGradientBB; }
    inline void                 SetSkyGradient              ( unsigned char& ucTR, unsigned char& ucTG, unsigned char& ucTB, unsigned char& ucBR, unsigned char& ucBG, unsigned char& ucBB ) { m_ucSkyGradientTR = ucTR; m_ucSkyGradientTG = ucTG; m_ucSkyGradientTB = ucTB; m_ucSkyGradientBR = ucBR; m_ucSkyGradientBG = ucBG; m_ucSkyGradientBB = ucBB; }

    inline bool                 HasHeatHaze                 ()        { return m_bHasHeatHaze; }
    inline void                 SetHasHeatHaze              ( bool bHasHeatHaze ) { m_bHasHeatHaze = bHasHeatHaze; }

    inline void                 GetHeatHaze                 ( SHeatHazeSettings& heatHazeSettings )         { heatHazeSettings = m_HeatHazeSettings; }
    inline void                 SetHeatHaze                 ( const SHeatHazeSettings& heatHazeSettings )   { m_HeatHazeSettings = heatHazeSettings; }

    inline bool                 GetInteriorSoundsEnabled    ()        { return m_bInteriorSoundsEnabled; }
    inline void                 SetInteriorSoundsEnabled    ( bool bEnable )    { m_bInteriorSoundsEnabled = bEnable; }

    inline bool                 HasWaterColor               ()        { return m_bOverrideWaterColor; }
    inline void                 SetHasWaterColor            ( bool bOverrideWaterColor ) { m_bOverrideWaterColor = bOverrideWaterColor; }

    inline void                 GetWaterColor               ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha ) { ucRed = m_ucWaterRed; ucGreen = m_ucWaterGreen; ucBlue = m_ucWaterBlue; ucAlpha = m_ucWaterAlpha; }
    inline void                 SetWaterColor               ( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue, unsigned char& ucAlpha ) { m_ucWaterRed = ucRed; m_ucWaterGreen = ucGreen; m_ucWaterBlue = ucBlue; m_ucWaterAlpha = ucAlpha; }

    inline bool                 HasRainLevel                ()        { return m_bOverrideRainLevel; }
    inline void                 SetHasRainLevel             ( bool bOverrideRainLevel ) { m_bOverrideRainLevel = bOverrideRainLevel; }

    inline float                GetRainLevel                ()        { return m_fRainLevel; }
    inline void                 SetRainLevel                ( float& fRainLevel ) { m_fRainLevel = fRainLevel; }

    inline bool                 HasSunSize                  ()        { return m_bOverrideSunSize; }
    inline void                 SetHasSunSize               ( bool bOverrideSunSize ) { m_bOverrideSunSize = bOverrideSunSize; }

    inline float                GetSunSize                  ()        { return m_fSunSize; }
    inline void                 SetSunSize                  ( float& fSunSize ) { m_fSunSize = fSunSize; }

    inline bool                 HasSunColor                 ()        { return m_bOverrideSunColor; }
    inline void                 SetHasSunColor              ( bool bOverrideSunColor ) { m_bOverrideSunColor = bOverrideSunColor; }

    inline void                 GetSunColor                 ( unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG, unsigned char& ucCoronaB )    { ucCoreR = m_ucSunCoreR; ucCoreG = m_ucSunCoreG; ucCoreB = m_ucSunCoreB; ucCoronaR = m_ucSunCoronaR; ucCoronaG = m_ucSunCoronaG; ucCoronaB = m_ucSunCoronaB; }
    inline void                 SetSunColor                 ( unsigned char& ucCoreR, unsigned char& ucCoreG, unsigned char& ucCoreB, unsigned char& ucCoronaR, unsigned char& ucCoronaG, unsigned char& ucCoronaB )    { m_ucSunCoreR = ucCoreR; m_ucSunCoreG = ucCoreG; m_ucSunCoreB = ucCoreB; m_ucSunCoronaR = ucCoronaR; m_ucSunCoronaG = ucCoronaG; m_ucSunCoronaB = ucCoronaB; }

    inline bool                 HasWindVelocity             ()        { return m_bOverrideWindVelocity; }
    inline void                 SetHasWindVelocity          ( bool bOverrideWindVelocity ) { m_bOverrideWindVelocity = bOverrideWindVelocity; }

    inline void                 GetWindVelocity             ( float& fVelX, float& fVelY, float& fVelZ )    { fVelX = m_fWindVelX; fVelY = m_fWindVelY; fVelZ = m_fWindVelZ; }
    inline void                 SetWindVelocity             ( float& fVelX, float& fVelY, float& fVelZ )    { m_fWindVelX = fVelX; m_fWindVelY = fVelY; m_fWindVelZ = fVelZ; }

    inline bool                 HasFarClipDistance          ()        { return m_bOverrideFarClip; }
    inline void                 SetHasFarClipDistance       ( bool bOverrideFarClip ) { m_bOverrideFarClip = bOverrideFarClip; }

    inline float                GetFarClipDistance          ()        { return m_fFarClipDistance; }
    inline void                 SetFarClipDistance          ( float& fFarClipDistance ) { m_fFarClipDistance = fFarClipDistance; }

    inline bool                 HasFogDistance              ()        { return m_bOverrideFogDistance; }
    inline void                 SetHasFogDistance           ( bool bOverrideFogDistance ) { m_bOverrideFogDistance = bOverrideFogDistance; }

    inline float                GetFogDistance              ()        { return m_fFogDistance; }
    inline void                 SetFogDistance              ( float& fFogDistance ) { m_fFogDistance = fFogDistance; }

    inline float                GetAircraftMaxHeight        () { return m_fAircraftMaxHeight; }
    inline void                 SetAircraftMaxHeight        ( float fMaxHeight ) { m_fAircraftMaxHeight = fMaxHeight; }

    inline bool*                GetGarageStates             ()        { return m_bGarageStates; }

    void                        Lock                        ();
    void                        Unlock                      ();

    inline bool                 IsBeingDeleted              ()        { return m_bBeingDeleted; }
    void                        ResetMapInfo                ();

    void                        SetGlitchEnabled            ( const std::string& strGlitch, bool bEnabled );
    bool                        IsGlitchEnabled             ( const std::string& strGlitch );
    bool                        IsGlitchEnabled             ( eGlitchType cGlitch );
    eGlitchType                 GetGlitchIndex              ( const std::string& strGlitch )    { return m_GlitchNames[strGlitch]; }
    bool                        IsGlitch                    ( const std::string& strGlitch )    { return m_GlitchNames.count(strGlitch) > 0; }

    void                        SetCloudsEnabled            ( bool bEnabled );
    bool                        GetCloudsEnabled            ();

    void                        PulseMasterServerAnnounce   ();
    void                        StartOpenPortsTest          ();

    bool                        IsServerFullyUp             ()        { return m_bServerFullyUp; }

private:
    void                        AddBuiltInEvents            ();
    void                        RelayPlayerPuresync         ( class CPacket& Packet );

    void                        ProcessTrafficLights        ( unsigned long ulCurrentTime );

    void                        Packet_PlayerJoin           ( NetServerPlayerID& Source );
    void                        Packet_PlayerJoinData       ( class CPlayerJoinDataPacket& Packet );
    void                        Packet_PedWasted            ( class CPedWastedPacket& Packet );
    void                        Packet_PlayerWasted         ( class CPlayerWastedPacket& Packet );
    void                        Packet_PlayerQuit           ( class CPlayerQuitPacket& Packet );
    void                        Packet_PlayerTimeout        ( class CPlayerTimeoutPacket& Packet );
    void                        Packet_PlayerPuresync       ( class CPlayerPuresyncPacket& Packet );
    void                        Packet_DetonateSatchels     ( class CDetonateSatchelsPacket& Packet );
    void                        Packet_ExplosionSync        ( class CExplosionSyncPacket& Packet );
    void                        Packet_ProjectileSync       ( class CProjectileSyncPacket& Packet );
    void                        Packet_Command              ( class CCommandPacket& Packet );
    void                        Packet_VehicleDamageSync    ( class CVehicleDamageSyncPacket& Packet );
    void                        Packet_VehiclePuresync      ( class CVehiclePuresyncPacket& Packet );
    void                        Packet_Keysync              ( class CKeysyncPacket& Packet );
    void                        Packet_Vehicle_InOut        ( class CVehicleInOutPacket& Packet );
    void                        Packet_VehicleTrailer       ( class CVehicleTrailerPacket& Packet );
    void                        Packet_LuaEvent             ( class CLuaEventPacket& Packet );
    void                        Packet_CustomData           ( class CCustomDataPacket& Packet );
    void                        Packet_Voice_Data           ( class CVoiceDataPacket& Packet );
    void                        Packet_Voice_End            ( class CVoiceEndPacket& Packet );
    void                        Packet_CameraSync           ( class CCameraSyncPacket& Packet );
    void                        Packet_PlayerTransgression  ( class CPlayerTransgressionPacket& Packet );
    void                        Packet_PlayerDiagnostic     ( class CPlayerDiagnosticPacket& Packet );
    void                        Packet_PlayerModInfo        ( class CPlayerModInfoPacket & Packet );

    static void                 PlayerCompleteConnect       ( CPlayer* pPlayer, bool bSuccess, const char* szError );

    // Technically, this could be put somewhere else.  It's a callback function
    // which the voice server library will call to send out data.

    CEvents                         m_Events;
    CRemoteCalls*                   m_pRemoteCalls;
    CResourceDownloader*            m_pResourceDownloader;
    CHTTPD*                         m_pHTTPD;
    CMainConfig*                    m_pMainConfig;
    CBlipManager*                   m_pBlipManager;
    CGroups*                        m_pGroups;
    CColManager*                    m_pColManager;
    CObjectManager*                 m_pObjectManager;
    CPickupManager*                 m_pPickupManager;
    CPlayerManager*                 m_pPlayerManager;
    CRadarAreaManager*              m_pRadarAreaManager;
    CVehicleManager*                m_pVehicleManager;
    CPacketTranslator*              m_pPacketTranslator;
    CMapManager*                    m_pMapManager;
    CElementDeleter                 m_ElementDeleter;
    CConnectHistory                 m_FloodProtect;
    CLuaManager*                    m_pLuaManager;
    CScriptDebugging*               m_pScriptDebugging;
    CConsole*                       m_pConsole;
    CUnoccupiedVehicleSync*         m_pUnoccupiedVehicleSync;
    CPedSync*                       m_pPedSync;
    CObjectSync*                    m_pObjectSync;
    CMarkerManager*                 m_pMarkerManager;
    CClock*                         m_pClock;
    CBanManager*                    m_pBanManager;
    CTeamManager*                   m_pTeamManager;
    CWhoWas                         m_WhoWas;
    CCommandLineParser              m_CommandLineParser;
    CRegisteredCommands*            m_pRegisteredCommands;
    CRegistryManager*               m_pRegistryManager;
    CRegistry*                      m_pRegistry;
    CAccountManager*                m_pAccountManager;
    CPedManager*                    m_pPedManager;
    CResourceManager*               m_pResourceManager;
    CAccessControlListManager*      m_pACLManager;
    CSettings*                      m_pSettings;
    CZoneNames*                     m_pZoneNames;
    ASE*                            m_pASE;
    CHandlingManager*               m_pHandlingManager;
    CRPCFunctions*                  m_pRPCFunctions;
    CLanBroadcast*                  m_pLanBroadcast;
    CWaterManager*                  m_pWaterManager;

    CSerialManager                  m_SerialManager;

    char*                       m_szCurrentFileName;

    // This client represents the console input
    CConsoleClient*             m_pConsoleClient;

    float                       m_fGravity;
    float                       m_fGameSpeed;
    float                       m_fJetpackMaxHeight;
    float                       m_fAircraftMaxHeight;

    unsigned char               m_ucTrafficLightState;
    bool                        m_bTrafficLightsLocked;
    unsigned long               m_ulLastTrafficUpdate;

    unsigned char               m_ucSkyGradientTR, m_ucSkyGradientTG, m_ucSkyGradientTB;
    unsigned char               m_ucSkyGradientBR, m_ucSkyGradientBG, m_ucSkyGradientBB;
    bool                        m_bHasSkyGradient;

    SHeatHazeSettings           m_HeatHazeSettings;
    bool                        m_bHasHeatHaze;

    bool                        m_bOverrideWaterColor;
    unsigned char               m_ucWaterRed, m_ucWaterGreen, m_ucWaterBlue, m_ucWaterAlpha;

    bool                        m_bInteriorSoundsEnabled;

    bool                        m_bOverrideRainLevel;
    float                       m_fRainLevel;

    bool                        m_bOverrideSunSize;
    float                       m_fSunSize;

    bool                        m_bOverrideSunColor;
    unsigned char               m_ucSunCoreR, m_ucSunCoreG, m_ucSunCoreB, m_ucSunCoronaR, m_ucSunCoronaG, m_ucSunCoronaB;

    bool                        m_bOverrideWindVelocity;
    float                       m_fWindVelX, m_fWindVelY, m_fWindVelZ;

    bool                        m_bOverrideFarClip;
    float                       m_fFarClipDistance;

    bool                        m_bOverrideFogDistance;
    float                       m_fFogDistance;

    bool                        m_bGarageStates[MAX_GARAGES];

    // FPS statistics
    unsigned long               m_ulLastFPSTime;
    unsigned short              m_usFrames;
    unsigned short              m_usFPS;
    std::map<std::string,eGlitchType> m_GlitchNames;
    bool                        m_Glitches[4];

    // This is true when the app should end
    bool                        m_bIsFinished;
    bool                        m_bBeingDeleted;

    // Clouds Enabled
    bool                        m_bCloudsEnabled;

    long long                   m_llLastAnnouceTime;
    class COpenPortsTester*     m_pOpenPortsTester;

    bool                        m_bServerFullyUp;       // No http operations should be allowed unless this is true
};

extern CFileTranslator *resCacheFileRoot;
extern CFileTranslator *resFileRoot;

#endif

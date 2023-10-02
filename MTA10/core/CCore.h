/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCore.h
*  PURPOSE:     Header file for base core class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CCore;

#ifndef __CCORE_H
#define __CCORE_H

#include "../version.h"

#include <core/CClientBase.h>
#include <core/CCoreInterface.h>
#include "CClientTime.h"
#include "CClientVariables.h"
#include "CCommands.h"
#include "CFileSystemHook.h"
#include "CServer.h"
#include "CSingleton.h"
#include "CGUI.h"
#include "CConnectManager.h"
#include "CDirect3DHookManager.h"
#include "CDirectInputHookManager.h"
#include "CGraphics.h"
#include "CSetCursorPosHook.h"
#include "CMessageLoopHook.h"
#include "CLogger.h"
#include "CModManager.h"
#include "CDirect3DData.h"
#include "tracking/CTCPManager.h"
#include "CClientVariables.h"
#include "CKeyBinds.h"
#include "CScreenShot.h"
#include "CCommunity.h"
#include <xml/CXML.h>
#include <ijsify.h>
#include "CXfireQuery.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "CJoystickManager.h"

#define BLUE_VERSION_STRING     "Multi Theft Auto v" MTA_DM_BUILDTAG_LONG "\n" \
                                "Copyright (C) 2003 - 2011 Multi Theft Auto" \

// Configuration file path (relative to Grand Theft Auto directory)
#define MTA_CONFIG_PATH             "mta/coreconfig.xml"
#define MTA_SERVER_CACHE_PATH       "mta/servercache.xml"
#define CONFIG_ROOT                 "mainconfig"
#define CONFIG_NODE_CVARS           "settings"                  // cvars node
#define CONFIG_NODE_KEYBINDS        "binds"                     // keybinds node
#define CONFIG_NODE_JOYPAD          "joypad"
#define CONFIG_NODE_UPDATER         "updater"
#define CONFIG_NODE_SERVER_INT      "internet_servers"          // backup of last successful master server list query
#define CONFIG_NODE_SERVER_FAV      "favourite_servers"         // favourite servers list node
#define CONFIG_NODE_SERVER_REC      "recently_played_servers"   // recently played servers list node
#define CONFIG_NODE_SERVER_OPTIONS  "serverbrowser_options"     // saved options for the server browser
#define CONFIG_NODE_SERVER_SAVED    "server_passwords"    // This contains saved passwords (as appose to save_server_passwords which is a setting)
#define CONFIG_NODE_SERVER_HISTORY  "connect_history"
#define CONFIG_INTERNET_LIST_TAG    "internet_server"
#define CONFIG_FAVOURITE_LIST_TAG   "favourite_server"
#define CONFIG_RECENT_LIST_TAG      "recently_played_server"
#define CONFIG_HISTORY_LIST_TAG     "connected_server"

extern CFileTranslator *tempFileRoot;
extern CFileTranslator *mtaFileRoot;
extern CFileTranslator *screenFileRoot;
extern CFileTranslator *dataFileRoot;
extern CFileTranslator *modFileRoot;
extern CFileTranslator *newsFileRoot;
extern CFileTranslator *gameFileRoot;

class CCore : public CCoreInterface, public CSingleton <CCore>
{
    friend class CModManager;
public:
                            CCore();
                            ~CCore();

    // Subsystems (query)
    eCoreVersion            GetVersion();
    CConsoleInterface*      GetConsole();
    CCommandsInterface*     GetCommands();
    inline CConnectManager* GetConnectManager()                                                 { return m_pConnectManager; };
    CGame*                  GetGame();
    CGUI*                   GetGUI();
    CGraphicsInterface*     GetGraphics();
    CModManager*            GetModManager();
    CServer*                GetServer()                                                         { return m_server; }
    CMultiplayer*           GetMultiplayer();
    CNet*                   GetNetwork();
    CXML*                   GetXML()                                                            { return m_pXML; };
    CXMLNode*               GetConfig();
    CClientVariables*       GetCVars()                                                          { return &m_ClientVariables; };
    CKeyBinds*              GetKeyBinds();
    CLocalGUI*              GetLocalGUI();
    CCommunityInterface*    GetCommunity()                                                      { return &m_Community; };
    CFileSystem*            GetFileSystem()                                                     { return m_fileSystem; };
    CJoystickManager*       GetJoystickManager()                                                { return m_joystick; };

    void                    SaveConfig();

    // Debug
    void                    DebugEcho( const char* szText );
    void                    DebugPrintf( const char* szFormat, ... );
    void                    SetDebugVisible( bool bVisible );
    bool                    IsDebugVisible();
    void                    DebugEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B );
    void                    DebugPrintfColor( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    DebugClear();

    // Chat
    void                    ChatEcho( const char* szText, bool bColorCoded = false );
    void                    ChatEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false );
    void                    ChatPrintf( const char* szFormat, bool bColorCoded, ... );
    void                    ChatPrintfColor( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... );
    void                    SetChatEnabled( bool enabled );
    bool                    IsChatEnabled() const;
    void                    SetChatVisible( bool bVisible );
    bool                    IsChatVisible();
    void                    EnableChatInput( char* szCommand, DWORD dwColor );
    bool                    IsChatInputEnabled();

    // Screenshots
    void                    TakeScreenShot();

    // GUI
    bool                    IsSettingsVisible();
    bool                    IsMenuVisible();
    bool                    IsCursorForcedVisible();
    bool                    IsCursorControlsToggled()                                           { return m_bCursorToggleControls; }
    void                    HideMainMenu();
    void                    HideQuickConnect();
    void                    SetCenterCursor( bool bEnabled );

    void                    ShowServerInfo( unsigned int WindowType );

    // Configuration
    void                    ApplyConsoleSettings();
    void                    ApplyGameSettings();
    void                    ApplyCommunityState();
    void                    UpdateRecentlyPlayed();

    // Net
    void                    SetConnected( bool bConnected );
    bool                    IsConnected();
    bool                    Reconnect( const char* szHost, unsigned short usPort, const char* szPassword, bool bSave = true );

    // Mod
    void                    SetOfflineMod( bool bOffline );
    void                    ForceCursorVisible( bool bVisible, bool bToggleControls = true );
    void                    SetMessageProcessor( pfnProcessMessage pfnMessageProcessor );
    void                    ShowMessageBox( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler = NULL );
    void                    RemoveMessageBox( bool bNextFrame = false );
    bool                    IsOfflineMod()                                                      { return m_bIsOfflineMod; }
    const char *            GetModInstallRoot( const char * szModName );
    CFileTranslator*        GetModRoot()                                                        { return m_modRoot; }
    CFileTranslator*        GetMTARoot()                                                        { return mtaFileRoot; }
    CFileTranslator*        GetDataRoot()                                                       { return dataFileRoot; }


    // Subsystems
    void                    CreateGame();
    void                    CreateMultiplayer();
    void                    CreateNetwork();
    void                    CreateXML();
    void                    InitGUI( IUnknown* pDevice );
    void                    CreateGUI();
    void                    DestroyGame();
    void                    DestroyMultiplayer();
    void                    DestroyNetwork();
    void                    DestroyXML();
    void                    DeinitGUI();
    void                    DestroyGUI();

    // Hooks
    void                    ApplyHooks();
    HWND                    GetHookedWindow();
    void                    SwitchRenderWindow( HWND hWnd, HWND hWndInput );
    void                    CallSetCursorPos( int X, int Y )                                    { m_pSetCursorPosHook->CallSetCursorPos(X,Y); }
    void                    SetClientMessageProcessor( pfnProcessMessage pfnMessageProcessor )  { m_pfnMessageProcessor = pfnMessageProcessor; };
    pfnProcessMessage       GetClientMessageProcessor()                                         { return m_pfnMessageProcessor; }
    void                    ChangeResolution( long width, long height, long depth );
    void                    ApplyLoadingCrashPatch();

    bool                    IsFocused()                                                         { return ( GetForegroundWindow ( ) == GetHookedWindow ( ) ); };
    bool                    IsWindowMinimized();

    // Pulse
    void                    DoPreFramePulse();
    void                    DoPostFramePulse();

    // Events
    bool                    OnMouseClick( CGUIMouseEventArgs Args );
    bool                    OnMouseDoubleClick( CGUIMouseEventArgs Args );
    void                    OnModUnload();
    void                    OnFocusLost();

    // Misc
    void                    RegisterCommands();
    bool                    IsValidNick( const char* szNick );     // Move somewhere else
    void                    Quit( bool bInstantly = true );
    void                    InitiateUpdate( const char* szType, const char* szData, const char* szHost )    { m_pLocalGUI->InitiateUpdate ( szType, szData, szHost ); }
    bool                    IsOptionalUpdateInfoRequired( const char* szHost )                  { return m_pLocalGUI->IsOptionalUpdateInfoRequired ( szHost ); }
    void                    InitiateDataFilesFix()                                              { m_pLocalGUI->InitiateDataFilesFix (); }

    uint                    GetFrameRateLimit()                                                 { return m_uiFrameRateLimit; }
    void                    RecalculateFrameRateLimit( uint uiServerFrameRateLimit = -1 );
    void                    ApplyFrameRateLimit( uint uiOverrideRate = -1 );
    void                    EnsureFrameRateLimitApplied();

    uint                    GetMinStreamingMemory();
    uint                    GetMaxStreamingMemory();

    SString                 GetConnectCommandFromURI( const char* szURI );  
    void                    GetConnectParametersFromURI( const char* szURI, std::string &strHost, unsigned short &usPort, std::string &strNick, std::string &strPassword );
    bool                    bScreenShot;

    typedef std::map <std::string, std::string> commandMap_t;

    commandMap_t&           GetCommandLineOptions()                                             { return m_CommandLineOptions; }
    const char *            GetCommandLineOption( const char* szOption );
    const char *            GetCommandLineArgs()                                                { return m_szCommandLineArgs; }
    void                    RequestNewNickOnStart()                                             { m_bWaitToSetNick = true; };

    //XFire
    SString                 UpdateXfire();
    void                    SetCurrentServer( in_addr Addr, unsigned short usQueryPort );
    void                    SetXfireData( std::string strServerName, std::string strVersion, bool bPassworded, std::string strGamemode, std::string strMap, std::string strPlayerName, std::string strPlayerCount );

    void                    OnPreHUDRender();
    void                    OnDeviceRestore();

    EDiagnosticDebugType    GetDiagnosticDebug();
    void                    SetDiagnosticDebug( EDiagnosticDebugType value );

private:
    // Core devices.
    CXML*                       m_pXML;
    CLocalGUI*                  m_pLocalGUI;
    CGraphics*                  m_pGraphics;
    CCommands*                  m_pCommands;
    CDirect3DData*              m_pDirect3DData;
    CConnectManager*            m_pConnectManager;
    CFileSystem*                m_fileSystem;
    CJoystickManager*           m_joystick;

    // Instances (put new classes here!)
    CXMLFile*                   m_pConfigFile;
    CClientVariables            m_ClientVariables;
    CCommunity                  m_Community;

    // Hook interfaces.
    CMessageLoopHook*           m_pMessageLoopHook;
    CDirectInputHookManager*    m_pDirectInputHookManager;
    CDirect3DHookManager*       m_pDirect3DHookManager;
    CSetCursorPosHook*          m_pSetCursorPosHook;
    CTCPManager*                m_pTCPManager;

    int                         m_iUnminimizeFrameCounter;
    bool                        m_bDidRecreateRenderTargets;

    // Module loader objects.
    CDynamicLibrary             m_GameModule;
    CDynamicLibrary             m_MultiplayerModule;
    CDynamicLibrary             m_NetModule;
    CDynamicLibrary             m_XMLModule;
    CDynamicLibrary             m_GUIModule;

    // Mod manager
    CModManager*                m_pModManager;
    CServer*                    m_server;
    CFileTranslator*            m_modRoot;

    // Module interfaces.
    CGame*                      m_pGame;
    CNet*                       m_pNet;
    CMultiplayer*               m_pMultiplayer;
    CGUI*                       m_pGUI;

    // Logger utility interface.
    CLogger*                    m_pLogger;

    CKeyBinds*                  m_pKeyBinds;

    bool                        m_bFirstFrame;
    bool                        m_bIsOfflineMod;
    bool                        m_bCursorToggleControls;
    pfnProcessMessage           m_pfnMessageProcessor;

    CGUIMessageBox*             m_pMessageBox;

    // screen res
    DEVMODE                     m_Current;

    //Xfire Update
    CXfireServerInfo*           m_pCurrentServer;
    time_t                      m_tXfireUpdate;

    filePath                    m_strModInstallRoot;

    bool                        m_bQuitOnPulse;
    bool                        m_bDestroyMessageBox;

    bool                        m_bDoneFrameRateLimit;
    unsigned int                m_uiServerFrameRateLimit;
    unsigned int                m_uiFrameRateLimit;
    double                      m_dLastTimeMs;
    double                      m_dPrevOverrun;
    bool                        m_bWaitToSetNick;
    unsigned int                m_uiNewNickWaitFrames;

    EDiagnosticDebugType        m_DiagnosticDebug;

    // Command line
    static void                 ParseCommandLine( std::map < std::string, std::string > & options, const char*& szArgs, const char** pszNoValOptions = NULL );
    commandMap_t                m_CommandLineOptions;     // e.g. "-o option" -> {"o" = "option"}
    const char*                 m_szCommandLineArgs;                // Everything that comes after the options
};

#endif

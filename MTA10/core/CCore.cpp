/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CCore.cpp
*  PURPOSE:     Base core class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <Accctrl.h>
#include <Aclapi.h>
#include "Userenv.h"        // This will enable SharedUtil::ExpandEnvString
#include "SharedUtil.hpp"
#include <clocale>

using namespace std;

static float fTest = 1;

extern CCore* g_pCore;
bool g_bBoundsChecker = false;
DWORD* g_Table = new DWORD[65535];
DWORD* g_TableSize = new DWORD[65535];
DWORD g_dwTable = 0;

// Global hooking instance.
#include "deviare_inproc/Include/NktHookLib.h"

CNktHookLib g_hookingLib;

// File access zones
CFileTranslator *tempFileRoot;
CFileTranslator *mtaFileRoot;
CFileTranslator *screenFileRoot;
CFileTranslator *dataFileRoot;
CFileTranslator *modFileRoot;
CFileTranslator *newsFileRoot;
CFileTranslator *gameFileRoot;

BOOL AC_RestrictAccess()
{
    EXPLICIT_ACCESS NewAccess;
    PACL pTempDacl;
    HANDLE hProcess;
    DWORD dwFlags;
    DWORD dwErr;

    ///////////////////////////////////////////////
    // Get the HANDLE to the current process.
    hProcess = GetCurrentProcess();

    ///////////////////////////////////////////////
    // Setup which accesses we want to deny.
    dwFlags = GENERIC_WRITE | PROCESS_ALL_ACCESS | WRITE_DAC | DELETE | WRITE_OWNER | READ_CONTROL;

    ///////////////////////////////////////////////
    // Build our EXPLICIT_ACCESS structure.
    BuildExplicitAccessWithName( &NewAccess, "CURRENT_USER", dwFlags, DENY_ACCESS, NO_INHERITANCE ); 


    ///////////////////////////////////////////////
    // Create our Discretionary Access Control List.
    if ( ERROR_SUCCESS != (dwErr = SetEntriesInAcl( 1, &NewAccess, NULL, &pTempDacl )) )
    {
#ifdef _DEBUG
//        pConsole->Con_Printf("Error at SetEntriesInAcl(): %i", dwErr);
#endif
        return FALSE;
    }

    ////////////////////////////////////////////////
    // Set the new DACL to our current process.
    if ( ERROR_SUCCESS != (dwErr = SetSecurityInfo( hProcess, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pTempDacl, NULL )) )
    {
#ifdef _DEBUG
//        pConsole->Con_Printf("Error at SetSecurityInfo(): %i", dwErr);
#endif
        return FALSE;
    }

    ////////////////////////////////////////////////
    // Free the DACL (see msdn on SetEntriesInAcl)
    LocalFree( pTempDacl );
    CloseHandle( hProcess );

    return TRUE;
}


CCore* CSingleton <CCore>::m_pSingleton = NULL;

CCore::CCore()
{
    // Initialize the global pointer
    g_pCore = this;

#if !defined(MTA_DEBUG) && !defined(MTA_ALLOW_DEBUG)
    AC_RestrictAccess ();
#endif

#if defined(_DEBUG) && 1
    while ( !IsDebuggerPresent() )
        Sleep( 1 );
#endif

    m_pConfigFile = NULL;

    // Set our locale to the C locale, except for character handling which is the system's default
    std::setlocale(LC_ALL,"C");
    std::setlocale(LC_CTYPE,"");

    // Parse the command line
    const char* pszNoValOptions[] =
    {
        "window",
        NULL
    };
    ParseCommandLine( m_CommandLineOptions, m_szCommandLineArgs, pszNoValOptions );

    // Create the file system
    {
        fs_construction_params constr_params;
        constr_params.nativeExecMan = NULL;     // we do not need threading support.

        m_fileSystem = CFileSystem::Create( constr_params );
    }

    char pathBuffer[1024];
    GetModuleFileName( NULL, pathBuffer, 1024 );

    SString mtaRoot = GetMTASABaseDir();

    // Add important access zones here + extern them
    filePath tempRoot = GetMTATempPath();
    CreateDirectory( tempRoot.c_str(), NULL );

    filePath newsRoot = GetMTADataPath() + "news/";
    CreateDirectory( newsRoot.c_str(), NULL );

    filePath screenRoot = filePath( mtaRoot + "screenshots/" );
    CreateDirectory( screenRoot.c_str(), NULL );

    tempFileRoot = m_fileSystem->CreateTranslator( tempRoot );
    mtaFileRoot = m_fileSystem->CreateTranslator( mtaRoot + "mta/" );
    screenFileRoot = m_fileSystem->CreateTranslator( screenRoot );
    dataFileRoot = m_fileSystem->CreateTranslator( GetMTADataPath() );
    modFileRoot = m_fileSystem->CreateTranslator( mtaRoot + "mods/" );
    newsFileRoot = m_fileSystem->CreateTranslator( newsRoot );
    gameFileRoot = m_fileSystem->CreateTranslator( pathBuffer );

    if ( !gameFileRoot )
    {
        MessageBox( NULL, "Could not bind GTA:SA root.", "Filesystem Error", MB_OK );

        TerminateProcess( GetCurrentProcess(), EXIT_FAILURE );
    }

    if ( !tempFileRoot || !mtaFileRoot || !dataFileRoot || !modFileRoot || !newsFileRoot )
    {
        MessageBox( NULL, "Your MTA:SA installation appears to be corrupted. Please reinstall!", "Filesystem Error", MB_OK );

        TerminateProcess( GetCurrentProcess(), EXIT_FAILURE );
    }

    // Create a logger instance.
    m_pLogger                   = new CLogger();

    // Create interaction objects.
    m_pCommands                 = new CCommands;
    m_pConnectManager           = new CConnectManager;
    m_joystick                  = new CJoystickManager;

    // Create the GUI manager and the graphics lib wrapper
    m_pLocalGUI                 = new CLocalGUI;
    m_pGraphics                 = new CGraphics( m_pLocalGUI );
    m_pGUI                      = NULL;

    // Misc utils
    CScreenShot::Init();

    // Create the mod manager
    m_pModManager               = new CModManager;
    m_server                    = new CServer( filePath( mtaRoot ) );

    m_pfnMessageProcessor       = NULL;
    m_pMessageBox = NULL;

    m_bFirstFrame = true;
    m_bIsOfflineMod = false;
    m_bQuitOnPulse = false;
    m_bDestroyMessageBox = false;
    m_bCursorToggleControls = false;
    m_bWaitToSetNick = false;

    // Initialize time
    CClientTime::InitializeTime();

    // Create our Direct3DData handler.
    m_pDirect3DData = new CDirect3DData;

    WriteDebugEvent( "CCore::CCore" );

    m_pKeyBinds = new CKeyBinds( this );

    // Create our hook objects.
    //m_pFileSystemHook           = new CFileSystemHook ( );
    m_pDirect3DHookManager      = new CDirect3DHookManager ( );
    m_pDirectInputHookManager   = new CDirectInputHookManager ( );
    m_pMessageLoopHook          = new CMessageLoopHook ( );
    m_pSetCursorPosHook         = new CSetCursorPosHook ( );
    m_pTCPManager               = new CTCPManager ( );

    // Register internal commands.
    RegisterCommands();

    // Setup our hooks.
    ApplyHooks();

    // Reset the screenshot flag
    bScreenShot = false;

    //Create our current server and set the update time to zero
    m_pCurrentServer = new CXfireServerInfo();
    m_tXfireUpdate = 0;

    // No initial fps limit
    m_bDoneFrameRateLimit = false;
    m_uiFrameRateLimit = 0;
    m_uiServerFrameRateLimit = 0;
    m_dLastTimeMs = 0;
    m_dPrevOverrun = 0;
    m_uiNewNickWaitFrames = 0;
    m_iUnminimizeFrameCounter = 0;
    m_bDidRecreateRenderTargets = false;
    m_modRoot = NULL;

    // Initialize core modules
    CCore::GetSingleton ( ).CreateNetwork ( );
    CCore::GetSingleton ( ).CreateGame ( );
    CCore::GetSingleton ( ).CreateMultiplayer ( );
}

CCore::~CCore()
{
    WriteDebugEvent ( "CCore::~CCore" );

    // Delete the mod manager
    delete m_pModManager;
    SAFE_DELETE ( m_pMessageBox );

    // Destroy early subsystems
    DestroyNetwork ();
    DestroyMultiplayer ();
    DestroyGame ();

    // Remove global events
    g_pCore->m_pGUI->ClearInputHandlers( INPUT_CORE );

    // Remove input hook
    CMessageLoopHook::GetSingleton ( ).RemoveHook ( );

    // Store core variables to cvars
    CVARS_SET ( "console_pos",                  m_pLocalGUI->GetConsole ()->GetPosition () );
    CVARS_SET ( "console_size",                 m_pLocalGUI->GetConsole ()->GetSize () );

    // Delete interaction objects.
    delete m_pCommands;
    delete m_pConnectManager;
    delete m_pDirect3DData;
    delete m_joystick;

    // Delete hooks.
    delete m_pSetCursorPosHook;
    //delete m_pFileSystemHook;
    delete m_pDirect3DHookManager;
    delete m_pDirectInputHookManager;
    delete m_pMessageLoopHook;
    delete m_pTCPManager;

    // Shutdown misc utils
    CScreenShot::Shutdown();

    // Delete the GUI manager    
    delete m_pLocalGUI;
    delete m_pGraphics;

    // Delete lazy subsystems
    DestroyGUI ();
    DestroyXML ();

    // Delete keybinds
    delete m_pKeyBinds;

    // Delete the logger
    delete m_pLogger;

    //Delete the Current Server
    delete m_pCurrentServer;

    // Free the file access zones
    delete tempFileRoot;
    delete mtaFileRoot;
    delete screenFileRoot;
    delete dataFileRoot;
    delete modFileRoot;
    delete newsFileRoot;
    delete gameFileRoot;
    
    CFileSystem::Destroy( m_fileSystem );
}

eCoreVersion CCore::GetVersion()
{
    return MTACORE_20;
}

CConsoleInterface* CCore::GetConsole()
{
    return m_pLocalGUI->GetConsole();
}

CCommandsInterface* CCore::GetCommands()
{
    return m_pCommands;
}

CGame* CCore::GetGame()
{
    return m_pGame;
}

CGraphicsInterface* CCore::GetGraphics()
{
    return m_pGraphics;
}

CModManager* CCore::GetModManager()
{
    return m_pModManager;
}

CMultiplayer* CCore::GetMultiplayer()
{
    return m_pMultiplayer;
}

CXMLNode* CCore::GetConfig()
{
    if ( !m_pConfigFile )
        return NULL;
    CXMLNode* pRoot = m_pConfigFile->GetRootNode ();
    if ( !pRoot )
        pRoot = m_pConfigFile->CreateRootNode ( CONFIG_ROOT );
    return pRoot;
}

CGUI* CCore::GetGUI()
{
    return m_pGUI;
}

CNet* CCore::GetNetwork()
{
    return m_pNet;
}

CKeyBinds* CCore::GetKeyBinds()
{
    return m_pKeyBinds;
}

CLocalGUI* CCore::GetLocalGUI()
{
    return m_pLocalGUI;
}

void CCore::SaveConfig()
{
    if ( m_pConfigFile )
    {
        CXMLNode* pBindsNode = GetConfig ()->FindSubNode ( CONFIG_NODE_KEYBINDS );

        if ( !pBindsNode )
            pBindsNode = GetConfig ()->CreateSubNode ( CONFIG_NODE_KEYBINDS );

        m_pKeyBinds->SaveToXML ( pBindsNode );
        GetVersionUpdater ()->SaveConfigToXML ();
        GetServerCache ()->SaveServerCache ();
        m_pConfigFile->Write ();
    }
}

void CCore::ChatEcho( const char* szText, bool bColorCoded )
{
    CChat* pChat = m_pLocalGUI->GetChat ();
    if ( pChat )
    {
        CColor color ( 255, 255, 255, 255 );
        pChat->SetTextColor ( color );
    }

    // Echo it to the console and chat
    m_pLocalGUI->EchoChat ( szText, bColorCoded );
    if ( bColorCoded )
    {
        std::string strWithoutColorCodes;
        CChatLine::RemoveColorCode ( szText, strWithoutColorCodes );
        m_pLocalGUI->EchoConsole ( strWithoutColorCodes.c_str () );
    }
    else
        m_pLocalGUI->EchoConsole ( szText );
}

void CCore::DebugEcho( const char* szText )
{
    CDebugView * pDebugView = m_pLocalGUI->GetDebugView ();
    if ( pDebugView )
    {
        CColor color ( 255, 255, 255, 255 );
        pDebugView->SetTextColor ( color );
    }

    m_pLocalGUI->EchoDebug ( szText );
}

void CCore::DebugPrintf( const char* szFormat, ... )
{
    // Convert it to a string buffer
    char szBuffer[1024];
    va_list ap;

    va_start( ap, szFormat );
    VSNPRINTF( szBuffer, 1024, szFormat, ap );
    va_end( ap );

    DebugEcho( szBuffer );
}

void CCore::SetDebugVisible( bool bVisible )
{
    if ( !m_pLocalGUI )
        return;

    m_pLocalGUI->SetDebugViewVisible( bVisible );
}

bool CCore::IsDebugVisible()
{
    if ( m_pLocalGUI )
        return m_pLocalGUI->IsDebugViewVisible();
    else
        return false;
}

void CCore::DebugEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B )
{
    // Set the color
    CDebugView * pDebugView = m_pLocalGUI->GetDebugView ();
    if ( pDebugView )
    {
        CColor color ( R, G, B, 255 );
        pDebugView->SetTextColor ( color );
    }

    m_pLocalGUI->EchoDebug ( szText );
}

void CCore::DebugPrintfColor( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... )
{
    // Convert it to a string buffer
    char szBuffer [1024];
    va_list ap;
    va_start ( ap, B );
    VSNPRINTF ( szBuffer, 1024, szFormat, ap );
    va_end ( ap );

    // Echo it to the console and chat
    DebugEchoColor ( szBuffer, R, G, B );
}

void CCore::DebugClear()
{
    CDebugView *pDebugView = m_pLocalGUI->GetDebugView();

    if ( !pDebugView )
        return;

    pDebugView->Clear();
}

void CCore::ChatEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded )
{
    // Set the color
    CChat* pChat = m_pLocalGUI->GetChat ();
    if ( pChat )
    {
        CColor color ( R, G, B, 255 );
        pChat->SetTextColor ( color );
    }

    // Echo it to the console and chat
    m_pLocalGUI->EchoChat ( szText, bColorCoded );

    if ( bColorCoded )
    {
        std::string strWithoutColorCodes;
        CChatLine::RemoveColorCode ( szText, strWithoutColorCodes );
        m_pLocalGUI->EchoConsole ( strWithoutColorCodes.c_str () );
    }
    else
        m_pLocalGUI->EchoConsole ( szText );
}

void CCore::ChatPrintf( const char* szFormat, bool bColorCoded, ... )
{
    // Convert it to a string buffer
    char szBuffer[1024];
    va_list ap;
    va_start( ap, bColorCoded );
    VSNPRINTF( szBuffer, 1024, szFormat, ap );
    va_end( ap );

    // Echo it to the console and chat
    ChatEcho( szBuffer, bColorCoded );
}

void CCore::ChatPrintfColor( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... )
{
    if ( !m_pLocalGUI )
        return;

    // Convert it to a string buffer
    char szBuffer [1024];
    va_list ap;

    va_start( ap, B );
    VSNPRINTF( szBuffer, 1024, szFormat, ap );
    va_end( ap );

    // Echo it to the console and chat
    ChatEchoColor( szBuffer, R, G, B, bColorCoded );
}

void CCore::SetChatEnabled( bool enabled )
{
    if ( !m_pLocalGUI )
        return;

    m_pLocalGUI->SetChatBoxEnabled( enabled );
}

bool CCore::IsChatEnabled() const
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->IsChatBoxEnabled();
}

void CCore::SetChatVisible( bool bVisible )
{
    if ( !m_pLocalGUI )
        return;

    m_pLocalGUI->SetChatBoxVisible( bVisible );
}

bool CCore::IsChatVisible()
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->IsChatBoxVisible ();
}

void CCore::TakeScreenShot()
{
    bScreenShot = true;
}

void CCore::EnableChatInput( char* szCommand, DWORD dwColor )
{
    if ( !m_pLocalGUI )
        return;

    if ( m_pGame->GetSystemState() == GS_PLAYING_GAME &&
        m_pModManager->GetCurrentMod() != NULL &&
        !IsOfflineMod() &&
        !m_pGame->IsAtMenu() &&
        !m_pLocalGUI->GetMainMenu()->IsVisible() &&
        !m_pLocalGUI->GetConsole()->IsVisible() &&
        !m_pLocalGUI->IsChatBoxInputEnabled() )
    {
        CChat* pChat = m_pLocalGUI->GetChat();
        pChat->SetCommand( szCommand );
        //pChat->SetInputColor( dwColor );
        m_pLocalGUI->SetChatBoxInputEnabled( true );
        m_pLocalGUI->SetVisibleWindows( true );
    }
}

bool CCore::IsChatInputEnabled()
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->IsChatBoxInputEnabled();
}

bool CCore::IsSettingsVisible()
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->GetMainMenu()->GetSettingsWindow()->IsVisible();
}

bool CCore::IsMenuVisible()
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->GetMainMenu()->IsVisible();
}

bool CCore::IsCursorForcedVisible()
{
    if ( !m_pLocalGUI )
        return false;

    return m_pLocalGUI->IsCursorForcedVisible();
}

void CCore::ApplyConsoleSettings()
{
    CVector2D vec;
    CConsole *pConsole = m_pLocalGUI->GetConsole();

    CVARS_GET( "console_pos", vec );
    pConsole->SetPosition( vec );
    CVARS_GET( "console_size", vec );
    pConsole->SetSize( vec );
}

void CCore::ApplyGameSettings()
{
    bool bval;
    int iVal;
    CControllerConfigManager *pController = m_pGame->GetControllerConfigManager ();

    CVARS_GET( "invert_mouse",     bval ); pController->SetMouseInverted ( bval );
    CVARS_GET( "fly_with_mouse",   bval ); pController->SetFlyWithMouse ( bval );
    CVARS_GET( "classic_controls", bval ); bval ? pController->SetInputType ( NULL ) : pController->SetInputType ( 1 );
    CVARS_GET( "async_loading",    iVal ); m_pGame->SetAsyncLoadingFromSettings ( iVal == 1, iVal == 2 );
    CVARS_GET( "volumetric_shadows", bval ); m_pGame->GetSettings ()->SetVolumetricShadowsEnabled ( bval );
    CVARS_GET( "aspect_ratio",     iVal ); m_pGame->GetSettings ()->SetAspectRatio ( (eAspectRatio)iVal );
}

void CCore::ApplyCommunityState()
{
    if ( !g_pCore->GetCommunity()->IsLoggedIn() )
        return;

    m_pLocalGUI->GetMainMenu()->GetSettingsWindow()->OnLoginStateChange( true );
}

void CCore::SetConnected( bool bConnected )
{
    m_pLocalGUI->GetMainMenu()->SetIsIngame( bConnected );
}

bool CCore::IsConnected()
{
    return m_pLocalGUI->GetMainMenu()->GetIsIngame();
}

bool CCore::Reconnect( const char* szHost, unsigned short usPort, const char* szPassword, bool bSave )
{
    return m_pConnectManager->Reconnect( szHost, usPort, szPassword, bSave );
}

void CCore::SetOfflineMod( bool bOffline )
{
    m_bIsOfflineMod = bOffline;
}

const char* CCore::GetModInstallRoot( const char* szModName )
{
    if ( !modFileRoot->GetFullPath( szModName, false, m_strModInstallRoot ) )
        return NULL;

    return m_strModInstallRoot.c_str();
}

void CCore::ForceCursorVisible( bool bVisible, bool bToggleControls )
{
    m_bCursorToggleControls = bToggleControls;
    m_pLocalGUI->ForceCursorVisible( bVisible );
}

void CCore::SetMessageProcessor( pfnProcessMessage pfnMessageProcessor )
{
    m_pfnMessageProcessor = pfnMessageProcessor;
}

void CCore::ShowMessageBox( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler )
{
    std::string curDir;

    if ( m_pMessageBox )
        delete m_pMessageBox;

    // Create the message box
    m_pMessageBox = m_pGUI->CreateMessageBox ( szTitle, szText, uiFlags );

    if ( ResponseHandler )
        m_pMessageBox->SetClickHandler( *ResponseHandler );

    // Make sure it doesn't auto-destroy, or we'll crash if the msgbox had buttons and the user clicks OK
    m_pMessageBox->SetAutoDestroy( false );
}

void CCore::RemoveMessageBox( bool bNextFrame )
{
    if ( bNextFrame )
    {
        m_bDestroyMessageBox = true;
        return;
    }
    else if ( !m_pMessageBox )
        return;

    delete m_pMessageBox;

    m_pMessageBox = NULL;
}

HWND CCore::GetHookedWindow()
{
    return CMessageLoopHook::GetSingleton().GetHookedWindowHandle();
}

void CCore::HideMainMenu()
{
    m_pLocalGUI->GetMainMenu()->SetVisible( false );
}

void CCore::HideQuickConnect()
{
    m_pLocalGUI->GetMainMenu()->GetQuickConnectWindow()->SetVisible( false );
}

void CCore::ShowServerInfo( unsigned int WindowType )
{
    RemoveMessageBox();
    CServerInfo::GetSingletonPtr()->Show( (eWindowType)WindowType );
}

void CCore::ApplyHooks()
{ 
    ApplyLoadingCrashPatch();

    // Create our hooks
    m_pDirectInputHookManager->ApplyHook();
    m_pDirect3DHookManager->ApplyHook();
    m_pSetCursorPosHook->ApplyHook();
}

void CCore::SetCenterCursor( bool bEnabled )
{
    if ( bEnabled )
        m_pSetCursorPosHook->EnableSetCursorPos ();
    else
        m_pSetCursorPosHook->DisableSetCursorPos ();
}

////////////////////////////////////////////////////////////////////////
//
// LoadModule
//
// Attempt to load a module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
void LoadModule( CDynamicLibrary& m_Loader, const SString& strName, const filePath& modPath )
{
    WriteDebugEvent( "Loading " + strName.ToLower () );

    // Load appropriate compilation-specific library
#ifdef MTA_DEBUG
    filePath strModuleFileName = modPath + "_d.dll";
#else
    filePath strModuleFileName = modPath + ".dll";
#endif

    filePath path;
    
    if ( !mtaFileRoot->GetFullPath( strModuleFileName.c_str(), true, path ) )
        return;

    try
    {
        m_Loader.Load( path.c_str() );
    }
    catch( std::exception& e )
    {
        MessageBox( 0, SString( "Error loading %s module! (%s)", *strName.ToLower(), e.what() ), "Error", MB_OK|MB_ICONEXCLAMATION );
        BrowseToSolution( modPath.convert_ansi() + "-not-loadable", true, true );
        return;
    }

    WriteDebugEvent( strName + " loaded." );
}


////////////////////////////////////////////////////////////////////////
//
// InitModule
//
// Attempt to initialize a loaded module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
template <class T, class U>
T* InitModule( CDynamicLibrary& m_Loader, const SString& strName, const char *init, U* pObj )
{
    // Get initializer function from DLL.
    typedef T* (*PFNINITIALIZER)( U* );
    PFNINITIALIZER pfnInit = (PFNINITIALIZER)m_Loader.GetProcedureAddress( init );

    if ( !pfnInit )
    {
        MessageBox( 0, strName + " module is incorrect!", "Error", MB_OK | MB_ICONEXCLAMATION );
        TerminateProcess( GetCurrentProcess(), EXIT_FAILURE );
    }

    // If we have a valid initializer, call it.
    T* pResult = pfnInit( pObj );

    WriteDebugEvent( strName + " initialized." );
    return pResult;
}

////////////////////////////////////////////////////////////////////////
//
// InitModuleEx
//
// Extended module initialization
//
////////////////////////////////////////////////////////////////////////
template <class T, class U>
T* InitModuleEx( CDynamicLibrary& m_Loader, const SString& strName, const char *init, U* pObj )
{
    // Get initializer function from DLL.
    typedef T* (*PFNINITIALIZER)( U*, CCoreInterface* );
    PFNINITIALIZER pfnInit = (PFNINITIALIZER)m_Loader.GetProcedureAddress( init );

    if ( !pfnInit )
    {
        MessageBox( 0, strName + " module is incorrect!", "Error", MB_OK | MB_ICONEXCLAMATION );
        TerminateProcess( GetCurrentProcess(), EXIT_FAILURE );
    }

    // If we have a valid initializer, call it.
    T* pResult = pfnInit( pObj, g_pCore );

    WriteDebugEvent( strName + " initialized." );
    return pResult;
}

////////////////////////////////////////////////////////////////////////
//
// CreateModule
//
// Attempt to load and initialize a module. Returns if successful.
// On failure, displays message box and terminates the current process.
//
////////////////////////////////////////////////////////////////////////
template <class T, class U>
T* CreateModule( CDynamicLibrary& m_Loader, const SString& strName, const filePath& path, const char *init, U* pObj )
{
    LoadModule( m_Loader, strName, path );
    return InitModule <T> ( m_Loader, strName, init, pObj );
}

void CCore::CreateGame()
{
    m_pGame = CreateModule <CGame> ( m_GameModule, "Game", filePath( "game_sa" ), "GetGameInterface", this );

    if ( m_pGame->GetGameVersion () >= VERSION_11 )
    {
        MessageBox( 0, "Only GTA:SA version 1.0 is supported! You are now being redirected to a page where you can patch your version.", "Error", MB_OK | MB_ICONEXCLAMATION );
        BrowseToSolution( "downgrade" );
        TerminateProcess( GetCurrentProcess(), 1 );
    }
}

void CCore::CreateMultiplayer()
{
    m_pMultiplayer = CreateModule <CMultiplayer> ( m_MultiplayerModule, "Multiplayer", filePath( "multiplayer_sa" ), "InitMultiplayerInterface", this );
    m_pGame->RegisterMultiplayer( m_pMultiplayer );
}

void CCore::DeinitGUI()
{
}

void CCore::InitGUI( IUnknown* pDevice )
{
    IDirect3DDevice9 *dev = (IDirect3DDevice9*)pDevice;
    m_pGUI = InitModuleEx <CGUI> ( m_GUIModule, "GUI", "InitGUIInterface", dev );

    // set the screenshot path to this default library
    filePath screenshots;
    mtaFileRoot->GetFullPath( "../", false, screenshots );

    std::string ansiScreenshots = screenshots.convert_ansi();

    CVARS_SET( "screenshot_path", SString( ansiScreenshots ) );
    CScreenShot::SetPath( ansiScreenshots.c_str() );
}

void CCore::CreateGUI()
{
    try
    {
        LoadModule( m_GUIModule, "GUI", filePath( "cgui" ) );
    }
    catch( std::exception& )
    {
        MessageBox( NULL, "Could not initialize the graphical user interface! Reinstall MTA please.", "CGUI Not Found", MB_OK );
        TerminateProcess( GetCurrentProcess(), EXIT_FAILURE );
    }
}

void CCore::DestroyGUI()
{
    WriteDebugEvent ( "CCore::DestroyGUI" );

    if ( m_pGUI )
    {
        delete m_pGUI;
        m_pGUI = NULL;
    }

    m_GUIModule.Unload();
}

void CCore::CreateNetwork()
{
    m_pNet = CreateModule <CNet> ( m_NetModule, "Network", filePath( "netc" ), "InitNetInterface", this );

    // Network module compatibility check
    typedef unsigned long (*PFNCHECKCOMPATIBILITY) ( unsigned long );
    PFNCHECKCOMPATIBILITY pfnCheckCompatibility = (PFNCHECKCOMPATIBILITY)m_NetModule.GetProcedureAddress( "CheckCompatibility" );

    if ( !pfnCheckCompatibility || !pfnCheckCompatibility( MTA_DM_CLIENT_NET_MODULE_VERSION ) )
    {
        // net.dll doesn't like our version number
        MessageBox( 0, "Network module not compatible!", "Error", MB_OK|MB_ICONEXCLAMATION );
        BrowseToSolution( "netc-not-compatible", true );

        TerminateProcess( GetCurrentProcess (), EXIT_FAILURE );
    }

    // Set mta version for report log here
    SetApplicationSetting( "mta-version-ext", 
        SString( "%d.%d.%d-%d.%05d.%d.%03d",
            MTASA_VERSION_MAJOR,
            MTASA_VERSION_MINOR,
            MTASA_VERSION_MAINTENANCE,
            MTASA_VERSION_TYPE,
            MTASA_VERSION_BUILD,
            m_pNet->GetNetRev(),
            m_pNet->GetNetRel()
        )
    );

    // Get our unique hardware serial
    char szSerial[64];
    m_pNet->GetSerial( szSerial, sizeof(szSerial) );

    SetApplicationSetting( "serial", szSerial );
}

void CCore::CreateXML()
{
    m_pXML = CreateModule <CXML> ( m_XMLModule, "XML", filePath( "xmll" ), "InitXMLInterface", this );

    filePath configPath;
    mtaFileRoot->GetFullPath( "coreconfig.xml", true, configPath );
   
    // Load config XML file
    m_pConfigFile = m_pXML->CreateXML( configPath.c_str() );

    assert( m_pConfigFile );

    m_pConfigFile->Parse();

    // Load the keybinds (loads defaults if the subnode doesn't exist)
    GetKeyBinds()->LoadFromXML( GetConfig()->FindSubNode( CONFIG_NODE_KEYBINDS ) );

    // Load the default commandbinds if not exist
    GetKeyBinds()->LoadDefaultCommands( false );

    // Load XML-dependant subsystems
    m_ClientVariables.Load ( );
}

void CCore::DestroyGame()
{
    WriteDebugEvent( "CCore::DestroyGame" );

    if ( m_pGame )
    {
        delete m_pGame;
        m_pGame = NULL;
    }

    m_GameModule.Unload();
}

void CCore::DestroyMultiplayer()
{
    WriteDebugEvent( "CCore::DestroyMultiplayer" );

    // Unregister from modules.
    m_pGame->UnregisterMultiplayer( m_pMultiplayer );

    if ( m_pMultiplayer )
        m_pMultiplayer = NULL;

    m_MultiplayerModule.Unload();
}

void CCore::DestroyXML()
{
    WriteDebugEvent( "CCore::DestroyXML" );

    // Save and unload configuration
    if ( m_pConfigFile )
    {
        SaveConfig ();
        delete m_pConfigFile;
    }

    if ( m_pXML )
    {
        m_pXML = NULL;
    }

    m_XMLModule.Unload();
}

void CCore::DestroyNetwork()
{
    WriteDebugEvent( "CCore::DestroyNetwork" );

    if ( m_pNet )
    {
        m_pNet = NULL;
    }

    m_NetModule.Unload();
}

bool CCore::IsWindowMinimized()
{
    return IsIconic( GetHookedWindow() ) != FALSE;
}

void CCore::DoPreFramePulse()
{
    m_pKeyBinds->DoPreFramePulse();

    // Pulse game
    m_pGame->OnPreFrame();

    // Output server messages
    m_server->DoPulse();

    // Notify the mod manager
    m_pModManager->DoPulsePreFrame();  

    m_pLocalGUI->DoPulse();
}

void CCore::DoPostFramePulse()
{
    if ( m_bQuitOnPulse )
        Quit ();

    if ( m_bDestroyMessageBox )
    {
        RemoveMessageBox ();
        m_bDestroyMessageBox = false;
    }

    static bool bFirstPulse = true;
    if ( bFirstPulse )
    {
        bFirstPulse = false;

        // Apply all settings
        ApplyConsoleSettings();
        ApplyGameSettings();

        m_pGUI->SetMouseClickHandler( INPUT_CORE, GUI_CALLBACK_MOUSE( &CCore::OnMouseClick, this ) );
        m_pGUI->SetMouseDoubleClickHandler( INPUT_CORE, GUI_CALLBACK_MOUSE( &CCore::OnMouseDoubleClick, this ) );
        m_pGUI->SelectInputHandlers( INPUT_CORE );

        m_Community.Initialize ();
    }

    switch( m_pGame->GetSystemState() )
    {
    case GS_INIT_ONCE:
        WatchDogCompletedSection( "L2" );      // gta_sa.set seems ok
        break;
    case GS_FRONTEND:
        if ( m_pGame->HasCreditScreenFadedOut() )
        {
            if ( m_bFirstFrame )
            {
                m_bFirstFrame = false;

                // Disable vsync while it's all dark
                m_pGame->DisableVSync();

                // Parse the command line
                // Does it begin with mtasa://?
                if ( m_szCommandLineArgs && strnicmp( m_szCommandLineArgs, "mtasa://", 8 ) == 0 )
                {
                    SString strArguments = GetConnectCommandFromURI ( m_szCommandLineArgs );

                    // Run the connect command
                    if ( !strArguments.empty() && !m_pCommands->Execute( strArguments ) )
                        ShowMessageBox( "Error", "Error executing URL", MB_BUTTON_OK | MB_ICON_ERROR );
                }
                else
                {
                    // We want to load a mod?
                    const char* szOptionValue;
                    if ( szOptionValue = GetCommandLineOption( "l" ) )
                    {
                        // Try to load the mod
                        if ( !m_pModManager->Load ( szOptionValue, m_szCommandLineArgs ) )
                        {
                            SString strTemp ( "Error running mod specified in command line ('%s')", szOptionValue );
                            ShowMessageBox ( "Error", strTemp, MB_BUTTON_OK | MB_ICON_ERROR );
                        }
                    }
                    // We want to connect to a server?
                    else if ( szOptionValue = GetCommandLineOption ( "c" ) )
                    {
                        CCommandFuncs::Connect ( szOptionValue );
                    }
                }
            }
        }

        if ( m_bWaitToSetNick && GetLocalGUI()->GetMainMenu()->IsVisible() && !GetLocalGUI()->GetMainMenu()->IsFading() )
        {
            if ( m_uiNewNickWaitFrames > 75 )
            {
                // Request a new nickname if we're waiting for one
                GetLocalGUI()->GetMainMenu()->GetSettingsWindow()->RequestNewNickname();

                m_bWaitToSetNick = false;
            }
            else
                m_uiNewNickWaitFrames++;
        }
        break;
    }

    GetJoystickManager()->DoPulse();      // Note: This may indirectly call CMessageLoopHook::ProcessMessage
    m_pKeyBinds->DoPostFramePulse ();

    // Pulse game
    m_pGame->OnFrame();

    // Notify the mod manager and the connect manager
    m_pModManager->DoPulsePostFrame();
    m_pConnectManager->DoPulse();

    m_Community.DoPulse();

    // XFire polling
    if ( IsConnected() )
    {
        time_t ttime;
        ttime = time ( NULL );

        if ( ttime >= m_tXfireUpdate + XFIRE_UPDATE_RATE )
        {
            if ( m_pCurrentServer->IsSocketClosed() )
            {
                // Init our socket
                m_pCurrentServer->Init();
            }

            // Get our xfire query reply
            SString strReply = UpdateXfire( );

            // If we Parsed or if the reply failed wait another XFIRE_UPDATE_RATE until trying again
            if ( strReply == "ParsedQuery" || strReply == "NoReply" ) 
            {
                m_tXfireUpdate = time ( NULL );
                // Close the socket
                m_pCurrentServer->SocketClose();
            }
        }
    }
    else
    {
        // Set our update time to zero to ensure that the first xfire update happens instantly when joining
        XfireSetCustomGameData ( 0, NULL, NULL );

        if ( m_tXfireUpdate != 0 )
            m_tXfireUpdate = 0;
    }
}

// Called after MOD is unloaded
void CCore::OnModUnload()
{
    // reattach the global event
    m_pGUI->SelectInputHandlers( INPUT_CORE );
    // remove unused events
    m_pGUI->ClearInputHandlers( INPUT_MOD );

    // Ensure all these have been removed
    m_pKeyBinds->RemoveAllFunctions();
    m_pKeyBinds->RemoveAllControlFunctions();
}

void CCore::OnFocusLost()
{
    // Fix for #4948
    m_pKeyBinds->CallAllGTAControlBinds( CONTROL_BOTH, false );
}

void CCore::RegisterCommands()
{
    //m_pCommands->Add ( "e", CCommandFuncs::Editor );
    //m_pCommands->Add ( "clear", CCommandFuncs::Clear );
    m_pCommands->Add ( "help",              "this help screen",                 CCommandFuncs::Help );
    m_pCommands->Add ( "exit",              "exits the application",            CCommandFuncs::Exit );
    m_pCommands->Add ( "quit",              "exits the application",            CCommandFuncs::Exit );
    m_pCommands->Add ( "ver",               "shows the version",                CCommandFuncs::Ver );
    m_pCommands->Add ( "time",              "shows the time",                   CCommandFuncs::Time );
    m_pCommands->Add ( "showhud",           "shows the hud",                    CCommandFuncs::HUD );
    m_pCommands->Add ( "binds",             "shows all the binds",              CCommandFuncs::Binds );

#if 0
    m_pCommands->Add ( "vid",               "changes the video settings (id)",  CCommandFuncs::Vid );
    m_pCommands->Add ( "window",            "enter/leave windowed mode",        CCommandFuncs::Window );
    m_pCommands->Add ( "load",              "loads a mod (name args)",          CCommandFuncs::Load );
    m_pCommands->Add ( "unload",            "unloads a mod (name)",             CCommandFuncs::Unload );
#endif

    m_pCommands->Add ( "connect",           "connects to a server (host port nick pass)",   CCommandFuncs::Connect );
    m_pCommands->Add ( "reconnect",         "connects to a previous server",    CCommandFuncs::Reconnect );
    m_pCommands->Add ( "bind",              "binds a key (key control)",        CCommandFuncs::Bind );
    m_pCommands->Add ( "unbind",            "unbinds a key (key)",              CCommandFuncs::Unbind );
    m_pCommands->Add ( "copygtacontrols",   "copies the default gta controls",  CCommandFuncs::CopyGTAControls );
    m_pCommands->Add ( "screenshot",        "outputs a screenshot",             CCommandFuncs::ScreenShot );
    m_pCommands->Add ( "saveconfig",        "immediately saves the config",     CCommandFuncs::SaveConfig );

    m_pCommands->Add ( "cleardebug",        "clears the debug view",            CCommandFuncs::DebugClear );
    m_pCommands->Add ( "chatscrollup",      "scrolls the chatbox upwards",      CCommandFuncs::ChatScrollUp );
    m_pCommands->Add ( "chatscrolldown",    "scrolls the chatbox downwards",    CCommandFuncs::ChatScrollDown );
    m_pCommands->Add ( "debugscrollup",     "scrolls the debug view upwards",   CCommandFuncs::DebugScrollUp );
    m_pCommands->Add ( "debugscrolldown",   "scrolls the debug view downwards", CCommandFuncs::DebugScrollDown );

    m_pCommands->Add ( "test",              "",                                 CCommandFuncs::Test );
}

void CCore::SwitchRenderWindow( HWND hWnd, HWND hWndInput )
{
    assert ( 0 );
#if 0
    // Make GTA windowed
    m_pGame->GetSettings()->SetCurrentVideoMode(0);

    // Get the destination window rectangle
    RECT rect;
    GetWindowRect ( hWnd, &rect );

    // Size the GTA window size to the same size as the destination window rectangle
    HWND hDeviceWindow = CDirect3DData::GetSingleton ().GetDeviceWindow ();
    MoveWindow ( hDeviceWindow,
                 0,
                 0,
                 rect.right - rect.left,
                 rect.bottom - rect.top,
                 TRUE );

    // Turn the GTA window into a child window of our static render container window
    SetParent ( hDeviceWindow, hWnd );
    SetWindowLong ( hDeviceWindow, GWL_STYLE, WS_VISIBLE | WS_CHILD );
#endif
}


bool CCore::IsValidNick( const char* szNick )
{
    // Too long or too short?
    size_t sizeNick = strlen ( szNick );
    if ( sizeNick >= MIN_PLAYER_NICK_LENGTH && sizeNick <= MAX_PLAYER_NICK_LENGTH )
    {
        // Check each character
        for ( unsigned int i = 0; i < sizeNick; i++ )
        {
            // Don't allow 0x0A, 0x0D and <space>
            if ( szNick [i] == 0x0A ||
                 szNick [i] == 0x0D ||
                 szNick [i] == ' ' )
            {
                return false;
            }
        }

        return true;
    }

    return false;
}


void CCore::Quit( bool bInstantly )
{
    if ( bInstantly )
    {
        // Show that we are quiting (for the crash dump filename)
        SetApplicationSettingInt ( "last-server-ip", 1 );

        // Destroy the client
        CModManager::GetSingleton ().Unload ();

        // Destroy ourself
        delete CCore::GetSingletonPtr ();

        // Use TerminateProcess for now as exiting the normal way crashes
        TerminateProcess ( GetCurrentProcess (), 0 );
        //PostQuitMessage ( 0 );
    }
    else
    {
        m_bQuitOnPulse = true;
    }
}


bool CCore::OnMouseClick( CGUIMouseEventArgs Args )
{
    bool bHandled = false;

    bHandled = m_pLocalGUI->GetMainMenu ()->GetServerBrowser ()->OnMouseClick ( Args );     // CServerBrowser

    return bHandled;
}


bool CCore::OnMouseDoubleClick( CGUIMouseEventArgs Args )
{
    // Call the event handlers, where necessary
    return
        m_pLocalGUI->GetMainMenu()->GetSettingsWindow()->OnMouseDoubleClick( Args ) |    // CSettings
        m_pLocalGUI->GetMainMenu()->GetServerBrowser()->OnMouseDoubleClick( Args );      // CServerBrowser
}

void CCore::ParseCommandLine( std::map < std::string, std::string > & options, const char*& szArgs, const char** pszNoValOptions )
{
    std::set <std::string> noValOptions;

    if ( pszNoValOptions )
    {
        while ( *pszNoValOptions )
        {
            noValOptions.insert ( *pszNoValOptions );
            pszNoValOptions++;
        }
    }

    const char* szCmdLine = GetCommandLine ();
    char szCmdLineCopy[512];
    strncpy ( szCmdLineCopy, szCmdLine, sizeof(szCmdLineCopy) );
    
    char* pCmdLineEnd = szCmdLineCopy + strlen ( szCmdLineCopy );
    char* pStart = szCmdLineCopy;
    char* pEnd = pStart;
    bool bInQuoted = false;
    std::string strKey;
    szArgs = NULL;

    while ( pEnd != pCmdLineEnd )
    {
        pEnd = strchr ( pEnd + 1, ' ' );
        if ( !pEnd )
            pEnd = pCmdLineEnd;
        if ( bInQuoted && *(pEnd - 1) == '"' )
            bInQuoted = false;
        else if ( *pStart == '"' )
            bInQuoted = true;

        if ( !bInQuoted )
        {
            *pEnd = 0;
            if ( strKey.empty () )
            {
                if ( *pStart == '-' )
                {
                    strKey = pStart + 1;
                    if ( noValOptions.find ( strKey ) != noValOptions.end () )
                    {
                        options [ strKey ] = "";
                        strKey.clear ();
                    }
                }
                else
                {
                    szArgs = pStart - szCmdLineCopy + szCmdLine;
                    break;
                }
            }
            else
            {
                if ( *pStart == '-' )
                {
                    options [ strKey ] = "";
                    strKey = pStart + 1;
                }
                else
                {
                    if ( *pStart == '"' )
                        pStart++;
                    if ( *(pEnd - 1) == '"' )
                        *(pEnd - 1) = 0;
                    options [ strKey ] = pStart;
                    strKey.clear ();
                }
            }
            pStart = pEnd;
            while ( pStart != pCmdLineEnd && *(++pStart) == ' ' );
            pEnd = pStart;
        }
    }
}

const char* CCore::GetCommandLineOption( const char* szOption )
{
    std::map < std::string, std::string >::iterator it = m_CommandLineOptions.find( szOption );

    if ( it == m_CommandLineOptions.end () )
        return NULL;

    return it->second.c_str ();
}

SString CCore::GetConnectCommandFromURI( const char* szURI )
{
    unsigned short usPort;
    std::string strHost, strNick, strPassword;
    GetConnectParametersFromURI ( szURI, strHost, usPort, strNick, strPassword );

    // Generate a string with the arguments to send to the mod IF we got a host
    SString strDest;
    if ( strHost.size() > 0 )
    {
        if ( strPassword.size() > 0 )
            strDest.Format ( "connect %s %u %s %s", strHost.c_str (), usPort, strNick.c_str (), strPassword.c_str () );
        else
            strDest.Format ( "connect %s %u %s", strHost.c_str (), usPort, strNick.c_str () );
    }

    return strDest;
}

void CCore::GetConnectParametersFromURI( const char* szURI, std::string &strHost, unsigned short &usPort, std::string &strNick, std::string &strPassword )
{
    // Grab the length of the string
    size_t sizeURI = strlen ( szURI );
    
    // Parse it right to left
    char szLeft [256];
    szLeft [255] = 0;
    char* szLeftIter = szLeft + 255;

    char szRight [256];
    szRight [255] = 0;
    char* szRightIter = szRight + 255;

    const char* szIterator = szURI + sizeURI;
    bool bHitAt = false;

    for ( ; szIterator >= szURI + 8; szIterator-- )
    {
        if ( !bHitAt && *szIterator == '@' )
        {
            bHitAt = true;
        }
        else
        {
            if ( bHitAt )
            {
                if ( szLeftIter > szLeft )
                {
                    *(--szLeftIter) = *szIterator;
                }
            }
            else
            {
                if ( szRightIter > szRight )
                {
                    *(--szRightIter) = *szIterator;
                }
            }
        }
    }

    // Parse the host/port
    char szHost [64];
    char szPort [12];
    char* szHostIter = szHost;
    char* szPortIter = szPort;
    memset ( szHost, 0, sizeof(szHost) );
    memset ( szPort, 0, sizeof(szPort) );

    bool bIsInPort = false;
    size_t sizeRight = strlen ( szRightIter );
    for ( size_t i = 0; i < sizeRight; i++ )
    {
        if ( !bIsInPort && szRightIter [i] == ':' )
        {
            bIsInPort = true;
        }
        else
        {
            if ( bIsInPort )
            {
                if ( szPortIter < szPort + 11 )
                {
                    *(szPortIter++) = szRightIter [i];
                }
            }
            else
            {
                if ( szHostIter < szHost + 63 )
                {
                    *(szHostIter++) = szRightIter [i];
                }
            }
        }

    }

    // Parse the nickname / password
    char szNickname [64];
    char szPassword [64];
    char* szNicknameIter = szNickname;
    char* szPasswordIter = szPassword;
    memset ( szNickname, 0, sizeof(szNickname) );
    memset ( szPassword, 0, sizeof(szPassword) );

    bool bIsInPassword = false;
    size_t sizeLeft = strlen ( szLeftIter );
    for ( size_t i = 0; i < sizeLeft; i++ )
    {
        if ( !bIsInPassword && szLeftIter [i] == ':' )
        {
            bIsInPassword = true;
        }
        else
        {
            if ( bIsInPassword )
            {
                if ( szPasswordIter < szPassword + 63 )
                {
                    *(szPasswordIter++) = szLeftIter [i];
                }
            }
            else
            {
                if ( szNicknameIter < szNickname + 63 )
                {
                    *(szNicknameIter++) = szLeftIter [i];
                }
            }
        }

    }

    // If we got any port, convert it to an integral type
    usPort = 22003;
    if ( strlen ( szPort ) > 0 )
    {
        usPort = static_cast < unsigned short > ( atoi ( szPort ) );
    }

    // Grab the nickname
    if ( strlen ( szNickname ) > 0 )
    {
        strNick = szNickname;
    }
    else
    {
        CVARS_GET ( "nick", strNick );
    }
    strHost = szHost;
    strPassword = szPassword;
}


void CCore::UpdateRecentlyPlayed()
{
    in_addr Address;

    //Get the current host and port
    unsigned int uiPort;
    std::string strHost;

    CVARS_GET ( "host", strHost );
    CVARS_GET ( "port", uiPort );

    // Save the connection details into the recently played servers list
    if ( CServerListItem::Parse ( strHost.c_str(), Address ) )
    {
        CServerBrowser* pServerBrowser = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetServerBrowser ();
        CServerList* pRecentList = pServerBrowser->GetRecentList ();

        pRecentList->Remove ( Address, uiPort + SERVER_LIST_QUERY_PORT_OFFSET );
        pRecentList->AddUnique ( Address, uiPort + SERVER_LIST_QUERY_PORT_OFFSET, true );
       
        pServerBrowser->SaveRecentlyPlayedList();

        if ( !m_pConnectManager->m_strLastPassword.empty() )
            pServerBrowser->SetServerPassword ( strHost + ":" + SString("%u",uiPort), m_pConnectManager->m_strLastPassword );

    }

    //Save our configuration file
    CCore::GetSingleton ().SaveConfig ();
}
void CCore::SetCurrentServer( in_addr Addr, unsigned short usQueryPort )
{
    //Set the current server info so we can query it with ASE for xfire
    m_pCurrentServer->Address = Addr;
    m_pCurrentServer->usQueryPort = usQueryPort;

}
SString CCore::UpdateXfire()
{
    // Check if a current server exists
    if ( !m_pCurrentServer )
        return "";

    // Get the result from the Pulse method
    std::string strResult = m_pCurrentServer->Pulse();

    // Have we parsed the query this function call?
    if ( strResult == "ParsedQuery" )
    {
        // Get our Nick from CVARS
        std::string strNick;

        CVARS_GET ( "nick", strNick );

        // Format a player count
        SString strPlayerCount("%i / %i", m_pCurrentServer->nPlayers, m_pCurrentServer->nMaxPlayers);

        // Set as our custom date
        SetXfireData( m_pCurrentServer->strName, m_pCurrentServer->strVersion, m_pCurrentServer->bPassworded, m_pCurrentServer->strGameMode, m_pCurrentServer->strMap, strNick, strPlayerCount );
    }
    // Return the result
    return strResult;
}

void CCore::SetXfireData ( std::string strServerName, std::string strVersion, bool bPassworded, std::string strGamemode, std::string strMap, std::string strPlayerName, std::string strPlayerCount )
{
    if ( !XfireIsLoaded() )
        return;

    // Set our "custom data"
    const char *szKey[7], *szValue[7];
    szKey[0] = "Server Name";
    szValue[0] = strServerName.c_str();

    szKey[1] = "Server Version";
    szValue[1] = strVersion.c_str();

    szKey[2] = "Passworded";
    szValue[2] = bPassworded ? "Yes" : "No";

    szKey[3] = "Gamemode";
    szValue[3] = strGamemode.c_str();

    szKey[4] = "Map";
    szValue[4] = strMap.c_str();

    szKey[5] = "Player Name";
    szValue[5] = strPlayerName.c_str();

    szKey[6] = "Player Count";
    szValue[6] = strPlayerCount.c_str();
    
    XfireSetCustomGameData ( 7, szKey, szValue );
}


//
// Patch to fix loading crash.
// Has to be done before the main window is created.
//
void CCore::ApplyLoadingCrashPatch()
{
    uchar* pAddress;

    if ( *(WORD *)0x748ADD == 0x53FF )
        pAddress = (uchar*)0x7468F9;    // US
    else
        pAddress = (uchar*)0x746949;    // EU

    uchar ucOldValue = 0xB7;
    uchar ucNewValue = 0x39;

    MEMORY_BASIC_INFORMATION info;
    VirtualQuery( pAddress, &info, sizeof(MEMORY_BASIC_INFORMATION) );

    if ( info.State == MEM_COMMIT && info.Protect & ( PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_READONLY | PAGE_READWRITE ) )
    {
        if ( pAddress[0] == ucOldValue )
        {
            DWORD dwOldProtect;
            if ( VirtualProtect( pAddress, 1, PAGE_READWRITE, &dwOldProtect ) )
            {
                pAddress[0] = ucNewValue;
                VirtualProtect( pAddress, 1, dwOldProtect, &dwOldProtect );
            }
        }
    }
}


//
// Recalculate FPS limit to use
//
// Uses client rate from config
// Uses server rate from argument, or last time if not supplied
//
void CCore::RecalculateFrameRateLimit( unsigned int serverFPS )
{
    // Save rate from server if valid
    if ( serverFPS != -1 )
        m_uiServerFrameRateLimit = serverFPS;

    // Fetch client setting
    uint uiClientRate;
    g_pCore->GetCVars()->Get( "fps_limit", uiClientRate );

    // Lowest wins (Although zero is highest)
    if ( ( m_uiServerFrameRateLimit > 0 && uiClientRate > m_uiServerFrameRateLimit ) || uiClientRate == 0 )
        m_uiFrameRateLimit = m_uiServerFrameRateLimit;
    else
        m_uiFrameRateLimit = uiClientRate;

    // Print new limits to the console
    SString strStatus( "Server FPS limit: %d", m_uiServerFrameRateLimit );

    if ( m_uiFrameRateLimit != m_uiServerFrameRateLimit )
        strStatus += SString(  " (Using %d)", m_uiFrameRateLimit );

    CCore::GetSingleton().GetConsole ()->Print( strStatus );
}

//
// Make sure the frame rate limit has been applied since the last call
//
void CCore::EnsureFrameRateLimitApplied ( void )
{
    if ( !m_bDoneFrameRateLimit )
        ApplyFrameRateLimit ();

    m_bDoneFrameRateLimit = false;
}

//
// Do FPS limiting
//
// This is called once a frame
//
void CCore::ApplyFrameRateLimit ( uint uiOverrideRate )
{
    // Non frame rate limit stuff
    if ( IsWindowMinimized () )
        m_iUnminimizeFrameCounter = 4;     // Tell script we have unminimized after a short delay

    // Frame rate limit stuff starts here
    m_bDoneFrameRateLimit = true;

    uint uiUseRate = uiOverrideRate != -1 ? uiOverrideRate : m_uiFrameRateLimit;

    if ( uiUseRate < 1 )
        return;

    // Calc required time in ms between frames
    const double dTargetTimeToUse = 1000.0 / uiUseRate;

    // Time now
    double dTimeMs = CClientTime::GetTimeNano() * 1000.0;

    // Get delta time in ms since last frame
    double dTimeUsed = dTimeMs - m_dLastTimeMs;

    // Apply any over/underrun carried over from the previous frame
    dTimeUsed += m_dPrevOverrun;

    if ( dTimeUsed < dTargetTimeToUse )
    {
        // Have time spare - maybe eat some of that now
        double dSpare = dTargetTimeToUse - dTimeUsed;

        double dUseUpNow = dSpare - dTargetTimeToUse * 0.2f;

        if ( dUseUpNow >= 1 )
            Sleep( static_cast < DWORD > ( floor ( dUseUpNow ) ) );

        // Redo timing calcs
        dTimeMs = CClientTime::GetTimeNano() * 1000.0;
        dTimeUsed = dTimeMs - m_dLastTimeMs;
        dTimeUsed += m_dPrevOverrun;
    }

    // Update over/underrun for next frame
    m_dPrevOverrun = dTimeUsed - dTargetTimeToUse;

    // Limit carry over
    m_dPrevOverrun = Clamp( dTargetTimeToUse * -0.9f, m_dPrevOverrun, dTargetTimeToUse * 0.1f );

    m_dLastTimeMs = dTimeMs;
}

//
// OnDeviceRestore
//
void CCore::OnDeviceRestore()
{
    m_iUnminimizeFrameCounter = 4;     // Tell script we have restored after 4 frames to avoid double sends
    m_bDidRecreateRenderTargets = true;
}

//
// OnPreHUDRender
//
void CCore::OnPreHUDRender()
{
    IDirect3DDevice9* pDevice = CGraphics::GetSingleton ().GetDevice ();

    // Create a state block.
    IDirect3DStateBlock9 *pDeviceState = NULL;
    pDevice->CreateStateBlock( D3DSBT_ALL, &pDeviceState );
    D3DXMATRIX matProjection;
    pDevice->GetTransform( D3DTS_PROJECTION, &matProjection );

    // Make sure linear sampling is enabled
    pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    // Make sure stencil is off to avoid problems with flame effects
    pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

    // Maybe capture screen
    CGraphics::GetSingleton().GetRenderItemManager()->UpdateBackBufferCopy ();

    // Handle script stuffs
    if ( m_iUnminimizeFrameCounter-- && !m_iUnminimizeFrameCounter )
    {
        m_pModManager->DoPulsePreHUDRender( true, m_bDidRecreateRenderTargets );
        m_bDidRecreateRenderTargets = false;
    }
    else
        m_pModManager->DoPulsePreHUDRender( false, false );

    // Draw pre-GUI primitives
    CGraphics::GetSingleton().DrawPreGUIQueue();

    // Restore the render states
    pDevice->SetTransform( D3DTS_PROJECTION, &matProjection );
    if ( pDeviceState )
    {
        pDeviceState->Apply();
        pDeviceState->Release();
    }
}

//
// GetMinStreamingMemory
//
unsigned int CCore::GetMinStreamingMemory()
{
    unsigned int uiAmount = 50;

#ifdef MTA_DEBUG
    uiAmount = 1;
#endif

    return Min( uiAmount, GetMaxStreamingMemory() );
}

//
// GetMaxStreamingMemory
//
unsigned int CCore::GetMaxStreamingMemory ()
{
    unsigned int iMemoryMB = g_pDeviceState->AdapterState.InstalledMemoryKB / 1024;
    return Max < uint > ( 64, iMemoryMB );
}

EDiagnosticDebugType CCore::GetDiagnosticDebug()
{
    return m_DiagnosticDebug;
}

void CCore::SetDiagnosticDebug( EDiagnosticDebugType value )
{
    m_DiagnosticDebug = value;
}

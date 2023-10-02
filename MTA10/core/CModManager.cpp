/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManager.cpp
*  PURPOSE:     Game mod loading manager
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <shellapi.h>
#define DECLARE_PROFILER_SECTION_CModManager
#include "profiler/SharedUtil.Profiler.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
                                    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                                    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                                    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
                                    );

template<> CModManager * CSingleton < CModManager > ::m_pSingleton = NULL;

extern CCore* g_pCore;

CModManager::CModManager ( void )
{
    // Init
    m_hClientDLL = NULL;
    m_pClientBase = NULL;
    m_bUnloadRequested = false;

    // Default mod name defaults to "default"
    m_strDefaultModName = "default";

    // Load the modlist from the folders modfolder
    InitializeModList("");

    // Set up our exception handler
#ifdef MTA_DEBUG
    SetCrashHandlerFilter ( HandleExceptionGlobal );
#endif
}

CModManager::~CModManager ( void )
{
    // Unload the current loaded mod (if loaded)
    Unload ();

    // Clear the modlist
    Clear ();
}

void CModManager::RequestLoad ( const char* szModName, const char* szArguments )
{
    // Reset an eventual old request (and free our strings)
    ClearRequest ();

    // An unload is now requested
    m_bUnloadRequested = true;

    // Requested a mod name?
    if ( szModName )
    {
        m_strRequestedMod = szModName;

        m_strRequestedModArguments = szArguments ? szArguments : "";
    }
}

void CModManager::RequestLoadDefault ( const char* szArguments )
{
    RequestLoad ( m_strDefaultModName.c_str (), szArguments );
}

void CModManager::RequestUnload ( void )
{
    RequestLoad ( NULL, NULL );
    CCore::GetSingletonPtr () -> OnModUnload ();
}

void CModManager::ClearRequest ( void )
{
    // Free the old mod name
    m_strRequestedMod = "";

    // Free the old mod arguments
    m_strRequestedModArguments = "";

    // No unload requested now
    m_bUnloadRequested = false;
}

bool CModManager::IsLoaded ( void )
{
    return ( m_hClientDLL != NULL );
}

CClientBase* CModManager::Load( const char* szName, const char* szArguments )
{
    // Make sure we haven't already loaded a mod
    Unload();

    // Get the entry for the given name
    filePath fileName( szName );

    modMap_t::iterator itMod = m_ModDLLFiles.find( fileName );

    if ( itMod == m_ModDLLFiles.end() )
    {
        CCore::GetSingleton().GetConsole()->Printf ( "Unable to load %s (unknown mod)", szName );
        return NULL;
    }

    // Load the library and use the supplied path as an extra place to search for dependencies
    m_hClientDLL = LoadLibraryEx( itMod->second.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

    if ( !m_hClientDLL )
    {
        DWORD dwError = GetLastError ();
        char szError[2048];
        char* p;

        FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        NULL, dwError, LANG_NEUTRAL, szError, sizeof ( szError ), NULL );

        // Remove newlines from the error message
        p = szError + strlen( szError ) - 1;
        while ( p >= szError && (*p == '\r' || *p == '\n' ) )
        {
            *p = '\0';
            --p;
        }

        CCore::GetSingleton().GetConsole()->Printf( "Unable to load %s's DLL (reason: %s)", szName, szError );
        return NULL;
    }

    // Set up the mod root
    g_pCore->m_modRoot = g_pCore->GetFileSystem()->CreateTranslator( itMod->second.c_str() );

    // Get the address of InitClient
    typedef CClientBase* (__cdecl pfnClientInitializer) ();

    pfnClientInitializer* pClientInitializer = (pfnClientInitializer*)GetProcAddress( m_hClientDLL, "InitClient" );

    if ( pClientInitializer == NULL )
    {
        CCore::GetSingleton().GetConsole()->Printf( "Unable to load %s's DLL (unknown mod)", szName );
        FreeLibrary( m_hClientDLL );
        return NULL;
    }

    // Call InitClient and store the Client interface in m_pClientBase
    m_pClientBase = pClientInitializer();

    // Call the client base initializer
    if ( !m_pClientBase || m_pClientBase->ClientInitialize( szArguments, CCore::GetSingletonPtr() ) != ERROR_SUCCESS )
    {
        CCore::GetSingleton().GetConsole()->Printf( "Unable to load %s's DLL (unable to init, bad version?)", szName );
        FreeLibrary( m_hClientDLL );

        m_pClientBase = NULL;
        return NULL;
    }

    // HACK: make the console input active if its visible
    if ( CLocalGUI::GetSingleton().IsConsoleVisible() )
        CLocalGUI::GetSingleton().GetConsole()->ActivateInput ();

    // Tell chat to start handling input
    CLocalGUI::GetSingleton().GetChat()->OnModLoad();
 
    // Return the interface
    return m_pClientBase;
}

void CModManager::Unload ( void )
{
    // If a mod is loaded, we call m_pClientBase->ClientShutdown and then free the library
    if ( m_hClientDLL != NULL )
    {
        // Call m_pClientBase->ClientShutdown
        if ( m_pClientBase )
        {
            m_pClientBase->ClientShutdown ();
            m_pClientBase = NULL;
        }

        // Destroy mod root
        delete CCore::GetSingleton().m_modRoot;

        CCore::GetSingleton().m_modRoot = NULL;

        // Unregister the commands it had registered
        CCore::GetSingleton ().GetCommands ()->DeleteAll ();

        // Free the Client DLL
        FreeLibrary ( m_hClientDLL );
        m_hClientDLL = NULL;

        // Call the on mod unload func
        CCore::GetSingletonPtr () -> OnModUnload ();

        // Reset chatbox status (so it won't prevent further input), and clear it
        /*CLocalGUI::GetSingleton ().GetChatBox ()->SetInputEnabled ( false );
        CLocalGUI::GetSingleton ().GetChatBox ()->Clear ();*/
        CLocalGUI::GetSingleton ().GetChat ()->SetInputVisible ( false );
        CLocalGUI::GetSingleton ().GetChat ()->Clear ();
        CLocalGUI::GetSingleton ().SetChatBoxVisible ( true );
        CLocalGUI::GetSingleton ().SetChatBoxEnabled ( true );

        // Reset the debugview status
        CLocalGUI::GetSingleton ().GetDebugView ()->SetVisible ( false );
        CLocalGUI::GetSingleton ().GetDebugView ()->Clear ();
        CLocalGUI::GetSingleton ().SetDebugViewVisible ( false );

        // NULL the message processor and the unhandled command handler
        CCore::GetSingleton ().SetClientMessageProcessor ( NULL );
        CCore::GetSingleton ().GetCommands ()->SetExecuteHandler ( NULL );

        // Reset the modules
        CCore::GetSingleton ().GetGame ()->Reset ();
        CCore::GetSingleton ().GetMultiplayer ()->Reset ();
        CCore::GetSingleton ().GetNetwork ()->Reset ();
        assert ( CCore::GetSingleton ().GetNetwork ()->GetServerBitStreamVersion () == 0 );

        // Enable the console again
        CCore::GetSingleton ().GetConsole ()->SetEnabled ( true );

        // Force the mainmenu back
        CCore::GetSingleton ().SetConnected ( false );
        CLocalGUI::GetSingleton ().GetMainMenu ()->SetIsIngame ( false );
        CLocalGUI::GetSingleton ().GetMainMenu ()->SetVisible ( true, false );

        if ( XfireIsLoaded () )
        {
            XfireSetCustomGameData ( 0, NULL, NULL ); 
        }
    }
}


void CModManager::DoPulsePreFrame ( void )
{
    if ( m_pClientBase )
    {
        m_pClientBase->PreFrameExecutionHandler ();
    }
}


void CModManager::DoPulsePreHUDRender ( bool bDidUnminimize, bool bDidRecreateRenderTargets )
{
    if ( m_pClientBase )
    {
        m_pClientBase->PreHUDRenderExecutionHandler ( bDidUnminimize, bDidRecreateRenderTargets );
    }
}


void CModManager::DoPulsePostFrame ( void )
{
    // Load/unload requested?
    if ( m_bUnloadRequested )
    {
        // Unload the current mod
        Unload ();

        // Load a new mod?
        if ( m_strRequestedMod != "" )
        {
            Load ( m_strRequestedMod, m_strRequestedModArguments );
        }

        // Clear the request
        ClearRequest ();
    }

    // Pulse the client
    if ( m_pClientBase )
    {
        m_pClientBase->PostFrameExecutionHandler ();
    }
    else
    {
        CCore::GetSingleton ().GetNetwork ()->DoPulse ();
    }

    // Make sure frame rate limit gets applied
    if ( m_pClientBase )
        CCore::GetSingleton ().EnsureFrameRateLimitApplied ();  // Catch missed frames
    else
        CCore::GetSingleton ().ApplyFrameRateLimit ( 60 );      // Limit when not connected

    // Load/unload requested?
    if ( m_bUnloadRequested )
    {
        // Unload the current mod
        Unload ();

        // Load a new mod?
        if ( m_strRequestedMod != "" )
        {
            Load ( m_strRequestedMod, m_strRequestedModArguments );
        }

        // Clear the request
        ClearRequest ();
    }
}

CClientBase* CModManager::GetCurrentMod ( void )
{
    return m_pClientBase;
}

void CModManager::RefreshMods ( void )
{
    // Clear the list, and load it again
    Clear();

    InitializeModList( "" );
}

long WINAPI CModManager::HandleExceptionGlobal ( _EXCEPTION_POINTERS* pException )
{
    // Create the exception information class
    CExceptionInformation_Impl* pExceptionInformation = new CExceptionInformation_Impl;
    pExceptionInformation->Set ( pException->ExceptionRecord->ExceptionCode, pException );

    // Grab the mod manager
    CModManager* pModManager = CModManager::GetSingletonPtr ();
    if ( pModManager )
    {
        // Got a client?
        if ( pModManager->m_pClientBase )
        {
            // Protect us from "double-faults"
            try
            {
                // Let the client handle it. If it could, continue the execution
                if ( pModManager->m_pClientBase->HandleException ( pExceptionInformation ) )
                {
                    // Delete the exception record and continue to search the exception stack
                    delete pExceptionInformation;
                    return EXCEPTION_CONTINUE_SEARCH;
                }

                // The client wants us to terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
            catch ( ... )
            {
                // Double-fault, terminate the process
                DumpCoreLog ( pExceptionInformation );
                DumpMiniDump ( pException, pExceptionInformation );
                RunErrorTool ( pExceptionInformation );
                TerminateProcess ( GetCurrentProcess (), 1 );
            }
        }
        else
        {
            // Continue if we're in debug mode, if not terminate
            #ifdef MTA_DEBUG
                return EXCEPTION_CONTINUE_SEARCH;
            #endif
        }
    }

    // Terminate the process
    DumpCoreLog ( pExceptionInformation );
    DumpMiniDump ( pException, pExceptionInformation );
    RunErrorTool ( pExceptionInformation );
    TerminateProcess ( GetCurrentProcess (), 1 );
    return EXCEPTION_CONTINUE_SEARCH;
}

#include <CFileSystem.common.stl.h>

void CModManager::DumpCoreLog ( CExceptionInformation* pExceptionInformation )
{
    // Write a log with the generic exception information
    CFile *file = mtaFileRoot->Open( "core.log", "w+" );

    if ( !file )
        return;

    // Header
    {
        FileSystem::fileStreamBuf stlWrapper( file );
        std::ostream output( &stlWrapper );

        output << "** -- Unhandled exception -- **\n\n";

        // Write the time
        time_t timeTemp;
        time ( &timeTemp );

        SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );

        SString strInfo;
        strInfo += SString ( "Version = %s\n", strMTAVersionFull.c_str () );
        strInfo += SString ( "Time = %s", ctime ( &timeTemp ) );

        strInfo += SString ( "Module = %s\n", pExceptionInformation->GetModulePathName () );

        // Write the basic exception information
        strInfo += SString ( "Code = 0x%08X\n", pExceptionInformation->GetCode () );
        strInfo += SString ( "Offset = 0x%08X\n\n", pExceptionInformation->GetAddressModuleOffset () );

        // Write the register info
        strInfo += SString ( "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n" \
                         "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n" \
                         "CS=%04X   DS=%04X  SS=%04X  ES=%04X   " \
                         "FS=%04X  GS=%04X\n\n",
                         pExceptionInformation->GetEAX (),
                         pExceptionInformation->GetEBX (),
                         pExceptionInformation->GetECX (),
                         pExceptionInformation->GetEDX (),
                         pExceptionInformation->GetESI (),
                         pExceptionInformation->GetEDI (),
                         pExceptionInformation->GetEBP (),
                         pExceptionInformation->GetESP (),
                         pExceptionInformation->GetEIP (),
                         pExceptionInformation->GetEFlags (),
                         pExceptionInformation->GetCS (),
                         pExceptionInformation->GetDS (),
                         pExceptionInformation->GetSS (),
                         pExceptionInformation->GetES (),
                         pExceptionInformation->GetFS (),
                         pExceptionInformation->GetGS () );


        output << strInfo;

        // End of unhandled exception
        output << "** -- End of unhandled exception -- **\n\n\n";

        // For the crash dialog
        SetApplicationSetting ( "diagnostics", "last-crash-info", strInfo );
    }

    // Close the file
    delete file;
}

void CModManager::DumpMiniDump ( _EXCEPTION_POINTERS* pException, CExceptionInformation* pExceptionInformation )
{
    if ( !g_pCore->GetFileSystem()->WriteMiniDump( "core.dmp", pException ) )
        return;

    // Grab the current time
    // Ask windows for the system time.
    SYSTEMTIME SystemTime;
    GetLocalTime ( &SystemTime );

    SString strModuleName = pExceptionInformation->GetModuleBaseName ();
    strModuleName = strModuleName.ReplaceI ( ".dll", "" ).Replace ( ".exe", "" ).Replace ( "_", "" ).Replace ( ".", "" ).Replace ( "-", "" );
    if ( strModuleName.length () == 0 )
        strModuleName = "unknown";

    SString strMTAVersionFull = SString ( "%s.%s", MTA_DM_BUILDTAG_LONG, *GetApplicationSetting ( "mta-version-ext" ).SplitRight ( ".", NULL, -2 ) );
    SString strSerialPart = GetApplicationSetting ( "serial" ).substr ( 0, 5 );
    uint uiServerIP = GetApplicationSettingInt ( "last-server-ip" );
    uint uiServerPort = GetApplicationSettingInt ( "last-server-port" );
    int uiServerTime = GetApplicationSettingInt ( "last-server-time" );
    int uiServerDuration = _time32 ( NULL ) - uiServerTime;
    uiServerDuration = Clamp ( 0, uiServerDuration + 1, 0xfff );

    // Get path to mta dir
    SString strPathCode;
    dirTree parts;
    bool file;

    mtaFileRoot->GetFullPathTree( "/", parts, file );

    for ( uint i = 0; i < parts.size(); i++ )
    {
        if ( parts[i] == "Program Files" )
            strPathCode += "Pr";
        else if ( parts[i] == "Program Files (x86)" )
            strPathCode += "Px";
        else if ( parts[i] == "MTA San Andreas" )
            strPathCode += "Mt";
#if 0
        else if ( parts[i].find( "MTA San Andreas" ) == 0 )
            strPathCode += "Mb";
#endif
        else
        {
            strPathCode += toupper( parts[i].at( 1 ) );
        }
    }

    // Copy the file over
    mtaFileRoot->Copy( "core.dmp", SString(
        "dumps\\client_%s_%s_%08x_%x_%s_%08X_%04X_%03X_%s_%04d%02d%02d_%02d%02d.dmp",
        strMTAVersionFull.c_str (),
        strModuleName.c_str (),
        pExceptionInformation->GetAddressModuleOffset (),
        pExceptionInformation->GetCode () & 0xffff,
        strPathCode.c_str (),
        uiServerIP,
        uiServerPort,
        uiServerDuration,
        strSerialPart.c_str (),
        SystemTime.wYear,
        SystemTime.wMonth,
        SystemTime.wDay,
        SystemTime.wHour,
        SystemTime.wMinute));

    // For the dump uploader
    filePath dmpPath;
    mtaFileRoot->GetFullPath( "core.dmp", true, dmpPath );

    SetApplicationSetting( "diagnostics", "last-dump-save", dmpPath.convert_ansi() );
}

void CModManager::RunErrorTool ( CExceptionInformation* pExceptionInformation )
{
// MTA Error Reporter is now integrated into the launcher

    // Only do once
    static bool bDoneReport = false;

    if ( bDoneReport )
        return;

    bDoneReport = false;

    // Log the basic exception information
    SString strMessage ( 
        "Crash 0x%08X 0x%08X %s"
        " EAX=%08X EBX=%08X ECX=%08X EDX=%08X ESI=%08X"
        " EDI=%08X EBP=%08X ESP=%08X EIP=%08X FLG=%08X"
        " CS=%04X DS=%04X SS=%04X ES=%04X"
        " FS=%04X GS=%04X",
        pExceptionInformation->GetCode (),
        pExceptionInformation->GetAddressModuleOffset (),
        pExceptionInformation->GetModulePathName (),
        pExceptionInformation->GetEAX (),
        pExceptionInformation->GetEBX (),
        pExceptionInformation->GetECX (),
        pExceptionInformation->GetEDX (),
        pExceptionInformation->GetESI (),
        pExceptionInformation->GetEDI (),
        pExceptionInformation->GetEBP (),
        pExceptionInformation->GetESP (),
        pExceptionInformation->GetEIP (),
        pExceptionInformation->GetEFlags (),
        pExceptionInformation->GetCS (),
        pExceptionInformation->GetDS (),
        pExceptionInformation->GetSS (),
        pExceptionInformation->GetES (),
        pExceptionInformation->GetFS (),
        pExceptionInformation->GetGS ()
    );

    AddReportLog ( 3120, strMessage );

    // Try relaunch with crashed flag
    SString strMTASAPath = GetMTASABaseDir ();
    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

#ifdef MTA_DEBUG
    #define MTA_EXE_NAME            "Multi Theft Auto_d.exe"
#else
    #define MTA_EXE_NAME            "Multi Theft Auto.exe"
#endif

    SString strFile = strMTASAPath + "\\" + MTA_EXE_NAME;
    ShellExecute( NULL, "open", strFile, "install_stage=crashed", NULL, SW_SHOWNORMAL );
}

void CModManager::InitializeModList ( const char* szModFolderPath )
{
    // Variables used to search the mod directory
    WIN32_FIND_DATAA FindData;
    HANDLE hFind;
    filePath modPath;

    if ( !modFileRoot->GetFullPath( szModFolderPath, false, modPath ) )
        return;

    SString strPathWildchars ( "%s*.*", modPath.c_str() );

    // Create a search
    hFind = FindFirstFile ( strPathWildchars, &FindData );

    // If we found a first file ...
    if ( hFind == INVALID_HANDLE_VALUE )
        return;

    // Add it to the list
    VerifyAndAddEntry ( szModFolderPath, FindData.cFileName );

    // Search until there aren't any files left
    while ( FindNextFile ( hFind, &FindData ) == TRUE )
        VerifyAndAddEntry ( szModFolderPath, FindData.cFileName );

    // End the search
    FindClose ( hFind );
}

void CModManager::Clear ( void )
{
    // Clear the list
    m_ModDLLFiles.clear ();
}

void CModManager::VerifyAndAddEntry( const char* szModFolderPath, const char* szName )
{
    filePath modPath;
    HMODULE hDLL;

    // NOTE: We do not have to check for race anymore!
    if ( strcmp ( szName, "." ) == 0 || strcmp ( szName, ".." ) == 0 )
        return;

    // Paths given by the OS are always absolute
    modFileRoot->GetFullPath( szModFolderPath, true, modPath );
    modPath += szName;
    modPath += '/';

    // Add the .dll's path to the environment variables
    char pathBuffer[4096];
    GetEnvironmentVariable( "PATH", pathBuffer, sizeof(pathBuffer) - 1 );

    size_t sEnv = strlen( pathBuffer );
    size_t extSize = modPath.size() + 1;

    if ( sEnv + extSize < sizeof(pathBuffer) )
    {
        // We insert the path, so it has highest priority
        memcpy( pathBuffer + extSize, pathBuffer, modPath.size() );
        pathBuffer[modPath.size()] = ';';

        // Insert our manly path
        memcpy( pathBuffer, modPath.c_str(), modPath.size() );

        SetEnvironmentVariable( "PATH", pathBuffer );
    }

#ifdef _DEBUG
    modPath += "Client_d.dll";
#else
    modPath += "Client.dll";
#endif

    // Attempt to load the primary client DLL
    hDLL = LoadLibraryEx( modPath.c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES );

    if ( !hDLL )
    {
        CLogger::GetSingleton ().ErrorPrintf ( "Invalid mod DLL: %s (reason: %d)", szName, GetLastError() );
        return;
    }

    // Check if InitClient symbol exists
    if ( GetProcAddress( hDLL, "InitClient" ) != NULL )
    {
        // Add it to the list
        m_ModDLLFiles[ filePath( szName ) ] = modPath;
    }
    else
        CLogger::GetSingleton().ErrorPrintf( "Unknown mod DLL: %s", szName );

    // Free the DLL
    FreeLibrary( hDLL );
}
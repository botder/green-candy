/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Main.cpp
*  PURPOSE:     MTA loader
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "direct.h"
#include "SharedUtil.Tests.hpp"
#include "SharedUtil.hpp"
int DoLaunchGame ( LPSTR lpCmdLine );
int LaunchGame ( LPSTR lpCmdLine );

HINSTANCE g_hInstance = NULL;

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain ( HINSTANCE _hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
#if defined(_DEBUG) && defined(__SHAREDUTIL_TESTING)
    SharedUtil_Tests ();
#endif

    int iReturnCode = 0;

    try
    {
        // Initialize the file system.
        {
            fs_construction_params params;
            params.nativeExecMan = NULL;   // no need for threading support.

            CFileSystem::Create( params );
        }

        try
        {
            //////////////////////////////////////////////////////////
            //
            // Handle duplicate launching, or running from mtasa:// URI ?
            //
            bool success = true;

            int iRecheckTimeLimit = 2000;
            while ( ! CreateSingleInstanceMutex () )
            {
                if ( strcmp ( lpCmdLine, "" ) != 0 )
                {
                    COPYDATASTRUCT cdStruct;

                    cdStruct.cbData = strlen(lpCmdLine)+1;
                    cdStruct.lpData = const_cast<char *>((lpCmdLine));
                    cdStruct.dwData = URI_CONNECT;

                    HWND hwMTAWindow = FindWindow( NULL, "MTA: San Andreas" );
#ifdef MTA_DEBUG
                    if( hwMTAWindow == NULL )
                        hwMTAWindow = FindWindow( NULL, "MTA: San Andreas [DEBUG]" );
#endif
                    if( hwMTAWindow != NULL )
                    {
                        SendMessage( hwMTAWindow,
                                    WM_COPYDATA,
                                    NULL,
                                    (LPARAM)&cdStruct );
                    }
                    else
                    {
                        if ( iRecheckTimeLimit > 0 )
                        {
                            // Sleep a little bit and check the mutex again
                            Sleep ( 500 );
                            iRecheckTimeLimit -= 500;
                            continue;
                        }
                        SString strMessage;
                        strMessage += "Trouble restarting MTA:SA\n\n";
                        strMessage += "If the problem persists, open Task Manager and\n";
                        strMessage += "stop the 'gta_sa.exe' and 'Multi Theft Auto.exe' processes\n\n\n";
                        strMessage += "Try to launch MTA:SA again?";
                        if ( MessageBox( 0, strMessage, "Error", MB_ICONERROR | MB_YESNO ) == IDYES )
                        {
                            SetMTASAPathSource( false );
                            ShellExecuteNonBlocking( "open", ( GetMTASAPath() + MTA_EXE_NAME ).convert_ansi(), lpCmdLine );            
                        }
                
                        success = false;
                        break;
                    }
                }
                else
                {
                    MessageBox ( 0, "Another instance of MTA is already running.", "Error", MB_ICONERROR );
            
                    success = false;
                    break;
                }
        
                success = false;
                break;
            }

            if ( success )
            {
                //
                // Continue any install procedure
                //
                SString strCmdLine = GetInstallManager ()->Continue ( lpCmdLine );

                //
                // Go for launch
                //
                iReturnCode = LaunchGame ( const_cast < char* > ( *strCmdLine ) );

                AddReportLog ( 1044, SString ( "* End (%d)* pid:%d",  iReturnCode, GetCurrentProcessId() ) );
            }
        }
        catch( ... )
        {
            CFileSystem::Destroy( fileSystem );

            throw;
        }

        // Destroy the file system access again.
        CFileSystem::Destroy( fileSystem );
    }
    catch( ... )
    {
        // We encountered a fatal error, so just pass things on.
        iReturnCode = -1;
    }

    return iReturnCode;
}


//////////////////////////////////////////////////////////
//
// HandleTrouble
//
//
//
//////////////////////////////////////////////////////////
void HandleTrouble ( void )
{
    int iResponse = MessageBox ( NULL, "Are you having problems running MTA:SA?.\n\nDo you want to revert to an earlier version?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
    if ( iResponse == IDYES )
    {
        MessageBox ( NULL, "Your browser will now display a web page with some help infomation.\n\nIf the page fails to load, please goto www.mtasa.com", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
        BrowseToSolution ( "crashing-before-gtagame", false, true );
    }
}


//////////////////////////////////////////////////////////
//
// HandleResetSettings
//
//
//
//////////////////////////////////////////////////////////
void HandleResetSettings ( void )
{
    wchar_t szResult[MAX_PATH] = L"";
    SHGetFolderPathW( NULL, CSIDL_PERSONAL, NULL, 0, szResult );
    filePath settingsFolderPath = ( filePath( szResult ) + "/GTA San Andreas User Files/" );

    CFileTranslator *settingsRoot = fileSystem->CreateTranslator( settingsFolderPath );

    if ( settingsRoot )
    {
        if ( settingsRoot->Exists( L"gta_sa.set" ) )
        {
            int iResponse = MessageBoxA( NULL, "There seems to be a problem launching MTA:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want to reset GTA settings now?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
            if ( iResponse == IDYES )
            {
                settingsRoot->Delete( L"gta_sa_old.set" );
                settingsRoot->Rename( L"gta_sa.set", L"gta_sa_old.set" );
                settingsRoot->Delete( L"gta_sa.set" );  // not required, but for safety.

                if ( !settingsRoot->Exists( L"gta_sa.set" ) )
                {
                    AddReportLog( 4053, "Deleted gta_sa.set" );
                    MessageBoxA( NULL, "GTA settings have been reset.\n\nPress OK to continue.", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
                }
                else
                {
                    AddReportLog( 5054, "Delete gta_sa.set failed" );
                    MessageBoxA( NULL, "GTA:SA Settings File could not be deleted", "Error", MB_OK | MB_ICONERROR );
                }
            }
        }
        else
        {
            // No settings to delete, or can't find them
            int iResponse = MessageBoxA( NULL, "Are you having problems running MTA:SA?.\n\nDo you want to see some online help?", "MTA: San Andreas", MB_YESNO | MB_ICONERROR );
            if ( iResponse == IDYES )
            {
                MessageBoxA( NULL, "Your browser will now display a web page with some help infomation.\n\nIf the page fails to load, please goto www.mtasa.com", "MTA: San Andreas", MB_OK | MB_ICONINFORMATION );
                BrowseToSolution( "crashing-before-gtalaunch", false, true );
            }
        }

        delete settingsRoot;
    }
}


//////////////////////////////////////////////////////////
//
// LaunchGame
//
//
//
//////////////////////////////////////////////////////////
int LaunchGame ( LPSTR lpCmdLine )
{
    //
    // "L0" is opened before the launch sequence and is closed if MTA shutsdown with no error
    // "L1" is opened before the launch sequence and is closed if GTA is succesfully started
    // "CR1" is a counter which is incremented if GTA was not started and MTA shutsdown with an error
    //
    // "L2" is opened before the launch sequence and is closed if the GTA loading screen is shown
    // "CR2" is a counter which is incremented at startup, if the previous run didn't make it to the loading screen
    //

    // Check for unclean stop on previous run
    if ( WatchDogIsSectionOpen ( "L0" ) )
        WatchDogSetUncleanStop ( true );    // Flag to maybe do things differently if MTA exit code on last run was not 0
    else
        WatchDogSetUncleanStop ( false );

    // Reset counter if gta game was run last time
    if ( !WatchDogIsSectionOpen ( "L1" ) )
        WatchDogClearCounter ( "CR1" );

    // If crashed 3 times in a row before starting the game, do something
    if ( WatchDogGetCounter ( "CR1" ) >= 3 )
    {
        WatchDogReset ();
        HandleTrouble ();
    }

    // Check for possible gta_sa.set problems
    if ( WatchDogIsSectionOpen ( "L2" ) )
    {
        WatchDogIncCounter ( "CR2" );       // Did not reach loading screen last time
        WatchDogCompletedSection ( "L2" );
    }
    else
        WatchDogClearCounter ( "CR2" );

    // If didn't reach loading screen 5 times in a row, do something
    if ( WatchDogGetCounter ( "CR2" ) >= 5 )
    {
        WatchDogClearCounter ( "CR2" );
        HandleResetSettings ();
    }

    WatchDogBeginSection ( "L0" );      // Gets closed if MTA exits with a return code of 0
    WatchDogBeginSection ( "L1" );      // Gets closed when online game has started

    int iReturnCode = DoLaunchGame ( lpCmdLine );

    if ( iReturnCode == 0 )
    {
        WatchDogClearCounter ( "CR1" );
        WatchDogCompletedSection ( "L0" );
    }

    return iReturnCode;
}


//////////////////////////////////////////////////////////
//
// DoLaunchGame
//
//
//
//////////////////////////////////////////////////////////
#include <memory>

int DoLaunchGame ( LPSTR lpCmdLine )
{
    assert ( !CreateSingleInstanceMutex () );

    //////////////////////////////////////////////////////////
    //
    // Handle GTA already running
    //
    if ( !TerminateGTAIfRunning () )
    {
        DisplayErrorMessageBox ( "MTA: SA couldn't start because another instance of GTA is running." );
        return 1;
    }

    //////////////////////////////////////////////////////////
    //
    // Get path to GTASA
    //
    filePath strGTAPath;
    ePathResult iResult = GetGamePath ( strGTAPath, true );
    if ( iResult == GAME_PATH_MISSING ) {
        DisplayErrorMessageBox ( "Registry entries are is missing. Please reinstall Multi Theft Auto: San Andreas.", "reg-entries-missing" );
        return 5;
    }
    else if ( iResult == GAME_PATH_STEAM ) {
        DisplayErrorMessageBox ( "It appears you have a Steam version of GTA:SA, which is currently incompatible with MTASA.  You are now being redirected to a page where you can find information to resolve this issue." );
        BrowseToSolution ( "downgrade-steam" );
        return 5;
    }

    // Make sure we append a slash.
    strGTAPath += "\\";

    const filePath strMTASAPath = GetMTASAPath();

    std::wstring wideMTASAPath = strMTASAPath.convert_unicode();
    std::wstring wideGTAPath = strGTAPath.convert_unicode();

    std::unique_ptr <CFileTranslator> gtaRootTranslator( fileSystem->CreateTranslator( strGTAPath ) );

    if ( !gtaRootTranslator )
    {
        return 5;
    }

    std::unique_ptr <CFileTranslator> mtaRootTranslator( fileSystem->CreateTranslator( strMTASAPath ) );

    if ( !mtaRootTranslator )
    {
        return 5;
    }

    SetCurrentDirectoryW( wideMTASAPath.c_str() );
    SetDllDirectoryW( wideMTASAPath.c_str() );

    //////////////////////////////////////////////////////////
    //
    // Show splash screen and wait 2 seconds
    //
    ShowSplash( g_hInstance );

    //////////////////////////////////////////////////////////
    //
    // Basic check for some essential files
    //
    const char* dataFilesFiles [] = { "\\MTA\\cgui\\images\\background_logo.png"
                                     ,"\\MTA\\D3DX9_42.dll"
                                     ,"\\MTA\\D3DCompiler_42.dll"
                                     ,"\\MTA\\bass.dll"};

    for ( uint i = 0 ; i < NUMELMS( dataFilesFiles ) ; i++ )
    {
        if ( !mtaRootTranslator->Exists( dataFilesFiles[ i ] ) )
        {
            return DisplayErrorMessageBox ( "Load failed. Please ensure that the latest data files have been installed correctly.", "mta-datafiles-missing" );
        }
    }

    if ( mtaRootTranslator->Size( "MTA\\bass.dll" ) != 0x00018838 )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that the latest data files have been installed correctly.", "mta-datafiles-missing" );
    }

    // Check for client file
    if ( !mtaRootTranslator->Exists( CHECK_DM_CLIENT_NAME ) )
    {
        return DisplayErrorMessageBox ( "Load failed. Please ensure that '" CHECK_DM_CLIENT_NAME "' is installed correctly.", "client-missing" );
    }

    // Grab the MTA folder
    filePath strDir = ( strMTASAPath + "mta\\" );

    // Make sure the gta executable exists
    SetCurrentDirectoryW( wideGTAPath.c_str() );
    if ( !gtaRootTranslator->Exists( MTA_GTAEXE_NAME ) )
    {
        std::string ansiGTAPath = strGTAPath.convert_ansi();

        return DisplayErrorMessageBox( SString( "Load failed. Could not find gta_sa.exe in %s.", ansiGTAPath.c_str() ), "gta_sa-missing" );
    }

    // Make sure important dll's do not exist in the wrong place
    char* dllCheckList[] = { "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll" };
    for ( int i = 0 ; i < NUMELMS( dllCheckList ); i++ )
    {
        if ( gtaRootTranslator->Exists( dllCheckList[i] ) )
        {
            return DisplayErrorMessageBox( SString( "Load failed. %s exists in the GTA directory. Please delete before continuing.", dllCheckList[i] ), "file-clash" );
        }    
    }

    // Warning if d3d9.dll exists in the GTA install directory
    if ( gtaRootTranslator->Exists( L"d3d9.dll" ) )
    {
        std::string ansiD3DPath = strGTAPath.convert_ansi();
        ansiD3DPath += "d3d9.dll";

        ShowD3dDllDialog( g_hInstance, ansiD3DPath );
        HideD3dDllDialog();
    }    

    // Strip out flag from command line
    SString strCmdLine = lpCmdLine;
    bool bDoneAdmin = strCmdLine.Contains ( "/done-admin" );
    strCmdLine = strCmdLine.Replace ( " /done-admin", "" );

    //////////////////////////////////////////////////////////
    //
    // Hook 'n' go
    //
    // Launch GTA using CreateProcess
    PROCESS_INFORMATION piLoadee;
    STARTUPINFOW siLoadee;
    memset( &piLoadee, 0, sizeof ( PROCESS_INFORMATION ) );
    memset( &siLoadee, 0, sizeof ( STARTUPINFO ) );
    siLoadee.cb = sizeof ( STARTUPINFO );

    WatchDogBeginSection ( "L2" );      // Gets closed when loading screen is shown

    // Start GTA
    std::wstring wideCmdLine = CharacterUtil::ConvertStrings <char, wchar_t> ( strCmdLine.c_str() );
    std::wstring wideGTAEXEPath = wideGTAPath + MTA_GTAEXE_NAME_WIDE;

    if ( 0 == CreateProcessW( wideGTAEXEPath.c_str(),
                              (wchar_t*)wideCmdLine.c_str(),
                              NULL,
                              NULL,
                              FALSE,
#ifdef _DEBUG
                              CREATE_SUSPENDED | DEBUG_PROCESS,
#else
                              CREATE_SUSPENDED,
#endif
                              NULL,
                              NULL,
                              &siLoadee,
                              &piLoadee ) )
    {
        DWORD dwError = GetLastError ();

        if ( dwError == ERROR_ELEVATION_REQUIRED && !bDoneAdmin )
        {
            // Try to relaunch as admin if not done so already
            ReleaseSingleInstanceMutex ();
            ShellExecuteNonBlocking( "runas", ( strMTASAPath + MTA_EXE_NAME ).convert_ansi(), strCmdLine + " /done-admin" );            
            return 5;
        }
        else
        {
            // Otherwise, show error message
            SString strError = GetSystemErrorMessage ( dwError );            
            DisplayErrorMessageBox ( "Could not start Grand Theft Auto: San Andreas.  "
                                "Please try restarting, or if the problem persists,"
                                "contact MTA at www.multitheftauto.com. \n\n[" + strError + "]", "createprocess-fail;" + strError );
            return 5;
        }
    }

    SString strCoreDLL = strMTASAPath + "mta\\" + MTA_DLL_NAME;

    // Check if the core (mta_blue.dll or mta_blue_d.dll exists)
    if ( !fileSystem->Exists( strCoreDLL ) )
    {
        DisplayErrorMessageBox ( "Load failed.  Please ensure that "
                            "the file core.dll is in the modules "
                            "directory within the MTA root directory.", "core-missing" );

        // Kill GTA and return errorcode
        TerminateProcess ( piLoadee.hProcess, 1 );
        return 1;
    }

    SetDllDirectory( SString ( strMTASAPath + "\\mta" ) );

    // Wait until the splash has been displayed the required amount of time
    HideSplash ( true );

    // Inject the core into GTA
    RemoteLoadLibrary ( piLoadee.hProcess, strCoreDLL );

    // Actually hide the splash
    HideSplash ();
    
    // Clear previous on quit commands
    SetOnQuitCommand ( "" );

    // Resume execution for the game.
    ResumeThread ( piLoadee.hThread );

    // Wait for game to exit
    if ( piLoadee.hThread )
        WaitForObject ( piLoadee.hProcess, NULL, INFINITE, g_hMutex );

    // Get its exit code
    DWORD dwExitCode = -1;
    GetExitCodeProcess( piLoadee.hProcess, &dwExitCode );

    //////////////////////////////////////////////////////////
    //
    // On exit
    //
    // Cleanup and exit.
    CloseHandle ( piLoadee.hProcess );
    CloseHandle ( piLoadee.hThread );
    ReleaseSingleInstanceMutex ();


    //////////////////////////////////////////////////////////
    //
    // Handle OnQuitCommand
    //
    // Maybe spawn an exe
    {
        SetCurrentDirectoryW( wideMTASAPath.c_str() );
        SetDllDirectoryW( wideMTASAPath.c_str() );

        SString strOnQuitCommand = GetRegistryValue( "", "OnQuitCommand" );

        std::vector <SString> vecParts;
        strOnQuitCommand.Split( "\t", vecParts );

        if ( vecParts.size() > 4 && vecParts[0].length() )
        {
            SString strOperation = vecParts[0];
            SString strFile = vecParts[1];
            SString strParameters = vecParts[2];
            SString strDirectory = vecParts[3];
            SString strShowCmd = vecParts[4];

            if ( strOperation == "restart" )
            {
                strOperation = "open";
                strFile = ( strMTASAPath + "\\" + MTA_EXE_NAME ).convert_ansi();
            }

            LPCTSTR lpOperation     = strOperation == "" ? NULL : strOperation.c_str ();
            LPCTSTR lpFile          = strFile.c_str();
            LPCTSTR lpParameters    = strParameters == "" ? NULL : strParameters.c_str ();
            LPCTSTR lpDirectory     = strDirectory == "" ? NULL : strDirectory.c_str ();
            INT nShowCmd            = strShowCmd == "" ? SW_SHOWNORMAL : atoi( strShowCmd );

            if ( lpOperation && lpFile )
            {
                ShellExecuteNonBlocking( lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );            
            }
        }
    }

    // Maybe show help if trouble was encountered
    ProcessPendingBrowseToSolution ();

    // Success, maybe
    return dwExitCode;
}


extern "C" _declspec(dllexport)
int DoWinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    return WinMain ( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
}

int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    g_hInstance = hModule;
    return TRUE;
}

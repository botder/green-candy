/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace
{
    //////////////////////////////////////////////////////////
    //
    // Helper functions for this file
    //
    //
    //
    //////////////////////////////////////////////////////////

    // Ensure report log stuff has correct tags
    void UpdateSettingsForReportLog ( void )
    {
        UpdateMTAVersionApplicationSetting ();
        SetApplicationSetting ( "os-version",       GetOSVersion () );
        SetApplicationSetting ( "real-os-version",  GetRealOSVersion () );
        SetApplicationSetting ( "is-admin",         IsUserAdmin () ? "1" : "0" );
        SetApplicationSettingInt ( "last-server-ip", 0 );
    }

    // Comms between 'Admin' and 'User' processes
    void SendStringToUserProcess ( const SString& strText )
    {
        SetApplicationSetting ( "admin2user_comms", strText );
    }

    SString ReceiveStringFromAdminProcess ( void )
    {
        return GetApplicationSetting ( "admin2user_comms" );
    }

    bool IsBlockingUserProcess ( void )
    {
        return GetApplicationSetting ( "admin2user_comms" ) == "user_waiting";
    }

    void SetIsBlockingUserProcess ( void )
    {
        SetApplicationSetting ( "admin2user_comms", "user_waiting" );
    }

    void ClearIsBlockingUserProcess ( void )
    {
        if ( IsBlockingUserProcess () )
            SetApplicationSetting ( "admin2user_comms", "" );
    }
}


//////////////////////////////////////////////////////////
//
// CInstallManager global object
//
//
//
//////////////////////////////////////////////////////////
CInstallManager* g_pInstallManager = NULL;

CInstallManager* GetInstallManager ( void )
{
    if ( !g_pInstallManager )
        g_pInstallManager = new CInstallManager ();
    return g_pInstallManager;
}


//////////////////////////////////////////////////////////
//
// CInstallManager::InitSequencer
//
//
//
//////////////////////////////////////////////////////////
void CInstallManager::InitSequencer ( void )
{
    #define CR "\n"
    SString strSource =
                CR "initial: "
                CR "            CALL CheckOnRestartCommand "
                CR "            IF LastResult != ok GOTO update_end: "
                CR " "                                              ////// Start of 'update game' //////
                CR "            CALL MaybeSwitchToTempExe "         // If update files comes with and .exe, switch to that for the install
                CR "copy_files: "
                CR "            CALL InstallFiles "                 // Try to install update files
                CR "            IF LastResult == ok GOTO update_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If install failed, try as admin
                CR "            IF LastResult == ok GOTO copy_files: "
                CR " "
                CR "            CALL ShowCopyFailDialog "           // If install failed as admin, show message box
                CR "            IF LastResult == retry GOTO copy_files: "
                CR " "
                CR "update_end: "                                   ////// End of 'update game' //////
                CR "            CALL SwitchBackFromTempExe "
                CR " "        
                CR "newlayout_check:"                               ////// Start of 'new layout check' //////
                CR "            CALL ProcessLayoutChecks "
                CR "            IF LastResult == ok GOTO newlayout_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO newlayout_check: "
                CR "newlayout_end: "                                ////// End of 'new layout check' //////
                CR " "        
                CR "aero_check: "                                   ////// Start of 'Windows 7 aero desktop fix' //////
                CR "            CALL ProcessAeroChecks "            // Make changes to comply with user setting
                CR "            IF LastResult == ok GOTO aero_end: "
                CR " "
                CR "            CALL ChangeToAdmin "                // If changes failed, try as admin
                CR "            IF LastResult == ok GOTO aero_check: "
                CR " "
                CR "aero_end: "                                     ////// End of 'Windows 7 aero desktop fix' //////
                CR "            CALL ChangeFromAdmin "              
                CR "            CALL InstallNewsItems "             // Install pending news
                CR "            GOTO launch: "
                CR " "
                CR "crashed: "
                CR "            CALL ShowCrashFailDialog "
                CR "            IF LastResult == ok GOTO initial: "
                CR "            CALL Quit "
                CR " "
                CR "launch: ";


    m_pSequencer = new CSequencerType ();
    m_pSequencer->SetSource ( this, strSource );
    m_pSequencer->AddFunction ( "ShowCrashFailDialog",     &CInstallManager::_ShowCrashFailDialog );
    m_pSequencer->AddFunction ( "CheckOnRestartCommand",   &CInstallManager::_CheckOnRestartCommand );
    m_pSequencer->AddFunction ( "MaybeSwitchToTempExe",    &CInstallManager::_MaybeSwitchToTempExe );
    m_pSequencer->AddFunction ( "SwitchBackFromTempExe",   &CInstallManager::_SwitchBackFromTempExe );
    m_pSequencer->AddFunction ( "InstallFiles",            &CInstallManager::_InstallFiles );
    m_pSequencer->AddFunction ( "ChangeToAdmin",           &CInstallManager::_ChangeToAdmin );
    m_pSequencer->AddFunction ( "ShowCopyFailDialog",      &CInstallManager::_ShowCopyFailDialog );
    m_pSequencer->AddFunction ( "ProcessLayoutChecks",     &CInstallManager::_ProcessLayoutChecks );
    m_pSequencer->AddFunction ( "ProcessAeroChecks",       &CInstallManager::_ProcessAeroChecks );
    m_pSequencer->AddFunction ( "ChangeFromAdmin",         &CInstallManager::_ChangeFromAdmin );
    m_pSequencer->AddFunction ( "InstallNewsItems",        &CInstallManager::_InstallNewsItems );
    m_pSequencer->AddFunction ( "Quit",                    &CInstallManager::_Quit );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::Continue
//
// Process next step
//
//////////////////////////////////////////////////////////
SString CInstallManager::Continue ( const SString& strCommandLineIn  )
{
    ShowSplash ( g_hInstance );

    // Update some settings which are used by ReportLog
    UpdateSettingsForReportLog ();
    ClearPendingBrowseToSolution ();

    // Restore sequencer
    InitSequencer ();
    RestoreSequencerFromSnapshot ( strCommandLineIn );

    // If command line says we're not running from the launch directory, get the launch directory location from the registry
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
        SetMTASAPathSource ( true );
    else
        SetMTASAPathSource ( false );

    // Initial report line
    DWORD dwProcessId = GetCurrentProcessId();
    SString GotPathFrom = ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" ) ? "registry" : "module location";
    AddReportLog ( 1041, SString ( "* Launch * pid:%d '%s' MTASAPath set from %s '%s'", dwProcessId, GetMTASAModuleFileName ().c_str (), GotPathFrom.c_str (), GetMTASAPath ().c_str () ) );

    // Run sequencer
    for ( int i = 0 ; !m_pSequencer->AtEnd () && i < 1000 ; i++ )
        m_pSequencer->ProcessNextLine ();

    // Remove unwanted files
    CleanDownloadCache ();

    // Extract command line launch args
    SString strCommandLineOut;
    for ( int i = 0 ; i < m_pSequencer->GetVariableInt ( "_argc" ) ; i++ )
        strCommandLineOut += m_pSequencer->GetVariable ( SString ( "_arg_%d", i ) ) + " ";

    AddReportLog ( 1060, SString ( "CInstallManager::Continue - return %s", *strCommandLineOut ) );
    return *strCommandLineOut.TrimEnd ( " " );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::RestoreSequencerFromSnapshot
//
// Set current sequencer position from a string
//
//////////////////////////////////////////////////////////
void CInstallManager::RestoreSequencerFromSnapshot ( const SString& strText )
{
    AddReportLog ( 1061, SString ( "CInstallManager::RestoreSequencerState %s", *strText ) );
    std::vector < SString > parts;
    strText.Split ( " ", parts );

    int iFirstArg = 0;
    if ( parts.size () > 0 && parts[0].Contains ( "=" ) )
    {
        // Upgrade variables
        if ( parts[0].Contains ( INSTALL_STAGE ) && !parts[0].Contains ( "_pc_label" ) )
            parts[0] = parts[0].Replace ( INSTALL_STAGE, "_pc_label" );

        m_pSequencer->RestoreStateFromString ( parts[0] );
        iFirstArg++;
    }

    // Add any extra command line args
    if ( m_pSequencer->GetVariableInt ( "_argc" ) == 0 )
    {
        m_pSequencer->SetVariable ( "_argc", parts.size () - iFirstArg );
        for ( uint i = iFirstArg ; i < parts.size () ; i++ )
        {
            m_pSequencer->SetVariable ( SString ( "_arg_%d", i - iFirstArg ), parts[i] );
        }
    }

    // Ignore LastResult
    m_pSequencer->SetVariable ( "LastResult", "ok" );
}


//////////////////////////////////////////////////////////
//
// CInstallManager::GetSequencerSnapshot
//
// Save current sequencer position to a string
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetSequencerSnapshot ( void )
{
    m_pSequencer->SetVariable ( "LastResult", "" );
    return m_pSequencer->SaveStateToString ();
}


//////////////////////////////////////////////////////////
//
// CInstallManager::GetLauncherPathFilename
//
// Get path to launch exe
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetLauncherPathFilename ( void )
{
    SString strLocation = m_pSequencer->GetVariable( INSTALL_LOCATION );
    SString strResult = ( ( strLocation == "far" ? GetCurrentDirectory() : GetMTASAPath() ) +  MTA_EXE_NAME ).convert_ansi();
    AddReportLog( 1062, SString ( "GetLauncherPathFilename %s", *strResult ) );
    return strResult;
}


//
// Functions called from the sequencer
//

//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeToAdmin
//
//
// Save the state of the sequencer and launch process as admin
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeToAdmin ( void )
{
    if ( !IsUserAdmin () )
    {
        MessageBox( NULL, SString ( "MTA:SA needs Administrator access for the following task:\n\n  '%s'\n\nPlease confirm in the next window.", *m_strAdminReason ), "Multi Theft Auto: San Andreas", MB_OK );
        SetIsBlockingUserProcess ();
        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteBlocking ( "runas", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
        {
            // Will return here once admin process has finished
            CreateSingleInstanceMutex ();
            UpdateSettingsForReportLog ();
            RestoreSequencerFromSnapshot ( ReceiveStringFromAdminProcess () );
            ClearIsBlockingUserProcess ();
            return "ok";    // This will appear as the result for _ChangeFromAdmin
        }
        CreateSingleInstanceMutex ();
        ClearIsBlockingUserProcess ();
        MessageBox( NULL, SString ( "MTA:SA could not complete the following task:\n\n  '%s'\n", *m_strAdminReason ), "Multi Theft Auto: San Andreas", MB_OK );
    }
    return "fail";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeFromAdmin
//
// Save the state of the sequencer and exit back to the user process
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeFromAdmin ( void )
{
    if ( IsUserAdmin () && IsBlockingUserProcess () )
    {
        SendStringToUserProcess ( GetSequencerSnapshot () );
        AddReportLog ( 1003, SString ( "CInstallManager::_ChangeToAdmin - exit(0) %s", "" ) );
        ExitProcess ( 0 );
    }
    return "fail";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCrashFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCrashFailDialog ( void )
{
    // Crashed before gta game started ?
    if ( WatchDogIsSectionOpen ( "L1" ) )
        WatchDogIncCounter ( "CR1" );

    HideSplash ();

    SString strMessage = GetApplicationSetting ( "diagnostics", "last-crash-info" );
    strMessage = strMessage.Replace ( "\r", "" ).Replace ( "\n", "\r\n" );

    SString strResult = ShowCrashedDialog ( g_hInstance, strMessage );
    HideCrashedDialog ();

    return strResult;
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_CheckOnRestartCommand
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_CheckOnRestartCommand ( void )
{
    // Check for pending update
    const SString strResult = CheckOnRestartCommand ();

    if ( strResult.Contains ( "install" ) )
    {
        // New settings for install
        m_pSequencer->SetVariable ( INSTALL_LOCATION, strResult.Contains ( "far" )    ? "far" : "near" );
        m_pSequencer->SetVariable ( SILENT_OPT,       strResult.Contains ( "silent" ) ? "yes" : "no" );
        return "ok";
    }
    else
    if ( !strResult.Contains ( "no update" ) )
    {
        AddReportLog ( 4047, SString ( "_CheckOnRestartCommand: CheckOnRestartCommand returned %s", strResult.c_str () ) );
    }

    return "no_action";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_MaybeSwitchToTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_MaybeSwitchToTempExe ( void )
{
    // If a new "Multi Theft Auto.exe" exists, let that complete the install
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
    {
        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteNonBlocking ( "open", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
            ExitProcess ( 0 );     // All done here
        CreateSingleInstanceMutex ();
        return "fail";
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_SwitchBackFromTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_SwitchBackFromTempExe ( void )
{
    // If currently running temp install exe, switch back
    if ( m_pSequencer->GetVariable ( INSTALL_LOCATION ) == "far" )
    {
        m_pSequencer->SetVariable ( INSTALL_LOCATION, "near" );

        ReleaseSingleInstanceMutex ();
        if ( ShellExecuteNonBlocking ( "open", GetLauncherPathFilename (), GetSequencerSnapshot () ) )
            ExitProcess ( 0 );     // All done here
        CreateSingleInstanceMutex ();
        return "fail";
    }
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallFiles
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallFiles ( void )
{
    WatchDogReset ();
    // Install new files
    if ( !InstallFiles ( m_pSequencer->GetVariable ( SILENT_OPT ) != "no" ) )
    {
        if ( !IsUserAdmin () )
            AddReportLog ( 3048, SString ( "_InstallFiles: Install - trying as admin %s", "" ) );
        else
            AddReportLog ( 5049, SString ( "_InstallFiles: Couldn't install files %s", "" ) );

        m_strAdminReason = "Install updated MTA:SA files";
        return "fail";
    }
    else
    {
        UpdateMTAVersionApplicationSetting ();
        AddReportLog ( 2050, SString ( "_InstallFiles: ok %s", "" ) );
        return "ok";
    }
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCopyFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCopyFailDialog ( void )
{
    int iResponse = MessageBox ( NULL, "Could not update due to file conflicts. Please close other applications and retry", "Error", MB_RETRYCANCEL | MB_ICONERROR );
    if ( iResponse == IDRETRY )
        return "retry";
    return "ok";
}


void ShowLayoutError ( const SString& strExtraInfo )
{
    SString errorMsg( "Multi Theft Auto has not been installed properly, please reinstall. %s", *strExtraInfo );

    MessageBoxA( 0, errorMsg.c_str(), "Error", MB_OK );
    
    throw std::exception( "layout error" );
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLayoutChecks
//
// Make sure new reg/dir structure is ok
//
//////////////////////////////////////////////////////////
#include <memory>

SString CInstallManager::_ProcessLayoutChecks ( void )
{
    //
    // Validation
    //

    // Check data dir exists
    {
        filePath dataDirLoc = GetMTADataPath();

        bool hasDataDir = false;
        {
            std::unique_ptr <CFileTranslator> mtaDataRoot( fileSystem->CreateTranslator( dataDirLoc ) );

            hasDataDir = ( mtaDataRoot != NULL );
        }

        if ( !hasDataDir )
        {
            // If it does not exist we attempt to create it.
            bool gotToCreateDir = false;
            {
                std::unique_ptr <CFileTranslator> minAccPoint( fileSystem->CreateSystemMinimumAccessPoint( dataDirLoc ) );

                if ( minAccPoint )
                {
                    gotToCreateDir = minAccPoint->CreateDir( dataDirLoc );
                }
            }

            if ( !gotToCreateDir )
            {
                ShowLayoutError( "[Data directory not present]" );   // Can't find directory
            }
        }
    }

    // Check data dir writable
    {
        std::unique_ptr <CFileTranslator> mtaDataRoot( fileSystem->CreateTranslator( GetMTADataPath() ) );

        if ( mtaDataRoot )
        {
            mtaDataRoot->Delete( "testdir/testfile.txt" );

            SString strContent = "test";
            if ( !FileSystem::TranslatorWriteData( mtaDataRoot.get(), "testdir/testfile.txt", strContent.c_str(), strContent.size() ) )
            {
                ShowLayoutError( "[Data directory not writable]" );   // Can't save file
            }

            mtaDataRoot->Delete( "testdir/testfile.txt" );
        }
    }

    // Check reg key writable
    {
        RemoveRegistryKey ( "testkeypath" );

        SString strValue = GetTimeString( true, true );
        SetRegistryValue ( "testkeypath", "testname", strValue );
        SString strValueCheck = GetRegistryValue ( "testkeypath", "testname" );
        if ( strValueCheck != strValue )
            ShowLayoutError ( "[Registry key not writable]" );   // Can't write reg key

        RemoveRegistryKey ( "testkeypath" );
    }

    // Check install dir writable
    {
        std::unique_ptr <CFileTranslator> mtaBaseRoot( fileSystem->CreateTranslator( GetMTASABaseDir() ) );

        if ( mtaBaseRoot )
        {
            mtaBaseRoot->Delete( "mta/testfile.txt" );

            SString strContent = "test";
            if ( !FileSystem::TranslatorWriteData( mtaBaseRoot.get(), "mta/testfile.txt", strContent.c_str(), strContent.size() ) )
                ShowLayoutError( "[Install directory not writable]" );   // Can't save file

            mtaBaseRoot->Delete( "mta/testfile.txt" );
        }
    }

    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessAeroChecks
//
// Change the link timestamp in gta_sa.exe to trick windows 7 into using aero
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessAeroChecks()
{
    // Check is Windows 7
    if ( GetApplicationSetting ( "os-version" ) >= "6.1" )
    {
        filePath strGTAPath;
        if ( GetGamePath( strGTAPath ) == GAME_PATH_OK )
        {
            std::wstring strGTAEXEPath = ( strGTAPath + "\\" + MTA_GTAEXE_NAME ).convert_unicode();
            // Get the top byte of the file link timestamp
            uchar ucTimeStamp = 0;

            FILE* fh = _wfopen( strGTAEXEPath.c_str(), L"rb" );
            if ( fh )
            {
                if ( !fseek ( fh, 0x8B, SEEK_SET ) )
                {
                    if ( fread ( &ucTimeStamp, sizeof ( ucTimeStamp ), 1, fh ) != 1 )
                    {
                        ucTimeStamp = 0;
                    }
                }
                fclose ( fh );
            }

            const uchar AERO_DISABLED = 0x42;
            const uchar AERO_ENABLED  = 0x43;

            // Check it's a value we're expecting
            bool bCanChangeAeroSetting = ( ucTimeStamp == AERO_DISABLED || ucTimeStamp == AERO_ENABLED );
            SetApplicationSettingInt ( "aero-changeable", bCanChangeAeroSetting );

            if ( bCanChangeAeroSetting )
            {
                uchar ucTimeStampRequired = GetApplicationSettingInt ( "aero-enabled" ) ? AERO_ENABLED : AERO_DISABLED;
                if ( ucTimeStamp != ucTimeStampRequired )
                {
                    // Change needed!
                    FILE* fh = _wfopen( strGTAEXEPath.c_str(), L"r+b" );
                    if ( !fh )
                    {
                        m_strAdminReason = "Update Aero setting";
                        return "fail";
                    }
                    if ( !fseek ( fh, 0x8B, SEEK_SET ) )
                    {
                        fwrite ( &ucTimeStampRequired, sizeof ( ucTimeStampRequired ), 1, fh );
                    }
                    fclose ( fh );
                }
            }
        }
    }
    return "ok";
}



//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallNewsItems
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallNewsItems ( void )
{
    return "ok";
}


//////////////////////////////////////////////////////////
//
// CInstallManager::_Quit
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_Quit ( void )
{
    ExitProcess ( 0 );
    //return "ok";
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Utils.cpp
*  PURPOSE:     Loading utilities
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <assert.h>
#include <tchar.h>
#include <strsafe.h>
#include <Tlhelp32.h>
#define BUFSIZE 512

static bool bCancelPressed = false;
static bool bOkPressed = false;
static bool bOtherPressed = false;
static int iOtherCode = 0;
static HWND hwndSplash = NULL;
static unsigned long ulSplashStartTime = 0;
static HWND hwndProgressDialog = NULL;
static unsigned long ulProgressStartTime = 0;
static filePath g_strMTASAPath;
static HWND hwndCrashedDialog = NULL;
static HWND hwndD3dDllDialog = NULL;
HANDLE g_hMutex = NULL;
HMODULE hLibraryModule = NULL;

HMODULE RemoteLoadLibrary( HANDLE hProcess, const char *szLibPath )
{
    /* Called correctly? */
    if ( szLibPath == NULL )
    {
        return 0;
    }

    size_t pathLen = strlen( szLibPath ) + 1;

    /* Allocate memory in the remote process for the library path */
    HANDLE hThread = INVALID_HANDLE_VALUE;
    void *pLibPathRemote = NULL;

    HMODULE hKernel32 = GetModuleHandle( "Kernel32" );
    pLibPathRemote = VirtualAllocEx( hProcess, NULL, pathLen, MEM_COMMIT, PAGE_READWRITE );
    
    if ( pLibPathRemote == NULL )
    {
        return 0;
    }

    /* Make sure pLibPathRemote is always freed */
    __try
    {
        size_t dirPathLen = pathLen;
        DWORD byteswritten;

        // First write the library location
        for (;;)
        {
            switch( szLibPath[--dirPathLen] )
            {
            case '/':
            case '\\':
                goto seperatedPath;
            }
        }

seperatedPath:
        char buf[MAX_PATH];
        strncpy( buf, szLibPath, dirPathLen );

        buf[dirPathLen++] = 0;

        WriteProcessMemory( hProcess, pLibPathRemote, buf, dirPathLen, &byteswritten );

        if ( byteswritten != dirPathLen )
            return NULL;

        // Set the module location
        hThread = CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress( hKernel32, "SetDllDirectoryA" ), pLibPathRemote, 0, NULL );

        if ( hThread == INVALID_HANDLE_VALUE )
            return NULL;

        // Finish the preloader
        WaitForObject( hProcess, hThread, INFINITE, NULL );

        /* Write the DLL library path to the remote allocation */
        WriteProcessMemory( hProcess, pLibPathRemote, (void*)szLibPath, pathLen, &byteswritten );

        if ( byteswritten != pathLen )
            return NULL;

        /* Start a remote thread executing LoadLibraryA exported from Kernel32. Passing the
           remotly allocated path buffer as an argument to that thread (and also to LoadLibraryA)
           will make the remote process load the DLL into it's userspace (giving the DLL full
           access to the game executable).*/
        hThread = CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress( hKernel32, "LoadLibraryA" ), pLibPathRemote, 0, NULL);

        if ( hThread == INVALID_HANDLE_VALUE )
            return NULL;

    }
    __finally
    {
        VirtualFreeEx( hProcess, pLibPathRemote, pathLen, MEM_RELEASE );
    }

    /*  We wait for the created remote thread to finish executing. When it's done, the DLL
        is loaded into the game's userspace, and we can destroy the thread-handle. We wait
        5 seconds which is way longer than this should take to prevent this application
        from deadlocking if something goes really wrong allowing us to kill the injected
        game executable and avoid user inconvenience.*/
    WaitForObject ( hProcess, hThread, INFINITE, NULL );


    /* Get the handle of the remotely loaded DLL module */
    DWORD hLibModule = 0;
    GetExitCodeThread ( hThread, &hLibModule );


    /* Clean up the resources we used to inject the DLL */
    VirtualFreeEx (hProcess, pLibPathRemote, pathLen, MEM_RELEASE );

    /* Success */
    return ( HINSTANCE )( 1 );
}


///////////////////////////////////////////////////////////////////////////
//
// devicePathToWin32Path
//
// Code from the merky depths of MSDN
//
///////////////////////////////////////////////////////////////////////////
SString devicePathToWin32Path ( const SString& strDevicePath ) 
{
    TCHAR pszFilename[MAX_PATH+2];
    strncpy ( pszFilename, strDevicePath, MAX_PATH );
    pszFilename[MAX_PATH] = 0;

    // Translate path with device name to drive letters.
    TCHAR szTemp[BUFSIZE];
    szTemp[0] = '\0';

    if (GetLogicalDriveStrings(BUFSIZE-1, szTemp)) 
    {
        TCHAR szName[MAX_PATH];
        TCHAR szDrive[3] = TEXT(" :");
        BOOL bFound = FALSE;
        TCHAR* p = szTemp;

        do 
        {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, MAX_PATH))
            {
                UINT uNameLen = _tcslen(szName);

                if (uNameLen < MAX_PATH) 
                {
                    bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0;

                    if (bFound && *(pszFilename + uNameLen) == _T('\\')) 
                    {
                        // Reconstruct pszFilename using szTempFile
                        // Replace device path with DOS path
                        TCHAR szTempFile[MAX_PATH];
                        StringCchPrintf(szTempFile,
                        MAX_PATH,
                        TEXT("%s%s"),
                        szDrive,
                        pszFilename+uNameLen);
                        StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
                    }
                }
            }
            // Go to the next NULL character.
            while (*p++);

        } while (!bFound && *p); // end of string
    }
    return pszFilename;
}


typedef WINBASEAPI BOOL (WINAPI *LPFN_QueryFullProcessImageNameA)(__in HANDLE hProcess, __in DWORD dwFlags, __out_ecount_part(*lpdwSize, *lpdwSize) LPSTR lpExeName, __inout PDWORD lpdwSize);

///////////////////////////////////////////////////////////////////////////
//
// GetPossibleProcessPathFilenames
//
// Get all image names for a processID
//
///////////////////////////////////////////////////////////////////////////
std::vector < SString > GetPossibleProcessPathFilenames ( DWORD processID )
{
    static LPFN_QueryFullProcessImageNameA fnQueryFullProcessImageNameA = NULL;
    static bool bDoneGetProcAddress = false;

    std::vector < SString > result;

    if ( !bDoneGetProcAddress )
    {
        // Find 'QueryFullProcessImageNameA'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle ( "Kernel32.dll" );
        fnQueryFullProcessImageNameA = static_cast < LPFN_QueryFullProcessImageNameA > ( static_cast < PVOID > ( GetProcAddress( hModule, "QueryFullProcessImageNameA" ) ) );
    }

    if ( fnQueryFullProcessImageNameA )
    {
        for ( int i = 0 ; i < 2 ; i++ )
        {
            HANDLE hProcess = OpenProcess ( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

            if ( hProcess )
            {
                TCHAR szProcessName[MAX_PATH] = TEXT("");
                DWORD dwSize = sizeof(szProcessName)/sizeof(TCHAR);
                DWORD bOk = fnQueryFullProcessImageNameA ( hProcess, 0, szProcessName, &dwSize );
                CloseHandle( hProcess );

                if ( bOk && strlen ( szProcessName ) > 0 )
                    ListAddUnique ( result, SString ( SStringX ( szProcessName ) ) );
            }
        }
    }

    {
        HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

        if ( hProcess )
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("");
            DWORD bOk = GetModuleFileNameEx ( hProcess, NULL, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            CloseHandle ( hProcess );

            if ( bOk && strlen ( szProcessName ) > 0 )
                ListAddUnique ( result, SString ( SStringX ( szProcessName ) ) );
        }
    }

    for ( int i = 0 ; i < 2 ; i++ )
    {
        HANDLE hProcess = OpenProcess ( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

        if ( hProcess )
        {
            TCHAR szProcessName[MAX_PATH] = TEXT("");
            DWORD bOk = GetProcessImageFileName ( hProcess, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            CloseHandle( hProcess );

            if ( bOk && strlen ( szProcessName ) > 0 )
                ListAddUnique ( result, SString ( SStringX ( devicePathToWin32Path ( szProcessName ) ) ) );
        }
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////
//
// MyEnumProcesses
//
//
//
///////////////////////////////////////////////////////////////////////////
std::vector < DWORD > MyEnumProcesses ( void )
{
    uint uiSize = 200;
    std::vector < DWORD > processIdList;
    while ( true )
    {
        processIdList.resize ( uiSize );
        DWORD pBytesReturned = 0;
        if ( !EnumProcesses ( &processIdList[0], uiSize * sizeof(DWORD), &pBytesReturned ) )
        {
            processIdList.clear ();
            break;
        }
        uint uiNumProcessIds = pBytesReturned / sizeof(DWORD);

        if ( uiNumProcessIds != uiSize )
        {
            processIdList.resize ( uiNumProcessIds );
            break;
        }

        uiSize *= 2;
    }

    return processIdList;
}


///////////////////////////////////////////////////////////////////////////
//
// FindProcessId
//
// Find a process id by process name
//
///////////////////////////////////////////////////////////////////////////
DWORD FindProcessId ( const SString& processName )
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof ( processInfo );

	HANDLE processesSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, NULL );
	if ( processesSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	Process32First ( processesSnapshot , &processInfo );
    do
	{
		if ( processName.CompareI ( processInfo.szExeFile ) )
		{
			CloseHandle ( processesSnapshot );
			return processInfo.th32ProcessID;
		}
	}
	while ( Process32Next ( processesSnapshot, &processInfo ) );
	
	CloseHandle ( processesSnapshot );
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//
// GetGTAProcessList
//
// Get list of process id's with the image name ending in "gta_sa.exe"
//
///////////////////////////////////////////////////////////////////////////
std::vector < DWORD > GetGTAProcessList ( void )
{
    std::vector < DWORD > result;

    std::vector < DWORD > processIdList = MyEnumProcesses ();
    for ( uint i = 0; i < processIdList.size (); i++ )
    {
        DWORD processId = processIdList[i];
        // Skip 64 bit processes to avoid errors
        if ( !Is32bitProcess ( processId ) )
            continue;

        std::vector < SString > filenameList = GetPossibleProcessPathFilenames ( processId );
        for ( uint i = 0; i < filenameList.size (); i++ )
            if ( filenameList[i].EndsWith ( MTA_GTAEXE_NAME ) )
                result.push_back ( processId );
    }

    if ( DWORD processId = FindProcessId ( MTA_GTAEXE_NAME ) )
        ListAddUnique ( result, processId );

    return result;
}


///////////////////////////////////////////////////////////////////////////
//
// TerminateGTAIfRunning
//
//
//
///////////////////////////////////////////////////////////////////////////
bool TerminateGTAIfRunning ( bool bSilent )
{
    std::vector < DWORD > processIdList = GetGTAProcessList ();

    if ( processIdList.size () )
    {
        if ( !bSilent )
            if ( MessageBox ( 0, "An instance of GTA: San Andreas is already running. It needs to be terminated before MTA:SA can be started. Do you want to do that now?", "Information", MB_YESNO | MB_ICONQUESTION ) == IDNO )
                return false;

        // Try to stop all GTA process id's
        for ( uint i = 0 ; i < 3 && processIdList.size () ; i++ )
        {
            for ( std::vector < DWORD > ::iterator iter = processIdList.begin () ; iter != processIdList.end (); ++iter )
            {
                HANDLE hProcess = OpenProcess ( PROCESS_TERMINATE, 0, *iter );
                if ( hProcess )
                {
                    TerminateProcess ( hProcess, 0 );
                    CloseHandle ( hProcess );
                }
            }
            Sleep ( 1000 );
            processIdList = GetGTAProcessList ();
        }

        if ( processIdList.size () )
        {
            if ( !bSilent )
                MessageBox ( 0, "Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer.", "Information", MB_OK | MB_ICONQUESTION );
            return false;
        }
    }
    return true;
}


int CALLBACK DialogProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{ 
    switch (uMsg) 
    { 
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            {  
                case IDCANCEL:
                    bCancelPressed = true;
                    return TRUE; 
                case IDOK:
                    bOkPressed = true;
                    return TRUE; 
                default:
                    if ( iOtherCode && iOtherCode == LOWORD(wParam) )
                    {
                        bOtherPressed = true;
                        return TRUE; 
                    }
            } 
    } 
    return FALSE; 
} 


//
// Show splash dialog
//
void ShowSplash ( HINSTANCE hInstance )
{
#ifndef MTA_DEBUG
    if ( !hwndSplash )
    {
        hwndSplash = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc );
        ulSplashStartTime = GetTickCount32 ();
    }
    SetForegroundWindow ( hwndSplash );
    SetWindowPos ( hwndSplash, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif
}


//
// Hide splash dialog
//
void HideSplash ( bool bOnlyDelay  )
{
    if ( hwndSplash )
    {
        // Show splash for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount32 () - ulSplashStartTime;
        if ( ulTimeElapsed < 2000 )
            Sleep ( 2000 - ulTimeElapsed );

        if ( !bOnlyDelay )
        {
            DestroyWindow ( hwndSplash );
            hwndSplash = NULL;
        }
    }
}


//
// General error message box
//
long DisplayErrorMessageBox ( const SString& strMessage, const SString& strTroubleType )
{
    HideSplash ();
    MessageBox( 0, strMessage, "Error! (CTRL+C to copy)", MB_ICONEXCLAMATION|MB_OK );

    if ( strTroubleType != "" )
        BrowseToSolution ( strTroubleType, true );
    return 1;
}


filePath GetMTASAModuleFileName ( void )
{
    // Get current module full path
    wchar_t szBuffer[64000];
    GetModuleFileNameW( NULL, szBuffer, NUMELMS(szBuffer) - 1 );

    szBuffer[ NUMELMS(szBuffer) - 1 ] = 0;

    return szBuffer;
}


filePath GetLaunchPath ( void )
{
    // Get current module full path
    wchar_t szBuffer[64000];
    GetModuleFileNameW( NULL, szBuffer, NUMELMS(szBuffer) - 1 );

    szBuffer[ NUMELMS(szBuffer) - 1 ] = 0;

    // Strip the module name out of the path.
    PathRemoveFileSpecW( szBuffer );

    filePath result( szBuffer );
    result += "\\";

    return result;
}


void SetMTASAPathSource ( bool bReadFromRegistry )
{
    if ( bReadFromRegistry )
    {
        g_strMTASAPath = GetMTASABaseDir();
    }
    else
    {
        // Get current module full path
        char szBuffer[64000];
        GetModuleFileName ( NULL, szBuffer, sizeof(szBuffer) - 1 );

        SString strHash = "-";
        {
            MD5 md5;
            CMD5Hasher Hasher;
            if ( Hasher.Calculate ( szBuffer, md5 ) )
            {
                char szHashResult[33];
                Hasher.ConvertToHex ( md5, szHashResult );
                strHash = szHashResult;
            }
        }

        SetRegistryValue ( "", "Last Run Path", szBuffer );
        SetRegistryValue ( "", "Last Run Path Hash", strHash );
        SetRegistryValue ( "", "Last Run Path Version", MTA_DM_ASE_VERSION );

        // Also save for legacy 1.0 to see
        SString strThisVersion = SStringX ( MTA_DM_ASE_VERSION ).TrimEnd ( "n" );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path", szBuffer );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path Hash", strHash );
        SetVersionRegistryValueLegacy ( strThisVersion, "", "Last Run Path Version", MTA_DM_ASE_VERSION );

        // Strip the module name out of the path.
        PathRemoveFileSpec ( szBuffer );

        // Save to a temp registry key
        SetRegistryValue ( "", "Last Run Location", szBuffer );
        g_strMTASAPath = szBuffer;
    }
}


filePath GetMTASAPath ( void )
{
    if ( g_strMTASAPath == "" )
        SetMTASAPathSource ( true );
    return g_strMTASAPath + "\\";
}


///////////////////////////////////////////////////////////////
//
// LookForGtaProcess
//
//
//
///////////////////////////////////////////////////////////////
bool LookForGtaProcess ( SString& strOutPathFilename )
{
    std::vector < DWORD > processIdList = GetGTAProcessList ();
    for ( uint i = 0 ; i < processIdList.size () ; i++ )
    {
        std::vector < SString > filenameList = GetPossibleProcessPathFilenames ( processIdList[i] );
        for ( uint i = 0 ; i < filenameList.size () ; i++ )
        {
            if ( fileSystem->Exists( filenameList[i] ) )
            {
                strOutPathFilename = filenameList[i];
                return true;
            }
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// DoUserAssistedSearch
//
//
//
///////////////////////////////////////////////////////////////
SString DoUserAssistedSearch ( void )
{
    SString strResult;

    ShowProgressDialog( g_hInstance, "Searching for Grand Theft Auto San Andreas", true );

    while ( !UpdateProgress ( 0, 100, "Please start Grand Theft Auto San Andreas" ) )
    {
        SString strPathFilename;
        // Check if user has started GTA
        if ( LookForGtaProcess ( strPathFilename ) )
        {
            // If so, get the exe path
            ExtractFilename ( strPathFilename, &strResult, NULL );
            // And then stop it
            TerminateGTAIfRunning ( true );
            break;
        }

        Sleep ( 200 );
    }

    HideProgressDialog ();
    return strResult;
}


///////////////////////////////////////////////////////////////
//
// GetGamePath
//
//
//
///////////////////////////////////////////////////////////////
inline ePathResult hasDirectoryGTASAExecutable( const filePath& fullDirName )
{
    CFileTranslator *dirTrans = fileSystem->CreateTranslator( fullDirName + "\\" );

    ePathResult result = GAME_PATH_MISSING;

    if ( dirTrans )
    {
        if ( dirTrans->Exists( MTA_GTAEXE_NAME ) )
        {
            result = GAME_PATH_OK;
        }
        else if ( dirTrans->Exists( MTA_GTASTEAMEXE_NAME ) )
        {
            result = GAME_PATH_STEAM;
        }

        delete dirTrans;
    }

    return result;
}

ePathResult GetGamePath( filePath& strOutResult, bool bFindIfMissing )
{
    // Registry places to look
    std::vector <filePath> pathList;

    // Try HKLM "SOFTWARE\\Multi Theft Auto: San Andreas All\\Common\\"
    pathList.push_back( filePath( GetCommonRegistryValue( "", "GTA:SA Path" ) ) );
    // Then HKCU "SOFTWARE\\Multi Theft Auto: San Andreas 1.0\\"
    pathList.push_back( filePath( GetVersionRegistryValueLegacy( "1.0", "", "GTA:SA Path" ) ) );
    // Then HKCU "SOFTWARE\\Multi Theft Auto: San Andreas 1.1\\"
    pathList.push_back( filePath( GetVersionRegistryValueLegacy( "1.1", "", "GTA:SA Path Backup" ) ) );

    // Then step through looking for an existing file
    bool bFoundSteamExe = false;
    filePath strRegPath;
    for ( uint i = 0 ; i < pathList.size (); i++ )
    {
        ePathResult subResult = hasDirectoryGTASAExecutable( pathList[i] );

        if ( subResult == GAME_PATH_OK )
        {
            strRegPath = pathList[i];
            break;
        }
        else if ( subResult == GAME_PATH_STEAM )
        {
            bFoundSteamExe = true;
        }
    }

    // Found an exe?
    if ( !strRegPath.empty () )
    {
        // Update registry.
        SetCommonRegistryValue ( "", "GTA:SA Path", strRegPath.convert_ansi() );

        strOutResult = std::move( strRegPath );
        return GAME_PATH_OK;
    }

    // Found a steam exe?
    if ( bFoundSteamExe )
    {
        return GAME_PATH_STEAM;
    }

    // Try to find?
    if ( !bFindIfMissing )
        return GAME_PATH_MISSING;


    // Ask user to browse for GTA
    BROWSEINFOA bi = { 0 };
    bi.lpszTitle = "Select your Grand Theft Auto: San Andreas Installation Directory";
    LPITEMIDLIST pidl = SHBrowseForFolderA( &bi );

    if ( pidl != 0 )
    {
        char szBuffer[MAX_PATH];
        // get the name of the  folder
        if ( SHGetPathFromIDListA( pidl, szBuffer ) )
        {
            strRegPath = szBuffer;
        }

        // free memory used
        CoTaskMemFree( pidl );
    }

    // Check browse result
    {
        ePathResult browsePathResult = hasDirectoryGTASAExecutable( strRegPath );

        if ( browsePathResult != GAME_PATH_MISSING )
        {
            if ( browsePathResult == GAME_PATH_OK )
            {
                // File found. Update registry.
                SetCommonRegistryValue( "", "GTA:SA Path", strRegPath.convert_ansi() );

                strOutResult = std::move( strRegPath );
            }

            return browsePathResult;
        }
    }

    // If browse didn't help, try another method
    strRegPath = DoUserAssistedSearch();
    {
        ePathResult userAssistResult = hasDirectoryGTASAExecutable( strRegPath );

        if ( userAssistResult != GAME_PATH_MISSING )
        {
            if ( userAssistResult == GAME_PATH_OK )
            {
                // File found. Update registry.
                SetCommonRegistryValue( "", "GTA:SA Path", strRegPath.convert_ansi() );

                strOutResult = std::move( strRegPath );
            }

            return userAssistResult;
        }
    }

    return GAME_PATH_MISSING;
}


///////////////////////////////////////////////////////////////
//
// Progress dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowProgressDialog( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel )
{
    if ( !hwndProgressDialog )
    {
        HideSplash ();
        bCancelPressed = false;
        hwndProgressDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_PROGRESS_DIALOG), 0, DialogProc );
        SetWindowText ( hwndProgressDialog, strTitle );
        HWND hwndButton = GetDlgItem( hwndProgressDialog, IDCANCEL );
        ShowWindow( hwndButton, bAllowCancel ? SW_SHOW : SW_HIDE );
        ulProgressStartTime = GetTickCount32 ();
    }
    SetForegroundWindow ( hwndProgressDialog );
    SetWindowPos ( hwndProgressDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
}

void HideProgressDialog ( void )
{
    if ( hwndProgressDialog )
    {
        // Show progress for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount32 () - ulProgressStartTime;
        if ( ulTimeElapsed < 2000 )
        {
            UpdateProgress( 100, 100 );
            Sleep ( 2000 - ulTimeElapsed );
        }

        DestroyWindow ( hwndProgressDialog );
        hwndProgressDialog = NULL;
    }
}


// returns true if canceled
bool UpdateProgress( int iPos, int iMax, const SString& strMsg )
{
    if ( hwndProgressDialog)
    {
        HWND hwndText = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_STATIC );
        char buffer[ 1024 ] = "";
        ::GetWindowText ( hwndText, buffer, sizeof(buffer) - 2 );
        if ( strMsg.length () > 0 && strMsg != buffer )
            SetWindowText ( hwndText, strMsg );
        HWND hwndBar = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_BAR );
        PostMessage(hwndBar, PBM_SETPOS, iPos * 100 / Max ( 1, iMax ), 0 );
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return bCancelPressed;
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// Progress
//
//
//
///////////////////////////////////////////////////////////////
void StartPseudoProgress( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg )
{
    ShowProgressDialog ( hInstance, strTitle );
    UpdateProgress( 10, 100, strMsg );
    Sleep ( 100 );
    UpdateProgress( 30, 100 );
}

void StopPseudoProgress( void )
{
    if ( hwndProgressDialog )
    {
        UpdateProgress( 60, 100 );
        Sleep ( 100 );
        UpdateProgress( 90, 100 );
        Sleep ( 100 );
        HideProgressDialog ();
    }
}




///////////////////////////////////////////////////////////////
//
// Crashed dialog
//
//
//
///////////////////////////////////////////////////////////////
SString ShowCrashedDialog( HINSTANCE hInstance, const SString& strMessage )
{
    if ( !hwndCrashedDialog )
    {
        HideSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = 0;
        hwndCrashedDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_CRASHED_DIALOG), 0, DialogProc );
        SetWindowText ( GetDlgItem( hwndCrashedDialog, IDC_CRASH_INFO_EDIT ), strMessage );
        SendDlgItemMessage( hwndCrashedDialog, IDC_SEND_DUMP_CHECK, BM_SETCHECK, GetApplicationSetting ( "diagnostics", "send-dumps" ) != "no" ? BST_CHECKED : BST_UNCHECKED, 0 );
    }
    SetForegroundWindow ( hwndCrashedDialog );
    SetWindowPos ( hwndCrashedDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

    while ( !bCancelPressed && !bOkPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

    LRESULT res = SendMessageA( GetDlgItem( hwndCrashedDialog, IDC_SEND_DUMP_CHECK ), BM_GETCHECK, 0, 0 );
    SetApplicationSetting ( "diagnostics", "send-dumps", res ? "yes" : "no" );

    if ( bCancelPressed )
        return "quit";
    //if ( bOkPressed )
        return "ok";
}


void HideCrashedDialog ( void )
{
    if ( hwndCrashedDialog )
    {
        DestroyWindow ( hwndCrashedDialog );
        hwndCrashedDialog = NULL;
    }
}


///////////////////////////////////////////////////////////////
//
// d3d dll dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowD3dDllDialog( HINSTANCE hInstance, const SString& strPath )
{
    // Calc hash of target file
    SString strFileHash;
    MD5 md5;
    CMD5Hasher Hasher;
    if ( Hasher.Calculate ( strPath, md5 ) )
    {
        char szHashResult[33];
        Hasher.ConvertToHex ( md5, szHashResult );
        strFileHash = szHashResult;
    }

	// Maybe skip dialog
    if ( GetApplicationSetting ( "diagnostics", "d3d9-dll-last-hash" ) == strFileHash )
    {
        if ( GetApplicationSetting ( "diagnostics", "d3d9-dll-not-again" ) == "yes" )
            return;
    }

	// Create and show dialog
    if ( !hwndD3dDllDialog )
    {
        HideSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_SHOW_DIR;
        hwndD3dDllDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_D3DDLL_DIALOG), 0, DialogProc );
        SetWindowText ( GetDlgItem( hwndD3dDllDialog, IDC_EDIT_D3DDLL_PATH ), strPath );
    }
    SetForegroundWindow ( hwndD3dDllDialog );
    SetWindowPos ( hwndD3dDllDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// Wait for input
    while ( !bCancelPressed && !bOkPressed && !bOtherPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

	// Process input
    LRESULT res = SendMessageA( GetDlgItem( hwndD3dDllDialog, IDC_CHECK_NOT_AGAIN ), BM_GETCHECK, 0, 0 );
    SetApplicationSetting ( "diagnostics", "d3d9-dll-last-hash", strFileHash );
    SetApplicationSetting ( "diagnostics", "d3d9-dll-not-again", res ? "yes" : "no" );

    if ( bCancelPressed )
    {
        ExitProcess(0);
    }
    if ( bOtherPressed )
    {
        if ( ITEMIDLIST *pidl = ILCreateFromPath ( strPath ) )
        {
            SHOpenFolderAndSelectItems ( pidl, 0, 0, 0 );
            ILFree ( pidl );
        }
        else
            ShellExecuteNonBlocking ( "open", ExtractPath ( strPath ) );

        ExitProcess(0);
    }
}


void HideD3dDllDialog ( void )
{
    if ( hwndD3dDllDialog )
    {
        DestroyWindow ( hwndD3dDllDialog );
        hwndD3dDllDialog = NULL;
    }
}


///////////////////////////////////////////////////////////////
//
// MakeRandomIndexList
//
// Create a list of randomlu ordered indices from 0 to Size-1
//
///////////////////////////////////////////////////////////////
void MakeRandomIndexList ( int Size, std::vector < int >& outList )
{
    for ( int i = 0 ; i < Size ; i ++ )
        outList.push_back ( i );

    for ( int i = 0 ; i < Size ; i ++ )
    {
        int otherIdx = rand() % Size;
        int Temp = outList[i];
        outList[i] = outList[otherIdx];
        outList[otherIdx] = Temp;
    }
}


///////////////////////////////////////////////////////////////
//
// GetOSVersion
//
// Affected by compatibility mode
//
///////////////////////////////////////////////////////////////
SString GetOSVersion ( void )
{
    OSVERSIONINFO versionInfo;
    memset ( &versionInfo, 0, sizeof ( versionInfo ) );
    versionInfo.dwOSVersionInfoSize = sizeof ( versionInfo );
    GetVersionEx ( &versionInfo );
    return SString ( "%d.%d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion );
}


///////////////////////////////////////////////////////////////
//
// GetRealOSVersion
//
// Ignoring compatibility mode
//
///////////////////////////////////////////////////////////////
SString GetRealOSVersion ( void )
{
    SString strVersionAndBuild = GetWMIOSVersion ();
    std::vector < SString > parts;
    strVersionAndBuild.Split ( ".", parts );
    uint uiMajor = parts.size () > 0 ? atoi ( parts[0] ) : 0;
    uint uiMinor = parts.size () > 1 ? atoi ( parts[1] ) : 0;
    return SString ( "%u.%u", uiMajor, uiMinor );
}


///////////////////////////////////////////////////////////////
//
// IsUserAdmin
//
//
//
///////////////////////////////////////////////////////////////
BOOL IsUserAdmin(VOID)
/*++ 
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token. 
Arguments: None. 
Return Value: 
   TRUE - Caller has Administrators local group. 
   FALSE - Caller does not have Administrators local group. --
*/ 
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup; 
    b = AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &AdministratorsGroup); 
    if(b) 
    {
        if (!CheckTokenMembership( NULL, AdministratorsGroup, &b)) 
        {
             b = FALSE;
        } 
        FreeSid(AdministratorsGroup); 
    }

    return(b);
}


///////////////////////////////////////////////////////////////
//
// IsVistaOrHigher
//
//
//
///////////////////////////////////////////////////////////////
bool IsVistaOrHigher ( void )
{
    int iMajor = atoi ( GetRealOSVersion () );
    return iMajor >= 6;
}



static SString HashBuffer ( char* pData, uint uiLength )
{
    DWORD dwSum1 = 0;
    DWORD dwSum2 = 0x1234;
    for ( uint i = 0 ; i < uiLength ; i++ )
    {
        dwSum1 += pData[i];
        dwSum2 += pData[i];
        dwSum2 ^= ( dwSum2 << 2 ) + 0x93;
    }
    return SString ( "%08x%08x%08x", dwSum1, dwSum2, uiLength );
}


/////////////////////////////////////////////////////////////////////
//
// GetLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
HMODULE GetLibraryHandle( const filePath& strFilename )
{
    if ( !hLibraryModule )
    {
        // Get path to the relevant file
        filePath strLibPath = ( GetLaunchPath() + "mta/" );
        filePath strLibPathFilename = ( strLibPath + strFilename );

        filePath strPrevCurDir = SharedUtil::GetCurrentDirectory();

        std::wstring wideLibPath = strLibPath.convert_unicode();
        std::wstring wideLibPathFilename = strLibPathFilename.convert_unicode();

        SetCurrentDirectoryW( wideLibPath.c_str() );
        SetDllDirectoryW( wideLibPath.c_str() );

        hLibraryModule = LoadLibraryW( wideLibPathFilename.c_str() );

        std::wstring unicodePrevCurDir = strPrevCurDir.convert_unicode();

        SetCurrentDirectoryW ( unicodePrevCurDir.c_str() );
        SetDllDirectoryW( unicodePrevCurDir.c_str() );
    }

    return hLibraryModule;
}


/////////////////////////////////////////////////////////////////////
//
// FreeLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
void FreeLibraryHandle ( void )
{
    if ( hLibraryModule )
    {
        FreeLibrary ( hLibraryModule );
        hLibraryModule = NULL;
    }
}


/////////////////////////////////////////////////////////////////////
//
// UpdateMTAVersionApplicationSetting
//
// Make sure "mta-version-ext" is correct. eg "1.0.4-9.01234.2.000"
//
/////////////////////////////////////////////////////////////////////
void UpdateMTAVersionApplicationSetting ( void )
{
#ifdef MTA_DEBUG
    filePath strFilename = L"netc_d.dll";
#else
    filePath strFilename = L"netc.dll";
#endif

    //
    // Determine NetRev number
    //

    // Get saved status
    unsigned short usNetRev = 65535;
    unsigned short usNetRel = 0;
    std::vector < SString > parts;
    GetApplicationSetting ( "mta-version-ext" ).Split ( ".", parts );
    if ( parts.size () == 6 )
    {
        usNetRev = atoi ( parts[4] );
        usNetRel = atoi ( parts[5] );
    }

    HMODULE hModule = GetLibraryHandle ( strFilename );
    if ( hModule )
    {
        typedef unsigned short (*PFNGETNETREV) ( void );
        PFNGETNETREV pfnGetNetRev = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRev" ) ) );
        if ( pfnGetNetRev )
            usNetRev = pfnGetNetRev ();
        PFNGETNETREV pfnGetNetRel = static_cast < PFNGETNETREV > ( static_cast < PVOID > ( GetProcAddress ( hModule, "GetNetRel" ) ) );
        if ( pfnGetNetRel )
            usNetRel = pfnGetNetRel ();
    }
    else
    {
        std::string ansiFilename = strFilename.convert_ansi();

        SString strError = GetSystemErrorMessage ( GetLastError () );     
        SString strModnameLower = SString( ansiFilename ).ToLower();
        MessageBox ( 0, SString ( "Error loading %s module! (%s)", strModnameLower.c_str(), *strError ), "Error", MB_OK|MB_ICONEXCLAMATION );
        BrowseToSolution ( ansiFilename + "-not-loadable", true, true );
    }

    SetApplicationSetting ( "mta-version-ext", SString ( "%d.%d.%d-%d.%05d.%c.%03d"
                                ,MTASA_VERSION_MAJOR
                                ,MTASA_VERSION_MINOR
                                ,MTASA_VERSION_MAINTENANCE
                                ,MTASA_VERSION_TYPE
                                ,MTASA_VERSION_BUILD
                                ,usNetRev == 65535 ? '+' : usNetRev + '0'
                                ,usNetRel
                                ) );
}


///////////////////////////////////////////////////////////////////////////
//
// Is32bitProcess
//
// Determine if processID is a 32 bit process or not.
// Assumes current process is 32 bit.
//
// (Calling GetModuleFileNameEx or EnumProcessModules on a 64bit process from a 32bit process can cause problems)
//
///////////////////////////////////////////////////////////////////////////
bool Is32bitProcess ( DWORD processID )
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    static bool bDoneGetProcAddress = false;
    static bool bDoneIs64BitOS = false;
    static bool bIs64BitOS = false;

    if ( !bDoneGetProcAddress )
    {
        // Find 'IsWow64Process'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle ( "Kernel32.dll" );
        fnIsWow64Process = static_cast < LPFN_ISWOW64PROCESS > ( static_cast < PVOID > ( GetProcAddress( hModule, "IsWow64Process" ) ) );
    }

    // Function not there? Must be 32bit everything
    if ( !fnIsWow64Process )
        return true;


    // See if this is a 64 bit O/S
    if ( !bDoneIs64BitOS )
    {
        bDoneIs64BitOS = true;

        // We know current process is 32 bit. See if it is running under WOW64
        BOOL bIsWow64 = FALSE;
        BOOL bOk = fnIsWow64Process ( GetCurrentProcess (), &bIsWow64 );
        if ( bOk )
        {
            // Must be 64bit O/S if current (32 bit) process is running under WOW64
            if ( bIsWow64 )
                bIs64BitOS = true;
        }
    }

    // Not 64 bit O/S? Must be 32bit everything
    if ( !bIs64BitOS )
        return true;

    // Call 'IsWow64Process' on query process
    for ( int i = 0 ; i < 2 ; i++ )
    {
        HANDLE hProcess = OpenProcess( i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID );

        if ( hProcess )
        {
            BOOL bIsWow64 = FALSE;
            BOOL bOk = fnIsWow64Process ( hProcess, &bIsWow64 );
            CloseHandle( hProcess );

            if ( bOk )
            {
                if ( bIsWow64 == FALSE )
                    return false;       // 64 bit O/S and process not running under WOW64, so it must be a 64 bit process
                return true;
            }
        }
    }

    return false;   // Can't determine. Guess it's 64 bit
}


///////////////////////////////////////////////////////////////////////////
//
// CreateSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
bool CreateSingleInstanceMutex ( void )
{
    HANDLE hMutex = CreateMutex ( NULL, FALSE, TEXT( MTA_GUID ) );

    if ( GetLastError() == ERROR_ALREADY_EXISTS )
    {
        if ( hMutex )
            CloseHandle ( hMutex );
        return false;
    }
    assert ( !g_hMutex );
    g_hMutex = hMutex;
    return true;
}


///////////////////////////////////////////////////////////////////////////
//
// ReleaseSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
void ReleaseSingleInstanceMutex ( void )
{
    assert ( g_hMutex );
    CloseHandle ( g_hMutex );
    g_hMutex = NULL;
}


///////////////////////////////////////////////////////////////////////////
//
// WaitForObject
//
// Wait for object to finish
// Returns non-zero if wait failed.
//
///////////////////////////////////////////////////////////////////////////
uint WaitForObject ( HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex )
{
    uint uiResult = 0;

    HMODULE hModule = GetLibraryHandle ( "kernel32.dll" );

    if ( hModule )
    {
        typedef unsigned long (*PFNWaitForObject) ( HANDLE, ... );
        PFNWaitForObject pfnWaitForObject = static_cast< PFNWaitForObject > ( static_cast < PVOID > ( GetProcAddress ( hModule, "WaitForObject" ) ) );

        if ( !pfnWaitForObject || pfnWaitForObject ( hProcess, hThread, dwMilliseconds, hMutex ) )
            uiResult = 1;
    }

    return uiResult;
}


///////////////////////////////////////////////////////////////////////////
//
// GetFileAge
//
// Returns time in seconds since a file/directory was created
//
///////////////////////////////////////////////////////////////////////////
int GetFileAge ( const filePath& strPathFilename )
{
    std::wstring widePath = strPathFilename.convert_unicode();

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW ( widePath.c_str(), &findFileData );
    if ( hFind != INVALID_HANDLE_VALUE )
    {
        FindClose ( hFind );
        FILETIME ftNow;
        GetSystemTimeAsFileTime ( &ftNow );
        ULARGE_INTEGER creationTime = { findFileData.ftCreationTime.dwLowDateTime, findFileData.ftCreationTime.dwHighDateTime };
        ULARGE_INTEGER timeNow = { ftNow.dwLowDateTime, ftNow.dwHighDateTime };
        return static_cast < int > ( ( timeNow.QuadPart - creationTime.QuadPart ) / ( LONGLONG ) 10000000 );
    }
    return 0;
}


///////////////////////////////////////////////////////////////////////////
//
// CleanDownloadCache
//
// Remove old files from the download cache
//
///////////////////////////////////////////////////////////////////////////
void CleanDownloadCache ( void )
{
    const uint uiMaxCleanTime = 5;                      // Limit clean time (seconds)
    const uint uiCleanFileAge = 60 * 60 * 24 * 7;       // Delete files older than this

    const time_t tMaxEndTime = time ( NULL ) + uiMaxCleanTime;

    // Search possible cache locations
    std::list <filePath> cacheLocationList;
    cacheLocationList.push_back ( GetMTADataPath() + "upcache/" );
    cacheLocationList.push_back ( GetMTATempPath() + "upcache/" );
    cacheLocationList.push_back ( GetMTATempPath() );

    for ( ; !cacheLocationList.empty () ; cacheLocationList.pop_front () )
    {
        const filePath& strCacheLocation = cacheLocationList.front ();

        CFileTranslator *fileRoot = fileSystem->CreateTranslator( strCacheLocation );

        if ( fileRoot )
        {
            auto cleanupEntryFromDisk = [&]( const filePath& itemPath )
            {
                // Check if over 7 days old
                if ( GetFileAge( itemPath ) > uiCleanFileAge )
                {
                    // Delete as directory or file
                    fileRoot->Delete( itemPath );

#if 0
                    // Check time spent
                    if ( time ( NULL ) > tMaxEndTime )
                        break;
#endif
                }
            };

            // Get list of files & directories in this cache location
            fileRoot->ScanDirectory( "/", "*", false,
                cleanupEntryFromDisk,
                cleanupEntryFromDisk
            );

            delete fileRoot;
        }
    }
}

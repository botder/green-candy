/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Misc.hpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "UTF8.h"
#include "CNickGen.h"
#ifdef WIN32
#include <direct.h>
#include <shellapi.h>
#endif

#include <fstream>

#ifdef MTA_CLIENT
#ifdef WIN32

#define TROUBLE_URL1 "http://updatesa.multitheftauto.com/sa/trouble/?v=%VERSION%&id=%ID%&tr=%TROUBLE%"


#ifndef MTA_DM_ASE_VERSION
    #include <../../MTA10/version.h>
#endif


//
// Get startup directory as saved in the registry by the launcher
// Used in the Win32 Client only
//
SString SharedUtil::GetMTASABaseDir ( void )
{
    static SString strInstallRoot;
    if ( strInstallRoot.empty () )
    {
        strInstallRoot = GetRegistryValue ( "", "Last Run Location" );
        if ( strInstallRoot.empty () )
        {
            MessageBox ( 0, "Multi Theft Auto has not been installed properly, please reinstall.", "Error", MB_OK );
            TerminateProcess ( GetCurrentProcess (), 9 );
        }
    }
    return strInstallRoot + '\\';
}

//
// Write a registry string value
//
static void WriteRegistryStringValue ( HKEY hkRoot, const char* szSubKey, const char* szValue, const SString& strBuffer )
{
    HKEY hkTemp;
    RegCreateKeyEx ( hkRoot, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkTemp, NULL );
    if ( hkTemp )
    {
        RegSetValueEx ( hkTemp, szValue, NULL, REG_SZ, (LPBYTE)strBuffer.c_str (), strBuffer.length () + 1 );
        RegCloseKey ( hkTemp );
    }
}

//
// Read a registry string value
//
static SString ReadRegistryStringValue ( HKEY hkRoot, const char* szSubKey, const char* szValue, int* iResult )
{
    // Clear output
    SString strOutResult = "";

    bool bResult = false;
    HKEY hkTemp = NULL;
    if ( RegOpenKeyExA ( hkRoot, szSubKey, 0, KEY_READ, &hkTemp ) == ERROR_SUCCESS )
    {
        DWORD dwBufferSize;
        if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, NULL, &dwBufferSize ) == ERROR_SUCCESS )
        {
            char *szBuffer = static_cast < char* > ( alloca ( dwBufferSize + 1 ) );
            if ( RegQueryValueExA ( hkTemp, szValue, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize ) == ERROR_SUCCESS )
            {
                strOutResult = szBuffer;
                bResult = true;
            }
        }
        RegCloseKey ( hkTemp );
    }
    if ( iResult )
        *iResult = bResult;
    return strOutResult;
}


//
// Delete a registry key and its subkeys
//
static bool DeleteRegistryKey ( HKEY hkRoot, const char* szSubKey )
{
    return RegDeleteKey ( hkRoot, szSubKey ) == ERROR_SUCCESS;
}




//
// GetMajorVersionString
//
SString SharedUtil::GetMajorVersionString ( void )
{
    return SStringX ( MTA_DM_ASE_VERSION ).Left ( 3 );
}


// Old layout:
//              HKCU Software\\Multi Theft Auto: San Andreas\\             - For 1.0
//              HKCU Software\\Multi Theft Auto: San Andreas 1.1\\         - For 1.1
//
static SString MakeVersionRegistryPathLegacy ( const SString& strVersion, const SString& strPath )
{
    SString strResult = "Software\\Multi Theft Auto: San Andreas";

    if ( strVersion != "1.0" )
        strResult += " " + strVersion;

    strResult += strPath;
    strResult = strResult.TrimEnd ( "\\" );
    return strResult;
}


// Get/set registry values for a version using the old (HKCU) layout
void SharedUtil::SetVersionRegistryValueLegacy ( const SString& strVersion, const SString& strPath, const SString& strName, const SString& strValue )
{
    WriteRegistryStringValue ( HKEY_CURRENT_USER, MakeVersionRegistryPathLegacy ( strVersion, strPath ), strName, strValue );
}

SString SharedUtil::GetVersionRegistryValueLegacy ( const SString& strVersion, const SString& strPath, const SString& strName )
{
    return ReadRegistryStringValue ( HKEY_CURRENT_USER, MakeVersionRegistryPathLegacy ( strVersion, strPath ), strName, NULL );
}



//
// New layout:
//              HKLM Software\\Multi Theft Auto: San Andreas All\\Common    - For all versions
//              HKLM Software\\Multi Theft Auto: San Andreas All\\1.1       - For 1.1
//              HKLM Software\\Multi Theft Auto: San Andreas All\\1.2       - For 1.2
//
static SString MakeVersionRegistryPath ( const SString& strVersion, const SString& strPath )
{
    SString strResult = "Software\\Multi Theft Auto: San Andreas All\\";
    strResult += strVersion;
    strResult += "\\";
    strResult += strPath;
    return strResult.TrimEnd ( "\\" );
}

//
// Registry values
//
// Get/set registry values for the current version
void SharedUtil::SetRegistryValue ( const SString& strPath, const SString& strName, const SString& strValue )
{
    WriteRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( GetMajorVersionString (), strPath ), strName, strValue );
}

SString SharedUtil::GetRegistryValue ( const SString& strPath, const SString& strName )
{
    return ReadRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( GetMajorVersionString (), strPath ), strName, NULL );
}

bool SharedUtil::RemoveRegistryKey ( const SString& strPath )
{
    return DeleteRegistryKey ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( GetMajorVersionString (), strPath ) );
}

// Get/set registry values for a version
void SharedUtil::SetVersionRegistryValue ( const SString& strVersion, const SString& strPath, const SString& strName, const SString& strValue )
{
    WriteRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( strVersion, strPath ), strName, strValue );
}

SString SharedUtil::GetVersionRegistryValue ( const SString& strVersion, const SString& strPath, const SString& strName )
{
    return ReadRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( strVersion, strPath ), strName, NULL );
}

// Get/set registry values for all versions (common)
void SharedUtil::SetCommonRegistryValue ( const SString& strPath, const SString& strName, const SString& strValue )
{
    WriteRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( "Common", strPath ), strName, strValue );
}

SString SharedUtil::GetCommonRegistryValue ( const SString& strPath, const SString& strName )
{
    return ReadRegistryStringValue ( HKEY_LOCAL_MACHINE, MakeVersionRegistryPath ( "Common", strPath ), strName, NULL );
}

//
// Run ShellExecute with these parameters after exit
//
void SharedUtil::SetOnQuitCommand ( const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory, const SString& strShowCmd )
{
    // Encode into a string and set a registry key
    SString strValue ( "%s\t%s\t%s\t%s\t%s", strOperation.c_str (), strFile.c_str (), strParameters.c_str (), strDirectory.c_str (), strShowCmd.c_str () );
    SetRegistryValue ( "", "OnQuitCommand", strValue );
}

#ifdef MTASA_VERSION_MAJOR
//
// What to do on next restart
//
void SharedUtil::SetOnRestartCommand ( const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory, const SString& strShowCmd )
{
    // Encode into a string and set a registry key
    SString strVersion ( "%d.%d.%d-%d.%05d" ,MTASA_VERSION_MAJOR ,MTASA_VERSION_MINOR ,MTASA_VERSION_MAINTENANCE ,MTASA_VERSION_TYPE ,MTASA_VERSION_BUILD );
    SString strValue ( "%s\t%s\t%s\t%s\t%s\t%s", strOperation.c_str (), strFile.c_str (), strParameters.c_str (), strDirectory.c_str (), strShowCmd.c_str (), strVersion.c_str () );
    SetRegistryValue ( "", "OnRestartCommand", strValue );
}

//
// What to do on next restart
//
bool SharedUtil::GetOnRestartCommand ( SString& strOperation, SString& strFile, SString& strParameters, SString& strDirectory, SString& strShowCmd )
{
    SString strOnRestartCommand = GetRegistryValue ( "", "OnRestartCommand" );
    SetOnRestartCommand ( "" );

    std::vector < SString > vecParts;
    strOnRestartCommand.Split ( "\t", vecParts );
    if ( vecParts.size () > 5 && vecParts[0].length () )
    {
        SString strVersion ( "%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD );
        if ( vecParts[5] == strVersion )
        {
            strOperation = vecParts[0];
            strFile = vecParts[1];
            strParameters = vecParts[2];
            strDirectory = vecParts[3];
            strShowCmd = vecParts[4];
            return true;
        }
        //AddReportLog( 4000, SString ( "OnRestartCommand disregarded due to version change %s -> %s", vecParts[5].c_str (), strVersion.c_str () ) );
    }
    return false;
}
#endif


//
// Application settings
//

//
// Get/set string
//
void SharedUtil::SetApplicationSetting ( const SString& strPath, const SString& strName, const SString& strValue )
{
    SetRegistryValue ( "Settings\\" + strPath, strName, strValue );
}

SString SharedUtil::GetApplicationSetting ( const SString& strPath, const SString& strName )
{
    return GetRegistryValue ( "Settings\\" + strPath, strName );
}

// Delete a setting key
bool SharedUtil::RemoveApplicationSettingKey ( const SString& strPath )
{
    return RemoveRegistryKey ( "Settings\\" + strPath );
}


//
// Get/set int
//
void SharedUtil::SetApplicationSettingInt ( const SString& strPath, const SString& strName, int iValue )
{
    SetApplicationSetting ( strPath, strName, SString ( "%d", iValue ) );
}

int SharedUtil::GetApplicationSettingInt ( const SString& strPath, const SString& strName )
{
    return atoi ( GetApplicationSetting ( strPath, strName ) );
}



//
// Get/set string - Which uses 'general' key
//
void SharedUtil::SetApplicationSetting ( const SString& strName, const SString& strValue )
{
    SetApplicationSetting ( "general", strName, strValue );
}

SString SharedUtil::GetApplicationSetting ( const SString& strName )
{
    return GetApplicationSetting ( "general", strName );
}

void SharedUtil::SetApplicationSettingInt ( const SString& strName, int iValue )
{
    SetApplicationSettingInt ( "general", strName, iValue );
}

int SharedUtil::GetApplicationSettingInt ( const SString& strName )
{
    return GetApplicationSettingInt ( "general", strName );
}



//
// WatchDog
//

void SharedUtil::WatchDogReset ( void )
{
    RemoveApplicationSettingKey ( "watchdog" );
}

// Section
bool SharedUtil::WatchDogIsSectionOpen ( const SString& str )
{
    return GetApplicationSettingInt ( "watchdog", str ) != 0;
}

void SharedUtil::WatchDogBeginSection ( const SString& str )
{
    SetApplicationSettingInt ( "watchdog", str, 1 );
}

void SharedUtil::WatchDogCompletedSection ( const SString& str )
{
    SetApplicationSettingInt ( "watchdog", str, 0 );
}

// Counter
void SharedUtil::WatchDogIncCounter ( const SString& str )
{
    SetApplicationSettingInt ( "watchdog", str, WatchDogGetCounter ( str ) + 1 );
}

int SharedUtil::WatchDogGetCounter ( const SString& str )
{
    return GetApplicationSettingInt ( "watchdog", str );
}

void SharedUtil::WatchDogClearCounter ( const SString& str )
{
    SetApplicationSettingInt ( "watchdog", str, 0 );
}

static bool bWatchDogWasUncleanStopCached = false;
static bool bWatchDogWasUncleanStopValue = false;

bool SharedUtil::WatchDogWasUncleanStop ( void )
{
    if ( !bWatchDogWasUncleanStopCached )
    {
        bWatchDogWasUncleanStopCached = true;
        bWatchDogWasUncleanStopValue = GetApplicationSettingInt ( "watchdog", "uncleanstop" ) != 0;
    }
    return bWatchDogWasUncleanStopValue;
}

void SharedUtil::WatchDogSetUncleanStop ( bool bOn )
{
    SetApplicationSettingInt ( "watchdog", "uncleanstop", bOn );
    bWatchDogWasUncleanStopCached = true;
    bWatchDogWasUncleanStopValue = bOn;
}


//
// Direct players to info about trouble
//
void SharedUtil::BrowseToSolution ( const SString& strType, bool bAskQuestion, bool bTerminateProcess, bool bDoOnExit, const SString& strMessageBoxMessage )
{
    AddReportLog ( 3200, SString ( "Trouble %s", *strType ) );

    // Put args into a string and save in the registry
    CArgMap argMap;
    argMap.Set ( "type", strType.SplitLeft ( ";" ) );
    argMap.Set ( "bAskQuestion", bAskQuestion );
    argMap.Set ( "message", strMessageBoxMessage );
    SetApplicationSetting ( "pending-browse-to-solution", argMap.ToString () );

    // Do it now if required
    if ( !bDoOnExit )
        ProcessPendingBrowseToSolution ();

    // Exit if required
    if ( bTerminateProcess )
        TerminateProcess ( GetCurrentProcess (), 1 );
}

//
// Process next BrowseToSolution
//
void SharedUtil::ProcessPendingBrowseToSolution ( void )
{
    SString strType, strMessageBoxMessage;
    int bAskQuestion;

    // Get args from the registry
    CArgMap argMap;
    argMap.SetFromString ( GetApplicationSetting ( "pending-browse-to-solution" ) );
    argMap.Get ( "type", strType );
    argMap.Get ( "message", strMessageBoxMessage );
    argMap.Get ( "bAskQuestion", bAskQuestion );

    // Check if anything to do
    if ( strType.empty () )
        return;

    ClearPendingBrowseToSolution ();

    // Show message box if required
    if ( !strMessageBoxMessage.empty () )
        MessageBox ( 0, strMessageBoxMessage, "Error", MB_OK|MB_ICONEXCLAMATION );

    // Ask question if required, and then launch URL
    if ( !bAskQuestion || IDYES == MessageBox( 0, "Do you want to see some on-line help about this problem ?", "MTA: San Andreas", MB_ICONQUESTION|MB_YESNO ) )
    {
        SString strQueryURL = GetApplicationSetting ( "trouble-url" );
        if ( strQueryURL == "" )
            strQueryURL = TROUBLE_URL1;
        strQueryURL = strQueryURL.Replace ( "%VERSION%", GetApplicationSetting ( "mta-version-ext" ) );
        strQueryURL = strQueryURL.Replace ( "%ID%", GetApplicationSetting ( "serial" ) );
        strQueryURL = strQueryURL.Replace ( "%TROUBLE%", strType );
        ShellExecuteNonBlocking ( "open", strQueryURL.c_str () );
    }
}

//
// Clear BrowseToSolution
//
void SharedUtil::ClearPendingBrowseToSolution ( void )
{
    SetApplicationSetting ( "pending-browse-to-solution", "" );
}


//
// For tracking results of new features
//
static SString GetReportLogHeaderText ( void )
{
    SString strMTABuild      = GetApplicationSetting ( "mta-version-ext" ).SplitRight ( "-" );
    SString strOSVersion     = GetApplicationSetting ( "os-version" );
    SString strRealOSVersion = GetApplicationSetting ( "real-os-version" );
    SString strIsAdmin       = GetApplicationSetting ( "is-admin" );

    SString strResult = "[";
    if ( strMTABuild.length () )
        strResult += strMTABuild + " ";
    if ( strOSVersion.length () )
    {
        if ( strOSVersion == strRealOSVersion )
            strResult += strOSVersion + " ";
        else
            strResult += strOSVersion + "(" + strRealOSVersion + ") ";
    }
    if ( strIsAdmin == "1" )
        strResult += "a";

    return strResult.TrimEnd ( " " ) + "]";
}

void SharedUtil::AddReportLog ( uint uiId, const SString& strText )
{
    return;

    std::fstream log;

    log.open( GetMTADataPath().convert_ansi() + "report.log", std::fstream::out | std::fstream::app );

    if ( !log.is_open() )
        return;

    log << uiId << ": " << GetTimeString( true, false ) << " " << GetReportLogHeaderText() << " - " << strText << std::endl;

#if MTA_DEBUG
    OutputDebugString( SStringX( "ReportLog: " ) + strText );
#endif
}

void SharedUtil::SetReportLogContents( const SString& strText )
{
    return;

    std::ofstream log;

    log.open( GetMTADataPath().convert_ansi() + "report.log", std::ofstream::app | std::ofstream::out );

    if ( !log.is_open() )
        return;

    if ( !strText.empty() )
        log << strText;
}

SString SharedUtil::GetReportLogContents()
{
    return "";

    std::ifstream log;

    log.open( GetMTADataPath().convert_ansi() + "report.log" );

    if ( !log.is_open() )
        return SString();

    return std::string( std::istreambuf_iterator <char>( log ), std::istreambuf_iterator <char>() );
}


///////////////////////////////////////////////////////////////////////////
//
// SharedUtil::GetSystemErrorMessage
//
// Get Windows error message text from a last error code.
//
///////////////////////////////////////////////////////////////////////////
SString SharedUtil::GetSystemErrorMessage ( uint uiError, bool bRemoveNewlines, bool bPrependCode )
{
    SString strResult;

    LPSTR szErrorText = NULL;
    FormatMessageA ( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, uiError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&szErrorText, 0, NULL );

    if ( szErrorText )
    {
        strResult = szErrorText;
        LocalFree ( szErrorText );
        szErrorText = NULL;
    }

    if ( bRemoveNewlines )
        strResult = strResult.Replace ( "\n", "" ).Replace ( "\r", "" );

    if ( bPrependCode )
        strResult = SString ( "Error %u: %s", uiError, *strResult );

    return strResult;
}


//
// Return true if currently executing the main thread.
// Main thread being defined as the thread the function is first called from.
bool SharedUtil::IsMainThread ( void )
{
    static DWORD dwMainThread = GetCurrentThreadId ();
    return GetCurrentThreadId () == dwMainThread;
}

#endif


#ifdef ExpandEnvironmentStringsForUser
//
// eg "%HOMEDRIVE%" -> "C:"
//
SString SharedUtil::ExpandEnvString ( const SString& strInput )
{
    HANDLE hProcessToken;
    if ( !OpenProcessToken ( GetCurrentProcess (), TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE, &hProcessToken ) )
        return strInput;

    const static int iBufferSize = 32000;
    char envBuf [ iBufferSize + 2 ];
    ExpandEnvironmentStringsForUser ( hProcessToken, strInput, envBuf, iBufferSize );
    return envBuf;
}
#endif



///////////////////////////////////////////////////////////////
//
// MyShellExecute
//
// Returns true if successful
//
///////////////////////////////////////////////////////////////
static bool MyShellExecute ( bool bBlocking, const SString& strAction, const SString& strInFile, const SString& strInParameters = "", const SString& strDirectory = "", int nShowCmd = SW_SHOWNORMAL )
{
    SString strFile = strInFile;
    SString strParameters = strInParameters;

    if ( strAction == "open" && strFile.BeginsWithI ( "http://" ) && strParameters.empty () )
    {
        strParameters = "url.dll,FileProtocolHandler " + strFile;
        strFile = "rundll32.exe";
    }

    if ( bBlocking )
    {
        SHELLEXECUTEINFO info;
        memset( &info, 0, sizeof ( info ) );
        info.cbSize = sizeof ( info );
        info.fMask = SEE_MASK_NOCLOSEPROCESS;
        info.lpVerb = strAction;
        info.lpFile = strFile;
        info.lpParameters = strParameters;
        info.lpDirectory = strDirectory;
        info.nShow = nShowCmd;
        bool bResult = ShellExecuteExA( &info ) != FALSE;
        if ( info.hProcess )
        {
            WaitForSingleObject ( info.hProcess, INFINITE );
            CloseHandle ( info.hProcess );
        }
        return bResult;
    }
    else
    {
        int iResult = (int)ShellExecute ( NULL, strAction, strFile, strParameters, strDirectory, nShowCmd );
        return iResult > 32;
    }
}


///////////////////////////////////////////////////////////////
//
// SharedUtil::ShellExecuteBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::ShellExecuteBlocking ( const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory, int nShowCmd )
{
    return MyShellExecute ( true, strAction, strFile, strParameters, strDirectory );
}


///////////////////////////////////////////////////////////////
//
// SharedUtil::ShellExecuteNonBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::ShellExecuteNonBlocking ( const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory, int nShowCmd )
{
    return MyShellExecute ( false, strAction, strFile, strParameters, strDirectory );
}


#endif  // MTA_CLIENT


static char ToHexChar ( char c )
{
    return c > 9 ? c - 10 + 'A' : c + '0';
}

static char FromHexChar ( char c )
{
    return c > '9' ? c - 'A' + 10 : c - '0';
}

SString SharedUtil::EscapeString ( const SString& strText, const SString& strDisallowedChars, char cSpecialChar )
{
    // Replace each disallowed char with #FF
    SString strResult;
    for ( uint i = 0 ; i < strText.length () ; i++ )
    {
        char c = strText[i];
        if ( strDisallowedChars.find ( c ) == std::string::npos && c != cSpecialChar )
            strResult += c;
        else
        {
            strResult += cSpecialChar;
            strResult += ToHexChar ( c >> 4 );
            strResult += ToHexChar ( c & 0x0f );
        }
    }
    return strResult;
}


SString SharedUtil::UnescapeString ( const SString& strText, char cSpecialChar )
{
    SString strResult;
    // Replace #FF with char
    for ( uint i = 0 ; i < strText.length () ; i++ )
    {
        char c = strText[i];
        if ( c == cSpecialChar && i < strText.length () - 2 )
        {
            char c0 = FromHexChar ( strText[++i] );
            char c1 = FromHexChar ( strText[++i] );
            c = ( c0 << 4 ) | c1;
        }
        strResult += c;
    }
    return strResult;
}


//
// Cross platform critical section
//
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

SharedUtil::CCriticalSection::CCriticalSection ( void )
{
    m_pCriticalSection = new CRITICAL_SECTION;
    InitializeCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
}

SharedUtil::CCriticalSection::~CCriticalSection ( void )
{
    DeleteCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
    delete ( CRITICAL_SECTION* ) m_pCriticalSection;
}

void SharedUtil::CCriticalSection::Lock ( void )
{
    if ( m_pCriticalSection )
        EnterCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
}

void SharedUtil::CCriticalSection::Unlock ( void )
{
    if ( m_pCriticalSection )
        LeaveCriticalSection ( ( CRITICAL_SECTION* ) m_pCriticalSection );
}

#else

#include <pthread.h>

SharedUtil::CCriticalSection::CCriticalSection ( void )
{
    m_pCriticalSection = new pthread_mutex_t;
    pthread_mutex_init ( ( pthread_mutex_t* ) m_pCriticalSection, NULL );
}

SharedUtil::CCriticalSection::~CCriticalSection ( void )
{
    pthread_mutex_destroy ( ( pthread_mutex_t* ) m_pCriticalSection );
    delete ( pthread_mutex_t* ) m_pCriticalSection;
}

void SharedUtil::CCriticalSection::Lock ( void )
{
    pthread_mutex_lock ( ( pthread_mutex_t* ) m_pCriticalSection );
}

void SharedUtil::CCriticalSection::Unlock ( void )
{
    pthread_mutex_unlock ( ( pthread_mutex_t* ) m_pCriticalSection );
}

#endif



//
// Expiry stuff
//
#ifdef WIN32

#include <time.h>

#define YEAR ((((__DATE__ [7]-'0')*10+(__DATE__ [8]-'0'))*10+(__DATE__ [9]-'0'))*10+(__DATE__ [10]-'0'))

/* Month: 0 - 11 */
#define MONTH (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 0 : 5) \
              : __DATE__ [2] == 'b' ? 1 \
              : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M'? 2 : 3) \
              : __DATE__ [2] == 'y' ? 4 \
              : __DATE__ [2] == 'l' ? 6 \
              : __DATE__ [2] == 'g' ? 7 \
              : __DATE__ [2] == 'p' ? 8 \
              : __DATE__ [2] == 't' ? 9 \
              : __DATE__ [2] == 'v' ? 10 : 11)

#define DAY ((__DATE__ [4]==' ' ? 0 : __DATE__[4]-'0')*10+(__DATE__[5]-'0'))

int SharedUtil::GetBuildAge ( void )
{
    tm when;
    memset ( &when, 0, sizeof ( when ) );
    when.tm_year = YEAR - 1900;
    when.tm_mon = MONTH;
    when.tm_mday = DAY;
    return ( int )( time ( NULL ) - mktime( &when ) ) / ( 60 * 60 * 24 );
}

#if defined(MTA_DM_EXPIRE_DAYS)

int SharedUtil::GetDaysUntilExpire ( void )
{
    tm when;
    memset ( &when, 0, sizeof ( when ) );
    when.tm_year = YEAR - 1900;
    when.tm_mon = MONTH;
    when.tm_mday = DAY + MTA_DM_EXPIRE_DAYS;
    return ( int )( mktime( &when ) - time ( NULL ) ) / ( 60 * 60 * 24 );
}

#endif
#endif

// Copied from CChatLine::RemoveColorCode()
std::string SharedUtil::RemoveColorCode ( const char* szString )
{
    std::string strOut;
    const char* szStart = szString;
    const char* szEnd = szString;

    while ( true )
    {
        if ( *szEnd == '\0' )
        {
            strOut.append ( szStart, szEnd - szStart );
            break;
        }
        else
        {
            bool bIsColorCode = false;
            if ( *szEnd == '#' )
            {
                bIsColorCode = true;
                for ( int i = 0; i < 6; i++ )
                {
                    char c = szEnd [ 1 + i ];
                    if ( !isdigit ( (unsigned char)c ) && (c < 'A' || c > 'F') && (c < 'a' || c > 'f') )
                    {
                        bIsColorCode = false;
                        break;
                    }
                }
            }

            if ( bIsColorCode )
            {
                strOut.append ( szStart, szEnd - szStart );
                szStart = szEnd + 7;
                szEnd = szStart;
            }
            else
            {
                szEnd++;
            }
        }
    }

    return strOut;
}

// Convert a standard multibyte UTF-8 std::string into a UTF-16 std::wstring
std::wstring SharedUtil::MbUTF8ToUTF16 (const std::string& input)
{
    return utf8_mbstowcs (input);
}

// Convert a UTF-16 std::wstring into a multibyte UTF-8 string
std::string SharedUtil::UTF16ToMbUTF8 (const std::wstring& input)
{
    return utf8_wcstombs (input);
}

// Get UTF8 confidence
int SharedUtil::GetUTF8Confidence (unsigned char* input, int len)
{
    return icu_getUTF8Confidence (input, len);
}

// Translate a true ANSI string to the UTF-16 equivalent (reencode+convert)
std::wstring SharedUtil::ANSIToUTF16 ( const std::string& input )
{
    size_t len = mbstowcs ( nullptr, input.c_str(), input.length() );
    if ( len == (size_t)-1 )
        return L"?";
    wchar_t* wcsOutput = new wchar_t[len+1];
    mbstowcs ( wcsOutput, input.c_str(), input.length() );
    wcsOutput[len] = 0; //zero terminate the string
    std::wstring strOutput(wcsOutput);
    delete wcsOutput;
    return strOutput;
}

#ifdef MTA_DEBUG
//
// Output timestamped line into the debugger
//
void SharedUtil::OutputDebugLine ( const char* szMessage )
{
    SString strMessage = GetLocalTimeString ( false, true ) + " - " + szMessage;
    if ( strMessage.length () > 0 && strMessage[ strMessage.length () - 1 ] != '\n' )
        strMessage += "\n";
#ifdef _WIN32
    OutputDebugString ( strMessage );
#else
    // Other platforms here
#endif
}
#endif


//
// Return true if supplied version string will sort correctly
//
bool SharedUtil::IsValidVersionString ( const SString& strVersion )
{
    const SString strCheck = "0.0.0-0.00000.0.000";
    size_t uiLength = Min ( strCheck.length (), strVersion.length () );
    for ( size_t i = 0 ; i < uiLength ; i++ )
    {
        char c = strVersion[i];
        char d = strCheck[i];
        if ( !isdigit( c ) || !isdigit( d ) )
            if ( c != d )
                return false;
    }
    return true;
}

SString SharedUtil::GenerateNickname()
{
    return CNickGen::GetRandomNickname();
}

namespace SharedUtil
{
    CArgMap::CArgMap ( const SString& strArgSep, const SString& strPartsSep, const SString& strExtraDisallowedChars )
        : m_strArgSep ( strArgSep )
        , m_strPartsSep ( strPartsSep )
    {
        m_strDisallowedChars = strExtraDisallowedChars + m_strArgSep + m_strPartsSep;
        m_cEscapeCharacter = '#';
    }

    void CArgMap::SetEscapeCharacter ( char cEscapeCharacter )
    {
        m_cEscapeCharacter = cEscapeCharacter;
    }

    void CArgMap::Merge ( const CArgMap& other, bool bAllowMultiValues )
    {
        MergeFromString ( other.ToString (), bAllowMultiValues );
    }

    void CArgMap::SetFromString ( const SString& strLine, bool bAllowMultiValues )
    {
        m_Map.clear ();
        MergeFromString ( strLine, bAllowMultiValues );
    }

    void CArgMap::MergeFromString ( const SString& strLine, bool bAllowMultiValues )
    {
        std::vector < SString > parts;
        strLine.Split( m_strPartsSep, parts );
        for ( uint i = 0 ; i < parts.size () ; i++ )
        {
            SString strCmd, strArg;
            parts[i].Split ( m_strArgSep, &strCmd, &strArg );
            if ( !bAllowMultiValues )
                m_Map.erase ( strCmd );
            if ( strCmd.length () ) // Key can not be empty
                MapInsert ( m_Map, strCmd, strArg );
        }
    }

    SString CArgMap::ToString ( void ) const
    {
        SString strResult;
        for ( std::multimap < SString, SString >::const_iterator iter = m_Map.begin () ; iter != m_Map.end () ; ++iter )
        {
            if ( strResult.length () )
                strResult += m_strPartsSep;
            strResult += iter->first + m_strArgSep + iter->second;
        }
        return strResult;
    }

    bool CArgMap::HasMultiValues ( void ) const
    {
        for ( std::multimap < SString, SString >::const_iterator iter = m_Map.begin () ; iter != m_Map.end () ; ++iter )
        {
            std::vector < SString > newItems;
            MultiFind ( m_Map, iter->first, &newItems );
            if ( newItems.size () > 1 )
                return true;
        }
        return false;
    }

    void CArgMap::RemoveMultiValues ( void )
    {
        if ( HasMultiValues () )
            SetFromString ( ToString (), false );
    }

    SString CArgMap::Escape ( const SString& strIn ) const
    {
        return EscapeString ( strIn, m_strDisallowedChars, m_cEscapeCharacter );
    }

    SString CArgMap::Unescape ( const SString& strIn ) const
    {
        return UnescapeString ( strIn, m_cEscapeCharacter );
    }


    // Set a unique key string value
    void CArgMap::Set ( const SString& strCmd, const SString& strValue )
    {
        m_Map.erase ( Escape ( strCmd ) );
        Insert ( strCmd, strValue );
    }

    // Set a unique key int value
    void CArgMap::Set ( const SString& strCmd, int iValue )
    {
        m_Map.erase ( Escape ( strCmd ) );
        Insert ( strCmd, iValue );
    }

    // Insert a key int value
    void CArgMap::Insert ( const SString& strCmd, int iValue )
    {
        Insert ( strCmd, SString ( "%d", iValue ) );
    }

    // Insert a key string value
    void CArgMap::Insert ( const SString& strCmd, const SString& strValue )
    {
        if ( strCmd.length () ) // Key can not be empty
            MapInsert ( m_Map, Escape ( strCmd ), Escape ( strValue ) );
    }


    // Test if key exists
    bool CArgMap::Contains ( const SString& strCmd  ) const
    {
        return MapFind ( m_Map, Escape ( strCmd ) ) != NULL;
    }

    // First result as string
    bool CArgMap::Get ( const SString& strCmd, SString& strOut, const char* szDefault ) const
    {
        assert ( szDefault );
        if ( const SString* pResult = MapFind ( m_Map, Escape ( strCmd ) ) )
        {
            strOut = Unescape ( *pResult );
            return true;
        }
        strOut = szDefault;
        return false;
    }

    // First result as string
    SString CArgMap::Get ( const SString& strCmd ) const
    {
        SString strResult;
        Get ( strCmd, strResult );
        return strResult;
    }

    // All results as strings
    bool CArgMap::Get ( const SString& strCmd, std::vector < SString >& outList ) const
    {
        std::vector < SString > newItems;
        MultiFind ( m_Map, Escape ( strCmd ), &newItems );
        for ( uint i = 0 ; i < newItems.size () ; i++ )
            newItems[i] = Unescape ( newItems[i] );
        ListAppend ( outList, newItems );
        return newItems.size () > 0;
    }

    // First result as int
    bool CArgMap::Get ( const SString& strCmd, int& iValue, int iDefault ) const
    {
        SString strResult;
        if ( Get ( strCmd, strResult ) )
        {
            iValue = atoi ( strResult );
            return true;
        }
        iValue = iDefault;
        return false;
    }

    // All keys
    void CArgMap::GetKeys ( std::vector < SString >& outList  ) const
    {
        for ( std::multimap < SString, SString > ::const_iterator iter = m_Map.begin () ; iter != m_Map.end () ; ++iter )
            outList.push_back ( iter->first );
    }

}

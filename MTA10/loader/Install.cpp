/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        loader/Install.cpp
*  PURPOSE:     MTA loader
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


bool TerminateProcessFromPathFilename ( const SString& strPathFilename )
{
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        DWORD id1 = GetCurrentProcessId();
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
            DWORD id2 = dwProcessIDs[i];
            if ( id2 == id1 )
                continue;
            // Skip 64 bit processes to avoid errors
            if ( !Is32bitProcess ( dwProcessIDs[i] ) )
                continue;
            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessIDs[i]);
            if ( hProcess )
            {
                HMODULE pModule;
                DWORD cbNeeded;
                if ( EnumProcessModules ( hProcess, &pModule, sizeof ( HMODULE ), &cbNeeded ) )
                {
                    char szModuleName[500];
                    if ( GetModuleFileNameEx( hProcess, pModule, szModuleName, 500 ) )
                    {
                        if ( stricmp ( szModuleName, strPathFilename ) == 0 )
                        {
                            TerminateProcess ( hProcess, 0 );
                            CloseHandle ( hProcess );
                            return true;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }

    return false;
}


struct SFileItem
{
    filePath strSrcPathFilename;
    filePath strDestPathFilename;
    filePath strBackupPathFilename;
};

///////////////////////////////////////////////////////////////
//
// DoInstallFiles
//
// Copy directory tree at current dirctory to GetMTASAPath ()
//
///////////////////////////////////////////////////////////////
#include <memory>

bool DoInstallFiles ( void )
{
    filePath strCurrentDir = SharedUtil::GetCurrentDirectory();

    std::unique_ptr <CFileTranslator> transCurDir( fileSystem->CreateTranslator( strCurrentDir ) );

    if ( !transCurDir )
        return false;

    filePath strMTASAPath = GetMTASAPath();

    filePath strDestRoot = strMTASAPath;
    filePath strSrcRoot = strCurrentDir;
    filePath strBakRoot = MakeUniquePath( strCurrentDir + "_bak_/" );

    // Clean backup dir
    if ( !transCurDir->CreateDir( strBakRoot ) )
    {
        AddReportLog ( 5020, SString ( "InstallFiles: Couldn't make dir '%s'", strBakRoot.c_str () ) );
        return false;
    }

    std::unique_ptr <CFileTranslator> transDestRoot( fileSystem->CreateTranslator( strDestRoot ) );

    if ( !transDestRoot )
        return false;

    std::unique_ptr <CFileTranslator> transSrcRoot( fileSystem->CreateTranslator( strSrcRoot ) );

    if ( !transSrcRoot )
        return false;

    std::unique_ptr <CFileTranslator> transBakRoot( fileSystem->CreateTranslator( strBakRoot ) );

    if ( !transBakRoot )
        return false;

    // Get list of files to install
    std::vector <SFileItem> itemList;
    {
        transCurDir->ScanDirectory( "/", "*", true,
            NULL,
            [&]( const filePath& absPath )
        {
            // Get the relative portion of this file path.
            filePath relPath;
            transCurDir->GetRelativePathFromRoot( absPath, true, relPath );

            SFileItem item;
            item.strSrcPathFilename = absPath;
            item.strDestPathFilename = strDestRoot + relPath;
            item.strBackupPathFilename = strBakRoot + relPath;
            itemList.push_back( item );
        }, NULL );
    }

    // See if any files to be updated are running.
    // If so, terminate them
    for ( unsigned int i = 0 ; i < itemList.size () ; i++ )
    {
        SString strFile = itemList[i].strDestPathFilename;
        if ( strFile.ToLower ().substr ( Max < int > ( 0, strFile.length () - 4 ) ) == ".exe" )
            TerminateProcessFromPathFilename ( strFile );
    }

    // Copy current(old) files into backup location
    for ( unsigned int i = 0 ; i < itemList.size() ; i++ )
    {
        const SFileItem& fileItem = itemList[i];

        bool success = FileSystem::FileCopy( transDestRoot.get(), fileItem.strDestPathFilename, transBakRoot.get(), fileItem.strBackupPathFilename );

        if ( !success )
        {
            std::string ansiDestPathFilename = fileItem.strDestPathFilename.convert_ansi();

            if ( transDestRoot->Exists( fileItem.strDestPathFilename ) )
            {
                std::string ansiBackupPathFilename = fileItem.strBackupPathFilename.convert_ansi();

                AddReportLog( 5021, SString( "InstallFiles: Couldn't backup '%s' to '%s'", ansiDestPathFilename.c_str(), ansiBackupPathFilename.c_str() ) );
                return false;
            }
            AddReportLog( 4023, SString( "InstallFiles: Couldn't backup '%s' as it does not exist", ansiDestPathFilename.c_str() ) );
        }
    }

    // Try copy new files
    bool bOk = true;
    std::vector < SFileItem > fileListSuccess;
    for ( unsigned int i = 0; i < itemList.size(); i++ )
    {
        const SFileItem& fileItem = itemList[i]; 

        bool success = FileSystem::FileCopy( transSrcRoot.get(), fileItem.strSrcPathFilename, transDestRoot.get(), fileItem.strDestPathFilename );

        if ( !success )
        {
            std::string ansiSrcPathFilename = fileItem.strSrcPathFilename.convert_ansi();
            std::string ansiDestPathFilename = fileItem.strDestPathFilename.convert_ansi();

            AddReportLog( 5022, SString( "InstallFiles: Couldn't copy '%s' to '%s'", ansiSrcPathFilename.c_str(), ansiDestPathFilename.c_str() ) );
            bOk = false;
            break;
        }
        fileListSuccess.push_back( fileItem );
    }

    // If fail, copy back old files
    if ( !bOk )
    {
        bool bPossibleDisaster = false;

        for ( unsigned int i = 0; i < fileListSuccess.size(); i++ )
        {
            const SFileItem& fileItem = fileListSuccess[i];

            int iRetryCount = 3;

            while ( true )
            {
                bool success = FileSystem::FileCopy( transBakRoot.get(), fileItem.strBackupPathFilename, transDestRoot.get(), fileItem.strDestPathFilename );

                if ( success )
                    break;

                if ( !--iRetryCount )
                {
                    std::string ansiBackupPathFilename = fileItem.strBackupPathFilename.convert_ansi();
                    std::string ansiDestPathFilename = fileItem.strDestPathFilename.convert_ansi();

                    AddReportLog( 5023, SString( "InstallFiles: Possible disaster restoring '%s' to '%s'", ansiBackupPathFilename.c_str(), ansiDestPathFilename.c_str() ) );
                    bPossibleDisaster = true;
                    break;
                }
            }
        }

        //if ( bPossibleDisaster )
        //    MessageBox ( NULL, "Installation may be corrupt. Please redownload from www.mtasa.com", "Error", MB_OK | MB_ICONERROR );
        //else 
        //    MessageBox ( NULL, "Could not update due to file conflicts.", "Error", MB_OK | MB_ICONERROR );
    }

    // Launch MTA_EXE_NAME
    return bOk;
}


///////////////////////////////////////////////////////////////
//
// InstallFiles
//
// Handle progress bar if required
//
///////////////////////////////////////////////////////////////
bool InstallFiles ( bool bSilent )
{
    // Start progress bar
    if ( !bSilent )
       StartPseudoProgress( g_hInstance, "MTA: San Andreas", "Installing update..." );

    bool bResult = DoInstallFiles ();

    // Stop progress bar
    StopPseudoProgress();
    return bResult;
}


//////////////////////////////////////////////////////////
//
// CheckOnRestartCommand
//
// Changes current directory if required
//
//////////////////////////////////////////////////////////
SString CheckOnRestartCommand ( void )
{
    const SString strMTASAPath = GetMTASAPath ();

    SetCurrentDirectory ( strMTASAPath );
    SetDllDirectory( strMTASAPath );

    SString strOperation, strFile, strParameters, strDirectory, strShowCmd;
    if ( GetOnRestartCommand ( strOperation, strFile, strParameters, strDirectory, strShowCmd ) )
    {
        if ( strOperation == "files" || strOperation == "silent" )
        {
            //
            // Update
            //

            // Make temp path name and go there
            filePath strArchivePath, strArchiveName;
            strArchiveName = FileSystem::GetFileNameItem( strFile.c_str(), false, &strArchivePath, NULL );

            filePath strTempPath = MakeUniquePath( strArchivePath + "_" + strArchiveName + "_tmp_/" );

            std::unique_ptr <CFileTranslator> archivePathRoot( fileSystem->CreateTranslator( strArchivePath ) );

            if ( !archivePathRoot )
                return "FileErrorMeow";

            if ( !archivePathRoot->CreateDir( strTempPath ) )
                return "FileError1";

            std::wstring wideTempPath = strTempPath.convert_unicode();

            if ( !SetCurrentDirectoryW( wideTempPath.c_str() ) )
                return "FileError2";

            // Start progress bar
            if ( strOperation != "silent" )
               StartPseudoProgress( g_hInstance, "MTA: San Andreas", "Extracting files..." );

            // Run self extracting archive to extract files into the temp directory
            ShellExecuteBlocking ( "open", strFile, "-s" );

            // Stop progress bar
            StopPseudoProgress();

            // If a new "Multi Theft Auto.exe" exists, let that complete the install
            if ( archivePathRoot->Exists( MTA_EXE_NAME_RELEASE ) )
                return "install from far " + strOperation;

            // Otherwise use the current exe to install
            return "install from near " + strOperation;
        }
        else
        {
            AddReportLog ( 5052, SString ( "CheckOnRestartCommand: Unknown restart command %s", strOperation.c_str () ) );
        }
    }
    return "no update";
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.File.h
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <CFileSystem.common.filePath.h>

namespace SharedUtil
{
    //
    // Get startup directory as saved in the registry by the launcher
    // Used in the Win32 Client only
    //
    SString GetMTASABaseDir ( void );

    void            ExtractFilename                 ( const SString& strPathFilename, SString* strPath, SString* strFilename );
    bool            ExtractExtention                ( const SString& strFilename, SString* strRest, SString* strExt );
    SString         ExtractPath                     ( const SString& strPathFilename );
    SString         ExtractFilename                 ( const SString& strPathFilename );
    SString         ExtractExtention                ( const SString& strPathFilename );
    SString         ExtractBeforeExtention          ( const SString& strPathFilename );

    void            MakeSureDirExists               ( const SString& strPath );

    filePath        GetCurrentDirectory             ();
#ifdef _WIN32
    filePath        GetWindowsDirectory             ();
#endif
    filePath        MakeUniquePath                  ( const filePath& strPathFilename );

    filePath        GetSystemLocalAppDataPath       ();
    filePath        GetSystemCommonAppDataPath      ();
    filePath        GetSystemTempPath               ();
    filePath        GetMTADataPath                  ();
    filePath        GetMTATempPath                  ();
}

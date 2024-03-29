/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CScreenShot.cpp
*  PURPOSE:     Screen capture file handling
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <libpng/png.h>

extern CCore* g_pCore;

static bool bIsChatVisible = false;
static bool bIsDebugVisible = false;

// Variables used for saving the screen shot file on a separate thread
static bool     ms_bIsSaving        = false;
static RECT     ms_ScreenSize       = { 0, 0, 0, 0 };
static void*    ms_pBits            = NULL;
static DWORD    ms_ulPitch          = 0;
static SString  ms_strFileName;


void CScreenShot::Init()
{
}

void CScreenShot::Shutdown()
{
}

SString CScreenShot::PreScreenShot ()
{
    bIsChatVisible = g_pCore->IsChatVisible ();
    bIsDebugVisible = g_pCore->IsDebugVisible ();

    // make the chat and debug windows invisible
    g_pCore->SetChatVisible ( false );
    g_pCore->SetDebugVisible ( false );

    return GetValidScreenshotFilename();
}

void CScreenShot::PostScreenShot ( const char *szFileName )
{
    // print a notice
    g_pCore->GetConsole()->Printf ( "Screenshot taken: '%s'", szFileName );

    // make the chat and debug windows visible again
    g_pCore->SetChatVisible ( bIsChatVisible );
    g_pCore->SetDebugVisible ( bIsDebugVisible );
}

void CScreenShot::SetPath( const char *path )
{
}

static void _screenshot_IncrementFileCount( const filePath& path, unsigned int& num )
{
    num++;
}

unsigned int CScreenShot::GetScreenShots()
{
    unsigned int numFiles = 0;

    // Use the fileSystem API
    screenFileRoot->ScanDirectory( "/", "mta-screen*.png", true, NULL, (pathCallback_t)_screenshot_IncrementFileCount, &numFiles );
    return numFiles;
}

SString CScreenShot::GetValidScreenshotFilename()
{
    //Get the system time
    SYSTEMTIME sysTime;
    GetLocalTime( &sysTime );

    filePath root;
    screenFileRoot->GetFullPath( SString( "/mta-screen_%d-%02d-%02d_%02d-%02d-%02d.png", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond ).c_str(), true, root );
    return root.convert_ansi();
}

struct _scrshot_findstruct
{
    unsigned int curIdx;
    unsigned int idxFind;
    SString outname;
};

static void _screenshot_GetByIndex( const filePath& path, _scrshot_findstruct *info )
{
    if ( info->curIdx++ == info->idxFind )
        return;

    info->outname = path.convert_ansi();
}

SString CScreenShot::GetScreenShotPath( unsigned int num )
{
    _scrshot_findstruct info;
    info.curIdx = 1;
    info.idxFind = num;

    // Use the fileSystem API
    screenFileRoot->ScanDirectory( "/", "mta-screen*.png", true, NULL, (pathCallback_t)_screenshot_GetByIndex, &info );

    // Return the file directory
    return info.outname;
}

static void Png_Writer( png_struct *png, char *data, size_t size )
{
    ((CFile*)png_get_io_ptr( png ))->Write( data, size, 1 );
}

static void Png_Flusher( png_struct *png )
{
    ((CFile*)png_get_io_ptr( png ))->Flush();
}

// Callback for threaded save
// Static function
DWORD CScreenShot::ThreadProc ( LPVOID lpdwThreadParam )
{
    unsigned long ulScreenHeight = ms_ScreenSize.bottom - ms_ScreenSize.top;
    unsigned long ulScreenWidth = ms_ScreenSize.right - ms_ScreenSize.left;

    // Create the screen data buffer
    BYTE** ppScreenData = NULL;
    ppScreenData = new BYTE* [ ulScreenHeight ];
    for ( unsigned short y = 0; y < ulScreenHeight; y++ )
    {
        ppScreenData[y] = new BYTE [ ulScreenWidth * 4 ];
    }

    // Copy the surface data into a row-based buffer for libpng
    #define BYTESPERPIXEL 4
    unsigned long ulLineWidth = ulScreenWidth * 4;
    for ( unsigned int i = 0; i < ulScreenHeight; i++ )
    {
        memcpy ( ppScreenData[i], (BYTE*) ms_pBits + i* ms_ulPitch, ulLineWidth );
        for ( unsigned int j = 3; j < ulLineWidth; j += BYTESPERPIXEL )
        {
            ppScreenData[i][j] = 0xFF;
        }
    }

    CFile *file = screenFileRoot->Open( ms_strFileName, "wb" );

    // Do the .png logic
    png_struct *png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    png_info* info_ptr = png_create_info_struct( png_ptr );
    png_set_write_fn( png_ptr, file, (png_rw_ptr)Png_Writer, (png_flush_ptr)Png_Flusher );
    png_set_filter( png_ptr, 0, PNG_FILTER_NONE );
    png_set_compression_level( png_ptr, 1 );
    png_set_IHDR( png_ptr, info_ptr, ulScreenWidth, ulScreenHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
    png_set_rows( png_ptr, info_ptr, ppScreenData );
    png_write_png( png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, NULL );
    png_write_end( png_ptr, info_ptr );
    png_destroy_write_struct ( &png_ptr, &info_ptr );

    delete file;

    // Clean up the screen data buffer
    if ( ppScreenData )
    {
        for ( unsigned short y = 0; y < ulScreenHeight; y++ ) {
            delete [] ppScreenData[y];
        }
        delete [] ppScreenData;
    }

    ms_bIsSaving = false;
    return 0;
}

// Static function
void CScreenShot::BeginSave ( const char *szFileName, void* pBits, unsigned long ulPitch, RECT ScreenSize )
{
    if ( ms_bIsSaving )
        return;

    ms_strFileName = szFileName;
    ms_pBits = pBits;
    ms_ulPitch = ulPitch;
    ms_ScreenSize = ScreenSize;

    HANDLE hThread = CreateThread ( NULL, 0, (LPTHREAD_START_ROUTINE)CScreenShot::ThreadProc, NULL, CREATE_SUSPENDED, NULL );
    if ( !hThread )
    {
        CCore::GetSingleton ().GetConsole ()->Printf ( "Could not create screenshot thread." );
    }
    else
    {
        ms_bIsSaving = true;
        SetThreadPriority ( hThread, THREAD_PRIORITY_LOWEST );
        ResumeThread ( hThread );
    }
}

// Static function
bool CScreenShot::IsSaving ( void )
{
    return ms_bIsSaving;
}

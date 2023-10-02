/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CScreenShot.h
*  PURPOSE:     Header file for screen capture handling class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCORESCREENSHOT_H
#define __CCORESCREENSHOT_H

#include "CCore.h"
#include <time.h>

class CScreenShot
{
public:
    static void                     Init();
    static void                     Shutdown();

    static SString                  PreScreenShot();
    static void                     PostScreenShot( const char *path );
    static void                     SetPath( const char *path );

    static SString                  GetScreenShotPath( unsigned int num );
    static SString                  GetValidScreenshotFilename();
    static unsigned int             GetScreenShots();

    static void                     BeginSave( const char *path, void *bits, unsigned long pitch, RECT screenSize );
    static bool                     IsSaving();
    static DWORD                    ThreadProc( LPVOID lpdwThreadParam );
};

#endif

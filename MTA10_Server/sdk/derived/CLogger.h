/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CLogger.h
*  PURPOSE:     Logger
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LOGGER_
#define _LOGGER_

class CLogger : public Logger
{
public:
    static void         LogPrintf           ( const char *fmt, ... );
    static void         LogPrint            ( const char *msg );

    static void         LogPrintfNoStamp    ( const char *fmt, ... );
    static void         LogPrintNoStamp     ( const char *msg );

    static void         ErrorPrintf         ( const char *fmt, ... );
    static void         DebugPrintf         ( const char *msg, ... );


    static void         LogPrintf           ( eLogLevel logLevel, const char *fmt, ... );
    static void         LogPrint            ( eLogLevel logLevel, const char *msg );

    static void         AuthPrintf          ( const char *fmt, ... );
    static bool         SetAuthFile         ( const char *path );

    static void         SetMinLogLevel      ( eLogLevel logLevel );

    static void         ProgressDotsBegin   ();
    static void         ProgressDotsUpdate  ();
    static void         ProgressDotsEnd     ();

    static void         BeginConsoleOutputCapture  ();
    static SString      EndConsoleOutputCapture    ();

private:
    static void         HandleLogPrint      ( bool time, const char *pre, const char *msg, bool console, bool auth, eLogLevel logLevel = LOGLEVEL_MEDIUM );

    static CFile*       m_authLog;

    static eLogLevel    m_MinLogLevel;
    static bool         m_bPrintingDots;
    static SString      m_strCaptureBuffer;
    static bool         m_bCaptureConsole;
    static CCriticalSection m_CaptureBufferMutex;
};

#endif //_LOGGER_
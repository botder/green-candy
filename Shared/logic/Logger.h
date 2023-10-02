/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/Logger.h
*  PURPOSE:     Logger base
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LOGGER_BASE_
#define _LOGGER_BASE_

enum eLogLevel
{
    LOGLEVEL_LOW       = 1,
    LOGLEVEL_MEDIUM    = 2
};

class Logger
{
public:
    static void         LogPrintf           ( const char *fmt, ... );
    static void         LogPrint            ( const char *msg );

    static void         LogPrintfNoStamp    ( const char *fmt, ... );
    static void         LogPrintNoStamp     ( const char *msg );

    static void         ErrorPrintf         ( const char *fmt, ... );
    static void         DebugPrintf         ( const char *msg, ... );

    static bool         SetLogFile          ( const char *path );

protected:
    static void         HandleLogPrint      ( bool time, const char *pre, const char *msg );
    static void         BuildEntry          ( bool time, const char *pre, const char *msg, std::string *shrt, std::string *lng );

    static CFile*       m_file;
};

#endif //_LOGGER_BASE_
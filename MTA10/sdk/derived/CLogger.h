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
    static void         LogPrintfNoStamp    ( const char *fmt, ... );
    static void         LogPrintNoStamp     ( const char *msg );

    static void         ErrorPrintf         ( const char *fmt, ... );
    static void         DebugPrintf         ( const char *msg, ... );

private:
    static void         HandleLogPrint      ( bool time, const char *pre, const char *msg );
};

#endif //_LOGGER_
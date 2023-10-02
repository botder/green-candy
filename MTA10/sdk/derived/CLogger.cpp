/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CLogger.cpp
*  PURPOSE:     Logger
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

#define MAX_STRING_LENGTH 2048

void CLogger::LogPrintfNoStamp( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    LogPrintNoStamp( szBuffer );
}

void CLogger::LogPrintNoStamp( const char* szText )
{
    // Send to the console and logfile
    HandleLogPrint( false, "", szText );
}

void CLogger::ErrorPrintf( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint( true, "ERROR: ", szBuffer );
}

void CLogger::DebugPrintf( const char* szFormat, ... )
{
#ifdef MTA_DEBUG
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    // Timestamp and send to the console and logfile
    HandleLogPrint( true, "DEBUG: ", szBuffer );
#endif
}

void CLogger::HandleLogPrint( bool time, const char *pre, const char *msg )
{
    string outShort;

    Logger::HandleLogPrint( time, pre, msg );
    BuildEntry( time, pre, msg, &outShort, NULL );

    g_pCore->GetConsole()->Printf( outShort.c_str() );
}
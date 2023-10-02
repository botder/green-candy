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

CFile* CLogger::m_authLog = NULL;
eLogLevel CLogger::m_MinLogLevel = LOGLEVEL_LOW;
bool CLogger::m_bPrintingDots = false;
SString CLogger::m_strCaptureBuffer;
bool CLogger::m_bCaptureConsole = false;
CCriticalSection CLogger::m_CaptureBufferMutex;

#define MAX_STRING_LENGTH 2048
void CLogger::LogPrintf( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    LogPrint( szBuffer );
}

void CLogger::LogPrint( const char* szText )
{
    // Timestamp and send to the console and logfile
    HandleLogPrint( true, "", szText, true, false );
}

// As above, but with a log level
void CLogger::LogPrintf( eLogLevel logLevel, const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    LogPrint( logLevel, szBuffer );
}

void CLogger::LogPrint( eLogLevel logLevel, const char* szText )
{
    // Timestamp and send to the console and logfile
    HandleLogPrint( true, "", szText, true, false, logLevel );
}

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
    HandleLogPrint( false, "", szText, true, false );
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
    HandleLogPrint( true, "ERROR: ", szBuffer, true, false );
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
    HandleLogPrint( true, "DEBUG: ", szBuffer, true, false );
#endif
}

void CLogger::AuthPrintf( const char* szFormat, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, szFormat );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, szFormat, marker );
    va_end( marker );

    // Timestamp and send to the console, logfile and authfile
    HandleLogPrint( true, "", szBuffer, true, true );
}

bool CLogger::SetAuthFile( const char *filename )
{
    // Eventually delete our current file
    if ( m_authLog )
    {
        delete m_authLog;
        m_authLog = NULL;
    }

    if ( !filename || !filename[0] )
        return true;

    m_authLog = modFileRoot->Open( filename, "a+" );
    return m_authLog != NULL;
}

void CLogger::SetMinLogLevel( eLogLevel logLevel )
{
    m_MinLogLevel = logLevel;
}

void CLogger::ProgressDotsBegin()
{
    m_bPrintingDots = true;
}

void CLogger::ProgressDotsUpdate()
{
    if ( !m_bPrintingDots )
        return;

    HandleLogPrint( false, "", ".", true, false );
}

void CLogger::ProgressDotsEnd()
{
    if ( !m_bPrintingDots )
        return;

    m_bPrintingDots = false;
    HandleLogPrint( false, "", "\n", true, false );
}

void CLogger::BeginConsoleOutputCapture()
{
    m_CaptureBufferMutex.Lock();

    m_strCaptureBuffer.clear();
    m_bCaptureConsole = true;

    m_CaptureBufferMutex.Unlock();
}

SString CLogger::EndConsoleOutputCapture()
{
    m_CaptureBufferMutex.Lock();

    SString strTemp = m_strCaptureBuffer;
    m_bCaptureConsole = false;
    m_strCaptureBuffer.clear();

    m_CaptureBufferMutex.Unlock();
    return strTemp;
}

// Handle where to send the message
void CLogger::HandleLogPrint( bool time, const char *pre, const char *msg, bool console, bool auth, eLogLevel logLevel )
{
    if ( logLevel < m_MinLogLevel )
        return;

    // Handle interruption of progress dots
    if ( m_bPrintingDots && ( time || strlen( pre ) != 0 || strlen( msg ) > 1 || msg[0] != '.' ) )
        ProgressDotsEnd();

    Logger::HandleLogPrint( time, pre, msg );

    // Put the timestamp at the beginning of the string
    string outShort;
    string outLong;

    BuildEntry( time, pre, msg, &outShort, &outLong );

    if ( console )
        g_pServerInterface->Printf( "%s", outShort.c_str() );

    if ( console && m_bCaptureConsole )
    {
        m_CaptureBufferMutex.Lock();
        m_strCaptureBuffer += pre;
        m_strCaptureBuffer += msg;

        if ( m_strCaptureBuffer.length() > 1000 )
            m_bCaptureConsole = false;

        m_CaptureBufferMutex.Unlock();
    }

    if ( auth && m_authLog )
    {
        m_authLog->Printf( "%s", outLong.c_str() );
        m_authLog->Flush();
    }
}

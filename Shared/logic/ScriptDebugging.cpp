/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ScriptDebugging.cpp
*  PURPOSE:     Script debugging base
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define MAX_STRING_LENGTH 2048

ScriptDebugging::ScriptDebugging( LuaManager& manager ) : m_system( manager )
{
    m_fileLevel = 0;
    m_file = NULL;
}

ScriptDebugging::~ScriptDebugging()
{
    CloseLogFile();
}

void ScriptDebugging::LogCustom( unsigned char red, unsigned char green, unsigned char blue, const char *fmt, ... )
{
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, fmt );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, fmt, marker );
    va_end( marker );

    LogString( "", szBuffer, 0, red, green, blue );
}

void ScriptDebugging::LogCustom( const char *msg, unsigned char red, unsigned char green, unsigned char blue )
{
    LogString( "", msg, 0, red, green, blue );
}

void ScriptDebugging::LogInformation( const char *fmt, ... )
{
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, fmt );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, fmt, marker );
    va_end( marker );

    LogString( "INFO: ", szBuffer, 3 );
}

void ScriptDebugging::LogWarning( const char *fmt, ... )
{
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, fmt );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, fmt, marker );
    va_end( marker );

    LogString( "WARNING: ", szBuffer, 2 );
}

void ScriptDebugging::LogError( const char *fmt, ... )
{
    // Compose the formatted message
    char szBuffer[MAX_STRING_LENGTH];
    va_list marker;
    va_start( marker, fmt );
    VSNPRINTF( szBuffer, MAX_STRING_LENGTH, fmt, marker );
    va_end( marker );

    // Log it
    LogString( "ERROR: ", szBuffer, 1 );
}

void ScriptDebugging::LogBadPointer( const char *func, const char *argType, unsigned int argID )
{
    LogWarning( "Bad '%s' pointer @ '%s'(%u)", argType, func, argID );
}

void ScriptDebugging::LogBadType( const char *func )
{
    LogWarning( "Bad argument @ '%s'", func );
}

void ScriptDebugging::LogBadLevel( const char *func, unsigned int level )
{
    LogWarning( "Requires level '%d' @ '%s", level, func );
}

inline void _FileWriteString( CFile *file, const std::string& str )
{
    file->Write( str.c_str(), 1, str.size() );
}

void ScriptDebugging::CloseLogFile()
{
    if ( !m_file )
        return;

    _FileWriteString( m_file, "INFO: Logging to this file ended\n" );
        
    delete m_file;

    m_file = NULL;
}

void ScriptDebugging::LogString( const char *pre, const char *msg, unsigned int minLevel, unsigned char red, unsigned char green, unsigned char blue )
{
    SString strText;
    lua_Debug debugInfo;

    // Initialize values for onClientDebugMessage
    SString strMsg = msg;
    filePath path;
    int iLine = -1;
    lua_State *lua = m_system.GetThread();

    if ( lua && lua_getstack( lua, 1, &debugInfo ) )
    {
        lua_getinfo( lua, "nlS", &debugInfo );

        // Make sure this function isn't defined in a string (eg: from runcode)
        if ( debugInfo.source[0] == '@' )
        {
            // Get and store the location of the debug message
            PathRelative( debugInfo.source + 1, path );
            iLine = debugInfo.currentline;

            // Populate a message to print/send (unless "info" type)
            if ( minLevel < 3 )
                strText = SString( "%s%s:%d: %s", pre, path.c_str(), debugInfo.currentline, msg );
        }
        else
        {
            path = debugInfo.short_src;

            if ( minLevel < 3 )
                strText = SString( "%s%s %s", pre, msg, path.c_str() );
        }

        // Create a different message if type is "INFO"
        if ( minLevel > 2 )
            strText = SString( "%s%s", pre, msg );
    }
    else
        strText = SString( "%s%s", pre, msg );

    NotifySystem( minLevel, path, iLine, strText, red, green, blue );

    // Log it to the file if enough level
    if ( m_fileLevel >= minLevel )
        PrintLog( strText );
}

void ScriptDebugging::PrintLog( const char* szText )
{
    // Got a logfile?
    if ( !m_file )
        return;

    // Log it, timestamped
    char szBuffer[64];
    time_t timeNow;
    time( &timeNow );
    strftime( szBuffer, 32, "[%Y-%m-%d %H:%M:%S]", localtime( &timeNow ) );

    _FileWriteString( m_file, SString( "%s %s\n", szBuffer, szText ) );
    m_file->Flush();
}

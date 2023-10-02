/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CScriptDebugging.cpp
*  PURPOSE:     Script debugging
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CScriptDebugging::CScriptDebugging( CLuaManager& debug ) : ScriptDebugging( debug )
{
    m_triggerCall = false;
}

void CScriptDebugging::NotifySystem( unsigned int level, const filePath& filename, int line, std::string& msg, unsigned char r, unsigned char g, unsigned char b )
{
    if ( !m_triggerCall )
    {
        m_triggerCall = true;

        // Prepare onDebugMessage
        lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();

        lua_checkstack( L, 4 );

        lua_pushlstring( L, msg.c_str(), msg.size() );
        lua_pushnumber( L, 1 );

        // Push the file name (if any)
        if ( !filename.empty() )
            lua_pushlstring( L, filename.c_str(), filename.size() );
        else
            lua_pushnil( L );

        // Push the line (if any)
        if ( line != -1 )
            lua_pushnumber( L, line );
        else
            lua_pushnil( L );
        
        // Call onDebugMessage
        g_pClientGame->GetRootEntity()->CallEvent( "onClientDebugMessage", L, 4 );

        m_triggerCall = false;
    }

#ifdef MTA_DEBUG
    if ( !g_pCore->IsDebugVisible() )
        return;
#endif

    switch( level )
    {
    case 1:
        r = 255, g = 0, b = 0;
        break;
    case 2:
        r = 255, g = 128, b = 0;
        break;
    case 3:
        r = 0, g = 255, b = 0;
        break;
    default:
        r = 180, g = 180, b = 220;
        break;
    }

    g_pCore->DebugEchoColor( msg.c_str(), r, g, b );
}

void CScriptDebugging::PathRelative( const char *in, filePath& out )
{
    modFileRoot->GetRelativePath( in, true, out );
}

inline void _FileWriteString( CFile *file, const std::string& string )
{
    file->Write( string.c_str(), 1, string.size() );
}

bool CScriptDebugging::SetLogfile( const char *filename, unsigned int level )
{
    // Close the previously loaded file
    if ( m_file )
    {
        _FileWriteString( m_file, "INFO: Logging to this file ended\n" );
        
        delete m_file;
        m_file = NULL;
    }

    if ( !filename || !*filename )
        return false;

    CFile *file = modFileRoot->Open( filename, "a+" );

    if ( !file )
        return false;

    // Set the new pointer and level and return true
    m_fileLevel = level;
    m_file = file;
    return true;
}
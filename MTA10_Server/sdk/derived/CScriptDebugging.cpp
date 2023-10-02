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

CScriptDebugging::CScriptDebugging()
{
    m_triggerCall = false;
    m_htmlLogLevel = 0;
}

CScriptDebugging::~CScriptDebugging()
{
    ClearPlayers();
}

bool CScriptDebugging::AddPlayer( CPlayer& player, unsigned int level )
{
    if ( level )
    {
        // He's not already debugging? Add him to our list.
        if ( !player.m_uiScriptDebugLevel )
            m_players.push_back( &player );
    }
    else
    {
        // He's already script debugging? Remove him from our list. 
        if ( player.m_uiScriptDebugLevel )
            m_players.remove( &player );
    }

    player.m_uiScriptDebugLevel = level;
    return true;
}

bool CScriptDebugging::RemovePlayer( CPlayer& player )
{
    // Is he script debugging?
    if ( player.m_uiScriptDebugLevel == 0 )
        return false;

    player.m_uiScriptDebugLevel = 0;

    m_players.remove( &player );
    return true;
}

void CScriptDebugging::ClearPlayers()
{
    // Unreference us from all players that we were logging to
    std::list <CPlayer*>::const_iterator iter = m_players.begin();

    for ( ; iter != m_players.end(); iter++ )
        (*iter)->m_uiScriptDebugLevel = 0;

    m_players.clear();
}

void CScriptDebugging::LogError( const char *path, int line, SString msg )
{
    filePath relPath;
    PathRelative( path, relPath );

    SString strText = SString( "ERROR: %s:%d: %s", relPath.c_str(), iLine, strMsg.c_str() );

    NotifySystem( strFile, iLine, strText.c_str() );

    // Log it to the file if enough level
    if ( m_fileLevel >= 1 )
        PrintLog( strText.c_str() );

    // Log to console
    CLogger::LogPrintf( "%s\n", strText.c_str() );

    // Tell the players
    Broadcast( CDebugEchoPacket( strText, 1, 255, 255, 255 ), 1 );
}

bool CScriptDebugging::SetLogfile( const char *filename, unsigned int level )
{
    // Close the previously loaded file
    if ( m_file )
    {
        m_file->Printf( "INFO: Logging to this file ended\n" );
        
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

void CScriptDebugging::SetHTMLLogLevel( unsigned int level )
{
    m_htmlLogLevel = level;
}

void CScriptDebugging::NotifySystem( unsigned int level, const filePath& filename, int line, std::string& msg, unsigned char r, unsigned char g, unsigned char b )
{
    const CLuaMain *context = g_pGame->GetLuaManager()->GetStatus();

    if ( m_triggerCall )
    {
        m_triggerCall = true;

        // Prepare onDebugMessage
        CLuaArguments Arguments;
        Arguments.PushString( msg.c_str() );
        Arguments.PushNumber( 1 );

        // Push the file name (if any)
        if ( !filename.empty() )
            Arguments.PushString( filename.c_str() );
        else
            Arguments.PushNil();

        // Push the line (if any)
        if ( line != -1 )
            Arguments.PushNumber( line );
        else
            Arguments.PushNil();
        
        // Call onDebugMessage
        g_pClientGame->GetRootEntity()->CallEvent( "onDebugMessage", Arguments, false );

        m_triggerCall = false;
    }

    // Log to console
    CLogger::LogPrintf( "%s\n", msg.c_str() );

    if ( m_htmlLogLevel >= level && context )
    {
        if ( CResourceFile *file = context->GetResourceFile() && file->GetType() == CResourceHTMLItem::RESOURCE_FILE_TYPE_HTML )
        {
            CResourceHTMLItem *html = (CResourceHTMLItem*)file;
            html->AppendToPageBuffer( msg.c_str(), msg.size() );
            html->AppendToPageBuffer( "<br/>" );
        }
    }

    // Tell the players
    Broadcast( CDebugEchoPacket( strText, level, r, g, b ), level );
}

void CScriptDebugging::PathRelative( const char *in, filePath& out )
{
    modFileRoot->GetRelativePath( in, true, out );
}

void CScriptDebugging::Broadcast( const CPacket& packet, unsigned int level )
{
    // Tell everyone we log to about it
    std::list <CPlayer*>::const_iterator iter = m_players.begin();

    for ( ; iter != m_players.end(); iter++ )
    {
        if ( (*iter)->m_uiScriptDebugLevel < level )
            continue;

        (*iter)->Send( packet );
    }
}
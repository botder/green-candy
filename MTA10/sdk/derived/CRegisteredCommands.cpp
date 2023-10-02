/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CRegisteredCommands.cpp
*  PURPOSE:     Extended Console command registry
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CCommand::CCommand( CRegisteredCommands& cmds, lua_State *L ) : Command( cmds, L )
{
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, RegisteredCommands::luaconstructor_command, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );
}

CCommand::~CCommand()
{
}

bool CCommand::Execute( std::vector <std::string>& args )
{
    args.insert( args.begin(), key );
    return Command::Execute( args );
}

CRegisteredCommands::CRegisteredCommands( CLuaManager& manager ) : RegisteredCommands( manager )
{
}

CRegisteredCommands::~CRegisteredCommands()
{
}

bool CRegisteredCommands::Add( LuaMain& lua, const std::string& key, const LuaFunctionRef& ref, bool caseSensitive )
{
    if ( key.size() == 0 )
        return false;

    Command& cmd = *new CCommand( *this, *lua );
    cmd.SetRoot( lua.GetResource() );
    cmd.lua = &lua;
    cmd.key = key;
    cmd.ref = ref;
    cmd.caseSensitive = caseSensitive;

    m_commands.push_back( &cmd );
    return true;
}
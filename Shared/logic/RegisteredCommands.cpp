/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/RegisteredCommands.cpp
*  PURPOSE:     Console command registry
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg cmdInterface[] =
{
    { NULL, NULL }
};

int RegisteredCommands::luaconstructor_command( lua_State *L )
{
    lua_basicprotect( L );

    lua_refclass( L, LUA_ENVIRONINDEX )->RegisterInterface( L, cmdInterface, 0 );

    lua_pushlstring( L, "command", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

typedef std::vector <std::string> argList_t;

Command::~Command()
{
    manager.m_commands.remove( this );
}

bool Command::Execute( argList_t& args )
{
    lua_State *L = **lua;

    lua->PushReference( ref );

    argList_t::const_iterator iter = args.begin();

    for ( ; iter != args.end(); ++iter )
        lua_pushlstring( L, iter->c_str(), iter->size() );

    return lua->PCallStackVoid( (int)args.size() );
}

RegisteredCommands::RegisteredCommands( LuaManager& manager ) : m_system( manager )
{
}

RegisteredCommands::~RegisteredCommands()
{
}

bool RegisteredCommands::Remove( LuaMain *lua, const std::string& key )
{
    Command *cmd = Get( key.c_str(), lua );

    if ( !cmd )
        return false;

    // Delete it and remove it from our list
    cmd->Destroy();
    return true;
}

void RegisteredCommands::Clear()
{
    commandList_t::iterator iter = m_commands.begin();
    luaRefs refs;

    for ( ; iter != m_commands.end(); ++iter )
    {
        (*iter)->Reference( refs );
        (*iter)->Destroy();
    }
}

void RegisteredCommands::CleanUp( LuaMain *lua )
{
    commandList_t::iterator iter = m_commands.begin();
    luaRefs refs;

    for ( ; iter != m_commands.end(); ++iter )
    {
        Command& cmd = **iter;

        if ( cmd.lua != lua )
            continue;

        cmd.Reference( refs );
        cmd.Destroy();
    }
}

bool RegisteredCommands::Exists( const char *key, LuaMain *lua )
{
    return Get( key, lua ) != NULL;
}

Command* RegisteredCommands::Get( const char *key, LuaMain *lua )
{
    commandList_t::iterator iter = m_commands.begin ();
    bool exact;

    for ( ; iter != m_commands.end(); ++iter )
    {
        Command& cmd = **iter;

        if ( cmd.caseSensitive )
            exact = ( cmd.key == key );
        else
            exact = stricmp( cmd.key.c_str(), key ) == 0;

        // Matching vm's and names?
        if ( ( lua && cmd.lua != lua ) || !exact )
            continue;

        return &cmd;
    }

    return NULL;
}

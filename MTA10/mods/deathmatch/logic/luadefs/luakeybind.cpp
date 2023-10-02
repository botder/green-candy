/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/luakeybind.cpp
*  PURPOSE:     Keybind Lua instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static LUA_DECLARE( destroy )
{
    delete (CScriptKeyBind*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg keybind_interface[] =
{
    { "destroy", destroy },
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_keybind )
{
    CScriptKeyBind *bind = (CScriptKeyBind*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_KEYBIND, bind );

    lua_basicprotect( L );

    lua_pushvalue( L, LUA_ENVIRONINDEX );
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    luaL_openlib( L, NULL, keybind_interface, 1 );

    // Create the arguments table
    lua_newtable( L );
    lua_setfield( L, LUA_ENVIRONINDEX, "args" );

    lua_pushlstring( L, "keybind", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static ILuaClass* _trefget( lua_State *L, CScriptKeyBind *bind )
{
    lua_pushlightuserdata( L, bind );
    lua_pushcclosure( L, luaconstructor_keybind, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );
    
	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

CScriptKeyBind::CScriptKeyBind( CLuaMain *lua ) : LuaClass( **lua, _trefget( **lua, this ) ), m_lua( lua )
{
    g_pClientGame->GetScriptKeyBinds()->m_List.push_back( this );
}

CScriptKeyBind::~CScriptKeyBind()
{
    g_pClientGame->GetScriptKeyBinds()->m_List.remove( this );
}

void CScriptKeyBind::AcquireArguments( lua_State *L, int argCount )
{
    if ( argCount < 1 )
        return;

    int tidx = lua_gettop( L ) - argCount;

    m_class->PushEnvironment( L );
    lua_getfield( L, -1, "args" );

    lua_insert( L, tidx );
    lua_pop( L, 1 );

    lua_stack2table( L, tidx, argCount );

    lua_pop( L, 1 );
}

void CScriptKeyBind::Execute( bool state )
{
    lua_State *L = **m_lua;

    PushStack( L );

    m_lua->PushReference( m_ref );
    lua_pushstring( L, GetKeyName() );
    lua_pushstring( L, state ? "down" : "up" ); // Here we assume that the executive environment manages us properly

    lua_getfield( L, -4, "args" );
    unsigned int count = lua_objlen( L, lua_gettop( L ) );

    lua_checkstack( L, (int)count + 2 );
    lua_unpack( L );

    m_lua->PCallStackVoid( 2 + count );
    lua_pop( L, 1 );
}
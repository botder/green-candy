/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEvent.cpp
*  PURPOSE:     Map event class
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg mapevent_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_mapevent( lua_State *L )
{
    CMapEvent *evt = (CMapEvent*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_MAPEVENT, evt );

    j.RegisterInterfaceTrans( L, mapevent_interface, 0, LUACLASS_MAPEVENT );

    lua_pushlstring( L, "mapevent", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CMapEvent::CMapEvent( CLuaMain *main, CMapEventManager& manager, const char *name, const LuaFunctionRef& funcRef, bool propagated ) : LuaElement( **main ), m_system( manager )
{
    // Lua instancing
    lua_State *L = **main;

    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_mapevent, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Init
    m_main = main;
    m_funcRef = funcRef;
    m_propagated = propagated;

    strncpy( m_name, name, MAPEVENT_MAX_LENGTH_NAME );
    m_name[ MAPEVENT_MAX_LENGTH_NAME ] = '\0';
}

CMapEvent::~CMapEvent()
{
    // Clean up after ourself
    LIST_REMOVE( m_node );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaTimer.cpp
*  PURPOSE:     Lua timed execution
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static int timer_reset( lua_State *L )
{
    ((LuaTimer*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->SetStartTime( std::chrono::system_clock::now() );
    return 0;
}

static int timer_getDetails( lua_State *L )
{
    LuaTimer& timer = *(LuaTimer*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    lua_pushnumber( L, (lua_Number)timer.GetTimeLeft().count() );
    lua_pushnumber( L, timer.GetRepeats() );
    lua_pushnumber( L, (lua_Number)timer.GetDelay().count() );
    return 3;
}

static int timer_destroy( lua_State *L )
{
    delete (LuaTimer*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg timer_methods[] =
{
    { "reset", timer_reset },
    { "getDetails", timer_getDetails },
    { "destroy", timer_destroy },
    { NULL, NULL }
};

static int luaconstructor_timer( lua_State *L )
{
    LuaTimer *timer = (LuaTimer*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass *j = lua_refclass( L, 1 );
    j->SetTransmit( LUACLASS_TIMER, timer );

    lua_basicprotect( L );

    lua_newtable( L );
    lua_setfield( L, LUA_ENVIRONINDEX, "args" );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j->RegisterInterface( L, timer_methods, 1 );
    return 0;
}

static ILuaClass* _trefget( LuaTimer& timer, lua_State *L )
{
    lua_pushlightuserdata( L, &timer );
    lua_pushcclosure( L, luaconstructor_timer, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );

	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

LuaTimer::LuaTimer( lua_State *lua, LuaTimerManager *manager, const LuaFunctionRef& ref ) : LuaClass( lua, _trefget( *this, lua ) )
{
    m_ref = ref;
    m_manager = manager;
}

LuaTimer::~LuaTimer()
{
    // Remove it from the internal table
    m_manager->m_list.remove( this );
}

void LuaTimer::ObtainArguments( lua_State *L, int idx )
{
    PushStack( L );
    lua_getfield( L, -1, "args" );
    lua_insert( L, idx );
    lua_pop( L, 1 );

    int top = lua_gettop( L );

    lua_stack2table( L, idx, top - idx );
}

void LuaTimer::Execute( LuaMain& main )
{
    if ( !VERIFY_FUNCTION( m_ref ) )
        return;

    lua_class_reference ref;
    Reference( ref );

    lua_State *L = *main;

    PushStack( L );
    main.PushReference( m_ref );

    int top = lua_gettop( L );
    lua_getfield( L, -2, "args" );
    lua_unpack( L );
    main.PCallStackVoid( lua_gettop( L ) - top );

    lua_pop( L, 1 );

    if ( m_repCount == 1 )
    {
        // Request destruction if not to be repeated anymore
        ref.GetClass()->RequestDestruction();
        return;
    }

    // 0 means infinite repetitions
    if ( m_repCount != 0 )
        m_repCount--;
}

LuaTimer::duration LuaTimer::GetTimeLeft()
{
    return ( m_startTime - std::chrono::system_clock::now() ) + m_delay;
}

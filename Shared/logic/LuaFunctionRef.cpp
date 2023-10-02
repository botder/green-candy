/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaFunctionRef.cpp
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

LuaFunctionRef::LuaFunctionRef()
{
    m_lua = NULL;
    m_ref = LUA_REFNIL;
    m_call = NULL;
}

LuaFunctionRef::LuaFunctionRef( LuaMain *lua, int ref, const void *call )
{
    m_lua = lua;
    m_ref = ref;
    m_call = call;
}

LuaFunctionRef::LuaFunctionRef( const LuaFunctionRef& other )
{
    m_lua = other.m_lua;
    m_ref = other.m_ref;
    m_call = other.m_call;

    m_lua->Reference( *this );
}

LuaFunctionRef::~LuaFunctionRef()
{
    if ( m_lua )
        m_lua->Dereference( *this );
}

void LuaFunctionRef::Push( lua_State *L )
{
    m_lua->m_system.PushReference( L, *this );
}

LuaFunctionRef& LuaFunctionRef::operator = ( const LuaFunctionRef& other )
{
    if ( m_ref != LUA_REFNIL )
        m_lua->Dereference( *this );

    m_lua = other.m_lua;
    m_ref = other.m_ref;
    m_call = other.m_call;

    m_lua->Reference( *this );
    return *this;
}

int LuaFunctionRef::ToInt() const
{
    return m_ref;
}

bool LuaFunctionRef::operator == ( const LuaFunctionRef& other ) const
{
    return m_lua == other.m_lua && m_ref == other.m_ref;
}

bool LuaFunctionRef::operator != ( const LuaFunctionRef& other ) const
{
    return !(operator == ( other ));
}

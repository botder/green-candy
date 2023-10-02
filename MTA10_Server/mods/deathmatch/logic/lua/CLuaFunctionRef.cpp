/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionRef.cpp
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"


CLuaFunctionRef::CLuaFunctionRef ( void )
{
    m_luaVM = NULL;
    m_iFunction = LUA_REFNIL;
    m_pFuncPtr = NULL;
}

CLuaFunctionRef::CLuaFunctionRef ( lua_State *luaVM, int iFunction, const void* pFuncPtr )
{
    m_luaVM = lua_getmainstate ( luaVM );
    m_iFunction = iFunction;
    m_pFuncPtr = pFuncPtr;
}

CLuaFunctionRef::CLuaFunctionRef ( const CLuaFunctionRef& other )
{
    m_luaVM = other.m_luaVM;
    m_iFunction = other.m_iFunction;
    m_pFuncPtr = other.m_pFuncPtr;
    luaM_inc_use ( m_luaVM, m_iFunction, m_pFuncPtr );
}

CLuaFunctionRef::~CLuaFunctionRef ( void )
{
    luaM_dec_use ( m_luaVM, m_iFunction, m_pFuncPtr );
}

CLuaFunctionRef& CLuaFunctionRef::operator=( const CLuaFunctionRef& other )
{
    luaM_dec_use ( m_luaVM, m_iFunction, m_pFuncPtr );

    m_luaVM = other.m_luaVM;
    m_iFunction = other.m_iFunction;
    m_pFuncPtr = other.m_pFuncPtr;
    luaM_inc_use ( m_luaVM, m_iFunction, m_pFuncPtr );
    return *this;
}

int CLuaFunctionRef::ToInt ( void ) const
{
    return m_iFunction;
}

bool CLuaFunctionRef::operator==( const CLuaFunctionRef& other ) const
{
    return m_luaVM == other.m_luaVM
        && m_iFunction == other.m_iFunction;
}

bool CLuaFunctionRef::operator!=( const CLuaFunctionRef& other ) const
{
    return !operator==( other );
}

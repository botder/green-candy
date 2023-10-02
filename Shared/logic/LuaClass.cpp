/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaClass.cpp
*  PURPOSE:     Lua type instancing for persistent VM objects
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Needs to be put here so we can access g_luaManager for GC registering!
LuaClass::LuaClass( lua_State *L, ILuaClass *j )
{
    m_lua = lua_getmainstate( L );		// for security reasons; main state will always be preserved
    m_class = j;

	LIST_APPEND( g_luaManager->m_gcList.root, m_gcList );
}
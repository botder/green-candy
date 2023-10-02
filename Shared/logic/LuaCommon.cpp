/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaCommon.cpp
*  PURPOSE:     Master lua header
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

void lua_createEnvProxy( lua_State *L )
{
    lua_newtable( L );
    lua_newtable( L );
    lua_pushvalue( L, LUA_GLOBALSINDEX );
    lua_setfield( L, -2, "__index" );
    lua_setmetatable( L, -2 );
}
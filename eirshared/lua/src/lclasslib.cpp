/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vendor/lua/src/lclasslib.c
*  PURPOSE:     Class library
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "luacore.h"

#include "lclass.h"
#include "lauxlib.h"
#include "lualib.h"


static int class_construct( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TFUNCTION );
    luaJ_construct( L, lua_gettop( L ) - 1 );
    return 1;
}

static const luaL_Reg classlib[] = {
    { "construct", class_construct },
    { nullptr, nullptr }
};

LUA_API int luaopen_class( lua_State *L )
{
    luaL_register( L, LUA_CLASSLIBNAME, classlib );
    return 1;
}
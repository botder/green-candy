/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vendor/lua/src/ldispatch.h
*  PURPOSE:     Lua dispatch extension
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_DISPATCHER_
#define _LUA_DISPATCHER_

LUAI_FUNC void luaQ_free( lua_State *L, Dispatch *q );

// Module initialization.
LUAI_FUNC void luaQ_init( lua_config *cfg );
LUAI_FUNC void luaQ_shutdown( lua_config *cfg );

// Runtime initialization.
LUAI_FUNC void luaQ_runtimeinit( global_State *g );
LUAI_FUNC void luaQ_runtimeshutdown( global_State *g );

#endif //_LUA_DISPATCHER_
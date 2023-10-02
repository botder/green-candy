/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luaclass.h
*  PURPOSE:     Lua filesystem access
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef FU_CLASS
#define FU_CLASS

void luaclass_pushshim( lua_State *L, void *classptr );
void* luaclass_getpointer( lua_State *L, int idx );

#define LUA_CHECK( val ) if ( !(val) ) { lua_pushboolean( L, false ); return 1; }

#endif //FU_CLASS

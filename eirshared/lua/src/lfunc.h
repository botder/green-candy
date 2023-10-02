/*
** $Id: lfunc.h,v 2.4.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#ifndef lfunc_h
#define lfunc_h


#include "lobject.h"

LUAI_FUNC Proto *luaF_newproto (lua_State *L);
LUAI_FUNC void luaF_freeproto (lua_State *L, Proto *p);
LUAI_FUNC ConstValueAddress luaF_getprotoconstaddress (lua_State *L, Proto *p, int idx);
LUAI_FUNC CClosureBasic *luaF_newCclosure (lua_State *L, int nelems, GCObject *e);
LUAI_FUNC LClosure *luaF_newLclosure (lua_State *L, int nelems, GCObject *e);
LUAI_FUNC TValue* luaF_getcurraccessor( lua_State *L );
LUAI_FUNC UpVal *luaF_newupval (lua_State *L);
LUAI_FUNC void luaF_freeupval (lua_State *L, UpVal *u);
LUAI_FUNC UpVal *luaF_findupval (lua_State *L, StkId level);
LUAI_FUNC void luaF_gcruntime( lua_State *L );
LUAI_FUNC void luaF_close (lua_State *L, StkId_const level);
LUAI_FUNC void luaF_freeclosure (lua_State *L, Closure *c);
LUAI_FUNC const char *luaF_getlocalname (const Proto *func, stackOffset_t local_number, unsigned int pc);

// Module initialization.
LUAI_FUNC void luaF_init( lua_config *cfg );
LUAI_FUNC void luaF_shutdown( lua_config *cfg );

// Runtime initialization.
LUAI_FUNC void luaF_runtimeinit( global_State *g );
LUAI_FUNC void luaF_runtimeshutdown( global_State *g );

#endif

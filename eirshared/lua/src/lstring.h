/*
** $Id: lstring.h,v 1.43.1.1 2007/12/27 13:02:25 roberto Exp $
** String table (keep all strings handled by Lua)
** See Copyright Notice in lua.h
*/

#ifndef lstring_h
#define lstring_h


#include "lgc.h"
#include "lobject.h"
#include "lstate.h"


#define luaS_new(L, s)	        (luaS_newlstr(L, s, strlen(s)))
#define luaS_newliteral(L, s)	(luaS_newlstr(L, "" s, (sizeof(s)/sizeof(char))-1))

#define luaS_fix(s)	            l_setbit((s)->marked, FIXEDBIT)

LUAI_FUNC void luaS_resize (lua_State *L, int newsize);
LUAI_FUNC Udata *luaS_newudata (lua_State *L, size_t s, GCObject *e);
LUAI_FUNC void luaS_freeudata (lua_State *L, Udata *u);
LUAI_FUNC TString *luaS_newlstr (lua_State *L, const char *str, size_t l);
LUAI_FUNC int luaS_concat (lua_State *L, int topOffset, int total);
LUAI_FUNC void luaS_globalgc (lua_State *L);
LUAI_FUNC void luaS_free (lua_State *L, TString *s);

// Global state initialization.
LUAI_FUNC void luaS_stateinit( lua_State *L );
LUAI_FUNC void luaS_stateshutdown( lua_State *L );

// Module initialization.
LUAI_FUNC void luaS_init( lua_config *cfg );
LUAI_FUNC void luaS_shutdown( lua_config *cfg );

#endif

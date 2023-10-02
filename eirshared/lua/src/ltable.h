/*
** $Id: ltable.h,v 2.10.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"

// Public Table API.
LUAI_FUNC ConstValueAddress     luaH_getnum (lua_State *L, Table *t, int key);
LUAI_FUNC ConstValueAddress     luaH_getznum(lua_State *L, Table *t, size_t key);
LUAI_FUNC ValueAddress          luaH_setnum (lua_State *L, Table *t, int key);
LUAI_FUNC ValueAddress          luaH_setznum(lua_State *L, Table *t, size_t key);
LUAI_FUNC ConstValueAddress     luaH_getstr (lua_State *L, Table *t, TString *key);
LUAI_FUNC ValueAddress          luaH_setstr (lua_State *L, Table *t, TString *key);
LUAI_FUNC ConstValueAddress     luaH_get (lua_State *L, Table *t, const TValue *key);
LUAI_FUNC ValueAddress          luaH_set (lua_State *L, Table *t, const TValue *key);
LUAI_FUNC Table*                luaH_new (lua_State *L, size_t narray, size_t lnhash);
LUAI_FUNC void                  luaH_free (lua_State *L, Table *t);
LUAI_FUNC void                  luaH_resizearray (lua_State *L, Table *t, size_t nasize);
LUAI_FUNC void                  luaH_ensureslots (lua_State *L, Table *t, size_t last);
LUAI_FUNC bool                  luaH_next (lua_State *L, Table *t);
LUAI_FUNC size_t                luaH_getn (lua_State *L, Table *t);

// Module initialization.
void luaH_init( lua_config *cfg );
void luaH_shutdown( lua_config *cfg );
void luaH_clearRuntimeMemory( global_State *g );

#endif

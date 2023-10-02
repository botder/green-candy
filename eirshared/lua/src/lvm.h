/*
** $Id: lvm.h,v 2.5.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "lobject.h"
#include "ltm.h"


LUAI_FUNC int luaV_lessthan                 (lua_State *L, ConstValueAddress& l, ConstValueAddress& r);
LUAI_FUNC int luaV_equalval                 (lua_State *L, ConstValueAddress& t1, ConstValueAddress& t2);
LUAI_FUNC ConstValueAddress luaV_tonumber   (ConstValueAddress& obj, ValueAddress& n);
LUAI_FUNC int luaV_tostring                 (lua_State *L, ValueAddress& obj);
LUAI_FUNC void luaV_handle_index            (lua_State *L, ConstValueAddress& obj, ConstValueAddress& tm, ConstValueAddress& key, ValueAddress& val);
LUAI_FUNC void luaV_gettable                (lua_State *L, ConstValueAddress& t, ConstValueAddress& key, ValueAddress& val);
LUAI_FUNC void luaV_handle_newindex         (lua_State *L, ConstValueAddress& obj, ConstValueAddress& tm, ConstValueAddress& key, ConstValueAddress& val);
LUAI_FUNC void luaV_settable                (lua_State *L, ConstValueAddress& t, ConstValueAddress& key, ConstValueAddress& val);
LUAI_FUNC void luaV_execute                 (lua_State *L, int nexeccalls, eCallFrameModel callFrameModel);
LUAI_FUNC void luaV_concat                  (lua_State *L, stackOffset_t total, stackOffset_t last);

// Macros to simplify the API.
FASTAPI int tostring( lua_State *L, ValueAddress& o )
{
    return ((ttype(o) == LUA_TSTRING) || (luaV_tostring(L, o)));
}

FASTAPI int tonumber( ConstValueAddress& o, ValueAddress& n )
{
    return (ttype(o) == LUA_TNUMBER || (o = luaV_tonumber(o,n)) != nullptr);
}

FASTAPI int equalobj( lua_State *L, ConstValueAddress& o1, ConstValueAddress& o2 )
{
    return (ttype(o1) == ttype(o2) && luaV_equalval(L, o1, o2));
}

// Module initialization.
LUAI_FUNC void luaV_init( lua_config *cfg );
LUAI_FUNC void luaV_shutdown( lua_config *cfg );

#endif

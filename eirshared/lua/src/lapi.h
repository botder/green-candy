/*
** $Id: lapi.h,v 2.2.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include "lobject.h"


FASTAPI void api_checknelems( lua_State *L, stackOffset_t n )
{
    api_check( L, (n) <= rt_stackcount( L ) );
}
FASTAPI void api_checkvalidindex( lua_State *L, const TValue *i )
{
    api_check(L, (i) != luaO_nilobject);
}

LUAI_FUNC void luaA_pushobject (lua_State *L, const TValue *o);

// Library lifetime management.
LUAI_FUNC void lua_reflibrary( void );
LUAI_FUNC void lua_unreflibrary( void );

// Module initialization.
LUAI_FUNC void luaapi_init( lua_config *cfg );
LUAI_FUNC void luaapi_shutdown( lua_config *cfg );

#endif

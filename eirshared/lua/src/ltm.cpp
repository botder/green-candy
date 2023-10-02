/*
** $Id: ltm.c,v 2.8.1.1 2007/12/27 13:02:25 roberto Exp $
** Tag methods
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"

#include "ltable.hxx"


const char *const luaT_typenames[] =
{
    "nil",          // 0
    "boolean",      // 1
    "userdata",     // 2
    "number",       // 3
    "string",       // 4
    "table",        // 5
    "function",     // 6
    "userdata",     // 7
    "class",        // 8
    "dispatch",     // 9
    "object",       // 10
    "thread",       // 11
    "runtime"       // 12
};


void luaT_init (lua_State *L)
{
    static const char *const luaT_eventname[] =
    {  /* ORDER TM */
        "__index",
        "__newindex",
        "__gc",
        "__mode",
        "__eq",
        "__add",
        "__sub",
        "__mul",
        "__div",
        "__mod",
        "__pow",
        "__unm",
        "__len",
        "__lt",
        "__le",
        "__concat",
        "__call",
        "__redirect",
        "__metatable",
        "__type"
    };

    global_State *g = G(L);

    for ( int i = 0; i < TM_N; i++ )
    {
        TString *cachedTMName = luaS_new(L, luaT_eventname[i]);

        g->tmname[i] = cachedTMName;

        luaS_fix(cachedTMName);  /* never collect these names */

        // Since we fixed the TM name, we can dereference it.
        cachedTMName->DereferenceGC( L );
    }
}

#define _LUA_NUMELMS(x)     ( sizeof(x) / sizeof(*x) )

unsigned int luaT_getnumtypes( lua_State *L )
{
    UNUSED(L);
    return _LUA_NUMELMS( luaT_typenames );
}

int luaT_gettype( lua_State *L, unsigned int index )
{
    UNUSED(L);

    const int theTypes[] =
    {
        LUA_TNIL,
        LUA_TBOOLEAN,
        LUA_TUSERDATA,
        LUA_TNUMBER,
        LUA_TSTRING,
        LUA_TTABLE,
        LUA_TFUNCTION,
        LUA_TUSERDATA,
        LUA_TCLASS,
        LUA_TDISPATCH,
        LUA_TTHREAD,
        LUA_TOBJECT,
        LUA_TRUNTIME
    };

    int iTypeNum = -1;

    if ( index < _LUA_NUMELMS(theTypes) )
    {
        iTypeNum = theTypes[ index ];
    }

    return iTypeNum;
}

const char* luaT_gettypename( lua_State *L, int typeIndex )
{
    return (typeIndex == LUA_TNONE || typeIndex < 0 || typeIndex >= (int)luaT_getnumtypes( L )) ? "no value" : luaT_typenames[typeIndex];
}

/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
bool luaT_gettm (lua_State *L, Table *events, TMS event, TString *ename, ConstValueAddress& outTM)
{
    ConstValueAddress tm = luaH_getstr(L, events, ename);

    lua_assert(event <= TM_EQ);

    if (ttisnil(tm))
    {  /* no tag method? */

        tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( events );

        if ( nativeTable )
        {
            nativeTable->flags |= bitmask((lu_byte)event);  /* cache this fact */
        }
        return false;
    }
    else
    {
        outTM = std::move( tm );
        return true;
    }
}

ConstValueAddress gfasttm( global_State *g, Table *et, TMS e )
{
    ConstValueAddress retAddr;

    if ( et != nullptr )
    {
        tableNativeImplementation *nativeTable = tableNativeImplementation::GetNativeImplementation( et );

        if ( nativeTable != nullptr )
        {
            // Use the table optimization flags.
            // They are reset on any table modification.
            if ( testbit( nativeTable->flags, (lu_byte)e ) == false )
            {
                luaT_gettm( g->mainthread, et, e, (g)->tmname[e], retAddr );
            }
        }
    }
    return retAddr;
}

Table* luaT_getmetabyobj( lua_State *L, const TValue *o )
{
    switch( ttype(o) )
    {
    case LUA_TTABLE:
        return hvalue(o)->metatable;
    case LUA_TUSERDATA:
        return uvalue(o)->metatable;
    }

    return L->mt[ttype(o)];
}

ConstValueAddress luaT_gettmbyobj( lua_State *L, const TValue *o, TMS event )
{
    if ( ttype(o) == LUA_TCLASS )
    {
        Table *classStorage = jvalue(o)->storage;

        if ( classStorage )
        {
            ConstValueAddress res = luaH_getstr( L, classStorage, G(L)->tmname[event] );

            if ( res )
                return res;
        }
    }
    Table *mt = luaT_getmetabyobj( L, o );
    return (mt ? luaH_getstr( L, mt, G(L)->tmname[event] ) : luaO_getnilcontext( L ));
}

// Module initialization.
void luaT_moduleinit( lua_config *cfg )
{
    return;
}

void luaT_moduleshutdown( lua_config *cfg )
{
    return;
}

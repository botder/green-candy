// Lua GC common interface shared include
#ifndef _LUA_GC_SHARED_INTERFACE_
#define _LUA_GC_SHARED_INTERFACE_

#include "lobject.h"
#include "lgc.h"

// Exports from lgc.c
void reallymarkobject( global_State *g, GCObject *o );

#define maskmarks	cast_byte(~(bitmask(BLACKBIT)|WHITEBITS))

FASTAPI void makewhite( global_State *g, GCObject *x )              { ((x)->marked = cast_byte(((x)->marked & maskmarks) | luaC_white(g))); }
FASTAPI void white2gray( GCObject *x )                              { reset2bits((x)->marked, WHITE0BIT, WHITE1BIT); }
FASTAPI void black2gray( GCObject *x )                              { resetbit((x)->marked, BLACKBIT); }
FASTAPI void stringmark( TString *s )                               { reset2bits((s)->marked, WHITE0BIT, WHITE1BIT); }

FASTAPI bool isfinalized( GCObject *u )                             { return testbit((u)->marked, FINALIZEDBIT); }
FASTAPI void markfinalized( GCObject *u )                           { l_setbit((u)->marked, FINALIZEDBIT); }

FASTAPI void markvalue( global_State *g, TValue *o )
{
    checkconsistency(o);

    if ( iscollectable(o) && iswhite(gcvalue(o)) )
    {
        reallymarkobject(g,gcvalue(o));
    }
}
FASTAPI void markobject( global_State *g, GCObject *x )
{
    if ( iswhite( x ) )
    {
        reallymarkobject( g, x );
    }
}

#endif //_LUA_GC_SHARED_INTERFACE_
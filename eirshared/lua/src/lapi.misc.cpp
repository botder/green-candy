#include "luacore.h"

#include "lapi.h"
#include "ldebug.h"
#include "lgc.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lclass.h"
#include "lvm.h"

#include "lfunc.upval.hxx"

#include "lapi.hxx"


/*
** miscellaneous functions
*/

LUA_API void lua_pushtype( lua_State *L, int idx )
{
    ConstValueAddress o = index2constadr( L, idx );

    if ( ttype(o) == LUA_TCLASS )
    {
        if ( jvalue(o)->destroyed )
        {
            lua_pushlstring( L, "destroyed_class", 15 );
            return;
        }

        const TValue *tm = luaT_gettmbyobj( L, o, TM_TYPE );

        switch( ttype(tm) )
        {
        case LUA_TSTRING:
            pushsvalue( L, tsvalue( tm ) );
            return;
        case LUA_TFUNCTION:
            pushclvalue( L, clvalue( tm ) );
            pushjvalue( L, jvalue( o ) );
            lua_call( L, 1, 1 );
            return;
        }
    }

    lua_pushstring(L, lua_typename( L, ttype(o) ));
}


LUA_API int lua_error (lua_State *L) {
  lua_lock(L);
  api_checknelems(L, 1);
  luaG_errormsg(L);
  lua_unlock(L);
  return 0;  /* to avoid warnings */
}

LUA_API bool lua_next( lua_State *L, int idx )
{
    lua_lock(L);

    ConstValueAddress t = index2constadr(L, idx);
    api_check(L, ttistable(t));

    bool more = luaH_next(L, hvalue(t));

    lua_unlock(L);
    return more;
}

LUA_API void lua_concat (lua_State *L, int n)
{
    lua_lock(L);
    api_checknelems(L, n);
    if (n >= 2)
    {
        luaC_checkGC(L);
        luaV_concat(L, n, rt_stackcount( L ) - 1);
        popstack( L, n-1 );
    }
    else if (n == 0)
    {  /* push empty string */
        TString *emptyStr = luaS_newlstr(L, "", 0);

        try
        {
            pushsvalue(L, emptyStr);
        }
        catch( ... )
        {
            // Even pushing things onto the stack can trigger exceptions.
            emptyStr->DereferenceGC( L );
            throw;
        }

        // Since the empty string is on the stack now, we can dereference it.
        emptyStr->DereferenceGC( L );
    }
    /* else n == 1; nothing to do */
    lua_unlock(L);
}

LUA_API void *lua_newuserdata (lua_State *L, size_t size)
{
    lua_lock(L);
    luaC_checkGC(L);

    Udata *u = luaS_newudata(L, size, getcurrenv(L));

    try
    {
        pushuvalue(L, u);
    }
    catch( ... )
    {
        u->DereferenceGC( L );
        throw;
    }

    // Since the userdata is now on the stack, we can dereference it.
    u->DereferenceGC( L );

    lua_unlock(L);
    return u + 1;
}

static const char *aux_upvalue (StkId fi, unsigned int n, TValue **val)
{
    Closure *f;

    if (!ttisfunction(fi))
        return nullptr;

    f = clvalue(fi);

    if (f->isC)
    {
        if ( n == 0 || n > 256 )
            return nullptr;

        TValue *up = f->ReadUpValue( (unsigned char)( n - 1 ) );

        if ( up == luaO_nilobject )
            return nullptr;

        *val = up;
        return "";
    }
    else
    {
        LClosure *cl = f->GetLClosure();
        Proto *p = cl->p;

        if ( n == 0 || n > p->sizeupvalues )
            return nullptr;

        *val = cl->upvals[n-1]->v;
        return getstr(p->upvalues[n-1]);
    }
}

LUA_API const char *lua_getupvalue (lua_State *L, int funcindex, int n)
{
    const char *name;
    TValue *val;
    lua_lock(L);

    if ( n < 0 )
        return nullptr;

    ValueAddress func = index2adr( L, funcindex );

    name = aux_upvalue(func, (unsigned int)n, &val);

    if ( name )
    {
        pushtvalue(L, val);
    }

    lua_unlock(L);
    return name;
}


LUA_API const char *lua_setupvalue (lua_State *L, int funcindex, int n)
{
    const char *name;
    TValue *val;
    lua_lock(L);

    if ( n < 0 )
        return nullptr;

    ValueAddress fi = index2adr(L, funcindex);
    api_checknelems(L, 1);
    name = aux_upvalue(fi, (unsigned int)n, &val);
    if (name)
    {
        popstkvalue( L, val );
        luaC_barrier(L, clvalue(fi), val);
    }
    lua_unlock(L);
    return name;
}

LUA_API void lua_setevent( lua_State *L, eLuaEvent evt, lua_CFunction proto )
{
    if ( evt > LUA_NUM_EVENTS-1 )
        return;

    lua_pushcclosure( L, proto, 0 );

    TValue tmp;
    popstkvalue( L, &tmp );

    G(L)->events[evt] = clvalue( &tmp );
}

LUA_API void lua_constructclassex( lua_State *L, int nargs, unsigned int flags )
{
    lua_lock( L );

    Class *c = luaJ_new( L, nargs, flags );
    pushjvalue( L, c );

    c->DereferenceGC( L );

    lua_unlock( L );
}

LUA_API void lua_constructclass( lua_State *L, int nargs )
{
    lua_constructclassex( L, nargs, 0 );
}

LUA_API void lua_newclass( lua_State *L )
{
    lua_constructclass( L, 0 );
}

LUA_API void lua_newclassex( lua_State *L, unsigned int flags )
{
    lua_constructclassex( L, 0, flags );
}

LUA_API ILuaClass* lua_refclass( lua_State *L, int idx )
{
    ConstValueAddress val = index2constadr( L, idx );

    if ( !iscollectable( val ) )
        return nullptr;

    return ( val->value.gc )->GetClass();
}

LUA_API void lua_basicprotect( lua_State *L )
{
    luaJ_basicprotect( L );
}

LUA_API void lua_basicextend( lua_State *L )
{
    luaJ_basicextend( L );
}

LUA_API ILuaState& lua_getstateapi( lua_State *L )
{
    return *L;
}
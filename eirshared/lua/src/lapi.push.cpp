#include "luacore.h"

#include "lapi.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lclass.h"

#include "lapi.hxx"


/*
** push functions (C -> stack)
*/

LUA_API void lua_pushnil (lua_State *L)
{
    lua_lock(L);
    pushnilvalue(L);
    lua_unlock(L);
}


LUA_API void lua_pushnumber (lua_State *L, lua_Number n)
{
    lua_lock(L);
    pushnvalue(L, n);
    lua_unlock(L);
}


LUA_API void lua_pushinteger (lua_State *L, lua_Integer n)
{
    lua_lock(L);
    pushnvalue(L, cast_num(n));
    lua_unlock(L);
}


LUA_API void lua_pushwideinteger( lua_State *L, lua_WideInteger n )
{
    lua_lock(L);
    pushnvalue(L, cast_num(n));
    lua_unlock(L);
}


LUA_API void lua_pushlstring (lua_State *L, const char *s, size_t len)
{
    lua_lock(L);
    luaC_checkGC(L);

    TString *newStr = luaS_newlstr(L, s, len);

    try
    {
        pushsvalue(L, newStr);
    }
    catch( ... )
    {
        // Exception safety is very important in C++ code.
        // Because code does not only fail when it is bad.
        newStr->DereferenceGC( L );
        throw;
    }

    // Since the new string is on the stack now, we can dereference it.
    newStr->DereferenceGC( L );

    lua_unlock(L);
}


LUA_API void lua_pushstring (lua_State *L, const char *s)
{
    if (s == nullptr)
    {
        pushnilvalue( L );
    }
    else
    {
        lua_pushlstring(L, s, strlen(s));
    }
}


LUA_API const char *lua_pushvfstring (lua_State *L, const char *fmt, va_list argp)
{
    lua_lock(L);
    luaC_checkGC(L);
    const char *ret = luaO_pushvfstring(L, fmt, argp);
    lua_unlock(L);
    return ret;
}


LUA_API const char *lua_pushfstring (lua_State *L, const char *fmt, ...)
{
    const char *ret;
    va_list argp;
    lua_lock(L);
    luaC_checkGC(L);
    va_start(argp, fmt);
    ret = luaO_pushvfstring(L, fmt, argp);
    va_end(argp);
    lua_unlock(L);
    return ret;
}


LUA_API void lua_pushcclosure (lua_State *L, lua_CFunction fn, int n)
{
    lua_lock(L);
    luaC_checkGC(L);
    api_checknelems(L, n);

    CClosureBasic *cl = luaF_newCclosure(L, n, getcurrenv(L));
    cl->f = fn;

    // Account for any kind of failure.
    try
    {
        L->GetCurrentStackFrame().CrossMoveTopExtern( L, L->rtStack, cl->upvalues, n );

        pushclvalue(L, cl);
    }
    catch( ... )
    {
        cl->DereferenceGC( L );
        throw;
    }

    // Since the closure now resides on the stack, we can dereference it.
    cl->DereferenceGC( L );

    lua_unlock(L);
}


LUA_API void lua_pushboolean (lua_State *L, int b)
{
    lua_lock(L);
    pushbvalue(L, (b != 0));  /* ensure that C integer 1 is true */
    lua_unlock(L);
}


LUA_API void lua_pushlightuserdata (lua_State *L, void *p)
{
    lua_lock(L);
    pushpvalue(L, p);
    lua_unlock(L);
}


LUA_API int lua_pushthread (lua_State *L)
{
    lua_lock(L);
    pushthvalue(L, L);
    lua_unlock(L);
    return (G(L)->mainthread == L);
}


LUA_API int lua_pushthreadex( lua_State *L, lua_State *thread )
{
    lua_lock(L);
    pushthvalue( L, thread );
    lua_unlock(L);
    return (G(L)->mainthread == thread);
}

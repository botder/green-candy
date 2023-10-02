#include "luacore.h"

#include "lapi.h"
#include "lobject.h"
#include "lstate.h"
#include "lgc.h"
#include "ltable.h"
#include "lvm.h"

#include "lapi.hxx"


/*
** access functions (stack -> C)
*/

LUA_API int lua_type (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return (o == luaO_nilobject) ? LUA_TNONE : ttype(o);
}

LUA_API const char *lua_typename (lua_State *L, int t)
{
    return luaT_gettypename( L, t );
}

LUA_API int lua_iscfunction (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return iscfunction(o);
}

LUA_API int lua_isnumber (lua_State *L, int idx)
{
    LocalValueAddress n;
    ConstValueAddress o = index2constadr(L, idx);
    return tonumber(o, n);
}

LUA_API int lua_isstring (lua_State *L, int idx) {
  int t = lua_type(L, idx);
  return (t == LUA_TSTRING || t == LUA_TNUMBER);
}

LUA_API int lua_isuserdata (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return (ttisuserdata(o) || ttislightuserdata(o));
}

LUA_API int lua_rawequal (lua_State *L, int index1, int index2)
{
    ConstValueAddress o1 = index2constadr(L, index1);
    ConstValueAddress o2 = index2constadr(L, index2);
    return (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : luaO_rawequalObj(o1, o2);
}

LUA_API int lua_equal (lua_State *L, int index1, int index2)
{
    lua_lock(L);  /* may call tag method */
    ConstValueAddress o1 = index2constadr(L, index1);
    ConstValueAddress o2 = index2constadr(L, index2);
    int i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : equalobj(L, o1, o2);
    lua_unlock(L);
    return i;
}

LUA_API int lua_lessthan (lua_State *L, int index1, int index2)
{
    lua_lock(L);  /* may call tag method */
    ConstValueAddress o1 = index2constadr(L, index1);
    ConstValueAddress o2 = index2constadr(L, index2);
    int i = (o1 == luaO_nilobject || o2 == luaO_nilobject) ? 0 : luaV_lessthan(L, o1, o2);
    lua_unlock(L);
    return i;
}

LUA_API lua_Number lua_tonumber (lua_State *L, int idx)
{
    LocalValueAddress n;
    ConstValueAddress o = index2constadr(L, idx);
    if (!tonumber(o, n))
        return 0;
    return nvalue(o);
}


LUA_API lua_Integer lua_tointeger (lua_State *L, int idx)
{
    LocalValueAddress n;
    ConstValueAddress o = index2constadr(L, idx);

    if (!tonumber(o, n))
        return 0;

    lua_Integer res;
    lua_Number num = nvalue(o);
    lua_number2integer(res, num);
    return res;
}

// MTA Specific
LUA_API lua_Integer lua_tointegerW (lua_State *L, int idx)
{
    LocalValueAddress n;
    ConstValueAddress o = index2constadr(L, idx);

    if (!tonumber(o, n))
        return 0;

    // The_GTA: Do not try to outsmart compilers.
    return (lua_Integer)nvalue(o);
}

LUA_API lua_WideInteger lua_towideinteger( lua_State *L, int idx )
{
    LocalValueAddress n;
    ConstValueAddress o = index2constadr(L, idx);

    if (!tonumber(o, n))
        return 0;

    // Transform to a wide integer (often that is 64bit)
    return (lua_WideInteger)nvalue(o);
}

LUA_API int lua_toboolean (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return !l_isfalse(o);
}

LUA_API lua_String lua_tolstring (lua_State *L, int idx, size_t *len)
{
    lua_String outString = nullptr;    // can return NULL if impossible operation.

    ConstValueAddress o = index2constadr(L, idx);

    TString *outLuaStr = nullptr;

    if (!ttisstring(o))
    {
        if ( ttype( o ) != LUA_TNIL )
        {
            ValueAddress exVal = index2adr( L, idx );

            lua_lock(L);  /* `luaV_tostring' may create a new string */
            if ( luaV_tostring(L, exVal) != 0 )
            {
                luaC_checkGC(L);

                outLuaStr = tsvalue( exVal );
            }
            lua_unlock(L);
        }
    }
    else
    {
        outLuaStr = tsvalue(o);
    }

    if ( outLuaStr != nullptr )
    {
        outString = getstr( outLuaStr );

        if ( len )
        {
            *len = outLuaStr->len;
        }
    }

    if ( outString == nullptr && len != nullptr )
    {
        *len = 0;
    }

    return outString;
}


LUA_API size_t lua_objlen (lua_State *L, int idx)
{
    ValueAddress o = index2adr(L, idx);

    switch (ttype(o))
    {
    case LUA_TSTRING: return tsvalue(o)->len;
    case LUA_TUSERDATA: return uvalue(o)->len;
    case LUA_TTABLE: return luaH_getn(L, hvalue(o));
    case LUA_TNUMBER: {
        size_t l;
        lua_lock(L);  /* `luaV_tostring' may create a new string */
        l = (luaV_tostring(L, o) ? tsvalue(o)->len : 0);
        lua_unlock(L);
        return l;
    }
    }
    return 0;
}

LUA_API lua_CFunction lua_tocfunction (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return (!iscfunction(o)) ? nullptr : clvalue(o)->GetCClosure()->f;
}

LUA_API void *lua_touserdata (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);

    switch (ttype(o))
    {
    case LUA_TUSERDATA: return (rawuvalue(o) + 1);
    case LUA_TLIGHTUSERDATA: return pvalue(o);
    }
    return nullptr;
}

LUA_API lua_State *lua_tothread (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    return (!ttisthread(o)) ? nullptr : thvalue(o);
}


LUA_API const void *lua_topointer (lua_State *L, int idx)
{
    ConstValueAddress o = index2constadr(L, idx);
    switch (ttype(o))
    {
    case LUA_TTABLE:
        return hvalue(o);
    case LUA_TFUNCTION:
        return clvalue(o);
    case LUA_TTHREAD:
        return thvalue(o);
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        return lua_touserdata(L, idx);
    case LUA_TCLASS:
        return jvalue(o);
    case LUA_TDISPATCH:
        return qvalue(o);
    default:
        return nullptr;
    }
}
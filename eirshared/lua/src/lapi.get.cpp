#include "luacore.h"

#include "lapi.h"
#include "lfunc.h"
#include "lgc.h"
#include "lstring.h"
#include "ltable.h"
#include "lclass.h"
#include "lvm.h"

#include "lfunc.class.hxx"

#include "lapi.hxx"


/*
** get functions (Lua -> stack)
*/


LUA_API void lua_gettable (lua_State *L, int idx)
{
    lua_lock(L);
    ConstValueAddress t = index2constadr(L, idx);
    api_checkvalidindex(L, t);
    ValueAddress keyAndVal = index2adr( L, -1 );
    luaV_gettable(L, t, keyAndVal.ConstCast(), keyAndVal);
    lua_unlock(L);
}


LUA_API void lua_getfield (lua_State *L, int idx, const char *k)
{
    lua_lock(L);

    ConstValueAddress t = index2constadr(L, idx);
    api_checkvalidindex(L, t);

    TString *newFieldString = luaS_new(L, k);

    try
    {
        LocalValueAddress key;

        setsvalue(L, key, newFieldString);

        {
            RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

            ValueAddress stackItem = newstackslot( L );

            luaV_gettable( L, t, key.ConstCast(), stackItem );
        }
    }
    catch( ... )
    {
        // NEVER forget to account for exceptions being thrown at any point in the runtime.
        newFieldString->DereferenceGC( L );
        throw;
    }

    // We do not need the field string anymore.
    newFieldString->DereferenceGC( L );

    lua_unlock(L);
}


LUA_API void lua_rawget (lua_State *L, int idx)
{
    lua_lock(L);
    ConstValueAddress t = index2constadr(L, idx);
    api_check(L, ttistable(t));
    {
        RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

        StkId stackTop = L->GetCurrentStackFrame().TopMutable( L, *rtStack );
        
        ConstValueAddress tableValueAddr = luaH_get(L, hvalue(t), stackTop);

        setobj2s(L, stackTop, tableValueAddr);
    }
    lua_unlock(L);
}

LUA_API void lua_rawgeti (lua_State *L, int idx, int n)
{
    lua_lock(L);

    ConstValueAddress o = index2constadr(L, idx);
    api_check(L, ttistable(o));

    ConstValueAddress tableValueAddr = luaH_getnum(L, hvalue(o), n);

    pushtvalue( L, tableValueAddr );

    lua_unlock(L);
}

LUA_API void lua_rawgetz (lua_State *L, int idx, size_t n)
{
    lua_lock(L);

    ConstValueAddress o = index2constadr(L, idx);
    api_check(L, ttistable(o));

    ConstValueAddress tableValueAddr = luaH_getznum(L, hvalue(o), n);

    pushtvalue( L, tableValueAddr );

    lua_unlock(L);
}

LUA_API void lua_createtable (lua_State *L, size_t narray, size_t nrec)
{
    lua_lock(L);
    luaC_checkGC(L);

    // We need to safely create a table and push it onto the stack.
    Table *newTable = luaH_new(L, narray, nrec);

    try
    {
        pushhvalue(L, newTable);
    }
    catch( ... )
    {
        // Stack exceptions are possible, out of memory or too big stack, for example.
        newTable->DereferenceGC( L );
        throw;
    }

    newTable->DereferenceGC( L );

    lua_unlock(L);
}


LUA_API int lua_getmetatable (lua_State *L, int objindex)
{
    Table *mt = nullptr;
    int res;
    lua_lock(L);

    {
        ConstValueAddress t = index2constadr( L, objindex );

        mt = luaT_getmetabyobj( L, t );
    }

    if ( mt == nullptr )
        res = 0;
    else
    {
        pushhvalue(L, mt);
        res = 1;
    }
    lua_unlock(L);
    return res;
}


LUA_API void lua_getfenv (lua_State *L, int idx)
{
    lua_lock(L);

    ConstValueAddress o = index2constadr(L, idx);
    api_checkvalidindex(L, o);

    switch( ttype(o) )
    {
    case LUA_TFUNCTION:
        {
            Closure *cl = clvalue( o );

            // Lua may not retrieve the environment of locked closures
            if ( cl->IsEnvLocked() )
            {
                pushnilvalue( L );
            }
            else
                pushgcvalue(L, cl->env);
        }
        break;
    case LUA_TUSERDATA:
        {
            GCObject *udEnv = uvalue(o)->env;

            if ( udEnv )
            {
                pushgcvalue( L, udEnv );
            }
            else
            {
                pushnilvalue( L );
            }
        }
        break;
    case LUA_TTHREAD:
        pushgcvalue(L, gcvalue(gt(thvalue(o))));
        break;
    case LUA_TCLASS:
        pushgcvalue(L, jvalue(o)->env);
        break;
    default:
        pushnilvalue(L);
        break;
    }
    lua_unlock(L);
}


LUA_API ILuaClass* lua_getmethodclass( lua_State *L )
{
    return ((CClosureMethodBase*)curr_func( L ))->m_class;
}

LUA_API void* lua_getmethodtrans( lua_State *L )
{
    return ((CClosureMethodTrans*)curr_func( L ))->data;
}

LUA_API void lua_pushmethodsuper( lua_State *L )
{
    lua_lock( L );

    CClosureMethodBase *method = (CClosureMethodBase*)curr_func( L );

    if ( method->super )
    {
        pushclvalue( L, method->super );
    }
    else
    {
        pushnilvalue( L );
    }

    lua_unlock( L );
}

LUA_API void lua_getclass( lua_State *L )
{
    lua_lock( L );

    RtCtxItem classIdCtx = index2stackadr( L, -1 );

    StkId classId = classIdCtx.Pointer();

    lua_assert( iscollectable( classId ) );

    Class *j = gcvalue( classId )->GetClass();

    if ( j )
    {
        setjvalue( L, classId, j );
    }
    else
    {
        setnilvalue( classId );
    }

    lua_unlock( L );
}
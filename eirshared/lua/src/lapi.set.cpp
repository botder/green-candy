#include "luacore.h"

#include "lapi.h"
#include "lfunc.h"
#include "lgc.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lclass.h"
#include "ltm.h"
#include "lvm.h"

#include "lapi.hxx"

#include "ltable.hxx"


/*
** set functions (stack -> Lua)
*/


LUA_API void lua_settable (lua_State *L, int idx)
{
    lua_lock(L);
    api_checknelems(L, 2);
    ConstValueAddress t = index2constadr(L, idx);
    api_checkvalidindex(L, t);
    {
        ConstValueAddress keyItem = index2constadr( L, -2 );
        ConstValueAddress valItem = index2constadr( L, -1 );

        luaV_settable(L, t, keyItem, valItem);
        popstack( L, 2 );  /* pop index and value */
    }
    lua_unlock(L);
}

LUA_API void lua_setfieldl( lua_State *L, int idx, const char *k, size_t l )
{
    lua_lock(L);
    api_checknelems(L, 1);
    ConstValueAddress t = index2constadr(L, idx);
    api_checkvalidindex(L, t);
    {
        TString *newKeyString = luaS_newlstr(L, k, l);

        // Important to catch exceptions because this is a fairly complicated operation!
        try
        {
            LocalValueAddress key;
            setsvalue(L, key, newKeyString);

            ConstValueAddress valItem = index2constadr( L, -1 );

            luaV_settable(L, t, key.ConstCast(), valItem);
            popstack( L, 1 );  /* pop value */
        }
        catch( ... )
        {
            newKeyString->DereferenceGC( L );
            throw;
        }

        // We do not need the key string anymore.
        newKeyString->DereferenceGC( L );
    }
    lua_unlock(L);
}

LUA_API void lua_setfield (lua_State *L, int idx, const char *k)
{
    lua_setfieldl( L, idx, k, strlen(k) );
}

LUA_API void lua_rawset (lua_State *L, int idx)
{
    lua_lock(L);
    api_checknelems(L, 2);
    ConstValueAddress t = index2constadr(L, idx);
    api_check(L, ttistable(t));
    {
        Table *setToTable = hvalue( t );

        RtCtxItem keyItem = index2stackadr( L, -2 );
        RtCtxItem valItem = index2stackadr( L, -1 );

        const TValue *keyItemPointer = keyItem.Pointer();
        const TValue *valItemPointer = valItem.Pointer();

        lua_assert( keyItemPointer != nullptr );
        lua_assert( valItemPointer != nullptr );

        {
            ValueAddress tableItemPtr = luaH_set( L, setToTable, keyItemPointer );

            setobj2t(L, tableItemPtr, valItemPointer);
        }
        popstack( L, 2 );
    }
    lua_unlock(L);
}

LUA_API void lua_rawseti (lua_State *L, int idx, int n)
{
    lua_lock(L);
    api_checknelems(L, 1);
    ConstValueAddress o = index2constadr(L, idx);
    api_check(L, ttistable(o));
    {
        Table *setToTable = hvalue( o );

        RtCtxItem valItem = index2stackadr( L, -1 );

        const TValue *valItemPointer = valItem.Pointer();

        lua_assert( valItemPointer != nullptr );

        ValueAddress numObj = luaH_setnum(L, setToTable, n);

        setobj2t(L, numObj, valItemPointer);
    }
    popstack( L, 1 );
    lua_unlock(L);
}

LUA_API void lua_rawsetz (lua_State *L, int idx, size_t n)
{
    lua_lock(L);
    api_checknelems(L, 1);
    ConstValueAddress o = index2constadr(L, idx);
    api_check(L, ttistable(o));
    {
        Table *setToTable = hvalue( o );

        RtCtxItem valItem = index2stackadr( L, -1 );

        const TValue *valItemPointer = valItem.Pointer();

        lua_assert( valItemPointer != nullptr );

        ValueAddress numObj = luaH_setznum(L, setToTable, n);

        setobj2t(L, numObj, valItemPointer);
    }
    popstack( L, 1 );
    lua_unlock(L);
}

LUA_API int lua_setmetatable (lua_State *L, int objindex)
{
    Table *mt;
    lua_lock(L);
    api_checknelems(L, 1);
    ConstValueAddress obj = index2constadr(L, objindex);
    api_checkvalidindex(L, obj);
    {
        RtStackView& currentStackFrame = L->GetCurrentStackFrame();

        L->rtStack.Lock( L );

        StkId_const stackTop = currentStackFrame.Top( L, L->rtStack );

        if ( ttisnil( stackTop ) )
            mt = nullptr;
        else
        {
            api_check(L, ttistable(stackTop));
            mt = hvalue(stackTop);
        }

        popstack( L, 1 );

        L->rtStack.Unlock( L );
    }

    switch (ttype(obj))
    {
    case LUA_TTABLE:
        {
            Table *h = hvalue(obj);

            h->metatable = mt;
            if ( mt )
            {
                luaC_objbarriert(L, h, mt);

                tableNativeImplementation *nativeMetaTable = tableNativeImplementation::GetNativeImplementation( mt );

                if ( nativeMetaTable )
                {
                    nativeMetaTable->flags = 0; // reset the metatable flags
                }
            }
        }
        break;
    case LUA_TUSERDATA:
        {
            Udata *u = uvalue(obj );

            u->metatable = mt;
            if ( mt )
            {
                luaC_objbarrier(L, u, mt);
            }
        }
        break;
    default:
        {
            L->mt[ttype(obj)] = mt;
        }
        break;
    }

    lua_unlock(L);
    return 1;
}

LUA_API int lua_setfenv (lua_State *L, int idx)
{
    int res = 1;
    lua_lock(L);
    api_checknelems(L, 1);
    ConstValueAddress o = index2constadr(L, idx);
    api_checkvalidindex(L, o);
    {
        rtStack_t& rtStack = L->rtStack;

        RtStackView& currentStackFrame = L->GetCurrentStackFrame();

        rtStack.Lock( L );

        StkId_const tableSlot = currentStackFrame.Top( L, rtStack );

        api_check(L, iscollectable(tableSlot));

        switch( ttype(o) )
        {
        case LUA_TFUNCTION:
            {
                Closure *cl = clvalue(o);

                if ( !cl->IsEnvLocked() )
                {
                    cl->env = gcvalue(tableSlot);
                }
                else
                {
                    res = 0;
                }
            }
            break;
        case LUA_TUSERDATA:
            uvalue(o)->env = gcvalue(tableSlot);
            break;
        case LUA_TTHREAD:
            setgcvalue(L, gt(thvalue(o)), gcvalue(tableSlot));
            break;
        default:
            res = 0;
            break;
        }

        if ( res == 1 )
        {
            luaC_objbarrier(L, gcvalue(o), gcvalue(tableSlot));
        }

        rtStack.Unlock( L );

        popstack( L, 1 );
    }
    lua_unlock(L);
    return res;
}
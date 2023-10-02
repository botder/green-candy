#include "luacore.h"

#include "lapi.h"
#include "ldebug.h"
#include "lgc.h"

#include "lapi.hxx"


/*
** basic stack manipulation
*/

LUA_API int lua_gettop (lua_State *L)
{
    return (int)rt_stackcount( L );
}

LUA_API void lua_settop( lua_State *L, int idx )
{
    lua_lock(L);

    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    if ( idx != 0 )
    {
        stackOffset_t stackOffset = index2stackindex( L, idx );

        stackOffset_t absoluteStackOffset = currentStackFrame.GetAbsoluteStackOffset( L, L->rtStack, stackOffset );

        currentStackFrame.SetTopOffset( L, L->rtStack, absoluteStackOffset );
    }
    else
    {
        currentStackFrame.Clear( L, L->rtStack );
    }

    lua_unlock(L);
}

LUA_API void lua_remove (lua_State *L, int idx)
{
    lua_lock(L);

    RtCtxItem itemCtx = index2stackadr( L, idx );

    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    stackOffset_t topOffset = currentStackFrame.GetStackOffset( L, L->rtStack, itemCtx.Pointer() );

    currentStackFrame.RemoveItem( L, L->rtStack, topOffset );

    lua_unlock(L);
}

LUA_API void lua_insert (lua_State *L, int idx)
{
    lua_lock(L);

    RtCtxItem itemCtx = index2stackadr( L, idx );

    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    stackOffset_t topOffset = currentStackFrame.GetStackOffset( L, L->rtStack, itemCtx.Pointer() );

    currentStackFrame.TopInsertItem( L, L->rtStack, topOffset );

    lua_unlock(L);
}

LUA_API void lua_swap( lua_State *L, int idx )
{
	lua_lock( L );

    std::list <std::string> stack_prev = luaG_stackdump(L);

    RtCtxItem itemCtx = index2stackadr( L, idx );

    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    stackOffset_t topOffset = currentStackFrame.GetStackOffset( L, L->rtStack, itemCtx.Pointer() );

    currentStackFrame.TopSwapItem( L, L->rtStack, topOffset );

    std::list <std::string> stack_after = luaG_stackdump(L);

	lua_unlock( L );
}

LUA_API void lua_replace (lua_State *L, int idx)
{
    lua_lock(L);

    /* explicit test for incompatible code */
    if ( idx == LUA_ENVIRONINDEX && !hascallingenv( L ) )
    {
        luaG_runerror(L, "no calling environment");
    }

    api_checknelems(L, 1);
    {
        rtStack_t& rtStack = L->rtStack;

        rtStack.Lock( L );

        StkId stackTop = L->GetCurrentStackFrame().TopMutable( L, L->rtStack );

        if ( idx == LUA_ENVIRONINDEX )
        {
            Closure *func = curr_func(L);
            const TValue *val = stackTop;

            func->env = gcvalue(val);
            luaC_barrier(L, func, val);
        }
        else if ( idx == LUA_STORAGEINDEX )
        {
            const TValue *val = stackTop;

            setgcvalue( L, &L->storage, gcvalue( val ) );
            luaC_barrier( L, L, val );
        }
        else
        {
            ValueAddress o = index2adr(L, idx);
            api_checkvalidindex(L, o);

            setobj( L, o, stackTop );

            if ( idx < LUA_GLOBALSINDEX )  /* function upvalue? */
            {
                luaC_barrier(L, curr_func(L), stackTop);
            }
        }

        popstack( L, 1 );

        rtStack.Unlock( L );
    }
    lua_unlock(L);
}

LUA_API void lua_pushvalue (lua_State *L, int idx)
{
    lua_lock(L);
    ConstValueAddress o = index2constadr( L, idx );
    pushtvalue( L, o );
    lua_unlock(L);
}

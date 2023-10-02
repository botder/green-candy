#include "luacore.h"

#include "lapi.h"
#include "lfunc.h"
#include "lgc.h"
#include "lstate.h"
#include "lundump.h"
#include "lvm.h"

#include "lapi.hxx"


/*
** `load' and `call' functions (run Lua code)
*/

FASTAPI void adjustresults( lua_State *L, int nres )
{
    if ( nres == LUA_MULTRET )
    {
        //__noop(); //L->ciStack.Top()->top = L->rtStack.Top();
    }
}

FASTAPI void checkresults( lua_State *L, int na, int nr )
{
    api_check( L,
        nr == LUA_MULTRET ||
        true
    );
}


LUA_API void lua_call (lua_State *L, int nargs, int nresults)
{
    lua_lock(L);
    api_checknelems(L, nargs+1);
    checkresults(L, nargs, nresults);
    RtCtxItem func = index2stackadr( L, -(nargs+1) );
    stackOffset_t topOffset = L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack );
    luaD_call(L, func, nresults, CALLFRAME_DYNAMIC, topOffset);
    adjustresults(L, nresults);
    lua_unlock(L);
}



/*
** Execute a protected call.
*/
struct CallS
{  /* data to `f_call' */
    RtCtxItem func;
    int nresults;
};


static void f_call (lua_State *L, void *ud)
{
    struct CallS *c = cast(struct CallS *, ud);
    stackOffset_t topOffset = L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack );
    luaD_call(L, c->func, c->nresults, CALLFRAME_DYNAMIC, topOffset);
}

LUA_API int lua_pcallex (lua_State *L, int nargs, int nresults, int errfunc, lua_Debug *debug)
{
    struct CallS c;
    int status;
    stackOffset_t func = 0;
    lua_lock(L);
    api_checknelems(L, nargs+1);
    checkresults(L, nargs, nresults);
    if ( errfunc != 0 )
    {
        ConstValueAddress o = index2constadr(L, errfunc);

        if ( o != nullptr )
        {
            api_checkvalidindex(L, o);
            func = savestack(L, o);
        }
    }
    c.func = index2stackadr( L, -(nargs+1) );  /* function to be called */
    c.nresults = nresults;
    status = luaD_pcall(L, f_call, &c, savestack(L, c.func.Pointer()), func, debug);
    adjustresults(L, nresults);
    lua_unlock(L);
    return status;
}


LUA_API int lua_pcall (lua_State *L, int nargs, int nresults, int errfunc)
{
    return lua_pcallex( L, nargs, nresults, errfunc, nullptr );
}


/*
** Execute a protected C call.
*/
struct CCallS {  /* data to `f_Ccall' */
  lua_CFunction func;
  void *ud;
};

static void f_Ccall (lua_State *L, void *ud)
{
    struct CCallS *c = cast(struct CCallS *, ud);
    CClosure *cl = luaF_newCclosure(L, 0, getcurrenv(L));

    try
    {
        cl->f = c->func;
        pushclvalue(L, cl);  /* push function */
        pushpvalue(L, c->ud);  /* push only argument */
    }
    catch( ... )
    {
        // We must account for stack failures.
        cl->DereferenceGC( L );
        throw;
    }

    // Since the function is on stack now, it can be dereferenced from GC keep-alive.
    cl->DereferenceGC( L );

    RtCtxItem funcCtx = index2stackadr( L, -2 );

    stackOffset_t topOffset = L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack );

    luaD_call(L, funcCtx, 0, CALLFRAME_DYNAMIC, topOffset);
}


LUA_API int lua_cpcall (lua_State *L, lua_CFunction func, void *ud)
{
    struct CCallS c;
    int status;
    lua_lock(L);
    c.func = func;
    c.ud = ud;
    status = luaD_pcall(L, f_Ccall, &c, savestack(L, index2constadr( L, -1 )), 0, nullptr);
    lua_unlock(L);
    return status;
}


LUA_API int lua_load (lua_State *L, lua_Reader reader, void *data, const char *chunkname)
{
    ZIO z;
    lua_lock(L);

    if (!chunkname)
    {
        chunkname = "?";
    }

    luaZ_init(L, &z, reader, data);

    int status = luaD_protectedparser(L, &z, chunkname);

    lua_unlock(L);
    return status;
}


LUA_API int lua_dump (lua_State *L, lua_Writer writer, void *data)
{
    int status;
    lua_lock(L);

    api_checknelems(L, 1);
    RtCtxItem oCtx = index2stackadr( L, -1 );

    StkId o = oCtx.Pointer();

    if (isLfunction(o))
        status = luaU_dump(L, clvalue(o)->GetLClosure()->p, writer, data, 0);
    else
        status = 1;

    lua_unlock(L);
    return status;
}


LUA_API int  lua_status (lua_State *L)
{
	if ( !L->IsThread() )
		return 0;

	return ((lua_Thread*)L)->status;
}

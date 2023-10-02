#include "luacore.h"

#include "lapi.h"
#include "lgc.h"
#include "lvm.h"

#include "lapi.hxx"

// Include garbage collector internals.
#include "lgc.internal.hxx"


/*
** Garbage-collection function
*/

LUA_API int lua_gc (lua_State *L, int what, int data)
{
    int res = 0;
    global_State *g;

    lua_lock(L);

    g = G(L);

    // Only perform if we can get the global GC env.
    globalStateGCEnv *gcEnv = GetGlobalGCEnvironment( g );

    if ( gcEnv )
    {
        switch (what) {
        case LUA_GCSTOP: {
            gcEnv->GCthreshold = MAX_LUMEM;
            break;
        }
        case LUA_GCRESTART: {
            gcEnv->GCthreshold = g->totalbytes;
            break;
        }
        case LUA_GCCOLLECT: {
            luaC_fullgc(L);
            break;
        }
        case LUA_GCCOUNT: {
            /* GC values are expressed in Kbytes: #bytes/2^10 */
            res = cast_int(g->totalbytes >> 10);
            break;
        }
        case LUA_GCCOUNTB: {
            res = cast_int(g->totalbytes & 0x3ff);
            break;
        }
        case LUA_GCSTEP: {
            lu_mem a = (cast(lu_mem, data) << 10);

            if (a <= g->totalbytes)
                gcEnv->GCthreshold = g->totalbytes - a;
            else
                gcEnv->GCthreshold = 0;

            while (gcEnv->GCthreshold <= g->totalbytes)
            {
                luaC_step(L);

                if (gcEnv->gcstate == GCSpause)
                {  /* end of cycle? */
                    res = 1;  /* signal it */
                    break;
                }
            }
            break;
        }
        case LUA_GCSETPAUSE: {
            res = gcEnv->gcpause;
            gcEnv->gcpause = data;
            break;
        }
        case LUA_GCSETSTEPMUL: {
            res = gcEnv->gcstepmul;
            gcEnv->gcstepmul = data;
            break;
        }
        default: res = -1;  /* invalid option */
        }
    }

    lua_unlock(L);

    return res;
}

LUA_API void lua_gcpaycost( lua_State *L, unsigned int cost )
{
    global_State *g = G(L);

    // Get the global gc environment.
    globalStateGCEnv *gcEnv = GetGlobalGCEnvironment( g );

    if ( gcEnv )
    {
        lua_Thread *gcL = gcEnv->GCthread;
        
        if ( gcL != L || gcL->status != THREAD_RUNNING )
            return;

        luaC_paycost( g, gcL, cost );
    }
}
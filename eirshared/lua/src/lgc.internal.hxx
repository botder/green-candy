// Lua GC internal definitions.
#ifndef _LUA_GC_INTERNALS_
#define _LUA_GC_INTERNALS_

#include "lobject.h"

#include "lpluginutil.hxx"

struct globalStateGCEnv
{
    lu_byte gcstate;  /* state of garbage collector */
    int sweepstrgc;  /* position of sweep in `strt' */
    gcObjList_t rootgc;  /* list of all collectable objects */
    gcObjList_t::removable_iterator sweepgc;  /* position of sweep in `rootgc' */
    grayObjList_t gray;  /* list of gray objects */
    grayObjList_t grayagain;  /* list of objects to be traversed atomically */
    grayObjList_t weak;  /* list of weak tables (to be cleared) */
    gcObjList_t tmudata;  /* elements waiting to call their TM_GC methods during GC */
    lua_Thread *GCthread; /* garbage collector runtime */
    lu_mem GCthreshold;
    lu_mem GCcollect; /* amount of memory to be collected until stop */
    lu_mem estimate;  /* an estimate of number of bytes actually in use */
    lu_mem gcdept;  /* how much GC is `behind schedule' */
    int gcpause;  /* size of pause between successive GCs */
    int gcstepmul;  /* GC `granularity' */
};

typedef PluginConnectingBridge
    <globalStateGCEnv,
        globalStateStructFactoryMeta <globalStateGCEnv, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        gcEnvConnectingBridge_t;

extern gcEnvConnectingBridge_t gcEnvConnectingBridge;

inline globalStateGCEnv* GetGlobalGCEnvironment( global_State *g )
{
    return gcEnvConnectingBridge.GetPluginStruct( g->config, g );
}

#endif //_LUA_GC_INTERNALS_
#include "luacore.h"

#include "lauxlib.h"
#include "lualib.h"
#include "lrtlib.h"
#include "ltypelib.h"

#include "lruntime.hxx"

static int luaRT_create( lua_State *L )
{
    return 0;
}

static int luaRT_lockenter( lua_State *L )
{
    global_State *g = G(L);

    runtimeTypeInfoPlugin_t *rtInfo = runtimeTypeInfoPluginRegister.GetPluginStruct( g->config );

    if ( rtInfo )
    {
        //ReadWriteLock *theLock = (ReadWriteLock*)luaTL_checktype( L, 1, rtInfo->
    }

    return 0;
}

static int luaRT_lockleave( lua_State *L )
{
    return 0;
}

static int luaRT_locksharedenter( lua_State *L )
{
    return 0;
}

static int luaRT_locksharedleave( lua_State *L )
{
    return 0;
}

static const luaL_Reg runtime_library[] =
{
    { "create", luaRT_create },
    { "lockenter", luaRT_lockenter },
    { "lockleave", luaRT_lockleave },
    { "locksharedenter", luaRT_locksharedenter },
    { "locksharedleave", luaRT_locksharedleave },
    { nullptr, nullptr }
};

LUAI_FUNC int luaopen_runtime( lua_State *L )
{
    luaL_register( L, LUA_RUNTIMELIBNAME, runtime_library );
    return 1;
}
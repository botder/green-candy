#ifndef _LUA_MULTITHREADING_ENVIRONMENT_
#define _LUA_MULTITHREADING_ENVIRONMENT_

#include "lpluginutil.hxx"

#include <NativeExecutive/CExecutiveManager.h>

struct globalStateMultithreadingPlugin
{
    inline void Initialize( global_State *g )
    {
        this->nativeMan = NativeExecutive::CExecutiveManager::Create();
    }

    inline void Shutdown( global_State *g )
    {
        if ( NativeExecutive::CExecutiveManager *manager = this->nativeMan )
        {
            NativeExecutive::CExecutiveManager::Delete( manager );

            this->nativeMan = nullptr;
        }
    }

    NativeExecutive::CExecutiveManager *nativeMan;
};

typedef PluginConnectingBridge
    <globalStateMultithreadingPlugin,
        globalStateDependantStructFactoryMeta <globalStateMultithreadingPlugin, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        mtEnvConnectingBridge_t;

extern mtEnvConnectingBridge_t mtEnvConnectingBridge;

inline globalStateMultithreadingPlugin* GetGlobalStateMultithreadingEnv( global_State *g )
{
    return mtEnvConnectingBridge.GetPluginStruct( g->config, g );
}

inline NativeExecutive::CExecutiveManager* GetGlobalStateMultithreadingManager( global_State *g )
{
    globalStateMultithreadingPlugin *mtPlugin = GetGlobalStateMultithreadingEnv( g );

    if ( mtPlugin )
    {
        return mtPlugin->nativeMan;
    }

    return nullptr;
}

#endif //_LUA_MULTITHREADING_ENVIRONMENT_
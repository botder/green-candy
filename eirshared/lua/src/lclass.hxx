// Lua class shared include file
#ifndef _LUA_CLASS_SHARED_INCLUDE_
#define _LUA_CLASS_SHARED_INCLUDE_

#include "lapi.h"
#include "lgc.h"
#include "lmem.h"
#include "lstate.h"
#include "ltable.h"
#include "lclass.h"
#include "lauxlib.h"
#include "lstring.h"
#include "lfunc.h"
#include "lvm.h"
#include "ldebug.h"

#include "lpluginutil.hxx"

struct namespaceClassTypeInfo
{
    inline void Initialize( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Create all type information.
        this->classTypeInfo = typeSys.RegisterStructType <Class> ( "class", cfg->gcobjTypeInfo );

        // Set up properties.
        typeSys.SetTypeInfoExclusive( this->classTypeInfo, true );
    }

    inline void Shutdown( lua_config *cfg )
    {
        if ( LuaTypeSystem::typeInfoBase *typeInfo = this->classTypeInfo )
        {
            cfg->typeSys.DeleteType( typeInfo );
        }
    }

    // Types used by the class system.
    LuaTypeSystem::typeInfoBase *classTypeInfo;
};

typedef PluginDependantStructRegister <namespaceClassTypeInfo, namespaceFactory_t> classTypeInfoPlugin_t;

extern classTypeInfoPlugin_t classTypeInfoPlugin;

struct globalStateClassEnvPlugin
{
    inline globalStateClassEnvPlugin( void )
    {
        this->superCached = nullptr;
    }

    TString *superCached;   // 'super' string
};

typedef PluginConnectingBridge
    <globalStateClassEnvPlugin,
        globalStateStructFactoryMeta <globalStateClassEnvPlugin, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        classEnvConnectingBridge_t;

extern classEnvConnectingBridge_t classEnvConnectingBridge;

inline globalStateClassEnvPlugin* GetGlobalClassEnv( global_State *g )
{
    return classEnvConnectingBridge.GetPluginStruct( g->config, g );
}

inline TString* GetSuperString( lua_State *L )
{
    global_State *g = G(L);

    TString *theString = nullptr;
    
    globalStateClassEnvPlugin *classEnv = GetGlobalClassEnv( g );

    if ( classEnv )
    {
        theString = classEnv->superCached;
    }

    if ( !theString )
    {
        const char *_cStr = "super";
        size_t _cStrLen = strlen( _cStr );

        theString = luaS_newlstr( L, _cStr, _cStrLen );

        if ( classEnv )
        {
            classEnv->superCached = theString;
        }

        // Since we store the "super" string in the global class environment now, we can dereference it.
        theString->DereferenceGC( L );
    }

    return theString;
}

#endif //_LUA_CLASS_SHARED_INCLUDE_
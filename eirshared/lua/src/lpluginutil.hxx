// Plugin utilities to easy the developer's frustration.
// Reading this code is discouraged, since it may wreck them brainz!!!
#ifndef _LUA_PLUGIN_UTILITIES_
#define _LUA_PLUGIN_UTILITIES_

#include <sdk/PluginHelpers.h>

struct _globalStateStructFactoryMeta
{
    typedef globalStateFactory_t factoryType;

    inline factoryType& ResolveFactoryLink( lua_config& cfgStruct )
    {
        return cfgStruct.globalStateFactory;
    }

    template <typename structType>
    inline globalStatePluginOffset_t RegisterPlugin( lua_config *cfgStruct, unsigned int pluginId )
    {
        return cfgStruct->globalStateFactory.RegisterStructPlugin <structType> ( pluginId );
    }
};

template <typename structType, typename factoryType, typename hostType>
struct globalStateStructFactoryMeta : public factoryMetaDefault <hostType, factoryType, structType, _globalStateStructFactoryMeta>
{
};

struct _globalStateDependantStructFactoryMeta
{
    typedef globalStateFactory_t factoryType;

    inline factoryType& ResolveFactoryLink( lua_config& cfgStruct )
    {
        return cfgStruct.globalStateFactory;
    }

    template <typename structType>
    inline globalStatePluginOffset_t RegisterPlugin( lua_config *cfgStruct, unsigned int pluginId )
    {
        return cfgStruct->globalStateFactory.RegisterDependantStructPlugin <structType> ( pluginId );
    }
};

template <typename structType, typename factoryType, typename hostType>
struct globalStateDependantStructFactoryMeta : public factoryMetaDefault <hostType, factoryType, structType, _globalStateDependantStructFactoryMeta>
{
};

template <typename structType, typename factoryType, typename hostType, typename registerConditionalType>
struct globalStateStructConditionalFactoryMeta : public factoryMetaConditional <hostType, factoryType, structType, _globalStateStructFactoryMeta, registerConditionalType>
{
};

template <typename structType, typename factoryType, typename hostType, typename registerConditionalType>
struct globalStateDependantStructConditionalFactoryMeta : public factoryMetaConditional <hostType, factoryType, structType, _globalStateDependantStructFactoryMeta, registerConditionalType>
{
};

#endif //_LUA_PLUGIN_UTILITIES_

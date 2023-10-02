// Lua state low-level management.
#ifndef _LUA_STATE_LOW_LEVEL_
#define _LUA_STATE_LOW_LEVEL_

struct GlobalStateAllocPluginData
{
    inline GlobalStateAllocPluginData( void ) : _memAlloc( nullptr, nullptr )
    {
        this->constructor = nullptr;
    }

    inline void Init( lua_config *cfg, lua_Alloc allocCallback, void *userdata )
    {
        _memAlloc.allocCallback = allocCallback;
        _memAlloc.userdata = userdata;

        this->constructor = cfg->globalStateFactory.CreateConstructor( _memAlloc );
    }

    inline void Shutdown( lua_config *cfg )
    {
        if ( this->constructor )
        {
            cfg->globalStateFactory.DeleteConstructor( this->constructor );
        }
    }

    // Structure that is used for bootstrapping with anonymous destruction support.
    GeneralMemoryAllocator _memAlloc;
    globalStateFactory_t::DeferredConstructor <GeneralMemoryAllocator> *constructor;
};

#endif //_LUA_STATE_LOW_LEVEL_
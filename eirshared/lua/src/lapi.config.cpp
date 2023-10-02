// Lua configuration functions.
#include "luacore.h"

#include "lapi.h"
#include "lobject.h"
#include "lgc.h"
#include "lstring.h"
#include "ldispatch.h"
#include "ltable.h"
#include "lfunc.h"
#include "ltm.h"
#include "lclass.h"
#include "lvm.h"
#include "lapi.h"

#include "lapi.hxx"

#include "lstate.lowlevel.hxx"

// The namespace factory to create lua_config objects!
namespaceFactory_t namespaceFactory;

// New style entry point to the Lua5.1ex library.
// Here we initialize the library namespace.
static volatile unsigned long _libraryReferenceCount = 0;

// Special local plugins.
static namespacePluginOffset_t _allocDataPluginOffset = namespaceFactory_t::INVALID_PLUGIN_OFFSET;

inline GlobalStateAllocPluginData* GetNamespaceAllocData( lua_config *cfg )
{
    return namespaceFactory_t::RESOLVE_STRUCT <GlobalStateAllocPluginData> ( cfg, _allocDataPluginOffset );
}

// Library extensions.
extern void luaMT_libraryinit( void );
extern void luaC_libraryinit( void );
extern void luaF_libraryinit( void );
extern void luaS_libraryinit( void );
extern void luaJ_libraryinit( void );
extern void luaH_libraryinit( void );

extern void luaMT_libraryshutdown( void );
extern void luaC_libraryshutdown( void );
extern void luaF_libraryshutdown( void );
extern void luaS_libraryshutdown( void );
extern void luaJ_libraryshutdown( void );
extern void luaH_libraryshutdown( void );

static void _referenceLibrary( void )
{
    // Increase the global reference count.
    if ( _libraryReferenceCount++ == 0 )
    {
        // Here modules have to register themselves that need per-namespace data.
        // This is usually data about global_State factory plugins.
        // We call this process "booting".

        // First initialize local plugins.
        _allocDataPluginOffset =
            namespaceFactory.RegisterStructPlugin <GlobalStateAllocPluginData> ( namespaceFactory_t::ANONYMOUS_PLUGIN_ID );

        // Boot all modules.
        luaE_libraryinit();
        luaMT_libraryinit();
        luaC_libraryinit();
        luaF_libraryinit();
        luaS_libraryinit();
        luaJ_libraryinit();
        luaH_libraryinit();
    }
}

static void _dereferenceLibrary( void )
{
    // Decrease the global reference count.
    if ( --_libraryReferenceCount == 0 )
    {
        // Terminate module activity.
        luaH_libraryshutdown();
        luaJ_libraryshutdown();
        luaS_libraryshutdown();
        luaF_libraryshutdown();
        luaC_libraryshutdown();
        luaMT_libraryshutdown();
        luaE_libraryshutdown();

        // Delete local plugins.
        namespaceFactory.UnregisterPlugin( _allocDataPluginOffset );
    }
}

/*
** configuration functions to create specialized Lua VMs
*/
// Initialize the newly created module namespace (lua_config type).
void _initNamespace( lua_config *cfg )
{
    // register global modules here!
    luaM_init( cfg );
    luaC_moduleinit( cfg );
    luaO_init( cfg );
    luaS_init( cfg );
    luaQ_init( cfg );
    luaH_init( cfg );
    luaF_init( cfg );
    luaT_moduleinit( cfg );
    luaJ_init( cfg );
    luaV_init( cfg );
    luaapi_init( cfg );

    // todo: add more.
}

// Clear the namespace configuration again.
void _shutdownNamespace( lua_config *cfg )
{
    // Shutdown global modules.
    luaapi_shutdown( cfg );
    luaV_shutdown( cfg );
    luaJ_shutdown( cfg );
    luaT_moduleshutdown( cfg );
    luaF_shutdown( cfg );
    luaH_shutdown( cfg );
    luaQ_shutdown( cfg );
    luaS_shutdown( cfg );
    luaO_shutdown( cfg );
    luaC_moduleshutdown( cfg );
    luaM_shutdown( cfg );

    // todo: add more.
}

LUA_API lua_config* lua_newconfig( lua_Alloc allocFunc, void *ud )
{
    GeneralMemoryAllocator memAlloc( ud, allocFunc );

    // Create the allocator meta-data.
    // Create the memory allocator for bootstrapping.
    // Create the factory that can produce and destroy global states.
    {
        // Reference the library.
        _referenceLibrary();

        struct configConstructor
        {
            GeneralMemoryAllocator *allocMem;

            inline lua_config* Construct( void *mem ) const
            {
                lua_config *cfg = new (mem) lua_config;

                // Set type system allocation data.
                cfg->typeSys._memAlloc = allocMem;

                // Register root types.
                {
                    LuaTypeSystem& typeSys = cfg->typeSys;

                    cfg->gcobjTypeInfo = typeSys.RegisterAbstractType <GCObject> ( "gcobj" );
                    cfg->grayobjTypeInfo = typeSys.RegisterAbstractType <GrayObject> ( "grayobj", cfg->gcobjTypeInfo );
                }

                return cfg;
            }
        };

        configConstructor constructor;
        constructor.allocMem = &memAlloc;

        lua_config *cfgStruct = namespaceFactory.ConstructTemplate( memAlloc, constructor );

        if ( cfgStruct )
        {
            // Get the pointer to allocation data.
            GlobalStateAllocPluginData *allocData = GetNamespaceAllocData( cfgStruct );

            if ( allocData )
            {
                // Specialize the allocation data.
                allocData->Init( cfgStruct, allocFunc, ud );

                // Set new allocation data to the type system.
                cfgStruct->typeSys._memAlloc = &allocData->_memAlloc;

                // Default the values.
                cfgStruct->isMultithreaded = false;

                // Store the allocation data into the configuration.
                // It will be used to request memory from the application.
                cfgStruct->allocData = allocData;

                // Initialize the configuration.
                _initNamespace( cfgStruct );

                return cfgStruct;
            }

            // We failed to get allocation data, so no point in continuing.
            namespaceFactory.Destroy( memAlloc, cfgStruct );
        }

        // We failed, so dereference the library again.
        _dereferenceLibrary();
    }

    return nullptr;
}

LUA_API lua_Alloc lua_getallocf (lua_State *L, void **ud)
{
    lua_Alloc f;

    lua_lock(L);
    {
        global_State *g = G(L);

        lua_config *cfg = g->config;

        GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)cfg->allocData;

        if ( ud )
        {
            *ud = allocData->_memAlloc.userdata;
        }

        f = allocData->_memAlloc.allocCallback;
    }
    lua_unlock(L);

    return f;
}

LUA_API void lua_setallocf (lua_State *L, lua_Alloc f, void *ud)
{
    lua_lock(L);
    {
        global_State *g = G(L);

        lua_config *cfg = g->config;

        GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)cfg->allocData;

        allocData->_memAlloc.userdata = ud;
        allocData->_memAlloc.allocCallback = f;
    }
    lua_unlock(L);
}

inline bool _is_string_equal( const char *left, const char *right )
{
    return ( strcmp( left, right ) == 0 );
}

template <typename cfgDispatcher>
static inline bool DispatchConfigItem( lua_config *cfg, const char *cfgName, lua_cfgValue& value, cfgDispatcher dispatch )
{
    bool success = true;

    // Only allow setting and getting certain items if the system is capable.
    bool systemSupportsMultithreading = false;  // by default it does not.

#ifdef _WIN32
    // Win32 does support it.
    systemSupportsMultithreading = true;
#endif //_WIN32

    if ( systemSupportsMultithreading && _is_string_equal( cfgName, "mt" ) )
    {
        dispatch.Multithreading( cfg, value );
    }
    else
    {
        success = false;
    }

    return success;
}

struct SetConfigItemDispatch
{
    inline void Multithreading( lua_config *cfg, lua_cfgValue value )
    {
        cfg->isMultithreaded = ( value != LUA_FALSE );
    }
};

// I wish that future C++ standards make creating such functions much easier through templates.
LUA_API int lua_setconfig( lua_config *cfg, const char *cfgName, lua_cfgValue valueIn )
{
    lua_assert( cfg != nullptr );

    SetConfigItemDispatch dispatch;

    bool success = DispatchConfigItem( cfg, cfgName, valueIn, dispatch );

    return ( success ) ? LUA_TRUE : LUA_FALSE;
}

struct GetConfigItemDispatch
{
    inline void Multithreading( lua_config *cfg, lua_cfgValue& value )
    {
        value = ( cfg->isMultithreaded ? LUA_TRUE : LUA_FALSE );
    }
};

// Because things just tend to reoccur and its hard to maintain.
LUA_API int lua_getconfig( lua_config *cfg, const char *cfgName, lua_cfgValue *valueOut )
{
    lua_assert( cfg != nullptr );
    lua_assert( valueOut != nullptr );

    GetConfigItemDispatch dispatch;

    bool success = DispatchConfigItem( cfg, cfgName, *valueOut, dispatch );

    return ( success ) ? LUA_TRUE : LUA_FALSE;
}

LUA_API void lua_freeconfig( lua_config *cfg )
{
    lua_assert( cfg != nullptr );

    // Release namespace resources.
    _shutdownNamespace( cfg );

    // Grab the allocation data interface.
    GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)cfg->allocData;

    // Get the memory allocator onto the stack.
    GeneralMemoryAllocator allocMem = allocData->_memAlloc;

    // Shutdown the allocation data.
    allocData->Shutdown( cfg );

    // Free the configuration itself.
    namespaceFactory.Destroy( allocMem, cfg );

    // Dereference the library.
    _dereferenceLibrary();
}

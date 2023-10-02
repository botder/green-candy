/*
** $Id: lstate.c,v 2.36.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lclass.h"
#include "lvm.h"
#include "lapi.h"

#include "lpluginutil.hxx"

#include "lmultithreading.hxx"

// Main state of the whole Lua VM.
struct lua_MainState : public lua_State
{
    inline lua_MainState( global_State *g, void *construction_params ) : lua_State( g )
    {
        return;
    }

    inline void Initialize( global_State *g )
    {
        // Initialize the main thread.

        // Currentwhite is the flag used by the propagation runtime to determine alive objects.
        // We start the runtime with the "WHITE0BIT" basically the starting bit out of the two.
        // Any object that has the "fixed" bit is not collected either.
        g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);

        luaC_register( g, this, LUA_TTHREAD );

        // This thread is never collected as it has a ref-count.

        for ( unsigned int i = 0; i < NUM_TAGS; i++ )
        {
            this->mt[i] = nullptr;
        }

        g->mainthread = this;
    }

    inline void Shutdown( global_State *g )
    {
        g->mainthread = nullptr;
    }
};

// Type information holding plugin.
static namespacePluginOffset_t _globalStateMetaInfoPluginOffset = namespaceFactory_t::INVALID_PLUGIN_OFFSET;

struct globalStateMainStateFactoryMeta
{
    typedef globalStateFactory_t factoryType;

    lua_config *managerStruct;

    // Type information.
    LuaTypeSystem::typeInfoBase *luaStateTypeInfo;
    LuaTypeSystem::typeInfoBase *luaThreadTypeInfo;

    unsigned int refCount;

    // Required field.
    globalStatePluginOffset_t endingPointPluginOffset;

    inline void Initialize( lua_config *cfgStruct )
    {
        LuaTypeSystem& typeSys = cfgStruct->typeSys;

        luaStateTypeInfo = typeSys.RegisterStructType <lua_MainState> ( "main_state", cfgStruct->grayobjTypeInfo );
        luaThreadTypeInfo = typeSys.RegisterStructType <lua_Thread> ( "coroutine", cfgStruct->grayobjTypeInfo );

        // Set inheritance properties.
        typeSys.SetTypeInfoExclusive( luaStateTypeInfo, true );
        typeSys.SetTypeInfoExclusive( luaThreadTypeInfo, true );

        refCount = 0;
        endingPointPluginOffset = globalStateFactory_t::INVALID_PLUGIN_OFFSET;

        managerStruct = cfgStruct;
    }

    inline void Shutdown( lua_config *cfgStruct )
    {
        if ( this->luaThreadTypeInfo != nullptr )
        {
            cfgStruct->typeSys.DeleteType( this->luaThreadTypeInfo );
        }

        if ( this->luaStateTypeInfo != nullptr )
        {
            cfgStruct->typeSys.DeleteType( this->luaStateTypeInfo );
        }
    }

    inline void Reference( void )
    {
        if ( this->refCount++ == 0 )
        {
            // Register the plugin to the global_State!
            this->endingPointPluginOffset = this->managerStruct->typeSys.StaticPluginRegistryRegisterTypeConstruction <lua_MainState> (
                this->managerStruct->globalStateFactory,
                this->luaStateTypeInfo, nullptr
            );
        }
    }

    inline void Dereference( void )
    {
        if ( --this->refCount == 0 )
        {
            // Delete the plugin again if present.
            if ( globalStateFactory_t::IsOffsetValid( this->endingPointPluginOffset ) )
            {
                this->managerStruct->globalStateFactory.UnregisterPlugin( this->endingPointPluginOffset );
            }
        }
    }
};

static PluginConnectingBridge <LuaRTTI, globalStateMainStateFactoryMeta, namespaceFactory_t> mainLuaStateConnectingBridge;


static void stack_init (lua_State *L1, lua_State *L)
{
    RtStackAddr rtStack = L1->rtStack.LockedAcquisition( L );
    CiStackAddr ciStack = L1->ciStack.LockedAcquisition( L );

    // Link the stacks to the runtime thread.
    L1->rtStack.runtimeThread = L1;

    global_State *g = G(L);

    /* initialize CallInfo array */
    ciStack->Initialize( g, L, BASIC_CI_SIZE );
    CallInfo *mainFrame = ciStack->ObtainItem( L );     // there must be an item on the ci stack.
    /* initialize stack array */
    rtStack->Initialize( g, L, BASIC_STACK_SIZE );
    StkId firstStackValue = rtStack->ObtainItem( L );    // there must be an item on the rt stack.
    /* initialize first ci */
    mainFrame->func = firstStackValue;
    setnilvalue( firstStackValue );  /* `function' entry for this `ci' */
    mainFrame->stack.Initialize( firstStackValue, firstStackValue );
}


static void freestack (lua_State *L, lua_State *L1)
{
    L1->ciStack.Shutdown( G(L), L );
    L1->rtStack.Shutdown( G(L), L );
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud)
{
    global_State *g = G(L);

    UNUSED(ud);

    stack_init(L, L);  /* init stack */
    luaS_resize(L, MINSTRTABSIZE);  /* initial size of string table */
    luaT_init(L);
    luaX_init(L);

    // We want to especially make the memory error message string cached.
    {
        TString *memErrString = luaS_newliteral(L, MEMERRMSG);

        luaS_fix( memErrString );

        memErrString->DereferenceGC( L );
    }

    luaC_setthreshold( L, 4*g->totalbytes );
}


static inline void preinit_state (lua_State *L, global_State *g)
{
    L->hook = nullptr;
    L->hookmask = 0;
    L->basehookcount = 0;
    L->allowhook = true;
    resethookcount(L);
    L->openupval.Clear();
    L->nCcalls = 0;
    L->savedpc = nullptr;
    L->errfunc = 0;

    Table *newStorageTable = luaH_new( L, 0, 0 );

    sethvalue( L, &L->storage, newStorageTable );

    // Since the storage table is now on the reference thread, we can dereference the table.
    newStorageTable->DereferenceGC( L );

    setnilvalue(gt(L));
}

static void luaE_threadEntryPoint( NativeExecutive::CFiber *theFiber, void *ud )
{
    lua_Thread *L = (lua_Thread*)ud;

    try
    {
        // First yield is invisible
        L->yield();

        int posOfFunction = 2;
        int currentTop = lua_gettop( L );

        int numArgs = ( currentTop - posOfFunction );

        lua_call( L, numArgs, LUA_MULTRET );

	    L->errorCode = 0;	// we gracefully quit :)
    }
    catch( lua_exception& e )
    {
        if ( G( e.getThread() )->mainthread != G( L )->mainthread )
        {
            // We do not need any stack values
            lua_settop( L, 0 );

		    L->errorCode = LUA_ERRSYS;
        }
        else
        {
		    L->errorCode = e.status();

            L->rtStack.Lock( L );

            luaD_seterrorobj( L, L->errorCode );
            L->rtStack.SetTopOffset( L, L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack ) );

            L->rtStack.Unlock( L );
        }
    }
    catch( lua_thread_termination& e )
    {
        lua_settop( L, 0 );

	    L->errorCode = e.status();
    }
    catch( ... )
    {
        lua_settop( L, 0 );

	    L->errorCode = LUA_ERRSYS;
    }
}

struct threadConstructionParams
{
    lua_State *runtimeThread;
};

lua_Thread::lua_Thread( global_State *g, void *construction_params ) : lua_State( g )
{
    threadConstructionParams *params = (threadConstructionParams*)construction_params;

    lua_State *rtThread = g->mainthread;

    if ( params )
    {
        rtThread = params->runtimeThread;
    }

    this->isMain = false;
    this->yieldDisabled = false;
    this->callee = nullptr;
    this->fiber = nullptr;
	this->status = THREAD_SUSPENDED;

    luaC_link(g, this, LUA_TTHREAD);

    LIST_INSERT( g->threads.root, this->threadNode );  /* we need to be aware of all threads */

    preinit_state(this, g);
    stack_init(this, rtThread);  /* init stack */

    setobj2n(rtThread, gt(this), gt(rtThread));  /* share table of globals */

    // Share hooking properties.
    this->hookmask = rtThread->hookmask;
    this->basehookcount = rtThread->basehookcount;
    this->hook = rtThread->hook;
    resethookcount(this);

    // Inherit the metatables
    for ( unsigned int n = 0; n < NUM_TAGS; n++ )
    {
        this->mt[n] = rtThread->mt[n];
    }

    // Allocate the OS resources only if necessary!

    lua_assert(iswhite(this));
}

// Factory for thread creation and destruction.
lua_Thread* luaE_newthread( lua_State *L )
{
    global_State *g = G(L);

    // Get the type information.
    globalStateMainStateFactoryMeta *typeInfo = mainLuaStateConnectingBridge.GetMetaStruct( g->config );

    // No point in continuing if we do not have type information.
    if ( !typeInfo )
        return nullptr;

    threadConstructionParams params;
    params.runtimeThread = L;

    lua_Thread *L1 = lua_new <lua_Thread> ( L, typeInfo->luaThreadTypeInfo, &params );

    return L1;
}

void luaE_freethread( lua_State *L, lua_State *L1 )
{
    lua_delete <lua_Thread> ( L, (lua_Thread*)L1 );
}

void luaE_terminate( lua_Thread *L )
{
    NativeExecutive::CExecutiveManager *manager = GetGlobalStateMultithreadingManager( L->gstate );

    if ( !manager )
        return;

    NativeExecutive::CFiber *env = (NativeExecutive::CFiber*)L->fiber;

    if ( !env )
        return;

    if ( L->status != THREAD_TERMINATED )
    {
        // Terminate it natively.
        manager->TerminateFiber( env );
    }
	else
	{
        luaF_close(L, L->rtStack.Base());  /* close all upvalues for this thread */
        lua_assert(L->openupval.IsEmpty() == true);

		// Threads clean their environments after themselves
		manager->CloseFiber( env );

		L->fiber = nullptr;
	}
}

bool lua_Thread::AllocateRuntime( void )
{
    if ( fiber )
        return true;

    NativeExecutive::CExecutiveManager *mtManager = GetGlobalStateMultithreadingManager( G(this) );

    if ( !mtManager )
        return false;

    NativeExecutive::CFiber::fiberexec_t fiberProc = luaE_threadEntryPoint;

    if ( fiberProc != nullptr )
    {
        fiber = mtManager->CreateFiber( fiberProc, this );

        if ( fiber != nullptr )
        {
            // initiate it
            resume();
            return true;
        }
    }
    return false;
}

void lua_Thread::resume( void )
{
    NativeExecutive::CFiber *env = (NativeExecutive::CFiber*)this->fiber;

    env->resume();
}

void lua_Thread::yield( void )
{
    status = THREAD_SUSPENDED;

    NativeExecutive::CFiber *env = (NativeExecutive::CFiber*)this->fiber;

    env->yield();

    status = THREAD_RUNNING;
}

lua_State::lua_State( const lua_State& right ) : GrayObject( right )
{
    // TODO: I think cloning a thread sometimes is possible.
    throw lua_exception( this->gstate->mainthread, LUA_ERRRUN, "attempt to clone a thread", 1 );
}

lua_State::~lua_State( void )
{
    return;
}

lua_Thread::~lua_Thread( void )
{
    global_State *g = this->gstate;

    // unlist ourselves
    LIST_REMOVE( this->threadNode );

    luaE_terminate( this );
    luai_userstatefree( this );

    freestack( g->mainthread, this );

    luaC_unlink( g, this );
}

LUAI_FUNC void luaE_newenvironment( lua_State *L )
{
    Table *g = luaH_new( L, 0, 2 );

    sethvalue( L, gt(L), g );

    // Since the new environment is now living on the thread, we can dereference it.
    g->DereferenceGC( L );

    luaC_objbarriert( L, g, L );

    for ( unsigned int n = 0; n < NUM_TAGS; n++ )
    {
        L->mt[n] = nullptr;
    }
}

LUAI_FUNC LuaTypeSystem::typeInfoBase* luaE_getmainthreadtype( global_State *g )
{
    globalStateMainStateFactoryMeta *metaInfo = mainLuaStateConnectingBridge.GetMetaStruct( g->config );

    return metaInfo->luaStateTypeInfo;
}

// Entry point to the Lua library (old style).
#include "lstate.lowlevel.hxx"

void luaE_libraryinit( void )
{
    mainLuaStateConnectingBridge.RegisterPluginStruct( namespaceFactory );

#if 0
    _globalStateMetaInfoPluginOffset =
        namespaceFactory.RegisterDependantStructPlugin <mainStateTypeInfo> ( namespaceFactory_t::ANONYMOUS_PLUGIN_ID );

    mainLuaStateConnectingBridge.RegisterPluginStruct( namespaceFactory );
#endif
}

void luaE_libraryshutdown( void )
{
    mainLuaStateConnectingBridge.UnregisterPluginStruct();
}

static void close_state (lua_State *L)
{
    global_State *g = G(L);

    luaF_close(L, L->rtStack.Base());  /* close all upvalues for this thread */
    luaC_freeall(L);  /* collect all objects */
    lua_assert(g->strt.nuse == 0);
    luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size);

    // Call post-state destructors.
    luaS_stateshutdown( L );

    freestack(L, L);

    // Close down type runtimes.
    luaQ_runtimeshutdown( g );
    luaF_runtimeshutdown( g );

    luaH_clearRuntimeMemory( g );
}

static void dealloc_state( global_State *globalState )
{
    // Get the Lua VM configuration.
    lua_config *config = globalState->config;

    bool hasUniqueConfiguration = globalState->hasUniqueConfig;

    // Delete the plugin object and destroy the allocator info.
    GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)config->allocData;

    // If this assert fails, then GC is flawed because it did not clean up everything.
    // A flawed GC can have many reasons, even can be caused by the application!
    lua_assert(globalState->totalbytes == config->globalStateFactory.GetClassSize());

    allocData->constructor->Destroy( globalState );

    // Dereference our meta struct.
    globalStateMainStateFactoryMeta *metaInfo = mainLuaStateConnectingBridge.GetMetaStruct( config );

    metaInfo->Dereference();

    // Get allocator information to destroy the last bit of data.
    GeneralMemoryAllocator memAlloc( allocData->_memAlloc );

    // Destroy configuration (if unique).
    if ( hasUniqueConfiguration )
    {
        // This should delete all remaining memory.
        lua_freeconfig( config );
    }
}

inline lua_State *_newstatetemplate( lua_config *cfg, bool hasUniqueConfig, bool& hasCleanedUpConfig )
{
    // Get the allocation data.
    GlobalStateAllocPluginData *allocData = (GlobalStateAllocPluginData*)cfg->allocData;

    // Get our environment inside of the configuration struct.
    globalStateMainStateFactoryMeta *metaInfo = mainLuaStateConnectingBridge.GetMetaStruct( cfg );

    if ( metaInfo )
    {
        // Reference it.
        metaInfo->Reference();

        // Construct it specially.
        struct globalStateConstructor
        {
            lua_config *config;
            bool hasUniqueConfig;

            inline global_State* Construct( void *mem ) const
            {
                global_State *g = new (mem) global_State;

                // Set the configuration.
                // Store whether we have an unique configuration.
                // If so, we will have to destroy it when we close this state.
                g->hasUniqueConfig = hasUniqueConfig;
                g->config = config;

                // Store local configuration values.
                // These values must not change across the runtime.
                g->isMultithreaded = config->isMultithreaded;

                return g;
            }
        };

        // Attempt to construct our global state.
        globalStateConstructor constructor;
        constructor.config = cfg;
        constructor.hasUniqueConfig = hasUniqueConfig;

        global_State *g = allocData->constructor->ConstructTemplate( constructor );

        if ( g )
        {
            // Attempt to get the main thread from the global state.
            lua_MainState *L = nullptr;
            {
                LuaRTTI *rtObjMainState = mainLuaStateConnectingBridge.GetPluginStructFromMetaStruct( metaInfo, g );

                // State-cast to the main state.
                L = (lua_MainState*)cfg->typeSys.GetObjectFromTypeStruct( rtObjMainState );
            }

            if ( L )
            {
                // Properly initialize the main state global object.
                g->strt.size = 0;
                g->strt.nuse = 0;
                g->strt.hash = nullptr;
                g->totalbytes = (lu_mem)cfg->globalStateFactory.GetClassSize();

                // Initialize type runtimes.
                luaF_runtimeinit( g );
                luaQ_runtimeinit( g );

                // Initialize the garbage collector
                luaC_init( g );
                preinit_state( L, g );
                luaE_newenvironment( L );   // create the first environment; threads will inherit it

                Table *registryTable = luaH_new(L, 0, 2);

                sethvalue(L, &g->l_registry, registryTable);  /* registry */

                // Since the registry table is now living on the global_State, we can dereference it.
                registryTable->DereferenceGC( L );

                // To properly initialize the main state, we must call post-state initializators.
                luaS_stateinit( L );

                for ( unsigned int i = 0; i < LUA_NUM_EVENTS; i++ )
                {
                    g->events[i] = nullptr;
                }

                std::string errMsg;

                // Initialize GCthread
                luaC_initthread( g );

                bool luaInternalInitSuccess = ( luaD_rawrunprotected( L, f_luaopen, nullptr, errMsg, nullptr ) == 0 );

                if ( luaInternalInitSuccess )
                {
                    luai_userstateopen(L);
                    return L;
                }

                /* memory allocation error: free partial state */
                close_state( L );
            }

            dealloc_state( g );

            hasCleanedUpConfig = true;
        }
    }

    return nullptr;
}

lua_State *lua_newstate (lua_Alloc f, void *ud)
{
    // Create a Lua configuration entity.
    lua_config *cfg = lua_newconfig( f, ud );

    if ( !cfg )
        return nullptr;

    bool hasCleanedUpConfig = false;

    if ( lua_State *returnState = _newstatetemplate( cfg, true, hasCleanedUpConfig ) )
    {
        return returnState;
    }

    if ( !hasCleanedUpConfig )
    {
        lua_freeconfig( cfg );
    }

    // No success creating a Lua state.
    return nullptr;
}

lua_State* lua_newstateconfig( lua_config *config )
{
    // We already have a config, so just try constructing.
    bool hasCleanedUpConfig;

    return _newstatetemplate( config, false, hasCleanedUpConfig );
}

LUA_API void lua_close (lua_State *L)
{
    // The pointer passed to us must be a valid main state pointer.
    global_State *globalState = G(L);

    {
        L = G(L)->mainthread;  /* only the main thread can be closed */

        lua_lock(L);
        luaF_close(L, L->rtStack.Base());  /* close all upvalues for this thread */

        // Collect all pending memory
        luaC_shutdown( G(L) );

        luai_userstateclose(L);

        close_state(L);
    }

    dealloc_state( globalState );

    // Finito!
}

// MTA specific
LUA_API lua_State* lua_getmainstate (lua_State *L)
{
    return G(L)->mainthread;
}


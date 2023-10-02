#ifndef _LUA_RUNTIME_OS_RESOURCE_THREAD_INTERNAL_
#define _LUA_RUNTIME_OS_RESOURCE_THREAD_INTERNAL_

#include "lobject.h"
#include "lgc.h"

#include "lpluginutil.hxx"

#include <NativeExecutive/CExecutiveManager.h>

#include "lmultithreading.hxx"

/*
** Lua Native OS thread type for pre-emptive multithreading.
** A native OS thread that runs by resuming a lua_Thread.
*/
struct Runtime : public GrayObject
{
    inline Runtime( global_State *g, void *construction_params ) : GrayObject( g )
    {
        luaC_link( g, this, LUA_TRUNTIME );

        this->execEnv = nullptr;
        this->nativeThread = nullptr;
    }

    inline Runtime( const Runtime& right ) : GrayObject( right )
    {
        throw lua_exception( right.gstate->mainthread, LUA_ERRRUN, "cannot clone a native Runtime object" );
    }

    inline ~Runtime( void )
    {
        global_State *g = this->gstate;

        luaC_unlink( g, this );
    }

    size_t Propagate( global_State *g )
    {
        return 0;
    }

    bool GCRequiresBackBarrier( void ) const
    {
        return false;
    }

    NativeExecutive::CExecThread *nativeThread;
    lua_Thread *execEnv;
};

// TODO: before creating this lock we must ensure that g->isMultithreaded is TRUE
struct ReadWriteLock : public GCObject
{
    inline ReadWriteLock( global_State *g, void *construction_params ) : GCObject( g )
    {
        luaC_link( g, this, LUA_TOBJECT );

        globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g );

        if ( mtEnv )
        {
            NativeExecutive::CExecutiveManager *nativeMan = mtEnv->nativeMan;

            if ( nativeMan )
            {
                nativeMan->CreatePlacedReadWriteLock( this + 1 );
            }
        }
    }

    inline ReadWriteLock( const ReadWriteLock& right ) : GCObject( right )
    {
        throw lua_exception( this->gstate->mainthread, LUA_ERRRUN, "cannot clone native read write lock" );
    }

    inline ~ReadWriteLock( void )
    {
        global_State *g = this->gstate;

        globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g );

        if ( mtEnv )
        {
            NativeExecutive::CExecutiveManager *nativeMan = mtEnv->nativeMan;

            if ( nativeMan )
            {
                nativeMan->ClosePlacedReadWriteLock( (NativeExecutive::CReadWriteLock*)( this + 1 ) );
            }
        }

        luaC_unlink( g, this );
    }

    // This is a dynamic struct that has the native lock constructed at its end.
    inline NativeExecutive::CReadWriteLock* GetLock( void )
    {
        return (NativeExecutive::CReadWriteLock*)( this + 1 );
    }
};

struct runtimeTypeInfoPlugin_t
{
    struct rwlockMetaTypeInfo_t : public LuaTypeSystem::structTypeMetaInfo
    {
        static inline size_t GetAdditionalTypeSize( global_State *g )
        {
            size_t additionalStructSize = 0;

            if ( globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g ) )
            {
                NativeExecutive::CExecutiveManager *nativeMan = mtEnv->nativeMan;

                if ( nativeMan )
                {
                    additionalStructSize += nativeMan->GetReadWriteLockStructSize();
                }
            }

            return additionalStructSize;
        }

        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            return sizeof( ReadWriteLock ) + GetAdditionalTypeSize( g );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *mem ) const override
        {
            return sizeof( ReadWriteLock ) + GetAdditionalTypeSize( g );
        }
    };

    rwlockMetaTypeInfo_t _rwlockMetaTypeInfo;

    inline void Initialize( lua_config *cfg )
    {
        // Create multithreading system types.
        this->runtimeTypeInfo = cfg->typeSys.RegisterStructType <Runtime> ( "runtime", cfg->grayobjTypeInfo );
        this->rwlockTypeInfo = cfg->typeSys.RegisterDynamicStructType <ReadWriteLock> ( "rwlock", &_rwlockMetaTypeInfo, false, cfg->gcobjTypeInfo );

        // Configure our types.
        if ( LuaTypeSystem::typeInfoBase *runtimeTypeInfo = this->runtimeTypeInfo )
        {
            cfg->typeSys.SetTypeInfoExclusive( runtimeTypeInfo, true );
        }
    }

    inline void Shutdown( lua_config *cfg )
    {
        // Destroy our multithreading system types.
        if ( this->rwlockTypeInfo )
        {
            cfg->typeSys.DeleteType( this->rwlockTypeInfo );
        }

        if ( this->runtimeTypeInfo )
        {
            cfg->typeSys.DeleteType( this->runtimeTypeInfo );
        }
    }

    // We need to hold our two types.
    LuaTypeSystem::typeInfoBase *runtimeTypeInfo;
    LuaTypeSystem::typeInfoBase *rwlockTypeInfo;
};

typedef PluginDependantStructRegister <runtimeTypeInfoPlugin_t, namespaceFactory_t> runtimeTypeInfoPluginRegister_t;

extern runtimeTypeInfoPluginRegister_t runtimeTypeInfoPluginRegister;

#endif //_LUA_RUNTIME_OS_RESOURCE_THREAD_INTERNAL_
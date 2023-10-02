// Lua Closure interface local shared include
#ifndef _LUA_CLOSURE_SHARED_INCLUDE_
#define _LUA_CLOSURE_SHARED_INCLUDE_

#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"

#include "lfunc.class.hxx"
#include "lfunc.upval.hxx"

#include "lpluginutil.hxx"

struct cclosureSharedConstructionParams
{
    int nelems;
    lua_State *runtimeThread;
};

// Closure type info plugin.
struct closureTypeInfoPlugin_t
{
    struct cclosureBasicMetaTypeInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

            int nelems = 0;

            if ( params )
            {
                nelems = params->nelems;
            }

            return sizeCclosure( nelems );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *mem ) const override
        {
            CClosureBasic *cl = (CClosureBasic*)mem;

            return sizeCclosure( cl->nupvalues );
        }
    };

    struct cclosureMethodMetaTypeInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

            int nelems = 0;

            if ( params )
            {
                nelems = params->nelems;
            }

            return sizeCmethod( nelems );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *mem ) const override
        {
            CClosureMethod *method = (CClosureMethod*)mem;

            return sizeCmethod( method->nupvalues );
        }
    };

    struct cclosureMethodTransMetaTypeInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

            int nelems = 0;

            if ( params )
            {
                nelems = params->nelems;
            }

            return sizeCmethodt( nelems );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *mem ) const override
        {
            CClosureMethodTrans *method = (CClosureMethodTrans*)mem;

            return sizeCmethodt( method->nupvalues );
        }
    };

    struct lclosureMetaTypeInfo : public LuaTypeSystem::structTypeMetaInfo
    {
        size_t GetTypeSize( global_State *g, void *construction_params ) const override
        {
            cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

            int nelems = 0;

            if ( params )
            {
                nelems = params->nelems;
            }

            return sizeLclosure( nelems );
        }

        size_t GetTypeSizeByObject( global_State *g, const void *mem ) const override
        {
            LClosure *cl = (LClosure*)mem;

            return sizeLclosure( cl->nupvalues );
        }
    };

    // Closure meta stuff.
    cclosureBasicMetaTypeInfo _cclosureBasicMeta;
    cclosureMethodMetaTypeInfo _cclosureMethodMeta;
    cclosureMethodTransMetaTypeInfo _cclosureMethodTransMeta;
    lclosureMetaTypeInfo _lclosureMeta;

    inline void Initialize( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Set up types.
        closureTypeInfo =
            typeSys.RegisterAbstractType <Closure> ( "closure", cfg->grayobjTypeInfo );
        cclosureTypeInfo =
            typeSys.RegisterAbstractType <CClosure> ( "cclosure", closureTypeInfo );
        cclosureMethodRedirectTypeInfo =
            typeSys.RegisterStructType <CClosureMethodRedirect> ( "cclosure_MethodRedirect", cclosureTypeInfo );
        cclosureMethodRedirectSuperTypeInfo =
            typeSys.RegisterStructType <CClosureMethodRedirectSuper> ( "cclosure_MethodRedirectSuper", cclosureTypeInfo );
        cclosureBasicTypeInfo =
            typeSys.RegisterDynamicStructType <CClosureBasic> ( "cclosure_Basic", &_cclosureBasicMeta, false, cclosureTypeInfo );
        cclosureMethodBaseTypeInfo =
            typeSys.RegisterAbstractType <CClosureMethodBase> ( "cclosure_MethodBase", cclosureTypeInfo );
        cclosureMethodTypeInfo =
            typeSys.RegisterDynamicStructType <CClosureMethod> ( "cclosure_Method", &_cclosureMethodMeta, false, cclosureMethodBaseTypeInfo );
        cclosureMethodTransTypeInfo =
            typeSys.RegisterDynamicStructType <CClosureMethodTrans> ( "cclosure_MethodTrans", &_cclosureMethodTransMeta, false, cclosureMethodBaseTypeInfo );
        lclosureTypeInfo =
            typeSys.RegisterDynamicStructType <LClosure> ( "lclosure", &_lclosureMeta, false, closureTypeInfo );
        upvalueTypeInfo =
            typeSys.RegisterStructType <UpVal> ( "upvalue", cfg->gcobjTypeInfo );
        protoTypeInfo =
            typeSys.RegisterStructType <Proto> ( "proto", cfg->grayobjTypeInfo );

        // Set up properties.
        typeSys.SetTypeInfoExclusive( this->cclosureMethodRedirectTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->cclosureMethodRedirectSuperTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->cclosureBasicTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->cclosureMethodTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->cclosureMethodTransTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->lclosureTypeInfo, true );
        typeSys.SetTypeInfoExclusive( this->upvalueTypeInfo, true );
    }

    inline void Shutdown( lua_config *cfg )
    {
        // Destroy types.
        LuaTypeSystem& typeSys = cfg->typeSys;

        typeSys.DeleteType( lclosureTypeInfo );
        typeSys.DeleteType( cclosureMethodTransTypeInfo );
        typeSys.DeleteType( cclosureMethodTypeInfo );
        typeSys.DeleteType( cclosureMethodBaseTypeInfo );
        typeSys.DeleteType( cclosureBasicTypeInfo );
        typeSys.DeleteType( cclosureMethodRedirectSuperTypeInfo );
        typeSys.DeleteType( cclosureMethodRedirectTypeInfo );
        typeSys.DeleteType( cclosureTypeInfo );
        typeSys.DeleteType( closureTypeInfo );
    }

    // Type information.
    LuaTypeSystem::typeInfoBase *closureTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureTypeInfo;
    LuaTypeSystem::typeInfoBase *lclosureTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureMethodRedirectTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureMethodRedirectSuperTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureBasicTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureMethodBaseTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureMethodTypeInfo;
    LuaTypeSystem::typeInfoBase *cclosureMethodTransTypeInfo;
    LuaTypeSystem::typeInfoBase *upvalueTypeInfo;
    LuaTypeSystem::typeInfoBase *protoTypeInfo;
};

typedef PluginDependantStructRegister <closureTypeInfoPlugin_t, namespaceFactory_t> closureTypeInfo_t;

extern closureTypeInfo_t closureTypeInfo;

// Closure global state plugin.
struct globalStateClosureEnvPlugin
{
    inline globalStateClosureEnvPlugin( void )
    {
        return;
    }

    RwList <UpVal> uvhead;  /* head of double-linked list of all open upvalues */

    struct ProtoConstValueAccessContext : public ConstValueContext
    {
        LuaCachedConstructedClassAllocator <ProtoConstValueAccessContext> *_usedAlloc;

        Proto *owningProto;

        const TValue *theValue;

        unsigned long refCount;

        inline ProtoConstValueAccessContext( void )
        {
            this->_usedAlloc = nullptr;
            this->owningProto = nullptr;
            this->theValue = nullptr;
            this->refCount = 0;
        }

        void Reference( lua_State *L )
        {
            this->refCount++;
        }

        void Dereference( lua_State *L )
        {
            if ( this->refCount-- == 1 )
            {
                this->_usedAlloc->Free( L, this );
            }
        }

        const TValue* const* GetValuePointer( lua_State *L )
        {
            return &theValue;
        }
    };

    LuaCachedConstructedClassAllocator <ProtoConstValueAccessContext> _constProtoValueAccessAlloc;

    inline ProtoConstValueAccessContext* NewProtoConstValueAccessContext( lua_State *L, Proto *theProto )
    {
        ProtoConstValueAccessContext *valCtx = this->_constProtoValueAccessAlloc.Allocate( L );

        valCtx->_usedAlloc = &this->_constProtoValueAccessAlloc;
        valCtx->owningProto = theProto;

        return valCtx;
    }

    inline void Initialize( global_State *g )
    {
        // TODO.
    }

    inline void ClearMemory( global_State *g )
    {
        // Deallocate the cached structures.
        _constProtoValueAccessAlloc.Shutdown( g );
    }

    inline void Shutdown( global_State *g )
    {
        ClearMemory( g );
    }
};

typedef PluginConnectingBridge
    <globalStateClosureEnvPlugin,
        globalStateDependantStructFactoryMeta <globalStateClosureEnvPlugin, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        closureEnvConnectingBridge_t;

extern closureEnvConnectingBridge_t closureEnvConnectingBridge;

#endif //_LUA_CLOSURE_SHARED_INCLUDE_

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vendor/lua/src/lclass.c
*  PURPOSE:     Lua class system extension
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "luacore.h"
#include <algorithm>

#include "lfunc.h"
#include "lfunc.class.hxx"
#include "ldispatch.class.hxx"

#include "lclass.hxx"

#include <sdk/MacroUtils.h>

// Class type info plugin.
classTypeInfoPlugin_t classTypeInfoPlugin( namespaceFactory, namespaceFactory_t::ANONYMOUS_PLUGIN_ID );

// Class maintenance plugin inside of the global_State.
classEnvConnectingBridge_t classEnvConnectingBridge;

class ClassMethodRegister : public VirtualClassEntry
{
public:
    ClassMethodRegister( lua_CFunction proto, Table *methTable ) : m_proto( proto ), m_methTable( methTable )
    {
    }

    inline void     BaseCall( lua_State *L, int argCount ) const
    {
        lua_xcopy( L, L, argCount );

        RtStackView::baseProtect protect = L->GetCurrentStackFrame().ProtectSlots( L, L->rtStack, argCount );

        m_proto( L );
    }

    virtual Closure*    Create( lua_State *L, Class *j ) const = 0;
    virtual void    Call( lua_State *L, Class *j, int argCount ) const = 0;

    void    MarkValue( global_State *g ) const
    {
        luaC_markobject( g, m_methTable );
    }

    Table*  GetMethodTable( void ) const
    {
        return m_methTable;
    }

    void* operator new( size_t size, lua_State *L, ClassStringTable *alloc, TString *methName )
    {
        return alloc->GetNodeValue( alloc->AllocateNode( L, methName, size ) );
    }

    void operator delete( void *ptr, lua_State *L, ClassStringTable *alloc, TString *methName )
    {
        alloc->DeleteNode( L, alloc->FindNode( methName ) );
    }

    void operator delete( void *ptr )
    {
        lua_assert( 0 );
    }

protected:
    lua_CFunction m_proto;
    Table* m_methTable;
};

AINLINE void class_allocateRegisterCache( lua_State *L, Class *j )
{
    if ( !j->methodCache )
    {
        j->methodCache = new (L) ClassStringTable;
    }
}

AINLINE Closure* class_getDelayedMethodRegister( lua_State *L, Class *j, TString *methName, bool excludeInternal )
{
    if ( !j->methodCache )
        return nullptr;

    STableItemHeader *node = j->methodCache->FindNode( methName );

    if ( !node )
        return nullptr;

    ClassMethodRegister *reg = (ClassMethodRegister*)j->methodCache->GetNodeValue( node );
    Table *regTable = reg->GetMethodTable();

    if ( excludeInternal && regTable == j->internStorage )
        return nullptr;

    Closure *res = reg->Create( L, j );

    j->SetMethod( L, methName, res, reg->GetMethodTable() );

    // Now that we stored this method in the class, we can dereference it.
    res->DereferenceGC( L );

    j->methodCache->DeleteNode( L, node );
    return res;
}

AINLINE void class_readFromStorage( lua_State *L, Class *j, const TValue *key, StkId val, bool excludeInternal )
{
    {
        ConstValueAddress envRes = luaH_get( L, j->storage, key );

        if ( envRes != luaO_nilobject )
        {
            setobj( L, val, envRes );
            return;
        }
    }

    if ( ttype( key ) == LUA_TSTRING )
    {
        // Check the delayed method registry
        TString *methName = tsvalue( key );
        Closure *cached = class_getDelayedMethodRegister( L, j, methName, excludeInternal );

        if ( cached )
        {
            setclvalue( L, val, cached );
            return;
        }
    }

    setnilvalue( val );
}

AINLINE ConstValueAddress class_getFromStorage( lua_State *L, Class *j, const TValue *key, bool excludeInternal )
{
    {
        ConstValueAddress envRes = luaH_get( L, j->storage, key );

        if ( envRes != luaO_nilobject )
        {
            return envRes;
        }
    }

    if ( ttype( key ) == LUA_TSTRING )
    {
        // Check the delayed method registry
        TString *methName = tsvalue( key );
        Closure *cached = class_getDelayedMethodRegister( L, j, methName, excludeInternal );

        if ( cached )
        {
            ValueAddress tmpVal = L->GetTemporaryValue();

            setclvalue( L, tmpVal, cached );
            return tmpVal;
        }
    }

    return luaO_getnilcontext( L );
}

AINLINE ConstValueAddress class_getFromStorageString( lua_State *L, Class *j, TString *key, bool excludeInternal )
{
    // Try the storage.
    {
        ConstValueAddress envRes = luaH_getstr( L, j->storage, key );

        if ( envRes != luaO_nilobject )
        {
            return envRes;
        }
    }

    // Check the delayed method registry
    Closure *cached = class_getDelayedMethodRegister( L, j, key, excludeInternal );

    if ( cached )
    {
        ValueAddress tmpVal = L->GetTemporaryValue();

        setclvalue( L, tmpVal, cached );
        return tmpVal;
    }

    return luaO_getnilcontext( L );
}

void ClassOutEnvDispatch::Index( lua_State *L, ConstValueAddress& key, ValueAddress& val )
{
    class_readFromStorage( L, m_class, key, val, true );
}

AINLINE void class_checkMethodDelayOverride( lua_State *L, Class *j, const TValue *key )
{
    if ( j->methodCache && ttype( key ) == LUA_TSTRING )
    {
        if ( j->methodCache->FindNode( tsvalue( key ) ) )
            throw lua_exception( L, LUA_ERRRUN, "cannot overwrite methods of class internals" );
    }
}

void ClassOutEnvDispatch::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    class_checkMethodDelayOverride( L, m_class, key );

    ValueAddress tabidx = luaH_set( L, m_class->storage, key );

    if ( ttype( tabidx ) == LUA_TFUNCTION )
        throw lua_exception( L, LUA_ERRRUN, "cannot overwrite methods of class internals" );

    setobj( L, tabidx, val );
}

AINLINE void class_checkEnvIndex( lua_State *L, Class *j )
{
    if ( j->destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to index the environment of a destroyed class" );
}

AINLINE bool class_obtainPrivateEnvValue( lua_State *L, Class *j, ConstValueAddress& key, ValueAddress& value )
{
    const TValue *val = j->GetEnvValue( L, key );

    if ( ttype( val ) != LUA_TNIL )
    {
        setobj( L, value, val );
        return true;
    }

    LocalValueAddress tabVal;

    // Otherwise we result in the inherited environments
    for ( Class::envList_t::const_iterator iter = j->envInherit.begin(); iter != j->envInherit.end(); iter++ )
    {
        (*iter)->Index( L, key, tabVal );

        if ( ttype( tabVal ) != LUA_TNIL )
        {
            setobj( L, value, tabVal );
            return true;
        }
    }

    return false;
}

void ClassEnvDispatch::Index( lua_State *L, ConstValueAddress& key, ValueAddress& value )
{
    class_checkEnvIndex( L, m_class );

    if ( class_obtainPrivateEnvValue( L, m_class, key, value ) )
        return;

    // No return value.
    setnilvalue( value );
}

void Class::Index( lua_State *L, ConstValueAddress& key, ValueAddress& val )
{
    if ( destroyed )
        throw lua_exception( L, LUA_ERRRUN, "cannot index a destroyed class" );

    ConstValueAddress tm = class_getFromStorageString( L, this, G(L)->tmname[TM_INDEX], true );

    if ( tm != luaO_nilobject )
    {
        if ( ttisfunction( tm ) )
        {
            RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

            luaD_checkstack( L, 3 );

            pushclvalue( L, clvalue( tm ) );
            pushjvalue( L, this );
            pushtvalue( L, key );
            lua_call( L, 2, 1 );

            popstkvalue( L, val );
        }
        else
            luaV_gettable( L, tm, key, val );
    }
    else
    {
        // We redirect our call to the outer environment
        outenv->Index( L, key, val );
    }
}

void Class::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    if ( destroyed )
        throw lua_exception( L, LUA_ERRRUN, "cannot index a destroyed class" );

    ConstValueAddress tm = class_getFromStorageString( L, this, G(L)->tmname[TM_NEWINDEX], true );

    if ( tm != luaO_nilobject )
    {
        if ( ttisfunction( tm ) )
        {
            RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

            luaD_checkstack( L, 4 );    // allocate a new stack

            // Work with the new stack
            pushclvalue( L, clvalue( tm ) );
            pushjvalue( L, this );
            pushtvalue( L, key );
            pushtvalue( L, val );
            lua_call( L, 3, 0 );
        }
        else
        {
            // Repeat the process with the index object
            // For security reasons, we should increment the callstack depth
            callstack_ref indexRef( *L );

            luaV_settable( L, tm, key, val );
        }
    }
    else
    {
        // We redirect our call to the outer environment
        outenv->NewIndex( L, key, val );
    }
}

void Class::IncrementMethodStack( lua_State *lua )
{
    lua_assert( !destroyed );

    inMethod++;
}

inline static void class_unlinkParent( lua_State *L, Class& j )
{
    // Unlink us from our parent, if present
    if ( !j.parent )
        return;

    lua_checkstack( L, 1 );

    j.childAPI->PushMethod( L, "destroy" );
    lua_call( L, 0, 0 );

    j.childAPI->DecrementMethodStack( L );

    j.childAPI = nullptr;
    j.parent = nullptr;
}

inline void class_runDestructor( lua_State *L, Class& j, Closure *cl )
{
    try
    {
        pushclvalue( L, cl );
        lua_call( L, 0, 0 );
    }
    catch( lua_exception& )
    {
        Closure *handler = G(L)->events[LUA_EVENT_DESTRUCTOR_EXCEPTION];

        if ( handler )
        {
            lua_checkstack( L, 2 );

            pushclvalue( L, handler );
            pushjvalue( L, &j );
            lua_pcall( L, 1, 0, 0 );
        }
    }
}

static int childapi_notifyDestroy( lua_State *L )
{
    ConstValueAddress jvalParent = index2constadr( L, lua_upvalueindex( 1 ) );
    ConstValueAddress jvalChild = index2constadr( L, lua_upvalueindex( 2 ) );

    Class& j = *jvalue( jvalParent );

    ConstValueAddress val = jvalue( jvalChild )->GetSuperMethod( L );

    // First let all other handlers finish
    if ( ttype( val ) == LUA_TFUNCTION )
    {
        pushtvalue( L, val );
        lua_call( L, 0, 0 );
    }

    if ( j.childCount != 0 )
        return 0;

    class_unlinkParent( L, j );

    lua_checkstack( L, 1 );

    class_runDestructor( L, j, j.destructor );
    return 0;
}

inline static bool class_preDestructor( lua_State *L, Class& j )
{
    if ( j.destroying )
        return false;

    // Make sure we do not try to destroy twice
    j.destroying = true;

    bool reqWorthy = false;

    // We potentially are operating at an unstable state here, since we use child_iter from
    // a destroyed class. It is a thrilling optimization not having to secure it from GC!
    // If crashes report at this loop, I will take care of it (very unlikely due to current GC architecture).
    LIST_FOREACH_BEGIN( Class, j.children.root, child_iter )
        item->PushMethod( L, "destroy" );
        lua_call( L, 0, 0 );

        //TODO: This may be risky, I have to analyze whether GC may kill our runtime
        //REPORT: I have seen no errors so far!

        if ( !item->destroyed )
        {
            pushjvalue( L, &j );
            pushjvalue( L, item->childAPI );
            lua_pushcclosure( L, childapi_notifyDestroy, 2 );
            item->childAPI->RegisterMethod( L, "notifyDestroy" );

            reqWorthy = true;
        }
    LIST_FOREACH_END

    if ( !reqWorthy )
        class_unlinkParent( L, j );

    return !reqWorthy;
}

bool Class::PreDestructor( lua_State *L )
{
    return class_preDestructor( L, *this );
}

void Class::CheckDestruction( lua_State *L )
{
    // I am not sure whether the stack could corrupt.
    // This needs testing with exception handling
    // Yes, exceptions will not hinder the requested destruction of a class.

    // The new lua thread architecture preserves class referencing!
    // Now there is no issue with keeping a coroutine state to save a class
    // from destruction. Hahaha!
    // For this reason, be sure to prevent yield-deadlocks by using lua_yield_shield
    // because the Lua environment is coroutine-safe. A halted coroutine in a class callback
    // handler runtime will prevent the class' destruction until the runtime left the class'
    // methods!

    // Only problem could occur if the VM is short of memory.
    // Exceptions could be raised within exceptions, and I am unsure how to handle this.
    if ( reqDestruction && inMethod == 0 )
    {
        lua_yield_shield _ref( L ); // prevent destructor yielding

        if ( !class_preDestructor( L, *this ) )
            return;

        class_runDestructor( L, *this, destructor );
    }
}

void Class::DecrementMethodStack( lua_State *lua )
{
    inMethod--;

    CheckDestruction( lua );
}

// Used to clear referebces put by scripts; MTA clears them for
// every internal class prior to destruction
void Class::ClearReferences( lua_State *lua )
{
    if ( !refCount )
        return;

    inMethod -= refCount;

    refCount = 0;

    CheckDestruction( lua );
}

void Class::Push( lua_State *L )
{
    lua_assert( !destroyed );

    pushjvalue( L, this );
}

void Class::PushMethod( lua_State *L, const char *key )
{
    lua_assert( !destroyed );

    const TValue *val = GetEnvValueString( L, key );

    if ( ttype( val ) != LUA_TFUNCTION )
    {
        pushnilvalue( L );
    }
    else
    {
        pushtvalue( L, val );
    }
}

void Class::PushSuperMethod( lua_State *L )
{
    ConstValueAddress superMethodPtr = GetSuperMethod( L );

    pushtvalue( L, superMethodPtr );
}

void Class::CallMethod( lua_State *L, const char *key )
{
    // TODO: fast way to call class methods.
}

void Class::SetTransmit( int type, void *entity )
{
    lua_assert( !destroyed );

	unsigned int idx = transCount++;

	// Grow the array
	trans = (trans_t*)luaM_realloc__( this->gstate, trans, idx * sizeof(trans_t), transCount * sizeof(trans_t) );

	// Append the item
    trans_t& item = trans[idx];
	item.id = (unsigned char)type;
	item.ptr = entity;
}

bool Class::GetTransmit( int type, void*& entity )
{
    if ( destroyed )
    {
        return false;
    }

	if ( !trans )
	{
		return false;
    }

    for ( unsigned int n = 0; n < transCount; n++ )
	{
		trans_t& item = trans[n];

		if ( item.id == (unsigned char)type )
		{
			entity = item.ptr;
			return true;
		}
	}

	return false;
}

int Class::GetTransmit() const
{
	if ( !trans )
		return -1;

    return trans[transCount - 1].id;
}

bool Class::IsTransmit( int type ) const
{
	if ( !trans )
		return false;

    for ( unsigned int n = 0; n < transCount; n++ )
	{
		trans_t& item = trans[n];

		if ( item.id == (unsigned char)type )
			return true;
	}

	return false;
}

bool Class::IsDestroying() const
{
    return destroying;
}

bool Class::IsDestroyed() const
{
    return destroyed;
}

bool Class::IsRootedIn( Class *root ) const
{
    lua_assert( !destroyed );

    Class *elder = parent;

    while ( elder )
    {
        if ( root == elder )
            return true;

        elder = elder->parent;
    }

    return false;
}

bool Class::IsRootedIn( lua_State *L, int idx ) const
{
    Class *rootClass = nullptr;
    {
        ConstValueAddress jval = index2constadr( L, idx );

        rootClass = jvalue( jval );
    }

    return IsRootedIn( rootClass );
}

void Class::PushEnvironment( lua_State *L )
{
    lua_assert( !destroyed );

    pushgcvalue( L, env );
}

void Class::PushOuterEnvironment( lua_State *L )
{
    lua_assert( !destroyed );

    pushgcvalue( L, outenv );
}

void Class::PushInternStorage( lua_State *L )
{
    lua_assert( !destroyed );

    pushhvalue( L, internStorage );
}

void Class::PushChildAPI( lua_State *L )
{
    lua_assert( !destroyed );

    if ( !parent )
    {
        pushnilvalue( L );
    }
    else
    {
        pushjvalue( L, childAPI );
    }
}

void Class::PushParent( lua_State *L )
{
    lua_assert( !destroyed );

    if ( !parent )
    {
        pushnilvalue( L );
    }
    else
    {
        pushjvalue( L, parent );
    }
}

ConstValueAddress Class::GetEnvValue( lua_State *L, const TValue *key )
{
    // Browse the read-only storage
    {
        ConstValueAddress res = luaH_get( L, internStorage, key );

        if ( ttype( res ) != LUA_TNIL )
            return res;
    }

    return class_getFromStorage( L, this, key, false );
}

ConstValueAddress Class::GetEnvValueString( lua_State *L, const char *name )
{
    TString *key = luaS_new( L, name );

    try
    {
        {
            ConstValueAddress res = luaH_getstr( L, internStorage, key );

            if ( ttype( res ) != LUA_TNIL )
            {
                key->DereferenceGC( L );
                return res;
            }
        }

        {
            ConstValueAddress res = class_getFromStorageString( L, this, key, false );

            // We do not need the string anymore.
            key->DereferenceGC( L );

            return res;
        }
    }
    catch( ... )
    {
        key->DereferenceGC( L );
        throw;
    }

    // Never reached.
    return ConstValueAddress();
}

void Class::RequestDestruction()
{
    reqDestruction = true;
}

ValueAddress Class::SetSuperMethod( lua_State *L )
{
    return luaH_setstr( L, internStorage, GetSuperString( L ) );
}

ConstValueAddress Class::GetSuperMethod( lua_State *L )
{
    return luaH_getstr( L, internStorage, GetSuperString( L ) );
}

AINLINE void class_checkDestroyedMethodCall( lua_State *L, Class *j )
{
    if ( j->destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to access a destroyed class' method" );
}

// Easy wrapper for C++ support
class MethodStackAllocation
{
public:
    MethodStackAllocation( lua_State *thread, Class *myClass, Closure *prevSuper )
    {
        class_checkDestroyedMethodCall( thread, myClass );

        {
            ValueAddress superMethod = myClass->SetSuperMethod( thread );

            setobj( thread, &m_prevSuper, superMethod );

            if ( prevSuper )
            {
                setclvalue( thread, superMethod, prevSuper );
                luaC_objbarriert( thread, myClass->internStorage, prevSuper );
            }
            else
                setbvalue( superMethod, false );
        }

        myClass->IncrementMethodStack( thread );

        m_instance = myClass;
        m_thread = thread;
    }

    ~MethodStackAllocation( void )
    {
        {
            ValueAddress superMethodPtr = m_instance->SetSuperMethod( m_thread );

            setobj( m_thread, superMethodPtr, &m_prevSuper );
        }
        m_instance->DecrementMethodStack( m_thread );
    }

    Class*      m_instance;
    lua_State*  m_thread;
    TValue      m_prevSuper;
};

static int classmethod_root( lua_State *L )
{
    CClosureMethodRedirect *cl = (CClosureMethodRedirect*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, nullptr );
    int top = lua_gettop( L );

    pushclvalue( L, cl->redirect );
    lua_insert( L, 1 );
    lua_call( L, top, LUA_MULTRET );
    return lua_gettop( L );
}

static int classmethod_super( lua_State *L )
{
    CClosureMethodRedirectSuper *cl = (CClosureMethodRedirectSuper*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, cl->super );
    int top = lua_gettop( L );

    pushclvalue( L, cl->redirect );
    lua_insert( L, 1 );
    lua_call( L, top, LUA_MULTRET );
    return lua_gettop( L );
}

// Very fast C++-only prototypes
static int classmethod_rootNative( lua_State *L )
{
    CClosureMethodBase *cl = (CClosureMethodBase*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, nullptr );

    return cl->method( L );
}

static int classmethod_superNative( lua_State *L )
{
    CClosureMethodBase *cl = (CClosureMethodBase*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, cl->super );

    return cl->method( L );
}

static int classmethod_rootTransNative( lua_State *L )
{
    CClosureMethodTrans *cl = (CClosureMethodTrans*)curr_func( L );
    Class *j = cl->m_class;
    MethodStackAllocation member( L, j, nullptr );

    if ( !j->GetTransmit( cl->trans, cl->data ) )
        throw lua_exception( L, LUA_ERRRUN, "attempt to call a method whose native interface was destroyed" );

    return cl->method( L );
}

static int classmethod_superTransNative( lua_State *L )
{
    CClosureMethodTrans *cl = (CClosureMethodTrans*)curr_func( L );
    Class *j = cl->m_class;
    MethodStackAllocation member( L, j, cl->super );

    if ( !j->GetTransmit( cl->trans, cl->data ) )
        throw lua_exception( L, LUA_ERRRUN, "attempt to call a method whose native interface was destroyed" );

    return cl->method( L );
}

static int classmethod_forceSuperRoot( lua_State *L )
{
    CClosureMethodRedirect *cl = (CClosureMethodRedirect*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, nullptr );
    int top = lua_gettop( L );

    pushclvalue( L, cl->redirect );
    lua_insert( L, 1 );
    lua_call( L, top, 0 );
    return 0;
}

static int classmethod_forceSuper( lua_State *L )
{
    CClosureMethodRedirectSuper *cl = (CClosureMethodRedirectSuper*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, nullptr );
    int top = lua_gettop( L );

    // Push the current method which we must call
    pushclvalue( L, cl->redirect );

    // Push arguments (if any) (to clone them)
    for ( int n = 0; n < top; n++ )
    {
        lua_pushvalue( L, n+1 );
    }

    lua_call( L, top, 0 );

    // Now call the super method
    pushclvalue( L, cl->super );
    lua_insert( L, 1 );
    lua_call( L, top, 0 );
    return 0;
}

static int classmethod_forceSuperRootNative( lua_State *L )
{
    // Well, should do the same
    classmethod_rootNative( L );
    return 0;
}

static int classmethod_forceSuperNative( lua_State *L )
{
    CClosureMethodBase *cl = (CClosureMethodBase*)curr_func( L );
    MethodStackAllocation member( L, cl->m_class, nullptr );
    int top = lua_gettop( L );

    // Copy arguments while protecting slots
    for ( int n = 1; n <= top; n++ )
    {
        ConstValueAddress copyArgument = index2constadr( L, n );

        pushtvalue( L, copyArgument );
    }

    // Protect the slots.
    {
        RtStackView::baseProtect protect = L->GetCurrentStackFrame().ProtectSlots( L, L->rtStack, top );

        cl->method( L );    // call the actual method.
        lua_settop( L, 0 ); // restore the stack
    }

    pushclvalue( L, cl->super );
    lua_insert( L, 1 );
    lua_call( L, top, 0 );
    return 0;
}

static int classmethod_forceSuperRootTransNative( lua_State *L )
{
    CClosureMethodTrans *cl = (CClosureMethodTrans*)curr_func( L );
    Class *j = cl->m_class;
    MethodStackAllocation member( L, j, nullptr );

    if ( !j->GetTransmit( cl->trans, cl->data ) )
        throw lua_exception( L, LUA_ERRRUN, "attempt to call a method whose native interface was destroyed" );

    cl->method( L );
    return 0;
}

static int classmethod_forceSuperTransNative( lua_State *L )
{
    CClosureMethodTrans *cl = (CClosureMethodTrans*)curr_func( L );

    if ( !cl->m_class->GetTransmit( cl->trans, cl->data ) )
    {
        int top = lua_gettop( L );

        pushclvalue( L, cl );
        lua_insert( L, 1 );
        lua_call( L, top, 0 );
        return 0;
    }

    classmethod_forceSuperNative( L );
    return 0;
}

static Closure* classmethod_fsFSCCHandler( lua_State *L, Closure *newMethod, Class *j, Closure *prevMethod )
{
    if ( !prevMethod )
    {
        CClosureMethodRedirect *cl = luaF_newCmethodredirect( L, j->env, newMethod, j );
        cl->f = classmethod_forceSuperRoot;
        return cl;
    }
    else
    {
        CClosureMethodRedirectSuper *cl = luaF_newCmethodredirectsuper( L, j->env, newMethod, j, prevMethod );
        cl->f = classmethod_forceSuper;
        return cl;
    }
}

static Closure* classmethod_fsFSCCHandlerNative( lua_State *L, lua_CFunction proto, Class *j, Closure *prevMethod, _methodRegisterInfo& info )
{
    int num = info.numUpValues;

    if ( info.isTrans )
    {
        CClosureMethodTrans *cl = luaF_newCmethodtrans( L, num, j->env, j, info.transID );

        if ( prevMethod )
        {
            cl->super = prevMethod;
            cl->f = classmethod_forceSuperTransNative;
        }
        else
        {
            cl->super = nullptr;
            cl->f = classmethod_forceSuperRootTransNative;
        }

        cl->method = proto;

        L->GetCurrentStackFrame().CrossCopyTopExtern( L, L->rtStack, cl->upvalues, num );
        return cl;
    }
    else
    {
        CClosureMethod *cl = luaF_newCmethod( L, num, j->env, j );

        if ( prevMethod )
        {
            cl->super = prevMethod;
            cl->f = classmethod_forceSuperNative;
        }
        else
        {
            cl->super = nullptr;
            cl->f = classmethod_forceSuperRootNative;
        }

        cl->method = proto;

        L->GetCurrentStackFrame().CrossCopyTopExtern( L, L->rtStack, cl->upvalues, num );
        return cl;
    }
}

// Register future method definitions for the Forced Super Calling Convention (FSCC)
static int classmethod_registerForcedSuper( lua_State *L )
{
    Class& j = *((CClosureMethodBase*)curr_func( L ))->m_class;

    if ( j.destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to register a forced-super declaration inheritance for a destroyed class" );

    luaL_checktype( L, 1, LUA_TSTRING );

    ConstValueAddress stringVal = index2constadr( L, 1 );

    Class::forceSuperItem item;
    item.cb = classmethod_fsFSCCHandler;
    item.cbNative = classmethod_fsFSCCHandlerNative;
    j.forceSuper->SetItem( L, tsvalue( stringVal ), item );
    return 0;
}

static inline void luaJ_envtypeerror( lua_State *L, int type )
{
    lua_pushstring( L, "class environment has to be a real object (got " );
    lua_pushstring( L, lua_typename( L, type ) );
    lua_pushstring( L, ")" );
    lua_concat( L, 3 );
    lua_error( L );
}

static int classmethod_envPutFront( lua_State *L )
{
    ConstValueAddress topItem = index2constadr( L, -1 );

    if ( !iscollectable( topItem ) )
    {
        luaJ_envtypeerror( L, ttype( topItem ) );
    }

    ((CClosureMethodBase*)curr_func( L ))->m_class->EnvPutFront( L );
    return 0;
}

static int classmethod_envPutBack( lua_State *L )
{
    ConstValueAddress topItem = index2constadr( L, -1 );

    if ( !iscollectable( topItem ) )
    {
        luaJ_envtypeerror( L, ttype( topItem ) );
    }

    CClosureMethodBase *cl = (CClosureMethodBase*)curr_func( L );

    cl->m_class->EnvPutBack( L );
    return 0;
}

static int classmethod_envAcquireDispatcher( lua_State *L )
{
    Dispatch *env;

    ConstValueAddress dispatchEnv = index2constadr( L, 1 );

    if ( iscollectable( dispatchEnv ) )
        env = ((CClosureMethodBase*)curr_func( L ))->m_class->AcquireEnvDispatcherEx( L, gcvalue( dispatchEnv ) );
    else
        env = ((CClosureMethodBase*)curr_func( L ))->m_class->AcquireEnvDispatcher( L );

    pushqvalue( L, env );

    // Since the environment is on the stack, we can dereference it.
    env->DereferenceGC( L );

    return 1;
}

static int classmethod_setChild( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TCLASS );

    ConstValueAddress theChildVal = index2constadr( L, 1 );

    Class& j = *jvalue( theChildVal );

    if ( j.parent != ((CClosureMethodBase*)curr_func( L ))->m_class )
    {
        pushbvalue( L, false );
    }
    else
    {
        pushjvalue( L, j.childAPI );
    }
    return 1;
}

static const luaL_Reg internStorage_interface[] =
{
    { "registerForcedSuper", classmethod_registerForcedSuper },
    { "setChild", classmethod_setChild },
    { nullptr, nullptr }
};

static const luaL_Reg internStorage_envDispatch_interface[] =
{
    { "envPutFront", classmethod_envPutFront },
    { "envPutBack", classmethod_envPutBack },
    { "envAcquireDispatcher", classmethod_envAcquireDispatcher },
    { nullptr, nullptr }
};

static int classmethod_fsDestroyRoot( lua_State *L )
{
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 1 ) );

    Class& j = *jvalue( jval );

    if ( j.inMethod != 0 )
    {
        j.reqDestruction = true;
        return 0;
    }

    if ( j.destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to destroy a destroyed class" );

    if ( !class_preDestructor( L, j ) )
        return 0;

    lua_yield_shield _ref( L ); // prevent destructor yielding

    ConstValueAddress clval = index2constadr( L, lua_upvalueindex( 2 ) );

    class_runDestructor( L, j, clvalue( clval ) );
    return 0;
}

static int classmethod_fsDestroySuper( lua_State *L )
{
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 2 ) );

    Class& j = *jvalue( jval );

    if ( j.inMethod != 0 )
    {
        j.reqDestruction = true;
        return 0;
    }

    if ( j.destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempted to destroy a destroyed class" );

    if ( !class_preDestructor( L, j ) )
        return 0;

    lua_yield_shield _ref( L ); // prevent destructor yielding

    ConstValueAddress clval1 = index2constadr( L, lua_upvalueindex( 3 ) );
    ConstValueAddress clval2 = index2constadr( L, lua_upvalueindex( 1 ) );

    class_runDestructor( L, j, clvalue( clval1 ) );
    class_runDestructor( L, j, clvalue( clval2 ) );
    return 0;
}

static int classmethod_fsDestroyBridge( lua_State *L )
{
    // No check required, internal method
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 3 ) );

    Class *j = jvalue( jval );

    ConstValueAddress clval1 = index2constadr( L, lua_upvalueindex( 1 ) );
    ConstValueAddress clval2 = index2constadr( L, lua_upvalueindex( 2 ) );

    class_runDestructor( L, *j, clvalue( clval1 ) );
    class_runDestructor( L, *j, clvalue( clval2 ) );
    return 0;
}

static Closure* classmethod_fsDestroyHandler( lua_State *L, Closure *newMethod, Class *j, Closure *prevMethod )
{
    // We drop prevMethod, since we apply a guardian before the destructor
    Closure *prevDest = j->destructor;

    // Contruct the bridge
    CClosureBasic *cl = luaF_newCclosure( L, 3, j->env );
    setclvalue( L, &cl->upvalues[0], newMethod );
    setclvalue( L, &cl->upvalues[1], prevDest );
    setjvalue( L, &cl->upvalues[2], j );
    cl->f = classmethod_fsDestroyBridge;
    cl->isEnvLocked = true;

    j->destructor = cl;
    luaC_forceupdatef( L, cl );

    // Since the closure is stored on the class already, we can dereference it.
    cl->DereferenceGC( L );

    // The actual new constructor we apply to the class
    CClosureBasic *retCl = luaF_newCclosure( L, 3, j->env );
    setclvalue( L, &retCl->upvalues[0], prevDest );
    setjvalue( L, &retCl->upvalues[1], j );
    setclvalue( L, &retCl->upvalues[2], newMethod );
    retCl->f = classmethod_fsDestroySuper;
    retCl->isEnvLocked = true;
    return retCl;
}

static Closure* classmethod_fsDestroyHandlerNative( lua_State *L, lua_CFunction proto, Class *j, Closure *prevMethod, _methodRegisterInfo& info )
{
    CClosureBasic *cl = luaF_newCclosure( L, info.numUpValues, j->env );
    unsigned char num = info.numUpValues;

    L->GetCurrentStackFrame().CrossCopyTopExtern( L, L->rtStack, cl->upvalues, num );

    cl->f = proto;
    cl->isEnvLocked = true;

    Closure *actualMethod = classmethod_fsDestroyHandler( L, cl, j, prevMethod );

    // The closure should be stored inside actualMethod now, so lets dereference it.
    cl->DereferenceGC( L );

    return actualMethod;
}

void ClassMethodDispatch::Index( lua_State *L, ConstValueAddress& key, ValueAddress& val )
{
    if ( !m_class->IsDestroyed() )
    {
        if ( class_obtainPrivateEnvValue( L, m_class, key, val ) )
            return;
    }

    m_prevEnv->Index( L, key, val );
}

void ClassMethodDispatch::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    class_checkEnvIndex( L, m_class );

    m_class->env->NewIndex( L, key, val );
}

Dispatch* Class::AcquireEnvDispatcherEx( lua_State *L, GCObject *curEnv )
{
    // If the environment matches any of the inherited ones, we skip creating a dispatcher
    if ( curEnv == env )
    {
        env->ReferenceGC( L );
        return env;
    }

    for ( envList_t::const_iterator iter = envInherit.begin(); iter != envInherit.end(); iter++ )
    {
        if ( curEnv == *iter )
        {
            env->ReferenceGC( L );
            return env;
        }
    }

    // Create the dispatcher which accesses class env first then curEnv
    return luaQ_newclassmethodenv( L, this, curEnv );
}

Dispatch* Class::AcquireEnvDispatcher( lua_State *L )
{
    ConstValueAddress envval = index2constadr( L, LUA_ENVIRONINDEX );

    return AcquireEnvDispatcherEx( L, gcvalue( envval ) );
}

Closure* Class::GetMethod( lua_State *L, TString *name, Table*& table )
{
    {
        ConstValueAddress val = luaH_getstr( L, internStorage, name );

        if ( ttype( val ) == LUA_TFUNCTION )
        {
            table = internStorage;

            Closure *retCl = clvalue( val );

            // We reference it, as we return it.
            retCl->ReferenceGC( L );

            return retCl;
        }
    }

    {
        ConstValueAddress val = luaH_getstr( L, storage, name );

        if ( ttype( val ) == LUA_TFUNCTION )
        {
            table = storage;

            Closure *retCl = clvalue( val );

            // We reference it, as we return it.
            retCl->ReferenceGC( L );

            return retCl;
        }
    }

    if ( methodCache )
    {
        STableItemHeader *node = methodCache->FindNode( name );

        if ( node )
        {
            ClassMethodRegister *reg = (ClassMethodRegister*)methodCache->GetNodeValue( node );
            table = reg->GetMethodTable();

            Closure *cl = reg->Create( L, this );
            methodCache->DeleteNode( L, node );     // remove as it no longer serves a purpose

            // The closure is referenced, because it was just created.

            return cl;
        }
    }

    table = storage;
    return nullptr;
}

void Class::SetMethod( lua_State *L, TString *name, Closure *method, Table *table )
{
    bool methWhite = iswhite( method ) != 0;

    ValueAddress methodPtr = luaH_setstr( L, table, name );

    setclvalue( L, methodPtr, method );

    if ( methWhite && isblack( table ) )
    {
        luaC_barrierback( L, table );
    }
}

AINLINE void Class::RegisterMethod( lua_State *L, TString *methName, bool handlers )
{
    Table *methTable;
    Closure *prevMethod = GetMethod( L, methName, methTable ); // Acquire the previous method

    try
    {
        // Get the method closure and check whether we can apply it
        Closure *cl = nullptr;
        {
            ConstValueAddress topAddr = index2constadr( L, -1 );

            cl = clvalue( topAddr );
        }

        // We cannot apply the method if the environment is locked.
        // It usually is an indicator that the closure is a class method already.
        if ( cl->IsEnvLocked() )
            throw lua_exception( L, LUA_ERRRUN, "attempt to set a class method whose environment is locked" );

        // Apply the environment to the new method
        cl->env = AcquireEnvDispatcherEx( L, cl->env );
        luaC_objbarrier( L, cl, cl->env );

        // Since the Dispatch object is now stored in the closure, we can dereference it.
        cl->env->DereferenceGC( L );

        // Lock its environment so the scripter cannot break the class system.
        cl->isEnvLocked = true;

        Closure *handler;

        bool hasItem = false;

        if ( handlers )
        {
            Class::forceSuperItem *item = forceSuper->GetItem( methName );

            if ( item && item->cb )
            {
                handler = item->cb( L, cl, this, prevMethod );

                hasItem = true;
            }
        }

	    if ( !hasItem )
	    {
            if ( prevMethod )
            {
                CClosureMethodRedirectSuper *redirect = luaF_newCmethodredirectsuper( L, env, cl, this, prevMethod );
                redirect->f = classmethod_super;

                handler = redirect;
            }
            else
            {
                CClosureMethodRedirect *redirect = luaF_newCmethodredirect( L, env, cl, this );
                redirect->f = classmethod_root;

                handler = redirect;
            }
	    }

        try
        {
            // Store the new method
            SetMethod( L, methName, handler, methTable );
        }
        catch( ... )
        {
            // Setting the method into the class can actually fail.
            // We have to account for that.
            handler->DereferenceGC( L );
            throw;
        }

        // Since the new method has been stored now, we can dereference it.
        handler->DereferenceGC( L );
    }
    catch( ... )
    {
        // Make sure we really free this reference.
        if ( prevMethod )
        {
            prevMethod->DereferenceGC( L );
        }
        throw;
    }

    if ( prevMethod )
    {
        // Since we stored the previous method in the closure, we can dereference it.
        prevMethod->DereferenceGC( L );
    }

    // Pop the raw function
    popstack( L, 1 );
}

AINLINE Closure* class_createBaseMethod( lua_State *L, lua_CFunction proto, Closure *prevMethod, unsigned char numUpValues, GCObject *env, Class *j )
{
    CClosureMethod *meth = luaF_newCmethod( L, numUpValues, env, j );

    if ( prevMethod )
    {
        meth->super = prevMethod;
        meth->f = classmethod_superNative;
    }
    else
    {
        meth->super = nullptr;
        meth->f = classmethod_rootNative;
    }

    L->GetCurrentStackFrame().CrossCopyTopExtern( L, L->rtStack, meth->upvalues, numUpValues );

    meth->method = proto;
    return meth;
}

AINLINE Closure* class_createMethodTrans( lua_State *L, lua_CFunction proto, Closure *prevMethod, unsigned char numUpValues, GCObject *env, Class *j, int trans )
{
    CClosureMethodTrans *meth = luaF_newCmethodtrans( L, numUpValues, env, j, trans );

    if ( prevMethod )
    {
        meth->super = prevMethod;
        meth->f = classmethod_superTransNative;
    }
    else
    {
        meth->super = nullptr;
        meth->f = classmethod_rootTransNative;
    }

    L->GetCurrentStackFrame().CrossCopyTopExtern( L, L->rtStack, meth->upvalues, numUpValues );

    meth->method = proto;
    return meth;
}

template <bool handlers = true>
class StatelessMethodRegister : public ClassMethodRegister
{
public:
    StatelessMethodRegister( lua_CFunction proto, Table *methTable ) : ClassMethodRegister( proto, methTable )
    {
    }

    Closure* Create( lua_State *L, Class *j ) const
    {
        return class_createBaseMethod( L, m_proto, nullptr, 0, j->env, j );
    }

    void    Call( lua_State *L, Class *j, int argCount ) const
    {
        class_checkDestroyedMethodCall( L, j );

        BaseCall( L, argCount );
    }

    size_t  GetSize( void ) const
    {
        return sizeof(*this);
    }
};

template <bool handlers = true>
class TransMethodRegister : public ClassMethodRegister
{
public:
    TransMethodRegister( lua_CFunction proto, Table *methTable, int transID ) : ClassMethodRegister( proto, methTable ), m_transID( transID )
    {
    }

    Closure* Create( lua_State *L, Class *j ) const
    {
        // Optimization: do not create the closure if the class does not have the type anymore.
        if ( !j->IsTransmit( m_transID ) )
            return nullptr;

        return class_createMethodTrans( L, m_proto, nullptr, 0, j->env, j, m_transID );
    }

    // TODO: Need to work on the transition data availability for this to work.
    // Currently we grab transition data from a special closure class.
    // Need to change it in a special way.
    void    Call( lua_State *L, Class *j, int argCount ) const
    {
        class_checkDestroyedMethodCall( L, j );
        //class_checkTransitionType( L, j, m_transID );

        BaseCall( L, argCount );
    }

    size_t  GetSize( void ) const
    {
        return sizeof(*this);
    }

    int m_transID;
};

AINLINE void class_setMethod( lua_State *L, Class *j, TString *methName, lua_CFunction proto, Closure *prevMethod, Table *methTable, _methodRegisterInfo& info, bool handlers = false )
{
    Closure *handler;

    bool hasItem = false;

    if ( handlers )
    {
        Class::forceSuperItem *item = j->forceSuper->GetItem( methName );

        if ( item && item->cbNative )
        {
            handler = item->cbNative( L, proto, j, prevMethod, info );

            hasItem = true;
        }
    }

	if ( !hasItem )
	{
        unsigned char num = info.numUpValues;

        if ( num == 0 && !prevMethod )
        {
            class_allocateRegisterCache( L, j );

            if ( handlers )
            {
                if ( info.isTrans )
                    new (L, j->methodCache, methName) TransMethodRegister <true> ( proto, methTable, info.transID );
                else
                    new (L, j->methodCache, methName) StatelessMethodRegister <true> ( proto, methTable );
            }
            else
            {
                if ( info.isTrans )
                    new (L, j->methodCache, methName) TransMethodRegister <false> ( proto, methTable, info.transID );
                else
                    new (L, j->methodCache, methName) StatelessMethodRegister <false> ( proto, methTable );
            }

            // Keep alive the crucial methName
            luaC_stringmark( L, methName );

            // Do not continue registration process.
            // Like a coroutine we are halting the registration process here
            // to continue it on demand.
            // It is a very optimized coroutine, as there is no stack.
            // * I had an idea about creation of optimizable coroutines + prototypes.
            return;
        }
        else
        {
            if ( info.isTrans )
                handler = class_createMethodTrans( L, proto, prevMethod, num, j->env, j, info.transID );
            else
                handler = class_createBaseMethod( L, proto, prevMethod, num, j->env, j );
        }
	}

    try
    {
        // Store the new method
        j->SetMethod( L, methName, handler, methTable );
    }
    catch( ... )
    {
        // Settings things into the table can yield in an exception, so be careful.
        handler->DereferenceGC( L );
        throw;
    }

    // We can now dereference the new method.
    handler->DereferenceGC( L );
}

AINLINE void Class::RegisterMethod( lua_State *L, TString *methName, lua_CFunction proto, _methodRegisterInfo& info, bool handlers )
{
    Table *methTable;
    Closure *prevMethod = GetMethod( L, methName, methTable ); // Acquire the previous method

    try
    {
        class_setMethod( L, this, methName, proto, prevMethod, methTable, info, handlers );
    }
    catch( ... )
    {
        if ( prevMethod )
        {
            prevMethod->DereferenceGC( L );
        }

        throw;
    }

    // Since the previous method is supposed to be stored in the method, we can dereference it now.
    if ( prevMethod )
    {
        prevMethod->DereferenceGC( L );
    }

    // Pop the upvalues
    popstack( L, info.numUpValues );
}

AINLINE void Class::RegisterMethodAt( lua_State *L, TString *methName, lua_CFunction proto, Table *methodTable, _methodRegisterInfo& info, bool handlers )
{
    Table *methTable;
    Closure *prevMethod = GetMethod( L, methName, methTable );

    try
    {
        // Use the user-specified methodTable
        class_setMethod( L, this, methName, proto, prevMethod, methodTable, info, handlers );
    }
    catch( ... )
    {
        if ( prevMethod )
        {
            prevMethod->DereferenceGC( L );
        }

        throw;
    }

    // We have to dereference here to prevent an object leak.
    if ( prevMethod )
    {
        prevMethod->DereferenceGC( L );
    }

    // Pop the upvalues
    popstack( L, info.numUpValues );
}

void Class::RegisterMethod( lua_State *L, const char *name, bool handlers )
{
    // This method is now secure since Lua objects are always alive by default due to supplemental GC ref counting.

    TString *nameString = luaS_new( L, name );

    try
    {
        RegisterMethod( L, nameString, handlers );
    }
    catch( ... )
    {
        // Even class method registration is allowed to throw exceptions, so make it safe.
        nameString->DereferenceGC( L );
        throw;
    }

    nameString->DereferenceGC( L );
}

void Class::RegisterMethod( lua_State *L, const char *name, lua_CFunction proto, int nupval, bool handlers )
{
    _methodRegisterInfo info;
    info.numUpValues = nupval;

    TString *nameString = luaS_new( L, name );

    try
    {
        RegisterMethod( L, nameString, proto, info, handlers );
    }
    catch( ... )
    {
        nameString->DereferenceGC( L );
        throw;
    }

    nameString->DereferenceGC( L );
}

void Class::RegisterInterface( lua_State *L, const luaL_Reg *intf, int nupval, bool handlers )
{
    _methodRegisterInfo info;
    info.numUpValues = nupval;

    while ( const char *name = intf->name )
    {
        for ( int n = 0; n < nupval; n++ )
        {
            lua_pushvalue( L, -nupval );
        }

        TString *nameString = luaS_new( L, name );

        try
        {
            RegisterMethod( L, nameString, intf->func, info, handlers );
        }
        catch( ... )
        {
            // I do not care whether you use exception handling or not.
            // The code has to be valid.
            nameString->DereferenceGC( L );
            throw;
        }

        nameString->DereferenceGC( L );

        intf++;
    }
}

void Class::RegisterInterfaceAt( lua_State *L, const luaL_Reg *intf, int nupval, Table *methodTable, bool handlers )
{
    _methodRegisterInfo info;
    info.numUpValues = nupval;

    while ( const char *name = intf->name )
    {
        for ( int n = 0; n < nupval; n++ )
        {
            lua_pushvalue( L, -nupval );
        }

        TString *nameString = luaS_new( L, name );

        try
        {
            RegisterMethodAt( L, nameString, intf->func, methodTable, info, handlers );
        }
        catch( ... )
        {
            // Otherwise we end up with code that is deemed valid because it works like 90% of the time.
            nameString->DereferenceGC( L );
            throw;
        }

        nameString->DereferenceGC( L );

        intf++;
    }
}

void Class::RegisterMethodTrans( lua_State *L, const char *name, lua_CFunction proto, int nupval, int trans, bool handlers )
{
    _methodRegisterInfo info;
    info.isTrans = true;
    info.transID = trans;
    info.numUpValues = nupval;

    TString *nameString = luaS_new( L, name );

    try
    {
        RegisterMethod( L, nameString, proto, info, handlers );
    }
    catch( ... )
    {
        // That was the problem of the original Lua 5.1 implementation. It sucks.
        nameString->DereferenceGC( L );
        throw;
    }

    nameString->DereferenceGC( L );
}

void Class::RegisterInterfaceTrans( lua_State *L, const luaL_Reg *intf, int nupval, int trans, bool handlers )
{
    _methodRegisterInfo info;
    info.isTrans = true;
    info.transID = trans;
    info.numUpValues = nupval;

    while ( intf->name )
    {
        for ( int n = 0; n < nupval; n++ )
        {
            lua_pushvalue( L, -nupval );
        }

        TString *nameString = luaS_new( L, intf->name );

        try
        {
            RegisterMethod( L, nameString, intf->func, info, handlers );
        }
        catch( ... )
        {
            nameString->DereferenceGC( L );
            throw;
        }

        nameString->DereferenceGC( L );

        intf++;
    }
}

void Class::RegisterInterfaceTransAt( lua_State *L, const luaL_Reg *intf, int nupval, int trans, Table *methodTable, bool handlers )
{
    _methodRegisterInfo info;
    info.isTrans = true;
    info.transID = trans;
    info.numUpValues = nupval;

    while ( intf->name )
    {
        for ( int n = 0; n < nupval; n++ )
        {
            lua_pushvalue( L, -nupval );
        }

        TString *nameString = luaS_new( L, intf->name );

        try
        {
            RegisterMethodAt( L, nameString, intf->func, methodTable, info, handlers );
        }
        catch( ... )
        {
            nameString->DereferenceGC( L );
            throw;
        }

        nameString->DereferenceGC( L );

        intf++;
    }
}

void Class::RegisterLightMethod( lua_State *L, const char *_name )
{
    // It is now a secure string method :3
    // Thanks to supplemental GC ref counting.

    // Light methods go directly into the environment
    TString *name = luaS_new( L, _name );

    try
    {
        Closure *cl = nullptr;
        {
            ConstValueAddress topItem = index2constadr( L, -1 );

            cl = clvalue( topItem );
        }

        // Pop the closure from the top.
        popstack( L, 1 );

        // They go into storage only
        SetMethod( L, name, cl, storage );
    }
    catch( ... )
    {
        name->DereferenceGC( L );
        throw;
    }

    // We do not need the name anymore.
    name->DereferenceGC( L );
}

void Class::RegisterLightMethodTrans( lua_State *L, const char *name, int trans )
{

}

static int classmethod_lightguard( lua_State *L )
{
    // Check for security
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 2 ) );

    Class& j = *jvalue( jval );

    if ( j.destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to access a destroyed class' light method" );

    ConstValueAddress lightfunc = index2constadr( L, lua_upvalueindex( 3 ) );

    return ((lua_CFunction)uvalue( lightfunc ))( L );
}

void Class::RegisterLightInterface( lua_State *L, const luaL_Reg *intf, void *udata )
{
    while ( intf->name )
    {
        pushpvalue( L, udata );
        pushjvalue( L, this );
        pushpvalue( L, (void*)intf->func );
        lua_pushcclosure( L, classmethod_lightguard, 3 );

        // Apply environment dispatching
        Closure *cl = nullptr;
        {
            ConstValueAddress topItem = index2constadr( L, -1 );

            cl = clvalue( topItem );
        }
        cl->env = AcquireEnvDispatcherEx( L, cl->env );
        luaC_objbarrier( L, cl, cl->env );

        // Since the Dispatch object is now stored inside of the closure, we can dereference it.
        cl->env->DereferenceGC( L );

        RegisterLightMethod( L, intf->name );

        intf++;
    }
}

void Class::RegisterLightInterfaceTrans( lua_State *L, const luaL_Reg *intf, void *udata, int trans )
{
    // TODO: implement this?
}

void Class::EnvPutFront( lua_State *L )
{
    {
        ConstValueAddress topItem = index2constadr( L, -1 );

        envInherit.insert( envInherit.begin(), gcvalue( topItem ) );
    }
    popstack( L, 1 );
}

void Class::EnvPutBack( lua_State *L )
{
    {
        ConstValueAddress topItem = index2constadr( L, -1 );

        envInherit.push_back( gcvalue( topItem ) );
    }
    popstack( L, 1 );
}

static int classmethod_reference( lua_State *L )
{
    Class& j = *((CClosureMethodBase*)curr_func( L ))->m_class;

    if ( j.destroying )
        return 0;

    j.inMethod++;
    j.refCount++;
    return 0;
}

static int classmethod_dereference( lua_State *L )
{
    Class& j = *((CClosureMethodBase*)curr_func( L ))->m_class;

    if ( j.refCount == 0 )
        return 0;

    j.inMethod--;
    j.refCount--;

    j.CheckDestruction( L );
    return 0;
}

static int classmethod_isValidChild( lua_State *L )
{
    // TODO: maybe use RTII?
    pushbvalue( L,
        lua_type( L, 1 ) == LUA_TCLASS
    );
    return 1;
}

static int classmethod_getChildren( lua_State *L )
{
    Class& j = *((CClosureMethodBase*)curr_func( L ))->m_class;

    Table *tab = luaH_new( L, j.childCount, 0 );

    try
    {
        unsigned int n = 1;

        LIST_FOREACH_BEGIN( Class, j.children.root, child_iter )
            ValueAddress numObj = luaH_setnum( L, tab, n++ );

            setjvalue( L, numObj, item );
            luaC_objbarriert( L, tab, item );
        LIST_FOREACH_END

        pushhvalue( L, tab );
    }
    catch( ... )
    {
        tab->DereferenceGC( L );
        throw;
    }

    tab->DereferenceGC( L );
    return 1;
}

static int childapi_destroy( lua_State *L )
{
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 1 ) );

    Class& j = *jvalue( jval );

    LIST_REMOVE( j.child_iter ); // Child
    j.parent->childCount--;  // Parent

    // We do not have to remove the childAPI from internStorage. Shall we?
    // Remove the parent link
    j.parent = nullptr;

    // Tell this event to any possible registree
    // We use a seperate function so we can post this message after unlinking from
    // the parent's children!
    const TValue *method = j.childAPI->GetEnvValueString( L, "notifyDestroy" );

    if ( ttype( method ) != LUA_TNIL )
    {
        pushtvalue( L, method );
        lua_call( L, 0, 0 );
    }

    return 0;
}

static const luaL_Reg childapi_interface[] =
{
    { "destroy", childapi_destroy },
    { nullptr, nullptr }
};

static int childapi_constructor( lua_State *L )
{
    luaJ_basicextend( L );

    // The destruction has to be notified to every registree
    // Overwriting the method would otherwise bug the class children runtime
    // since parents want to be notified about their child's unlink event
    // so they can safely destroy themselves
    lua_getfield( L, LUA_ENVIRONINDEX, "registerForcedSuper" );
    lua_pushlstring( L, "notifyDestroy", 13 );
    lua_call( L, 1, 0 );

    lua_pushvalue( L, LUA_ENVIRONINDEX );
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    luaL_openlib( L, nullptr, childapi_interface, 1 );
    return 0;
}

static int classmethod_setParent( lua_State *L )
{
    Class& j = *((CClosureMethodBase*)curr_func( L ))->m_class;

    if ( j.destroying )
    {
        pushbvalue( L, false );
        return 1;
    }

    if ( lua_type( L, 1 ) != LUA_TCLASS )
    {
        // If we have not been passed a valid argument, i.e nil, remove our parent
        if ( j.parent )
        {
            j.childAPI->PushMethod( L, "destroy" );
            lua_call( L, 0, 0 );

            j.childAPI->DecrementMethodStack( L );
        }

        lua_pushboolean( L, true );
        return 1;
    }

    // We want our class as argument only
    lua_settop( L, 1 );

    ConstValueAddress classVal = index2constadr( L, 1 );

    Class& c = *jvalue( classVal );

    if ( &c == &j || c.destroying || c.destroyed )
    {
        pushbvalue( L, false );
        return 1;
    }

    // Prevent circle jerks or pervert child relationships
    if ( c.IsRootedIn( &j ) )
    {
        pushbvalue( L, false );
        return 1;
    }

    c.PushMethod( L, "isValidChild" );
    pushjvalue( L, &j );
    lua_call( L, 1, 1 );

    if ( !lua_toboolean( L, 2 ) )
    {
        pushbvalue( L, false );
        return 1;
    }

    // Pop the boolean from stack.
    popstack( L, 1 );

    if ( j.parent )
    {
        j.childAPI->DecrementMethodStack( L );

        j.childAPI->PushMethod( L, "destroy" );
        lua_call( L, 0, 0 );
    }

    // ???
    popstack( L, 1 );

    pushjvalue( L, &j );
    lua_pushcclosure( L, childapi_constructor, 1 );
    lua_newclassex( L, LCLASS_API_NOENVDISPATCH | LCLASS_NOPARENTING );

    // Get the childapi.
    Class *childAPI = nullptr;
    {
        ConstValueAddress topItem = index2constadr( L, -1 );

        childAPI = jvalue( topItem );
    }

    // Store the API
    j.parent = &c;
    j.childAPI = childAPI;
    j.childAPI->IncrementMethodStack( L );

    // Allow internal storage access to it
    {
        TString *childAPIString = luaS_new( L, "childAPI" );

        ValueAddress stm = luaH_setstr( L, j.internStorage, childAPIString );

        // We do not need the key string anymore.
        childAPIString->DereferenceGC( L );

        setjvalue( L, stm, j.childAPI );
        luaC_objbarriert( L, j.internStorage, j.childAPI );
    }

    // Call setChild in protected mode as the process may not be interupted anymore
    c.PushMethod( L, "setChild" );
    pushjvalue( L, &j );
    lua_pcall( L, 1, 0, 0 );

    // Add it to the parent's children list
    LIST_APPEND( c.children.root, j.child_iter );
    c.childCount++;

    pushbvalue( L, true );
    return 1;
}

static int classmethod_getParent( lua_State *L )
{
    Class *j = ((CClosureMethodBase*)curr_func( L ))->m_class;

    if ( !j->parent )
    {
        pushbvalue( L, false );
        return 1;
    }

    pushjvalue( L, j->parent );
    return 1;
}

static int classmethod_destructor( lua_State *L )
{
    ConstValueAddress jval = index2constadr( L, lua_upvalueindex( 1 ) );

    Class *j = jvalue( jval );

    // Free unnecessary references.
    j->env = nullptr;
    j->outenv = nullptr;
    j->storage = nullptr;
    j->internStorage = nullptr;
    j->destructor = nullptr;

    if ( ClassStringTable *methodCache = j->methodCache )
    {
        methodCache->Clear( L );

        methodCache->~DynamicStringTable();

        methodCache->deleteMem( L );

        j->methodCache = nullptr;
    }

    if ( Class::ForceSuperTable *forceSuper = j->forceSuper )
    {
        forceSuper->Clear( L );

        forceSuper->~StringTable();

        forceSuper->deleteMem( L );

        j->forceSuper = nullptr;
    }

    j->destroyed = true;
    j->destroying = false;
    j->reqDestruction = false;
    return 0;
}

static const luaL_Reg classmethods_c[] =
{
    { "reference", classmethod_reference },
    { "dereference", classmethod_dereference },
    { "isValidChild", classmethod_isValidChild },
    { "getChildren", classmethod_getChildren },
    { nullptr, nullptr }
};

static const luaL_Reg classmethods_parenting[] =
{
    { "getParent", classmethod_getParent },
    { "setParent", classmethod_setParent },
    { nullptr, nullptr }
};

void ClassEnvDispatch::NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val )
{
    Class& j = *m_class;

    // If the class leaked it's environment, outer range code might try to modify the class during obscure scenarios
    // We do not want that, as modification of a destroyed class is obscure!
    if ( j.destroyed )
        throw lua_exception( L, LUA_ERRRUN, "attempt to modify a destroyed class" );

    if ( ttype( val ) == LUA_TFUNCTION )
    {
        if ( ttype( key ) == LUA_TSTRING )
        {
            // Register the method
            pushtvalue( L, val );
            j.RegisterMethod( L, tsvalue( key ), true );
        }
    }
    else
    {
        // Check whether we have a method with this key in the storage.
        // If so, we cannot write to this key because its protected/hidden.
        {
            ConstValueAddress storageValue = luaH_get( L, j.storage, key );

            if ( ttype( storageValue ) == LUA_TFUNCTION )
            {
                throw lua_exception( L, LUA_ERRRUN, "class methods cannot be overwritten" );
            }
        }

        // Make sure we do not hit delayed method writes
        class_checkMethodDelayOverride( L, m_class, key );

        // Apply a regular member
        ValueAddress storageItemPtr = luaH_set( L, j.storage, key );

        setobj( L, storageItemPtr, val );
    }
}

class ClassConstructionAllocation
{
public:
    ClassConstructionAllocation( lua_State *L, Class *j )
    {
        pushjvalue( L, j );
        m_id = luaL_ref( L, LUA_REGISTRYINDEX );
        m_thread = L;
    }

    ~ClassConstructionAllocation( void )
    {
        luaL_unref( m_thread, LUA_REGISTRYINDEX, m_id );
    }

private:
    lua_State*  m_thread;
    int         m_id;
};

Class::Class( global_State *g, void *construction_params ) : GCObject( g )
{
    // Link it into the GC system
    luaC_linktmu( g, this, LUA_TCLASS );

    this->env = nullptr;
    this->outenv = nullptr;
    this->storage = nullptr;
    this->internStorage = nullptr;
    this->childAPI = nullptr;

    this->forceSuper = nullptr;
    this->methodCache = nullptr;   // only allocate once required

    this->destroyed = false;
    this->destroying = false;
    this->reqDestruction = false;
    this->inMethod = 0;
    this->refCount = 0;
    this->trans = nullptr;
	this->transCount = 0;
    this->parent = nullptr;
    this->childCount = 0;
    this->destructor = nullptr;
}

Class::Class( const Class& right ) : GCObject( right )
{
    // TODO: think about how to clone a class.
    throw lua_exception( this->gstate->mainthread, LUA_ERRRUN, "attempt to clone a runtime class", 1 );
}

Class::~Class( void )
{
    global_State *g = this->gstate;

    // Free runtime data.
    lua_assert( this->inMethod == 0 );

    if ( void *trans = this->trans )
    {
        luaM_realloc__( g, trans, this->transCount * sizeof(Class::trans_t), 0 );
    }

    // Unregister from GC.
    luaC_unlinktmu( g, this );
}

Class* luaJ_new( lua_State *L, int nargs, unsigned int flags )
{
    global_State *g = G(L);

    lua_config *cfg = g->config;

    // Attempt to get the namespace class type info interface.
    namespaceClassTypeInfo *typeInfo = classTypeInfoPlugin.GetPluginStruct( cfg );

    // If we cannot, there is no point in constructing classes.
    if ( !typeInfo )
        return nullptr;

    Closure *constructor = nullptr;
    {
        ConstValueAddress topItem = index2constadr( L, -1 );

        constructor = clvalue( topItem );
    }

    // We cannot construct using closure which are, say, class methods already.
    if ( constructor->IsEnvLocked() )
        throw lua_exception( L, LUA_ERRRUN, "attempt to construct a class using a locked closure" );

    // Lock the environment
    constructor->isEnvLocked = true;

    // Allocate the class type object.
    Class *c = lua_new <Class> ( L, typeInfo->classTypeInfo );

    if ( !c )
    {
        return nullptr;
    }

    try
    {
        // Set up the environments
        c->env = luaQ_newclassenv( L, c ); c->env->DereferenceGC( L );
        c->outenv = luaQ_newclassoutenv( L, c ); c->outenv->DereferenceGC( L );
        c->storage = luaH_new( L, 0, 0 ); c->storage->DereferenceGC( L );
        c->internStorage = luaH_new( L, 0, 0 ); c->internStorage->DereferenceGC( L );

        c->forceSuper = new (L) Class::ForceSuperTable();

        lua_pushnil(L);
        popstack( L, 1 );

        // Perform a temporary keep
        ClassConstructionAllocation construction( L, c );

#if 0
        // Register the previous environment
        // Update: this ain't workin' anymore; causing troubles.
        // Was a stupid optimization for memory saving anyway.
        lua_pushvalue( L, LUA_ENVIRONINDEX );
        c->EnvPutBack( L );
#endif

        // Init the forceSuper table
        Class::forceSuperItem destrItem;
        destrItem.cb = classmethod_fsDestroyHandler;
        destrItem.cbNative = classmethod_fsDestroyHandlerNative;

        TString *destroyNameString = luaS_new( L, "destroy" );

        c->forceSuper->SetItem( L, destroyNameString, destrItem );

        // Used the string, so lets dereference it again.
        destroyNameString->DereferenceGC( L );

        // Create a class-only storage
        pushhvalue( L, c->internStorage );

        if ( !( flags & LCLASS_API_NO_ENV_AWARENESS ) )
        {
            // Distribute the environment
            pushqvalue( L, c->env );
            lua_setfield( L, -2, "_ENV" );
            pushqvalue( L, c->outenv );
            lua_setfield( L, -2, "_OUTENV" );

            // Only set the this pointer if required.
            pushjvalue( L, c );
            lua_setfield( L, -2, "this" );
        }

        // Set some internal functions
        c->RegisterInterfaceAt( L, internStorage_interface, 0, c->internStorage );

        if ( !( flags & LCLASS_API_NOENVDISPATCH ) )
        {
            // Include the env dispatcher API
            c->RegisterInterfaceAt( L, internStorage_envDispatch_interface, 0, c->internStorage );
        }

        // Pop the internal storage
        popstack( L, 1 );

        // Register the interface
        if ( !( flags & LCLASS_NOPARENTING ) )
        {
            // Add functionality for Lua parenting
            c->RegisterInterface( L, classmethods_parenting, 0 );
        }

        // Register the light interface
        c->RegisterInterface( L, classmethods_c, 0 );

        pushhvalue( L, c->storage );

        // Prepare the destructor
        pushjvalue( L, c );

        pushjvalue( L, c );
        lua_pushcclosure( L, classmethod_destructor, 1 );

        Closure *theDestructor = nullptr;
        {
            ConstValueAddress topItem = index2constadr( L, -1 );

            theDestructor = clvalue( topItem );
        }

        c->destructor = theDestructor;
        luaC_forceupdatef( L, theDestructor );

        lua_pushcclosure( L, classmethod_fsDestroyRoot, 2 );

        lua_setfield( L, -2, "destroy" );

        popstack( L, 1 );

        // Apply the environment to the constructor
        constructor->env = c->AcquireEnvDispatcherEx( L, constructor->env );
        luaC_objbarrier( L, constructor, c->env );

        // Since the Dispatch is now stored inside of the constructor closure, we can dereference it.
        constructor->env->DereferenceGC( L );

        // Call the constructor (class as first arg)
        pushjvalue( L, c );
        lua_insert( L, -nargs - 1 );

        lua_call( L, nargs + 1, 0 );
    }
    catch( ... )
    {
        c->DereferenceGC( L );

        throw;
    }

    return c;
}

void luaJ_free( lua_State *L, Class *j )
{
    // Delete the class again.
    lua_delete <Class> ( L, j );
}

void luaJ_construct( lua_State *L, int nargs )
{
    Class *j = luaJ_new( L, nargs, 0 );

    if ( j )
    {
        pushjvalue( L, j );

        j->DereferenceGC( L );
    }
    else
    {
        pushnilvalue( L );
    }
}

// Basic protection for classes
static int protect_index( lua_State *L )
{
    size_t len;
    const char *str = lua_tolstring( L, 2, &len );

    if ( len > 1 && *(unsigned short*)str == 0x5f5f )
        return 0;

    lua_getmethodclass( L )->PushOuterEnvironment( L );
    lua_pushvalue( L, 2 );
    lua_gettable( L, 3 );
    return 1;
}

static int protect_newindex( lua_State *L )
{
    return 0;
}

static const luaL_Reg bprotect_methods[] =
{
    { "__index", protect_index },
    { "__newindex", protect_newindex },
    { nullptr, nullptr }
};

void luaJ_basicprotect( lua_State *L )
{
    lua_refclass( L, LUA_ENVIRONINDEX )->RegisterInterface( L, bprotect_methods, 0 );
}

static int extend_handler( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TFUNCTION );

    // Make it class root
    Closure *extender = nullptr;
    {
        ConstValueAddress constructorValue = index2constadr( L, 1 );

        extender = clvalue( constructorValue );
    }

    if ( extender->IsEnvLocked() )
        throw lua_exception( L, LUA_ERRRUN, "cannot run a function with locked environment as class extension" );

    Class *j = ((CClosureMethodBase*)curr_func( L ))->m_class;

    extender->env = j->AcquireEnvDispatcherEx( L, extender->env );

    // Since the Dispatch is now stored in the closure, we can dereference it.
    extender->env->DereferenceGC( L );

    j->Push( L );
    lua_insert( L, 2 );

    lua_call( L, lua_gettop( L ) - 1, 0 );
    return 0;
}

void luaJ_basicextend( lua_State *L )
{
    lua_refclass( L, LUA_ENVIRONINDEX )->RegisterMethod( L, "extend", extend_handler, 0, true );
}

// Module initialization.
void luaJ_init( lua_config *cfg )
{
    return;
}

void luaJ_shutdown( lua_config *cfg )
{
    return;
}

void luaJ_libraryinit( void )
{
    classEnvConnectingBridge.RegisterPluginStruct( namespaceFactory );
}

void luaJ_libraryshutdown( void )
{
    classEnvConnectingBridge.UnregisterPluginStruct();
}

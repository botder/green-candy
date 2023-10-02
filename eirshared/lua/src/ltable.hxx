#ifndef _LUA_TABLE_NATIVE_IMPLEMENTATION_
#define _LUA_TABLE_NATIVE_IMPLEMENTATION_

struct TKey
{
    TValue tvk;
    struct Node *next;  /* for chaining */
};

struct Node
{
    TValue i_val;
    TKey i_key;
};

// Utility macros.
#define twoto(x)	((size_t)1<<(x))

#define gnode(t,i)	(&(t)->node[i])
#define gkey(n)		(&(n)->i_key.tvk)
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->i_key.next)

#define key2tval(n)	(&(n)->i_key.tvk)

#define sizenode(t)	(twoto((t)->lsizenode))

#define ceillog2(x)	(luaO_log2((x)-1) + 1)


#include "lpluginutil.hxx"

#include "lmultithreading.hxx"

inline Node* GetDummyNode( global_State *g );

// Internal native implementation data of the public Lua Table type.
// This is appended to any Table to make it actually do stuff.
// Note that this is not a standard C++ type, use the methods that it exports for struct size and construction!
struct tableNativeImplementation
{
    static inline size_t GetStructSize( global_State *g )
    {
        // If we support multi-threading, we want to include support for locking.
        size_t ourSize = sizeof( tableNativeImplementation );

        if ( g->isMultithreaded )
        {
            globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g );

            if ( mtEnv )
            {
                NativeExecutive::CExecutiveManager *manager = mtEnv->nativeMan;

                if ( manager )
                {
                    ourSize += manager->GetReadWriteLockStructSize();
                }
            }
        }

        return ourSize;
    }

    static inline tableNativeImplementation* GetNativeImplementation( Table *tab )
    {
        return (tableNativeImplementation*)( tab + 1 );
    }

    static inline const tableNativeImplementation* GetConstNativeImplementation( const Table *tab )
    {
        return (const tableNativeImplementation*)( tab + 1 );
    }

private:
    inline void* GetNativeRWLockPointer( void ) const
    {
        return (void*)( this + 1 );
    }

    inline void _construct_lock( global_State *g )
    {
        // Construct the native lock, since we can.
        if ( g->isMultithreaded )
        {
            globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g );

            if ( mtEnv )
            {
                NativeExecutive::CExecutiveManager *nativeMan = mtEnv->nativeMan;

                if ( nativeMan )
                {
                    void *lockMem = this->GetNativeRWLockPointer();

                    nativeMan->CreatePlacedReadWriteLock( lockMem );
                }
            }
        }
    }

public:
    inline tableNativeImplementation( void )
    {
        this->flags = 0;
        this->lsizenode = 0;
        this->array = nullptr;
        this->node = nullptr;
        this->lastfree = nullptr;
        this->sizearray = 0;
    }

    inline tableNativeImplementation( const tableNativeImplementation& right, global_State *g )
    {
        lua_State *L = g->mainthread;

        // Copy data from the source table.
        {
            NativeExecutive::CReadWriteReadContextSafe <> readContext( right.GetRuntimeLock( g ) );

            this->flags = right.flags;

            Node *rightNode = right.node;

            Node *dummyNode = GetDummyNode( g );

            Node *newNodes = rightNode;

            if ( newNodes != dummyNode )
            {
                int nodesize = sizenode(&right);

                newNodes = luaM_newvector <Node> ( L, nodesize );

                for ( int i = 0; i < nodesize; i++ )
                {
                    const Node *srcNode = &rightNode[i];

                    Node *newItem = &newNodes[i];

                    // Decide about node linkage.
                    const Node *srcNextNode = gnext(srcNode);

                    if ( srcNextNode == nullptr )
                    {
                        gnext(newItem) = nullptr;
                    }
                    else
                    {
                        // Link into our array.
                        ptrdiff_t nodeoff = ( srcNextNode - rightNode );

                        gnext(newItem) = &newNodes[ nodeoff ];
                    }

                    // Copy values.
                    setobj( L, gkey(newItem), gkey(srcNode) );
                    setobj( L, gval(newItem), gval(srcNode) );
                }
            }

            this->node = newNodes;
            this->lsizenode = right.lsizenode;

            try
            {
                // Now copy the last free identifier.
                Node *rightLastfree = right.lastfree;

                Node *newLastfree = nullptr;

                if ( rightLastfree != nullptr )
                {
                    // We need to work with offsets.
                    ptrdiff_t lastfreeoff = ( rightLastfree - rightNode );

                    newLastfree = newNodes + lastfreeoff;
                }

                this->lastfree = newLastfree;

                // Now copy the array of TValues.
                size_t sizearray = right.sizearray;

                TValue *newArray = nullptr;

                if ( sizearray > 0 )
                {
                    newArray = luaM_clonevector( g->mainthread, right.array, sizearray );
                }

                this->sizearray = sizearray;
                this->array = newArray;
            }
            catch( ... )
            {
                if ( newNodes && newNodes != dummyNode )
                {
                    int nodesize = sizenode(&right);

                    luaM_freearray( g->mainthread, newNodes, nodesize );
                }

                throw;
            }
        }

        _construct_lock( g );
    }

    inline ~tableNativeImplementation( void )
    {
        lua_assert( this->array == nullptr );
        lua_assert( this->node == nullptr );

        lua_assert( LIST_EMPTY( this->mutableDynamicValues.root ) == true );
        lua_assert( LIST_EMPTY( this->immutableDynamicValues.root ) == true );
    }

    inline void Initialize( global_State *g )
    {
        this->flags = cast_byte(~0);
        /* main table implementation parameters */
        this->array = nullptr;
        this->sizearray = 0;
        this->lsizenode = 0;
        this->node = GetDummyNode( g );
        this->lastfree = nullptr;

        // Construct the native lock, since we can.
        _construct_lock( g );
    }

    inline void Shutdown( global_State *g )
    {
        // Destroy the lock.
        if ( g->isMultithreaded )
        {
            globalStateMultithreadingPlugin *mtEnv = GetGlobalStateMultithreadingEnv( g );

            if ( mtEnv )
            {
                NativeExecutive::CExecutiveManager *nativeMan = mtEnv->nativeMan;

                if ( nativeMan )
                {
                    NativeExecutive::CReadWriteLock *theLock = (NativeExecutive::CReadWriteLock*)this->GetNativeRWLockPointer();

                    nativeMan->ClosePlacedReadWriteLock( theLock );
                }
            }
        }

        // Clean up runtime data of the table.
        lua_State *L = g->mainthread;

        if ( Node *node = this->node )
        {
            if ( node != GetDummyNode( g ) )
            {
                luaM_freearray( L, node, sizenode( this ) );
            }

            this->node = nullptr;
        }

        if ( TValue *array = this->array )
        {
            luaM_freearray( L, array, this->sizearray );

            this->array = nullptr;
        }
    }

    NativeExecutive::CReadWriteLock* GetRuntimeLock( global_State *g ) const
    {
        if ( g->isMultithreaded )
        {
            return (NativeExecutive::CReadWriteLock*)this->GetNativeRWLockPointer();
        }

        return nullptr;
    }

    // Helpers for typical critical regions inside of a Table.
    inline void EnterCriticalReadRegion( global_State *g ) const
    {
        NativeExecutive::CReadWriteLock *theLock = this->GetRuntimeLock( g );

        if ( theLock )
        {
            theLock->EnterCriticalReadRegion();
        }
    }

    inline void LeaveCriticalReadRegion( global_State *g ) const
    {
        NativeExecutive::CReadWriteLock *theLock = this->GetRuntimeLock( g );

        if ( theLock )
        {
            theLock->LeaveCriticalReadRegion();
        }
    }

    inline void EnterCriticalWriteRegion( global_State *g ) const
    {
        NativeExecutive::CReadWriteLock *theLock = this->GetRuntimeLock( g );

        if ( theLock )
        {
            theLock->EnterCriticalWriteRegion();
        }
    }

    inline void LeaveCriticalWriteRegion( global_State *g ) const
    {
        NativeExecutive::CReadWriteLock *theLock = this->GetRuntimeLock( g );

        if ( theLock )
        {
            theLock->LeaveCriticalWriteRegion();
        }
    }

    lu_byte flags;  /* 1<<p means tagmethod(p) is not present */
    std::uint16_t lsizenode;  /* log2 of size of `node' array */
    TValue *array;  /* array part */
    Node *node;
    Node *lastfree;  /* any free position is before this position */
    size_t sizearray;  /* size of `array' array */

    // Table access contexts for TValue reading and writing.
    template <typename valueType, bool updateBarrier>
    struct TableValueAccessContextTemplate : public DataContext <valueType>
    {
        typedef LuaCachedConstructedClassAllocator <TableValueAccessContextTemplate> allocator_t;

        allocator_t *_usedAlloc;

        long refCount;

        // These values have to be set on allocation.
        Table *linkedTable;

        TValue key;
        valueType *value;

        bool hasFetchedValuePointer;

        RwListEntry <TableValueAccessContextTemplate> managerNode;

        inline TableValueAccessContextTemplate( void )
        {
            this->refCount = 0;
            this->_usedAlloc = nullptr;
            this->linkedTable = nullptr;
            setnilvalue( &key );
            this->value = nullptr;
            this->hasFetchedValuePointer = false;
        }

        inline ~TableValueAccessContextTemplate( void )
        {
            lua_assert( this->refCount == 0 );
        }

        void Reference( lua_State *L )
        {
            this->refCount++;

            // maybe do something?
        }

        void Dereference( lua_State *L )
        {
            if ( this->hasFetchedValuePointer )
            {
                if ( updateBarrier )
                {
                    // Update the table barrier.
                    luaC_barriert( L, this->linkedTable, this->value );
                }

                tableNativeImplementation *nativeTable = GetNativeImplementation( this->linkedTable );

                if ( nativeTable )
                {
                    if ( updateBarrier )
                    {
                        nativeTable->LeaveCriticalWriteRegion( G(L) );
                    }
                    else
                    {
                        nativeTable->LeaveCriticalReadRegion( G(L) );
                    }
                }

                this->hasFetchedValuePointer = false;
            }

            if ( this->refCount-- == 1 )
            {
                LIST_REMOVE( this->managerNode );

                this->_usedAlloc->Free( L, this );
            }
        }

        valueType* const* GetValuePointer( lua_State *L )
        {
            if ( this->hasFetchedValuePointer == false )
            {
                this->hasFetchedValuePointer = true;

                tableNativeImplementation *nativeTable = GetNativeImplementation( this->linkedTable );

                if ( nativeTable )
                {
                    global_State *g = G(L);

                    if ( updateBarrier )
                    {
                        nativeTable->EnterCriticalWriteRegion( g );
                    }
                    else
                    {
                        nativeTable->EnterCriticalReadRegion( g );
                    }
                }
            }

            return &value;
        }

        inline void UpdateContextValue( TValue *arrold, Node *nold, TValue *arrnew, Node *nodenew, size_t oldasize, size_t oldnsize, size_t newasize, size_t newnsize )
        {
            // We kind of never hit that during testing sessions.
            // I assume that Tables just do not update their size if they still hold a context value pointer.
            // Why would they anyway? These value pointers are the rarest/most-shortlived in the whole runtime, for good reasons!

            valueType *ourValuePointer = this->value;

            if ( ourValuePointer == luaO_nilobject )
                return;

            bool hasDone = false;

            // Check whether this item is part of the array.
            if ( hasDone == false )
            {
                ptrdiff_t arroff = ( ourValuePointer - arrold );

                if ( arroff >= 0 )
                {
                    size_t pos_arroff = (size_t)arroff;

                    if ( pos_arroff < oldasize && arrold != arrnew )
                    {
                        this->value = ( arrnew + pos_arroff );

                        hasDone = true;
                    }
                }
            }

            if ( hasDone == false )
            {
                // Check whether this item if part of the nodes.
                Node *asNodeValue = (Node*)( (char*)ourValuePointer - offsetof( Node, i_val ) );

                size_t real_oldnsize = twoto( oldnsize );

                ptrdiff_t nodeoff = ( asNodeValue - nold );

                if ( nodeoff >= 0 )
                {
                    size_t pos_nodeoff = (size_t)nodeoff;

                    if ( pos_nodeoff < real_oldnsize && nold != nodenew )
                    {
                        this->value = &( nodenew + pos_nodeoff )->i_val;

                        hasDone = true;
                    }
                }
            }

            // Well, if there had to be something done, I guess we really should have something done!
            lua_assert( !( ( arrold != arrnew || nold != nodenew ) && hasDone == false ) );
        }
    };

    typedef TableValueAccessContextTemplate <TValue, true> TableValueAccessContext;
    typedef TableValueAccessContextTemplate <const TValue, false> TableValueConstAccessContext;

    RwList <TableValueAccessContext> mutableDynamicValues;
    RwList <TableValueConstAccessContext> immutableDynamicValues;
};

typedef tableNativeImplementation::TableValueAccessContext TableValueAccessContext;
typedef tableNativeImplementation::TableValueConstAccessContext TableValueConstAccessContext;

// Table environment plugin.
struct globalStateTableEnvPlugin
{
    inline globalStateTableEnvPlugin( void )
    {
        // Set up the dummy node.
        setnilvalue( &dummynode.i_key.tvk );
        dummynode.i_key.next = nullptr;
        setnilvalue( &dummynode.i_val );
    }

    inline void Initialize( global_State *g )
    {
        return;
    }

    inline void ClearMemory( global_State *g )
    {
        _tvalueAccessAllocator.Shutdown( g );
        _tvalueConstAccessAllocator.Shutdown( g );
    }

    inline void Shutdown( global_State *g )
    {
        ClearMemory( g );
    }

    // Dummy node that is used for optimization.
    Node dummynode;

    // Allocator for TValue access contexts.
    TableValueAccessContext::allocator_t _tvalueAccessAllocator;
    TableValueConstAccessContext::allocator_t _tvalueConstAccessAllocator;
};

typedef PluginConnectingBridge
    <globalStateTableEnvPlugin,
        globalStateDependantStructFactoryMeta <globalStateTableEnvPlugin, globalStateFactory_t, lua_config>,
    namespaceFactory_t>
        tableEnvConnectingBridge_t;

extern tableEnvConnectingBridge_t tableEnvConnectingBridge;

inline globalStateTableEnvPlugin* GetGlobalTableEnv( global_State *g )
{
    return tableEnvConnectingBridge.GetPluginStruct( g->config, g );
}

inline Node* GetDummyNode( global_State *g )
{
    globalStateTableEnvPlugin *globalEnv = GetGlobalTableEnv( g );

    assert( globalEnv != nullptr );

    return &globalEnv->dummynode;
}

inline TableValueAccessContext* NewTableValueAccessContext( lua_State *L, Table *t, tableNativeImplementation *tableImpl )
{
    globalStateTableEnvPlugin *tableEnv = GetGlobalTableEnv( G(L) );

    if ( tableEnv )
    {
        TableValueAccessContext *ctx = tableEnv->_tvalueAccessAllocator.Allocate( L );

        ctx->_usedAlloc = &tableEnv->_tvalueAccessAllocator;
        ctx->linkedTable = t;
        ctx->hasFetchedValuePointer = false;

        LIST_INSERT( tableImpl->mutableDynamicValues.root, ctx->managerNode );

        return ctx;
    }
    return nullptr;
}

inline TableValueConstAccessContext* NewTableValueConstAccessContext( lua_State *L, Table *t, tableNativeImplementation *tableImpl )
{
    globalStateTableEnvPlugin *tableEnv = GetGlobalTableEnv( G(L) );

    if ( tableEnv )
    {
        TableValueConstAccessContext *ctx = tableEnv->_tvalueConstAccessAllocator.Allocate( L );

        ctx->_usedAlloc = &tableEnv->_tvalueConstAccessAllocator;
        ctx->linkedTable = t;
        ctx->hasFetchedValuePointer = false;

        LIST_INSERT( tableImpl->immutableDynamicValues.root, ctx->managerNode );

        return ctx;
    }
    return nullptr;
}

// Table type information plugin.
struct tableTypeInfoPlugin : public LuaTypeSystem::typeInterface
{
    // Type methods.
    void Construct( void *objMem, global_State *g, void *construct_params ) const
    {
        // First construct the table item.
        Table *newTable = new (objMem) Table( g, construct_params );

        // Now that it is constructed, we must remember to destroy it when something goes wrong.
        try
        {
            void *advObjMem = ( newTable + 1 );

            // Construct the advanced object.
            tableNativeImplementation *nativeImpl = new (advObjMem) tableNativeImplementation();

            try
            {
                // Initialize the object.
                nativeImpl->Initialize( g );
            }
            catch( ... )
            {
                nativeImpl->~tableNativeImplementation();

                throw;
            }
        }
        catch( ... )
        {
            newTable->~Table();

            throw;
        }
    }

    void CopyConstruct( void *objMem, const void *srcMem ) const
    {
        const Table *srcTable = (const Table*)srcMem;

        Table *newTable = new (objMem) Table( *srcTable );

        try
        {
            // Also clone the advanced struct.
            void *advObjMem = ( newTable + 1 );

            const void *srcAdvObjMem = ( srcTable + 1 );

            // Copy construct it.
            tableNativeImplementation *nativeImpl =
                new (advObjMem) tableNativeImplementation( *(const tableNativeImplementation*)srcAdvObjMem, srcTable->gstate );

            (void)nativeImpl;

            // Fin.
        }
        catch( ... )
        {
            newTable->~Table();

            throw;
        }
    }

    void Destruct( void *objMem ) const
    {
        Table *theTable = (Table*)objMem;

        // Destroy the advanced object first.
        {
            void *advObjMem = ( theTable + 1 );

            tableNativeImplementation *nativeImpl = (tableNativeImplementation*)advObjMem;

            nativeImpl->Shutdown( theTable->gstate );

            nativeImpl->~tableNativeImplementation();
        }

        // Finally kill our table object.
        ((Table*)objMem)->~Table();
    }

    size_t GetTypeSize( global_State *g, void *construct_params ) const
    {
        return sizeof( Table ) + tableNativeImplementation::GetStructSize( g );
    }

    size_t GetTypeSizeByObject( global_State *g, const void *mem ) const
    {
        return sizeof( Table ) + tableNativeImplementation::GetStructSize( g );
    }

    inline void Initialize( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Register type information.
        tableTypeInfo = typeSys.RegisterType( "table", this, cfg->grayobjTypeInfo );
    }

    inline void Shutdown( lua_config *cfg )
    {
        LuaTypeSystem& typeSys = cfg->typeSys;

        // Delete the types again.
        typeSys.DeleteType( tableTypeInfo );
    }
    LuaTypeSystem::typeInfoBase *tableTypeInfo;
};

extern PluginDependantStructRegister <tableTypeInfoPlugin, namespaceFactory_t> tableTypeInfo;

#endif //_LUA_TABLE_NATIVE_IMPLEMENTATION_

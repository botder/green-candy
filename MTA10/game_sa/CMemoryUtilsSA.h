/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMemoryUtilsSA.h
*  PURPOSE:     Classes for memory abstraction
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _MEMORY_ABSTRACTION_UTILITIES_
#define _MEMORY_ABSTRACTION_UTILITIES_

#include <sdk/MemoryUtils.h>

class defaultGrowableArrayAllocator
{
public:
    inline void* Allocate( size_t memSize, unsigned int flags )
    {
#ifndef _USE_ROCKSTAR_MEMORY_FUNCTIONS
        return malloc( memSize );
#else
        return RenderWare::GetInterface()->m_memory.m_malloc( memSize, flags );
#endif //_USE_ROCKSTAR_MEMORY_FUNCTIONS
    }

    inline void* Realloc( void *memPtr, size_t memSize, unsigned int flags )
    {
#ifndef _USE_ROCKSTAR_MEMORY_FUNCTIONS
        return realloc( memPtr, memSize );
#else
        return RenderWare::GetInterface()->m_memory.m_realloc( memPtr, memSize, flags );
#endif //_USE_ROCKSTAR_MEMORY_FUNCTIONS
    }

    inline void Free( void *memPtr )
    {
#ifndef _USE_ROCKSTAR_MEMORY_FUNCTIONS
        free( memPtr );
#else
        RenderWare::GetInterface()->m_memory.m_free( memPtr );
#endif //_USE_ROCKSTAR_MEMORY_FUNCTIONS
    }
};

template <typename dataType, unsigned int pulseCount, unsigned int allocFlags, typename arrayMan, typename countType>
struct growableArray : growableArrayEx <dataType, pulseCount, allocFlags, arrayMan, countType, defaultGrowableArrayAllocator>
{
};

template <typename dataType>
struct iterativeGrowableArrayManager
{
    AINLINE void InitField( dataType& theField )
    {
        return;
    }
};

template <typename dataType, unsigned int pulseCount, unsigned int allocFlags, typename countType>
struct iterativeGrowableArray : growableArray <dataType, pulseCount, allocFlags, iterativeGrowableArrayManager <dataType>, countType>
{
};

// Plugin container for dynamic structure extension.
template <typename pluginType>
class CPluginContainerSA
{
public:
    // pluginType requirements:
    /*
        pluginSentry_t pluginSentry;  // data member, for OnPluginObjectCreate, OnPluginObjectDestroy
        pluginType::pluginMemory_t pluginMemory;    // data member, for OnPluginObjectCreate
    */

    struct pluginInterface
    {
        virtual bool OnPluginConstruct( pluginType *thePlugin, void *mem, unsigned int pluginId )
        {
            return false;
        }

        virtual void OnPluginDestroy( pluginType *thePlugin, void *mem, unsigned int pluginId )
        {
            return;
        }
    };

    struct pluginInfo
    {
        pluginInterface *plugInterface;
        size_t pluginSize;
        unsigned int pluginId;
    };

    typedef iterativeGrowableArray <pluginInfo, 4, 0, unsigned int> pluginInfoList_t;

    pluginInfoList_t plugins;

    // Data member for plugin types.
    struct pluginSentry_t
    {
        struct regPlugInfo
        {
            pluginInfo *descriptor;
            void *allocMem;
        };

        typedef iterativeGrowableArray <regPlugInfo, 4, 0, unsigned int> registeredPlugins_t;

        registeredPlugins_t plugins;

        inline void* GetPluginByID( unsigned int id ) const
        {
            for ( unsigned int n = 0; n < plugins.GetCount(); n++ )
            {
                const regPlugInfo& regInfo = plugins.Get( n );

                if ( regInfo.descriptor->pluginId == id )
                {
                    return regInfo.allocMem;
                }
            }
            return NULL;
        }
    };

    inline bool RegisterPlugin( size_t pluginSize, unsigned int pluginId, pluginInterface *plugInterface )
    {
        if ( pluginSize == 0 || plugInterface == NULL )
            return false;

        pluginInfo& info = plugins.ObtainItem();

        info.plugInterface = plugInterface;
        info.pluginId = pluginId;
        info.pluginSize = pluginSize;
        return true;
    }

    inline void OnPluginObjectCreate( pluginType *obj )
    {
        pluginSentry_t& sentry = obj->pluginSentry;
        pluginType::pluginMemory_t& alloc = obj->pluginMemory;

        for ( unsigned int n = 0; n < plugins.GetCount(); n++ )
        {
            pluginInfo& theInfo = plugins.Get( n );

            // Allocate memory for the plugin (if possible).
            void *mem = alloc.Allocate( theInfo.pluginSize );

            if ( mem )
            {
                bool constructSuccess = theInfo.plugInterface->OnPluginConstruct( obj, mem, theInfo.pluginId );

                if ( constructSuccess )
                {
                    pluginSentry_t::regPlugInfo& regInfo = sentry.plugins.ObtainItem();
                    
                    regInfo.descriptor = &theInfo;
                    regInfo.allocMem = mem;
                }
                else
                {
                    alloc.Free( mem );
                }
            }
        }
    }

    inline void OnPluginObjectDestroy( pluginType *obj )
    {
        pluginSentry_t& sentry = obj->pluginSentry;

        for ( unsigned int n = 0; n < sentry.plugins.GetCount(); n++ )
        {
            pluginSentry_t::regPlugInfo& regInfo = sentry.plugins.Get( n );

            regInfo.descriptor->plugInterface->OnPluginDestroy( obj, regInfo.allocMem, regInfo.descriptor->pluginId );

            // Free the plugin memory again.
            obj->pluginMemory.Free( regInfo.allocMem );
        }
    }
};

// Very fast class allocator.
template <typename dataType>
struct CachedConstructedClassAllocator
{
    struct dataEntry : dataType
    {
        AINLINE dataEntry( CachedConstructedClassAllocator *storage )
        {
            LIST_INSERT( storage->m_freeList.root, this->node );
        }

        RwListEntry <dataEntry> node;
    };

    AINLINE CachedConstructedClassAllocator( unsigned int startCount )
    {
        void *mem = (void*)new char[ sizeof( dataEntry ) * startCount ];

        for ( unsigned int n = 0; n < startCount; n++ )
        {
            new ( (dataEntry*)mem + n ) dataEntry( this );
        }
    }

    AINLINE dataEntry* AllocateNew( void )
    {
        return new dataEntry( this );
    }

    AINLINE dataType* Allocate( void )
    {
        dataEntry *entry = NULL;

        if ( !LIST_EMPTY( m_freeList.root ) )
        {
            entry = LIST_GETITEM( dataEntry, m_freeList.root.next, node );
        }

        if ( !entry )
        {
            entry = AllocateNew();
        }

        if ( entry )
        {
            LIST_REMOVE( entry->node );
            LIST_INSERT( m_usedList.root, entry->node );
        }

        return entry;
    }

    AINLINE void Free( dataType *data )
    {
        dataEntry *entry = (dataEntry*)data;

        LIST_REMOVE( entry->node );
        LIST_INSERT( m_freeList.root, entry->node );
    }

protected:
    RwList <dataEntry> m_usedList;
    RwList <dataEntry> m_freeList;
};

template <typename dataType>
struct updatableCachedDataList
{
    struct node
    {
        dataType data;
        RwListEntry <node> listNode;
    };

    AINLINE updatableCachedDataList( void )
    {
        InitializeCriticalSection( &update_lock );
    }

    AINLINE ~updatableCachedDataList( void )
    {
        assert( LIST_EMPTY( this->activeNodes.root ) == true );

        DeleteCriticalSection( &update_lock );
    }

    AINLINE void Clear( void )
    {
        EnterCriticalSection( &update_lock );

        LIST_FOREACH_BEGIN( node, activeNodes.root, listNode )
            nodeAlloc.Free( item );
        LIST_FOREACH_END

        LIST_CLEAR( activeNodes.root );

        toBeAdded.Clear();
        toBeRemoved.Clear();

        LeaveCriticalSection( &update_lock );
    }

    AINLINE void AddItem( dataType data )
    {
        EnterCriticalSection( &update_lock );

        if ( toBeAdded.Find( data ) == false )
        {
            toBeAdded.AddItem( data );
        }

        LeaveCriticalSection( &update_lock );
    }

    AINLINE void RemoveItem( dataType data )
    {
        EnterCriticalSection( &update_lock );

        if ( toBeRemoved.Find( data ) == false )
        {
            toBeRemoved.AddItem( data );
        }

        LeaveCriticalSection( &update_lock );
    }

    AINLINE void Update( void )
    {
        EnterCriticalSection( &update_lock );
        
        for ( unsigned int n = 0; n < toBeAdded.GetCount(); n++ )
        {
            const dataType& value = toBeAdded.GetFast( n );

            _AddItemInternal( value );
        }
        toBeAdded.Clear();

        for ( unsigned int n = 0; n < toBeRemoved.GetCount(); n++ )
        {
            const dataType& value = toBeRemoved.GetFast( n );

            _RemoveItemInternal( value );
        }
        toBeRemoved.Clear();

        LeaveCriticalSection( &update_lock );
    }

    AINLINE bool NeedsUpdating( void ) const
    {
        return ( toBeAdded.GetCount() != (unsigned int)0 || toBeRemoved.GetCount() != (unsigned int)0 );
    }

    struct iterator
    {
        AINLINE iterator( updatableCachedDataList& manager ) : manager( manager )
        {
            curIter = manager.activeNodes.root.next;
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( curIter == &manager->activeNodes.root );
        }

        AINLINE dataType& Resolve( void )
        {
            return LIST_GETITEM( node, curIter, listNode )->data;
        }

        AINLINE void Increment( void )
        {
            curIter = curIter->next;
        }

        RwListEntry <node> *curIter;
        updatableCachedDataList& manager;
    };

protected:
    AINLINE node* AllocateNode( void )
    {
        node *theNode = nodeAlloc.Allocate();

        if ( theNode )
        {
            LIST_INSERT( activeNodes.root, theNode->listNode );
        }

        return theNode;
    }

    AINLINE void FreeNode( node *ptr )
    {
        LIST_REMOVE( ptr->listNode );

        nodeAlloc.Free( ptr );
    }

    AINLINE void _AddItemInternal( dataType data )
    {
        node *holderNode = AllocateNode();

        holderNode->data = data;
    }

    AINLINE void _RemoveItemInternal( dataType data )
    {
        LIST_FOREACH_BEGIN( node, activeNodes.root, listNode )
            if ( item->data == data )
            {
                item->data = dataType();

                FreeNode( item );
                return;
            }
        LIST_FOREACH_END
    }

    RwList <node> activeNodes;
    CachedConstructedClassAllocator <node> nodeAlloc;

    typedef iterativeGrowableArray <dataType, 8, 0, unsigned int> dataList_t;

    dataList_t toBeAdded;
    dataList_t toBeRemoved;

    mutable CRITICAL_SECTION update_lock;
};

#endif //_MEMORY_ABSTRACTION_UTILITIES_
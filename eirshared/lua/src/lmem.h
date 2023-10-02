/*
** $Id: lmem.h,v 1.31.1.1 2007/12/27 13:02:25 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/

#ifndef lmem_h
#define lmem_h


#include <stddef.h>

#include <sdk/MemoryUtils.h>

#include "llimits.h"
#include "lua.h"

struct global_State;

#define MEMERRMSG	"not enough memory"

#define FASTAPI inline


LUAI_FUNC void *luaM_realloc__(global_State *g, void *block, size_t oldSize, size_t size);
LUAI_FUNC void *luaM_realloc_ (lua_State *L, void *block, size_t oldsize, size_t size);
LUAI_FUNC void *luaM_toobig   (lua_State *L);
LUAI_FUNC void *luaM_growaux_ (lua_State *L, void *block, size_t *size,
                               size_t size_elem, size_t limit,
                               const char *errormsg);

FASTAPI void* luaM_reallocv( lua_State *L, void *block, size_t oldN, size_t newN, size_t e )
{
    if ( cast(size_t, (newN)+1) > MAX_SIZET/(e) )  /* +1 to avoid warnings */ \
    {
        luaM_toobig( L );
        return nullptr;
    }

    return luaM_realloc_( L, block, oldN*e, newN*e );
}
FASTAPI void luaM_freemem( lua_State *L, void *block, size_t memSize )
{
    luaM_realloc_( L, block, memSize, 0 );
}
template <typename structType>
FASTAPI void luaM_free( lua_State *L, structType *theData )
{
    luaM_realloc_( L, theData, sizeof(*theData), 0 );
}
template <typename structType>
FASTAPI void luaM_freearray( lua_State *L, structType *block, size_t n )
{
    for ( size_t i = 0; i < n; i++ )
    {
        (block+i)->~structType();
    }

    luaM_reallocv( L, block, n, 0, sizeof( structType ) );
}

FASTAPI void* luaM_malloc( lua_State *L, size_t memSize )
{
    return luaM_realloc_( L, nullptr, 0, memSize );
}
template <typename structType>
FASTAPI structType* luaM_newvector( lua_State *L, size_t n )
{
    structType *vecOut = nullptr;

    if ( n != 0 )
    {
        void *mem = luaM_reallocv( L, nullptr, 0, n, sizeof( structType ) );

        if ( mem )
        {
            for ( size_t i = 0; i < n; i++ )
            {
                new ((structType*)mem + i) structType;
            }

            vecOut = (structType*)mem;
        }
    }

    return vecOut;
}
template <typename structType>
FASTAPI structType* luaM_clonevector( lua_State *L, structType *srcVector, size_t n )
{
    structType *vecOut = nullptr;

    if ( n != 0 )
    {
        void *mem = luaM_reallocv( L, nullptr, 0, n, sizeof( structType ) );

        if ( mem )
        {
            for ( size_t i = 0; i < n; i++ )
            {
                const structType& srcObj = srcVector[ i ];

                new ((structType*)mem + i) structType( srcObj );
            }

            vecOut = (structType*)mem;
        }
    }

    return vecOut;
}
template <typename structType>
FASTAPI void luaM_growvector( lua_State *L, structType*& vInOut, size_t nelems, size_t& size, size_t limit, const char *errormsg )
{
    if ( nelems + 1 > size )
    {
        void *vMem = vInOut;

        size_t oldSize = size;

        void *vPartial = luaM_growaux_( L, vMem, &size, sizeof(structType), limit, errormsg );

        // Construct the new objects.
        size_t newSize = size;

        for ( size_t n = oldSize; n < newSize; n++ )
        {
            new ((structType*)vPartial + n) structType;
        }

        vInOut = (structType*)vPartial;
    }
}
template <typename structType>
FASTAPI void luaM_reallocvector( lua_State *L, structType*& vInOut, size_t oldN, size_t newN )
{
    if ( oldN != newN )
    {
        // If the array should decrease itself, delete the entries that are removed.
        for ( size_t n = newN; n < oldN; n++ )
        {
            ( vInOut + n )->~structType();
        }

        // Reallocate the memory.
        void *vPartial = luaM_reallocv( L, vInOut, oldN, newN, sizeof( structType ) );

        // Create entries if the array increased in size.
        for ( size_t n = oldN; n < newN; n++ )
        {
            new ((structType*)vPartial + n) structType;
        }

        // The vector is complete now, return it.
        vInOut = (structType*)vPartial;
    }
}

// Module initialization.
LUAI_FUNC void luaM_init( lua_config *cfg );
LUAI_FUNC void luaM_shutdown( lua_config *cfg );

// Default class factory memory allocator.
struct GeneralMemoryAllocator
{
    lua_Alloc allocCallback;
    void *userdata;

    inline GeneralMemoryAllocator( void *userdata, lua_Alloc allocCallback )
    {
        this->allocCallback = allocCallback;
        this->userdata = userdata;
    }

    inline ~GeneralMemoryAllocator( void )
    {
        return;
    }

    inline void* Allocate( size_t memSize )
    {
        return allocCallback( this->userdata, nullptr, 0, memSize );
    }

    inline void Free( void *ptr, size_t memSize )
    {
        allocCallback( this->userdata, ptr, memSize, 0 );
    }

    inline void* ReAlloc( void *ptr, size_t oldMemSize, size_t newMemSize )
    {
        return allocCallback( this->userdata, ptr, oldMemSize, newMemSize );
    }
};

// Cached class allocator for Lua.
// See also "CachedConstructedClassAllocator" struct.
template <typename dataType>
struct LuaCachedConstructedClassAllocator
{
protected:
    struct dataEntry : dataType
    {
        AINLINE dataEntry( LuaCachedConstructedClassAllocator *storage, bool isSummoned )
        {
            LIST_INSERT( storage->m_freeList.root, this->node );

            this->isSummoned = isSummoned;
        }

        RwListEntry <dataEntry> node;
        bool isSummoned;
    };

    struct summonBlockTail
    {
        unsigned int allocCount;
        RwListEntry <summonBlockTail> node;
    };

public:
    AINLINE LuaCachedConstructedClassAllocator( void )
    {
        return;
    }

    AINLINE ~LuaCachedConstructedClassAllocator( void )
    {
        lua_assert( LIST_EMPTY( m_freeList.root ) );
        lua_assert( LIST_EMPTY( m_usedList.root ) );

        lua_assert( LIST_EMPTY( m_summonBlocks.root ) );
    }

    AINLINE void SummonEntries( lua_State *L, unsigned int startCount )
    {
        void *mem = luaM_realloc_( L, nullptr, 0, sizeof( dataEntry ) * startCount + sizeof( summonBlockTail ) );

        if ( mem )
        {
            for ( unsigned int n = 0; n < startCount; n++ )
            {
                new ( (dataEntry*)mem + n ) dataEntry( this, true );
            }

            // Get the tail of the summon block and add it to the list of summoned blocks.
            summonBlockTail *theTail = (summonBlockTail*)( (dataEntry*)mem + startCount );

            theTail->allocCount = startCount;

            LIST_APPEND( m_summonBlocks.root, theTail->node );
        }
    }

    AINLINE void Shutdown( global_State *g )
    {
        // Having actually used blocks by this point is deadly.
        // Make sure there are none.
        lua_assert( LIST_EMPTY( m_usedList.root ) );

        // Allocate all free blocks.
        LIST_FOREACH_BEGIN( dataEntry, m_freeList.root, node )
            if ( !item->isSummoned )
            {
                // Free this memory.
                luaM_realloc__( g, item, sizeof( dataEntry ), 0 );
            }
        LIST_FOREACH_END

        LIST_CLEAR( m_freeList.root );

        // Deallocate all summoned blocks now.
        LIST_FOREACH_BEGIN( summonBlockTail, m_summonBlocks.root, node )
            // Get the memory begin pointer.
            void *blockStart = (void*)( (dataEntry*)item - item->allocCount );

            // Free it.
            luaM_realloc__( g, blockStart, sizeof( dataEntry ) * item->allocCount + sizeof( summonBlockTail ), 0 );
        LIST_FOREACH_END

        LIST_CLEAR( m_summonBlocks.root );
    }

private:
    AINLINE dataEntry* AllocateNew( lua_State *L )
    {
        void *mem = luaM_realloc_( L, nullptr, 0, sizeof( dataEntry ) );

        if ( mem )
        {
            return new (mem) dataEntry( this, false );
        }
        return nullptr;
    }

public:
    AINLINE dataType* Allocate( lua_State *L )
    {
        dataEntry *entry = nullptr;

        if ( !LIST_EMPTY( m_freeList.root ) )
        {
            entry = LIST_GETITEM( dataEntry, m_freeList.root.next, node );
        }

        if ( !entry )
        {
            entry = AllocateNew( L );
        }

        if ( entry )
        {
            LIST_REMOVE( entry->node );
            LIST_INSERT( m_usedList.root, entry->node );
        }

        return entry;
    }

    AINLINE void Free( lua_State *L, dataType *data )
    {
        dataEntry *entry = (dataEntry*)data;

        LIST_REMOVE( entry->node );
        LIST_INSERT( m_freeList.root, entry->node );
    }

protected:
    RwList <dataEntry> m_usedList;
    RwList <dataEntry> m_freeList;

    RwList <summonBlockTail> m_summonBlocks;
};

#endif


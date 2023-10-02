/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        vendor/lua/src/lstrtable.h
*  PURPOSE:     Lua fast string table
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_STRINGTABLE_
#define _LUA_STRINGTABLE_

struct STableItemHeader
{
    RwListEntry <STableItemHeader> node;
    TString *key;
};

// Inline dynamic string table struct
// Used to allocate nodes with
template <class typeinfo>
struct DynamicStringTable
{
    void* operator new( size_t size, lua_State *main )
    {
        return luaM_malloc( main, size );
    }

    void operator delete( void *ptr )
    {
        assert( 0 );
    }

    void operator delete( void *ptr, lua_State *main )
    {
        abort();
    }

    inline void deleteMem( lua_State *L )
    {
        luaM_free( L, this );
    }

    DynamicStringTable( void )
    {
        return;
    }

    ~DynamicStringTable( void )
    {
        assert( LIST_EMPTY( m_list.root ) == true );
    }

    inline void Clear( lua_State *L )
    {
        while ( !LIST_EMPTY( m_list.root ) )
        {
            DeleteNode( L, LIST_GETITEM( STableItemHeader, m_list.root.next, node ) );
        }
    }

    inline STableItemHeader* AllocateNode( lua_State *L, TString *key, size_t size )
    {
        STableItemHeader *item = (STableItemHeader*)luaM_malloc( L, sizeof(STableItemHeader) + size );
        item->key = key;
        LIST_INSERT( m_list.root, item->node );
        return item;
    }

    inline STableItemHeader* FindNode( const TString *key )
    {
        LIST_FOREACH_BEGIN( STableItemHeader, m_list.root, node )
            if ( item->key == key )
                return item;
        LIST_FOREACH_END

        return nullptr;
    }

    template <class type>
    inline void SetNodeValue( STableItemHeader *node, const type& val )
    {
        *(type*)( node + 1 ) = val;
    }

    inline void* GetNodeValue( STableItemHeader *node ) const
    {
        return node + 1;
    }

    inline void DeleteNode( lua_State *L, STableItemHeader *node )
    {
        // Get the value size prior to destroying it.
        size_t valueSize = typeinfo::GetNodeValueSize( GetNodeValue( node ) );

        // Notify the destructor
        typeinfo::DestroyNodeValue( GetNodeValue( node ) );

        LIST_REMOVE( node->node );
        luaM_freemem( L, node, valueSize + sizeof(*node) );
    }

    void    TraverseGC( global_State *g );

    RwList <STableItemHeader> m_list;
};

template <class type>
struct NodeTypeInfo
{
    static inline void DestroyNodeValue( void *ptr )
    {
        ((type*)ptr)->~type();
    }
};

template <class type>
struct StaticNodeTypeInfo : NodeTypeInfo <type>
{
    static inline size_t GetNodeValueSize( void *ptr )
    {
        return sizeof(type);
    }

    static inline void MarkValue( global_State *g, void *ptr )
    {
        return;
    }
};

template <class type>
class StringTable : public DynamicStringTable <StaticNodeTypeInfo <type>>
{
public:
    void    SetItem( lua_State *L, TString *key, type& val )
    {
        STableItemHeader *item = this->FindNode( key );

        if ( !item )
            item = this->AllocateNode( L, key, sizeof(type) );

        this->SetNodeValue( item, val );
    }

    type*   GetItem( const TString *key )
    {
        STableItemHeader *item = this->FindNode( key );

        if ( !item )
            return nullptr;

        return (type*)this->GetNodeValue( item );
    }

    void    UnsetItem( lua_State *L, TString *key )
    {
        STableItemHeader *item = this->FindNode( key );

        if ( !item )
            return;

        this->DeleteNode( L, item );
    }

    size_t    GetNodeSize( STableItemHeader *node ) const
    {
        return sizeof(type);
    }
};

// Dynamic class string table
struct VirtualClassEntry abstract
{
    virtual ~VirtualClassEntry( void )      {}

    virtual size_t  GetSize( void ) const = 0;
    virtual void    MarkValue( global_State *g ) const = 0;
};

struct DynamicNodeTypeInfo : public NodeTypeInfo <VirtualClassEntry>
{
    static inline size_t GetNodeValueSize( void *ptr )
    {
        return ((VirtualClassEntry*)ptr)->GetSize();
    }

    static inline void MarkValue( global_State *g, void *ptr )
    {
        ((VirtualClassEntry*)ptr)->MarkValue( g );
    }
};

// String table which requires inline handling
typedef DynamicStringTable <DynamicNodeTypeInfo> ClassStringTable;

#endif //_LUA_STRINGTABLE_

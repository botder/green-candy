/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArguments.cpp
*  PURPOSE:     Lua networking
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CClientGame* g_pClientGame;

static inline bool RakNet_ReadArgumentByType( NetBitStreamInterface& stream, lua_State *L, int type )
{
    union
    {
        bool boolean;
        float number;
        int iNumber;
        CClientEntity *entity;
    };

    switch( type )
    {
    case LUA_TNIL:
        lua_pushnil( L );
        return true;
    case LUA_TBOOLEAN:
        if ( !stream.ReadBit( boolean ) )
            return false;

        lua_pushboolean( L, boolean );
        return true;
    case LUA_TNUMBER:
        if ( stream.ReadBit( boolean ) && boolean )
        {
            if ( !stream.Read( number ) )
                return false;
            
            lua_pushnumber( L, number );
        }
        else
        {
            if ( !stream.ReadCompressed( iNumber ) )
                return false;

            lua_pushinteger( L, iNumber );
        }
        return true;
    case LUA_TSTRING:
        {
            std::string string;

            if ( !stream.ReadStringCompressed( string ) )
                return false;

            lua_pushlstring( L, string.c_str(), string.size() );
        }
        return true;
    case LUA_TLIGHTUSERDATA:
    case LUA_TUSERDATA:
        {
            ElementID id;

            if ( !stream.Read( id ) )
                return false;

            entity = CElementIDs::GetElement( id );

            if ( !entity )
                return false;

            entity->PushStack( L );
        }
        return true;
    }

    return false;
}

template <class handler>
static inline bool RakNet_ReadTable( NetBitStreamInterface& stream, lua_State *L, handler& cb );

template <class handler>
static inline bool _RakNet_ReadArguments( NetBitStreamInterface& stream, lua_State *L, handler& cb )
{
    unsigned short count;

    if ( !stream.ReadCompressed( count ) )
        return false;

    cb.beginRead( L, count );

    for ( unsigned short n = 0; n < count; n++ )
    {
        SLuaTypeSync type;

        if ( !stream.Read( &type ) )
            return false;

        if ( !RakNet_ReadArgumentByType( stream, L, type.data.ucType ) )
        {
            unsigned long refId;

            switch( type.data.ucType )
            {
            case LUA_TTABLE:
                if ( !RakNet_ReadTable( stream, L, cb ) )
                    return false;

                break;
            case LUA_TTABLEREF:
                if ( !stream.ReadCompressed( refId ) )
                    return false;

                cb.obtainCache( L, refId );

                type.data.ucType = LUA_TTABLE;
                break;
            default:
                return false;
            }
        }

        // Notify the handler for every lua thing
        cb.read( L, type.data.ucType );
    }

    return true;
}

struct read_cache_handler
{
    inline read_cache_handler( int cacheIdx )
    {
        m_cacheIdx = cacheIdx;
    }

    inline void beginRead( lua_State *, int ) const
    {
    }

    inline void start( lua_State *L, int tidx )
    {
        lua_pushvalue( L, tidx );
        luaL_ref( L, m_cacheIdx );
    }

    inline void read( lua_State *, int ) const
    {
    }

    inline void obtainCache( lua_State *L, int idx )
    {
        lua_rawgeti( L, m_cacheIdx, idx );
    }

    int m_cacheIdx;
};

struct table_handler : public read_cache_handler
{
    inline table_handler( int tidx, int cacheIdx ) : read_cache_handler( cacheIdx )
    {
        m_tidx = tidx;
        m_isKey = true;
        m_ignoreValue = false;
    }

    inline void read( lua_State *L, int type )
    {
        // Check whether we have a key here
        if ( !( m_isKey = !m_isKey ) )
        {
            if ( type == LUA_TNIL || type == LUA_TNONE )
            {
                m_ignoreValue = true;

                lua_pop( L, 1 );
            }

            return;
        }

        if ( m_ignoreValue )
        {
            lua_pop( L, 1 );

            m_ignoreValue = false;
            return;
        }

        lua_settable( L, m_tidx );
    }

    bool m_isKey;
    int m_tidx;
    bool m_ignoreValue;
};

template <class handler>
static inline bool RakNet_ReadTable( NetBitStreamInterface& stream, lua_State *L, handler& cb )
{
    int top = lua_gettop( L );

    // The table we save things at
    lua_createtable( L, 2, 0 );

    cb.start( L, top + 1 );

    if ( !_RakNet_ReadArguments( stream, L, table_handler( top + 1, cb.m_cacheIdx ) ) )
    {
        lua_settop( L, top );
        return false;
    }

    return true;
}

bool RakNet_ReadArgument( NetBitStreamInterface& stream, lua_State *L )
{
    SLuaTypeSync type;

    if ( !stream.Read( &type ) )
        return false;

    lua_checkstack( L, 1 );

    if ( RakNet_ReadArgumentByType( stream, L, type.data.ucType ) )
        return true;

    if ( type.data.ucType == LUA_TTABLE )
    {
        // Table
        lua_newtable( L );

        int tidx = lua_gettop( L );

        // Cache
        lua_newtable( L );

        bool ret = RakNet_ReadTable( stream, L, table_handler( tidx, tidx + 1 ) );

        lua_settop( L, tidx );
        return ret;
    }

    return false;
}

struct stack_handler : public read_cache_handler
{
    inline stack_handler( int cacheIdx, unsigned short& count ) : read_cache_handler( cacheIdx ), m_count( count )
    {
    }

    inline void beginRead( lua_State *L, int count )
    {
        lua_checkstack( L, count );
    }

    inline void read( lua_State *L, int )
    {
        m_count++;
    }

    unsigned short& m_count;
};

bool RakNet_ReadArguments( NetBitStreamInterface& stream, lua_State *L, unsigned short& count )
{
    int top = lua_gettop( L );

    lua_newtable( L );

    if ( _RakNet_ReadArguments( stream, L, stack_handler( top + 1, count ) ) )
    {
        lua_remove( L, top + 1 );
        return true;
    }

    lua_settop( L, top );
    return false;
}

static inline bool RakNet_WriteArgumentByType( NetBitStreamInterface& stream, lua_State *L, int idx, int luaType )
{
    SLuaTypeSync type;
    float num;
    int iNum;

    switch( luaType )
    {
    case LUA_TNIL:
    case LUA_TNONE:
        type.data.ucType = LUA_TNIL;
        stream.Write( &type );
        return true;
    case LUA_TBOOLEAN:
        type.data.ucType = LUA_TBOOLEAN;
        stream.Write( &type );

        stream.WriteBit( lua_toboolean( L, idx ) );
        return true;
    case LUA_TNUMBER:
        type.data.ucType = LUA_TNUMBER;
        stream.Write( &type );

        num = (float)lua_tonumber( L, idx );
        iNum = (int)num;

        if ( num == iNum )
        {
            stream.WriteBit( false );
            stream.WriteCompressed( iNum );
        }
        else
        {
            stream.WriteBit( true );
            stream.Write( num );
        }
        return true;
    case LUA_TSTRING:
        type.data.ucType = LUA_TSTRING;
        stream.Write( &type );

        stream.WriteStringCompressed( lua_getstring( L, idx ) );
        return true;
    case LUA_TCLASS:
        {
            CClientEntity *entity;

            if ( !lua_refclass( L, idx )->GetTransmit( LUACLASS_ENTITY, (void*&)entity ) )
                return false;

            if ( entity->IsLocalEntity() )
                return false;

            type.data.ucType = LUA_TLIGHTUSERDATA;
            stream.Write( &type );

            stream.Write( entity->GetID() );
        }
        return true;
    }

    return false;
}

struct put_cache_table
{
    inline put_cache_table( std::vector <const void *>& cache ) : m_cache( cache )
    {
    }

    inline void put( lua_State *L, const void *ptr )
    {
        m_cache.push_back( ptr );
    }

    inline bool obtainCache( const void *ptr, unsigned long& extId )
    {
        unsigned long id = 0;

        for ( ; id != m_cache.size(); id++ )
        {
            if ( m_cache[id] == ptr )
            {
                extId = id;
                return true;
            }
        }

        return false;
    }

    std::vector <const void *>& m_cache;
};

static inline unsigned short RakNet_GetTableEntryCount( lua_State *L, int idx )
{
    unsigned short count = 0;

    lua_pushnil( L );

    while ( lua_next( L, idx ) )
    {
        if ( !lua_isnil( L, -2 ) )
        {
            if ( count > 0x10000 - 3 )
                throw lua_exception( L, LUA_ERRRUN, "too many table entries (65336 max!)" );

            count += 2;
        }

        lua_pop( L, 1 );
    }

    return count;
}

template <class handler>
static inline void RakNet_WriteTable( NetBitStreamInterface& stream, lua_State *L, int idx, handler& cb );

template <class handler>
static inline void RakNet_WriteLinkedArgument( NetBitStreamInterface& stream, lua_State *L, int idx, handler& cb )
{
    int type = lua_type( L, idx );

    if ( !RakNet_WriteArgumentByType( stream, L, idx, type ) )
    {
        SLuaTypeSync syncType;
        const void *ptr = lua_topointer( L, idx );
        unsigned long refId;

        switch( type )
        {
        case LUA_TTABLE:
            if ( cb.obtainCache( ptr, refId ) )
            {
                syncType.data.ucType = LUA_TTABLEREF;
                stream.Write( &syncType );

                stream.WriteCompressed( refId );
            }
            else
            {
                syncType.data.ucType = LUA_TTABLE;
                stream.Write( &syncType );

                RakNet_WriteTable( stream, L, idx, cb );
            }
            break;
        default:
            syncType.data.ucType = LUA_TNIL;
            stream.Write( &syncType );
            break;
        }
    }
}

template <class handler>
static inline void RakNet_WriteTable( NetBitStreamInterface& stream, lua_State *L, int idx, handler& cb )
{
    cb.put( L, lua_topointer( L, idx ) );

    // Space for table and key and value
    lua_checkstack( L, 3 );

    lua_pushvalue( L, idx );

    int tidx = lua_gettop( L );

    // The other side has to know how much we wrote
    stream.WriteCompressed( RakNet_GetTableEntryCount( L, tidx ) );

    lua_pushnil( L );

    // Loop through the table
    while ( lua_next( L, tidx ) )
    {
        if ( !lua_isnil( L, -2 ) )
        {
            RakNet_WriteLinkedArgument( stream, L, -2, cb );
            RakNet_WriteLinkedArgument( stream, L, -1, cb );
        }

        lua_pop( L, 1 );
    }

    lua_pop( L, 1 );
}

void RakNet_WriteArguments( NetBitStreamInterface& stream, lua_State *L, int startIdx, unsigned short count )
{
    stream.WriteCompressed( count );

    std::vector <const void*> cacheList;

    put_cache_table cache( cacheList );

    while ( count-- )
        RakNet_WriteLinkedArgument( stream, L, startIdx++, cache );
}

void RakNet_WriteArgument( NetBitStreamInterface& stream, lua_State *L, int idx )
{
    int type = lua_type( L, idx );

    if ( RakNet_WriteArgumentByType( stream, L, idx, type ) )
        return;
    
    if ( type == LUA_TTABLE )
        RakNet_WriteTable( stream, L, idx, put_cache_table( std::vector <const void*>() ) );
    else
    {
        SLuaTypeSync sync;
        sync.data.ucType = LUA_TNIL;

        stream.Write( &sync );
    }
}
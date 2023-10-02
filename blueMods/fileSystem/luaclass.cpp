/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luaclass.cpp
*  PURPOSE:     Lua filesystem access
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static int luaclass_destroy( lua_State *L )
{
    void *obj = luaclass_getpointer( L, 1 );

    // TODO: allow clean-up of the given object, we have to support all possible ones.

    if ( fsIsFile( obj ) )
    {
        fsCleanupFile( (CFile*)obj );
    }
    else if ( fsIsTranslator( obj ) )
    {
        fsCleanupTranslator( (CFileTranslator*)obj );
    }

    return 0;
}

static const luaL_Reg _common_class_meths[] =
{
    { "destroy", luaclass_destroy },
    { nullptr, nullptr }
};

void luaclass_makemeta( lua_State *L, int ridx )
{
    lua_newtable( L );
    lua_pushvalue( L, ridx - 1 );
    luaL_openlib( L, nullptr, _common_class_meths, 1 );
}

void* luaclass_getpointer( lua_State *L, int idx )
{
    luaL_checktype( L, idx, LUA_TUSERDATA );

    if ( lua_objlen( L, idx ) < sizeof(void*) )
    {
        lua_pushlstring( L, "invalid userdata", 16 );
        lua_error( L );
    }

    void **ptr_class = (void**)lua_touserdata( L, idx );
    
    assert( ptr_class != nullptr );

    return *ptr_class;
}

static int luaclassmt_index( lua_State *L )
{
    void *obj = luaclass_getpointer( L, 1 );

    // TODO: return the methods that are appropriate for the given object.

    if ( fsIsFile( obj ) )
    {
        lua_getmetatable( L, lua_upvalueindex( 1 ) );
        lua_pushlstring( L, "metas", 5 );
        lua_rawget( L, -2 );
        lua_pushlstring( L, "file", 4 );
        lua_rawget( L, -2 );
        goto doIndex;
    }
    else if ( fsIsTranslator( obj ) )
    {
        lua_getmetatable( L, lua_upvalueindex( 1 ) );
        lua_pushlstring( L, "metas", 5 );
        lua_rawget( L, -2 );
        lua_pushlstring( L, "ftrans", 6 );
        lua_rawget( L, -2 );
        goto doIndex;
    }

    return 0;

doIndex:
    lua_pushvalue( L, 2 );
    lua_rawget( L, -2 );

    if ( lua_isnil( L, -1 ) )
    {
        lua_pop( L, 2 );

        // Try to index general class methods instead.
        lua_pushlstring( L, "class", 5 );
        lua_rawget( L, -2 );
        lua_pushvalue( L, 2 );
        lua_rawget( L, -2 );
    }

    return 1;
}

static int luaclassmt_gc( lua_State *L )
{
    return luaclass_destroy( L );
}

static const luaL_Reg _luaclass_metatable[] =
{
    { "__index", luaclassmt_index },
    { "__gc", luaclassmt_gc },
    { nullptr, nullptr }
};

void luaclassmt_make( lua_State *L, int ridx )
{
    lua_newtable( L );
    lua_pushvalue( L, ridx - 1 );
    luaL_openlib( L, nullptr, _luaclass_metatable, 1 );
}

static void luaclassmt_push( lua_State *L )
{
    assert( lua_type( L, lua_upvalueindex( 1 ) ) == LUA_TUSERDATA );
    lua_getmetatable( L, lua_upvalueindex( 1 ) );
    assert( lua_type( L, -1 ) == LUA_TTABLE );
    lua_pushlstring( L, "classmt", 7 );
    lua_rawget( L, -2 );
    lua_remove( L, -2 );
}

void luaclass_pushshim( lua_State *L, void *classptr )
{
    void **ptrud = (void**)lua_newuserdata( L, sizeof(void*) );

    *ptrud = classptr;

    luaclassmt_push( L );
    lua_setmetatable( L, -2 );
}
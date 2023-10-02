/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luamd5.cpp
*  PURPOSE:     Lua md5 hasher instance
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static int init( lua_State *L )
{
    ((CMD5Hasher*)lua_touserdata( L, lua_upvalueindex( 1 )))->Init();
    return 0;
}

static int update( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );

    size_t len;
    const unsigned char *str = (const unsigned char*)lua_tolstring( L, 1, &len );

    ((CMD5Hasher*)lua_touserdata( L, lua_upvalueindex( 1 )))->Update( str, len );
    return 0;
}

static int finalize( lua_State *L )
{
    ((CMD5Hasher*)lua_touserdata( L, lua_upvalueindex( 1 )))->Finalize();
    return 0;
}

static int tohex( lua_State *L )
{
    char hash[33];

    CMD5Hasher::ConvertToHex( *(MD5*)((CMD5Hasher*)lua_touserdata( L, lua_upvalueindex( 1 )))->m_digest, hash );
    
    lua_pushlstring( L, hash, 32 );
    return 1;
}

static int destroy( lua_State *L )
{
    delete (CMD5Hasher*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg md5hasher_interface[] =
{
    { "init", init },
    { "update", update },
    { "finalize", finalize },
    { "tohex", tohex },
    { "destroy", destroy },
    { NULL, NULL }
};

static int luaconstructor_md5( lua_State *L )
{
    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_MD5HASHER, lua_touserdata( L, lua_upvalueindex( 1 ) ) );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, md5hasher_interface, 1 );

    lua_pushlstring( L, "md5hasher", 9 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void lua_createmd5hasher( lua_State *L, CMD5Hasher *hasher )
{
    lua_pushlightuserdata( L, hasher );
    lua_pushcclosure( L, luaconstructor_md5, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );
}
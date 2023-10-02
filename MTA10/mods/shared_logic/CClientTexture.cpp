/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTexture.cpp
*  PURPOSE:     Core texture management interface for deathmatch
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg texture_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_texture( lua_State *L )
{
    CClientTexture *tex = (CClientTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_CORETEXTURE, tex );

    j.RegisterInterfaceTrans( L, texture_interface, 0, LUACLASS_CORETEXTURE );

    lua_pushlstring( L, "core-texture", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientTexture::CClientTexture( CClientManager *pManager, ElementID ID, lua_State *L, CTextureItem *pTextureItem ) : CClientMaterial( pManager, ID, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_texture, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName( "texture" );
    
    m_pRenderItem = pTextureItem;
}

static const luaL_Reg target_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_target( lua_State *L )
{
    CClientRenderTarget *tex = (CClientRenderTarget*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_CORERENDERTARGET, tex );

    j.RegisterInterfaceTrans( L, target_interface, 0, LUACLASS_CORERENDERTARGET );

    lua_pushlstring( L, "rendertarget", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientRenderTarget::CClientRenderTarget( CClientManager* pManager, ElementID ID, lua_State *L, CRenderTargetItem* pRenderTargetItem ) : CClientTexture( pManager, ID, L, pRenderTargetItem )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_target, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName( "rendertarget" );
}

static const luaL_Reg screensource_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_screensource( lua_State *L )
{
    CClientScreenSource *tex = (CClientScreenSource*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_CORESCREENSOURCE, tex );

    j.RegisterInterfaceTrans( L, screensource_interface, 0, LUACLASS_CORESCREENSOURCE );

    lua_pushlstring( L, "screensource", 12 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientScreenSource::CClientScreenSource( CClientManager* pManager, ElementID ID, lua_State *L, CScreenSourceItem* pScreenSourceItem ) : CClientTexture( pManager, ID, L, pScreenSourceItem )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_screensource, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName( "screensource" );
}
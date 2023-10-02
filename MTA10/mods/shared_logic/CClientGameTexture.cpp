/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGameTexture.cpp
*  PURPOSE:     RenderWare texture entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

typedef std::vector <unsigned short> imports_t;

static LUA_DECLARE( getName )
{
    CClientGameTexture *tex = (CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) );
    const std::string& name = tex->GetName();

    lua_pushlstring( L, name.c_str(), name.size() );
    return 1;
}

static LUA_DECLARE( getHash )
{
    lua_pushnumber( L, ((CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->GetHash() );
    return 1;
}

static LUA_DECLARE( setTXD )
{
    CClientTXD *txd;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( txd, LUACLASS_TXD, NULL );
    LUA_ARGS_END;

    ((CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->SetTXD( txd );
    LUA_SUCCESS;
}

static LUA_DECLARE( getTXD )
{
    CClientTXD *txd = ((CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->GetTXD();

    if ( !txd )
        return 0;

    txd->PushStack( L );
    return 1;
}

static LUA_DECLARE( setFiltering )
{
    bool filter;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( filter );
    LUA_ARGS_END;

    ((CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->m_tex.SetFiltering( filter );
    LUA_SUCCESS;
}

static LUA_DECLARE( isFiltering )
{
    lua_pushboolean( L, ((CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->m_tex.IsFiltering() );
    return 1;
}

static const luaL_Reg texture_interface[] =
{
    LUA_METHOD( getName ),
    LUA_METHOD( getHash ),
    LUA_METHOD( setTXD ),
    LUA_METHOD( getTXD ),
    LUA_METHOD( setFiltering ),
    LUA_METHOD( isFiltering ),
    { NULL, NULL }
};

static int luaconstructor_texture( lua_State *L )
{
    CClientGameTexture *tex = (CClientGameTexture*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_TEXTURE, tex );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, texture_interface, 1 );

    lua_pushlstring( L, "texture", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientGameTexture::CClientGameTexture( lua_State *L, CTexture& tex ) : LuaElement( L ), m_tex( tex )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_texture, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Initially we do not have any owner
    m_owner = NULL;
    LIST_INITNODE( m_ownerTex );

    m_keepAlive = true;

    // Set up our members
    m_txd = NULL;
    LIST_INITNODE( m_textures );
}

CClientGameTexture::~CClientGameTexture()
{
    // Remove from the manager
    UnlinkOwner();

    // Unlink ourselves
    RemoveFromTXD();

    delete &m_tex;
}

void CClientGameTexture::SetTXD( CClientTXD *txd )
{
    if ( m_txd == txd )
        return;

    RemoveFromTXD();

    m_txd = txd;

    if ( txd )
    {
        LIST_INSERT( txd->m_textures.root, m_textures );

        m_tex.SetTXD( &txd->m_txd );

        SetRoot( txd );
    }
}

void CClientGameTexture::RemoveFromTXD()
{
    if ( !m_txd )
        return;

    SetRoot( NULL );

    m_tex.RemoveFromTXD();

    LIST_REMOVE( m_textures );
    m_txd = NULL;
}

bool CClientGameTexture::Import( unsigned short id )
{
    if ( !m_tex.Import( id ) )
        return false;

    g_pClientGame->GetManager()->Restream( id );
    return true;
}

bool CClientGameTexture::IsImported( unsigned short id ) const
{
    return m_tex.IsImported( id );
}

void CClientGameTexture::Remove( unsigned short id )
{
    if ( !m_tex.Remove( id ) )
        return;

    g_pClientGame->GetManager()->Restream( id );
}

void CClientGameTexture::UnlinkOwner()
{
    if ( !m_owner )
        return;

    LIST_REMOVE( m_ownerTex );

    m_owner = NULL;
}

void CClientGameTexture::SetOwner( CResource *res )
{
    UnlinkOwner();

    m_owner = res;

    if ( !m_owner )
        return;

    LIST_INSERT( res->m_gameTextures.root, m_ownerTex );
}

void CClientGameTexture::MarkGC( lua_State *L )
{
    if ( m_tex.IsUsed() || m_keepAlive )
        LuaClass::MarkGC( L );
}
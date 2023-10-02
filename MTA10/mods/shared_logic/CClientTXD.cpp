/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTXD.cpp
*  PURPOSE:     TXD manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               arc_
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

typedef std::list <unsigned short> imports_t;

static LUA_DECLARE( getTextures )
{
    CClientTXD *txd = ((CClientTXD*)lua_getmethodtrans( L ));
    RwList <CClientGameTexture>& list = txd->m_textures;

    lua_settop( L, 0 );
    lua_newtable( L );

    unsigned int n = 1;

    LIST_FOREACH_BEGIN( CClientGameTexture, list.root, m_textures )
        item->PushStack( L );
        lua_rawseti( L, 1, n++ );
    LIST_FOREACH_END

    return 1;
}

static LUA_DECLARE( setGlobalEmitter )
{
    ((CClientTXD*)lua_getmethodtrans( L ))->m_txd.SetGlobalEmitter();
    return 0;
}

static const luaL_Reg txd_interface_trans[] =
{
    LUA_METHOD( getTextures ),
    LUA_METHOD( setGlobalEmitter ),
    { NULL, NULL }
};

typedef std::list <CTexture*> textures_t;

static int luaconstructor_txd( lua_State *L )
{
    CClientTXD *txd = (CClientTXD*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_TXD, txd );

    j.RegisterInterfaceTrans( L, txd_interface_trans, 0, LUACLASS_TXD );

    lua_pushlstring( L, "txd", 3 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientTXD::CClientTXD( lua_State *L, CTexDictionary& txd, CResource *owner ) : CClientRwObject( L, txd ), m_txd( txd )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_txd, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Apply the owner
    SetOwner( owner );

    // Create game textures
    textures_t& tex = txd.GetTextures();
    textures_t::iterator iter = tex.begin();

    for ( ; iter != tex.end(); iter++ )
    {
        CClientGameTexture *tex = new CClientGameTexture( L, **iter );
        tex->SetTXD( this );
        tex->SetOwner( owner );
        tex->DisableKeepAlive();
    }
}

CClientTXD::~CClientTXD()
{
    while ( !LIST_EMPTY( m_textures.root ) )
    {
        CClientGameTexture *item = LIST_GETITEM( CClientGameTexture, m_textures.root.next, m_textures );

        // We have to unlink our textures here, so they do not crash
        item->RemoveFromTXD();

        if ( item->GetRefCount() == 0 )
            item->Destroy();
    }
}

bool CClientTXD::Import( unsigned short id )
{
    if ( !m_txd.Import( id ) )
        return false;

    g_pClientGame->GetManager()->Restream( id );
    return true;
}

bool CClientTXD::IsImported( unsigned short id ) const
{
    return m_txd.IsImported( id );
}

void CClientTXD::Remove( unsigned short id )
{
    if ( !m_txd.Remove( id ) )
        return;

    g_pClientGame->GetManager()->Restream( id );
}

void CClientTXD::RemoveAll()
{
    imports_t imports = m_txd.GetImportedList();
    imports_t::const_iterator iter = imports.begin();

    m_txd.ClearImports();

    for ( ; iter != imports.end(); iter++ )
        g_pClientGame->GetManager()->Restream( *iter );
}

void CClientTXD::MarkGC( lua_State *L )
{
    // Propagate all textures
    LIST_FOREACH_BEGIN( CClientGameTexture, m_textures.root, m_textures )
        item->m_class->Propagate( L );
        lua_gcpaycost( L, sizeof(*item) );
    LIST_FOREACH_END

    if ( m_txd.IsUsed() )
        goto success;

    // Check if one of it's textures is used
    LIST_FOREACH_BEGIN( CClientGameTexture, m_textures.root, m_textures )
        if ( item->m_tex.IsUsed() )
            goto success;
    LIST_FOREACH_END

    if ( !m_keepAlive )
        return;

success:
    LuaClass::MarkGC( L );
}
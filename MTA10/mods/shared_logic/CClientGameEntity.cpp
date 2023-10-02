/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGameEntity.cpp
*  PURPOSE:     Game rendered entity instance
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static LUA_DECLARE( isFading )
{
    CEntity *entity = ((CClientGameEntity*)lua_getmethodtrans( L ))->GetGameEntity();

    lua_pushboolean( L, entity && entity->IsFading() );
    return 1;
}

static LUA_DECLARE( getFadingAlpha )
{
    CEntity *entity = ((CClientGameEntity*)lua_getmethodtrans( L ))->GetGameEntity();

    if ( !entity )
        lua_pushboolean( L, false );
    else
        lua_pushnumber( L, entity->GetFadingAlpha() );

    return 1;
}

static LUA_DECLARE( setColModel )
{
    CClientColModel *col;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( col, LUACLASS_COLMODEL, NULL );
    LUA_ARGS_END;

    ((CClientGameEntity*)lua_getmethodtrans( L ))->SetColModel( col );
    LUA_SUCCESS;
}

static LUA_DECLARE( getColModel )
{
    CClientColModel *col = ((CClientGameEntity*)lua_getmethodtrans( L ))->GetColModel();

    if ( !col )
        return 0;

    col->PushStack( L );
    return 1;
}

static LUA_DECLARE( replaceTexture )
{
    const char *texName;
    CClientGameTexture *tex;

    LUA_ARGS_BEGIN;
    argStream.ReadString( texName );
    argStream.ReadClass( tex, LUACLASS_TEXTURE );
    LUA_ARGS_END;

    ((CClientGameEntity*)lua_getmethodtrans( L ))->ReplaceTexture( texName, tex );
    LUA_SUCCESS;
}

static LUA_DECLARE( isRwObjectPresent )
{
    CEntity *entity = ((CClientGameEntity*)lua_getmethodtrans( L ))->GetGameEntity();

    lua_pushboolean( L, entity && entity->IsRwObjectPresent() );
    return 1;
}

static luaL_Reg gameentity_interface_trans[] =
{
    LUA_METHOD( isFading ),
    LUA_METHOD( getFadingAlpha ),
    LUA_METHOD( setColModel ),
    LUA_METHOD( getColModel ),
    LUA_METHOD( replaceTexture ),
    LUA_METHOD( isRwObjectPresent ),
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_gameentity )
{
    CClientGameEntity *entity = (CClientGameEntity*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_GAMEENTITY, entity );

    j.RegisterInterfaceTrans( L, gameentity_interface_trans, 0, LUACLASS_GAMEENTITY );

    lua_pushlstring( L, "gameentity", 10 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientGameEntity::CClientGameEntity( CClientStreamer *pStreamer, ElementID ID, lua_State *L, bool system ) : CClientStreamElement( pStreamer, ID, L, system )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_gameentity, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_colModel = NULL;
}

CClientGameEntity::~CClientGameEntity()
{
    SetColModel( NULL );

    // Unreference all replaced textures
    for ( textureMap_t::const_iterator iter = m_replacedTextures.begin(); iter != m_replacedTextures.end(); iter++ )
        (*iter).second->DecrementMethodStack();
}

void CClientGameEntity::SetColModel( CClientColModel *col )
{
    if ( col == m_colModel )
        return;

    CEntity *entity = GetGameEntity();

    if ( m_colModel )
    {
        if ( entity )
            entity->SetColModel( NULL );

        m_colModel->DecrementMethodStack();
        m_colModel = NULL;
    }

    if ( !col )
        return;

    m_colModel = col;
    col->IncrementMethodStack();

    if ( entity )
        entity->SetColModel( col->m_pColModel );
}

void CClientGameEntity::ReplaceTexture( const char *name, CClientGameTexture *tex )
{
    // Reference our texture as we will be using it
    tex->IncrementMethodStack();

    // Remove any previous replacement on that name (if found)
    RestoreTexture( name );

    // Install the texture
    m_replacedTextures.insert( textureMap_t::value_type( name, tex ) );

    CEntity *entity = GetGameEntity();

    if ( entity )
        entity->ReplaceTexture( name, &tex->m_tex );
}

bool CClientGameEntity::IsReplacingTexture( const char *name )
{
    return m_replacedTextures.find( name ) != m_replacedTextures.end();
}

void CClientGameEntity::RestoreTexture( const char *name )
{
    textureMap_t::iterator iter = m_replacedTextures.find( name );
    
    if ( iter == m_replacedTextures.end() )
        return;

    // Dereference the texture
    CClientGameTexture *tex = iter->second;
    tex->DecrementMethodStack();

    // Tell the internal element to restore
    CEntity *entity = GetGameEntity();

    if ( entity )
        entity->RestoreTexture( name );
}

void CClientGameEntity::NotifyCreate()
{
    CEntity *entity = GetGameEntity();

    // Apply collision if present
    if ( m_colModel )
        entity->SetColModel( m_colModel->m_pColModel );

    CClientStreamElement::NotifyCreate();
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientLight.cpp
*  PURPOSE:     RenderWare light virtualization
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( clone )
{
    CClientLight *light = new CClientLight( L, NULL, *((CClientLight*)lua_getmethodtrans( L ))->m_light.Clone() );
    light->PushStack( L );
    light->DisableKeepAlive();
    return 1;
}

static LUA_DECLARE( getLightType )
{
    const std::string& typeName = EnumToString( ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetLightType() );

    lua_pushlstring( L, typeName.c_str(), typeName.size() );
    return 1;
}

static LUA_DECLARE( setClump )
{
    CClientDFF *model;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( model, LUACLASS_DFF, NULL );
    LUA_ARGS_END;

    CClientLight *light = (CClientLight*)lua_getmethodtrans( L );

    // Force the light into the clump's hierarchy
    // If model is nil, we remove the connection
    if ( model )
    {
        light->SetModel( model );
        light->m_light.AddToModel( &model->m_model );   // do the internal magic :)

        light->SetParent( model->m_parent );
        light->SetRoot( model->m_parent );
        model->m_lights.insert( model->m_lights.begin(), light );
    }
    else if ( light->m_clump )
    {
        light->m_clump->m_lights.remove( light );
        light->SetRoot( NULL );

        light->m_light.RemoveFromModel();
        light->SetModel( NULL );
    }

    LUA_SUCCESS;
}

static LUA_DECLARE( getClump )
{
    CClientDFF *model = ((CClientLight*)lua_getmethodtrans( L ))->m_clump;

    if ( !model )
        return 0;

    model->PushStack( L );
    return 1;
}

static LUA_DECLARE( setRadius )
{
    float radius;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( radius );
    LUA_ARGS_END;

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetRadius( radius );
    LUA_SUCCESS;
}

static LUA_DECLARE( getRadius )
{
    lua_pushnumber( L, ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetRadius() );
    return 1;
}

static LUA_DECLARE( setConeAngle )
{
    float angle;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( angle );
    LUA_ARGS_END;

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetConeAngle( angle );
    LUA_SUCCESS;
}

static LUA_DECLARE( getConeAngle )
{
    lua_pushnumber( L, ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetConeAngle() );
    return 1;
}

static LUA_DECLARE( setColor )
{
    RwColorFloat color;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( color.r );
    argStream.ReadNumber( color.g );
    argStream.ReadNumber( color.b );
    argStream.ReadNumber( color.a );
    LUA_ARGS_END;

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetColor( color );
    LUA_SUCCESS;
}

static LUA_DECLARE( getColor )
{
    const RwColorFloat& color = ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetColor();

    lua_pushnumber( L, color.r );
    lua_pushnumber( L, color.g );
    lua_pushnumber( L, color.b );
    lua_pushnumber( L, color.a );
    return 4;
}

static LUA_DECLARE( setAttenuation )
{
    CVector atten;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( atten );
    LUA_ARGS_END;

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetAttenuation( atten );
    LUA_SUCCESS;
}

static LUA_DECLARE( getAttenuation )
{
    const CVector& atten = ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetAttenuation();

    lua_pushnumber( L, atten[0] );
    lua_pushnumber( L, atten[1] );
    lua_pushnumber( L, atten[2] );
    return 3;
}

static LUA_DECLARE( setFalloff )
{
    float falloff;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( falloff );
    LUA_ARGS_END;

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetFalloff( falloff );
    LUA_SUCCESS;
}

static LUA_DECLARE( getFalloff )
{
    lua_pushnumber( L, ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetFalloff() );
    return 1;
}

static LUA_DECLARE( setLightIndex )
{
    unsigned int idx;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( idx );
    LUA_ARGS_END;

    LUA_ASSERT( idx < 8, "invalid light index" );

    ((CClientLight*)lua_getmethodtrans( L ))->m_light.SetLightIndex( idx );
    LUA_SUCCESS;
}

static LUA_DECLARE( getLightIndex )
{
    lua_pushnumber( L, ((CClientLight*)lua_getmethodtrans( L ))->m_light.GetLightIndex() );
    return 1;
}

static LUA_DECLARE( addToScene )
{
    ((CClientLight*)lua_getmethodtrans( L ))->m_light.AddToScene();
    LUA_SUCCESS;
}

static LUA_DECLARE( isAddedToScene )
{
    lua_pushboolean( L, ((CClientLight*)lua_getmethodtrans( L ))->m_light.IsAddedToScene() );
    return 1;
}

static LUA_DECLARE( removeFromScene )
{
    ((CClientLight*)lua_getmethodtrans( L ))->m_light.RemoveFromScene();
    LUA_SUCCESS;
}

static luaL_Reg light_interface_trans[] =
{
    LUA_METHOD( clone ),
    LUA_METHOD( getLightType ),
    LUA_METHOD( setClump ),
    LUA_METHOD( getClump ),
    LUA_METHOD( setRadius ),
    LUA_METHOD( getRadius ),
    LUA_METHOD( setConeAngle ),
    LUA_METHOD( getConeAngle ),
    LUA_METHOD( setColor ),
    LUA_METHOD( getColor ),
    LUA_METHOD( setAttenuation ),
    LUA_METHOD( getAttenuation ),
    LUA_METHOD( setFalloff ),
    LUA_METHOD( getFalloff ),
    LUA_METHOD( setLightIndex ),
    LUA_METHOD( getLightIndex ),
    LUA_METHOD( addToScene ),
    LUA_METHOD( isAddedToScene ),
    LUA_METHOD( removeFromScene ),
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_light )
{
    CClientLight *light = (CClientLight*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_LIGHT, light );

    j.RegisterInterfaceTrans( L, light_interface_trans, 0, LUACLASS_LIGHT );

    lua_pushlstring( L, "rwlight", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientLight::CClientLight( lua_State *L, CClientDFF *model, CRpLight& light ) : CClientRwObject( L, light ), m_light( light )
{
    // Lua interfacing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_light, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_clump = NULL;

    // Assign the model if given
    SetModel( model );
}

CClientLight::~CClientLight()
{
    SetModel( NULL );
}

void CClientLight::SetModel( CClientDFF *model )
{
    if ( model == m_clump )
        return;

    if ( m_clump )
        m_clump->m_lights.remove( this );

    m_clump = model;

    if ( !model )
        return;

    model->m_lights.insert( model->m_lights.begin(), this );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElement.cpp
*  PURPOSE:     Client module's version of something renderable
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg render_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_render( lua_State *L )
{
    CClientRenderElement *render = (CClientRenderElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RENDERELEMENT, render );

    j.RegisterInterfaceTrans( L, render_interface, 0, LUACLASS_RENDERELEMENT );

    lua_pushlstring( L, "render-element", 14 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientRenderElement::CClientRenderElement( CClientManager* pManager, ElementID ID, lua_State *L ) : CClientEntity( ID, false, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_render, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_pRenderElementManager = pManager->GetRenderElementManager ();
    bDoneUnlink = false;
    m_pRenderItem = NULL;
}

CClientRenderElement::~CClientRenderElement()
{
    Unlink();
}

void CClientRenderElement::Unlink()
{
    if ( !bDoneUnlink )
    {
        bDoneUnlink = true;
        m_pRenderElementManager->Remove( this );
    }
}

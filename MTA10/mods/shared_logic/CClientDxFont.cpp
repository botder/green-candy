/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDxFont.cpp
*  PURPOSE:     Custom font bucket
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg dxfont_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_dxfont( lua_State *L )
{
    CClientDxFont *font = (CClientDxFont*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_DXFONT, font );

    j.RegisterInterfaceTrans( L, dxfont_interface, 0, LUACLASS_DXFONT );

    lua_pushlstring( L, "dx-font", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientDxFont::CClientDxFont( CClientManager *pManager, ElementID ID, lua_State *L, CDxFontItem *pFontItem ) : CClientRenderElement( pManager, ID, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_dxfont, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName ( "dx-font" );
    m_pRenderItem = pFontItem;
}

ID3DXFont* CClientDxFont::GetD3DXFont ( void )
{
    return GetDxFontItem ()->m_pFntNormal;
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/gui/CClientGUIScrollbar.cpp
*  PURPOSE:     GUI scrollbar extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg scrollbar_interface[] =
{
    { NULL, NULL }
};

int luaconstructor_guiscrollbar( lua_State *L )
{
    CClientGUIElement *gui = (CClientGUIElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_GUISCROLLBAR, gui );

    j.RegisterInterfaceTrans( L, scrollbar_interface, 0, LUACLASS_GUISCROLLBAR );

    lua_pushlstring( L, "gui-scrollbar", 13 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}
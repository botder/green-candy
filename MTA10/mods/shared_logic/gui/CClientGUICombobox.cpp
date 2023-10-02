/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/gui/CClientGUICombobox.cpp
*  PURPOSE:     GUI combobox extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg combobox_interface[] =
{
    { NULL, NULL }
};

int luaconstructor_guicombobox( lua_State *L )
{
    CClientGUIElement *gui = (CClientGUIElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_GUICOMBOBOX, gui );

    j.RegisterInterfaceTrans( L, combobox_interface, 0, LUACLASS_GUICOMBOBOX );

    lua_pushlstring( L, "gui-combobox", 12 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}
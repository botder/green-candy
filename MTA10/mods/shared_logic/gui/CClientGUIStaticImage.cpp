/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/gui/CClientGUIStaticImage.cpp
*  PURPOSE:     GUI static image extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg staticimage_interface[] =
{
    { NULL, NULL }
};

int luaconstructor_guistaticimage( lua_State *L )
{
    CClientGUIElement *gui = (CClientGUIElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_GUISTATICIMAGE, gui );

    j.RegisterInterfaceTrans( L, staticimage_interface, 0, LUACLASS_GUISTATICIMAGE );

    lua_pushlstring( L, "gui-staticimage", 15 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}
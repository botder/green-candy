/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientShader.cpp
*  PURPOSE:     Core shader rendering management for deathmatch
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg shader_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_shader( lua_State *L )
{
    CClientShader *shader = (CClientShader*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_SHADER, shader );

    j.RegisterInterfaceTrans( L, shader_interface, 0, LUACLASS_SHADER );

    lua_pushlstring( L, "shader", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientShader::CClientShader( CClientManager* pManager, ElementID ID, lua_State *L, CShaderItem* pShaderItem ) : CClientMaterial( pManager, ID, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_shader, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName( "shader" );

    m_pRenderItem = pShaderItem;
}

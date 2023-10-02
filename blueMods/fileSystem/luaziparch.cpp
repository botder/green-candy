/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luaziparch.cpp
*  PURPOSE:     Lua implementation of the FileSystem CZIPArchiveTranslator class
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CFileSystemInterface *pubFileSystem;

static const luaL_Reg ziparch_methods[] =
{
    { nullptr, nullptr }
};

void luaziparch_makemeta( lua_State *L, int ridx )
{
    lua_newtable( L );
    lua_pushvalue( L, ridx - 1 );
    luaL_openlib( L, nullptr, ziparch_methods, 1 );
}
/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.cpp
*  PURPOSE:     Lua filesystem utils
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUAFILE_UTILS_
#define _LUAFILE_UTILS_

static inline void luafile_pushStats( lua_State *L, const struct stat& stats )
{
    lua_newtable( L );

    int top = lua_gettop( L );

    lua_pushnumber( L, (lua_Number)stats.st_atime );
    lua_setfield( L, top, "accessTime" );
    lua_pushnumber( L, (lua_Number)stats.st_ctime );;
    lua_setfield( L, top, "creationTime" );
    lua_pushnumber( L, (lua_Number)stats.st_mtime );
    lua_setfield( L, top, "modTime" );
    lua_pushnumber( L, stats.st_size );
    lua_setfield( L, top, "size" );
}

#endif //_LUAFILE_UITLS_

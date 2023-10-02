/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafslib.h
*  PURPOSE:     Lua Eir FileSystem module main implementation file
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_EIR_FILESYSTEM_FSLIB_HEADER_
#define _LUA_EIR_FILESYSTEM_FSLIB_HEADER_

struct fslib_config
{
    bool doBufferAllRaw = false;
};

fslib_config* fsLuaGetConfig( lua_State *L );

#endif //_LUA_EIR_FILESYSTEM_FSLIB_HEADER_
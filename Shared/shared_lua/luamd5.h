/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luamd5.h
*  PURPOSE:     Lua md5 hasher instance
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_MD5_HASHER_
#define _LUA_MD5_HASHER_

#define LUACLASS_MD5HASHER  138

using SharedUtil::CMD5Hasher;

void lua_createmd5hasher( lua_State *L, CMD5Hasher *hasher );

#endif //_LUA_MD5_HASHER_
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaArguments.h
*  PURPOSE:     Lua networking
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAARGUMENTS_H
#define __CLUAARGUMENTS_H

#include <net/bitstream.h>

#define LUA_TTABLEREF 9

#define MAX_CUSTOMDATA_NAME_LENGTH 128

// Reading
bool    RakNet_ReadArgument( NetBitStreamInterface& stream, lua_State *L );
bool    RakNet_ReadArguments( NetBitStreamInterface& stream, lua_State *L, unsigned short& count );

// Writing
void    RakNet_WriteArgument( NetBitStreamInterface& stream, lua_State *L, int idx );
void    RakNet_WriteArguments( NetBitStreamInterface& stream, lua_State *L, int startIdx, unsigned short count );

#endif



/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaMatrix.h
*  PURPOSE:     Lua RwMatrix instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_MATRIX_
#define _LUA_MATRIX_

#define LUACLASS_MATRIX     24

void lua_creatematrix( lua_State *L, const RwMatrix& matrix );

#endif //_LUA_MATRIX_
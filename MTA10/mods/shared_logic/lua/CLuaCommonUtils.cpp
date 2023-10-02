/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/LuaCommon.cpp
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Temporary until we change these funcs:
#include "../luadefs/CLuaDefs.h"
// End of temporary

#include <lua.h>
#include <sdk/xml/CXMLNode.h>

// Prevent the warning issued when doing unsigned short -> void*
#pragma warning(disable:4312)

//#pragma message(__LOC__"Use RTTI/dynamic_casting here for safety?")

CXMLNode* lua_toxmlnode( lua_State *L, int arg )
{
    return g_pCore->GetXML()->GetNodeFromID( (unsigned long)lua_touserdata( L, arg ) );
}

void lua_pushxmlnode( lua_State *L, CXMLNode *xml )
{
    lua_pushlightuserdata( L, (void*)xml->GetID() );
}
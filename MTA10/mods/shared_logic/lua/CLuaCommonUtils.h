/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/LuaCommon.h
*  PURPOSE:     Lua common functions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUACOMMON_H
#define __CLUACOMMON_H

// Predeclarations of our classes
class CClientColModel;
class CClientColShape;
class CScriptFile;
class CClientDFF;
class CClientEntity;
class CClientGUIElement;
class CClientMarker;
class CClientObject;
class CClientPed;
class CClientPickup;
class CClientPlayer;
class CClientRadarMarker;
class CClientTeam;
class CClientTXD;
class CClientVehicle;
class CClientWater;
class CClientRadarArea;
class CResource;
class CXMLNode;

// temporary
enum eAccessType
{
    ACCESS_PUBLIC,
    ACCESS_PRIVATE
};


// Lua push/pop macros for our datatypes
CXMLNode*               lua_toxmlnode       ( lua_State* luaVM, int iArgument );

void                    lua_pushxmlnode     ( lua_State* luaVM, CXMLNode* pElement );

#define lua_istype(luavm, number,type) (lua_type(luavm,number) == type)

#endif

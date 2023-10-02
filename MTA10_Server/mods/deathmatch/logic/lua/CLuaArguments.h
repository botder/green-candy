/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArguments.h
*  PURPOSE:     Lua argument manager class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAARGUMENTS_H
#define __CLUAARGUMENTS_H

#include "CLuaArgument.h"
#include <vector>
#include "../common/CBitStream.h"
#include "json.h"
#include "CLuaFunctionRef.h"

#if MTA_DEBUG
    // Tight allocation in debug to find trouble.
    #define LUA_CHECKSTACK(vm,space) lua_checkstack(vm, (space) )
#else
    // Extra room in release to avoid trouble.
    #define LUA_CHECKSTACK(vm,space) lua_checkstack(vm, (space)*2 )
#endif

class CAccessControlList;
class CAccessControlListGroup;
class CAccount;
class CElement;
class CLuaTimer;
class CResource;
class CTextDisplay;
class CTextItem;

class CLuaArguments : public LuaArguments
{
public:
                                                        CLuaArguments( NetBitStreamInterface& stream );

    void                                                ReadArgument( lua_State *L, int idx );

    CLuaArgument*                                       PushNil();
    CLuaArgument*                                       PushBoolean( bool b );
    CLuaArgument*                                       PushNumber( double num );
    CLuaArgument*                                       PushString( const std::string& str );
    CLuaArgument*                                       PushUserData( void *data );
    CLuaArgument*                                       PushArgument( const LuaArgument& argument );
    CLuaArgument*                                       PushTable( const LuaArguments& table );
    CLuaArgument*                                       PushElement( CElement* pElement );
    CLuaArgument*                                       PushACL( CAccessControlList* pACL );
    CLuaArgument*                                       PushACLGroup( CAccessControlListGroup* pACLGroup );
    CLuaArgument*                                       PushAccount( CAccount* pAccount );
    CLuaArgument*                                       PushResource( CResource* pResource );
    CLuaArgument*                                       PushTextDisplay( CTextDisplay* pTextDisplay );
    CLuaArgument*                                       PushTextItem( CTextItem* pTextItem );
    CLuaArgument*                                       PushTimer( CLuaTimer* pLuaTimer );

    bool                                                ReadFromBitStream( NetBitStreamInterface& bitStream );
    bool                                                WriteToBitStream( NetBitStreamInterface& bitStream ) const;

    bool                                                ReadFromJSONString( const char* szJSON );
    bool                                                WriteToJSONString( std::string& strJSON, bool bSerialize = false );
    json_object*                                        WriteTableToJSONObject( bool bSerialize = false, std::map < CLuaArguments*, unsigned long > * pKnownTables = NULL );
    json_object*                                        WriteToJSONArray( bool bSerialize );
    bool                                                ReadFromJSONObject( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );
    bool                                                ReadFromJSONArray( json_object * object, std::vector < CLuaArguments* > * pKnownTables = NULL );
};

#endif

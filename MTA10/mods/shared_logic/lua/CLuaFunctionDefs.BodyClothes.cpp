/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.BodyClothes.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( getBodyPartName )
    {
        unsigned char id;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( id );

        if ( !argStream.HasErrors() )
        {
            char szBuffer[256];
            if ( CStaticFunctionDefinitions::GetBodyPartName( id, szBuffer ) )
            {
                lua_pushstring( L, szBuffer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( getClothesByTypeIndex )
    {
        unsigned char type, index;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( type );
        argStream.ReadNumber( index );

        if ( !argStream.HasErrors() )
        {
            char szTexture[128], szModel[128];
            if ( CStaticFunctionDefinitions::GetClothesByTypeIndex( type, index, szTexture, szModel ) )
            {
                lua_pushstring( L, szTexture );
                lua_pushstring( L, szModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTypeIndexFromClothes )
    {
        const char *texture, *model;

        CScriptArgReader argStream( L );
        argStream.ReadString( texture );
        argStream.ReadString( model, NULL );

        if ( !argStream.HasErrors() )
        {
            unsigned char ucType, ucIndex;
            if ( CStaticFunctionDefinitions::GetTypeIndexFromClothes( texture, model, ucType, ucIndex ) )
            {
                lua_pushnumber( L, ucType );
                lua_pushnumber( L, ucIndex );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( getClothesTypeName )
    {
        unsigned char type;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( type );

        if ( !argStream.HasErrors() )
        {
            char szName[40];
            if ( CStaticFunctionDefinitions::GetClothesTypeName( type, szName ) )
            {
                lua_pushstring( L, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }
}
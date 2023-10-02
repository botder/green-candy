/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Weapon.cpp
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
    LUA_DECLARE( getWeaponNameFromID )
    {
        unsigned char id;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( id );

        if ( !argStream.HasErrors() )
        {
            char szBuffer[256];
            if ( CStaticFunctionDefinitions::GetWeaponNameFromID( id, szBuffer, sizeof(szBuffer) ) )
            {
                lua_pushstring( L, szBuffer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSlotFromWeapon )
    {
        unsigned char id;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( id );

        if ( !argStream.HasErrors() )
        {
            char cSlot = CWeaponNames::GetSlotFromWeapon( id );

            if ( cSlot >= 0 )
                lua_pushnumber( L, cSlot );
            else
                lua_pushboolean( L, false );

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( getWeaponIDFromName )
    {
        const char *name;

        CScriptArgReader argStream( L );
        argStream.ReadString( name );

        if ( !argStream.HasErrors() )
        {
            unsigned char ucID;

            if ( CStaticFunctionDefinitions::GetWeaponIDFromName( name, ucID ) )
            {
                lua_pushnumber( L, ucID );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
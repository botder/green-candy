/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Pickup.cpp
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
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( createPickup )
    {
        return 0;

        CVector pos;
        unsigned char type;
        double weaponOrAmmo;
        unsigned long respawnInterval;
        double ammo;

        CScriptArgReader argStream( L );

        argStream.ReadVector( pos );
        argStream.ReadNumber( type );
        argStream.ReadNumber( weaponOrAmmo );
        argStream.ReadNumber( respawnInterval, 30000 );
        argStream.ReadNumber( ammo, 50.0 );

        // The first 6 are always numeric saying position, type and weapon/health/armor
        // TODO: Check argument 7 incase type is weapon
        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource();
            CClientPickup* pPickup = CStaticFunctionDefinitions::CreatePickup( *pResource, pos, type, weaponOrAmmo, respawnInterval, ammo );
            if ( pPickup )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( ( CClientEntity* ) pPickup );
                }

                // Return the handle
                pPickup->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPickupType )
    {
        // Verify the argument
        if ( CClientPickup *pickup = lua_readclass <CClientPickup> ( L, 1, LUACLASS_PICKUP ) )
        {
            lua_pushnumber( L, pickup->m_ucType );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPickupType", "pickup", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPickupWeapon )
    {
        // Verify the argument
        if ( CClientPickup *pickup = lua_readclass <CClientPickup> ( L, 1, LUACLASS_PICKUP ) )
        {
            lua_pushnumber( L, pickup->m_ucWeaponType );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPickupWeapon", "pickup", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPickupAmount )
    {
        // Verify the argument
        if ( CClientPickup *pickup = lua_readclass <CClientPickup> ( L, 1, LUACLASS_PICKUP ) )
        {
            lua_pushnumber( L, pickup->m_fAmount );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPickupAmount", "pickup", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPickupAmmo )
    {
        // Verify the argument
        if ( CClientPickup *pickup = lua_readclass <CClientPickup> ( L, 1, LUACLASS_PICKUP ) )
        {
            lua_pushnumber ( L, pickup->m_usAmmo );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPickupAmmo", "pickup", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPickupType )
    {
        CClientEntity *entity;
        unsigned char type;
        double healthWeaponModel;
        double ammo;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( type );
        argStream.ReadNumber( healthWeaponModel );
        argStream.ReadNumber( ammo );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPickupType( *entity, type, healthWeaponModel, ammo ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
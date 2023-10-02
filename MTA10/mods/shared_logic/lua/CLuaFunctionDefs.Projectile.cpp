/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Projectile.cpp
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
    LUA_DECLARE( createProjectile )
    {
        CClientEntity *creator;
        unsigned char weaponType;
        CVector origin;
        float force;
        CClientEntity *target;
        CVector rotation;
        CVector moveSpeed;
        unsigned short model;

        CScriptArgReader argStream( L );

        if ( argStream.ReadClass( creator, LUACLASS_ENTITY ) )
            creator->GetPosition( origin );
        
        argStream.ReadNumber( weaponType );
        argStream.ReadVector( origin, origin );
        argStream.ReadNumber( force, 1.0f );
        argStream.ReadClass( target, NULL );
        bool gotRotation = argStream.ReadVector( rotation, CVector() );
        bool gotMoveSpeed = argStream.ReadVector( moveSpeed, CVector() );
        bool gotModel = argStream.ReadNumber( model, 0 );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource * pResource = pLuaMain->GetResource();
            CClientProjectile * pProjectile = CStaticFunctionDefinitions::CreateProjectile( *pResource, *creator, weaponType, origin, force, target,
                gotRotation ? &rotation : NULL, gotMoveSpeed ? &moveSpeed : NULL, model );

            if ( pProjectile )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( ( CClientEntity* ) pProjectile );
                }

                pProjectile->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getProjectileType )
    {
        // Verify the argument
        if ( CClientProjectile *proj = lua_readclass <CClientProjectile> ( L, 1, LUACLASS_PROJECTILE ) )
        {
            lua_pushnumber( L, proj->GetWeaponType() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getProjectileType", "projectile", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getProjectileTarget )
    {
        // Verify the argument
        if ( CClientProjectile *proj = lua_readclass <CClientProjectile> ( L, 1, LUACLASS_PROJECTILE ) )
        {
		    if ( proj->GetWeaponType() == WEAPONTYPE_ROCKET_HS && proj->GetTargetEntity() )
		    {
			    proj->GetTargetEntity()->PushStack( L );
                return 1;
		    }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getProjectileTarget", "projectile", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getProjectileCreator )
    {
        // Verify the argument
        if ( CClientProjectile *proj = lua_readclass <CClientProjectile> ( L, 1, LUACLASS_PROJECTILE ) )
        {
            if ( proj->GetCreator() )
            {
		        proj->GetCreator()->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getProjectileCreator", "projectile", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getProjectileForce )
    {
        // Verify the argument
        if ( CClientProjectile *proj = lua_readclass <CClientProjectile> ( L, 1, LUACLASS_PROJECTILE ) )
        {
		    lua_pushnumber( L, proj->GetForce() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getProjectileForce", "projectile", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }
}
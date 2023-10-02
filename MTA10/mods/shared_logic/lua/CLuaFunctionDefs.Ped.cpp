/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Ped.cpp
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
    LUA_DECLARE( getPedVoice )
    {
        // Right type?
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            if ( !ped->IsSpeechEnabled() )
            {
                lua_pushstring ( L, "PED_TYPE_DISABLED" );
                return 1;
            }

            const char *szVoiceType = NULL;
            const char *szVoiceBank = NULL;
            ped->GetVoice( &szVoiceType, &szVoiceBank );

            if ( szVoiceType && szVoiceBank )
            {
                lua_pushstring ( L, szVoiceType );
                lua_pushstring ( L, szVoiceBank );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedVoice", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedVoice )
    {
        CClientPed *ped;
        SString type;
        const char *bank;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadString( type );
        argStream.ReadString( bank, "" );

        // Right type?
        if ( !argStream.HasErrors() )
        {
            if ( type == "PED_TYPE_DISABLED" )
            {
                ped->SetSpeechEnabled( false );
                lua_pushboolean( L, true );
                return 1;
            }
            else if ( bank )
            {
                ped->SetSpeechEnabled( true );
                ped->SetVoice( type, bank );
                lua_pushboolean( L, true );
                return 1;
            }

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedWeapon )
    {
        // Right type?
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab the slot if specified
            unsigned char ucSlot = 0xFF;
            if ( lua_isnumber( L, 2 ) )
                ucSlot = (unsigned char)lua_tonumber( L, 2 );

            if ( ucSlot == 0xFF )
                ucSlot = ped->GetCurrentWeaponSlot();

            CWeapon *pWeapon = ped->GetWeapon( (eWeaponSlot)ucSlot );
            if ( pWeapon )
            {
                lua_pushnumber( L, (unsigned char)pWeapon->GetType() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedWeapon", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedWeaponSlot )
    {
        // Right type?
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushnumber ( L, ped->GetCurrentWeaponSlot() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedWeaponSlot", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedAmmoInClip )
    {
        CClientPed *ped;
        unsigned char slot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadNumber( slot, 0xFF );

        if ( !argStream.HasErrors() )
        {
            if ( slot == 0xFF )
                slot = ped->GetCurrentWeaponSlot();

            CWeapon *pWeapon = ped->GetWeapon( (eWeaponSlot)slot );
            if ( pWeapon )
            {
                lua_pushnumber( L, (unsigned short)pWeapon->GetAmmoInClip() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTotalAmmo )
    {
        CClientPed *ped;
        unsigned char slot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadNumber( slot, 0xFF );

        if ( !argStream.HasErrors() )
        {
            if ( slot == 0xFF )
                slot = ped->GetCurrentWeaponSlot();

            // Grab the ammo and return
            CWeapon *pWeapon = ped->GetWeapon( (eWeaponSlot)slot );
            if ( pWeapon )
            {
                lua_pushnumber( L, (unsigned short)pWeapon->GetAmmoTotal() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedWeaponMuzzlePosition )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            CVector vecMuzzlePos;
            if ( CStaticFunctionDefinitions::GetPedWeaponMuzzlePosition( *ped, vecMuzzlePos ) )
            {
                lua_pushnumber( L, vecMuzzlePos.fX );
                lua_pushnumber( L, vecMuzzlePos.fY );
                lua_pushnumber( L, vecMuzzlePos.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedWeaponMuzzlePosition", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedOccupiedVehicle )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his occupied vehicle
            CClientVehicle *pVehicle = ped->GetOccupiedVehicle();
            if ( pVehicle )
            {
                pVehicle->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedOccupiedVehicle", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTask )
    {
        CClientPed *ped;
        SString priority;
        unsigned int taskType;
        
        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadString( priority );
        argStream.ReadNumber( taskType );

        if ( !argStream.HasErrors() )
        {
            bool bPrimary;
            if ( ( bPrimary = priority.CompareI( "primary" ) ) || priority.CompareI( "secondary" ) )
            {
                // Grab the taskname list and return it
                std::vector < SString > taskHierarchy;
                if ( CStaticFunctionDefinitions::GetPedTask( *ped, bPrimary, taskType, taskHierarchy ) )
                {
                    for ( unsigned int i = 0; i < taskHierarchy.size(); i++ )
                        lua_pushlstring( L, taskHierarchy[i].c_str(), taskHierarchy[i].size() );

                    return taskHierarchy.size();
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedSimplestTask )
    {
        // Check types
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his simplest task and return it
            const char *szTaskName = CStaticFunctionDefinitions::GetPedSimplestTask( *ped );
            if ( szTaskName )
            {
                lua_pushstring( L, szTaskName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedSimplestTask", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedDoingTask )
    {
        CClientPed *ped;
        const char *taskName;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadString( taskName );

        // Check types
        if ( !argStream.HasErrors() )
        {
            // Check whether he's doing that task or not
            bool bIsDoingTask;
            if ( CStaticFunctionDefinitions::IsPedDoingTask( *ped, taskName, bIsDoingTask ) )
            {
                lua_pushboolean( L, bIsDoingTask );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTarget )
    {
        // Check types
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his target element
            CClientEntity *pEntity = CStaticFunctionDefinitions::GetPedTarget( *ped );
            if ( pEntity )
            {
                pEntity->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedTarget", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTargetStart )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his start aim position and return it
            CVector vecStart;
            ped->GetShotData( &vecStart );

            lua_pushnumber( L, vecStart.fX );
            lua_pushnumber( L, vecStart.fY );
            lua_pushnumber( L, vecStart.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedTargetStart", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTargetEnd )
    {
        // Check types
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab the ped end target position and return it
            CVector vecEnd;
            ped->GetShotData ( NULL, &vecEnd );

            lua_pushnumber ( L, vecEnd.fX );
            lua_pushnumber ( L, vecEnd.fY );
            lua_pushnumber ( L, vecEnd.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedTargetEnd", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTargetRange )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // TODO: getPedTargetRange
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedTargetRange", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedTargetCollision )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his target collision and return it
            CVector vecCollision;
            if ( CStaticFunctionDefinitions::GetPedTargetCollision ( *ped, vecCollision ) )
            {
                lua_pushnumber ( L, vecCollision.fX );
                lua_pushnumber ( L, vecCollision.fY );
                lua_pushnumber ( L, vecCollision.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedTargetCollision", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedArmor )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab the armor and return it
            lua_pushnumber( L, ped->GetArmor() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedArmor", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedStat )
    {
        CClientPed *ped;
        unsigned short stat;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadNumber( stat );

        if ( !argStream.HasErrors() )
        {
            // Check the stat
            if ( stat < NUM_PLAYER_STATS )
            {
                lua_pushnumber( L, ped->GetStat( stat ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedChoking )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Return whether he's choking or not
            lua_pushboolean( L, ped->IsChoking() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedChoking", "ped", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( isPedDucked )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Grab his ducked state
            lua_pushboolean ( L, ped->IsDucked() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedDucked", "ped", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( isPedInVehicle )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Return that state
            lua_pushboolean( L, ped->GetOccupiedVehicle() != NULL );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedInVehicle", "ped", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( doesPedHaveJetPack )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Find out whether he has a jetpack or not and return it
            lua_pushboolean ( L, ped->HasJetPack() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "doesPedHaveJetPack", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedOnGround )
    {
        // Check type
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            // Find out whether he's on the ground or not and return it
            lua_pushboolean ( L, ped->IsOnGround() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedOnGround", "ped", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedContactElement )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            CClientEntity *pEntity = ped->GetContactEntity();
            if ( pEntity )
            {
                pEntity->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedContactElement", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedRotation )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushnumber( L, ConvertRadiansToDegrees( ped->GetCurrentRotation() ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedRotation", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( canPedBeKnockedOffBike )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->GetCanBeKnockedOffBike() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "canPedBeKnockedOffBike", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedBonePosition )
    {
        CClientPed *ped;
        unsigned char bone;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadNumber( bone );

        if ( !argStream.HasErrors() )
        {
            if ( bone <= BONE_RIGHTFOOT )
            {
                CVector vecPosition;
                if ( CStaticFunctionDefinitions::GetPedBonePosition( *ped, (eBone)bone, vecPosition ) )
                {
                    lua_pushnumber( L, vecPosition.fX );
                    lua_pushnumber( L, vecPosition.fY );
                    lua_pushnumber( L, vecPosition.fZ );
                    return 3;
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

#define UNSIGN_INT(x) ( x & 0x7FFFFFFF )

    LUA_DECLARE( setPedWeaponSlot )
    {
        CClientEntity *entity;
        int slot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( slot );

        // Check type
        if ( !argStream.HasErrors() )
        {
            // Set his slot
            if ( CStaticFunctionDefinitions::SetPedWeaponSlot( *entity, UNSIGN_INT( slot ) ) )
            {
                lua_pushboolean( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedClothes )
    {
        CClientPed *ped;
        unsigned char type;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadNumber( type );

        if ( !argStream.HasErrors() )
        {
            char szTexture [ 128 ], szModel [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedClothes ( *ped, type, szTexture, szModel ) )
            {
                lua_pushstring ( L, szTexture );
                lua_pushstring ( L, szModel );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedControlState )
    {
        CClientPed *ped;
        const char *control;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );
        argStream.ReadString( control );

        if ( !argStream.HasErrors() )
        {
            bool bState;
            if ( CStaticFunctionDefinitions::GetPedControlState( *ped, control, bState ) )
            {
                lua_pushboolean( L, bState );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedDoingGangDriveby )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->IsDoingGangDriveby() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedDoingGangDriveby", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedAnimation )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            char szBlockName [ 128 ], szAnimName [ 128 ];
            if ( CStaticFunctionDefinitions::GetPedAnimation ( *ped, szBlockName, szAnimName, 128 ))
            {
                lua_pushstring ( L, szBlockName );
                lua_pushstring ( L, szAnimName );
                return 2;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedAnimation", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedMoveState )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            std::string strMoveState;
            if ( CStaticFunctionDefinitions::GetPedMoveState( *ped, strMoveState ) )
            {
                lua_pushlstring( L, strMoveState.c_str(), strMoveState.size() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "GetPedMoveState", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedMoveAnim )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushnumber( L, (unsigned int)ped->GetMoveAnim() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedWalkingStyle", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedHeadless )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->IsHeadless() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedHeadless", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedFrozen )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->IsFrozen() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedFrozen", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedFootBloodEnabled )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->IsFootBloodEnabled() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedFootBloodEnabled", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPedCameraRotation )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushnumber( L, RAD2DEG( ped->GetCameraRotation() ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPedCameraRotation", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPedOnFire )
    {
        if ( CClientPed *ped = lua_readclass <CClientPed> ( L, 1, LUACLASS_PED ) )
        {
            lua_pushboolean( L, ped->IsOnFire() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isPedOnFire", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedOnFire )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedOnFire( *entity, lua_toboolean( L, 2 ) == 1 ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "setPedOnFire", "ped", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( addPedClothes )
    {
        CClientEntity *entity;
        const char *texture, *model;
        unsigned char type;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( texture );
        argStream.ReadString( model );
        argStream.ReadNumber( type );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::AddPedClothes( *entity, texture, model, type ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( removePedClothes )
    {
        CClientEntity *entity;
        unsigned char type;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( type );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::RemovePedClothes( *entity, type ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedControlState )
    {
        CClientEntity *entity;
        const char *control;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( control );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedControlState( *entity, control, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setPedControlState" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedDoingGangDriveby )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedDoingGangDriveby( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedLookAt )
    {
        CClientEntity *entity;
        CVector lookAt;
        int time, blend;
        CClientEntity *target;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( lookAt );
        argStream.ReadNumber( time, 3000 );
        argStream.ReadNumber( blend, 1000 );
        argStream.ReadClass( target, LUACLASS_ENTITY, NULL );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedLookAt( *entity, lookAt, time, blend, target ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedHeadless )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedHeadless( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedFrozen )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedFrozen( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedFootBloodEnabled )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedFootBloodEnabled( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedCameraRotation )
    {
        CClientEntity *entity;
        float rotation;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( rotation );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedCameraRotation( *entity, rotation ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedAimTarget )
    {
        CClientEntity *entity;
        CVector target;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( target );
    
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedAimTarget( *entity, target ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedRotation )
    {
        CClientEntity *entity;
        float rotation;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( rotation );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedRotation( *entity, rotation ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedCanBeKnockedOffBike )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedCanBeKnockedOffBike( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedAnimation )
    {
        CClientEntity *entity;
        const char *block, *anim;
        int time;
        bool loop, updatePos, interruptable, freezeLastFrame;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( block, NULL );
        argStream.ReadString( anim, NULL );
        argStream.ReadNumber( time, -1 );
        argStream.ReadBool( loop, true ); argStream.ReadBool( updatePos, true ); argStream.ReadBool( interruptable, true );
        argStream.ReadBool( freezeLastFrame, true );

        // Check types
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedAnimation( *entity, block, anim, time, loop, updatePos, interruptable, freezeLastFrame ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedAnimationProgress )
    {
        CClientEntity *entity;
        const char *animName;
        float progress;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( animName, NULL );
        argStream.ReadNumber( progress, 0.0f );

        // Check types
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedAnimationProgress( *entity, animName, progress ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPedMoveAnim )
    {
        CClientEntity *entity;
        unsigned int moveAnim;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( moveAnim );

        // Check types
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetPedMoveAnim( *entity, moveAnim ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createPed )
    {
        unsigned short model;
        CVector pos;
        float rotation;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( model );
        argStream.ReadVector( pos );
        argStream.ReadNumber( rotation, 0.0f );

        // Valid types?
        if ( !argStream.HasErrors() )
        {
            // Create it
            CClientPed* pPed = CStaticFunctionDefinitions::CreatePed( *lua_readcontext( L )->GetResource(), model, pos, rotation );
            if ( pPed )
            {
                // Return it
                pPed->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( detonateSatchels )
    {
        lua_pushboolean( L, CStaticFunctionDefinitions::DetonateSatchels() );
        return 1;
    }
}
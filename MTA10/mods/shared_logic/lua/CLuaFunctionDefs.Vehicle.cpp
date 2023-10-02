/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Vehicle.cpp
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
    LUA_DECLARE( getVehicleType )
    {
        unsigned short ucModel = 0;
        if ( lua_type( L, 1 ) == LUA_TCLASS )
        {
            CClientVehicle* pVehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE );
            if ( pVehicle )
                ucModel = pVehicle->GetModel();
        }
        else if ( lua_isnumber( L, 1 ) )
        {
            ucModel = (unsigned short)lua_tonumber( L, 1 );
        }
        else
            m_pScriptDebugging->LogBadType( "getVehicleType" );

        CModelInfo *info = g_pGame->GetModelInfo( ucModel );

        if ( info->IsVehicle() )
        {
            switch( info->GetVehicleType() )
            {
            case VEHICLE_CAR:               lua_pushlstring( L, "Automobile", 10 ); break;
            case VEHICLE_PLANE:             lua_pushlstring( L, "Plane", 5 ); break;
            case VEHICLE_BIKE:              lua_pushlstring( L, "Bike", 4 ); break;
            case VEHICLE_BICYCLE:           lua_pushlstring( L, "BMX", 3 ); break;
            case VEHICLE_HELI:              lua_pushlstring( L, "Helicopter", 10 ); break;
            case VEHICLE_BOAT:              lua_pushlstring( L, "Boat", 4 ); break;
            case VEHICLE_TRAIN:             lua_pushlstring( L, "Train", 5 ); break;
            case VEHICLE_AUTOMOBILETRAILER: lua_pushlstring( L, "Trailer", 7 ); break;
            case VEHICLE_QUADBIKE:          lua_pushlstring( L, "Quad", 4 ); break;
            case VEHICLE_MONSTERTRUCK:      lua_pushlstring( L, "Monster Truck", 13 ); break;
            default:
                assert( 0 );
            }
        }
        else
            lua_pushboolean( L, false );

        return 1;
    }

    LUA_DECLARE( isVehicleTaxiLightOn )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            if ( vehicle->GetModel() == 438 || vehicle->GetModel() == 420 )
            {
                lua_pushboolean( L, vehicle->IsTaxiLightOn() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleTaxiLightOn", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleTaxiLightOn )
    {
        CClientVehicle *vehicle;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadBool( state, true );

        if ( !argStream.HasErrors() )
        {
            if ( vehicle->GetModel() == 438 || vehicle->GetModel() == 420 )
            {
                vehicle->SetTaxiLightOn( state );
                lua_pushboolean( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleColor )
    {
        CClientVehicle *vehicle;
        bool rgb;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadBool( rgb, false );

        if ( !argStream.HasErrors() )
        {
            CVehicleColor& color = vehicle->GetColor();

            if ( rgb )
            {
                for ( unsigned int i = 0; i < 4; i++ )
                {
                    SColor RGBColor = color.GetRGBColor ( i );
                    lua_pushnumber ( L, RGBColor.R );
                    lua_pushnumber ( L, RGBColor.G );
                    lua_pushnumber ( L, RGBColor.B );
                }
                return 12;
            }
            else
            {
                lua_pushnumber ( L, color.GetPaletteColor ( 0 ) );
                lua_pushnumber ( L, color.GetPaletteColor ( 1 ) );
                lua_pushnumber ( L, color.GetPaletteColor ( 2 ) );
                lua_pushnumber ( L, color.GetPaletteColor ( 3 ) );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleModelFromName )
    {
        if ( lua_isstring( L, 1 ) )
        {
            unsigned short usModel;
            if ( CStaticFunctionDefinitions::GetVehicleModelFromName( lua_tostring( L, 1 ), usModel ) )
            {
                lua_pushnumber( L, usModel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getVehicleModelFromName" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleLandingGearDown )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            // Return whether it has landing gears or not
            lua_pushboolean ( L, vehicle->IsLandingGearDown() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleLandingGear", "vehicle", 1 );

        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getVehicleMaxPassengers )
    {
        if ( lua_type ( L, 1 ) == LUA_TCLASS || lua_type ( L, 1 ) == LUA_TNUMBER )
        {
            unsigned short model = 0;

            if ( lua_type ( L, 1 ) == LUA_TCLASS )
            {
                CClientVehicle* pVehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE );

                if ( pVehicle )
                    model = pVehicle->GetModel();
                else
                {
                    m_pScriptDebugging->LogBadPointer( "getVehicleMaxPassengers", "vehicle", 1 );
                    lua_pushboolean( L, false );
                    return 1;
                }
            }
            else
            {
                model = (unsigned short) lua_tonumber ( L, 1 );

                if ( !CClientVehicleManager::IsValidModel(model) )
                {
                    m_pScriptDebugging->LogBadType( "getVehicleMaxPassengers" );
                    lua_pushboolean ( L, false );
                    return 1;
                }
            }
            
            unsigned int uiMaxPassengers = CClientVehicleManager::GetMaxPassengerCount( model );
            if ( uiMaxPassengers != 0xFF )
            {
                lua_pushnumber ( L, uiMaxPassengers );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getVehicleMaxPassengers" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleOccupant )
    {
        // Grab the seat argument if exists
        unsigned int uiSeat = 0;
        int iArgument2 = lua_type ( L, 2 );
        if ( lua_isnumber( L, 2 ) )
        {
            uiSeat = (unsigned int)lua_tonumber( L, 2 );
        }

        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CClientPed* pPed = vehicle->GetOccupant ( uiSeat );
            if ( pPed )
            {
                pPed->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleOccupant", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleOccupants )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            // Create a new table
            lua_newtable ( L );

            // Get the maximum amount of passengers
            unsigned char ucMaxPassengers = CClientVehicleManager::GetMaxPassengerCount( vehicle->GetModel() );

            // Make sure that if the vehicle doesn't have any seats, the function returns false
            if ( ucMaxPassengers == 255 )
            {
                lua_pushboolean ( L, false );
                return 1;
            }

            // Add All Occupants
            for ( unsigned char ucSeat = 0; ucSeat <= ucMaxPassengers; ++ucSeat )
            {
                CClientPed* pPed = vehicle->GetOccupant( ucSeat );
                if ( pPed )
                {
                    lua_pushnumber ( L, ucSeat );
                    pPed->PushStack( L );
                    lua_settable ( L, -3 );
                }
            }
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleOccupants", "vehicle", 1 );
                
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleController )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CClientPed *pPed = vehicle->GetControllingPlayer();
            if ( pPed )
            {
                pPed->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleController", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleSirensOn )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ))
        {
            // Does the vehicle have Sirens?
            if ( CClientVehicleManager::HasSirens ( vehicle->GetModel () ) )
            {
                // Return whether it has its Sirens on or not
                lua_pushboolean( L, vehicle->IsSirenOrAlarmActive() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleSirensOn", "vehicle", 1 );

        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getVehicleTurnVelocity )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CVector vecTurnVelocity;
            vehicle->GetTurnSpeed ( vecTurnVelocity );

            lua_pushnumber ( L, vecTurnVelocity.fX );
            lua_pushnumber ( L, vecTurnVelocity.fY );
            lua_pushnumber ( L, vecTurnVelocity.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleTurnVelocity", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleTurretPosition )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CVector2D vecPosition;
            vehicle->GetTurretRotation( vecPosition.fX, vecPosition.fY );

            lua_pushnumber( L, vecPosition.fX );
            lua_pushnumber( L, vecPosition.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleTurretPosition", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleLocked )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->AreDoorsLocked() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleLocked", "vehicle", 1 );

        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getVehicleUpgradeOnSlot )
    {
        CClientVehicle *vehicle;
        unsigned char slot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( slot );

        if ( !argStream.HasErrors() )
        {
            CVehicleUpgrades* pUpgrades = vehicle->GetUpgrades ();
            if ( pUpgrades )
            {
                lua_pushnumber ( L, (unsigned short)pUpgrades->GetSlotState( slot ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleUpgrades )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CVehicleUpgrades* pUpgrades = vehicle->GetUpgrades ();
            if ( pUpgrades )
            {
                // Create a new table
                lua_newtable ( L );

                // Add all the upgrades to the table
                unsigned short* usSlotStates = pUpgrades->GetSlotStates ();

                unsigned int uiIndex = 0;
                unsigned char ucSlot = 0;
                for ( ; ucSlot < VEHICLE_UPGRADE_SLOTS; ucSlot++ )
                {
                    if ( usSlotStates [ucSlot] != 0 )
                    {
                        lua_pushnumber ( L, ++uiIndex );
                        lua_pushnumber ( L, usSlotStates [ ucSlot ] );
                        lua_settable ( L, -3 );
                    }
                }

                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleUpgrades", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleUpgradeSlotName )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            unsigned long ulNumber = (unsigned long)lua_tonumber( L, 1 );

            if ( ulNumber < 17 )
            {
                char szUpgradeName [32];
                if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( (unsigned char)ulNumber, szUpgradeName, sizeof(szUpgradeName) ) )
                {
                    lua_pushstring ( L, szUpgradeName );
                    return 1;
                }
            }
            else if ( ulNumber >= 1000 && ulNumber <= 1193 )
            {
                char szUpgradeName [32];
                if ( CStaticFunctionDefinitions::GetVehicleUpgradeSlotName ( (unsigned char)ulNumber, szUpgradeName, sizeof(szUpgradeName) ) )
                {
                    lua_pushstring ( L, szUpgradeName );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogBadPointer( "getVehicleUpgradeSlotName", "slot/upgrade", 1 );
        }
        else
            m_pScriptDebugging->LogBadType( "getVehicleUpgradeSlotName" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleCompatibleUpgrades )
    {
        CClientVehicle *vehicle;
        unsigned char slot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( slot, 0xFF );

        if ( !argStream.HasErrors() )
        {
            CVehicleUpgrades *pUpgrades = vehicle->GetUpgrades();
            if ( pUpgrades )
            {
                // Create a new table
                lua_newtable ( L );

                unsigned int uiIndex = 0;
                for ( unsigned short usUpgrade = 1000 ; usUpgrade <= 1193 ; usUpgrade++ )
                {
                    if ( pUpgrades->IsUpgradeCompatible ( usUpgrade ) )
                    {
                        if ( slot != 0xFF )
                        {
                            unsigned char ucUpgradeSlot;
                            if ( !pUpgrades->GetSlotFromUpgrade ( usUpgrade, ucUpgradeSlot ) )
                                continue;

                            if ( ucUpgradeSlot != slot )
                                continue;
                        }

                        // Add this one to a list                        
                        lua_pushnumber ( L, ++uiIndex );
                        lua_pushnumber ( L, usUpgrade );
                        lua_settable ( L, -3 );
                    }
                }
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleWheelStates )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            unsigned char ucFrontLeft = vehicle->GetWheelStatus ( FRONT_LEFT_WHEEL );
            unsigned char ucRearLeft = vehicle->GetWheelStatus ( REAR_LEFT_WHEEL );
            unsigned char ucFrontRight = vehicle->GetWheelStatus ( FRONT_RIGHT_WHEEL );
            unsigned char ucRearRight = vehicle->GetWheelStatus ( REAR_RIGHT_WHEEL );

            lua_pushnumber ( L, ucFrontLeft );
            lua_pushnumber ( L, ucRearLeft );
            lua_pushnumber ( L, ucFrontRight );
            lua_pushnumber ( L, ucRearRight );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleWheelStates", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleDoorState )
    {
        CClientVehicle *vehicle;
        unsigned char door;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( door );

        if ( !argStream.HasErrors() )
        {
            lua_pushnumber( L, vehicle->GetDoorStatus( door ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleLightState )
    {
        CClientVehicle *vehicle;
        unsigned char light;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( light );

        if ( !argStream.HasErrors() )
        {
            lua_pushnumber( L, vehicle->GetLightStatus( light ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehiclePanelState )
    {
        CClientVehicle *vehicle;
        unsigned char panel;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( panel );

        if ( !argStream.HasErrors() )
        {
            lua_pushnumber( L, vehicle->GetPanelStatus( panel ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleOverrideLights )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushnumber ( L, vehicle->GetOverrideLights() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleOverrideLights", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleTowedByVehicle )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CClientVehicle *pTowedVehicle = vehicle->GetTowedVehicle();
            if ( pTowedVehicle )
            {
                pTowedVehicle->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleTowedByVehicle", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleTowingVehicle )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CClientVehicle* pTowedByVehicle = vehicle->GetTowedByVehicle();
            if ( pTowedByVehicle )
            {
                pTowedByVehicle->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleTowingVehicle", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehiclePaintjob )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushnumber( L, vehicle->GetPaintjob() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehiclePaintjob", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehiclePlateText )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            const char* szRegPlate = vehicle->GetRegPlate();
            if ( szRegPlate )
            {
                lua_pushstring( L, szRegPlate );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehiclePlateText", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleDamageProof )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, !vehicle->GetScriptCanBeDamaged() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleDamageProof", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleFuelTankExplodable )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean ( L, vehicle->GetCanShootPetrolTank() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleFuelTankExplodable", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleFrozen )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean ( L, vehicle->IsFrozen() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleFrozen", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleOnGround )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean ( L, vehicle->IsOnGround() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleOnGround", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleName )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            const char* szVehicleName = CVehicleNames::GetVehicleName( vehicle->GetModel() );
            if ( szVehicleName )
            {
                lua_pushstring( L, szVehicleName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleName", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleAdjustableProperty )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            if ( CClientVehicleManager::HasAdjustableProperty( vehicle->GetModel() ) )
            {
                lua_pushnumber( L, vehicle->GetAdjustablePropertyValue() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleAdjustableProperty", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getHelicopterRotorSpeed )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetHelicopterRotorSpeed( *vehicle, fSpeed ) )
            {
                lua_pushnumber( L, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getHelicopterRotorSpeed", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isTrainDerailed )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->IsDerailed() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isTrainDerailed", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isTrainDerailable )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->IsDerailable() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isTrainDerailable", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTrainDirection )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->GetTrainDirection() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getTrainDirection", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTrainSpeed )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            float fSpeed;
            if ( CStaticFunctionDefinitions::GetTrainSpeed( *vehicle, fSpeed ) )
            {
                lua_pushnumber( L, fSpeed );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getTrainSpeed", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleEngineState )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->IsEngineOn() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleEngineState", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleNameFromModel )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            char szVehicleName [32];

            if ( CStaticFunctionDefinitions::GetVehicleNameFromModel( (unsigned short)lua_tonumber( L, 1 ), szVehicleName, sizeof(szVehicleName) ) )
            {
                lua_pushstring( L, szVehicleName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getVehicleNameFromID" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createVehicle )
    {
        unsigned short model;
        CVector pos, rotation;
        const char *regPlate;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( model );
        argStream.ReadVector( pos );
        argStream.ReadVector( rotation, rotation );
        argStream.ReadString( regPlate, "" );

        // Verify the parameters
        if ( !argStream.HasErrors() )
        {
            CLuaMain *pLuaMain = lua_readcontext( L );
            CResource *pResource = pLuaMain->GetResource();

            // Create the vehicle and return its handle
            CClientVehicle* pVehicle = CStaticFunctionDefinitions::CreateVehicle ( *pResource, model, pos, rotation, regPlate );
            if ( pVehicle )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( ( CClientEntity* ) pVehicle );
                }

                pVehicle->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( fixVehicle )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::FixVehicle( *entity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "fixVehicle", "element", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( blowVehicle )
    {
        // Verify the element pointer argument
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::BlowVehicle( *entity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "blowVehicle", "element", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isVehicleBlown )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushboolean( L, vehicle->IsBlown() );
            return 1;
        } 
        else
            m_pScriptDebugging->LogBadPointer( "isVehicleBlown", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleHeadLightColor )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            SColor color = vehicle->GetHeadLightColor();
            lua_pushnumber( L, color.R );
            lua_pushnumber( L, color.G );
            lua_pushnumber( L, color.B );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleHeadLightColor", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleCurrentGear )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            lua_pushnumber( L, vehicle->GetCurrentGear() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleCurrentGear", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleTurnVelocity )
    {
        CClientEntity *entity;
        CVector turnVelocity;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( turnVelocity );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleTurnVelocity( *entity, turnVelocity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleColor )
    {
        // Count up number of args after the first one
        uchar ucParams[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        int i;
        for ( i = 0 ; i < 12 ; i++ )
        {
            int iArgumentType = lua_type ( L, i + 2 );
            if ( iArgumentType == LUA_TNUMBER || iArgumentType == LUA_TSTRING )
            {
                ucParams[i] = static_cast < unsigned char > ( Clamp ( 0.0, lua_tonumber ( L, i + 2 ), 255.0 ) );
            }
            else
                break;
        }

        if  ( ( lua_type ( L, 1 ) == LUA_TCLASS ) && ( i == 3 || i == 4 || i == 6 || i == 9 || i == 12 ) )
        {
            if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
            {
                CVehicleColor color;

                if ( i == 4 )
                {
                    // 4 args mean palette colours
                    color.SetPaletteColors ( ucParams[0], ucParams[1], ucParams[2], ucParams[3] );
                }
                else
                {
                    // 3,6,9 or 12 args mean rgb colours
                    color.SetRGBColors ( SColorRGBA ( ucParams[0], ucParams[1], ucParams[2], 0 ),
                                         SColorRGBA ( ucParams[3], ucParams[4], ucParams[5], 0 ),
                                         SColorRGBA ( ucParams[6], ucParams[7], ucParams[8], 0 ),
                                         SColorRGBA ( ucParams[9], ucParams[10], ucParams[11], 0 ) );
                }

                lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleColor( *entity, color ) );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer( "setVehicleColor", "element", 1 );
        }
        else
            m_pScriptDebugging->LogBadType( "setVehicleColor" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleLandingGearDown )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleLandingGearDown( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleLocked )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleLocked( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleDoorsUndamageable )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleDoorsUndamageable( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleSirensOn )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleSirensOn( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( addVehicleUpgrade )
    {
        CClientEntity *entity;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );

        if ( !argStream.HasErrors() )
        {
            if ( lua_isstring( L, 2 ) )
            {
                if ( strcmp( lua_tostring( L, 2 ), "all" ) == 0 )
                {
                    lua_pushboolean( L, CStaticFunctionDefinitions::AddAllVehicleUpgrades( *entity ) );
                    return 1;
                }
            }

            lua_pushboolean( L, CStaticFunctionDefinitions::AddVehicleUpgrade( *entity, (unsigned short)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( removeVehicleUpgrade )
    {
        CClientEntity *entity;
        unsigned short upgrade;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( upgrade );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::RemoveVehicleUpgrade( *entity, upgrade ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleDoorState )
    {
        CClientEntity *entity;
        unsigned char door, state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( door );
        argStream.ReadNumber( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleDoorState( *entity, door, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleWheelStates )
    {
        CClientEntity *entity;
        int frontLeft, rearLeft, frontRight, rearRight;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( frontLeft );
        argStream.ReadNumber( rearLeft, -1 ); argStream.ReadNumber( frontRight, -1 ); argStream.ReadNumber( rearRight, -1 );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleWheelStates( *entity, frontLeft, rearLeft, frontRight, rearRight ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleLightState )
    {
        CClientEntity *entity;
        unsigned char light, state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( light );
        argStream.ReadNumber( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleLightState( *entity, light, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehiclePanelState )
    {
        CClientEntity *entity;
        unsigned char panel, state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( panel );
        argStream.ReadNumber( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehiclePanelState( *entity, panel, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleOverrideLights )
    {
        CClientEntity *entity;
        unsigned char lights;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( lights );

        if ( !argStream.HasErrors() )
        {
            if ( lights <= 2 )
            {
                lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleOverrideLights( *entity, lights ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( attachTrailerToVehicle )
    {
        CClientVehicle *vehicle, *trailer;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadClass( trailer, LUACLASS_VEHICLE );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::AttachTrailerToVehicle( *vehicle, *trailer ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( detachTrailerFromVehicle )
    {
        CClientVehicle *vehicle, *trailer;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadClass( trailer, LUACLASS_VEHICLE, NULL );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::DetachTrailerFromVehicle( *vehicle, trailer ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleEngineState )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleEngineState( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleDirtLevel )
    {
        CClientEntity *entity;
        float dirtLevel;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( dirtLevel );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleDirtLevel( *entity, dirtLevel ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleDamageProof )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleDamageProof( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehiclePaintjob )
    {
        CClientEntity *entity;
        unsigned char paintJob;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( paintJob );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehiclePaintjob( *entity, paintJob ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleFuelTankExplodable )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleFuelTankExplodable( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleFrozen )
    {
        CClientEntity *entity;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleFrozen( *entity, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleAdjustableProperty )
    {
        CClientEntity *entity;
        unsigned short adjustableProperty;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( adjustableProperty );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleAdjustableProperty( *entity, adjustableProperty ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setHelicopterRotorSpeed )
    {
        CClientVehicle *vehicle;
        float speed;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( speed );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetHelicopterRotorSpeed( *vehicle, speed ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTrainDerailed )
    {
        CClientVehicle *vehicle;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetTrainDerailed( *vehicle, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTrainDerailable )
    {
        CClientVehicle *vehicle;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetTrainDerailable( *vehicle, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTrainDirection )
    {
        CClientVehicle *vehicle;
        bool state;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadBool( state );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetTrainDirection( *vehicle, state ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTrainSpeed )
    {
        CClientVehicle *vehicle;
        float speed;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( speed );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetTrainSpeed( *vehicle, speed ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    // Radio
    LUA_DECLARE( setRadioChannel )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetRadioChannel( (unsigned char)lua_tonumber( L, 1 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setRadioChannel" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getRadioChannel )
    {
        unsigned char ucChannel;
        if ( CStaticFunctionDefinitions::GetRadioChannel ( ucChannel ) )
        {
            lua_pushnumber( L, ucChannel );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getRadioChannelName )
    {
        static const char* szRadioStations[] = { "Radio off", "Playback FM", "K-Rose", "K-DST",
            "Bounce FM", "SF-UR", "Radio Los Santos", "Radio X", "CSR 103.9", "K-Jah West",
            "Master Sounds 98.3", "WCTR", "User Track Player" };

        if ( lua_isnumber( L, 1 ) )
        {
            int iChannel = (int)lua_tonumber( L, 1 );
            if ( iChannel >= 0 && iChannel < sizeof(szRadioStations)/sizeof(char*) )
            {
                lua_pushstring ( L, szRadioStations[iChannel] );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleGravity )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CVector vecGravity;
            vehicle->GetGravity ( vecGravity );
            lua_pushnumber ( L, vecGravity.fX );
            lua_pushnumber ( L, vecGravity.fY );
            lua_pushnumber ( L, vecGravity.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleGravity", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleGravity )
    {
        CClientVehicle *vehicle;
        CVector gravity;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadVector( gravity );

        if ( !argStream.HasErrors() )
        {
            vehicle->SetGravity( gravity );
            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleHeadLightColor )
    {
        CClientEntity *entity;
        unsigned char r, g, b;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadColor( r ); argStream.ReadColor( g ); argStream.ReadColor( b );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleHeadLightColor( *entity, SColorRGBA( r, g, b, 255 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleTurretPosition )
    {
        CClientVehicle *vehicle;
        float horizontal, vertical;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( horizontal );
        argStream.ReadNumber( vertical );

        if ( !argStream.HasErrors() )
        {
            vehicle->SetTurretRotation( horizontal, vertical );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleHandling )
    {
        if ( CClientVehicle *vehicle = lua_readclass <CClientVehicle> ( L, 1, LUACLASS_VEHICLE ) )
        {
            CHandlingEntry *pEntry = vehicle->GetHandlingData();

            lua_newtable ( L );

            lua_pushnumber ( L, pEntry->GetMass() );
            lua_setfield ( L, -2, "mass" );

            lua_pushnumber ( L, pEntry->GetTurnMass() );
            lua_setfield ( L, -2, "turnMass" );

            lua_pushnumber ( L, pEntry->GetDragCoeff() );
            lua_setfield ( L, -2, "dragCoeff" );

            lua_createtable ( L, 3, 0 );
            CVector vecCenter = pEntry->GetCenterOfMass ();
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, vecCenter.fX );
            lua_settable ( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, vecCenter.fY );
            lua_settable ( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, vecCenter.fZ );
            lua_settable ( L, -3 );
            lua_setfield ( L, -2, "centerOfMass" );

            lua_pushnumber ( L, pEntry->GetPercentSubmerged() );
            lua_setfield ( L, -2, "percentSubmerged" );

            lua_pushnumber ( L, pEntry->GetTractionMultiplier() );
            lua_setfield ( L, -2, "tractionMultiplier" );

            CHandlingEntry::eDriveType eDriveType = pEntry->GetCarDriveType ();
            if ( eDriveType == CHandlingEntry::FWD )
                lua_pushstring ( L, "fwd" );
            else if ( eDriveType==CHandlingEntry::RWD )
                lua_pushstring ( L, "rwd" );
            else if ( eDriveType==CHandlingEntry::FOURWHEEL )
                lua_pushstring ( L, "awd" );
            else // What the ... (yeah, security)
                lua_pushnil ( L );
            lua_setfield ( L, -2, "driveType" );
            CHandlingEntry::eEngineType eEngineType = pEntry->GetCarEngineType ();
            if ( eEngineType == CHandlingEntry::PETROL )
                lua_pushstring ( L, "petrol" );
            else if ( eEngineType == CHandlingEntry::DIESEL )
                lua_pushstring ( L, "diesel" );
            else if ( eEngineType == CHandlingEntry::ELECTRIC )
                lua_pushstring ( L, "electric" );
            else
                lua_pushnil ( L );
            lua_setfield ( L, -2, "engineType" );

            lua_pushnumber ( L, pEntry->GetNumberOfGears () );
            lua_setfield ( L, -2, "numberOfGears" );

            lua_pushnumber ( L, pEntry->GetEngineAcceleration () );
            lua_setfield ( L, -2, "engineAcceleration" );

            lua_pushnumber ( L, pEntry->GetEngineInertia () );
            lua_setfield ( L, -2, "engineInertia" );

            lua_pushnumber ( L, pEntry->GetMaxVelocity () );
            lua_setfield ( L, -2, "maxVelocity" );

            lua_pushnumber ( L, pEntry->GetBrakeDeceleration () );
            lua_setfield ( L, -2, "brakeDeceleration" );

            lua_pushnumber ( L, pEntry->GetBrakeBias () );
            lua_setfield ( L, -2, "brakeBias" );

            lua_pushboolean ( L, pEntry->GetABS () );
            lua_setfield ( L, -2, "ABS" );

            lua_pushnumber ( L, pEntry->GetSteeringLock () );
            lua_setfield ( L, -2, "steeringLock" );

            lua_pushnumber ( L, pEntry->GetTractionLoss () );
            lua_setfield ( L, -2, "tractionLoss" );

            lua_pushnumber ( L, pEntry->GetTractionBias () );
            lua_setfield ( L, -2, "tractionBias" );

            lua_pushnumber ( L, pEntry->GetSuspensionForceLevel () );
            lua_setfield ( L, -2, "suspensionForceLevel" );

            lua_pushnumber ( L, pEntry->GetSuspensionDamping () );
            lua_setfield ( L, -2, "suspensionDamping" );

            lua_pushnumber ( L, pEntry->GetSuspensionHighSpeedDamping () );
            lua_setfield ( L, -2, "suspensionHighSpeedDamping" );

            lua_pushnumber ( L, pEntry->GetSuspensionUpperLimit () );
            lua_setfield ( L, -2, "suspensionUpperLimit" );

            lua_pushnumber ( L, pEntry->GetSuspensionLowerLimit () );
            lua_setfield ( L, -2, "suspensionLowerLimit" );

            lua_pushnumber ( L, pEntry->GetSuspensionFrontRearBias () );
            lua_setfield ( L, -2, "suspensionFrontRearBias" );

            lua_pushnumber ( L, pEntry->GetSuspensionAntiDiveMultiplier () );
            lua_setfield ( L, -2, "suspensionAntiDiveMultiplier" );

            lua_pushnumber ( L, pEntry->GetCollisionDamageMultiplier () );
            lua_setfield ( L, -2, "collisionDamageMultiplier" );

            lua_pushnumber ( L, pEntry->GetSeatOffsetDistance () );
            lua_setfield ( L, -2, "seatOffsetDistance" );

            lua_pushnumber ( L, pEntry->GetHandlingFlags () );
            lua_setfield ( L, -2, "handlingFlags" );

            lua_pushnumber ( L, pEntry->GetModelFlags () );
            lua_setfield ( L, -2, "modelFlags" );

            lua_pushnumber ( L, pEntry->GetMonetary () );
            lua_setfield ( L, -2, "monetary" );

            CHandlingEntry::eLightType eHeadType = pEntry->GetHeadLight ();
            if ( eHeadType == CHandlingEntry::LONG )
                lua_pushstring ( L, "long" );
            else if ( eHeadType == CHandlingEntry::SMALL )
                lua_pushstring ( L, "small" );
            else if ( eHeadType == CHandlingEntry::BIG )
                lua_pushstring ( L, "big" );
            else
                lua_pushnil( L );
            lua_setfield ( L, -2, "headLight" );

            CHandlingEntry::eLightType eTailType = pEntry->GetTailLight ();
            if ( eTailType == CHandlingEntry::LONG )
                lua_pushstring ( L, "long" );
            else if ( eTailType == CHandlingEntry::SMALL )
                lua_pushstring ( L, "small" );
            else if ( eTailType == CHandlingEntry::BIG )
                lua_pushstring ( L, "big" );
            else
                lua_pushnil( L );
            lua_setfield ( L, -2, "tailLight" );

            lua_pushnumber ( L, pEntry->GetAnimGroup () );
            lua_setfield ( L, -2, "animGroup" );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getVehicleHandling", "vehicle", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getOriginalHandling )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            eVehicleTypes eModel = static_cast < eVehicleTypes > ( (int)lua_tonumber ( L, 1 ) );
            if ( eModel )
            {
                const CHandlingEntry* pEntry = g_pGame->GetHandlingManager()->GetOriginalHandlingData( eModel );

                lua_newtable ( L );
                lua_pushnumber ( L, pEntry->GetMass() );
                lua_setfield ( L, -2, "mass" );
                lua_pushnumber ( L, pEntry->GetTurnMass() );
                lua_setfield ( L, -2, "turnMass" );
                lua_pushnumber ( L, pEntry->GetDragCoeff() );
                lua_setfield ( L, -2, "dragCoeff" );
                lua_createtable ( L, 3, 0 );
                CVector vecCenter = pEntry->GetCenterOfMass ();
                lua_pushnumber ( L, 1 );
                lua_pushnumber ( L, vecCenter.fX );
                lua_settable ( L, -3 );
                lua_pushnumber ( L, 2 );
                lua_pushnumber ( L, vecCenter.fY );
                lua_settable ( L, -3 );
                lua_pushnumber ( L, 3 );
                lua_pushnumber ( L, vecCenter.fZ );
                lua_settable ( L, -3 );
                lua_setfield ( L, -2, "centerOfMass" );
                lua_pushnumber ( L, pEntry->GetPercentSubmerged() );
                lua_setfield ( L, -2, "percentSubmerged" );
                lua_pushnumber ( L, pEntry->GetTractionMultiplier() );
                lua_setfield ( L, -2, "tractionMultiplier" );
                CHandlingEntry::eDriveType eDriveType=pEntry->GetCarDriveType();
                if (eDriveType==CHandlingEntry::FWD)
                    lua_pushstring(L,"fwd");
                else if (eDriveType==CHandlingEntry::RWD)
                    lua_pushstring(L,"rwd");
                else if (eDriveType==CHandlingEntry::FOURWHEEL)
                    lua_pushstring(L,"awd");
                else // What the ... (yeah, security)
                    lua_pushnil ( L );
                lua_setfield ( L, -2, "driveType" );
                CHandlingEntry::eEngineType eEngineType=pEntry->GetCarEngineType();
                if (eEngineType==CHandlingEntry::PETROL)
                    lua_pushstring(L,"petrol");
                else if (eEngineType==CHandlingEntry::DIESEL)
                    lua_pushstring(L,"diesel");
                else if (eEngineType==CHandlingEntry::ELECTRIC)
                    lua_pushstring(L,"electric");
                else
                    lua_pushnil ( L );
                lua_setfield ( L, -2, "engineType" );
                lua_pushnumber ( L, pEntry->GetNumberOfGears() );
                lua_setfield ( L, -2, "numberOfGears" );
                lua_pushnumber ( L, pEntry->GetEngineAcceleration() );
                lua_setfield ( L, -2, "engineAcceleration" );
                lua_pushnumber ( L, pEntry->GetEngineInertia() );
                lua_setfield ( L, -2, "engineInertia" );
                lua_pushnumber ( L, pEntry->GetMaxVelocity() );
                lua_setfield ( L, -2, "maxVelocity" );
                lua_pushnumber ( L, pEntry->GetBrakeDeceleration() );
                lua_setfield ( L, -2, "brakeDeceleration" );
                lua_pushnumber ( L, pEntry->GetBrakeBias() );
                lua_setfield ( L, -2, "brakeBias" );
                lua_pushboolean ( L, pEntry->GetABS() );
                lua_setfield ( L, -2, "ABS" );
                lua_pushnumber ( L, pEntry->GetSteeringLock() );
                lua_setfield ( L, -2, "steeringLock" );
                lua_pushnumber ( L, pEntry->GetTractionLoss() );
                lua_setfield ( L, -2, "tractionLoss" );
                lua_pushnumber ( L, pEntry->GetTractionBias() );
                lua_setfield ( L, -2, "tractionBias" );
                lua_pushnumber ( L, pEntry->GetSuspensionForceLevel() );
                lua_setfield ( L, -2, "suspensionForceLevel" );
                lua_pushnumber ( L, pEntry->GetSuspensionDamping() );
                lua_setfield ( L, -2, "suspensionDamping" );
                lua_pushnumber ( L, pEntry->GetSuspensionHighSpeedDamping() );
                lua_setfield ( L, -2, "suspensionHighSpeedDamping" );
                lua_pushnumber ( L, pEntry->GetSuspensionUpperLimit() );
                lua_setfield ( L, -2, "suspensionUpperLimit" );
                lua_pushnumber ( L, pEntry->GetSuspensionLowerLimit() );
                lua_setfield ( L, -2, "suspensionLowerLimit" );
                lua_pushnumber ( L, pEntry->GetSuspensionFrontRearBias() );
                lua_setfield ( L, -2, "suspensionFrontRearBias" );
                lua_pushnumber ( L, pEntry->GetSuspensionAntiDiveMultiplier() );
                lua_setfield ( L, -2, "suspensionAntiDiveMultiplier" );
                lua_pushnumber ( L, pEntry->GetCollisionDamageMultiplier() );
                lua_setfield ( L, -2, "collisionDamageMultiplier" );
                lua_pushnumber ( L, pEntry->GetSeatOffsetDistance() );
                lua_setfield ( L, -2, "seatOffsetDistance" );
                lua_pushnumber ( L, pEntry->GetHandlingFlags() );
                lua_setfield ( L, -2, "handlingFlags" );
                lua_pushnumber ( L, pEntry->GetModelFlags() );
                lua_setfield ( L, -2, "modelFlags" );
                lua_pushnumber ( L, pEntry->GetMonetary() );
                lua_setfield ( L, -2, "monetary" );
                CHandlingEntry::eLightType eHeadType=pEntry->GetHeadLight();
                if (eHeadType==CHandlingEntry::LONG)
                    lua_pushstring(L,"long");
                else if (eHeadType==CHandlingEntry::SMALL)
                    lua_pushstring(L,"small");
                else if (eHeadType==CHandlingEntry::BIG)
                    lua_pushstring(L,"big");
                else
                    lua_pushnil( L );
                lua_setfield ( L, -2, "headLight" );
                CHandlingEntry::eLightType eTailType=pEntry->GetTailLight();
                if (eTailType==CHandlingEntry::LONG)
                    lua_pushstring(L,"long");
                else if (eTailType==CHandlingEntry::SMALL)
                    lua_pushstring(L,"small");
                else if (eTailType==CHandlingEntry::BIG)
                    lua_pushstring(L,"big");
                else
                    lua_pushnil( L );
                lua_setfield ( L, -2, "tailLight" );
                lua_pushnumber ( L, pEntry->GetAnimGroup() );
                lua_setfield ( L, -2, "animGroup" );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer( "getOriginalHandling", "model", 1 );
        }
        else
            m_pScriptDebugging->LogBadType( "getOriginalHandling" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setVehicleDoorOpenRatio )
    {
        CClientEntity *entity;
        unsigned char door;
        float ratio;
        unsigned long time;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( door );
        argStream.ReadNumber( ratio );
        argStream.ReadNumber( time, 0UL );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetVehicleDoorOpenRatio( *entity, door, ratio, time ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVehicleDoorOpenRatio )
    {
        CClientVehicle *vehicle;
        unsigned char door;

        CScriptArgReader argStream( L );

        argStream.ReadClass( vehicle, LUACLASS_VEHICLE );
        argStream.ReadNumber( door );

        if ( !argStream.HasErrors() )
        {
            if ( door <= 5 )
            {
                lua_pushnumber( L, vehicle->GetDoorOpenRatio( door ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
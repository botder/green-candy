/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/luavehicle.cpp
*  PURPOSE:     Vehicle Lua instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static LUA_DECLARE( setRotation )
{
    CVector rot;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( rot );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetRotationDegrees( rot );
    LUA_SUCCESS;
}

inline static void lua_pushvector( lua_State *L, const CVector& vec )
{
    lua_pushnumber( L, vec[0] );
    lua_pushnumber( L, vec[1] );
    lua_pushnumber( L, vec[2] );
}

static LUA_DECLARE( getRotation )
{
    CVector rot;
    ((CClientVehicle*)lua_getmethodtrans( L ))->GetRotationDegrees( rot );

    lua_pushvector( L, rot );
    return 3;
}

static LUA_DECLARE( setModel )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetModelBlocking( model );
    LUA_SUCCESS;
}

static LUA_DECLARE( getModel )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetModel() );
    return 1;
}

static LUA_DECLARE( setHealth )
{
    float health;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( health );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetHealth( (float)lua_tonumber( L, 1 ) );
    LUA_SUCCESS;
}

static LUA_DECLARE( getHealth )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetHealth() );
    return 1;
}

static LUA_DECLARE( fix )
{
    ((CClientVehicle*)lua_getmethodtrans( L ))->Fix();
    return 0;
}

static LUA_DECLARE( blow )
{
    bool allowMovement;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( allowMovement, true );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->Blow( allowMovement );
    LUA_SUCCESS;
}

static LUA_DECLARE( isBlown )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsBlown() );
    return 1;
}

static LUA_DECLARE( setExplodeTime )
{
    unsigned long time;
    
    LUA_ARGS_BEGIN;
    argStream.ReadNumber( time );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetExplodeTime( time );
    LUA_SUCCESS;
}

static LUA_DECLARE( getExplodeTime )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetExplodeTime() );
    return 1;
}

static LUA_DECLARE( setBurningTime )
{
    float time;
    
    LUA_ARGS_BEGIN;
    argStream.ReadNumber( time );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetBurningTime( time );
    LUA_SUCCESS;
}

static LUA_DECLARE( getBurningTime )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetBurningTime() );
    return 1;
}

static LUA_DECLARE( addUpgrade )
{
    CVehicleUpgrades *upgrades = ((CClientVehicle*)lua_getmethodtrans( L ))->GetUpgrades();

    LUA_ASSERT( upgrades, "vehicle does not support upgrades" );

    unsigned short upgrade;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( upgrade );
    LUA_ARGS_END;

    upgrades->AddUpgrade( upgrade );
    LUA_SUCCESS;
}

static LUA_DECLARE( getUpgradeOnSlot )
{
    CVehicleUpgrades *upgrades = ((CClientVehicle*)lua_getmethodtrans( L ))->GetUpgrades();

    LUA_ASSERT( upgrades, "vehicle does not support upgrades" );

    unsigned char slot;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( slot );
    LUA_ARGS_END;

    upgrades->GetSlotState( slot );
    LUA_SUCCESS;
}

static LUA_DECLARE( getUpgrades )
{
    CVehicleUpgrades *upgrades = ((CClientVehicle*)lua_getmethodtrans( L ))->GetUpgrades();

    LUA_ASSERT( upgrades, "vehicle does not support upgrades" );

    lua_settop( L, 0 );

    // Add all available ones to an indexed table
    lua_createtable( L, VEHICLE_UPGRADE_SLOTS, 0 );

    for ( unsigned char n=0; n<VEHICLE_UPGRADE_SLOTS; n++ )
    {
        unsigned short upgrade = upgrades->GetSlotState( n );

        if ( upgrade == 0 )
            continue;

        lua_pushnumber( L, upgrade );
        lua_rawseti( L, 1, n );
    }

    // Return the table
    return 1;
}

static LUA_DECLARE( setHandling )
{
    // TODO
    return 0;
}

static LUA_DECLARE( getHandling )
{
    // TODO
    return 0;
}

static LUA_DECLARE( removeUpgrade )
{
    CVehicleUpgrades *upgrades = ((CClientVehicle*)lua_getmethodtrans( L ))->GetUpgrades();

    LUA_ASSERT( upgrades, "vehicle does not support upgrades" );

    unsigned short upgrade;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( upgrade );
    LUA_ARGS_END;

    lua_pushboolean( L, upgrades->RemoveUpgrade( upgrade ) );
    return 1;
}

static LUA_DECLARE( getOccupant )
{
    unsigned char slot;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( slot, 0 );
    LUA_ARGS_END;

    CClientPed *ped = ((CClientVehicle*)lua_getmethodtrans( L ))->GetOccupant( slot );

    if ( !ped )
        return 0;

    ped->PushStack( L );
    return 1;
}

static LUA_DECLARE( getOccupants )
{
    CClientVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ));

    lua_settop( L, 0 );

    // Get all passengers + the driver
    lua_createtable( L, 9, 0 );

    for ( unsigned char n=0; n<9; n++ )
    {
        CClientPed *ped = veh->GetOccupant( n );

        if ( !ped )
            continue;

        ped->PushStack( L );
        lua_rawseti( L, 1, n );
    }

    return 1;
}

static LUA_DECLARE( setOverrideLights )
{
    unsigned char val;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( val );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetOverrideLights( val );
    LUA_SUCCESS;
}

static LUA_DECLARE( getOverrideLights )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetOverrideLights() );
    return 1;
}

static LUA_DECLARE( setTaxiLightOn )
{
    bool enable;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( enable );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTaxiLightOn( enable );
    LUA_SUCCESS;
}

static LUA_DECLARE( isTaxiLightOn )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsTaxiLightOn() );
    return 1;
}

static LUA_DECLARE( isUpsideDown )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsUpsideDown() );
    return 1;
}

static LUA_DECLARE( setColor )
{
    unsigned char a, b, c, d;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( a ); argStream.ReadNumber( b ); argStream.ReadNumber( c ); argStream.ReadNumber( d );
    LUA_ARGS_END;

    CVehicleColor color;
    color.SetPaletteColors( a, b, c, d );

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetColor( color );
    LUA_SUCCESS;
}

static LUA_DECLARE( setColorRGB )
{
    unsigned char r1, g1, b1;
    unsigned char r2, g2, b2;
    unsigned char r3, g3, b3;
    unsigned char r4, g4, b4;

    LUA_ARGS_BEGIN;
    argStream.ReadColor( r1 ); argStream.ReadColor( g1 ); argStream.ReadColor( b1 );
    argStream.ReadColor( r2 ); argStream.ReadColor( g2 ); argStream.ReadColor( b2 );
    argStream.ReadColor( r3 ); argStream.ReadColor( g3 ); argStream.ReadColor( b3 );
    argStream.ReadColor( r4 ); argStream.ReadColor( g4 ); argStream.ReadColor( b4 );
    LUA_ARGS_END;

    CVehicleColor color;
    color.SetRGBColors( SColorRGBA( r1, g1, b1, 0 ),
                        SColorRGBA( r2, g2, b2, 0 ),
                        SColorRGBA( r3, g3, b3, 0 ),
                        SColorRGBA( r4, g4, b4, 0 )
                        );

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetColor( color );
    LUA_SUCCESS;
}

static LUA_DECLARE( getColor )
{
    CVehicleColor& color = ((CClientVehicle*)lua_getmethodtrans( L ))->GetColor();

    lua_pushnumber( L, color.GetPaletteColor( 0 ) );
    lua_pushnumber( L, color.GetPaletteColor( 1 ) );
    lua_pushnumber( L, color.GetPaletteColor( 2 ) );
    lua_pushnumber( L, color.GetPaletteColor( 3 ) );
    return 4;
}

inline static void lua_pushcolorRGB( lua_State *L, const SColor& color )
{
    lua_pushnumber( L, color.R );
    lua_pushnumber( L, color.G );
    lua_pushnumber( L, color.B );
}

static LUA_DECLARE( getColorRGB )
{
    CVehicleColor& color = ((CClientVehicle*)lua_getmethodtrans( L ))->GetColor();

    // Make sure we cannot flood the stack
    lua_settop( L, 0 );

    for ( unsigned char n=0; n<4; n++ )
        lua_pushcolorRGB( L, color.GetRGBColor( n ) );

    return 12;
}

static LUA_DECLARE( setHeadLightColor )
{
    unsigned char r, g, b;

    LUA_ARGS_BEGIN;
    argStream.ReadColor( r ); argStream.ReadColor( g ); argStream.ReadColor( b );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetHeadLightColor( SColorRGBA( r, g, b, 0xFF ) );
    LUA_SUCCESS;
}

static LUA_DECLARE( getHeadLightColor )
{
    lua_pushcolorRGB( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetHeadLightColor() );
    return 3;
}

static LUA_DECLARE( attachTrailer )
{
    CClientVehicle *trailer;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( trailer, LUACLASS_VEHICLE );
    LUA_ARGS_END;

    if ( CClientVehicle *prev = trailer->GetTowedByVehicle() )
        prev->SetTowedVehicle( NULL );

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTowedVehicle( trailer );
    LUA_SUCCESS;
}

static LUA_DECLARE( detachTrailer )
{
    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTowedVehicle( NULL );
    return 0;
}

static LUA_DECLARE( getTowBarPos )
{
    CVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ))->GetGameVehicle();

    LUA_CHECK( veh );

    CVector pos;
    veh->GetTowBarPos( pos );

    lua_pushvector( L, pos );
    return 3;
}

static LUA_DECLARE( getTowHitchPos )
{
    CVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ))->GetGameVehicle();

    LUA_CHECK( veh );

    CVector pos;
    veh->GetTowHitchPos( pos );

    lua_pushvector( L, pos );
    return 3;
}

static LUA_DECLARE( setAdjustableProperty )
{
    unsigned short prop;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( prop );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetAdjustablePropertyValue( prop );
    LUA_SUCCESS;
}

static LUA_DECLARE( getAdjustableProperty )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetAdjustablePropertyValue() );
    return 1;
}

static LUA_DECLARE( setGravity )
{
    CVector grav;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( grav );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetGravity( grav );
    LUA_SUCCESS;
}

static LUA_DECLARE( getGravity )
{
    CVector grav;
    ((CClientVehicle*)lua_getmethodtrans( L ))->GetGravity( grav );

    lua_pushvector( L, grav );
    return 3;
}

static LUA_DECLARE( setDoorState )
{
    unsigned char door, state;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( door );
    argStream.ReadNumber( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDoorStatus( door, state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getDoorState )
{
    unsigned char door;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( door );
    LUA_ARGS_END;

    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetDoorStatus( door ) );
    return 1;
}

static LUA_DECLARE( setLightState )
{
    unsigned char light, state;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( light );
    argStream.ReadNumber( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetLightStatus( light, state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getLightState )
{
    unsigned char light;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( light );
    LUA_ARGS_END;

    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetLightStatus( light ) );
    return 1;
}

static LUA_DECLARE( setPanelState )
{
    unsigned char panel, state;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( panel );
    argStream.ReadNumber( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetPanelStatus( panel, state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getPanelState )
{
    unsigned char panel;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( panel );
    LUA_ARGS_END;

    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetPanelStatus( panel ) );
    return 1;
}

static LUA_DECLARE( setWheelStates )
{
    char a, b, c, d;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( a );
    argStream.ReadNumber( b, -1 );
    argStream.ReadNumber( c, -1 );
    argStream.ReadNumber( d, -1 );
    LUA_ARGS_END;

    CClientVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ));
    veh->SetWheelStatus( FRONT_LEFT_WHEEL, a );
    veh->SetWheelStatus( REAR_LEFT_WHEEL, b );
    veh->SetWheelStatus( FRONT_RIGHT_WHEEL, c );
    veh->SetWheelStatus( REAR_RIGHT_WHEEL, d );
    LUA_SUCCESS;
}

static LUA_DECLARE( getWheelStates )
{
    CClientVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ));
    
    lua_pushnumber( L, veh->GetWheelStatus( FRONT_LEFT_WHEEL ) );
    lua_pushnumber( L, veh->GetWheelStatus( REAR_LEFT_WHEEL ) );
    lua_pushnumber( L, veh->GetWheelStatus( FRONT_RIGHT_WHEEL ) );
    lua_pushnumber( L, veh->GetWheelStatus( REAR_RIGHT_WHEEL ) );
    return 4;
}

static LUA_DECLARE( cloneClump )
{
    CResource *res = CLuaFunctionDefs::lua_readcontext( L )->GetResource();
    CVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ))->GetGameVehicle();

    if ( !veh )
        return 0;

    CClientDFF *dff = new CClientDFF( L, *veh->CloneClump(), res );
    dff->PushStack( L );
    dff->DisableKeepAlive();
    return 1;
}

static LUA_DECLARE( getComponent )
{
    const char *name;

    LUA_ARGS_BEGIN;
    argStream.ReadString( name );
    LUA_ARGS_END;

    CClientVehicleComponent *comp = ((CClientVehicle*)lua_getmethodtrans( L ))->GetComponent( name );

    LUA_CHECK( comp );

    comp->PushStack( L );
    return 1;
}

static LUA_DECLARE( getComponentNames )
{
    CVehicle *veh = ((CClientVehicle*)lua_getmethodtrans( L ))->GetGameVehicle();

    LUA_CHECK( veh );

    std::vector <std::string> names;
    veh->GetComponentNameList( names );

    int num = names.size();

    // Clear the stack and put table on first slot
    lua_settop( L, 0 );
    lua_createtable( L, num, 0 );

    for ( int n = 0; n < num; n++ )
    {
        const std::string& id = names[n];

        lua_pushlstring( L, id.c_str(), id.size() );
        lua_rawseti( L, 1, n + 1 );
    }

    return 1;
}

static LUA_DECLARE( setEngineState )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetEngineOn( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getEngineState )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsEngineOn() );
    return 1;
}

static LUA_DECLARE( setHandbrakeOn )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetHandbrakeOn( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isHandbrakeOn )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsHandbrakeOn() );
    return 1;
}

static LUA_DECLARE( setLandingGearDown )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetLandingGearDown( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getLandingGearDown )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsLandingGearDown() );
    return 1;
}

static LUA_DECLARE( setLandingGearPosition )
{
    float pos;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( pos );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetLandingGearPosition( pos );
    LUA_SUCCESS;
}

static LUA_DECLARE( getLandingGearPosition )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetLandingGearPosition() );
    return 1;
}

static LUA_DECLARE( setBrakePedal )
{
    float percent;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( percent );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetBrakePedal( percent );
    LUA_SUCCESS;
}

static LUA_DECLARE( setGasPedal )
{
    float percent;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( percent );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetGasPedal( percent );
    LUA_SUCCESS;
}

static LUA_DECLARE( setSteerAngle )
{
    float rad;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( rad );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetSteerAngle( rad );
    LUA_SUCCESS;
}

static LUA_DECLARE( setSecSteerAngle )
{
    float rad;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( rad );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetSecSteerAngle( rad );
    LUA_SUCCESS;
}

static LUA_DECLARE( getCurrentGear )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetCurrentGear() );
    return 1;
}

static LUA_DECLARE( getBrakePedal )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetBrakePedal() );
    return 1;
}

static LUA_DECLARE( getGasPedal )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetGasPedal() );
    return 1;
}

static LUA_DECLARE( getSteerAngle )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetSteerAngle() );
    return 1;
}

static LUA_DECLARE( getSecSteerAngle )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetSecSteerAngle() );
    return 1;
}

static LUA_DECLARE( setNitrousFuel )
{
    float fuel;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( fuel );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetNitrousFuel( fuel );
    LUA_SUCCESS;
}

static LUA_DECLARE( getNitrousFuel )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetNitrousFuel() );
    return 1;
}

static LUA_DECLARE( setPaintjob )
{
    unsigned char id;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( id );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetPaintjob( id );
    LUA_SUCCESS;
}

static LUA_DECLARE( getPaintjob )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetPaintjob() );
    return 1;
}

static LUA_DECLARE( getPlateText )
{
    lua_pushstring( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetRegPlate() );
    return 1;
}

static LUA_DECLARE( setSirensOn )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetSirenOrAlarmActive( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( getSirensOn )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsSirenOrAlarmActive() );
    return 1;
}

static LUA_DECLARE( getTowedByVehicle )
{
    CClientVehicle *towedBy = ((CClientVehicle*)lua_getmethodtrans( L ))->GetTowedByVehicle();

    if ( !towedBy )
        return 0;

    towedBy->PushStack( L );
    return 1;
}

static LUA_DECLARE( getTowingVehicle )
{
    CClientVehicle *towing = ((CClientVehicle*)lua_getmethodtrans( L ))->GetTowedVehicle();

    if ( !towing )
        return 0;

    towing->PushStack( L );
    return 1;
}

static LUA_DECLARE( setTurnVelocity )
{
    CVector velocity;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( velocity );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTurnSpeed( velocity );
    LUA_SUCCESS;
}

static LUA_DECLARE( getTurnVelocity )
{
    CVector spin;
    ((CClientVehicle*)lua_getmethodtrans( L ))->GetTurnSpeed( spin );

    lua_pushvector( L, spin );
    return 3;
}

static LUA_DECLARE( setTurretRotation )
{
    float h, v;
    
    LUA_ARGS_BEGIN;
    argStream.ReadNumber( h );
    argStream.ReadNumber( v );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTurretRotation( h, v );
    LUA_SUCCESS;
}

static LUA_DECLARE( getTurretRotation )
{
    float h, v;
    ((CClientVehicle*)lua_getmethodtrans( L ))->GetTurretRotation( h, v );

    lua_pushnumber( L, h );
    lua_pushnumber( L, v );
    return 2;
}

static LUA_DECLARE( setDamageProof )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetScriptCanBeDamaged( !state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isDamageProof )
{
    lua_pushboolean( L, !((CClientVehicle*)lua_getmethodtrans( L ))->GetScriptCanBeDamaged() );
    return 1;
}

static LUA_DECLARE( setFrozen )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetFrozen( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isFrozen )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsFrozen() );
    return 1;
}

static LUA_DECLARE( setFuelTankExplodable )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetCanShootPetrolTank( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isFuelTankExplodable )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetCanShootPetrolTank() );
    return 1;
}

static LUA_DECLARE( setHeatSeekable )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetCanBeTargettedByHeatSeekingMissiles( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isHeatSeekable )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetCanBeTargettedByHeatSeekingMissiles() );
    return 1;
}

static LUA_DECLARE( setLocked )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDoorsLocked( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isLocked )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->AreDoorsLocked() );
    return 1;
}

static LUA_DECLARE( isOnGround )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsOnGround() );
    return 1;
}

static LUA_DECLARE( setDirtLevel )
{
    float level;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( level );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDirtLevel( level );
    LUA_SUCCESS;
}

static LUA_DECLARE( getDirtLevel )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetDirtLevel() );
    return 1;
}

static LUA_DECLARE( setDoorsUndamageable )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDoorsUndamageable( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( areDoorsUndamageable )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->AreDoorsUndamageable() );
    return 1;
}

static LUA_DECLARE( setDoorOpenRatio )
{
    unsigned char door;
    float ratio;
    unsigned long delay;
    bool forced;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( door );
    argStream.ReadNumber( ratio );
    argStream.ReadNumber( delay, 0 );
    argStream.ReadBool( forced, false );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDoorOpenRatio( door, ratio, delay, forced );
    LUA_SUCCESS;
}

static LUA_DECLARE( getDoorOpenRatio )
{
    unsigned char door;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( door );
    LUA_ARGS_END;

    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetDoorOpenRatio( door ) );
    return 1;
}

static LUA_DECLARE( allowSwingingDoors )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetSwingingDoorsAllowed( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( areSwingingDoorsAllowed )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->AreSwingingDoorsAllowed() );
    return 1;
}

static LUA_DECLARE( setRotorSpeed )
{
    float speed;
    
    LUA_ARGS_BEGIN;
    argStream.ReadNumber( speed );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetHeliRotorSpeed( speed );
    LUA_SUCCESS;
}

static LUA_DECLARE( getRotorSpeed )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetHeliRotorSpeed() );
    return 1;
}

static LUA_DECLARE( setSearchLightVisible )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetHeliSearchLightVisible( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isSearchLightVisible )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsHeliSearchLightVisible() );
    return 1;
}

static LUA_DECLARE( setHeliWinchType )
{
    unsigned char type;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( type );
    LUA_ARGS_END;

    LUA_ASSERT( type < NUM_WINCH_TYPE, "invalid winch type" );

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetWinchType( (eWinchType)type );
    LUA_SUCCESS;
}

static LUA_DECLARE( getHeliWinchType )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetWinchType() );
    return 1;
}

static LUA_DECLARE( pickUpWithWinch )
{
    CClientEntity *entity;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( entity, LUACLASS_ENTITY );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->PickupEntityWithWinch( entity ) );
    return 1;
}

static LUA_DECLARE( getWinchedEntity )
{
    CClientEntity *entity = ((CClientVehicle*)lua_getmethodtrans( L ))->GetPickedUpEntityWithWinch();

    if ( !entity )
        return 0;

    entity->PushStack( L );
    return 1;
}

static LUA_DECLARE( releaseWinched )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->ReleasePickedUpEntityWithWinch() );
    return 1;
}

static LUA_DECLARE( setSmokeTrailEnabled )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetSmokeTrailEnabled( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isSmokeTrailEnabled )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsSmokeTrailEnabled() );
    return 1;
}

static LUA_DECLARE( setTrainDirection )
{
    bool direction;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( direction );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTrainDirection( direction );
    LUA_SUCCESS;
}

static LUA_DECLARE( getTrainDirection )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetTrainDirection() );
    return 1;
}

static LUA_DECLARE( setTrainSpeed )
{
    float speed;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( speed );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetTrainSpeed( speed );
    LUA_SUCCESS;
}

static LUA_DECLARE( getTrainSpeed )
{
    lua_pushnumber( L, ((CClientVehicle*)lua_getmethodtrans( L ))->GetTrainSpeed() );
    return 1;
}

static LUA_DECLARE( setNextCarriage )
{
    CClientVehicle *carriage;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( carriage, LUACLASS_VEHICLE, NULL );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetNextTrainCarriage( carriage );
    LUA_SUCCESS;
}

static LUA_DECLARE( getNextCarriage )
{
    CClientVehicle *carriage = ((CClientVehicle*)lua_getmethodtrans( L ))->GetNextTrainCarriage();

    if ( !carriage )
        return 0;

    carriage->PushStack( L );
    return 1;
}

static LUA_DECLARE( setPreviousCarriage )
{
    CClientVehicle *carriage;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( carriage, LUACLASS_VEHICLE, NULL );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetPreviousTrainCarriage( carriage );
    LUA_SUCCESS;
}

static LUA_DECLARE( getPreviousCarriage )
{
    CClientVehicle *carriage = ((CClientVehicle*)lua_getmethodtrans( L ))->GetPreviousTrainCarriage();

    if ( !carriage )
        return 0;

    carriage->PushStack( L );
    return 1;
}

static LUA_DECLARE( setDerailable )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDerailable( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isDerailable )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsDerailable() );
    return 1;
}

static LUA_DECLARE( setDerailed )
{
    bool state;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( state );
    LUA_ARGS_END;

    ((CClientVehicle*)lua_getmethodtrans( L ))->SetDerailed( state );
    LUA_SUCCESS;
}

static LUA_DECLARE( isDerailed )
{
    lua_pushboolean( L, ((CClientVehicle*)lua_getmethodtrans( L ))->IsDerailed() );
    return 1;
}

static const luaL_Reg vehicle_interface_trans[] =
{
    LUA_METHOD( setRotation ),
    LUA_METHOD( getRotation ),
    LUA_METHOD( setModel ),
    LUA_METHOD( getModel ),
    LUA_METHOD( setHealth ),
    LUA_METHOD( getHealth ),
    LUA_METHOD( fix ),
    LUA_METHOD( blow ),
    LUA_METHOD( isBlown ),
    LUA_METHOD( setExplodeTime ),
    LUA_METHOD( getExplodeTime ),
    LUA_METHOD( setBurningTime ),
    LUA_METHOD( getBurningTime ),
    LUA_METHOD( addUpgrade ),
    LUA_METHOD( getUpgradeOnSlot ),
    LUA_METHOD( getUpgrades ),
    LUA_METHOD( setHandling ),
    LUA_METHOD( getHandling ),
    LUA_METHOD( removeUpgrade ),
    LUA_METHOD( getOccupant ),
    LUA_METHOD( getOccupants ),
    LUA_METHOD( setOverrideLights ),
    LUA_METHOD( getOverrideLights ),
    LUA_METHOD( setTaxiLightOn ),
    LUA_METHOD( isTaxiLightOn ),
    LUA_METHOD( isUpsideDown ),
    LUA_METHOD( setColor ),
    LUA_METHOD( setColorRGB ),
    LUA_METHOD( getColor ),
    LUA_METHOD( getColorRGB ),
    LUA_METHOD( setHeadLightColor ),
    LUA_METHOD( getHeadLightColor ),
    LUA_METHOD( attachTrailer ),
    LUA_METHOD( detachTrailer ),
    LUA_METHOD( getTowBarPos ),
    LUA_METHOD( getTowHitchPos ),
    LUA_METHOD( setAdjustableProperty ),
    LUA_METHOD( getAdjustableProperty ),
    LUA_METHOD( setGravity ),
    LUA_METHOD( getGravity ),
    LUA_METHOD( setDoorState ),
    LUA_METHOD( getDoorState ),
    LUA_METHOD( setLightState ),
    LUA_METHOD( getLightState ),
    LUA_METHOD( setPanelState ),
    LUA_METHOD( getPanelState ),
    LUA_METHOD( setWheelStates ),
    LUA_METHOD( getWheelStates ),
    LUA_METHOD( cloneClump ),
    LUA_METHOD( getComponent ),
    LUA_METHOD( getComponentNames ),
    LUA_METHOD( setEngineState ),
    LUA_METHOD( getEngineState ),
    LUA_METHOD( setHandbrakeOn ),
    LUA_METHOD( isHandbrakeOn ),
    LUA_METHOD( setLandingGearDown ),
    LUA_METHOD( getLandingGearDown ),
    LUA_METHOD( setLandingGearPosition ),
    LUA_METHOD( getLandingGearPosition ),
    LUA_METHOD( setBrakePedal ),
    LUA_METHOD( setGasPedal ),
    LUA_METHOD( setSteerAngle ),
    LUA_METHOD( setSecSteerAngle ),
    LUA_METHOD( getCurrentGear ),
    LUA_METHOD( getBrakePedal ),
    LUA_METHOD( getGasPedal ),
    LUA_METHOD( getSteerAngle ),
    LUA_METHOD( getSecSteerAngle ),
    LUA_METHOD( setNitrousFuel ),
    LUA_METHOD( getNitrousFuel ),
    LUA_METHOD( setPaintjob ),
    LUA_METHOD( getPaintjob ),
    LUA_METHOD( getPlateText ),
    LUA_METHOD( setSirensOn ),
    LUA_METHOD( getSirensOn ),
    LUA_METHOD( getTowedByVehicle ),
    LUA_METHOD( getTowingVehicle ),
    LUA_METHOD( setTurnVelocity ),
    LUA_METHOD( getTurnVelocity ),
    LUA_METHOD( setTurretRotation ),
    LUA_METHOD( getTurretRotation ),
    LUA_METHOD( setDamageProof ),
    LUA_METHOD( isDamageProof ),
    LUA_METHOD( setFrozen ),
    LUA_METHOD( isFrozen ),
    LUA_METHOD( setFuelTankExplodable ),
    LUA_METHOD( isFuelTankExplodable ),
    LUA_METHOD( setHeatSeekable ),
    LUA_METHOD( isHeatSeekable ),
    LUA_METHOD( setLocked ),
    LUA_METHOD( isLocked ),
    LUA_METHOD( isOnGround ),
    LUA_METHOD( setDirtLevel ),
    LUA_METHOD( getDirtLevel ),
    LUA_METHOD( setDoorsUndamageable ),
    LUA_METHOD( areDoorsUndamageable ),
    LUA_METHOD( setDoorOpenRatio ),
    LUA_METHOD( getDoorOpenRatio ),
    LUA_METHOD( allowSwingingDoors ),
    LUA_METHOD( areSwingingDoorsAllowed ),
    LUA_METHOD( setRotorSpeed ),
    LUA_METHOD( getRotorSpeed ),
    LUA_METHOD( setSearchLightVisible ),
    LUA_METHOD( isSearchLightVisible ),
    LUA_METHOD( setHeliWinchType ),
    LUA_METHOD( getHeliWinchType ),
    LUA_METHOD( pickUpWithWinch ),
    LUA_METHOD( getWinchedEntity ),
    LUA_METHOD( releaseWinched ),
    LUA_METHOD( setSmokeTrailEnabled ),
    LUA_METHOD( isSmokeTrailEnabled ),
    LUA_METHOD( setTrainDirection ),
    LUA_METHOD( getTrainDirection ),
    LUA_METHOD( setTrainSpeed ),
    LUA_METHOD( getTrainSpeed ),
    LUA_METHOD( setNextCarriage ),
    LUA_METHOD( getNextCarriage ),
    LUA_METHOD( setPreviousCarriage ),
    LUA_METHOD( getPreviousCarriage ),
    LUA_METHOD( setDerailable ),
    LUA_METHOD( isDerailable ),
    LUA_METHOD( setDerailed ),
    LUA_METHOD( isDerailed ),
    { NULL, NULL }
};

LUA_DECLARE( luaconstructor_vehicle )
{
    CClientVehicle *veh = (CClientVehicle*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_VEHICLE, veh );

    j.RegisterInterfaceTrans( L, vehicle_interface_trans, 0, LUACLASS_VEHICLE );

    lua_pushlstring( L, "vehicle", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAutomobile.h
*  PURPOSE:     Automobile vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_AUTOMOBILE
#define __CGAME_AUTOMOBILE

#include "CDamageManager.h"
#include "CDoor.h"
#include "Common.h"
#include "CVehicle.h"

enum eVehicleComponent
{
    VEHICLE_COMP_NULL,
    VEHICLE_COMP_CHASIS,

    VEHICLE_COMP_WHEEL_FR,
    VEHICLE_COMP_WHEEL_RR = 4,
    VEHICLE_COMP_WHEEL_FL,
    VEHICLE_COMP_WHEEL_RL = 7,

    VEHICLE_COMP_DOOR_DRIVER,
    VEHICLE_COMP_DOOR_2,
    VEHICLE_COMP_DOOR_3,
    VEHICLE_COMP_DOOR_4,

    VEHICLE_COMP_BUMPER_FRONT,
    VEHICLE_COMP_BUMPER_REAR,

    VEHICLE_COMP_BONNET = 16,
    VEHICLE_COMP_BOOT,
    VEHICLE_COMP_WINDSCREEN,
    VEHICLE_COMP_EXHAUST,

    NUM_VEHICLE_COMPONENTS = 25
};

class CAutomobile : public virtual CVehicle
{
public:
    virtual                     ~CAutomobile()  {};

    virtual CDoor*              GetDoor( unsigned char ucDoor ) = 0;
    virtual void                SetSwingingDoorsAllowed( bool bAllowed ) = 0;
    virtual bool                AreSwingingDoorsAllowed() const = 0;

    virtual void                ExtinguishCarFire() = 0;
    virtual void                PlaceOnRoadProperly() = 0;

    virtual CDamageManager*     GetDamageManager() = 0;

    virtual float               GetBurningTime() const = 0;
    virtual void                SetBurningTime( float time ) = 0;

    virtual float               GetNitrousFuel() const = 0;
    virtual void                SetNitrousFuel( float val ) = 0;

    virtual SColor              GetHeadLightColor() const = 0;
    virtual void                SetHeadLightColor( const SColor color ) = 0;

    virtual CObject*            SpawnFlyingComponent( int i_1, unsigned int ui_2 ) = 0;
    virtual void                SetWheelVisibility( eWheels wheel, bool bVisible ) = 0;
    virtual bool                GetWheelVisibility( eWheels wheel ) const = 0;

    virtual unsigned short      GetAdjustablePropertyValue() const = 0;
    virtual void                SetAdjustablePropertyValue( unsigned short value ) = 0;

    virtual void                SetTaxiLightOn( bool on ) = 0;
    virtual bool                IsTaxiLightOn() const = 0;

    virtual void                GetTurretRotation( float& horizontal, float& vertical ) const = 0;
    virtual void                SetTurretRotation( float horizontal, float vertical ) = 0;

    virtual void*               GetPrivateSuspensionLines() = 0;

    virtual void                RecalculateHandling() = 0;
    virtual void                RecalculateSuspensionLines() = 0;
    virtual bool                UpdateMovingCollision( float angle ) = 0;
};

#endif

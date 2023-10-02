/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CVehicle.h
*  PURPOSE:     Vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_VEHICLE
#define __CGAME_VEHICLE

#include "CColPoint.h"
#include "CPhysical.h"
#include "CDamageManager.h"
#include "CHandlingManager.h"
#include "CDoor.h"
#include "CWeaponInfo.h"
#include "CVehicleComponent.h"

enum eWinchType
{
    WINCH_NONE = 0,
    WINCH_BIG_MAGNET,
    WINCH_SMALL_MAGNET = 3,
    NUM_WINCH_TYPE
};

class CPed;
class CObject;
class CColModel;
class CRpAtomic;
class CModel;

class CVehicle : public virtual CPhysical
{
public:
    virtual                     ~CVehicle() {};

    virtual unsigned int        GetPoolIndex() const = 0;

    virtual float               GetHealth() const = 0;
    virtual void                SetHealth( float fHealth ) = 0;

    virtual unsigned char       GetNumberGettingIn() const = 0;
    virtual unsigned char       GetPassengerCount() const = 0;
    virtual unsigned char       GetMaxPassengerCount() const = 0;

    virtual CDoor*              GetDoor( unsigned char ucDoor ) = 0;
    virtual void                OpenDoor( unsigned char ucDoor, float fRatio, bool bMakeNoise = false ) = 0;
    virtual bool                AreDoorsLocked() const = 0;
    virtual void                LockDoors( bool bLocked ) = 0;
    virtual bool                AreDoorsUndamageable() const = 0;
    virtual void                SetDoorsUndamageable( bool bUndamageable ) = 0;

    virtual void                SetAlpha( unsigned char alpha ) = 0;
    virtual unsigned char       GetAlpha() const = 0;
    virtual void                SetColor( SColor color1, SColor color2, SColor color3, SColor color4, int ) = 0;
    virtual void                GetColor( SColor& color1, SColor& color2, SColor& color3, SColor& color4, int ) const = 0;

    virtual CModel*             CloneClump() const = 0;
    virtual CVehicleComponent*  GetComponent( const char *name ) = 0;
    virtual void                GetComponentNameList( std::vector <std::string>& list ) = 0;

    // Virtually internal shared functions
    virtual void                BurstTyre( unsigned char tyre ) = 0;
    virtual bool                CanPedEnterCar() const = 0;
    virtual bool                CanPedJumpOutCar( CPed *ped ) const = 0;
    virtual bool                CanPedLeanOut( CPed *ped ) const = 0;
    virtual bool                CanPedStepOutCar( bool unk ) const = 0;
    virtual bool                CarHasRoof() const = 0;
    virtual bool                GetTowBarPos( CVector& pos ) const = 0;
    virtual bool                GetTowHitchPos( CVector& pos ) const = 0;
    virtual bool                SetTowLink( CVehicle *vehicle ) = 0;
    virtual bool                BreakTowLink() = 0;
    virtual float               GetHeightAboveRoad() const = 0;
    virtual void                Fix() = 0;
    virtual void                BlowUp( CEntity *creator, unsigned long unk ) = 0;
    virtual void                BlowUpCutSceneNoExtras( unsigned int unk1, unsigned int unk2, unsigned int unk3, unsigned int unk4 ) = 0;
    virtual void                RecalculateSuspensionLines() = 0;

    virtual CVehicle*           GetTowedVehicle() const = 0;
    virtual CVehicle*           GetTowedByVehicle() const = 0;

    virtual void                SetExplodeTime( unsigned long ulTime ) = 0;
    virtual unsigned long       GetExplodeTime() const = 0;

    virtual bool                IsOnItsSide() const = 0;
    virtual bool                IsLawEnforcementVehicle() const = 0;

    virtual void                SetGasPedal( float percent ) = 0;
    virtual void                SetBrakePedal( float percent ) = 0;
    virtual void                SetSteerAngle( float rad ) = 0;
    virtual void                SetSecSteerAngle( float rad ) = 0;

    virtual unsigned char       GetCurrentGear() const = 0;
    virtual float               GetGasPedal() const = 0;
    virtual float               GetBrakePedal() const = 0;
    virtual float               GetSteerAngle() const = 0;
    virtual float               GetSecSteerAngle() const = 0;

    virtual bool                AddProjectile( eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target ) = 0;
    virtual void                AddVehicleUpgrade( unsigned short model ) = 0;
    virtual void                RemoveVehicleUpgrade( unsigned short model ) = 0;

    virtual unsigned int        GetBaseVehicleType() const = 0;

    virtual void                SetBodyDirtLevel( float fDirtLevel ) = 0;
    virtual float               GetBodyDirtLevel() const = 0;

    virtual CPed*               GetDriver() const = 0;
    virtual CPed*               GetPassenger( unsigned char ucSlot ) const = 0;
    virtual bool                IsBeingDriven() const = 0;
    virtual bool                IsPassenger( CPed *ped ) const = 0;
    virtual bool                IsSphereTouchingVehicle( const CVector& vecOrigin, float fRadius ) const = 0;
    virtual bool                IsUpsideDown() const = 0;
    virtual void                MakeDirty( CColPoint *point ) = 0;

    virtual void                PlaceOnRoadProperly() = 0;

    virtual void                SetRemap( int remap ) = 0;
    virtual int                 GetRemapIndex() const = 0;
    virtual void                SetRemapTexDictionary( int txd ) = 0;

    virtual bool                IsEngineBroken() const = 0;
    virtual bool                IsScriptLocked() const = 0;
    virtual bool                IsDamaged() const = 0;
    virtual bool                IsDrowning() const = 0;
    virtual bool                IsEngineOn() const = 0;
    virtual bool                IsHandbrakeOn() const = 0;
    virtual bool                IsRCVehicle() const = 0;
    virtual bool                GetAlwaysLeaveSkidMarks() const = 0;
    virtual bool                GetCanBeDamaged() const = 0;
    virtual bool                GetCanBeTargettedByHeatSeekingMissiles() const = 0;
    virtual bool                GetCanShootPetrolTank() const = 0;
    virtual bool                GetChangeColourWhenRemapping() const = 0;
    virtual bool                GetComedyControls() const = 0;
    virtual bool                GetGunSwitchedOff() const = 0;
    virtual bool                GetLightsOn() const = 0;
    virtual bool                GetTakeLessDamage() const = 0;
    virtual bool                GetTyresDontBurst() const = 0;
    virtual bool                IsSirenOrAlarmActive() const = 0;
    virtual bool                IsFadingOut() const = 0;
    virtual unsigned int        GetOverrideLights() const = 0;
    
    virtual void                SetEngineBroken( bool bEngineBroken ) = 0;
    virtual void                SetScriptLocked( bool bLocked ) = 0;
    virtual void                SetAlwaysLeaveSkidMarks( bool bAlwaysLeaveSkidMarks ) = 0;
    virtual void                SetCanBeDamaged( bool bCanBeDamaged ) = 0;
    virtual void                SetCanBeTargettedByHeatSeekingMissiles( bool bEnabled ) = 0;
    virtual void                SetCanShootPetrolTank( bool bCanShoot ) = 0;
    virtual void                SetChangeColourWhenRemapping( bool bChangeColour ) = 0;
    virtual void                SetComedyControls( bool bComedyControls ) = 0;
    virtual void                SetEngineOn( bool bEngineOn ) = 0;
    virtual void                SetGunSwitchedOff( bool bGunsOff ) = 0;
    virtual void                SetHandbrakeOn( bool bHandbrakeOn ) = 0;
    virtual void                SetLightsOn( bool bLightsOn ) = 0;
    virtual void                SetOverrideLights( unsigned int uiOverrideLights ) = 0;
    virtual void                SetTakeLessDamage( bool bTakeLessDamage ) = 0;
    virtual void                SetTyresDontBurst( bool bTyresDontBurst ) = 0;
    virtual void                SetSirenOrAlarmActive( bool active ) = 0;
    virtual void                SetFadingOut( bool enable ) = 0;

    virtual CHandlingEntry*     GetHandlingData() = 0;
    virtual void                SetHandlingData( CHandlingEntry *handling ) = 0;

    virtual void                GetGravity( CVector& grav ) const = 0;
    virtual void                SetGravity( const CVector& grav ) = 0;

    virtual CColModel*          GetSpecialColModel() const = 0;

    virtual void                RecalculateHandling() = 0;
};

#endif

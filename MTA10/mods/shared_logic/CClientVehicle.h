/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicle.h
*  PURPOSE:     Vehicle entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

struct CClientVehicleProperties;
class CClientVehicle;

#ifndef __CCLIENTVEHICLE_H
#define __CCLIENTVEHICLE_H

#include <game/CPlane.h>
#include <game/CVehicle.h>

#include "CClientCommon.h"
#include "CClientCamera.h"
#include "CClientModelRequestManager.h"
#include "CClientPed.h"
#include "CClientGameEntity.h"
#include "CClientVehicleManager.h"
#include "CVehicleUpgrades.h"
#include "CClientTime.h"

#include "luadefs/luavehicle.h"

#define INVALID_PASSENGER_SEAT 0xFF
#define DEFAULT_VEHICLE_HEALTH 1000
#define MAX_VEHICLE_HEALTH 10000

enum eClientVehicleType
{
    CLIENTVEHICLE_NONE,
    CLIENTVEHICLE_CAR,
    CLIENTVEHICLE_BOAT,
    CLIENTVEHICLE_TRAIN,
    CLIENTVEHICLE_HELI,
    CLIENTVEHICLE_PLANE,
    CLIENTVEHICLE_BIKE,
    CLIENTVEHICLE_MONSTERTRUCK,
    CLIENTVEHICLE_QUADBIKE,
    CLIENTVEHICLE_BMX,
    CLIENTVEHICLE_TRAILER
};

enum eDelayedSyncVehicleData
{
    DELAYEDSYNC_VEHICLE_KEYSYNC,
    DELAYEDSYNC_VEHICLE_ROTATION,
    DELAYEDSYNC_VEHICLE_MOVESPEED,
    DELAYEDSYNC_VEHICLE_TURNSPEED,
};

struct SDelayedSyncVehicleData
{
    unsigned long       ulTime;
    unsigned char       ucType;
    CControllerState    State;
    CVector             vecTarget;
    CVector             vecTarget2;
    CVector             vecTarget3;
};

struct SLastSyncedVehData
{
    CVector             vecPosition;
    CVector             vecRotation;
    CVector             vecMoveSpeed;
    CVector             vecTurnSpeed;
    float               fHealth;
    ElementID           Trailer;
};

// We should get rid of this
typedef std::map <std::string, class CClientVehicleComponent*> vehComponents_t;

class CClientProjectile;
class CClientAtomic;

class CClientVehicle : public CClientGameEntity
{
    friend class CClientCamera;
    friend class CClientPed;
    friend class CClientVehicleManager;
    friend class CClientGame;
    friend class CClientVehicleComponent;

protected: // Use CDeathmatchVehicle constructor for now. Will get removed later when this class is
           // cleaned up.
                                CClientVehicle( CClientManager* pManager, ElementID ID, lua_State *L, bool system, unsigned short usModel );

public:
                                ~CClientVehicle();
    
    void                        Unlink();

    inline eClientEntityType    GetType() const                                                         { return CCLIENTVEHICLE; }

    void                        GetName( char *szBuf ) const;
    inline const char*          GetNamePointer() const                                                  { return m_pModelInfo->GetNameIfVehicle(); }
    inline eClientVehicleType   GetVehicleType() const                                                  { return m_eVehicleType; }
    inline eVehicleType         GetModelType() const                                                    { return m_vehModelType; }

    void                        GetPosition( CVector& vecPosition ) const;
    void                        SetPosition( const CVector& vecPosition )                               { SetPosition ( vecPosition, true ); }
    void                        SetPosition( const CVector& vecPosition, bool bResetInterpolation );

    void                        GetRotationDegrees( CVector& vecRotation ) const;
    void                        GetRotationRadians( CVector& vecRotation ) const;
    void                        SetRotationDegrees( const CVector& vecRotation )                        { SetRotationDegrees ( vecRotation, true ); }
    void                        SetRotationDegrees( const CVector& vecRotation, bool bResetInterpolation );
    void                        SetRotationRadians( const CVector& vecRotation )                        { SetRotationRadians ( vecRotation, true ); }
    void                        SetRotationRadians( const CVector& vecRotation, bool bResetInterpolation );
    
    void                        AttachTo( CClientEntity * pEntity );

    float                       GetDistanceFromCentreOfMassToBaseOfModel() const;

    bool                        GetMatrix( RwMatrix& Matrix ) const;
    bool                        SetMatrix( const RwMatrix& Matrix );
    virtual CSphere             GetWorldBoundingSphere() const;

    void                        GetMoveSpeed( CVector& vecMoveSpeed ) const;
    void                        GetMoveSpeedMeters( CVector& vecMoveSpeed ) const;
    void                        SetMoveSpeed( const CVector& vecMoveSpeed );
    void                        GetTurnSpeed( CVector& vecTurnSpeed ) const;
    void                        SetTurnSpeed( const CVector& vecTurnSpeed );

    bool                        IsVisible() const;
    void                        SetVisible( bool bVisible );

    void                        SetDoorOpenRatio( unsigned char ucDoor, float fRatio, unsigned long ulDelay = 0, bool bForced = false );
    float                       GetDoorOpenRatio( unsigned char ucDoor ) const;
    void                        SetSwingingDoorsAllowed( bool bAllowed );
    bool                        AreSwingingDoorsAllowed() const;
    void                        AllowDoorRatioSetting( unsigned char ucDoor, bool bAllow );
    bool                        AreDoorsLocked() const;
    void                        SetDoorsLocked( bool bLocked );

private:
    void                        SetDoorOpenRatioInterpolated( unsigned char ucDoor, float fRatio, unsigned long ulDelay );
    void                        ResetDoorInterpolation();
    void                        CancelDoorInterpolation( unsigned char ucDoor );
    void                        ProcessDoorInterpolation();

public:
    bool                        AreDoorsUndamageable() const;
    void                        SetDoorsUndamageable( bool bUndamageable );

    float                       GetHealth() const;
    void                        SetHealth( float fHealth );
    void                        SetArmored( bool enable );
    bool                        IsArmored() const;
    void                        Fix();
    void                        Blow( bool bAllowMovement = false );
    void                        SetExplodeTime( unsigned long time );
    unsigned long               GetExplodeTime() const;
    void                        SetBurningTime( float time );
    float                       GetBurningTime() const;

    CVehicleColor&              GetColor();
    void                        SetColor( const CVehicleColor& color );

    void                        GetTurretRotation( float& fHorizontal, float& fVertical ) const;
    void                        SetTurretRotation( float fHorizontal, float fVertical );

    inline unsigned short       GetModel() const                                                        { return m_usModel; }
    void                        SetModelBlocking( unsigned short usModel, bool bLoadImmediately = false );

    bool                        IsEngineBroken() const;
    void                        SetEngineBroken( bool bEngineBroken );

    bool                        IsEngineOn() const;
    void                        SetEngineOn( bool bEngineOn );

    bool                        CanBeDamaged() const;
    void                        CalcAndUpdateCanBeDamagedFlag();
    void                        SetScriptCanBeDamaged( bool bCanBeDamaged );
    void                        SetSyncUnoccupiedDamage( bool bCanBeDamaged );
    bool                        GetScriptCanBeDamaged() const                                           { return m_bScriptCanBeDamaged; }

    bool                        GetTyresCanBurst() const;
    void                        CalcAndUpdateTyresCanBurstFlag();

    void                        SetBrakePedal( float percent );
    void                        SetGasPedal( float percent );
    void                        SetSteerAngle( float rad );
    void                        SetSecSteerAngle( float rad );

    float                       GetBrakePedal() const;
    float                       GetGasPedal() const;
    float                       GetSteerAngle() const;
    float                       GetSecSteerAngle() const;

    void                        SetNitrousFuel( float val );
    float                       GetNitrousFuel() const;

    bool                        IsBelowWater() const;
    bool                        IsDrowning() const;
    bool                        IsDriven();
    bool                        IsUpsideDown() const;
    bool                        IsBlown() const;

    bool                        IsSirenOrAlarmActive() const;
    void                        SetSirenOrAlarmActive( bool bActive );
    bool                        IsHandbrakeOn() const;
    void                        SetHandbrakeOn( bool enable );

    float                       GetLandingGearPosition() const;
    void                        SetLandingGearPosition( float fPosition );
    bool                        IsLandingGearDown() const;
    void                        SetLandingGearDown( bool bLandingGearDown );

    unsigned short              GetAdjustablePropertyValue() const;
    void                        SetAdjustablePropertyValue( unsigned short usValue );
    bool                        HasMovingCollision() const;
private:
    void                        _SetAdjustablePropertyValue( unsigned short usValue );
public:

    unsigned char               GetDoorStatus( unsigned char ucDoor ) const;
    unsigned char               GetWheelStatus( unsigned char ucWheel ) const;
    unsigned char               GetPanelStatus( unsigned char ucPanel ) const;
    unsigned char               GetLightStatus( unsigned char ucLight ) const;

    void                        SetDoorStatus( unsigned char ucDoor, unsigned char ucStatus );
    void                        SetWheelStatus( unsigned char ucWheel, unsigned char ucStatus, bool bSilent = true );
    void                        SetPanelStatus( unsigned char ucPanel, unsigned char ucStatus );
    void                        SetLightStatus( unsigned char ucLight, unsigned char ucStatus );

    CClientVehicleComponent*    GetComponent( const char *name );

    // TODO: Make the class remember on virtualization
    float                       GetHeliRotorSpeed() const;
    void                        SetHeliRotorSpeed( float fSpeed );

    bool                        IsHeliSearchLightVisible() const;
    void                        SetHeliSearchLightVisible( bool bVisible );

    void                        ReportMissionAudioEvent( unsigned short usSound );

    bool                        IsCollisionEnabled() const;
    void                        SetCollisionEnabled( bool bCollisionEnabled );

    bool                        GetCanShootPetrolTank() const;
    void                        SetCanShootPetrolTank( bool bCanShoot );
    
    bool                        GetCanBeTargettedByHeatSeekingMissiles() const;
    void                        SetCanBeTargettedByHeatSeekingMissiles( bool bEnabled );

    inline unsigned char        GetAlpha() const                                                        { return m_ucAlpha; }
    void                        SetAlpha( unsigned char ucAlpha );

    CClientPed*                 GetOccupant( int iSeat = 0 );
    CClientPed*                 GetControllingPlayer();
    void                        ClearForOccupants();

    void                        PlaceProperlyOnGround();

    void                        FuckCarCompletely( bool bKeepWheels );

    void                        WorldIgnore( bool bWorldIgnore );

    inline bool                 IsVirtual() const                                                       { return m_pVehicle == NULL; }

    void                        FadeOut( bool bFadeOut );
    bool                        IsFadingOut() const;

    inline bool                 IsFrozen() const                                                        { return m_bIsFrozen; }
    void                        SetFrozen( bool bFrozen );
    void                        SetScriptFrozen( bool bFrozen )                                         { m_bScriptFrozen = bFrozen; }
    bool                        IsFrozenWaitingForGroundToLoad() const;
    void                        SetFrozenWaitingForGroundToLoad( bool bFrozen );

    CClientVehicle*             GetPreviousTrainCarriage();
    CClientVehicle*             GetNextTrainCarriage();
    void                        SetPreviousTrainCarriage( CClientVehicle *previous );
    void                        SetNextTrainCarriage( CClientVehicle *next );

    bool                        IsDerailed() const;
    void                        SetDerailed( bool bDerailed );
    bool                        IsDerailable() const;
    void                        SetDerailable( bool bDerailable );

    bool                        GetTrainDirection() const;
    void                        SetTrainDirection( bool bDirection );

    float                       GetTrainSpeed() const;
    void                        SetTrainSpeed( float fSpeed );

    inline unsigned char        GetOverrideLights() const                                               { return m_ucOverrideLights; }
    void                        SetOverrideLights( unsigned char ucOverrideLights );
    bool                        SetTaxiLightOn( bool bLightOn );
    bool                        IsTaxiLightOn() { return m_bTaxiLightOn; }
    inline CVehicle*            GetGameVehicle()                                                        { return m_pVehicle; }
    inline CEntity*             GetGameEntity()                                                         { return m_pVehicle; }
    inline const CEntity*       GetGameEntity() const                                                   { return m_pVehicle; }
    inline CVehicleUpgrades*    GetUpgrades()                                                           { return m_pUpgrades; }
    inline CModelInfo*          GetModelInfo()                                                          { return m_pModelInfo; }

    CClientVehicle*             GetTowedVehicle();
    CClientVehicle*             GetRealTowedVehicle();
    bool                        SetTowedVehicle( CClientVehicle* pVehicle );
    inline CClientVehicle*      GetTowedByVehicle()                                                     { return m_pTowedByVehicle; }

    eWinchType                  GetWinchType() const                                                    { return m_eWinchType; }
    bool                        SetWinchType( eWinchType winchType );
    bool                        PickupEntityWithWinch( CClientEntity *entity );
    bool                        ReleasePickedUpEntityWithWinch();
    void                        SetRopeHeightForHeli( float height );
    float                       GetRopeHeightForHeli() const;
    CClientEntity*              GetPickedUpEntityWithWinch() const;

    inline const char*          GetRegPlate() const                                                     { return m_strRegPlate.empty () ? NULL : m_strRegPlate.c_str (); }
    void                        SetRegPlate( const char* szPlate );

    unsigned char               GetPaintjob() const;
    void                        SetPaintjob( unsigned char ucPaintjob );
    
    float                       GetDirtLevel() const;
    void                        SetDirtLevel( float fDirtLevel );

    float                       GetDistanceFromGround() const;

    void                        SetInWater( bool bState )                                               { m_bIsInWater = bState; }
    bool                        IsInWater() const;
    bool                        IsOnGround() const;
    bool                        IsOnWater() const;
    void                        LockSteering( bool bLock );

    bool                        IsSmokeTrailEnabled() const;
    void                        SetSmokeTrailEnabled( bool bEnabled );

    void                        ResetInterpolation();

    void                        Interpolate();
    void                        UpdateKeysync();

    void                        GetInitialDoorStates( unsigned char *pucDoorStates ) const;

    void                        AddMatrix( RwMatrix& Matrix, double dTime, unsigned short usTickRate );
    void                        AddVelocity( CVector& vecVelocity );


    // Time dependent interpolation
    inline void                 GetTargetPosition( CVector& vecPosition ) const                         { vecPosition = m_interp.pos.vecTarget; }
    void                        SetTargetPosition( CVector& vecPosition, unsigned long ulDelay, bool bValidVelocityZ = false, float fVelocityZ = 0.f );
    void                        RemoveTargetPosition();
    inline bool                 HasTargetPosition() const                                               { return ( m_interp.pos.ulFinishTime != 0 ); }

    inline void                 GetTargetRotation( CVector& vecRotation ) const                         { vecRotation = m_interp.rot.vecTarget; }
    void                        SetTargetRotation( CVector& vecRotation, unsigned long ulDelay );
    void                        RemoveTargetRotation();
    inline bool                 HasTargetRotation() const                                               { return ( m_interp.rot.ulFinishTime != 0 ); }

    void                        UpdateTargetPosition();
    void                        UpdateTargetRotation();


    inline unsigned long        GetIllegalTowBreakTime() const                                          { return m_ulIllegalTowBreakTime; }
    inline void                 SetIllegalTowBreakTime( unsigned long ulTime )                          { m_ulIllegalTowBreakTime = ulTime; }

    void                        GetGravity( CVector& vecGravity ) const                                 { vecGravity = m_vecGravity; }
    void                        SetGravity( const CVector& vecGravity );

    SColor                      GetHeadLightColor() const;
    void                        SetHeadLightColor( const SColor color );

    int                         GetCurrentGear() const;

    bool                        IsEnterable() const;
    bool                        HasRadio() const;
    bool                        HasPoliceRadio() const;

    void                        ReCreate();

    void                        ModelRequestCallback( CModelInfo* pModelInfo );

    // Warning: Don't use this to create a vehicle if CClientVehicleManager::IsVehicleLimitReached
    //          returns true. Also this messes with streaming so don't Destroy something unless
    //          you're going to recreate it very quickly again. CClientVehicleManager::IsVehicleLimitReached
    //          returns true often when the player is in an area full of vehicles so don't fatal
    //          error or something if it does return true.
    void                        Create();
    void                        Destroy();

    typedef std::list <CClientProjectile*> projectiles_t;

    inline void                 AddProjectile( CClientProjectile *projectile )                          { m_Projectiles.push_back( projectile ); }
    inline void                 RemoveProjectile( CClientProjectile *projectile )                       { m_Projectiles.remove( projectile ); }
    projectiles_t::iterator     ProjectilesBegin()                                                      { return m_Projectiles.begin (); }
    projectiles_t::iterator     ProjectilesEnd()                                                        { return m_Projectiles.end (); }

    void                        RemoveAllProjectiles();

    static void                 SetPedOccupiedVehicle( CClientPed *pClientPed, CClientVehicle *pVehicle, unsigned int uiSeat, unsigned char ucDoor );
    static void                 SetPedOccupyingVehicle( CClientPed *pClientPed, CClientVehicle *pVehicle, unsigned int uiSeat, unsigned char ucDoor );
    static void                 ValidatePedAndVehiclePair( CClientPed *pClientPed, CClientVehicle *pVehicle );
    static void                 UnpairPedAndVehicle( CClientPed *pClientPed, CClientVehicle *pVehicle );
    static void                 UnpairPedAndVehicle( CClientPed *pClientPed );
    
    void                        ApplyHandling();
    CHandlingEntry*             GetHandlingData();
    const CHandlingEntry*       GetOriginalHandlingData() const                                         { return m_pOriginalHandlingEntry; }

protected:
    void                        StreamIn( bool bInstantly );
    void                        StreamOut();

    void                        NotifyCreate();
    void                        NotifyDestroy();

    void                        HandleWaitingForGroundToLoad();

    void                        StreamedInPulse();
    void                        Dump( FILE* pFile, bool bDumpDetails, unsigned int uiIndex );

    class CClientObjectManager* m_pObjectManager;
    CClientVehicleManager*      m_pVehicleManager;
    CClientModelRequestManager* m_pModelRequester;
    unsigned short              m_usModel;
    eClientVehicleType          m_eVehicleType;
    eVehicleType                m_vehModelType;
    unsigned char               m_ucMaxPassengers;
    bool                        m_bIsVirtualized;
    CVehicle*                   m_pVehicle;
    CTrain*                     m_train;
    CBoat*                      m_boat;
    CPlane*                     m_plane;
    CAutomobileTrailer*         m_trailer;
    CQuadBike*                  m_quad;
    CMonsterTruck*              m_monster;
    CBike*                      m_bike;
    CBicycle*                   m_bicycle;
    CHeli*                      m_heli;
    CAutomobile*                m_automobile;
    CClientPed*                 m_pDriver;
    CClientPed*                 m_pPassengers[8];
    CClientPed*                 m_pOccupyingDriver;
    CClientPed*                 m_pOccupyingPassengers[8];
    short                       m_usRemoveTimer;

    CClientVehicle*             m_pPreviousLink;
    CClientVehicle*             m_pNextLink;
    RwMatrix                    m_Matrix;
    RwMatrix                    m_MatrixLast;
    RwMatrix                    m_MatrixPure;   
    CVector                     m_vecMoveSpeedInterpolate;
    CVector                     m_vecMoveSpeedMeters;
    CVector                     m_vecMoveSpeed;
    CVector                     m_vecTurnSpeed;
    float                       m_fHealth;
    unsigned long               m_explodeTime;
    float                       m_burningTime;
    float                       m_fTurretHorizontal;
    float                       m_fTurretVertical;
    float                       m_brakePedal;
    float                       m_fGasPedal;
    float                       m_steerAngle;
    float                       m_secSteerAngle;
    float                       m_nitrousFuel;
    bool                        m_bVisible;
    bool                        m_armored;
    bool                        m_bIsCollisionEnabled;
    bool                        m_bEngineOn;
    bool                        m_bEngineBroken;
    bool                        m_bSireneOrAlarmActive;
    bool                        m_bLandingGearDown;
    unsigned short              m_usAdjustablePropertyValue;
    bool                        m_bAllowDoorRatioSetting[6];
    float                       m_fDoorOpenRatio[6];

    struct
    {
        float                   fStart[6];
        float                   fTarget[6];
        unsigned long           ulStartTime[6];
        unsigned long           ulTargetTime[6];
    } m_doorInterp;

    bool                        m_bSwingingDoorsAllowed;
    bool                        m_bDoorsLocked;
    bool                        m_bDoorsUndamageable;
    bool                        m_bCanShootPetrolTank;
    bool                        m_bCanBeTargettedByHeatSeekingMissiles;
    bool                        m_bCanBeDamaged;
    bool                        m_bScriptCanBeDamaged;
    bool                        m_bSyncUnoccupiedDamage;
    bool                        m_bTyresCanBurst;
    unsigned char               m_ucDoorStates[MAX_DOORS];
    unsigned char               m_ucWheelStates[MAX_WHEELS];
    unsigned char               m_ucPanelStates[MAX_PANELS];
    unsigned char               m_ucLightStates[MAX_LIGHTS];
    vehComponents_t             m_compContainer;
    bool                        m_bJustBlewUp;
    eEntityStatus               m_NormalStatus;
    bool                        m_bColorSaved;
    CVehicleColor               m_Color;
    bool                        m_handbrake;
    bool                        m_bIsFrozen;
    bool                        m_bScriptFrozen;
    bool                        m_bFrozenWaitingForGroundToLoad;
    float                       m_fGroundCheckTolerance;
    float                       m_fObjectsAroundTolerance;
    RwMatrix                    m_matFrozen;
    CVehicleUpgrades*           m_pUpgrades;
    unsigned char               m_ucOverrideLights;
    CClientVehicle*             m_pTowedVehicle;
    CClientVehicle*             m_pTowedByVehicle;
    eWinchType                  m_eWinchType;
    CClientEntity*              m_pPickedUpWinchEntity;
    std::string                 m_strRegPlate;
    unsigned char               m_ucPaintjob;
    float                       m_fDirtLevel;
    bool                        m_bSmokeTrail;
    unsigned char               m_ucAlpha;
    bool                        m_bAlphaChanged;
    double                      m_dLastRotationTime;
    bool                        m_bBlowNextFrame;
    bool                        m_bIsOnGround;
    bool                        m_bHeliSearchLightVisible;
    float                       m_fHeliRotorSpeed;
    float                       m_heliRopeHeight;
    const CHandlingEntry*       m_pOriginalHandlingEntry;
    CHandlingEntry*             m_pHandlingEntry;

    bool                        m_bIsDerailed;
    bool                        m_bIsDerailable;
    bool                        m_bTrainDirection;
    float                       m_fTrainSpeed;

    // Time dependent error compensation interpolation
    struct
    {
        struct
        {
#ifdef MTA_DEBUG
            CVector         vecStart;
#endif
            CVector         vecTarget;
            CVector         vecError;
            float           fLastAlpha;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
        } pos;

        struct
        {
#ifdef MTA_DEBUG
            CVector         vecStart;
#endif
            CVector         vecTarget;
            CVector         vecError;
            float           fLastAlpha;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
        } rot;
    } m_interp;

    unsigned long               m_ulIllegalTowBreakTime;

    bool                        m_bBlown;

    bool                        m_bTaxiLightOn;
    projectiles_t               m_Projectiles;

    bool                        m_bIsInWater;

    CVector                     m_vecGravity;
    SColor                      m_HeadLightColor;

    bool                        m_bHasCustomHandling;

public:
    CClientPlayer*              m_pLastSyncer;
    unsigned long               m_ulLastSyncTime;
    char*                       m_szLastSyncType;
    SLastSyncedVehData*         m_LastSyncedData;
};

#endif

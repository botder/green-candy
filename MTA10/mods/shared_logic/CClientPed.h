/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPed.h
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientPed;

#ifndef __CCLIENTPED_H
#define __CCLIENTPED_H

#include "CAntiCheatModule.h"
#include "CClientCommon.h"
#include "CClientGameEntity.h"

#include <multiplayer/CMultiplayer.h>
#include "net/Packets.h"
#include "CClientPad.h"

#include "luadefs/luaped.h"

class CClientCamera;
class CClientManager;
class CClientModelRequestManager;
class CClientPed;
class CClientPedManager;
class CClientPlayer;
class CClientPlayerClothes;
class CClientVehicle;
class CClientProjectile;

enum eDelayedSyncData
{
    DELAYEDSYNC_KEYSYNC,
    DELAYEDSYNC_CHANGEWEAPON,
    DELAYEDSYNC_MOVESPEED,
};

enum eVehicleInOutState
{
    VEHICLE_INOUT_NONE = 0,
    VEHICLE_INOUT_GETTING_IN,
    VEHICLE_INOUT_GETTING_OUT,
    VEHICLE_INOUT_JACKING,
    VEHICLE_INOUT_GETTING_JACKED,
};

enum eBodyPart
{
    BODYPART_TORSO = 3,
    BODYPART_ASS = 4,
    BODYPART_LEFT_ARM = 5,
    BODYPART_RIGHT_ARM = 6,
    BODYPART_LEFT_LEG = 7,
    BODYPART_RIGHT_LEG = 8,
    BODYPART_HEAD = 9,
};

enum eMovementState
{
    MOVEMENTSTATE_UNKNOWN,
    MOVEMENTSTATE_STAND, //Standing still
    MOVEMENTSTATE_WALK, //Walking
    MOVEMENTSTATE_POWERWALK, //Walking quickly
    MOVEMENTSTATE_JOG, //Jogging
    MOVEMENTSTATE_SPRINT, //Sprinting
    MOVEMENTSTATE_CROUCH, //Crouching still
    // Duds for now.  We should add methods to detect these
    MOVEMENTSTATE_CRAWL, //Crouch-moving
    MOVEMENTSTATE_ROLL, //Crouch-rolling
};

enum eDeathAnims
{
    DEATH_ANIM_HEAD = 19,
    DEATH_ANIM_TORSO = 20,
};

struct SDelayedSyncData
{
    unsigned long       ulTime;
    unsigned char       ucType;
    CControllerState    State;
    bool                bDucking;
    eWeaponSlot         slot;
    unsigned short      usWeaponAmmo;
    CVector             vecTarget;
    bool                bUseSource;
    CVector             vecSource;
};

struct SLastSyncedPedData
{
    float               fHealth;
    float               fArmour;
    CVector             position;
    CVector             vVelocity;
    float               fRotation;
};

class CClientObject;

// To hide the ugly "pointer truncation from DWORD* to unsigned long warning
#pragma warning(disable:4311)

class CClientPed : public CClientGameEntity, public CAntiCheatModule
{
    friend class CClientCamera;
    friend class CClientPlayer;
    friend class CClientVehicle;
    friend class CClientPedManager;
public:
                                CClientPed( CClientManager *pManager, unsigned short ulModelID, ElementID ID, lua_State *L, bool system );
                                ~CClientPed();

    inline void                 Unlink()                                                    {}

    virtual eClientEntityType   GetType() const                                             { return CCLIENTPED; }
    
    inline CPlayerPed*          GetGamePlayer()                                             { return m_pPlayerPed; }
    inline CEntity*             GetGameEntity()                                             { return m_pPlayerPed; }
    inline const CEntity*       GetGameEntity() const                                       { return m_pPlayerPed; }

    inline bool                 IsLocalPlayer()                                             { return m_bIsLocalPlayer; }

    bool                        GetMatrix( RwMatrix& Matrix ) const;
    bool                        SetMatrix( const RwMatrix& Matrix );
    virtual CSphere             GetWorldBoundingSphere();

    void                        GetPosition( CVector& vecPosition ) const;
    void                        SetPosition( const CVector& vecPosition )                   { SetPosition( vecPosition, true ); }
    void                        SetPosition( const CVector& vecPosition, bool bResetInterpolation );

    void                        SetInterior( unsigned char ucInterior );

    void                        GetRotationDegrees( CVector& vecRotation ) const;
    void                        GetRotationRadians( CVector& vecRotation ) const;
    void                        SetRotationDegrees( const CVector& vecRotation );
    void                        SetRotationRadians( const CVector& vecRotation );

    void                        Teleport( const CVector& vecPosition );

    // This function spawns/respawns this ped in any location. This will force a recreation
    // and restoration of initial state. This will also remove all weapons, unfreeze,
    // remove jetpack, etc...
    void                        Spawn( const CVector& vecPosition, float fRotation, unsigned short usModel, unsigned char ucInterior );

    void                        SetTargetPosition( unsigned long ulDelay, const CVector& vecPosition );
    void                        ResetInterpolation();

    float                       GetCurrentRotation();
    void                        SetCurrentRotation( float fRotation, bool bIncludeTarget = true );
    void                        SetTargetRotation( float fRotation );
    void                        SetTargetRotation( unsigned long ulDelay, float fRotation, float fCameraRotation );

    float                       GetCameraRotation();
    void                        SetCameraRotation( float fRotation );

    void                        GetMoveSpeed( CVector& vecMoveSpeed ) const;
    void                        SetMoveSpeed( const CVector& vecMoveSpeed );

    void                        GetTurnSpeed( CVector& vecTurnSpeed ) const;
    void                        SetTurnSpeed( const CVector& vecTurnSpeed );

    void                        GetControllerState( CControllerState& ControllerState );
    void                        GetLastControllerState( CControllerState& ControllerState );
    void                        SetControllerState( const CControllerState& ControllerState );

    void                        AddKeysync( unsigned long ulDelay, const CControllerState& ControllerState, bool bDucking );
    void                        AddChangeWeapon( unsigned long ulDelay, eWeaponSlot slot, unsigned short usWeaponAmmo );
    void                        AddMoveSpeed( unsigned long ulDelay, const CVector& vecMoveSpeed );

    void                        SetTargetTarget( unsigned long ulDelay, const CVector& vecSource, const CVector& vecTarget );

    inline int                  GetVehicleInOutState()                                      { return m_iVehicleInOutState; }
    inline void                 SetVehicleInOutState( int iState )                          { m_iVehicleInOutState = iState; }

    inline unsigned short       GetModel()                                                  { return m_ulModel; }
    bool                        SetModel( unsigned short ulModel );

    bool                        GetCanBeKnockedOffBike();
    void                        SetCanBeKnockedOffBike( bool bCanBeKnockedOffBike );

    inline bool                 IsInVehicle()                                               { return GetOccupiedVehicle() != NULL; }
    inline CClientVehicle*      GetOccupiedVehicle()                                        { return m_pOccupiedVehicle; }
    inline unsigned int         GetOccupiedVehicleSeat()                                    { return m_uiOccupiedVehicleSeat; }
    inline CClientVehicle*      GetOccupyingVehicle()                                       { return m_pOccupyingVehicle; }

    CClientVehicle*             GetRealOccupiedVehicle();
    CClientVehicle*             GetClosestVehicleInRange( bool bGetPositionFromClosestDoor, bool bCheckDriverDoor, bool bCheckPassengerDoors, bool bCheckStreamedOutVehicles, unsigned int* uiClosestDoor = NULL, CVector* pClosestDoorPosition = NULL, float fWithinRange = 6000.0f );
    bool                        GetClosestDoor( CClientVehicle* pVehicle, bool bCheckDriverDoor, bool bCheckPassengerDoors, unsigned int& uiClosestDoor, CVector* pClosestDoorPosition = NULL );

    void                        GetIntoVehicle( CClientVehicle* pVehicle, unsigned int uiSeat = 0, unsigned char ucDoor = 0 );
    void                        GetOutOfVehicle( unsigned char ucDoor );

    void                        WarpIntoVehicle( CClientVehicle* pVehicle, unsigned int uiSeat = 0 );
    CClientVehicle*             RemoveFromVehicle( bool bIgnoreIfGettingOut = false );

    bool                        IsVisible();
    void                        SetVisible( bool bVisible );
    bool                        GetUsesCollision();
    void                        SetUsesCollision( bool bUsesCollision );

    float                       GetMaxHealth();
    float                       GetHealth();
    void                        SetHealth( float fHealth );
    void                        InternalSetHealth( float fHealth );
    float                       GetArmor();
    void                        SetArmor( float fArmor );

    void                        LockHealth( float fHealth );
    void                        LockArmor( float fArmor );
    inline void                 UnlockHealth()                                              { m_bHealthLocked = false; }
    inline void                 UnlockArmor()                                               { m_bArmorLocked = false; }
    inline bool                 IsHealthLocked() const                                      { return m_bHealthLocked; }
    inline bool                 IsArmorLocked() const                                       { return m_bArmorLocked; }

    bool                        IsDying();
    bool                        IsDead();
    void                        Kill( eWeaponType weaponType, unsigned char ucBodypart, bool bStealth = false, bool bSetDirectlyDead = false, AssocGroupId animGroup = 0, AnimationId animID = 15 );
    void                        StealthKill( CClientPed * pPed );

    inline int                  GetRespawnState()                                           { return m_pRespawnState; }
    inline void                 SetRespawnState( int iRespawnState )                        { m_pRespawnState = iRespawnState; }

    CWeapon*                    GiveWeapon( eWeaponType weaponType, unsigned int uiAmmo );
    bool                        SetCurrentWeaponSlot( eWeaponSlot weaponSlot );
    eWeaponSlot                 GetCurrentWeaponSlot();
    eWeaponType                 GetCurrentWeaponType();
    CWeapon*                    GetWeapon();
    CWeapon*                    GetWeapon( eWeaponSlot weaponSlot );
    CWeapon*                    GetWeapon( eWeaponType weaponType );
    bool                        HasWeapon( eWeaponType weaponType );
    void                        RemoveWeapon( eWeaponType weaponType );
    void                        RemoveAllWeapons();

    std::map <eMovementState,std::string> m_MovementStateNames;

    eMovementState              GetMovementState();
    bool                        GetMovementState( std::string& strStateName );

    CTask*                      GetCurrentPrimaryTask();
    bool                        IsSimplestTask( int iTaskType );
    bool                        HasTask( int iTaskType, int iTaskPriority = -1, bool bPrimary = true );
    bool                        SetTask( CTask* pTask, int iTaskPriority );
    bool                        SetTaskSecondary( CTask* pTask, int iTaskPriority );
    bool                        KillTask( int iTaskPriority, bool bGracefully = true );
    bool                        KillTaskSecondary( int iTaskPriority, bool bGracefully = true );

    void                        GetBonePosition( eBone bone, CVector& vecPosition ) const;
    void                        GetTransformedBonePosition( eBone bone, CVector& vecPosition ) const;

    inline void                 GetAim( float& fDirectionX, float& fDirectionY )            { if ( m_shotSyncData ) { fDirectionX = m_shotSyncData->m_fArmDirectionX; fDirectionY = m_shotSyncData->m_fArmDirectionY; } }
    inline const CVector&       GetAimSource()                                              { return m_shotSyncData->m_vecShotOrigin; }
    inline const CVector&       GetAimTarget()                                              { return m_shotSyncData->m_vecShotTarget; }
    inline unsigned char        GetVehicleAimAnim()                                         { return m_shotSyncData->m_cInVehicleAimDirection; }
    void                        SetAim( float fArmDirectionX, float fArmDirectionY, unsigned char cInVehicleAimAnim );
    void                        SetAimInterpolated( unsigned long ulDelay, float fArmDirectionX, float fArmDirectionY, bool bAkimboAimUp, unsigned char cInVehicleAimAnim );
    void                        SetAimingData( unsigned long ulDelay, const CVector& vecTargetPosition, float fArmDirectionX, float fArmDirectionY, char cInVehicleAimAnim, CVector* pSource, bool bInterpolateAim );

    void                        Duck( bool bDuck );
    bool                        IsDucked();

    void                        SetChoking( bool bChoking );
    bool                        IsChoking();
  
    void                        SetWearingGoggles( bool bWearing );
    inline bool                 IsWearingGoggles( bool bCheckMoving = false );
    bool                        IsMovingGoggles( bool& putOn );

    void                        WorldIgnore( bool bIgnore );

    void                        ResetToOutOfVehicleWeapon();

    void                        RebuildModel( bool bForceClothes = false );
    void                        SetStat( unsigned short usStat, float fValue );
    float                       GetStat( unsigned short usStat );
    void                        ResetStats();

    inline CClientPlayerClothes* GetClothes()                                               { return m_pClothes; }

    // This is kinda hacky, should be private but something depends on this. Should depend on some
    // streamer func. Perhaps use SetNeverStreamOut, but need something to reset that.
    void                        StreamIn( bool bInstantly );
    void                        StreamOut();

    bool                        SetHasJetPack( bool bHasJetPack );
    bool                        HasJetPack();

    float                       GetDistanceFromGround();

    bool                        IsInWater();
    bool                        IsOnGround();

    bool                        IsClimbing();
    bool                        IsRadioOn()                                                 { return m_bRadioOn; }
    void                        NextRadioChannel();
    void                        PreviousRadioChannel();
    bool                        SetCurrentRadioChannel( unsigned char ucChannel );
    inline unsigned char        GetCurrentRadioChannel()                                    { return m_ucRadioChannel; }

    inline CTaskManager*        GetTaskManager()                                            { return m_pTaskManager; }

    bool                        GetShotData( CVector * pvecOrigin, CVector * pvecTarget = NULL, CVector * pvecGunMuzzle = NULL, CVector * pvecFireOffset = NULL, float* fAimX = NULL, float* fAimY = NULL );

    eFightingStyle              GetFightingStyle();
    void                        SetFightingStyle( eFightingStyle style );

    eMoveAnim                   GetMoveAnim();
    void                        SetMoveAnim( eMoveAnim iAnim );

    typedef std::list <CClientProjectile*> projectileList_t;

    inline void                 AddProjectile( CClientProjectile * pProjectile )            { m_Projectiles.push_back( pProjectile ); }
    inline void                 RemoveProjectile( CClientProjectile * pProjectile )         { m_Projectiles.remove( pProjectile ); }
    projectileList_t::iterator  ProjectilesBegin()                                          { return m_Projectiles.begin(); }
    projectileList_t::iterator  ProjectilesEnd()                                            { return m_Projectiles.end(); }
    unsigned int                CountProjectiles( eWeaponType weaponType = WEAPONTYPE_UNARMED );

    void                        RemoveAllProjectiles();
    void                        DestroySatchelCharges( bool bBlow = true, bool bDestroy = true );
    
    CRemoteDataStorage*         GetRemoteData()                                             { return m_remoteDataStorage; }

    bool                        IsEnteringVehicle();
    bool                        IsLeavingVehicle();
    bool                        IsGettingIntoVehicle();
    bool                        IsGettingOutOfVehicle();
    bool                        IsGettingJacked();

    CClientEntity*              GetContactEntity();

    bool                        HasAkimboPointingUpwards();

    float                       GetDistanceFromCentreOfMassToBaseOfModel();

    inline unsigned char        GetAlpha()                                                  { return m_ucAlpha; }
    void                        SetAlpha( unsigned char ucAlpha );

    inline bool                 HasTargetPosition()                                         { return ( m_interp.pos.ulFinishTime != 0 ); }
    inline CClientEntity*       GetTargetOriginSource()                                     { return m_interp.pTargetOriginSource; }
    void                        GetTargetPosition( CVector& vecPosition );
    void                        SetTargetPosition( const CVector& vecPosition, unsigned long ulDelay, CClientEntity *pTargetOriginSource = NULL );
    void                        RemoveTargetPosition();
    void                        UpdateTargetPosition();

    CClientEntity*              GetTargetedEntity();    
    CClientPed*                 GetTargetedPed();

    inline CClientEntity*       GetCurrentContactEntity()                                   { return m_pCurrentContactEntity; }
    inline void                 SetCurrentContactEntity( CClientEntity *pEntity )           { m_pCurrentContactEntity = pEntity; }

    bool                        IsSunbathing();
    void                        SetSunbathing( bool bSunbathing, bool bStartStanding = true );

    bool                        LookAt( CVector vecOffset, int iTime = 1000, int iBlend = 1000, CClientEntity * pEntity = NULL );
    bool                        UseGun( CVector vecTarget, CClientEntity * pEntity );

    bool                        IsAttachToable();
    static char*                GetBodyPartName( unsigned char ucID );

    bool                        IsDoingGangDriveby();
    void                        SetDoingGangDriveby( bool bDriveby );

    bool                        IsRunningAnimation();
    void                        RunAnimation( AssocGroupId animGroup, AnimationId animID );
    void                        RunNamedAnimation( CAnimBlock *pBlock, const char *szAnimName, int iTime = -1, bool bLoop = true, bool bUpdatePosition = true, bool bInterruptable = false, bool bFreezeLastFrame = true, bool bRunInSequence = false, bool bOffsetPed = false, bool bHoldLastFrame = false );
    void                        KillAnimation();
    inline CAnimBlock*          GetAnimationBlock()                                         { return m_pAnimationBlock; }
    inline const char*          GetAnimationName() const                                    { return m_szAnimationName; }
    void                        SetAnimationProgress( const char *name, float progress );

    bool                        IsUsingGun();

    inline bool                 IsHeadless() const                                          { return m_bHeadless; }
    void                        SetHeadless( bool bHeadless );

    inline bool                 IsFrozen() const                                            { return m_bFrozen; }
    void                        SetFrozen( bool bFrozen );
    bool                        IsFrozenWaitingForGroundToLoad() const;
    void                        SetFrozenWaitingForGroundToLoad( bool bFrozen );

    bool                        IsFootBloodEnabled();
    void                        SetFootBloodEnabled( bool bHasFootBlood );

    inline bool                 IsOnFire() const                                            { return m_bIsOnFire; }
    void                        SetOnFire( bool bOnFire );

    void                        GetVoice( short* psVoiceType, short* psVoiceID );
    void                        GetVoice( const char** pszVoiceType, const char** pszVoice );
    void                        SetVoice( short sVoiceType, short sVoiceID );
    void                        SetVoice( const char* szVoiceType, const char* szVoice );

    bool                        IsSpeechEnabled();
    void                        SetSpeechEnabled( bool bEnabled );

    void                        PostWeaponFire();
    void                        SetBulletImpactData( CClientEntity* pVictim, const CVector& vecHitPosition );
    bool                        GetBulletImpactData( CClientEntity** ppVictim = 0, CVector* pvecHitPosition = 0 );
    void                        ClearBulletImpactData() { m_bBulletImpactData = false; }

    bool                        CanReloadWeapon();
    bool                        ReloadWeapon();

    bool                        ShouldBeStealthAiming();
    inline bool                 IsStealthAiming() const                                     { return m_bStealthAiming; }
    void                        SetStealthAiming( bool bAiming );

protected:
    // This constructor is for peds managed by a player. These are unknown to the ped manager.
                                CClientPed( CClientManager *pManager, unsigned short ulModelID, ElementID ID, lua_State *L, bool system, bool bIsLocalPlayer );

    void                        InstanceLua( lua_State *L, bool system );
    void                        Init( CClientManager* pManager, unsigned short ulModelID, bool bIsLocalPlayer );

    void                        Dump( FILE* pFile, bool bDumpDetails, unsigned int uiIndex );
    void                        StreamedInPulse();

    void                        Interpolate();
    void                        UpdateKeysync( bool bCleanup = false );

    // Used to destroy the current game ped and create a new one in the same state.
    void                        ReCreateModel();

    void                        _CreateModel();
    void                        _CreateLocalModel();
    void                        _DestroyModel();
    void                        _DestroyLocalModel();
    void                        _ChangeModel();

    void                        ModelRequestCallback( CModelInfo* pModelInfo );

    void                        InternalWarpIntoVehicle( CVehicle* pGameVehicle, unsigned int seat );
    void                        InternalRemoveFromVehicle( CVehicle* pGameVehicle );

    bool                        PerformChecks();
    void                        HandleWaitingForGroundToLoad();

    // Used to start and stop radio for local player
    void                        StartRadio();
    void                        StopRadio();
    bool                        m_bDontChangeRadio;

public:
    void                        _GetIntoVehicle( CClientVehicle* pVehicle, unsigned int uiSeat, unsigned char ucDoor );

    void                        Respawn( CVector * pvecPosition = NULL, bool bRestoreState = false, bool bCameraCut = false );

    void                        NotifyCreate();
    void                        NotifyDestroy();

    CClientModelRequestManager* m_pRequester;
    CPlayerPed*                 m_pPlayerPed;
    CTaskManager*               m_pTaskManager;
    CPad*                       m_pPad;
    bool                        m_bIsLocalPlayer;
    int                         m_pRespawnState;
    unsigned short              m_ulModel;  
    RwMatrix                    m_matFrozen;
    bool                        m_bRadioOn;
    unsigned char               m_ucRadioChannel;
    bool                        m_bHealthLocked;
    bool                        m_bArmorLocked;
    unsigned long               m_ulLastOnScreenTime;
    CClientVehicle*             m_pOccupiedVehicle;
    CClientVehicle*             m_pOccupyingVehicle;
    unsigned int                m_uiOccupiedVehicleSeat;
    bool                        m_bForceGettingIn;
    bool                        m_bForceGettingOut;
    CShotSyncData*              m_shotSyncData;
    CStatsData*                 m_stats;
    CControllerState*           m_currentControllerState;
    CControllerState*           m_lastControllerState;
    CRemoteDataStorage*         m_remoteDataStorage;
    unsigned long               m_ulLastTimeFired;
    unsigned long               m_ulLastTimeAimed;
    unsigned long               m_ulLastTimeBeganCrouch;
    unsigned long               m_ulLastTimeBeganStand;
    unsigned long               m_ulLastTimeMovedWhileCrouched;
    CModelInfo*                 m_pLoadedModelInfo;
    eWeaponSlot                 m_pOutOfVehicleWeaponSlot;
    float                       m_fBeginAimX;
    float                       m_fBeginAimY;
    float                       m_fTargetAimX;
    float                       m_fTargetAimY;
    unsigned long               m_ulBeginAimTime;
    unsigned long               m_ulTargetAimTime;
    bool                        m_bTargetAkimboUp;
    unsigned long               m_ulBeginRotationTime;
    unsigned long               m_ulEndRotationTime;
    float                       m_fBeginRotation;
    float                       m_fTargetRotationA;
    float                       m_fBeginCameraRotation;
    float                       m_fTargetCameraRotation;
    unsigned long               m_ulBeginTarget;
    unsigned long               m_ulEndTarget;
    CVector                     m_vecBeginSource;
    CVector                     m_vecBeginTarget;
    CVector                     m_vecBeginTargetAngle;
    CVector                     m_vecTargetSource;
    CVector                     m_vecTargetTarget;
    CVector                     m_vecTargetTargetAngle;
    CVector                     m_vecTargetInterpolateAngle;
    CClientEntity*              m_pTargetedEntity;
    std::list <SDelayedSyncData*>   m_SyncBuffer;
    bool                        m_bDucked;
    bool                        m_bWasDucked; //For knowing when to register standing up
    bool                        m_bIsChoking;
    bool                        m_bWearingGoggles;
    bool                        m_bVisible;
    bool                        m_bUsesCollision;
    float                       m_fHealth;
    float                       m_fArmor;
    bool                        m_bWorldIgnored;
    float                       m_fCurrentRotation;
    float                       m_fMoveSpeed;
    bool                        m_bCanBeKnockedOffBike;
    RwMatrix                    m_Matrix;
    CVector                     m_vecMoveSpeed;
    CVector                     m_vecTurnSpeed;
    eWeaponSlot                 m_CurrentWeaponSlot;
    eWeaponType                 m_WeaponTypes [ WEAPONSLOT_MAX ];
    bool                        m_bHasJetPack;
    CClientPlayerClothes*       m_pClothes;
    eFightingStyle              m_FightingStyle;
    eMoveAnim                   m_MoveAnim;
    projectileList_t            m_Projectiles;
    unsigned char               m_ucAlpha;
    float                       m_fTargetRotation;
    int                         m_iVehicleInOutState;
    bool                        m_bRecreatingModel;
    CClientEntity*              m_pCurrentContactEntity;
    bool                        m_bSunbathing;
    CClientPad                  m_Pad;
    bool                        m_bDestroyingSatchels;
    bool                        m_bDoingGangDriveby;
    CAnimBlock*                 m_pAnimationBlock;
    char*                       m_szAnimationName;
    bool                        m_bRequestedAnimation;
    int                         m_iTimeAnimation;
    bool                        m_bLoopAnimation;
    bool                        m_bUpdatePositionAnimation;
    bool                        m_bInterruptableAnimation;
    bool                        m_bFreezeLastFrameAnimation;
    bool                        m_bHeadless;
    bool                        m_bFrozen;
    bool                        m_bFrozenWaitingForGroundToLoad;
    float                       m_fGroundCheckTolerance;
    float                       m_fObjectsAroundTolerance;
    int                         m_iLoadAllModelsCounter;
    bool                        m_bIsOnFire;
    SLastSyncedPedData*         m_LastSyncedData;
    bool                        m_bSpeechEnabled;
    bool                        m_bStealthAiming;
    float                       m_fLighting;
    unsigned char               m_ucEnteringDoor;
    unsigned char               m_ucLeavingDoor;

    bool                        m_bBulletImpactData;
    CClientEntity*              m_pBulletImpactEntity;
    CVector                     m_vecBulletImpactHit;

    // Time dependent interpolation
    struct
    {
        struct
        {
            CVector         vecTarget;
            CVector         vecError;
            unsigned long   ulStartTime;
            unsigned long   ulFinishTime;
            float           fLastAlpha;
        } pos;

        CClientEntity*      pTargetOriginSource;
        // These variables are used to track the last known position
        // of the contact entity for if it's removed during the
        // interpolation.
        bool                bHadOriginSource;
        CVector             vecOriginSourceLastPosition;
    } m_interp;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.h
*  PURPOSE:     Header file for vehicle base entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_VEHICLE
#define __CGAMESA_VEHICLE

#include <game/CVehicle.h>

#include "Common.h"
#include "CPhysicalSA.h"
#include "CHandlingEntrySA.h"
#include "CHandlingManagerSA.h"
#include "CDoorSA.h"

class CVehicleSA;
class CPedSAInterface;

#define SIZEOF_CHELI                            2584

// 00431f80 public: static class CVehicle * __cdecl CCarCtrl::CreateCarForScript(int,class CVector,unsigned char)
#define FUNC_CCarCtrlCreateCarForScript         0x431f80 // ##SA##

//006d6a40  public: virtual void __thiscall CVehicle::SetModelIndex(unsigned int)
#define FUNC_CVehicleSetModelIndex              0x6d6a40

//006d16a0  public: void __thiscall CVehicle::SetDriver(class CPed *)
#define FUNC_CVehicleSetDriver                  0x6d16a0

//006d14d0  public: bool __thiscall CVehicle::AddPassenger(class CPed *,unsigned char)
#define FUNC_CVehicleAddPassengerWithSeat       0x6d14d0

//006d13a0  public: bool __thiscall CVehicle::AddPassenger(class CPed *)
#define FUNC_CVehicleAddPassenger               0x6d13a0

//006d1610  public: void __thiscall CVehicle::RemovePassenger(class CPed *)
#define FUNC_CVehicleRemovePassenger            0x6d1610

//006E3290  public: void __thiscall CVehicle::AddVehicleUpgrade(DWORD dwModelID)
#define FUNC_CVehicle_AddVehicleUpgrade         0x6E3290

//006DF930     ; public: void __thiscall CVehicle::RemoveVehicleUpgrade(int)
#define FUNC_CVehicle_RemoveVehicleUpgrade      0x6DF930

//006D1E80  public: bool __thiscall CVehicle::CanPedEnterCar(void)
#define FUNC_CVehicle_CanPedEnterCar            0x6D1E80

//006D5CF0  public: bool __thiscall CVehicle::CanPedLeanOut(CPed*)
#define FUNC_CVehicle_CanPedLeanOut             0x6D5CF0

//006D2460  public: void __thiscall CVehicle::ExtinguishCarFire(void)
#define FUNC_CVehicle_ExtinguishCarFire         0x6D2460

//006D2460  public: DWORD __thiscall CVehicle::GetBaseVehicleType(void)
#define FUNC_CVehicle_GetBaseVehicleType        0x411D50

//006D1DD0  public: bool __thiscall CVehicle::IsOnItsSide(void)
#define FUNC_CVehicle_IsOnItsSide               0x6D1DD0

//006D2370  public: bool __thiscall CVehicle::IsLawEnforcementVehicle(void)
#define FUNC_CVehicle_IsLawEnforcementVehicle   0x6D2370

//006D1BD0  public: bool __thiscall CVehicle::IsPassenger(CPed*)
#define FUNC_CVehicle_IsPassenger               0x6D1BD0

//006D84D0  public: bool __thiscall CVehicle.:IsSphereTouchingVehicle(float, float, float, float)
#define FUNC_CVehicle_IsSphereTouchingVehicle   0x6D84D0

//006D1D90  public: bool __thiscall CVehicle::IsUpsideDown(void)
#define FUNC_CVehicle_IsUpsideDown              0x6D1D90

//006D2BF0  public: void __thiscall CVehicle::MakeDirty(CColPoint*)
#define FUNC_CVehicle_MakeDirty                 0x6D2BF0

//0041BDD0  public: void __thiscall CVehicle::SetEngineOn(bool)
#define FUNC_CVehicle_SetEngineOn               0x41BDD0

//006F6CC0  public: void __thiscall CVehicle::RecalcOnRailDistance(void)
#define FUNC_CVehicle_RecalcOnRailDistance           0x6F6CC0

#define FUNC_CVehicle_QueryPickedUpEntityWithWinch              0x6d3cf0
#define FUNC_CVehicle_PickUpEntityWithWinch                     0x6d3cd0
#define FUNC_CVehicle_ReleasePickedUpEntityWithWinch            0x6d3cb0
#define FUNC_CVehicle_SetRopeHeightForHeli                      0x6d3d30

#define FUNC_CVehicle__SetRemapTexDictionary                    0x6D0BC0
#define FUNC_CVehicle__GetRemapIndex                            0x6D0B70
#define FUNC_CVehicle__SetRemap                                 0x6D0C00

#define FUNC_Automobile_PlaceOnRoadProperly     0x6AF420

#define VARS_CarCounts                          0x969094 // 5 values for each vehicle type

#define VAR_VehicleCount                        0x969094
#define VAR_PoliceVehicleCount                  0x969098

#define MAX_PASSENGERS                          8

#define NUM_RAILTRACKS                          4
#define ARRAY_NumRailTrackNodes                 0xC38014    // NUM_RAILTRACKS dwords
#define ARRAY_RailTrackNodePointers             0xC38024    // NUM_RAILTRACKS pointers to arrays of SRailNode

#define VAR_CVehicle_SpecialColModels           0xc1cc78

#define HANDLING_NOS_Flag                       0x00080000
#define	HANDLING_Hydraulics_Flag                0x00020000

typedef struct
{
    short sX;               // x coordinate times 8
    short sY;               // y coordinate times 8
    short sZ;               // z coordinate times 8
    WORD  sRailDistance;    // on-rail distance times 3
    WORD  padding;
} SRailNodeSA;

struct CTrainFlags
{
    unsigned char unknown1 : 3;
    unsigned char bIsTheChainEngine : 1; // Only the first created train on the chain gets this set to true, others get it set to false.
    unsigned char unknown2 : 1; // This is always set to true in mission trains construction.
    unsigned char bIsAtNode : 1;
    unsigned char bDirection : 1;
    unsigned char unknown3 : 1; // If the mission train was placed at the node, this is set to false in construction.

    unsigned char bIsDerailed : 1;
    unsigned char unknown4 : 1 ;
    unsigned char bIsDrivenByBrownSteak : 1;
    unsigned char unknown5 : 5;

    unsigned char unknown6 : 8;

    unsigned char unknown7 : 8;
};

class CVehicleAudioSAInterface
{
public:
                            CVehicleAudioSAInterface();
                            ~CVehicleAudioSAInterface();

    void*                   m_vtbl;         // 0

    void*                   m_unk2;         // 4
    BYTE                    m_pad[8];       // 8

    void*                   m_unk;          // 16

    BYTE                    m_pad2[114];    // 20
    unsigned char           m_soundType;    // 134

    BYTE                    m_pad2b[19];    // 135
    unsigned char           m_station;      // 154
    bool                    m_improved;     // 155

    BYTE                    m_pad3[8];      // 156
    unsigned char           m_unk3;         // 164

    BYTE                    m_pad4[223];    // 165
    void*                   m_unk5;         // 388

    BYTE                    m_pad5[12];     // 392
    void*                   m_unk4;         // 404

    BYTE                    m_pad6[116];    // 408
    unsigned short          m_unk6;         // 524

    BYTE                    m_pad7[22];     // 526
    void*                   m_unk7;         // 548
    void*                   m_unk8;         // 552
};

class CVehicleControlSAInterface
{
public:
                            CVehicleControlSAInterface();
                            ~CVehicleControlSAInterface();

    short                   m_unk;              // 0
    BYTE                    m_pad[2];           // 2
    short                   m_unk2;             // 4
    BYTE                    m_pad2[2];          // 6
    short                   m_unk3;             // 8

    BYTE                    m_pad3[2];          // 10
    void*                   m_unk10;            // 12
    unsigned int            m_unk11;            // 16

    short                   m_unk4;             // 20
    short                   m_unk5;             // 22
    short                   m_unk6;             // 24

    BYTE                    m_pad6[2];          // 26
    unsigned int            m_creationTime;     // 28
    unsigned int            m_time;             // 32

    unsigned char           m_unk22;            // 36

    bool                    m_unk8;             // 37
    bool                    m_unk9;             // 38

    unsigned char           m_unk39;            // 39
    unsigned char           m_unk12;            // 40
    unsigned char           m_unk13;            // 41
    unsigned char           m_unk14;            // 42
    unsigned char           m_unk15;            // 43

    BYTE                    m_pad13[4];         // 44
    void*                   m_unk29;            // 48
    unsigned char           m_unk30;            // 52

    unsigned char           m_unk28;            // 53
    
    BYTE                    m_pad8[2];          // 54
    float                   m_unk16;            // 56

    void*                   m_unk38;            // 60

    unsigned char           m_unk17;            // 64

    unsigned char           m_unk23;            // 65
    BYTE                    m_pad11[2];         // 66

    float                   m_unk24;            // 68

    unsigned char           m_unk31;            // 72
    
    unsigned char           m_unk32;            // 73
    unsigned char           m_unk33;            // 74

    unsigned char           m_flags;            // 75
    unsigned char           m_flags2;           // 76

    unsigned char           m_unk34;            // 77
    
    unsigned char           m_unk35;            // 78
    unsigned char           m_unk36;            // 79
    unsigned char           m_unk37;            // 80

    BYTE                    m_pad14[11];        // 81
    unsigned int            m_handlingFlags;    // 92

    BYTE                    m_pad4[8];          // 96
    unsigned int            m_unk7[8];          // 104

    unsigned short          m_unk18;            // 136
    BYTE                    m_pad10[2];         // 138
    void*                   m_unk19;            // 140

    void*                   m_unk25;            // 144
    
    unsigned char           m_unk26;            // 148
    unsigned char           m_unk27;            // 149

    BYTE                    m_padlast[2];       // 150
};

#define MAX_UPGRADES_ATTACHED       14

#define VEHICLE_POLICE              0x00000001
#define VEHICLE_AMBULANCE           0x00000002
#define VEHICLE_FIRETRUCK           0x00000004
#define VEHICLE_LOCKED              0x00000008
#define VEHICLE_ENGINESTATUS        0x00000010
#define VEHICLE_HANDBRAKE           0x00000020
#define VEHICLE_LIGHTSTATUS         0x00000040
#define VEHICLE_FREEBIES            0x00000080

#define VEHICLE_VAN                 0x00000100
#define VEHICLE_BUS                 0x00000200
#define VEHICLE_BIG                 0x00000400
#define VEHICLE_LOW                 0x00000800
#define VEHICLE_COMEDYCONTROLS      0x00001000
#define VEHICLE_WARNPEDS            0x00002000
#define VEHICLE_CRANEPROCESSED      0x00004000
#define VEHICLE_ARMORED             0x00008000

#define VEHICLE_DAMAGED             0x00010000
#define VEHICLE_PLAYERPOSSESION     0x00020000
#define VEHICLE_FADEOUT             0x00040000
#define VEHICLE_CARJACK             0x00080000
#define VEHICLE_ROADBLOCK           0x00100000
#define VEHICLE_DAMAGEABLE          0x00200000
#define VEHICLE_PROCESSEDPEDS       0x00400000
#define VEHICLE_WEAPONS             0x00800000
    
#define VEHICLE_COLLPROCESSED       0x01000000
#define VEHICLE_PARKED              0x02000000
#define VEHICLE_RECORDED            0x04000000
#define VEHICLE_CONVOY              0x08000000
#define VEHICLE_LOWTILT             0x10000000
#define VEHICLE_CHANGEGEAR          0x20000000
#define VEHICLE_DROWNING            0x40000000
#define VEHICLE_NOTYREBURST         0x80000000

#define VEHGENERIC_POLICETYPE       0x00000001
#define VEHGENERIC_NOSTATIC         0x00000002
#define VEHGENERIC_PARKING          0x00000004
#define VEHGENERIC_CANPARK          0x00000008
#define VEHGENERIC_FIREGUN          0x00000010
#define VEHGENERIC_DRIVER           0x00000020
#define VEHGENERIC_COMPLEX          0x00000040
#define VEHGENERIC_REMOTECONTROL    0x00000080

#define VEHGENERIC_SKIDMARKS        0x00000100
#define VEHGENERIC_ENGINEBROKEN     0x00000200
#define VEHGENERIC_TARGETABLE       0x00000400
#define VEHGENERIC_GANGWAR          0x00000800
#define VEHGENERIC_PICKUP           0x00001000
#define VEHGENERIC_IMPOUNDED        0x00002000
#define VEHGENERIC_HEATSEEK         0x00004000
#define VEHGENERIC_ALARM            0x00008000

#define VEHGENERIC_GANGLEANON       0x00010000
#define VEHGENERIC_GANGROADBLOCK    0x00020000
#define VEHGENERIC_ISGOODCOVER      0x00040000
#define VEHGENERIC_MADDRIVER        0x00080000
#define VEHGENERIC_UPGRADEDSTEREO   0x00100000
#define VEHGENERIC_PEDENTERING      0x00200000
#define VEHGENERIC_PETROLTANK       0x00400000
#define VEHGENERIC_NOPARTICLES      0x00800000

#define VEHGENERIC_RESPRAYED        0x01000000
#define VEHGENERIC_CHEATS           0x02000000
#define VEHGENERIC_NOCOLORREMAP     0x04000000
#define VEHGENERIC_REPLAY           0x08000000

struct CVehicleFlags
{
    unsigned char bIsLawEnforcer: 1; // Is this guy chasing the player at the moment
    unsigned char bIsAmbulanceOnDuty: 1; // Ambulance trying to get to an accident
    unsigned char bIsFireTruckOnDuty: 1; // Firetruck trying to get to a fire
    unsigned char bIsLocked: 1; // Is this guy locked by the script (cannot be removed)
    unsigned char bEngineOn: 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
    unsigned char bIsHandbrakeOn: 1; // How's the handbrake doing ?
    unsigned char bLightsOn: 1; // Are the lights switched on ?
    unsigned char bFreebies: 1; // Any freebies left in this vehicle ?

    unsigned char bIsVan: 1; // Is this vehicle a van (doors at back of vehicle)
    unsigned char bIsBus: 1; // Is this vehicle a bus
    unsigned char bIsBig: 1; // Is this vehicle a bus
    unsigned char bLowVehicle: 1; // Need this for sporty type cars to use low getting-in/out anims
    unsigned char bComedyControls: 1; // Will make the car hard to control (hopefully in a funny way)
    unsigned char bWarnedPeds: 1; // Has scan and warn peds of danger been processed?
    unsigned char bCraneMessageDone: 1; // A crane message has been printed for this car allready
    // unsigned char bExtendedRange: 1; // This vehicle needs to be a bit further away to get deleted
    unsigned char bTakeLessDamage: 1; // This vehicle is stronger (takes about 1/4 of damage)

    unsigned char bIsDamaged: 1; // This vehicle has been damaged and is displaying all its components
    unsigned char bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
    unsigned char bFadeOut: 1; // Fade vehicle out
    unsigned char bIsBeingCarJacked: 1; // Fade vehicle out
    unsigned char bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
    unsigned char bCanBeDamaged: 1; // Set to FALSE during cut scenes to avoid explosions
    // unsigned char bUsingSpecialColModel : 1;
    // Is player vehicle using special collision model, stored in player strucure
    unsigned char bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
    unsigned char bGunSwitchedOff: 1; // Level designers can use this to switch off guns on boats
    
    unsigned char bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
    unsigned char bIsCarParkVehicle: 1; // Car has been created using the special CAR_PARK script command
    unsigned char bHasAlreadyBeenRecorded : 1; // Used for replays
    unsigned char bPartOfConvoy: 1;
    unsigned char bHeliMinimumTilt: 1; // This heli should have almost no tilt really
    unsigned char bAudioChangingGear: 1; // sounds like vehicle is changing gear
    unsigned char bIsDrowning: 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
    unsigned char bTyresDontBurst: 1; // If this is set the tyres are invincible

    unsigned char bCreatedAsPoliceVehicle : 1;// True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
    unsigned char bRestingOnPhysical: 1; // Dont go static cause car is sitting on a physical object that might get removed
        unsigned char      bParking                    : 1;
        unsigned char      bCanPark                    : 1;
    unsigned char bFireGun: 1; // Does the ai of this vehicle want to fire it's gun?
    unsigned char bDriverLastFrame: 1; // Was there a driver present last frame ?
    unsigned char bNeverUseSmallerRemovalRange: 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
    unsigned char bIsRCVehicle: 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

    unsigned char bAlwaysSkidMarks: 1; // This vehicle leaves skidmarks regardless of the wheels' states.
    unsigned char bEngineBroken: 1; // Engine doesn't work. Player can get in but the vehicle won't drive
    unsigned char bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
    unsigned char bPartOfAttackWave: 1; // This car is used in an attack during a gang war
    unsigned char bWinchCanPickMeUp: 1; // This car cannot be picked up by any ropes.
    unsigned char bImpounded: 1; // Has this vehicle been in a police impounding garage
    unsigned char bVehicleCanBeTargettedByHS  : 1;// Heat seeking missiles will not target this vehicle.
    unsigned char bSirenOrAlarm: 1; // Set to TRUE if siren or alarm active, else FALSE

    unsigned char bHasGangLeaningOn: 1;
    unsigned char bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
    unsigned char bDoesProvideCover: 1; // If this is false this particular vehicle can not be used to take cover behind.
    unsigned char bMadDriver: 1; // This vehicle is driving like a lunatic
    unsigned char bUpgradedStereo: 1; // This vehicle has an upgraded stereo
    unsigned char bConsideredByPlayer: 1; // This vehicle is considered by the player to enter
    unsigned char bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
    unsigned char bDisableParticles: 1; // Disable particles from this car. Used in garage.

    unsigned char bHasBeenResprayed: 1; // Has been resprayed in a respray garage. Reset after it has been checked.
    unsigned char bUseCarCheats: 1; // If this is true will set the car cheat stuff up in ProcessControl()
    unsigned char bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
    unsigned char bUsedForReplay: 1; // This car is controlled by replay and should be removed when replay is done.
};

class CVehicleSAInterface : public CPhysicalSAInterface
{
public:
                                    CVehicleSAInterface( unsigned char createdBy );
                                    ~CVehicleSAInterface();

    inline CVehicleModelInfoSAInterface*    GetModelInfo()                              { return (CVehicleModelInfoSAInterface*)CEntitySAInterface::GetModelInfo(); }
    inline const CVehicleModelInfoSAInterface*  GetModelInfo() const                    { return (const CVehicleModelInfoSAInterface*)CEntitySAInterface::GetModelInfo(); }

    void                            HandlePopulation( bool create );

    virtual void __thiscall         ProcessControlCollisionCheck        () = 0;
    virtual void __thiscall         ProcessControlInputs                () = 0;
    virtual void __thiscall         GetComponentWorldPosition           () const = 0;
    virtual bool __thiscall         IsComponentPresent                  () const = 0;
    virtual void __thiscall         OpenDoor                            ( bool closed, unsigned int node, unsigned int door, float ratio, bool makeNoise ) = 0;
    virtual void __thiscall         ProcessDoorOpen                     () = 0;
    virtual float __thiscall        GetDoorAngleOpenRatio               ( unsigned int id ) const = 0;
    virtual float __thiscall        GetDoorAngleOpenRatioInternal       ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorReady                         ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorReadyInternal                 ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorFullyOpen                     ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorFullyOpenInternal             ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorClosed                        ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorClosedInternal                ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorMissing                       ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsDoorMissingInternal               ( unsigned int id ) const = 0;
    virtual bool __thiscall         IsRoofPresent                       () const = 0;
    virtual void __thiscall         RemoveVehicleReferences             () = 0;
    virtual void __thiscall         Blow                                ( CEntitySAInterface *cause, unsigned long unk ) = 0;
    virtual void __thiscall         BlowWithCutscene                    ( unsigned long unk, unsigned long unk2, unsigned long unk3, unsigned long unk4 ) = 0;
    virtual bool __thiscall         InitWheels                          () = 0;
    virtual bool __thiscall         BurstTyre                           ( unsigned char tyre, unsigned int unk ) = 0;
    virtual bool __thiscall         CanPedLeaveCar                      () const = 0;
    virtual void __thiscall         ProcessDrivingAnims                 () = 0;
    virtual void* __thiscall        GetRideAnimData                     () = 0;
    virtual void __thiscall         SetupSuspensionLines                () = 0;
    virtual void __thiscall         GetMovingCollisionSpeed             ( CVector& vec ) const = 0;
    virtual void __thiscall         Fix                                 () = 0;
    virtual void __thiscall         SetupDamageAfterLoad                () = 0;
    virtual void __thiscall         DoBurstAndSoftGroundRatios          () = 0;
    virtual float __thiscall        GetModelOffset                      () const = 0;
    virtual void __thiscall         PlayHorn                            () = 0;
    virtual unsigned int __thiscall GetNumContactWheels                 () const = 0;
    virtual void __thiscall         Damage                              () = 0;
    virtual bool __thiscall         CanPedStepOut                       ( bool unk ) const = 0;
    virtual bool __thiscall         CanPedJumpOut                       ( CPedSAInterface *passenger ) const = 0;
    virtual bool __thiscall         GetTowHitchPosition                 ( CVector& pos, unsigned int unk, unsigned int unk2 ) const = 0;
    virtual bool __thiscall         GetTowbarPosition                   ( CVector& pos, unsigned int unk, unsigned int unk2 ) const = 0;
    virtual bool __thiscall         SetTowLink                          ( CVehicleSAInterface *towVehicle, unsigned int unk ) = 0;
    virtual bool __thiscall         BreakTowLink                        () = 0;
    virtual float __thiscall        GetWheelWidth                       () const = 0;
    virtual void __thiscall         Save                                () = 0;
    virtual void __thiscall         Load                                () = 0;

    void*   operator new( size_t );
    void    operator delete( void *ptr );

    RpClump*                    GetRwObject()                               { return (RpClump*)CEntitySAInterface::GetRwObject(); }
    const RpClump*              GetRwObject() const                         { return (const RpClump*)CEntitySAInterface::GetRwObject(); }

    void __thiscall             RenderPassengers                ( void );
    void __thiscall             CreateLicensePlate              ( CVehicleModelInfoSAInterface *info );
    void __thiscall             SetupRender                     ( CVehicleSA *mtaVeh );
    void __thiscall             SetPlateTextureForRendering     ( CVehicleModelInfoSAInterface *info );
    void __thiscall             RestoreLicensePlate             ( CVehicleModelInfoSAInterface *info );
    void __thiscall             LeaveRender                     ( void );

    inline bool                 IsLocked() const                            { return IS_FLAG( m_vehicleFlags, VEHICLE_LOCKED ); }
    inline bool                 IsDamaged() const                           { return IS_FLAG( m_vehicleFlags, VEHICLE_DAMAGED ); }
    inline bool                 IsDrowning() const                          { return IS_FLAG( m_vehicleFlags, VEHICLE_DROWNING ); }
    inline bool                 IsEngineOn() const                          { return IS_FLAG( m_vehicleFlags, VEHICLE_ENGINESTATUS ); }
    inline bool                 IsHandbrakeOn() const                       { return IS_FLAG( m_vehicleFlags, VEHICLE_HANDBRAKE ); }
    inline bool                 CanBeDamaged() const                        { return IS_FLAG( m_vehicleFlags, VEHICLE_DAMAGEABLE ); }
    inline bool                 HasComedyControls() const                   { return IS_FLAG( m_vehicleFlags, VEHICLE_COMEDYCONTROLS ); }
    inline bool                 AreWeaponsDisabled() const                  { return IS_FLAG( m_vehicleFlags, VEHICLE_WEAPONS ); }
    inline bool                 AreLightsOn() const                         { return IS_FLAG( m_vehicleFlags, VEHICLE_LIGHTSTATUS ); }
    inline bool                 IsArmored() const                           { return IS_FLAG( m_vehicleFlags, VEHICLE_ARMORED ); }
    inline bool                 AreTyresBurst() const                       { return IS_FLAG( m_vehicleFlags, VEHICLE_NOTYREBURST ); }
    inline bool                 IsFadingOut() const                         { return IS_FLAG( m_vehicleFlags, VEHICLE_FADEOUT ); }
    inline bool                 IsEngineBroken() const                      { return IS_FLAG( m_genericFlags, VEHGENERIC_ENGINEBROKEN ); }
    inline bool                 IsRCVehicle() const                         { return IS_FLAG( m_genericFlags, VEHGENERIC_REMOTECONTROL ); }
    inline bool                 IsLeavingSkidMarks() const                  { return IS_FLAG( m_genericFlags, VEHGENERIC_SKIDMARKS ); }
    inline bool                 IsHeatTraceable() const                     { return IS_FLAG( m_genericFlags, VEHGENERIC_HEATSEEK ); }
    inline bool                 HasPetrolTank() const                       { return IS_FLAG( m_genericFlags, VEHGENERIC_PETROLTANK ); }
    inline bool                 IsColourRemapping() const                   { return IS_FLAG( m_genericFlags, VEHGENERIC_NOCOLORREMAP ); }
    inline bool                 IsHornActive() const                        { return IS_FLAG( m_genericFlags, VEHGENERIC_ALARM ); }

    inline void                 SetLocked( bool locked )                    { BOOL_FLAG( m_vehicleFlags, VEHICLE_LOCKED, locked ); }
    inline void                 EnableDamage( bool enable )                 { BOOL_FLAG( m_vehicleFlags, VEHICLE_DAMAGEABLE, enable ); }
    inline void                 SetComedyControls( bool enable )            { BOOL_FLAG( m_vehicleFlags, VEHICLE_COMEDYCONTROLS, enable ); }
    inline void                 SetEngineOn( bool enable )                  { BOOL_FLAG( m_vehicleFlags, VEHICLE_ENGINESTATUS, enable ); }
    inline void                 DisableGuns( bool disable )                 { BOOL_FLAG( m_vehicleFlags, VEHICLE_WEAPONS, disable ); }
    inline void                 SetHandbrakeOn( bool enable )               { BOOL_FLAG( m_vehicleFlags, VEHICLE_HANDBRAKE, enable ); }
    inline void                 SetLightsOn( bool enable )                  { BOOL_FLAG( m_vehicleFlags, VEHICLE_LIGHTSTATUS, enable ); }
    inline void                 SetArmored( bool enable )                   { BOOL_FLAG( m_vehicleFlags, VEHICLE_ARMORED, enable ); }
    inline void                 SetSteelTyres( bool enable )                { BOOL_FLAG( m_vehicleFlags, VEHICLE_NOTYREBURST, enable ); }
    inline void                 SetEngineBroken( bool broken )              { BOOL_FLAG( m_genericFlags, VEHGENERIC_ENGINEBROKEN, broken ); }
    inline void                 ActivateSkidMarks( bool enable )            { BOOL_FLAG( m_genericFlags, VEHGENERIC_SKIDMARKS, enable ); }
    inline void                 SetHeatTraceable( bool enable )             { BOOL_FLAG( m_genericFlags, VEHGENERIC_HEATSEEK, enable ); }
    inline void                 PutPetrolTank( bool enable )                { BOOL_FLAG( m_genericFlags, VEHGENERIC_PETROLTANK, enable ); }
    inline void                 SetColourRemapping( bool enable )           { BOOL_FLAG( m_genericFlags, VEHGENERIC_NOCOLORREMAP, enable ); }
    inline void                 SetHornActive( bool enable )                { BOOL_FLAG( m_genericFlags, VEHGENERIC_ALARM, enable ); }
    inline void                 SetFadingOut( bool enable )
    {
        //BOOL_FLAG( m_entityFlags, ENTITY_FADE, enable );      // this flag is for objects only, mainly
        BOOL_FLAG( m_vehicleFlags, VEHICLE_FADEOUT, enable );
    }

    CVehicleAudioSAInterface    m_audio;                                // 312

    BYTE                        m_pad0[32];                             // 868

    tHandlingDataSA*            pHandlingData;                          // 900
    BYTE                        m_pad[4];                               // 904
    unsigned int                m_handlingFlags;                        // 908
    
    CVehicleControlSAInterface  m_control;                              // 912

    union
    {
        CVehicleFlags           m_nVehicleFlags;                                    // +1064
        
        struct
        {
            unsigned int        m_vehicleFlags;                                     // +1064
            unsigned int        m_genericFlags;                                     // +1068
        };
    };

    unsigned int                m_timeOfCreation;                       // 1072, GetTimeInMilliseconds when this vehicle was created.

    unsigned char               m_colour1, m_colour2, m_colour3, m_colour4; // 1076
    char                        m_comp1, m_comp2;                       // 1080
    short                       m_upgrades[MAX_UPGRADES_ATTACHED];      // 1082
    unsigned short              m_numUpgrades;                          // 1110, No idea whether this is true
    float                       m_wheelScale;                           // 1112

    unsigned short              m_alarmState;                           // 1116
    unsigned short              m_forcedRandomSeed;                     // 1118, if this is non-zero the random wander gets deterministic
    
    CPedSAInterface*            pDriver;                                // 1120
    CPedSAInterface*            pPassengers[MAX_PASSENGERS];            // 1124
    unsigned char               m_numPassengers;                        // 1156
    unsigned char               m_numGettingIn;                         // 1157
    unsigned char               m_gettingInFlags;                       // 1158
    unsigned char               m_gettingOutFlags;                      // 1159
    unsigned char               m_maxPassengers;                        // 1160
    unsigned char               m_windowsOpenFlags;                     // 1161
    unsigned char               m_nitroBoosts;                          // 1162

    unsigned char               m_nSpecialColModel;                     // 1163
    CEntitySAInterface*         m_entityVisibilityCheck;                // 1164
    CFireSAInterface*           m_fire;                                 // 1168

    float                       m_steerAngle;                           // 1172
    float                       m_secondarySteerAngle;                  // 1176, used for steering 2nd set of wheels or elevators etc..
    float                       m_gasPedal;                             // 1180, 0...1
    float                       m_brakePedal;                           // 1184, 0...1

    unsigned char               m_createdBy;                            // 1188, Contains information on whether this vehicle should be deleted 
    unsigned short              m_extendedRemovalRange;                 // 1190

    unsigned char               m_bombOnBoard : 3;                      // 1192, 0 = None. 1 = Timed. 2 = On ignition, 3 = remotely set ? 4 = Timed Bomb has been activated. 5 = On ignition has been activated.
    unsigned char               m_overrideLights : 2;                   // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
    unsigned char               m_winchType : 3;                        // Does this vehicle use a winch?

    unsigned char               m_gunsCycleIndex : 2;                   // 1193, Cycle through alternate gun hardpoints on planes/helis
    unsigned char               m_ordnanceCycleIndex : 6;               // Cycle through alternate ordnance hardpoints on planes/helis

    unsigned char               m_usedForCover;                         // 1194, Has n number of cops hiding/attempting to hid behind it
    unsigned char               m_ammoInClip;                           // 1195, Used to make the guns on boat do a reload.
    unsigned char               m_pacMansCollected;                     // 1196
    unsigned char               m_pedsPositionForRoadBlock;             // 1197
    unsigned char               m_numPedsForRoadBlock;                  // 1198

    BYTE                        m_pad4[1];                              // 1199
    float                       m_bodyDirtLevel;                        // 1200, Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible, it may be altered at any time while vehicle's cycle of lige

    unsigned char               m_currentGear;                          // 1204, values used by transmission
    BYTE                        m_pad4b[3];                             // 1205
    float                       m_gearChangeCount;                      // 1208
    float                       m_wheelSpinAudio;                       // 1212

    float                       m_health;                               // 1216, 1000 = full health. 0 -> explode

    CVehicleSAInterface*        m_towedByVehicle;                       // 1220
    CVehicleSAInterface*        m_towedVehicle;                         // 1224
    void*                       m_unk13;                                // 1228
    void*                       m_unk14;                                // 1232

    void*                       m_unk7;                                 // 1236

    unsigned long               m_explodeTime;                          // 1240
    unsigned short              m_unk6;                                 // 1244

    unsigned short              m_unk3;                                 // 1246
    void*                       m_unk4;                                 // 1248

    void*                       m_unk21;                                // 1252
    void*                       m_unk20;                                // 1256

    unsigned char               m_unk10;                                // 1260

    BYTE                        m_pad5c[7];                             // 1261

    unsigned long               m_creationTime;                         // 1268

    unsigned int                m_doorState;                            // 1272

    unsigned int                m_unk15;                                // 1276
    unsigned int                m_unk16;                                // 1280
    unsigned int                m_unk17;                                // 1284

    char                        m_unk2;                                 // 1288
    BYTE                        m_pad6[3];                              // 1289

    unsigned int                m_unk18;                                // 1292

    unsigned char               m_unk19;                                // 1296
    unsigned char               m_unk22;                                // 1297
    unsigned char               m_unk23;                                // 1298
    unsigned char               m_unk24;                                // 1299

    unsigned int                m_hornActive;                           // 1300
    unsigned char               m_unk8;                                 // 1304
    unsigned char               m_unk9;                                 // 1305

    unsigned char               m_unk25;                                // 1306

    unsigned char               m_unk36;                                // 1307

    BYTE                        m_pad19[36];                            // 1308
    unsigned char               m_unk26;                                // 1344

    BYTE                        m_pad20[43];                            // 1345
    unsigned char               m_unk27;                                // 1388
    
    BYTE                        m_pad18[7];                             // 1389
    float                       m_unk37;                                // 1396

    float                       m_unk31;                                // 1400
    float                       m_unk32;                                // 1404
    float                       m_unk33;                                // 1408

    unsigned int                m_lightFlags;                           // 1412

    RwTexture*                  m_pCustomPlateTexture;                  // 1416

    float                       m_unk;                                  // 1420

    eVehicleType                m_vehicleType;                          // 1424
    unsigned int                m_unk39;                                // 1428

    short                       m_paintjobTxd;                          // 1432
    unsigned short              m_queuePaintjob;                        // 1434
    RwTexture*                  m_paintjobTexture;                      // 1436
};

#include "CVehicleSA.render.h"
#include "CVehicleSA.customplate.h"

class CVehicleSA : public virtual CVehicle, public CPhysicalSA
{
    friend class CPoolsSA;
    friend class CVehicleComponentSA;
public:
                                CVehicleSA( CVehicleSAInterface *vehicle );
                                ~CVehicleSA();

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    inline CVehicleSAInterface* GetInterface()                                          { return (CVehicleSAInterface*)m_pInterface; }
    inline const CVehicleSAInterface*   GetInterface() const                            { return (const CVehicleSAInterface*)m_pInterface; }

    unsigned int                GetPoolIndex() const                                    { return m_poolIndex; }

    void                        Init();

    float                       GetHealth() const                                       { return GetInterface()->m_health; }
    void                        SetHealth( float fHealth );

    unsigned char               GetNumberGettingIn() const                              { return GetInterface()->m_numGettingIn; }
    unsigned char               GetPassengerCount() const                               { return GetInterface()->m_numPassengers; }
    unsigned char               GetMaxPassengerCount() const                            { return GetInterface()->m_maxPassengers; }

    virtual CDoorSA*            GetDoor( unsigned char ucDoor )                         { return NULL; }
    virtual void                OpenDoor( unsigned char ucDoor, float fRatio, bool bMakeNoise = false );
    bool                        AreDoorsLocked() const;
    void                        LockDoors( bool bLocked );
    bool                        AreDoorsUndamageable() const                            { return GetInterface()->m_doorState == 1 || GetInterface()->m_doorState == 7; }
    void                        SetDoorsUndamageable( bool bUndamageable );

    inline void                 SetAlpha( unsigned char alpha )                         { m_alpha = alpha; }
    inline unsigned char        GetAlpha() const                                        { return m_alpha; }
    void                        SetColor( SColor color1, SColor color2, SColor color3, SColor color4, int );
    void                        GetColor( SColor& color1, SColor& color2, SColor& color3, SColor& color4, int ) const;
    void                        GetColor( SColor* color1, SColor* color2, SColor* color3, SColor* color4, int ) const;

    CModel*                     CloneClump() const;
    CVehicleComponent*          GetComponent( const char *name );
    void                        GetComponentNameList( std::vector <std::string>& list );

    // Virtually internal shared functions
    void                        BurstTyre( unsigned char tyre )                         { GetInterface()->BurstTyre( tyre, 1 ); }
    bool                        CanPedEnterCar() const;
    bool                        CanPedJumpOutCar( CPed *ped ) const                     { return GetInterface()->CanPedJumpOut( dynamic_cast <CPedSA*> ( ped )->GetInterface() ); }
    bool                        CanPedLeanOut( CPed *ped ) const;
    bool                        CanPedStepOutCar( bool unk ) const                      { return GetInterface()->CanPedStepOut( unk ); }
    bool                        CarHasRoof() const                                      { return GetInterface()->IsRoofPresent(); }
    bool                        GetTowBarPos( CVector& pos ) const                      { return GetInterface()->GetTowbarPosition( pos, 1, 0 ); }
    bool                        GetTowHitchPos( CVector& pos ) const                    { return GetInterface()->GetTowHitchPosition( pos, 1, 0 ); }
    bool                        SetTowLink( CVehicle *vehicle )                         { return GetInterface()->SetTowLink( dynamic_cast <CVehicleSA*> ( vehicle )->GetInterface(), 1 ); }
    bool                        BreakTowLink()                                          { return GetInterface()->BreakTowLink(); }
    float                       GetHeightAboveRoad() const                              { return GetInterface()->GetModelOffset(); }
    void                        Fix()                                                   { GetInterface()->Fix(); }
    void                        BlowUp( CEntity *creator, unsigned long unk );
    void                        BlowUpCutSceneNoExtras( unsigned int unk1, unsigned int unk2, unsigned int unk3, unsigned int unk4 )    { GetInterface()->BlowWithCutscene( unk1, unk2, unk3, unk4 ); }
    virtual void                RecalculateSuspensionLines()                            { GetInterface()->SetupSuspensionLines(); }

    CVehicle*                   GetTowedVehicle() const;
    CVehicle*                   GetTowedByVehicle() const;

    void                        SetExplodeTime( unsigned long ulTime )                  { GetInterface()->m_explodeTime = ulTime; }
    unsigned long               GetExplodeTime() const                                  { return GetInterface()->m_explodeTime; }

    bool                        IsOnItsSide() const;
    bool                        IsLawEnforcementVehicle() const;

    void                        SetGasPedal( float percentage )                         { GetInterface()->m_gasPedal = percentage; }
    void                        SetBrakePedal( float percentage )                       { GetInterface()->m_brakePedal = percentage; }
    void                        SetSteerAngle( float rad )                              { GetInterface()->m_steerAngle = rad; }
    void                        SetSecSteerAngle( float rad )                           { GetInterface()->m_secondarySteerAngle = rad; }

    unsigned char               GetCurrentGear() const                                  { return GetInterface()->m_currentGear; }
    float                       GetGasPedal() const                                     { return GetInterface()->m_gasPedal; }
    float                       GetBrakePedal() const                                   { return GetInterface()->m_brakePedal; }
    float                       GetSteerAngle() const                                   { return GetInterface()->m_steerAngle; }
    float                       GetSecSteerAngle() const                                { return GetInterface()->m_secondarySteerAngle; }

    bool                        AddProjectile( eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target );
    void                        AddVehicleUpgrade( unsigned short model );
    void                        RemoveVehicleUpgrade( unsigned short model );

    unsigned int                GetBaseVehicleType() const                              { return (unsigned int)GetInterface()->m_vehicleType; }

    void                        SetBodyDirtLevel( float fDirtLevel )                    { GetInterface()->m_bodyDirtLevel = fDirtLevel; }
    float                       GetBodyDirtLevel() const                                { return GetInterface()->m_bodyDirtLevel; }

    CPed*                       GetDriver() const;
    CPed*                       GetPassenger( unsigned char ucSlot ) const;
    bool                        IsBeingDriven() const                                   { return GetInterface()->pDriver != NULL; }
    bool                        IsPassenger( CPed *ped ) const;
    bool                        IsSphereTouchingVehicle( const CVector& pos, float fRadius ) const;
    bool                        IsUpsideDown() const                                    { return GetInterface()->Placeable.matrix->vUp.fZ <= -0.9; }
    void                        MakeDirty( CColPoint *point );

    virtual void                PlaceOnRoadProperly()                                   {}

    void                        SetRemap( int iRemap );
    int                         GetRemapIndex() const;
    void                        SetRemapTexDictionary( int iRemapTextureDictionary );

    bool                        IsEngineBroken() const                                  { return GetInterface()->IsEngineBroken(); }
    bool                        IsScriptLocked() const                                  { return GetInterface()->IsLocked(); }
    bool                        IsDamaged() const                                       { return GetInterface()->IsDamaged(); }
    bool                        IsDrowning() const                                      { return GetInterface()->IsDrowning(); }
    bool                        IsEngineOn() const                                      { return GetInterface()->IsEngineOn(); }
    bool                        IsHandbrakeOn() const                                   { return GetInterface()->IsHandbrakeOn(); }
    bool                        IsRCVehicle() const                                     { return GetInterface()->IsRCVehicle(); }
    bool                        GetAlwaysLeaveSkidMarks() const                         { return GetInterface()->IsLeavingSkidMarks(); }
    bool                        GetCanBeDamaged() const                                 { return GetInterface()->CanBeDamaged(); }
    bool                        GetCanBeTargettedByHeatSeekingMissiles() const          { return GetInterface()->IsHeatTraceable(); }
    bool                        GetCanShootPetrolTank() const                           { return GetInterface()->HasPetrolTank(); }
    bool                        GetChangeColourWhenRemapping() const                    { return GetInterface()->IsColourRemapping(); }
    bool                        GetComedyControls() const                               { return GetInterface()->HasComedyControls(); }
    bool                        GetGunSwitchedOff() const                               { return GetInterface()->AreWeaponsDisabled(); }
    bool                        GetLightsOn() const                                     { return GetInterface()->AreLightsOn(); }
    bool                        GetTakeLessDamage() const                               { return GetInterface()->IsArmored(); }
    bool                        GetTyresDontBurst() const                               { return GetInterface()->AreTyresBurst(); }
    bool                        IsSirenOrAlarmActive() const                            { return GetInterface()->IsHornActive(); }
    bool                        IsFadingOut() const                                     { return GetInterface()->IsFadingOut(); }
    unsigned int                GetOverrideLights() const                               { return GetInterface()->m_overrideLights; }
    
    void                        SetEngineBroken( bool bEngineBroken )                   { GetInterface()->SetEngineBroken( bEngineBroken ); }
    void                        SetScriptLocked( bool bLocked )                         { GetInterface()->SetLocked( bLocked ); }
    void                        SetAlwaysLeaveSkidMarks( bool bAlwaysLeaveSkidMarks )   { GetInterface()->ActivateSkidMarks( bAlwaysLeaveSkidMarks ); }
    void                        SetCanBeDamaged( bool bCanBeDamaged )                   { GetInterface()->EnableDamage( bCanBeDamaged ); }
    void                        SetCanBeTargettedByHeatSeekingMissiles( bool bEnabled ) { GetInterface()->SetHeatTraceable( bEnabled ); }
    void                        SetCanShootPetrolTank( bool bCanShoot )                 { GetInterface()->PutPetrolTank( bCanShoot ); }
    void                        SetChangeColourWhenRemapping( bool bChangeColour )      { GetInterface()->SetColourRemapping( bChangeColour ); }
    void                        SetComedyControls( bool bComedyControls )               { GetInterface()->SetComedyControls( bComedyControls ); }
    void                        SetEngineOn( bool bEngineOn );
    void                        SetGunSwitchedOff( bool bGunsOff )                      { GetInterface()->DisableGuns( bGunsOff ); }
    void                        SetHandbrakeOn( bool bHandbrakeOn )                     { GetInterface()->SetHandbrakeOn( bHandbrakeOn ); }
    void                        SetLightsOn( bool bLightsOn )                           { GetInterface()->SetLightsOn( bLightsOn ); }
    void                        SetTakeLessDamage( bool bTakeLessDamage )               { GetInterface()->SetArmored( bTakeLessDamage ); }
    void                        SetTyresDontBurst( bool bTyresDontBurst )               { GetInterface()->SetSteelTyres( bTyresDontBurst ); }
    void                        SetSirenOrAlarmActive( bool active )                    { GetInterface()->SetHornActive( active ); }
    void                        SetFadingOut( bool enable )                             { GetInterface()->SetFadingOut( enable ); }

    void                        SetOverrideLights( unsigned int uiOverrideLights )      { GetInterface()->m_overrideLights = uiOverrideLights; }

    CHandlingEntrySA*           GetHandlingData()                                       { return m_pHandlingData; }
    void                        SetHandlingData( CHandlingEntry *handling );

    void                        GetGravity( CVector& grav ) const                       { grav = m_vecGravity; }
    void                        SetGravity( const CVector& grav );

    CColModelSA*                GetSpecialColModel() const;

    virtual void                RecalculateHandling();

protected:
    CHandlingEntrySA*           m_pHandlingData;
    unsigned char               m_alpha;
    CVector                     m_vecGravity;
    SColor                      m_RGBColors[4];
    unsigned int                m_poolIndex;

    vehComponents_t             m_compContainer;
};

#endif

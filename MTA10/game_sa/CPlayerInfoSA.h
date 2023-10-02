/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerInfoSA.h
*  PURPOSE:     Header file for player ped type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PLAYERINFO
#define __CGAMESA_PLAYERINFO

#include <game/CPlayerInfo.h>
#include "Common.h"

class CPedSAInterface;
class CPlayerPedSAInterface;

#define CLASS_CPlayerInfo                           0xB7CD98    // ##SA##

#define FUNC_MakePlayerSafe                         0x56e870
#define FUNC_CancelPlayerEnteringCars               0x56e860
#define FUNC_ArrestPlayer                           0x56e5d0
#define FUNC_KillPlayer                             0x56e580

#define FUNC_TakeRemoteControlledCarFromPlayer      0x45ae80
#define FUNC_GivePlayerRemoteControlledCar          0x442020
#define FUNC_TakeRemoteControlOfCar                 0x45ad40
#define FUNC_StreamParachuteWeapon                  0x56eb30
#define FUNC_GivePlayerParachute                    0x56ec40

/**
 * \todo Implement FUNC_TakeRemoteControlOfCar, FUNC_StreamParachuteWeapon and FUNC_StreamParachuteWeapon
 */

#define VAR_PlayerPed                               0x94AD28
#define VAR_PlayerRCCar                             0x94AD2C
#define VAR_PlayerMoney                             0x94ADC8

#define NUM_CHARS_IN_NAME                           34

class CPedClothesDesc
{
public:
    unsigned int                            m_modelHash[9];                 // 0
    DWORD                                   m_pad;                          // 36
    unsigned int                            m_textureHash[10];              // 40
};

class CPlayerCrossHairSAInterface   //size: 12
{
public:
    bool    m_active;                   // 0
    float   m_tarX, m_tarY;             // 4, -1 ... 1 on screen
};

// WARNING: Keep this interface at SIZE=172
class CPlayerPedDataSAInterface
{
public:
                            CPlayerPedDataSAInterface( void );

    void* operator new( size_t );
    void operator delete( void *ptr );

    void                    Serialize( void );

    CWantedSAInterface*     m_Wanted;                           // 0
    CPedClothesDesc*        m_pClothes;                         // 4

    CPedSAInterface*        m_ArrestingOfficer;                 // 8, CCopPed*

    CVector2D               m_vecFightMovement;                 // 12
    float                   m_moveBlendRatio;                   // 20
    float                   m_fSprintEnergy;                    // 24
    //float                 m_fSprintControlCounter;            // Removed arbitatrily to aligned next byte, should be here really
    unsigned char           m_nChosenWeapon;                    // 28
    unsigned char           m_nCarDangerCounter;                // 29
    unsigned char           m_flags;                            // 30
    BYTE                    m_pad;                              // 31
    long                    m_nStandStillTimer;                 // 32
    unsigned int            m_nHitAnimDelayTimer;               // 36
    float                   m_fAttackButtonCounter;             // 40
    CVehicleSAInterface*    m_pDangerCar;                       // 44

    unsigned int            m_bStoppedMoving : 1;               // 48
    unsigned int            m_bAdrenaline : 1;
    unsigned int            m_bHaveTargetSelected : 1;          // Needed to work out whether we lost target this frame
    unsigned int            m_bFreeAiming : 1;
    unsigned int            m_bCanBeDamaged : 1;
    unsigned int            m_bAllMeleeAttackPtsBlocked : 1;    // if all of m_pMeleeAttackers[] is blocked by collision, just attack straight ahead
    unsigned int            m_JustBeenSnacking : 1;             // If this bit is true we have just bought something from a vending machine
    unsigned int            m_bRequireHandleBreath : 1;
       
    unsigned int            m_GroupStuffDisabled : 1;           // 49, if this is true the player can't recrout or give his group commands.
    unsigned int            m_GroupAlwaysFollow : 1;            // The group is told to always follow the player (used for girlfriend missions)
    unsigned int            m_GroupNeverFollow : 1;             // The group is told to always follow the player (used for girlfriend missions)
    unsigned int            m_bInVehicleDontAllowWeaponChange : 1; // stop weapon change once driveby weapon has been given
    unsigned int            m_bRenderWeapon : 1;                //  set to false during cutscenes so that knuckledusters are not rendered

    DWORD                   m_pad2;                             // 52
    long                    m_PlayerGroup;                      // 56

    unsigned int            m_AdrenalineEndTime;                // 60
    unsigned char           m_nDrunkenness;                     // 64
    bool                    m_bFadeDrunkenness;                 // 65
    unsigned char           m_nDrugLevel;                       // 66
    unsigned char           m_nScriptLimitToGangSize;           // 67
    
    float                   m_fBreath;                          // 68, for holding breath (ie underwater)

    unsigned int            m_meleeAnimRef;                     // 72
    unsigned int            m_meleeAnimExtraRef;                // 76

    float                   m_fFPSMoveHeading;                  // 80
    float                   m_fLookPitch;                       // 84
    float                   m_fSkateBoardSpeed;                 // 88
    float                   m_fSkateBoardLean;                  // 92
    
    RpAtomic*               m_pSpecialAtomic;                   // 96
    float                   m_fGunSpinSpeed;                    // 100
    float                   m_fGunSpinAngle;                    // 104

    unsigned int            m_LastTimeFiring;                   // 108
    unsigned int            m_nTargetBone;                      // 112
    CVector                 m_vecTargetBoneOffset;              // 116
    
    unsigned int            m_busFaresCollected;                // 128
    bool                    m_bPlayerSprintDisabled;            // 132
    bool                    m_bDontAllowWeaponChange;           // 133
    bool                    m_bForceInteriorLighting;           // 134
    unsigned char           m_pad3;                             // 135
    unsigned short          m_DPadDownPressedInMilliseconds;    // 136
    unsigned short          m_DPadUpPressedInMilliseconds;      // 138
        
    unsigned char           m_wetness;                          // 140
    unsigned char           m_playersGangActive;                // 141
    unsigned char           m_waterCoverPerc;                   // 142
    unsigned char           m_pad4;                             // 143
    float                   m_waterHeight;                      // 144

    unsigned int            m_heatseekAimTime;                  // 148
    CEntitySAInterface*     m_heatseekTarget;                   // 152
    
    long                    m_shotBuilding;                     // 156
    
    unsigned int            m_heatseekLockOnTime    :31;        // 160
    unsigned int            m_heakseekLockOn        :1;
    
    CPedSAInterface*        m_currentHooker;                    // 164
    CPedSAInterface*        m_lastHooker;                       // 168
};

enum ePlayerState : unsigned char
{
    PS_PLAYING,
    PS_DEAD,
    PS_ARRESTED,
    PS_FAILED_MISSION,
    PS_QUIT
};

//padlevel: 5
//todo: fix this class.
class CPlayerInfoSAInterface    //size: 400
{
public:
    class CPlayerPedSAInterface*    pPed;                               // 0, Pointer to the player ped (should always be set)
    CPlayerPedDataSAInterface       PlayerPedData;                      // 4, instance of player variables
    CVehicleSAInterface*            pRemoteVehicle;                     // 176, Pointer to vehicle player is driving remotely at the moment.(NULL if on foot)
    CVehicleSAInterface*            pSpecialCar;                        // 180, which car is using the special collision model
    long                            Score;                              // 184, Points for this player
    long                            DisplayScore;                       // 188, Points as they would be displayed
    long                            CollectablesPickedUp;               // 192, How many bags of sugar do we have
    long                            TotalNumCollectables;               // 196, How many bags of sugar are there to be had in the game

    unsigned int                    nLastBumpPlayerCarTimer;            // 200, Keeps track of when the last ped bumped into the player car

    unsigned int                    TaxiTimer;                          // 204, Keeps track of how long the player has been in a taxi with a passenger (in msecs)
    unsigned int                    vehicle_time_counter;               // 208, keeps track of how long player has been in car for driving skill
    bool                            bTaxiTimerScore;                    // 212, If TRUE then add 1 to score for each second that the player is driving a taxi 
    bool                            m_bTryingToExitCar;                 // 213, if player holds exit car button, want to trigger getout once car slowed enough

    BYTE                            pad[2];                             // 214

    CVehicleSAInterface*            pLastTargetVehicle;                 // 216, Last vehicle player tried to enter.

    ePlayerState                    PlayerState;                        // 220

    bool                            bAfterRemoteVehicleExplosion;       // 221
    bool                            bCreateRemoteVehicleExplosion;      // 222
    bool                            bFadeAfterRemoteVehicleExplosion;   // 223
    unsigned int                    TimeOfRemoteVehicleExplosion;       // 224

    unsigned int                    LastTimeEnergyLost;                 // 228, To make numbers flash on the HUD
    unsigned int                    LastTimeArmourLost;                 // 232

    unsigned int                    LastTimeBigGunFired;                // 236, Tank guns etc
    unsigned int                    TimesUpsideDownInARow;              // 240, Make car blow up if car upside down
    unsigned int                    TimesStuckInARow;                   // 244, Make car blow up if player cannot get out.
    
    // Stunt variables
    unsigned int                    nCarTwoWheelCounte;                 // 248, how long has player's car been on two wheels
    float                           fCarTwoWheelDist;                   // 252
    unsigned int                    nCarLess3WheelCounter;              // 256, how long has player's car been on less than 3 wheels
    unsigned int                    nBikeRearWheelCounter;              // 260, how long has player's bike been on rear wheel only
    float                           fBikeRearWheelDist;                 // 264
    unsigned int                    nBikeFrontWheelCounter;             // 268, how long has player's bike been on front wheel only
    float                           fBikeFrontWheelDist;                // 272
    unsigned int                    nTempBufferCounter;                 // 276, so wheels can leave the ground for a few frames without stopping above counters
    // best values for the script to check - will be zero most of the time, only value
    // when finished trick - script should retreve value then reset to zero
    unsigned int                    nBestCarTwoWheelsTimeMs;            // 280
    float                           fBestCarTwoWheelsDistM;             // 284
    unsigned int                    nBestBikeWheelieTimeMs;             // 288
    float                           fBestBikeWheelieDistM;              // 292
    unsigned int                    nBestBikeStoppieTimeMs;             // 296
    float                           fBestBikeStoppieDistM;              // 300

    unsigned short                  CarDensityForCurrentZone;           // 304
    BYTE                            pad2[2];                            // 306
    float                           RoadDensityAroundPlayer;            // 308

    unsigned int                    TimeOfLastCarExplosionCaused;       // 312
    long                            ExplosionMultiplier;                // 316
    unsigned int                    HavocCaused;                        // 320
    unsigned short                  TimeLastEaten;                      // 324
    BYTE                            pad3[2];                            // 326

    float                           CurrentChaseValue;                  // 328
    
    bool                            DoesNotGetTired;                    // 332
    bool                            FastReload;                         // 333
    bool                            FireProof;                          // 334
    unsigned char                   MaxHealth;                          // 335
    unsigned char                   MaxArmour;                          // 336

    bool                            bGetOutOfJailFree;                  // 337, Player doesn't lose money/weapons next time arrested
    bool                            bFreeHealthCare;                    // 338, Player doesn't lose money nexed time patched up at hospital

    bool                            bCanDoDriveBy;                      // 339
    
    unsigned char                   m_nBustedAudioStatus;               // 340
    BYTE                            pad4;                               // 341
    unsigned short                  m_nLastBustMessageNumber;           // 342

    CPlayerCrossHairSAInterface     CrossHair;                          // 344

    char                            m_skinName[32];                     // 356
    RwTexture*                      m_pSkinTexture;                     // 388, todo: fix this struct, darn't

    bool                            m_bParachuteReferenced;             // 392
    BYTE                            pad5[3];                            // 393
    unsigned int                    m_nRequireParachuteTimer;           // 396
};

// Internal function exports.
CPlayerPedSAInterface* __cdecl GetPlayerPed         ( int index );
CVehicleSAInterface* __cdecl GetPlayerVehicle       ( int index, bool excludeRemote );
const CVector& __cdecl FindPlayerCoords             ( CVector& pos, int id );
const CVector& __cdecl FindPlayerCenterOfWorld      ( int id );
float __cdecl FindPlayerHeading                     ( int id );

// Module initialization.
void PlayerInfo_Init( void );
void PlayerInfo_Shutdown( void );

// Used for extensional functionality.
namespace PlayerInfo
{
    inline CPlayerInfoSAInterface& GetInfo( int id )
    {
        static unsigned char* const VAR_currentPlayer = (unsigned char*)0x00B7CD74;

        if ( id < 0 )
            id = *VAR_currentPlayer;

        return *((CPlayerInfoSAInterface*)CLASS_CPlayerInfo + id);
    }
};

class CPlayerInfoSA : public CPlayerInfo
{
private:
    CPlayerInfoSAInterface* m_interface;
    CWantedSA*              m_wanted;

public:
    CPlayerInfoSA( CPlayerInfoSAInterface *info ) 
    { 
        m_interface = info;
        m_wanted = NULL;
    }

    ~CPlayerInfoSA( void )
    {
        if ( m_wanted )
            delete m_wanted;
    }

    CPlayerInfoSAInterface*         GetInterface                ( void )            { return m_interface; }
    const CPlayerInfoSAInterface*   GetInterface                ( void ) const      { return m_interface; }

    CPlayerPedSA*                   GetPlayerPed                ( void );
    CWantedSA*                      GetWanted                   ( void );
    long                            GetPlayerMoney              ( void );
    void                            SetPlayerMoney              ( long lMoney );

    bool                            GetCrossHair                ( float& tarX, float& tarY );

    void                            GivePlayerParachute         ( void );
    void                            StreamParachuteWeapon       ( bool bAllowParachute );

    unsigned short                  GetLastTimeEaten            ( void );
    void                            SetLastTimeEaten            ( unsigned short time );

    void                            MakePlayerSafe              ( bool safe );
    void                            CancelPlayerEnteringCars    ( CVehicleSA* veh );
    void                            ArrestPlayer                ( void );
    void                            KillPlayer                  ( void );

    // This does not make any sense here, no joke, CRemote? (todo)
    CVehicle*                       GiveRemoteVehicle           ( unsigned short model, float x, float y, float z );
    void                            StopRemoteControl           ( void );
    CVehicle*                       GetRemoteVehicle            ( void );

    float                           GetFPSMoveHeading           ( void );

    bool                            GetDoesNotGetTired          ( void );
    void                            SetDoesNotGetTired          ( bool award );

    unsigned int                    GetLastTimeBigGunFired      ( void );
    void                            SetLastTimeBigGunFired      ( unsigned int time );
    
    inline unsigned int             GetCarTwoWheelCounter       ( void ) const      { return m_interface->nBestCarTwoWheelsTimeMs; }
    inline float                    GetCarTwoWheelDist          ( void ) const      { return m_interface->fBestCarTwoWheelsDistM; }
    inline unsigned int             GetCarLess3WheelCounter     ( void ) const      { return m_interface->nCarLess3WheelCounter; }
    inline unsigned int             GetBikeRearWheelCounter     ( void ) const      { return m_interface->nBestBikeWheelieTimeMs; }
    inline float                    GetBikeRearWheelDist        ( void ) const      { return m_interface->fBestBikeWheelieDistM; }
    inline unsigned int             GetBikeFrontWheelCounter    ( void ) const      { return m_interface->nBestBikeStoppieTimeMs; }
    inline float                    GetBikeFrontWheelDist       ( void ) const      { return m_interface->fBestBikeStoppieDistM; }
};

#endif

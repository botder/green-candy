/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCameraSA.h
*  PURPOSE:     Header file for camera rendering class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CAMERA
#define __CGAMESA_CAMERA

#include <game/CCamera.h>
#include "CCamSA.h"
#include "CGarageSA.h"
#include "CEntitySA.h"
#include "COffsets.h"

#define FUNC_TakeControl                    0x50C7C0 // ##SA##
#define FUNC_TakeControlNoEntity            0x50C8B0
#define FUNC_TakeControlAttachToEntity      0x50C910
#define FUNC_Restore                        0x50B930
#define FUNC_SetCamPositionForFixedMode     0x50BEC0
#define FUNC_ConeCastCollisionResolve       0x51A5D0

#define FUNC_Find3rdPersonCamTargetVector   0x514970 //##SA##
#define FUNC_Find3rdPersonQuickAimPitch     0x50AD40
#define FUNC_TryToStartNewCamMode           0x467AAD
#define FUNC_VectorTrackLinear              0x50D1D0
#define FUNC_GetFadingDirection             0x50ADF0
#define FUNC_GetFading                      0x50ADE0
#define FUNC_Fade                           0x50AC20
#define FUNC_SetFadeColour                  0x50BF00
    
#define VAR_ActiveCam                       0x7E46FE
#define VAR_CameraRotation                  0xB6F178 // used for controling where the player faces
#define VAR_VehicleCameraView               0xB6F0DC


#define MAX_CAMS                            3 

#define MAX_NUM_OF_NODES                    800 // for trains

/*** R* Defines ***/
#define NUMBER_OF_VECTORS_FOR_AVERAGE 2
#define MAX_NUM_OF_SPLINETYPES (4)
/*** END R* Defines ***/

/*** BEGIN PURE R* CLASSES ***/

class CCamPathSplines //size: 4
{
public: 
    enum {MAXPATHLENGTH=800};
    FLOAT *m_arr_PathData;              // FLOAT m_arr_PathData[MAXPATHLENGTH];    
};

class CQueuedMode //size: 12
{
public:
    short       Mode;                   // 0, CameraMode
    FLOAT       Duration;               // 4, How long for (<0.0f -> indefinately)
    short       MinZoom, MaxZoom;       // 8, How far is player allowed to zoom in & out
};

class CTrainCamNode
{
public:
    CVector m_cvecCamPosition;
    CVector m_cvecPointToLookAt;
    CVector m_cvecMinPointInRange;//this is the minimum required distance the train has to be to the camera to 
    //allow a switch to the node cam 
    CVector m_cvecMaxPointInRange;//this is the minimum required distance the train has to be to from the camera 
    FLOAT m_fDesiredFOV;
    FLOAT m_fNearClip;
    //to switch from the  the node cam
};

/*** END PURE R* CLASSES ***/

class CCameraSAInterface : public CPlaceableSAInterface //size: 3132
{
public:
    bool __thiscall         IsSphereVisible             ( const CVector& pos, float radius, const RwMatrix& transform );
    float __thiscall        GetGroundLevel              ( unsigned int type );
    unsigned int __thiscall GetMusicFadeType            ( void ) const;

    void __thiscall         SetFadeColor                ( unsigned char red, unsigned char green, unsigned char blue );
    void __thiscall         Fade                        ( float fadeDuration, unsigned short direction );

    int __thiscall          GetFadeDirection            ( void ) const;

    CCamSAInterface&        GetActiveCam                ( void )                    { return Cams[ActiveCam]; }

    int __thiscall          GetActiveCamLookDirection   ( void );

    //move these out the class, have decided to set up a mirrored enumerated type thingy at the top
#if 0
    bool    m_bAboveGroundTrainNodesLoaded;                 // 20
    bool    m_bBelowGroundTrainNodesLoaded;                 // 21
    bool    m_bCamDirectlyBehind;                           // 22
    bool    m_bCamDirectlyInFront;                          // 23
#endif
    bool    m_bCameraJustRestored;                          // 24
    bool    m_bcutsceneFinished;                            // 25
    bool    m_bCullZoneChecksOn;                            // 26
    bool    m_bFirstPersonBeingUsed;                        // 27, To indicate if the m_bFirstPersonBeingUsed viewer is being used.
    bool    m_bJustJumpedOutOf1stPersonBecauseOfTarget;     // 28
    bool    m_bIdleOn;                                      // 29
    bool    m_bInATunnelAndABigVehicle;                     // 30
    bool    m_bInitialNodeFound;                            // 31
    bool    m_bInitialNoNodeStaticsSet;                     // 32
    bool    m_bIgnoreFadingStuffForMusic;                   // 33
    bool    m_bPlayerIsInGarage;                            // 34
    bool    m_bPlayerWasOnBike;                             // 35
    bool    m_bJustCameOutOfGarage;                         // 36
    bool    m_bJustInitalised;                              // 37, Just so the speed thingy doesn't go mad right at the start
    unsigned char   m_bJust_Switched;                       // 38, Variable to indicate that we have jumped somewhere, Raymond needs this for the audio engine
    bool    m_bLookingAtPlayer;                             // 39
    bool    m_bLookingAtVector;                             // 40
    bool    m_bMoveCamToAvoidGeom;                          // 41
    bool    m_bObbeCinematicPedCamOn;                       // 42
    bool    m_bObbeCinematicCarCamOn;                       // 43
    bool    m_bRestoreByJumpCut;                            // 44
    bool    m_bUseNearClipScript;                           // 45
    bool    m_bStartInterScript;                            // 46
    unsigned char   m_bStartingSpline;                      // 47
    bool    m_bTargetJustBeenOnTrain;                       // 48, this variable is needed to be able to restore the camera
    bool    m_bTargetJustCameOffTrain;                      // 49
    bool    m_bUseSpecialFovTrain;                          // 50
    bool    m_bUseTransitionBeta;                           // 51
    bool    m_bUseScriptZoomValuePed;                       // 52
    bool    m_bUseScriptZoomValueCar;                       // 53
    bool    m_bWaitForInterpolToFinish;                     // 54
    bool    m_bItsOkToLookJustAtThePlayer;                  // 55, Used when interpolating
    bool    m_bWantsToSwitchWidescreenOff;                  // 56
    bool    m_WideScreenOn;                                 // 57
    bool    m_1rstPersonRunCloseToAWall;                    // 58
    bool    m_bHeadBob;                                     // 59
    bool    m_bVehicleSuspenHigh;                           // 60
    bool    m_bEnable1rstPersonCamCntrlsScript;             // 61
    bool    m_bAllow1rstPersonWeaponsCamera;                // 62
    bool    m_bCooperativeCamMode;                          // 63
    bool    m_bAllowShootingWith2PlayersInCar;              // 64
    bool    m_bDisableFirstPersonInCar;                     // 65
    static bool m_bUseMouse3rdPerson;
    bool    bStaticFrustum;                                 // 66

    // for debug keyboard stuff
    unsigned char display_kbd_debug;                        // 67
    float kbd_fov_value;                                    // 68

    // The following fields allow the level designers to specify the camera for 2 player games.
    short   m_ModeForTwoPlayersSeparateCars;                // 72
    short   m_ModeForTwoPlayersSameCarShootingAllowed;      // 74
    short   m_ModeForTwoPlayersSameCarShootingNotAllowed;   // 76
    short   m_ModeForTwoPlayersNotBothInCar;                // 78

    // The_GTA: Here was another mess-up about the interface I was not sure about.
    // To properly align the fading information, I had to shift members around.

    bool    m_bGarageFixedCamPositionSet;                   // 80
    
    bool    m_bFading;                                      // 81, to indicate that we are fading 
    bool    m_bMusicFading;                                 // 82
    bool    m_bMusicFadedOut;                               // 83

    bool    m_vecDoingSpecialInterPolation;                 // 84 (81?)
    bool    m_bScriptParametersSetForInterPol;              // 85 (82?)

    bool    m_bFailedCullZoneTestPreviously;                // 86
    bool    m_FadeTargetIsSplashScreen;                     // 87, used as hack for fading 
    bool    WorldViewerBeingUsed;                           // 88, To indicate if the world viewer is being used.                                      

    
    unsigned char   m_uiTransitionJUSTStarted;              // 89, This is the first frame of a transition.
    unsigned char   m_uiTransitionState;                    // 90, 0:one mode 1:transition
    unsigned char   ActiveCam;                              // 91, Which one at the moment (0 or 1)
                                                            // Their is a fudge at the end when the renderware matrix will receive either
                                                            // the active camera or the worldviewer camera
    unsigned int    m_uiCamShakeStart;                      // 92, When did the camera shake start.
    unsigned int    m_uiFirstPersonCamLastInputTime;        // 96
    unsigned int    m_uiLongestTimeInMill;                  // 100
    unsigned int    m_uiNumberOfTrainCamNodes;              // 104
    unsigned int    m_uiTimeLastChange;                     // 108
    unsigned int    m_uiTimeWeLeftIdle_StillNoInput;        // 112
    unsigned int  m_uiTimeWeEnteredIdle;                    // 116
    unsigned int    m_uiTimeTransitionStart;                // 120, When was the transition started ?
    unsigned int    m_uiTransitionDuration;                 // 124, How long does the transition take ?
    unsigned int    m_uiTransitionDurationTargetCoors;      // 128
    int     m_BlurBlue;                                     // 132
    int     m_BlurGreen;                                    // 136
    int     m_BlurRed;                                      // 140
    int     m_BlurType;                                     // 144
    int     m_iWorkOutSpeedThisNumFrames;                   // 148, duh  
    int     m_iNumFramesSoFar;                              // 152, counter
    int     m_iCurrentTrainCamNode;                         // 156, variable indicating which camera node we are at for the train
    int     m_motionBlur;                                   // 160, to indicate that we are fading

    int     m_imotionBlurAddAlpha;                          // 164
    int     m_iCheckCullZoneThisNumFrames;                  // 168
    int     m_iZoneCullFrameNumWereAt;                      // 172
    int     WhoIsInControlOfTheCamera;                      // 176, to discern between obbe and scripts

//  float   CarZoomIndicator;
//  float   CarZoomValue;
//  float   CarZoomValueSmooth;
//  float   m_fCarZoomValueScript;
//  float   PedZoomIndicator;
//  float   m_fPedZoomValue;
//  float   m_fPedZoomValueSmooth;
//  float   m_fPedZoomValueScript;
    int     m_nCarZoom;                                     // 180, store zoom index
    float   m_fCarZoomBase;                                 // 184, store base zoom distance from index
    float   m_fCarZoomTotal;                                // 188, store total zoom after modded by camera modes
    float   m_fCarZoomSmoothed;                             // 192, buffered version of the var above
    float   m_fCarZoomValueScript;                          // 196
    int     m_nPedZoom;                                     // 200, store zoom index
    float   m_fPedZoomBase;                                 // 204, store base zoom distance from index
    float   m_fPedZoomTotal;                                // 208, store total zoom after modded by camera modes
    float   m_fPedZoomSmoothed;                             // 212, buffered version of the var above
    float   m_fPedZoomValueScript;                          // 216


    float   CamFrontXNorm, CamFrontYNorm;                   // 220
    float   DistanceToWater;                                // 228
    float   HeightOfNearestWater;                           // 232
    float   FOVDuringInter;                                 // 236
    float   LODDistMultiplier;                              // 240, This takes into account the FOV and the standard LOD multiplier Smaller aperture->bigger LOD multipliers.
    float   GenerationDistMultiplier;                       // 244, This takes into account the FOV but noy the standard LOD multiplier

    float   m_fAlphaSpeedAtStartInter;                      // 248
    float   m_fAlphaWhenInterPol;                           // 252
    float   m_fAlphaDuringInterPol;                         // 256
    float   m_fBetaDuringInterPol;                          // 260
    float   m_fBetaSpeedAtStartInter;                       // 264
    float   m_fBetaWhenInterPol;                            // 268
    float   m_fFOVWhenInterPol;                             // 272
    float   m_fFOVSpeedAtStartInter;                        // 276
    float   m_fStartingBetaForInterPol;                     // 280
    float   m_fStartingAlphaForInterPol;                    // 284
    float   m_PedOrientForBehindOrInFront;                  // 288

    float   m_CameraAverageSpeed;                           // 292, this is an average depending on how many frames we work it out
    float   m_CameraSpeedSoFar;                             // 296, this is a running total
    float   m_fCamShakeForce;                               // 300, How severe is the camera shake.
    float   m_fFovForTrain;                                 // 304
    float   m_fFOV_Wide_Screen;                             // 308

    float   m_fNearClipScript;                              // 312
    float   m_fOldBetaDiff;                                 // 316, Needed for interpolation between 2 modes
    float   m_fPositionAlongSpline;                         // 320, Variable used to indicate how far along the spline we are 0-1 for started to completed respectively
    float   m_ScreenReductionPercentage;                    // 324
    float   m_ScreenReductionSpeed;                         // 328
    float   m_AlphaForPlayerAnim1rstPerson;                 // 332
    
    float   Orientation;                                    // 336, The orientation of the camera. Used for peds walking.
    float   PlayerExhaustion;                               // 340, How tired is player (inaccurate sniping) 0.0f-1.0f

    // The following things are used by the sound code to
    // play reverb depending on the surroundings. From a point
    // in front of the camera the disance is measured to the
    // nearest obstacle (building)
    float   SoundDistUp;                                    // 344, SoundDistLeft, SoundDistRight;     // These ones are buffered and should be used by the audio
    float   SoundDistUpAsRead;                              // 348, SoundDistLeftAsRead, SoundDistRightAsRead;
    float   SoundDistUpAsReadOld;                           // 352, SoundDistLeftAsReadOld, SoundDistRightAsReadOld;
    // Very rough distance to the nearest water for the sound to use
    // Front vector X&Y normalised to 1. Used by loads of stuff.
    
    float   m_fAvoidTheGeometryProbsTimer;                  // 356
    short   m_nAvoidTheGeometryProbsDirn;                   // 360
    
    float   m_fWideScreenReductionAmount;                   // 364, 0 for not reduced 1 for fully reduced (Variable for Les)
    float   m_fStartingFOVForInterPol;                      // 368
    
    // These ones are static so that they don't get cleared in CCamera::Init()
    static  float m_fMouseAccelHorzntl;// acceleration multiplier for 1st person controls
    static  float m_fMouseAccelVertical;// acceleration multiplier for 1st person controls
    static  float m_f3rdPersonCHairMultX;
    static  float m_f3rdPersonCHairMultY;

    CCamSAInterface Cams[3];                                // 372
    // The actual cameras (usually only one of the two is active)
    // And to complicate this we have a third camera, this camera is 
    // used for debugging when we want to have a look at the world.
    // We can't change the camera mode because other objects depend on their


    // #########################################//
    // DATA NOT UPDATED FOR SA  BELOW HERE!!!!! //
    // #########################################//

    CGarageSAInterface *pToGarageWeAreIn;                   // 2076
    CGarageSAInterface *pToGarageWeAreInForHackAvoidFirstPerson;    // 2080
    CQueuedMode m_PlayerMode;                               // 2084
    // The higher priority player camera mode. This one is used
    // for the sniper mode and rocket launcher mode.
    // This one overwrites the m_PlayerMode above.
    CQueuedMode PlayerWeaponMode;                           // 2096
    CVector m_PreviousCameraPosition;                       // 2108, needed to work out speed
    CVector m_RealPreviousCameraPosition;                   // 2120, This cane be used by stuff outside the camera code. The one above is the same as the current coordinates outwidth the camera code.
                                                            // an active camera for range finding etc
    CVector m_cvecAimingTargetCoors;                        // 2132, Coors to look at with Gordons aiming thing
    // The player camera that is waiting to be used
    // This camera can replace the default camera where this is
    // needed (in tricky situations like tunnels for instance)
    CVector m_vecFixedModeVector;                           // 2144
    CVector m_vecFixedModeSource;                           // 2156
    CVector m_vecFixedModeUpOffSet;                         // 2168
    CVector m_vecCutSceneOffset;                            // 2180
    CVector m_cvecStartingSourceForInterPol;                // 2192
    CVector m_cvecStartingTargetForInterPol;                // 2204
    CVector m_cvecStartingUpForInterPol;                    // 2216
    CVector m_cvecSourceSpeedAtStartInter;                  // 2228
    CVector m_cvecTargetSpeedAtStartInter;                  // 2240
    CVector m_cvecUpSpeedAtStartInter;                      // 2252
    CVector m_vecSourceWhenInterPol;                        // 2264
    CVector m_vecTargetWhenInterPol;                        // 2276
    CVector m_vecUpWhenInterPol;                            // 2288
    CVector m_vecClearGeometryVec;                          // 2300
    CVector m_vecGameCamPos;                                // 2312
    CVector SourceDuringInter, TargetDuringInter, UpDuringInter;    // 2324


    CVector m_vecAttachedCamOffset;                         // 2360, for attaching the camera to a ped or vehicle (set by level designers for use in cutscenes)
    CVector m_vecAttachedCamLookAt;                         // 2372
    FLOAT m_fAttachedCamAngle;                              // 2384, for giving the attached camera a tilt.

    RwCamera* m_pRwCamera;                                  // 2388, RenderWare camera pointer

    CEntitySAInterface *pTargetEntity;                      // 2392, stuff for cut scenes
    CEntitySAInterface *pAttachedEntity;                    // 2396
    //CVector CutScene; 
    CCamPathSplines m_arrPathArray[MAX_NUM_OF_SPLINETYPES]; // 2400, These only get created when the script calls the load splines function
    // maybe this shouldn't be here depends if GTA_TRAIN is defined (its not)
    //CTrainCamNode     m_arrTrainCamNode[MAX_NUM_OF_NODES];

    bool m_bMirrorActive;                                   // 2416
    bool m_bResetOldMatrix;                                 // 2417

//  protected:
#if 0
    // Original
    RwMatrix m_cameraMatrix;                                // 2420
    RwMatrix m_cameraMatrixOld;                             // 2484
    RwMatrix m_viewMatrix;                                  // 2548
    RwMatrix m_matInverse;                                  // 2612
    RwMatrix m_matMirrorInverse;                            // 2676
    RwMatrix m_matMirror;                                   // 2740
#else
    // Looks more likely to be this
    RwMatrix m_cameraMatrix;                                // 2420
    int unk1[2];                                            // 2484
    RwMatrix m_cameraMatrixOld;                             // 2492
    int unk2[2];                                            // 2556
    RwMatrix m_viewMatrix;                                  // 2564
    int unk3[2];                                            // 2628
    RwMatrix m_matInverse;                                  // 2636
    int unk4[2];                                            // 2700
    RwMatrix m_matMirrorInverse;                            // 2708
    int unk5[8];                                            // 2772
#endif

    CVector m_vecFrustumNormals[4];                         // 2804
    CVector m_vecFrustumWorldNormals[4];                    // 2852
    CVector m_vecFrustumWorldNormals_Mirror[4];             // 2900

    float   m_fFrustumPlaneOffsets[4];                      // 2948
    float   m_fFrustumPlaneOffsets_Mirror[4];               // 2964

    CVector m_vecRightFrustumNormal;                        // 2980
    CVector m_vecBottomFrustumNormal;                       // 2992
    CVector m_vecTopFrustumNormal;                          // 3004

    // The_GTA: I miss 12 bytes in this interface somewhere (looking at
    // (1.0 US and 1.0 EU): 0x0050AC25). I decided to put these bytes
    // here. Correct me if I am wrong!
    BYTE    m_pad[12];                                      // 3016

    CVector m_vecOldSourceForInter;                         // 3028
    CVector m_vecOldFrontForInter;                          // 3040
    CVector m_vecOldUpForInter;                             // 3052
    float   m_vecOldFOVForInter;                            // 3064
    float   m_fFLOATingFade;                                // 3068, variable representing the FLOAT version of CDraw::Fade. Necessary to stop loss of precision
    float   m_fFLOATingFadeMusic;                           // 3072
    float   m_fTimeToFadeOut;                               // 3076
    float   m_fTimeToFadeMusic;                             // 3080
    float   m_fTimeToWaitToFadeMusic;                       // 3084
    float   m_fFractionInterToStopMoving;                   // 3088
    float   m_fFractionInterToStopCatchUp;                  // 3092
    float   m_fFractionInterToStopMovingTarget;             // 3096
    float   m_fFractionInterToStopCatchUpTarget;            // 3100

    float   m_fGaitSwayBuffer;                              // 3104
    float   m_fScriptPercentageInterToStopMoving;           // 3108
    float   m_fScriptPercentageInterToCatchUp;              // 3112
    DWORD   m_fScriptTimeForInterPolation;                  // 3116


    short   m_iFadingDirection;                             // 3120
    int     m_iModeObbeCamIsInForCar;                       // 3124
    short   m_iModeToGoTo;                                  // 3128
    short   m_iMusicFadingDirection;                        // 3130
    short   m_iTypeOfSwitch;                                // 3132

    BYTE    m_pad2[2];                                      // 3134

    DWORD   m_uiFadeTimeStarted;                            // 3136
    DWORD   m_uiFadeTimeStartedMusic;                       // 3140
};


namespace Camera
{
    inline CCameraSAInterface&     GetInterface( void )
    {
        return *(CCameraSAInterface*)0x00B6F028;
    }
};


class CCameraSA : public CCamera
{
    friend class COffsets;
private:
    CCameraSAInterface*         m_interface;
    CCamSA*                     m_cams[MAX_CAMS];

public:
                                CCameraSA( CCameraSAInterface *cam );
                                ~CCameraSA();

    CCameraSAInterface*         GetInterface()                                          { return m_interface; }
    const CCameraSAInterface*   GetInterface() const                                    { return m_interface; }

    void                        TakeControl( CEntity *entity, eCamMode mode, int switchStyle );
    void                        TakeControl( const CVector& pos, int switchStyle );
    void                        TakeControlAttachToEntity( CEntity *target, CEntity *attach, const CVector& offset, const CVector& lookAt, float tilt, int switchStyle );

    void                        Restore();
    void                        RestoreWithJumpCut();

    const RwMatrix&             GetMatrix() const;
    void                        SetMatrix( const RwMatrix& mat );

    void                        SetCamPositionForFixedMode( const CVector& pos, const CVector& upOffset );
    void                        Find3rdPersonCamTargetVector( float distance, const CVector& gunMuzzle, const CVector& source, CVector& target ) const;
    float                       Find3rdPersonQuickAimPitch() const;

    unsigned char               GetActiveCam() const;

    CCamSA*                     GetCam( unsigned char id );
    CCamSA*                     GetCam( CCamSAInterface *cam );

    void                        SetWidescreen( bool wide )                              { m_interface->m_WideScreenOn = wide; }
    bool                        GetWidescreen() const                                   { return m_interface->m_WideScreenOn; }

    float                       GetCarZoom() const;
    void                        SetCarZoom( float zoom );

    bool                        TryToStartNewCamMode( unsigned char mode );
    bool                        ConeCastCollisionResolve( const CVector& pos, const CVector& lookAt, const CVector& dest, float rad, float minDist, float& dist );
    void                        VectorTrackLinear( const CVector& to, const CVector& from, float time, bool smoothEnds );

    bool                        IsFading() const;
    int                         GetFadingDirection() const;
    void                        Fade( float fadeOutTime, int outOrIn );
    void                        SetFadeColor( unsigned char red, unsigned char green, unsigned char blue );

    float                       GetCameraRotation() const;
    const RwMatrix&             GetLTM() const;

    CEntity*                    GetTargetEntity() const;

    void                        SetCameraClip( bool ibjects, bool vehicles );
    unsigned char               GetCameraViewMode() const;
    void                        SetCameraViewMode( unsigned char mode );

    bool                        IsSphereVisible( const RwSphere& sphere ) const;
};

void Camera_Init( void );
void Camera_Shutdown( void );

#endif

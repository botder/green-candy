/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCamSA.h
*  PURPOSE:     Header file for camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CAM
#define __CGAMESA_CAM

#include <game/CCam.h>

const int NUMBER_OF_VECTORS_FOR_AVERAGE=2;
#define CAM_NUM_TARGET_HISTORY          4

//padlevel: 2
class CCamSAInterface // 568 bytes
{
public:
    bool    bBelowMinDist;                                          // 0, used for follow ped mode
    bool    bBehindPlayerDesired;                                   // 1, used for follow ped mode
    bool    m_bCamLookingAtVector;                                  // 2
    bool    m_bCollisionChecksOn;                                   // 3
    bool    m_bFixingBeta;                                          // 4, used for camera on a string
    bool    m_bTheHeightFixerVehicleIsATrain;                       // 5
    bool    LookBehindCamWasInFront;                                // 6
    bool    LookingBehind;                                          // 7
    bool    LookingLeft;                                            // 8
    bool    LookingRight;                                           // 9
    bool    ResetStatics;                                           // 10, for interpolation type stuff to work
    bool    Rotating;                                               // 11

    short   Mode;                                                   // 12, CameraMode
    BYTE    pad1[2];                                                // 14, alignment
    unsigned int  m_uiFinishTime;                                   // 16
    
    int     m_iDoCollisionChecksOnFrameNum;                         // 20
    int     m_iDoCollisionCheckEveryNumOfFrames;                    // 24
    int     m_iFrameNumWereAt;                                      // 28
    int     m_iRunningVectorArrayPos;                               // 32
    int     m_iRunningVectorCounter;                                // 36
    int     DirectionWasLooking;                                    // 40
    
    float   f_max_role_angle;                                       // 44, =DEGTORAD(5.0f);    
    float   f_Roll;                                                 // 48, used for adding a slight roll to the camera in the
    float   f_rollSpeed;                                            // 52, camera on a string mode
    float   m_fSyphonModeTargetZOffSet;                             // 56
    float   m_fAmountFractionObscured;                              // 60
    float   m_fAlphaSpeedOverOneFrame;                              // 64
    float   m_fBetaSpeedOverOneFrame;                               // 68
    float   m_fBufferedTargetBeta;                                  // 72
    float   m_fBufferedTargetOrientation;                           // 76
    float   m_fBufferedTargetOrientationSpeed;                      // 80
    float   m_fCamBufferedHeight;                                   // 84
    float   m_fCamBufferedHeightSpeed;                              // 88
    float   m_fCloseInPedHeightOffset;                              // 92
    float   m_fCloseInPedHeightOffsetSpeed;                         // 96
    float   m_fCloseInCarHeightOffset;                              // 100
    float   m_fCloseInCarHeightOffsetSpeed;                         // 104
    float   m_fDimensionOfHighestNearCar;                           // 108
    float   m_fDistanceBeforeChanges;                               // 112
    float   m_fFovSpeedOverOneFrame;                                // 116
    float   m_fMinDistAwayFromCamWhenInterPolating;                 // 120
    float   m_fPedBetweenCameraHeightOffset;                        // 124
    float   m_fPlayerInFrontSyphonAngleOffSet;                      // 128
    float   m_fRadiusForDead;                                       // 132
    float   m_fRealGroundDist;                                      // 136, used for follow ped mode
    float   m_fTargetBeta;                                          // 140
    float   m_fTimeElapsedFloat;                                    // 144
    float   m_fTilt;                                                // 148
    float   m_fTiltSpeed;                                           // 152

    float   m_fTransitionBeta;                                      // 156
    float   m_fTrueBeta;                                            // 160
    float   m_fTrueAlpha;                                           // 164
    float   m_fInitialPlayerOrientation;                            // 168, used for first person

    float   Alpha;                                                  // 172
    float   AlphaSpeed;                                             // 176
    float   FOV;                                                    // 180
    float   FOVSpeed;                                               // 184
    float   Beta;                                                   // 188
    float   BetaSpeed;                                              // 192
    float   Distance;                                               // 196
    float   DistanceSpeed;                                          // 200
    float   CA_MIN_DISTANCE;                                        // 204
    float   CA_MAX_DISTANCE;                                        // 208
    float   SpeedVar;                                               // 212
    float   m_fCameraHeightMultiplier;                              // 216, used by TwoPlayer_Separate_Cars_TopDown
    
    // ped onfoot zoom distance
    float m_fTargetZoomGroundOne;                                   // 220
    float m_fTargetZoomGroundTwo;                                   // 224
    float m_fTargetZoomGroundThree;                                 // 228
    // ped onfoot alpha angle offset
    float m_fTargetZoomOneZExtra;                                   // 232
    float m_fTargetZoomTwoZExtra;                                   // 236
    float m_fTargetZoomTwoInteriorZExtra;                           // 240, extra one for interior
    float m_fTargetZoomThreeZExtra;                                 // 244
    
    float m_fTargetZoomZCloseIn;                                    // 248
    float m_fMinRealGroundDist;                                     // 252
    float m_fTargetCloseInDist;                                     // 256

    // For targetting in cooperative mode.
    float   Beta_Targeting;                                         // 260
    float   X_Targetting, Y_Targetting;                             // 264
    int CarWeAreFocussingOn;                                        // 272, which car is closer to the camera in 2 player cooperative mode with separate cars.
    float   CarWeAreFocussingOnI;                                   // 276, interpolated version
    
    float m_fCamBumpedHorz;                                         // 280
    float m_fCamBumpedVert;                                         // 284
    int m_nCamBumpedTime;                                           // 288

    // NOT IN CLASS
    static int CAM_BUMPED_SWING_PERIOD;                 
    static int CAM_BUMPED_END_TIME;
    static float CAM_BUMPED_DAMP_RATE;
    static float CAM_BUMPED_MOVE_MULT;

    CVector m_cvecSourceSpeedOverOneFrame;                          // 292
    CVector m_cvecTargetSpeedOverOneFrame;                          // 304
    CVector m_cvecUpOverOneFrame;                                   // 316
    
    CVector m_cvecTargetCoorsForFudgeInter;                         // 328
    CVector m_cvecCamFixedModeVector;                               // 340
    CVector m_cvecCamFixedModeSource;                               // 352
    CVector m_cvecCamFixedModeUpOffSet;                             // 364
    CVector m_vecLastAboveWaterCamPosition;                         // 376, helper for when the player has gone under the water

    CVector m_vecBufferedPlayerBodyOffset;                          // 388

    // The three vectors that determine this camera for this frame
    CVector Front;                                                  // 400, Direction of looking in
    CVector Source;                                                 // 412, Coors in world space
    CVector SourceBeforeLookBehind;                                 // 424
    CVector Up;                                                     // 436, Just that
    CVector m_arrPreviousVectors[NUMBER_OF_VECTORS_FOR_AVERAGE];    // 448, used to average stuff

    CVector m_aTargetHistoryPos[CAM_NUM_TARGET_HISTORY];            // 472
    DWORD m_nTargetHistoryTime[CAM_NUM_TARGET_HISTORY];             // 520
    DWORD m_nCurrentHistoryPoints;                                  // 536

    CEntitySAInterface *CamTargetEntity;                            // 540

    float       m_fCameraDistance;                                  // 544
    float       m_fIdealAlpha;                                      // 548
    float       m_fPlayerVelocity;                                  // 552
    CVehicleSAInterface*    m_pLastCarEntered;                      // 556, So interpolation works
    CPedSAInterface*        m_pLastPedLookedAt;                     // 560, So interpolation works 
    bool        m_bFirstPersonRunAboutActive;                       // 564
    BYTE        pad2[3];                                            // 565, alignment
};

class CCamSA : public CCam
{
private:
    CCamSAInterface*    m_pInterface;

public:
                        CCamSA( CCamSAInterface* pInterface )       { m_pInterface = pInterface; }

    CCamSAInterface*    GetInterface()                              { return m_pInterface; }

    void                SetVehicleInterpolationSource( CVehicle *veh );
    void                SetPedInterpolationSource( CPed *ped );

    CVector*            GetFront() const                            { return &m_pInterface->Front; }
    CVector*            GetUp() const                               { return &m_pInterface->Up; }
    CVector*            GetSource() const                           { return &m_pInterface->Source; }
    unsigned int        GetMode() const                             { return m_pInterface->Mode; }
    float               GetFOV() const                              { return m_pInterface->FOV; }
    void                SetFOV( float fFOV )                        { m_pInterface->FOV = fFOV; }

    CVector*            GetFixedModeSource() const                  { return &m_pInterface->m_cvecCamFixedModeSource; }
    CVector*            GetFixedModeVector() const                  { return &m_pInterface->m_cvecCamFixedModeVector; }
    CVector*            GetTargetHistoryPos() const                 { return m_pInterface->m_aTargetHistoryPos; }

    CEntity*            GetTargetEntity() const;
};

#endif

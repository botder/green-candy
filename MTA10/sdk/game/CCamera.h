/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CCamera.h
*  PURPOSE:     Camera scene rendering interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CAMERA
#define __CGAME_CAMERA

#include "CEntity.h"
#include "CCam.h"


enum eCamMode 
{ 
    MODE_NONE = 0, 
    MODE_TOPDOWN, //=1, 
    MODE_GTACLASSIC,// =2, 
    MODE_BEHINDCAR, //=3, 
    MODE_FOLLOWPED, //=4, 
    MODE_AIMING, //=5, 
    MODE_DEBUG, //=6, 
    MODE_SNIPER, //=7, 
    MODE_ROCKETLAUNCHER, //=8,   
    MODE_MODELVIEW, //=9, 
    MODE_BILL, //=10, 
    MODE_SYPHON, //=11, 
    MODE_CIRCLE, //=12, 
    MODE_CHEESYZOOM, //=13, 
    MODE_WHEELCAM, //=14, 
    MODE_FIXED, //=15, 
    MODE_1STPERSON, //=16, 
    MODE_FLYBY, //=17, 
    MODE_CAM_ON_A_STRING, //=18,  
    MODE_REACTION, //=19, 
    MODE_FOLLOW_PED_WITH_BIND, //=20, 
    MODE_CHRIS, //21
    MODE_BEHINDBOAT, //=22, 
    MODE_PLAYER_FALLEN_WATER, //=23, 
    MODE_CAM_ON_TRAIN_ROOF, //=24, 
    MODE_CAM_RUNNING_SIDE_TRAIN, //=25,  
    MODE_BLOOD_ON_THE_TRACKS, //=26, 
    MODE_IM_THE_PASSENGER_WOOWOO, //=27, 
    MODE_SYPHON_CRIM_IN_FRONT,// =28, 
    MODE_PED_DEAD_BABY, //=29, 
    MODE_PILLOWS_PAPS, //=30, 
    MODE_LOOK_AT_CARS, //=31, 
    MODE_ARRESTCAM_ONE, //=32, 
    MODE_ARRESTCAM_TWO, //=33, 
    MODE_M16_1STPERSON, //=34, 
    MODE_SPECIAL_FIXED_FOR_SYPHON, //=35, 
    MODE_FIGHT_CAM, //=36, 
    MODE_TOP_DOWN_PED, //=37,
    MODE_LIGHTHOUSE, //=38
    ///all 1rst person run ablout run about modes now
    MODE_SNIPER_RUNABOUT, //=39, 
    MODE_ROCKETLAUNCHER_RUNABOUT, //=40,  
    MODE_1STPERSON_RUNABOUT, //=41,
    MODE_M16_1STPERSON_RUNABOUT, //=42,
    MODE_FIGHT_CAM_RUNABOUT, //=43,
    MODE_EDITOR, //=44,
    MODE_HELICANNON_1STPERSON, //= 45
    MODE_CAMERA, //46
    MODE_ATTACHCAM,  //47
    MODE_TWOPLAYER,
    MODE_TWOPLAYER_IN_CAR_AND_SHOOTING,
    MODE_TWOPLAYER_SEPARATE_CARS,   //50
    MODE_ROCKETLAUNCHER_HS, 
    MODE_ROCKETLAUNCHER_RUNABOUT_HS,
    MODE_AIMWEAPON,
    MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN,
    MODE_AIMWEAPON_FROMCAR, //55
    MODE_DW_HELI_CHASE,
    MODE_DW_CAM_MAN,
    MODE_DW_BIRDY,
    MODE_DW_PLANE_SPOTTER,
    MODE_DW_DOG_FIGHT, //60
    MODE_DW_FISH,
    MODE_DW_PLANECAM1,
    MODE_DW_PLANECAM2,
    MODE_DW_PLANECAM3,
    MODE_AIMWEAPON_ATTACHED //65
};

enum eVehicleCamMode
{
    MODE_BUMPER,
    MODE_CLOSE_EXTERNAL,
    MODE_MIDDLE_EXTERNAL,
    MODE_FAR_EXTERNAL,
    MODE_LOW_EXTERNAL,
    MODE_CINEMATIC
};

enum {FADE_OUT=0, FADE_IN};

class CCamera
{
public:
    // camMode 56 = heli chase, 14 = wheel cam
    // switchStyle 1 = smooth, 2 = cut
    virtual void                        TakeControl( CEntity *entity, eCamMode mode, int switchStyle ) = 0;
    virtual void                        TakeControl( const CVector& pos, int switchStyle ) = 0;
    virtual void                        TakeControlAttachToEntity( CEntity *target, CEntity *attach, const CVector& offset, const CVector& lookAt, float tilt, int switchStyle ) = 0;

    virtual void                        Restore() = 0;
    virtual void                        RestoreWithJumpCut() = 0;

    virtual const RwMatrix&             GetMatrix() const = 0;
    virtual void                        SetMatrix( const RwMatrix& mat ) = 0;

    virtual void                        SetCamPositionForFixedMode( const CVector& pos, const CVector& upOffset ) = 0;
    virtual void                        Find3rdPersonCamTargetVector( float distance, const CVector& gunMuzzle, const CVector& source, CVector& target ) const = 0;
    virtual float                       Find3rdPersonQuickAimPitch() const = 0;

    virtual unsigned char               GetActiveCam() const = 0;

    virtual CCam*                       GetCam( unsigned char id ) = 0;

    virtual void                        SetWidescreen( bool wide ) = 0;
    virtual bool                        GetWidescreen() const = 0;

    virtual float                       GetCarZoom() const = 0;
    virtual void                        SetCarZoom( float zoom ) = 0;

    virtual bool                        TryToStartNewCamMode( unsigned char mode ) = 0;
    virtual bool                        ConeCastCollisionResolve( const CVector& pos, const CVector& lookAt, const CVector& dest, float rad, float minDist, float& dist ) = 0;
    virtual void                        VectorTrackLinear( const CVector& to, const CVector& from, float time, bool smoothEnds ) = 0;

    virtual bool                        IsFading() const = 0;
    virtual int                         GetFadingDirection() const = 0;
    virtual void                        Fade( float fadeOutTime, int outOrIn ) = 0;
    virtual void                        SetFadeColor( unsigned char red, unsigned char green, unsigned char blue ) = 0;

    virtual float                       GetCameraRotation() const = 0;
    virtual const RwMatrix&             GetLTM() const = 0;

    virtual CEntity*                    GetTargetEntity() const = 0;

    virtual void                        SetCameraClip( bool ibjects, bool vehicles ) = 0;
    virtual unsigned char               GetCameraViewMode() const = 0;
    virtual void                        SetCameraViewMode( unsigned char mode ) = 0;

    virtual bool                        IsSphereVisible( const RwSphere& sphere ) const = 0;
};



#endif

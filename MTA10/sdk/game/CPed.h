/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPed.h
*  PURPOSE:     Ped entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PED
#define __CGAME_PED

class CTaskManager;

#include "Common.h"
#include "CPhysical.h"
#include "CWeapon.h"
#include "CPedIntelligence.h"
#include "CPedSound.h"

#include <CVector2D.h>

// forward declaration, avoid compile error
class CVehicle;
class CObject;

enum ePedPieceTypes {
    PED_PIECE_UNKNOWN = 0,

    PED_PIECE_TORSO = 3,
    PED_PIECE_ASS,
    PED_PIECE_LEFT_ARM,
    PED_PIECE_RIGHT_ARM,
    PED_PIECE_LEFT_LEG,
    PED_PIECE_RIGHT_LEG,
    PED_PIECE_HEAD
};

enum eBone { 
    BONE_PELVIS1 = 1,
    BONE_PELVIS,
    BONE_SPINE1,
    BONE_UPPERTORSO,
    BONE_NECK,
    BONE_HEAD2,
    BONE_HEAD1,
    BONE_HEAD,
    BONE_RIGHTUPPERTORSO = 21,
    BONE_RIGHTSHOULDER,
    BONE_RIGHTELBOW,
    BONE_RIGHTWRIST,
    BONE_RIGHTHAND,
    BONE_RIGHTTHUMB,
    BONE_LEFTUPPERTORSO = 31,
    BONE_LEFTSHOULDER,
    BONE_LEFTELBOW,
    BONE_LEFTWRIST,
    BONE_LEFTHAND,
    BONE_LEFTTHUMB,
    BONE_LEFTHIP = 41,
    BONE_LEFTKNEE,
    BONE_LEFTANKLE,
    BONE_LEFTFOOT,
    BONE_RIGHTHIP = 51,
    BONE_RIGHTKNEE,
    BONE_RIGHTANKLE,
    BONE_RIGHTFOOT
};

enum {
    BIKE_KNOCK_OFF_DEFAULT = 0,
    BIKE_KNOCK_OFF_NEVER,
    BIKE_KNOCK_OFF_ALWAYS_NORMAL,
};

enum {
    ATTACH_DIRECTION_FRONT = 0,
    ATTACH_DIRECTION_LEFT,
    ATTACH_DIRECTION_BACK,
    ATTACH_DIRECTION_RIGHT
};

enum eFightingStyle
{
    STYLE_STANDARD = 4,
    STYLE_BOXING,
    STYLE_KUNG_FU,
    STYLE_KNEE_HEAD,
    // various melee weapon styles
    STYLE_GRAB_KICK = 15,
    STYLE_ELBOWS = 16,
};

enum eMoveAnim
{
    MOVE_PLAYER = 54,
    MOVE_PLAYER_F,
    MOVE_PLAYER_M,
    MOVE_ROCKET,
    MOVE_ROCKET_F,
    MOVE_ROCKET_M,
    MOVE_ARMED,
    MOVE_ARMED_F,
    MOVE_ARMED_M,
    MOVE_BBBAT,
    MOVE_BBBAT_F,
    MOVE_BBBAT_M,
    MOVE_CSAW,
    MOVE_CSAW_F,
    MOVE_CSAW_M,
    MOVE_SNEAK,
    MOVE_JETPACK,
    MOVE_MAN = 118,
    MOVE_SHUFFLE,
    MOVE_OLDMAN,
    MOVE_GANG1,
    MOVE_GANG2,
    MOVE_OLDFATMAN,
    MOVE_FATMAN,
    MOVE_JOGGER,
    MOVE_DRUNKMAN,
    MOVE_BLINDMAN,
    MOVE_SWAT,
    MOVE_WOMAN,
    MOVE_SHOPPING,
    MOVE_BUSYWOMAN,
    MOVE_SEXYWOMAN,
    MOVE_PRO,
    MOVE_OLDWOMAN,
    MOVE_FATWOMAN,
    MOVE_JOGWOMAN,
    MOVE_OLDFATWOMAN,
    MOVE_SKATE
};

enum { PLAYER_PED, CIVILIAN_PED };

class CPed : public virtual CPhysical
{
public:
    virtual                             ~CPed() {};

    virtual void                        SetModelIndex( unsigned short modelId ) = 0;

    virtual unsigned int                GetPoolIndex() const = 0;

    virtual CPedIntelligence*           GetPedIntelligence() = 0;
    virtual CPedSound*                  GetPedSound() = 0;

    virtual unsigned char               GetType() const = 0;
    virtual float                       GetHealth() const = 0;
    virtual float                       GetArmor() const = 0;
    virtual CVehicle*                   GetVehicle() const = 0;
    virtual float                       GetCurrentRotation() const = 0;
    virtual float                       GetTargetRotation() const = 0;
    virtual eWeaponSlot                 GetCurrentWeaponSlot() const = 0;
    virtual CTask*                      GetPrimaryTask() const = 0;
    virtual eFightingStyle              GetFightingStyle() const = 0;
    virtual unsigned char               GetRunState() const = 0;
    virtual CEntity*                    GetContactEntity() const = 0;
    virtual CEntity*                    GetTargetedEntity() const = 0;
    virtual bool                        GetCanBeShotInVehicle() const = 0;
    virtual bool                        GetTestForShotInVehicle() const = 0;
    virtual unsigned int                GetFootBlood() const = 0;
    virtual unsigned char               GetOccupiedSeat() const = 0;
    virtual bool                        GetStayInSamePlace() const = 0;
    virtual unsigned char               GetCantBeKnockedOffBike() const = 0;

    virtual bool                        IsStealthAiming() const = 0;
    virtual bool                        IsDucking() const = 0;
    virtual bool                        IsStanding() const = 0;
    virtual bool                        IsInWater() const = 0;
    virtual bool                        IsDying() const = 0;
    virtual bool                        IsDead() const = 0;
    virtual bool                        IsOnFire() const = 0;
    virtual bool                        IsWearingGoggles() const = 0;
    virtual bool                        IsGettingIntoVehicle() const = 0;
    virtual bool                        IsEnteringVehicle() const = 0;
    virtual bool                        IsBeingJacked() const = 0;
    virtual bool                        IsLeavingVehicle() const = 0;
    virtual bool                        IsGettingOutOfVehicle() const = 0;
    virtual bool                        IsPlayingAnimation( const char *name ) const = 0;

    virtual void                        SetType( unsigned char type ) = 0;
    virtual void                        SetHealth( float fHealth ) = 0;
    virtual void                        SetArmor( float fArmor ) = 0;
    virtual void                        SetCurrentRotation( float rot ) = 0;
    virtual void                        SetTargetRotation( float rot ) = 0;
    virtual void                        SetStealthAiming( bool enable ) = 0;
    virtual void                        SetAnimationProgress( const char *name, float progress ) = 0;
    virtual void                        SetCurrentWeaponSlot( eWeaponSlot weaponSlot ) = 0;
    virtual void                        SetIsStanding( bool bStanding ) = 0;
    virtual void                        SetDucking( bool bDuck ) = 0;
    virtual void                        SetFightingStyle( eFightingStyle style, unsigned char extra = 6 ) = 0;
    virtual void                        SetTargetedEntity( CEntity *entity ) = 0;
    virtual void                        SetCanBeShotInVehicle( bool bShot ) = 0;
    virtual void                        SetTestForShotInVehicle( bool bTest ) = 0;
    virtual void                        SetFootBlood( unsigned int uiFootBlood ) = 0;
    virtual void                        SetOnFire( bool bOnFire ) = 0;
    virtual void                        SetGogglesState( bool bIsWearingThem ) = 0;
    virtual void                        SetOccupiedSeat( unsigned char seat ) = 0;
    virtual void                        SetStayInSamePlace( bool bStay ) = 0;
    virtual void                        SetCantBeKnockedOffBike( unsigned char val ) = 0;

    virtual void                        AttachPedToBike( CEntity *entity, const CVector& offset, unsigned short sUnk, float fUnk, float fUnk2, eWeaponType weaponType ) = 0;
    virtual void                        DetachPedFromEntity() = 0;

    virtual void                        QuitEnteringCar( CVehicle * vehicle, int iSeat, bool bUnknown ) = 0;
    
    virtual bool                        CanSeeEntity( CEntity *entity, float distance ) const = 0;
    virtual void                        Respawn( const CVector& pos, bool camCut ) = 0;
    virtual bool                        AddProjectile( eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target ) = 0;

    virtual CWeapon*                    GiveWeapon( eWeaponType weaponType, unsigned int uiAmmo ) = 0;
    virtual CWeapon*                    GetWeapon( eWeaponSlot weaponSlot ) = 0;
    virtual CWeapon*                    GetWeapon( eWeaponType weaponType ) = 0;
    virtual void                        RemoveWeaponModel( unsigned short model ) = 0;
    virtual void                        ClearWeapons() = 0;
    virtual void                        ClearWeapon( eWeaponType weaponType ) = 0;

    virtual void                        GetBonePosition( eBone bone, CVector& pos ) const = 0;
    virtual void                        GetTransformedBonePosition( eBone bone, CVector& pos ) const = 0;

    virtual void                        SetClothesTextureAndModel( const char *tex, const char *model, short txd ) = 0;
    virtual void                        RebuildPlayer() = 0;

    virtual void                        RemoveBodyPart( int i, char c ) = 0;

    virtual void                        GetVoice( short *psVoiceType, short *psVoiceID ) const = 0;
    virtual void                        GetVoice( const char **pszVoiceType, const char **pszVoice ) const = 0;
    virtual void                        SetVoice( short sVoiceType, short sVoiceID ) = 0;
    virtual void                        SetVoice( const char *szVoiceType, const char *szVoice ) = 0;
};

#endif

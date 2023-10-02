/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponInfoSA.h
*  PURPOSE:     Header file for weapon type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Chris McArthur <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WEAPONINFO
#define __CGAMESA_WEAPONINFO

#include <game/CWeaponInfo.h>
#include <CVector.h>
#include "Common.h"

class CWeaponInfoSAInterface // 112 byte long class
{
public:
    eFireType   m_eFireType;        // type - instant hit (e.g. pistol), projectile (e.g. rocket launcher), area effect (e.g. flame thrower)

    float       m_fTargetRange;     // max targeting range
    float       m_fWeaponRange;     // absolute gun range / default melee attack range
    int         m_modelId;          // modelinfo id
    int         m_modelId2;         // second modelinfo id
    
    eWeaponSlot m_nWeaponSlot;
    int         m_nFlags;           // flags defining characteristics

    // instead of storing pointers directly to anims, use anim association groups
    // NOTE: this is used for stealth kill anims for melee weapons
    unsigned int    m_animGroup;

    //////////////////////////////////
    // Gun Data
    /////////////////////////////////
    short       m_nAmmo;                // ammo in one clip
    short       m_nDamage;              // damage inflicted per hit
    CVector     m_vecFireOffset;        // offset from weapon origin to projectile starting point
    
    // skill settings
    eWeaponSkill    m_SkillLevel;       // what's the skill level of this weapontype
    int         m_nReqStatLevel;        // what stat level is required for this skill level
    float       m_fAccuracy;            // modify accuracy of weapon
    float       m_fMoveSpeed;           // how fast can move with weapon

    // anim timings
    float       m_animLoopStart;        // start of animation loop
    float       m_animLoopEnd;          // end of animation loop
    float       m_animFireTime;         // time in animation when weapon should be fired

    float       m_anim2LoopStart;       // start of animation2 loop
    float       m_anim2LoopEnd;         // end of animation2 loop
    float       m_anim2FireTime;        // time in animation2 when weapon should be fired

    float       m_animBreakoutTime;     // time after which player can break out of attack and run off
    
    // projectile/area effect specific info
    float       m_fSpeed;               // speed of projectile
    float       m_fRadius;              // radius affected
    float       m_fLifeSpan;            // time taken for shot to dissipate
    float       m_fSpread;              // angle inside which shots are created
    
    short       m_nAimOffsetIndex;      // index into array of aiming offsets

    //////////////////////////////////
    // Melee Data
    /////////////////////////////////
    unsigned char   m_defaultCombo;         // base combo for this melee weapon
    unsigned char   m_nCombosAvailable;     // how many further combos are available
};

class CWeaponInfoSA : public CWeaponInfo
{
public:
    // constructor
    CWeaponInfoSA( CWeaponInfoSAInterface *weapon, eWeaponType type )
    {
        m_interface = weapon;
        m_type = type;
    }

    // stolen from R*'s CPedIK :)
    void                        SetFlag( unsigned int flag )                        { m_interface->m_nFlags |= flag; }
    void                        ClearFlag( unsigned int flag )                      { m_interface->m_nFlags &= ~flag; }
    bool                        IsFlagSet( unsigned int flag )                      { return ((m_interface->m_nFlags & flag) > 0 ? 1 : 0); }  

    CWeaponInfoSAInterface*     GetInterface()                                      { return m_interface; }
    const CWeaponInfoSAInterface*   GetInterface() const                            { return m_interface; }

    eWeaponModel                GetModel() const                                    { return (eWeaponModel)m_interface->m_modelId; }

    float                       GetWeaponRange() const                              { return GetInterface()->m_fWeaponRange; }
    void                        SetWeaponRange( float fRange )                      { GetInterface()->m_fWeaponRange = fRange; }

    float                       GetTargetRange() const                              { return GetInterface()->m_fTargetRange; }
    void                        SetTargetRange( float fRange )                      { GetInterface()->m_fTargetRange = fRange; }

    const CVector&              GetFireOffset() const                               { return GetInterface()->m_vecFireOffset; }
    void                        SetFireOffset( const CVector& offset )              { GetInterface()->m_vecFireOffset = offset; }

    short                       GetDamagePerHit() const                             { return GetInterface()->m_nDamage; }
    void                        SetDamagePerHit( short sDamagePerHit )              { GetInterface()->m_nDamage = sDamagePerHit; }

    float                       GetAccuracy() const                                 { return m_interface->m_fAccuracy; }
    void                        SetAccuracy( float fAccuracy )                      { m_interface->m_fAccuracy = fAccuracy; }

    // projectile/areaeffect only
    float                       GetFiringSpeed() const                              { return GetInterface()->m_fSpeed; }
    void                        SetFiringSpeed( float fFiringSpeed )                { GetInterface()->m_fSpeed = fFiringSpeed; }

    // area effect only
    float                       GetRadius() const                                   { return GetInterface()->m_fRadius; }
    void                        SetRadius( float fRadius )                          { GetInterface()->m_fRadius = fRadius; }

    float                       GetLifeSpan() const                                 { return GetInterface()->m_fLifeSpan; }
    void                        SetLifeSpan( float fLifeSpan )                      { GetInterface()->m_fLifeSpan = fLifeSpan; }

    float                       GetSpread() const                                   { return GetInterface()->m_fSpread; }
    void                        SetSpread( float fSpread )                          { GetInterface()->m_fSpread = fSpread; }

    float                       GetAnimBreakoutTime() const                         { return GetInterface()->m_animBreakoutTime; }
    void                        SetAnimBreakoutTime( float fBreakoutTime )          { GetInterface()->m_animBreakoutTime = fBreakoutTime; }

    eWeaponSlot                 GetSlot() const                                     { return (eWeaponSlot)GetInterface()->m_nWeaponSlot; }
    void                        SetSlot( eWeaponSlot dwSlot )                       { GetInterface()->m_nWeaponSlot = (eWeaponSlot)dwSlot; }

    eWeaponSkill                GetSkill() const                                    { return GetInterface()->m_SkillLevel; }
    void                        SetSkill( eWeaponSkill weaponSkill )                { GetInterface ()->m_SkillLevel = weaponSkill; }

    float                       GetRequiredStatLevel() const                        { return (float)GetInterface()->m_nReqStatLevel; }
    void                        SetRequiredStatLevel( float fStatLevel )            { GetInterface()->m_nReqStatLevel = (int)fStatLevel; }

    eFireType                   GetFireType() const                                 { return GetInterface()->m_eFireType; }

private:
    eWeaponType                 m_type;
    CWeaponInfoSAInterface*     m_interface;
};

#endif

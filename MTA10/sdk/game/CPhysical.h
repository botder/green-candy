/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPhysical.h
*  PURPOSE:     Physical entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PHYSICAL
#define __CGAME_PHYSICAL

#include "CEntity.h"

class CPhysical : public virtual CEntity
{
public:
    virtual             ~CPhysical()    {};

    virtual void        GetMoveSpeed( CVector& moveSpeed ) const = 0;
    virtual void        GetTurnSpeed( CVector& turnSpeed ) const = 0;
    virtual void        SetMoveSpeed( const CVector& moveSpeed ) = 0;
    virtual void        SetTurnSpeed( const CVector& turnSpeed ) = 0;

    virtual float       GetMass() const = 0;
    virtual void        SetMass( float fMass ) = 0;
    virtual float       GetTurnMass() const = 0;
    virtual void        SetTurnMass( float fTurnMass ) = 0;
    virtual float       GetElasticity() const = 0;
    virtual void        SetElasticity( float fElasticity ) = 0;
    virtual float       GetBuoyancyConstant() const = 0;
    virtual void        SetBuoyancyConstant( float fBuoyancyConstant ) = 0;

    virtual void        ProcessCollision() = 0;

    virtual float       GetDamageImpulseMagnitude() const = 0;
    virtual void        SetDamageImpulseMagnitude( float fMagnitude ) = 0;
    virtual CEntity*    GetDamageEntity() const = 0;
    virtual void        SetDamageEntity( CEntity *pEntity ) = 0;
    virtual void        ResetLastDamage() = 0;

    virtual CEntity*    GetAttachedEntity() const = 0;
    virtual void        AttachTo( CPhysical& Entity, const CVector& vecPosition, const CVector& vecRotation ) = 0;
    virtual void        DetachFrom( float fUnkX, float fUnkY, float fUnkZ, bool bUnk ) = 0;
    virtual void        GetAttachedOffsets( CVector& pos, CVector& rot ) const = 0;
    virtual void        SetAttachedOffsets( const CVector& pos, const CVector& rot ) = 0;

    virtual void        GetImmunities( bool& bNoClip, bool& bFrozen, bool& bBulletProof, bool& bFlameProof, bool& bUnk, bool& bUnk2, bool& bCollisionProof, bool& bExplosionProof ) const = 0;

    virtual float       GetLighting() const = 0;
    virtual void        SetLighting( float fLighting ) = 0;
};

#endif

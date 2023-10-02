/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPhysicalSA.h
*  PURPOSE:     Header file for physical object entity base class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PHYSICAL
#define __CGAMESA_PHYSICAL

#include <game/CPhysical.h>
#include "CEntitySA.h"
#include <CVector.h>

#define FUNC_GetMoveSpeed                       0x404460
#define FUNC_GetTurnSpeed                       0x470030
#define FUNC_ProcessCollision                   0x54DFB0
#define FUNC_AttachEntityToEntity               0x54D570
#define FUNC_DetatchEntityFromEntity            0x5442F0

#define MAX_COLL_RECORDS                        6

class CPhysicalSAInterface : public CEntitySAInterface // begin +56 (256 bytes total?)
{
public:
                                    CPhysicalSAInterface( void );
                                    ~CPhysicalSAInterface( void );

    void __thiscall                 Link( void );
    void __thiscall                 Unlink( void );

    virtual void* __thiscall        ProcessEntityCollisions( CEntitySAInterface *entity ) = 0;

    float                   m_unk13;                    // 56

    BYTE                    m_pad2[3];                  // 60
    unsigned char           m_numImmunities;            // 63
    unsigned int            physicalFlags;              // 64

    CVector                 m_vecLinearVelocity;        // 68
    CVector                 m_spin;                     // 80
    CVector                 m_vecUnk;                   // 92
    CVector                 m_vecUnk2;                  // 104
    CVector                 m_vecUnk3;                  // 116
    CVector                 m_vecUnk4;                  // 128
    float                   m_mass;                     // 140
    float                   m_turnMass;                 // 144
    float                   m_massUnk;                  // 148
    float                   m_airResistance;            // 152

    float                   m_elasticity;               // 156
    float                   m_buoyancyConstant;         // 160
    CVector                 m_centerOfMass;             // 164

    typedef CPtrNodeDoubleSA <CPhysicalSAInterface> physicalLink_t;

    DWORD                   m_unk2;                     // 176
    physicalLink_t*         m_link;                     // 180

    unsigned char           m_complexStatus;            // 184, used for hydraulics
    unsigned char           m_numCollRecords;           // 185
    unsigned char           m_unk4;                     // 186
    unsigned char           m_unk5;                     // 187

    float                   m_collRecords[MAX_COLL_RECORDS];    // 188

    float                   m_distanceTravelled;        // 212
    float                   m_damageImpulseMagnitude;   // 216
    CEntitySAInterface*     m_damageEntity;             // 220

    CVector                 m_vecUnk5;                  // 224
    CVector                 m_vecUnk6;                  // 236

    unsigned short          m_unk6;                     // 248
    unsigned short          m_unk7;                     // 250

    CEntitySAInterface*     m_pAttachedEntity;          // 252
    CVector                 m_attachOffset;             // 256
    CVector                 m_attachRotation;           // 268

    unsigned int            m_unk10;                    // 280
    unsigned int            m_unk11;                    // 284
    unsigned int            m_unk12;                    // 288
    unsigned int            m_unk9;                     // 292
    unsigned int            m_unk8;                     // 296

    float                   m_fLighting;                // 300, col lighting? CPhysical::GetLightingFromCol
    float                   m_fLighting2;               // 304, added to col lighting in CPhysical::GetTotalLighting
    float                   m_fLighting3;               // 308
};

void Physical_Init();
void Physical_Shutdown();

class CPhysicalSA : public virtual CPhysical, public CEntitySA
{
public:
    inline CPhysicalSAInterface* GetInterface()                                 { return (CPhysicalSAInterface*)m_pInterface; }
    inline const CPhysicalSAInterface* GetInterface() const                     { return (const CPhysicalSAInterface*)m_pInterface; }

    void            GetMoveSpeed( CVector& moveSpeed ) const                    { moveSpeed = GetInterface()->m_vecLinearVelocity; }
    void            GetTurnSpeed( CVector& turnSpeed ) const                    { turnSpeed = GetInterface()->m_spin; }
    void            SetMoveSpeed( const CVector& moveSpeed )                    { GetInterface()->m_vecLinearVelocity = moveSpeed; }
    void            SetTurnSpeed( const CVector& turnSpeed )                    { GetInterface()->m_spin = turnSpeed; }

    float           GetMass() const                                             { return GetInterface()->m_mass; }
    void            SetMass( float mass )                                       { GetInterface()->m_mass = mass; }
    float           GetTurnMass() const                                         { return GetInterface()->m_turnMass; }
    void            SetTurnMass( float mass )                                   { GetInterface()->m_turnMass = mass; }
    float           GetElasticity() const                                       { return GetInterface()->m_elasticity; }
    void            SetElasticity( float elas )                                 { GetInterface()->m_elasticity = elas; }
    float           GetBuoyancyConstant() const                                 { return GetInterface()->m_buoyancyConstant; }
    void            SetBuoyancyConstant( float cnst )                           { GetInterface()->m_buoyancyConstant = cnst; }

    void            ProcessCollision()                                          { GetInterface()->ProcessCollision(); }

    float           GetDamageImpulseMagnitude() const                           { return GetInterface()->m_damageImpulseMagnitude; }
    void            SetDamageImpulseMagnitude( float fMagnitude )               { GetInterface()->m_damageImpulseMagnitude = fMagnitude; }
    CEntity*        GetDamageEntity() const;
    void            SetDamageEntity( CEntity *entity );
    void            ResetLastDamage();

    CEntity*        GetAttachedEntity() const;
    void            AttachTo( CPhysical& Entity, const CVector& pos, const CVector& rot );
    void            DetachFrom( float fUnkX, float fUnkY, float fUnkZ, bool bUnk );
    void            GetAttachedOffsets( CVector& pos, CVector& rot ) const;
    void            SetAttachedOffsets( const CVector& pos, const CVector& rot );

    virtual bool    InternalAttachTo( CEntitySAInterface *entity, const CVector& pos, const CVector& rot );

    void            GetImmunities( bool& bNoClip, bool& bFrozen, bool& bBulletProof, bool& bFlameProof, bool& bUnk, bool& bUnk2, bool& bCollisionProof, bool& bExplosionProof ) const;

    float           GetLighting() const                                         { return GetInterface()->m_fLighting; }
    void            SetLighting( float fLighting )                              { GetInterface()->m_fLighting = fLighting; }
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientProjectile.h
*  PURPOSE:     Projectile entity class header
*  DEVELOPERS:  Jax <>
*               Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

class CClientProjectile;

#ifndef __CCLIENTPROJECTILE_H
#define __CCLIENTPROJECTILE_H

#include "CClientEntity.h"
#include "CClientCommon.h"

#define LUACLASS_PROJECTILE     48

class CProjectile;
class CProjectileInfo;
class CClientEntity;
class CVector;
enum eWeaponType;
class CClientProjectileManager;
class CClientPed;
class CClientVehicle;

class CProjectileInitiateData
{
public:
    inline          CProjectileInitiateData ( void )
    {
        pvecPosition = NULL; pvecRotation = NULL;
        pvecVelocity = NULL; usModel = 0;
    }
    inline          ~CProjectileInitiateData ( void )
    {
        if ( pvecPosition ) delete pvecPosition;
        if ( pvecRotation ) delete pvecRotation;
        if ( pvecVelocity ) delete pvecVelocity;
    }
    CVector * pvecPosition;
    CVector * pvecRotation;    
    CVector * pvecVelocity;    
    unsigned short usModel;
};

class CClientProjectile : public CClientEntity
{
    friend class CClientProjectileManager;
    friend class CClientPed;
    friend class CClientVehicle;
public:
                                        CClientProjectile( class CClientManager *manager,
                                                           CProjectile *proj,
                                                           CProjectileInfo *info,
                                                           CClientEntity *creator,
                                                           CClientEntity *target,
                                                           eWeaponType weaponType,
                                                           CVector *origin,
                                                           CVector *vecTarget,
                                                           float force,
                                                           bool local,
                                                           lua_State *L );
                                        ~CClientProjectile();

    eClientEntityType                   GetType() const                                 { return CCLIENTPROJECTILE; }
    inline CEntity*                     GetGameEntity()                                 { return m_pProjectile; }
    inline const CEntity*               GetGameEntity() const                           { return m_pProjectile; }
    void                                Unlink();


    void                                DoPulse();
    void                                Initiate( CVector *pvecPosition, CVector *pvecRotation, CVector *pvecVelocity, unsigned short usModel );
    void                                Destroy();

    bool                                IsActive() const;
    bool                                GetMatrix( RwMatrix& matrix ) const;
    bool                                SetMatrix( const RwMatrix& matrix );
    void                                GetPosition( CVector& pos ) const;
    void                                SetPosition( const CVector& pos );
    void                                GetRotation( CVector& rot ) const;
    void                                GetRotationDegrees( CVector& rot ) const;
    void                                SetRotation( const CVector& rot );
    void                                SetRotationDegrees( const CVector& rot );
    void                                GetVelocity( CVector& vel ) const;
    void                                SetVelocity( CVector& vel );
    void                                SetModel( unsigned short usModel );

    inline CClientEntity*               GetCreator()                                    { return m_pCreator; }
    inline CClientEntity*               GetTargetEntity()                               { return m_pTarget; }
    inline eWeaponType                  GetWeaponType() const                           { return m_weaponType; }
    inline const CVector*               GetOrigin() const                               { return m_pvecOrigin; }
    inline const CVector*               GetTarget() const                               { return m_pvecTarget; }
    inline float                        GetForce() const                                { return m_fForce; }
    inline bool                         IsLocal() const                                 { return m_bLocal; }
    
protected:
    CClientProjectileManager*           m_pProjectileManager;
    bool                                m_bLinked;

    CProjectile*                        m_pProjectile;
    CProjectileInfo*                    m_pProjectileInfo;

    CClientEntity*                      m_pCreator;
    CClientEntity*                      m_pTarget;
    eWeaponType                         m_weaponType;
    CVector*                            m_pvecOrigin;
    CVector*                            m_pvecTarget;
    float                               m_fForce;
    bool                                m_bLocal;
    long long                           m_llCreationTime;

    bool                                m_bInitiate;
    CProjectileInitiateData*            m_pInitiateData;
};

#endif
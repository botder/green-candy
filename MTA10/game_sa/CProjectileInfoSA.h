/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileInfoSA.h
*  PURPOSE:     Header file for projectile type information class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PROJECTILEINFO
#define __CGAMESA_PROJECTILEINFO

#include <game/CProjectileInfo.h>
#include "CProjectileSA.h"

#define PROJECTILE_COUNT                    32
#define PROJECTILE_INFO_COUNT               32

#define FUNC_RemoveAllProjectiles           0x5C69D0
#define FUNC_RemoveProjectile               0x7388F0 //##SA##
#define FUNC_AddProjectile                  0x737C80 //##SA##

#define ARRAY_CProjectile                   0xC89110 //##SA##
#define ARRAY_CProjectileInfo               0xC891A8 //##SA##

#define VTBL_CProjectile 0x867030

class CProjectileInfoSAInterface
{
public:
    eWeaponType                     m_type;         // 0
    CEntitySAInterface*             m_owner;        // 4
    CEntitySAInterface*             m_target;       // 8
    unsigned int                    m_count;        // 12
    bool                            m_active;       // 16
    BYTE                            m_pad[3];       // 17
    CVector                         m_oldPos;       // 20
    unsigned int                    m_unk;          // 32, particle effect ptr
};

class CProjectileInfoSA : public CProjectileInfo
{
public:
    CProjectileInfoSA()
    {
        for ( unsigned int i = 0; i < PROJECTILE_INFO_COUNT; i++ )
            m_info[i] = new CProjectileInfoSA((CProjectileInfoSAInterface*)(ARRAY_CProjectileInfo + i * sizeof(CProjectileInfoSAInterface)));
    }

    CProjectileInfoSA( CProjectileInfoSAInterface *proj )
    {
        m_interface = proj;
    }


    void                    RemoveAllProjectiles();
    void                    RemoveProjectile( CProjectileInfo *info, CProjectile *proj );
    CProjectile*            GetProjectile( unsigned short id );
    CProjectile*            GetProjectile( void *projectilePointer );
    CProjectileInfo*        GetProjectileInfo( void *infoInt );
    CProjectileInfo*        GetProjectileInfo( unsigned short index );
    CProjectileInfo*        GetNextFreeProjectileInfo();
    bool                    AddProjectile( CEntity *creator, eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target );

    CEntity*                GetTarget() const;
    void                    SetTarget( CEntity *entity );

    bool                    IsActive() const;

private:
    CProjectileInfoSA*              m_info[PROJECTILE_INFO_COUNT];
    CProjectileInfoSAInterface*     m_interface;
};

#endif

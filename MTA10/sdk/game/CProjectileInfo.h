/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CProjectileInfo.h
*  PURPOSE:     Projectile entity information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PROJECTILEINFO
#define __CGAME_PROJECTILEINFO

class CProjectile;
class CPlayerPed;
class CProjectileInfo
{
public:
    virtual void                    RemoveAllProjectiles() = 0;
    virtual void                    RemoveProjectile( CProjectileInfo *info, CProjectile *proj ) = 0;
    virtual CProjectile*            GetProjectile( unsigned short id ) = 0;
    virtual CProjectile*            GetProjectile( void *infoInt ) = 0;
    virtual CProjectileInfo*        GetProjectileInfo( unsigned short index ) = 0;
    virtual CProjectileInfo*        GetNextFreeProjectileInfo() = 0;
    virtual bool                    AddProjectile( CEntity *creator, eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target ) = 0;

    virtual CEntity*                GetTarget() const = 0;
    virtual void                    SetTarget( CEntity *entity ) = 0;

    virtual bool                    IsActive() const = 0;
};

#endif

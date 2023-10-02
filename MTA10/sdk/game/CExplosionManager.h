/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CExplosionManager.h
*  PURPOSE:     Explosion manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_EXPLOSIONMANAGER
#define __CGAME_EXPLOSIONMANAGER

#include "CExplosion.h"
#include "Common.h"

class CExplosionManager
{
public:
    virtual CExplosion*         AddExplosion( CEntity *entity, CEntity *owner, eExplosionType type, const CVector& pos, unsigned int actDelay = 0, bool makeSound = true, float camShake = -1.0f, bool noDamage = false ) = 0;
    virtual void                RemoveAllExplosionsInArea( const CVector& pos, float radius ) = 0;
    virtual void                RemoveAllExplosions() = 0;
    virtual CExplosion*         GetExplosion( unsigned int id ) = 0;
    virtual CExplosion*         FindFreeExplosion() = 0;
};

#endif

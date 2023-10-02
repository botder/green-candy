/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CExplosionManagerSA.h
*  PURPOSE:     Header file for explosion manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_EXPLOSIONMANAGER
#define __CGAMESA_EXPLOSIONMANAGER

#include <game/CExplosionManager.h>
#include "CExplosionSA.h"

#define FUNC_CExplosion_AddExplosion    0x736A50 // ##SA##

#define ARRAY_Explosions                0xC88950

#define MAX_EXPLOSIONS                  48

class CExplosionManagerSA : public CExplosionManager
{
public:
                        CExplosionManagerSA();
                        ~CExplosionManagerSA();

    CExplosion*         AddExplosion( CEntity *entity, CEntity *owner, eExplosionType type, const CVector& pos, unsigned int actDelay = 0, bool makeSound = true, float camShake = -1.0f, bool noDamage = false );
    void                RemoveAllExplosionsInArea( const CVector& pos, float radius );
    void                RemoveAllExplosions();
    CExplosion*         GetExplosion( unsigned int id );
    CExplosion*         FindFreeExplosion();

private:
    CExplosionSA*       m_explosions[MAX_EXPLOSIONS];
};

#endif

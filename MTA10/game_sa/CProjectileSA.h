/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileSA.h
*  PURPOSE:     Header file for projectile entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PROJECTILE
#define __CGAMESA_PROJECTILE

#include <game/CProjectile.h>
#include "CObjectSA.h"

class CProjectileSAInterface : public CObjectSAInterface // entirely inherited from CObject
{
public:

};

class CProjectileSA : public virtual CProjectile, public CObjectSA
{
public:
                                        CProjectileSA( unsigned short modelId );
                                        CProjectileSA( class CProjectileSAInterface *proj );
                                        ~CProjectileSA();

    inline CProjectileSAInterface*      GetInterface()                                  { return (CProjectileSAInterface*)m_pInterface; }
    inline const CProjectileSAInterface*    GetInterface() const                        { return (const CProjectileSAInterface*)m_pInterface; }
    
    void                                SetProjectileInfo( CProjectileInfo *info )      { m_info = info; }

private:
    class CProjectileInfo*              m_info;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CProjectileSA.cpp
*  PURPOSE:     Projectile entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CProjectileSA::CProjectileSA( CProjectileSAInterface *proj ) : CObjectSA( proj )
{
    m_info = NULL;
    m_doNotRemoveFromGame = true;

    BOOL_FLAG( proj->m_entityFlags, ENTITY_PROC_OBJECT, false );  // removal from world prevention (?)
}              

CProjectileSA::~CProjectileSA()
{
    DEBUG_TRACE("CProjectileSA::~CProjectileSA()");
    pGame->GetProjectileInfo()->RemoveProjectile( m_info, this );
}
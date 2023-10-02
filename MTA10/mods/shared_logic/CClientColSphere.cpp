/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColSphere.cpp
*  PURPOSE:     Sphere-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CClientColSphere::CClientColSphere( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition, float fRadius ) : CClientColShape( pManager, ID, L, system )
{
    m_vecPosition = vecPosition;
    m_fRadius = fRadius;
    UpdateSpatialData();
}

bool CClientColSphere::DoHitDetection( const CVector& vecNowPosition, float fRadius )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint3D( vecNowPosition, m_vecPosition, fRadius + m_fRadius );
}

CSphere CClientColSphere::GetWorldBoundingSphere()
{
    return CSphere( m_vecPosition, m_fRadius );
}
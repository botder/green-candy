/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.cpp
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

CClientColCircle::CClientColCircle( CClientManager *manager, ElementID ID, lua_State *L, bool system, const CVector& pos, float fRadius ) : CClientColShape( manager, ID, L, system )
{
    m_vecPosition = pos;
    m_fRadius = fRadius;
    UpdateSpatialData();
}

bool CClientColCircle::DoHitDetection( const CVector& pos, float fRadius )
{
    // Do a simple distance check between now position and our position 
    return IsPointNearPoint2D( pos, m_vecPosition, fRadius + m_fRadius );
}

CSphere CClientColCircle::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = m_fRadius;
    return sphere;
}
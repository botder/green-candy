/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColRectangle.cpp
*  PURPOSE:     Rectangle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CClientColRectangle::CClientColRectangle( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition, const CVector2D& vecSize ) : CClientColShape( pManager, ID, L, system )
{
    m_pManager = pManager;
    m_vecPosition = vecPosition;
    m_vecSize = vecSize;

    UpdateSpatialData();
}

bool CClientColRectangle::DoHitDetection( const CVector& vecNowPosition, float fRadius )
{
    // FIXME: What about radius?

    // See if the now position is within our cube
    return ( vecNowPosition.fX >= m_vecPosition.fX &&
             vecNowPosition.fX <= m_vecPosition.fX + m_vecSize.fX &&
             vecNowPosition.fY >= m_vecPosition.fY &&
             vecNowPosition.fY <= m_vecPosition.fY + m_vecSize.fY );
}

CSphere CClientColRectangle::GetWorldBoundingSphere()
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX + m_vecSize.fX * 0.5f;
    sphere.vecPosition.fY = m_vecPosition.fY + m_vecSize.fY * 0.5f;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = Max ( m_vecSize.fX, m_vecSize.fY ) * 0.5f;
    return sphere;
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColPolygon.cpp
*  PURPOSE:     Polygon-shaped collision entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CClientColPolygon::CClientColPolygon( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition ) : CClientColShape( pManager, ID, L, system )
{
    m_pManager = pManager;
    m_vecPosition = vecPosition;

    // That's only to speed up things by not checking the polygon collision,
    // if the point is not even in the bounds
    m_fRadius = 0.0f;
}

bool CClientColPolygon::DoHitDetection( const CVector& vecNowPosition, float fRadius )
{
    if ( !IsInBounds ( vecNowPosition ) )
        return false;

    bool collides = false;

    int j = m_Points.size() - 1;

    float x = vecNowPosition.fX;
    float y = vecNowPosition.fY;

    for ( unsigned int i = 0; i < m_Points.size(); i++ )
    {
        CVector2D vecPi = m_Points[i];
        CVector2D vecPj = m_Points[j];

        if ( vecPi.fY < y && vecPj.fY >= y || vecPj.fY < y && vecPi.fY >= y )
        {
            if ( vecPi.fX + ( y - vecPi.fY ) / ( vecPj.fY - vecPi.fY ) * ( vecPj.fX - vecPi.fX ) < x )
            {
                collides = !collides;
            }
        }

        j = i;
    }

    return collides;
}


bool CClientColPolygon::ReadSpecialData ( void )
{
    int iTemp;
    if ( GetCustomDataInt ( "dimension", iTemp, true ) )
        m_usDimension = static_cast < unsigned short > ( iTemp );

    return true;
}


void CClientColPolygon::SetPosition ( const CVector& vecPosition )
{
    CVector vecDifference = m_vecPosition - vecPosition;

    for ( unsigned int i = 0; i < m_Points.size(); i++ )
    {
        m_Points[i].fX -= vecDifference.fX;
        m_Points[i].fY -= vecDifference.fY;
    }

    m_vecPosition = vecPosition;
    UpdateSpatialData ();
    // Add queued collider refresh for v1.1
}

void CClientColPolygon::AddPoint ( CVector2D vecPoint )
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );

    if ( fDist > m_fRadius )
    {
        m_fRadius = fDist;
        SizeChanged ();
    }

    m_Points.push_back ( vecPoint );
}

bool CClientColPolygon::IsInBounds ( CVector vecPoint )
{
    float fDistanceX = vecPoint.fX - m_vecPosition.fX;
    float fDistanceY = vecPoint.fY - m_vecPosition.fY;

    float fDist = sqrt ( fDistanceX * fDistanceX + fDistanceY * fDistanceY );

    return fDist <= m_fRadius;
}


CSphere CClientColPolygon::GetWorldBoundingSphere ( void )
{
    CSphere sphere;
    sphere.vecPosition.fX = m_vecPosition.fX;
    sphere.vecPosition.fY = m_vecPosition.fY;
    sphere.vecPosition.fZ = SPATIAL_2D_Z;
    sphere.fRadius        = m_fRadius;
    return sphere;
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWaterSA.cpp
*  PURPOSE:     Control the lakes and seas
*  DEVELOPERS:  arc_
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CWaterManagerSA* g_pWaterManager;

// -----------------------------------------------------
// Vertices

unsigned short CWaterVertexSA::GetID() const
{
    if ( !m_pInterface )
        return ~0;

    return (unsigned short)( m_pInterface - g_pWaterManager->m_VertexPool );
}

void CWaterVertexSA::GetPosition( CVector& vec ) const
{
    vec.fX = (float)m_pInterface->m_sX;
    vec.fY = (float)m_pInterface->m_sY;
    vec.fZ = m_pInterface->m_fZ;
}

bool CWaterVertexSA::SetPosition( const CVector& vec, void *pChangeSource )
{
    if ( pChangeSource )
        g_pWaterManager->AddChange( pChangeSource, this, new CWaterChangeVertexMove( this ) );
    
    m_pInterface->m_sX = ((short)vec.fX) & ~1;
    m_pInterface->m_sY = ((short)vec.fY) & ~1;
    m_pInterface->m_fZ = vec.fZ;
    return true;
}

// -----------------------------------------------------
// Polygons

void CWaterQuadSA::SetInterface( CWaterPolySAInterface *intf )
{
    m_pInterface = intf;
    m_wID = (unsigned short)( intf - (CWaterPolySAInterface*)&g_pWaterManager->m_QuadPool[0] );
}

void CWaterTriangleSA::SetInterface( CWaterPolySAInterface *intf )
{
    m_pInterface = intf;
    m_wID = (unsigned short)( intf - (CWaterPolySAInterface*)&g_pWaterManager->m_TrianglePool[0] );
}

CWaterVertex* CWaterPolySA::GetVertex( unsigned int index )
{
    if ( index >= GetNumVertices() )
        return NULL;

    return &g_pWaterManager->m_Vertices[
        GetType() == WATER_POLY_QUAD ? ((CWaterQuadSA*)this)->GetInterface()->m_wVertexIDs[index]
                                 : ((CWaterTriangleSA*)this)->GetInterface()->m_wVertexIDs[index]
    ];
}

bool CWaterPolySA::ContainsPoint( float fX, float fY ) const
{
    bool bInside = false;
    int numVertices = GetNumVertices ();
    unsigned short *pwVertexIDs =
        GetType () == WATER_POLY_QUAD ? ((CWaterQuadSA *)this)->GetInterface ()->m_wVertexIDs
                                  : ((CWaterTriangleSA *)this)->GetInterface ()->m_wVertexIDs;
    
    CWaterVertexSA* pFrom;
    CWaterVertexSA* pTo;
    CVector vecFrom;
    CVector vecTo;
    int next[4];
    if ( GetType () == WATER_POLY_QUAD )
    {
        next[0] = 1;
        next[1] = 3;
        next[2] = 0;
        next[3] = 2;
    }
    else
    {
        next[0] = 1;
        next[1] = 2;
        next[2] = 0;
    }
    for ( int i = 0; i < numVertices; i++ )
    {
        pFrom = &g_pWaterManager->m_Vertices [ pwVertexIDs[i] ];
        pTo   = &g_pWaterManager->m_Vertices [ pwVertexIDs[next[i]] ];
        pFrom->GetPosition ( vecFrom );
        pTo->GetPosition ( vecTo );
        
        if ( (vecFrom.fY > fY) != (vecTo.fY > fY) &&
             fX < vecFrom.fX + (vecTo.fX - vecFrom.fX) * (fY - vecFrom.fX) / (vecTo.fY - vecFrom.fY) )
           bInside = !bInside;
    }
    return bInside;
}

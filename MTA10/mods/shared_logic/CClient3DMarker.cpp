/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClient3DMarker.cpp
*  PURPOSE:     3D marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame *g_pClientGame;

CClient3DMarker::CClient3DMarker( CClientMarker *pThis )
{
    // Init
    m_pThis = pThis;
    m_streamedIn = false;
    m_visible = true;
    m_color = SColorRGBA( 255, 0, 0, 128 );
    m_size = 4.0f;
    m_type = MARKER3D_CYLINDER2;
    m_pMarker = NULL;
    m_id = (unsigned int)this;
}

CClient3DMarker::~CClient3DMarker()
{
}

unsigned long CClient3DMarker::Get3DMarkerType() const
{
    switch( m_type )
    {
    case MARKER3D_CYLINDER2:
        return TYPE_CYLINDER;
    case MARKER3D_ARROW:
        return TYPE_ARROW;
    }

    return TYPE_INVALID;
}

void CClient3DMarker::Set3DMarkerType( unsigned long ulType )
{
    switch( ulType )
    {
    case CClient3DMarker::TYPE_CYLINDER:
        m_type = MARKER3D_CYLINDER2;
        break;
    case CClient3DMarker::TYPE_ARROW:
        m_type = MARKER3D_ARROW;
        break;
    default:
        m_type = MARKER3D_CYLINDER2;
        break;
    }
}

bool CClient3DMarker::IsHit( const CVector& pos ) const
{
    return IsPointNearPoint3D( m_Matrix.vPos, pos, m_size + 4 );
}

void CClient3DMarker::StreamIn()
{
    // We're now streamed in
    m_streamedIn = true;
}

void CClient3DMarker::StreamOut()
{
    // We're no longer streamed in
    m_streamedIn = false;
    m_pMarker = NULL;
}

void CClient3DMarker::DoPulse()
{
    if ( !m_streamedIn || !m_visible || m_pThis->GetInterior() != g_pGame->GetWorld()->GetCurrentArea() )
        return;

    m_pMarker = g_pGame->Get3DMarkers()->CreateMarker( m_id, m_type, m_Matrix.vPos, m_size, 0.2f, m_color );

    if ( !m_pMarker )
        return;

    // Make sure it doesn't get cleaned up
    m_pMarker->SetActive();

    m_pMarker->SetColor( m_color );
    m_pMarker->SetSize( m_size );
    m_pMarker->SetMatrix( m_Matrix );

    // fixme: This appears to do nothing
    //g_pGame->GetVisibilityPlugins ()->SetClumpAlpha ( ( RpClump * ) m_pMarker->GetRwObject (), m_Color.A );
}
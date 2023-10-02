/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkerSA.cpp
*  PURPOSE:     3D Marker entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

void C3DMarkerSAInterface::DeleteRwObject( void )
{
    // Reset usage parameters
    m_nIdentifier = 0;
    m_nStartTime = 0;
    m_bIsUsed = false;
    m_bUnkFlag = false;
    m_nType = 257;

    // Destroy RenderWare objects
    RpAtomic *atom = m_pRwObject;
    RwFrame *frame = atom->parent;

    RpAtomicDestroy( atom );
    RwFrameDestroy( frame );

    // We do not own an atomic anymore.
    m_pRwObject = NULL;
}

void C3DMarkerSA::GetMatrix( RwMatrix& mat ) const
{
    mat = GetInterface()->m_mat;
}

void C3DMarkerSA::SetMatrix( const RwMatrix& mat )
{
    GetInterface()->m_mat = mat;
}

void C3DMarkerSA::SetPosition( const CVector& pos )
{
    DEBUG_TRACE("void C3DMarkerSA::SetPosition( const CVector& pos )");
    GetInterface()->m_mat.vPos = pos;
}

const CVector& C3DMarkerSA::GetPosition( void ) const
{
    DEBUG_TRACE("const CVector& C3DMarkerSA::GetPosition( void ) const");
    return GetInterface()->m_mat.vPos;
}

unsigned int C3DMarkerSA::GetType( void ) const
{
    DEBUG_TRACE("unsigned int C3DMarkerSA::GetType( void ) const");
    return GetInterface()->m_nType;
}

void C3DMarkerSA::SetType( unsigned int type )
{
    DEBUG_TRACE("void C3DMarkerSA::SetType( unsigned int type )");
    GetInterface()->m_nType = type;
}

bool C3DMarkerSA::IsActive( void ) const
{
    DEBUG_TRACE("bool C3DMarkerSA::IsActive( void ) const");
    return GetInterface()->m_bIsUsed;
}

unsigned int C3DMarkerSA::GetIdentifier( void ) const
{
    DEBUG_TRACE("unsigned int C3DMarkerSA::GetIdentifier( void ) const");
    return GetInterface()->m_nIdentifier;
}

SColor C3DMarkerSA::GetColor( void ) const
{
    DEBUG_TRACE("SColor C3DMarkerSA::GetColor( void ) const");

    // From ABGR
    unsigned long ulABGR = GetInterface()->rwColour;
    SColor color;
    color.A = ( ulABGR >> 24 ) && 0xff;
    color.B = ( ulABGR >> 16 ) && 0xff;
    color.G = ( ulABGR >> 8 ) && 0xff;
    color.R = ulABGR && 0xff;
    return color;
}

void C3DMarkerSA::SetColor( const SColor color )
{
    // To ABGR
    GetInterface()->rwColour = ( color.A << 24 ) | ( color.B << 16 ) | ( color.G << 8 ) | color.R;
}

void C3DMarkerSA::SetPulsePeriod( unsigned short period )
{
    DEBUG_TRACE("void C3DMarkerSA::SetPulsePeriod( unsigned short period )");
    GetInterface()->m_nPulsePeriod = period;
}

void C3DMarkerSA::SetRotateRate( short rate )
{
    DEBUG_TRACE("void C3DMarkerSA::SetRotateRate( short rate )");
    GetInterface()->m_nRotateRate = rate;
}

float C3DMarkerSA::GetSize( void ) const
{
    DEBUG_TRACE("float C3DMarkerSA::GetSize( void ) const");
    return GetInterface()->m_fSize;
}

void C3DMarkerSA::SetSize( float size )
{
    DEBUG_TRACE("void C3DMarkerSA::SetSize( float size )");
    GetInterface()->m_fSize = size;
}

float C3DMarkerSA::GetBrightness( void ) const
{
    DEBUG_TRACE("float C3DMarkerSA::GetBrightness( void ) const");
    return GetInterface()->m_fBrightness;
}

void C3DMarkerSA::SetBrightness( float brightness )
{
    DEBUG_TRACE("void C3DMarkerSA::SetBrightness( float brightness )");
    GetInterface()->m_fBrightness = brightness;
}

void C3DMarkerSA::SetCameraRange( float range )
{
    DEBUG_TRACE("void C3DMarkerSA::SetCameraRange( float range )");
    GetInterface()->m_fCameraRange = range;
}

void C3DMarkerSA::SetPulseFraction( float fraction )
{
    DEBUG_TRACE("void C3DMarkerSA::SetPulseFraction( float fraction )");
    GetInterface()->m_fPulseFraction = fraction;
}

float C3DMarkerSA::GetPulseFraction( void ) const
{
    DEBUG_TRACE("float C3DMarkerSA::GetPulseFraction( void ) const");
    return GetInterface()->m_fPulseFraction;
}

void C3DMarkerSA::Disable( void )
{
    DEBUG_TRACE("void C3DMarkerSA::Disable( void )");
    GetInterface()->m_nIdentifier = 0;
}

void C3DMarkerSA::DeleteMarkerObject( void )
{
    DEBUG_TRACE("void C3DMarkerSA::DeleteMarkerObject( void )");

    if ( !GetInterface()->m_pRwObject )
        return;

    GetInterface()->DeleteRwObject();
}

void C3DMarkerSA::Reset( void )
{
    DEBUG_TRACE("void C3DMarkerSA::Reset( void )");
    m_interface->m_lastPosition = m_interface->m_mat.vPos;
}

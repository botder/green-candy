/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkersSA.cpp
*  PURPOSE:     3D Marker entity manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

inline static unsigned int GetMarkerIndex( C3DMarkerSAInterface *intf )
{
    return ( intf - (C3DMarkerSAInterface*)ARRAY_3D_MARKERS );
}

C3DMarkersSA::C3DMarkersSA()
{
    DEBUG_TRACE("C3DMarkersSA::C3DMarkersSA()");

    for ( unsigned int i = 0; i < MAX_3D_MARKERS; i++ )
        m_markers[i] = new C3DMarkerSA( (C3DMarkerSAInterface*)( ARRAY_3D_MARKERS + i * sizeof(C3DMarkerSAInterface) ) );
}

C3DMarkersSA::~C3DMarkersSA()
{
    for ( unsigned int i = 0; i < MAX_3D_MARKERS; i++ )
        delete m_markers[i];
}

C3DMarkerSA* C3DMarkersSA::CreateMarker( unsigned int id, e3DMarkerType eType, const CVector& pos, float fSize, float pulseFraction, SColor color )
{
    DEBUG_TRACE("C3DMarker* C3DMarkersSA::CreateMarker( unsigned int id, e3DMarkerType type, const CVector& pos, float fSize, float pulseFraction, SColor color )");

    unsigned char r, g, b, a;
    r = color.A;
    g = color.G;
    b = color.B;
    a = color.A;

    DWORD dwFunc = FUNC_PlaceMarker;
    C3DMarkerSAInterface *intf;
    _asm
    {
        push    1           // zCheck  ##SA##
        push    0           // normalZ ##SA##
        push    0           // normalY ##SA##
        push    0           // normalX ##SA##
        push    0           // rotate rate
        push    pulseFraction       // pulse
        push    0           // period
        push    a           // alpha
        push    b           // blue
        push    g           // green
        push    r           // red
        push    fSize       // size
        push    pos         // position
        movzx   eax,eType
        push    eax         // type
        push    id          // identifier
        call    dwFunc
        mov     intf, eax
        add     esp, 0x3C
    }

    if ( !intf )
        return NULL;

    return m_markers[GetMarkerIndex( intf )];
}

C3DMarkerSA* C3DMarkersSA::FindFreeMarker()
{
    DEBUG_TRACE("C3DMarkerSA* C3DMarkersSA::FindFreeMarker()");

    for( unsigned int i = 0; i < MAX_3D_MARKERS; i++ )
    {
        if ( !m_markers[i]->IsActive() ) 
            return m_markers[i];
    }
    return NULL;
}

C3DMarkerSA* C3DMarkersSA::FindMarker( unsigned int id )
{
    DEBUG_TRACE("C3DMarkerSA* C3DMarkersSA::FindMarker( unsigned int idx )");

    for ( unsigned int i = 0; i < MAX_3D_MARKERS; i++ )
    {
        if ( m_markers[i]->GetIdentifier() == id ) 
            return m_markers[i];
    }
    return NULL;
}
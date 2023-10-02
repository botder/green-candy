/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkersSA.h
*  PURPOSE:     Header file for 3D Marker entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_MARKERS
#define __CGAMESA_MARKERS

#define FUNC_PlaceMarker        0x725120 // ##SA##

#define MAX_3D_MARKERS          32

#define ARRAY_3D_MARKERS        0xC7DD58

#include <game/C3DMarkers.h>
#include "C3DMarkerSA.h"

class C3DMarkersSA : public C3DMarkers
{
public: 
                            C3DMarkersSA();
                            ~C3DMarkersSA();

    C3DMarkerSA*            CreateMarker( unsigned int id, e3DMarkerType type, const CVector& pos, float size, float pulseFraction, SColor color );
    C3DMarkerSA*            FindFreeMarker();
    C3DMarkerSA*            FindMarker( unsigned int id );

private:
    C3DMarkerSA*            m_markers[MAX_3D_MARKERS];
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/C3DMarkers.h
*  PURPOSE:     3D marker manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_3DMARKERS
#define __CGAME_3DMARKERS

#include "C3DMarker.h"

class C3DMarkers abstract
{
public:
    virtual C3DMarker*          CreateMarker( unsigned int id, e3DMarkerType type, const CVector& pos, float size, float pulseFraction, SColor color ) = 0;
    virtual C3DMarker*          FindMarker( unsigned int id ) = 0;
};

#endif

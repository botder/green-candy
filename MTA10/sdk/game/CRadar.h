/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CRadar.h
*  PURPOSE:     Game radar interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_RADAR
#define __CGAME_RADAR

#include "CMarker.h"
#include "CObject.h"
#include "CPed.h"

class CVehicle;

class CRadar
{
public:
    virtual CMarker*            CreateMarker( const CVector& pos ) = 0;
    virtual CMarker*            CreateMarker( CVehicle *vehicle ) = 0;
    virtual CMarker*            CreateMarker( CObject *object ) = 0;
    virtual CMarker*            CreateMarker( CPed *ped ) = 0;

    virtual CMarker*            GetFreeMarker() = 0;
    virtual CMarker*            GetMarker( unsigned short id ) = 0;

    virtual void                ClearMarkerForEntity( CVehicle *vehicle ) = 0;
    virtual void                ClearMarkerForEntity( CObject *object ) = 0;
    virtual void                ClearMarkerForEntity( CPed *ped ) = 0;

    virtual void                DrawAreaOnRadar( float fX1, float fY1, float fX2, float fY2, const SColor color ) = 0;
};

#endif

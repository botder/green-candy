/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CMarker.h
*  PURPOSE:     Marker entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_MARKER
#define __CGAME_MARKER

#include "CEntity.h"
#include "CObject.h"
#include "CPed.h"

class CVehicle;

#define MARKER_SCALE_SMALL          1
#define MARKER_SCALE_NORMAL         2

class CMarker
{
public:
    /* Our Functions */
    virtual void                        SetSprite( eMarkerSprite Sprite ) = 0;
    virtual void                        SetDisplay( eMarkerDisplay wDisplay ) = 0;
    virtual void                        SetScale( unsigned short scale ) = 0;
    virtual void                        SetColor( eMarkerColor color ) = 0;
    virtual void                        SetColor( const SColor color ) = 0;
    virtual void                        Remove() = 0;
    virtual bool                        IsActive() const = 0;
    virtual void                        SetPosition( const CVector& pos ) = 0;
    virtual const CVector&              GetPosition() const = 0;
    virtual void                        SetEntity( CVehicle *vehicle ) = 0;
    virtual void                        SetEntity( CPed *ped ) = 0;
    virtual void                        SetEntity( CObject *object ) = 0;
    virtual CEntity*                    GetEntity() const = 0;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CBike.h
*  PURPOSE:     Bike vehicle entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_BIKE
#define __CGAME_BIKE

#include "CVehicle.h"

class CBike : public virtual CVehicle
{   
public:
    virtual                     ~CBike()    {};

    virtual unsigned char       GetBikeWheelStatus( unsigned char wheel ) const = 0;
    virtual void                SetBikeWheelStatus( unsigned char wheel, unsigned char status ) = 0;
};

#endif

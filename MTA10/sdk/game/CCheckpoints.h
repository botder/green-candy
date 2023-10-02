/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CCheckpoints.h
*  PURPOSE:     Checkpoint entity manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CHECKPOINTS
#define __CGAME_CHECKPOINTS

#include "CCheckpoint.h"

class CCheckpoints abstract
{
public:
    virtual CCheckpoint*        CreateCheckpoint( unsigned int id, unsigned short type, const CVector& pos, const CVector& dir, float size, float pulseFraction, const SColor color ) = 0;
};

#endif

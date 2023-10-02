/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointsSA.h
*  PURPOSE:     Header file for checkpoint entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CHECKPOINTS
#define __CGAMESA_CHECKPOINTS

#define FUNC_CCheckpoints__PlaceMarker  0x722c40 // ##SA##

#define MAX_CHECKPOINTS         32

#define ARRAY_CHECKPOINTS       0xC7F158

#include <game/CCheckpoints.h>
#include "CCheckpointSA.h"

class CCheckpointSA;

class CCheckpointsSA : public CCheckpoints
{
public: 
                                CCheckpointsSA();
                                ~CCheckpointsSA();

    CCheckpointSA*              CreateCheckpoint( unsigned int id, unsigned short type, const CVector& pos, const CVector& dir, float size, float pulseFraction, const SColor color );
    CCheckpointSA*              FindFreeMarker();

private:
    CCheckpointSA*              m_checkpoints[MAX_CHECKPOINTS];
};

#endif

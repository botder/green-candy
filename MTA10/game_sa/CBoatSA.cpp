/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBoatSA.cpp
*  PURPOSE:     Boat vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBoatSA::CBoatSA( CBoatSAInterface *boat ) : CVehicleSA( boat )
{
    DEBUG_TRACE("CBoatSA::CBoatSA( CBoatSAInterface *boat )");
}

CBoatSA::~CBoatSA()
{
}
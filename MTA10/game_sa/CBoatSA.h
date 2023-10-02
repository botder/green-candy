/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBoatSA.h
*  PURPOSE:     Header file for boat vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BOAT
#define __CGAMESA_BOAT

#include <game/CBoat.h>

class CBoatSAInterface : public CVehicleSAInterface
{
public:
    // fill this
};

class CBoatSA : public virtual CBoat, public CVehicleSA
{
public:
                                CBoatSA( CBoatSAInterface *boat );
                                ~CBoatSA();
};

#endif

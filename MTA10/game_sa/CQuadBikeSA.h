/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CQuadBikeSA.h
*  PURPOSE:     Quad bike vehicle entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CQuadBikeSA_H_
#define _CQuadBikeSA_H_

#include <game/CQuadBike.h>

class CQuadBikeSAInterface : public CAutomobileSAInterface  // size: 2492
{
public:
    BYTE                        m_pad50[52];                            // 2440
};

class CQuadBikeSA : public virtual CQuadBike, public CAutomobileSA
{
public:
                                CQuadBikeSA( CQuadBikeSAInterface *bike );
                                ~CQuadBikeSA();
};

#endif //_CQuadBikeSA_H_
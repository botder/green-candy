/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMonsterTruckSA.h
*  PURPOSE:     Monster Truck vehicle entity header
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CMonsterTruckSA_H_
#define _CMonsterTruckSA_H_

#include <game/CMonsterTruck.h>

class CMonsterTruckSAInterface : public CAutomobileSAInterface   // size: 2460
{
public:
    BYTE                        m_pad12[20];                            // 2440
};

class CMonsterTruckSA : public virtual CMonsterTruck, public CAutomobileSA
{
public:
                                CMonsterTruckSA( CMonsterTruckSAInterface *monster );
                                ~CMonsterTruckSA();
};

#endif //_CMonsterTruckSA_H_
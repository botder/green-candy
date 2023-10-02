/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAutomobileTrailerSA.h
*  PURPOSE:     Automobile Trailer entity header
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CAutomobileTrailerSA_H_
#define _CAutomobileTrailerSA_H_

#include <game/CAutomobileTrailer.h>

class CAutomobileTrailerSAInterface : public CAutomobileSAInterface // size: 2548
{
public:
    BYTE                        m_pad40[108];                           // 2440
};

class CAutomobileTrailerSA : public virtual CAutomobileTrailer, public CAutomobileSA
{
public:
                                    CAutomobileTrailerSA( CAutomobileTrailerSAInterface *trailer );
                                    ~CAutomobileTrailerSA();
};

#endif //_CAutomobileTrailerSA_H_
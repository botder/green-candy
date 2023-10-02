/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBicycleSA.h
*  PURPOSE:     Header file for bicycle vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_BMX
#define __CGAMESA_BMX

#include <game/CBicycle.h>

class CBicycleSAInterface : public CBikeSAInterface // síze: 2104
{
public:
    BYTE                            m_pad40[36];                        // 2068
};

class CBicycleSA : public virtual CBicycle, public CBikeSA
{
public:
                                CBicycleSA( CBicycleSAInterface *bike );
                                ~CBicycleSA();
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CBicycleSA.cpp
*  PURPOSE:     Bmx bike vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CBicycleSA::CBicycleSA( CBicycleSAInterface *bike ) : CBikeSA( bike )
{
    DEBUG_TRACE("CBicycleSA::CBicycleSA( CBicycleSAInterface *bike )");
}

CBicycleSA::~CBicycleSA()
{
}
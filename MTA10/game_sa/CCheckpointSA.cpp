/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointSA.cpp
*  PURPOSE:     Checkpoint entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

SColor CCheckpointSA::GetColor() const
{
    DEBUG_TRACE("SColor CCheckpointSA::GetColor() const");

    // From ABGR
    unsigned int ulABGR = GetInterface()->m_rwColor;
    SColor color;
    color.A = ( ulABGR >> 24 ) && 0xff;
    color.B = ( ulABGR >> 16 ) && 0xff;
    color.G = ( ulABGR >> 8 ) && 0xff;
    color.R = ulABGR && 0xff;
    return color;
}

void CCheckpointSA::SetColor( SColor color )
{
    DEBUG_TRACE("void CCheckpointSA::SetColor( SColor color )");

    // To ABGR
    GetInterface()->m_rwColor = ( color.A << 24 ) | ( color.B << 16 ) | ( color.G << 8 ) | color.R;
}

void CCheckpointSA::SetRotateRate( short rate )
{
    DEBUG_TRACE("void CCheckpointSA::SetRotateRate( short rate )");
    
    GetInterface()->m_rotateRate = rate;
    GetInterface()->m_rotFlag = ( rate != 0 );
}

void CCheckpointSA::Remove()
{
    DEBUG_TRACE("void CCheckpointSA::Remove()");

    GetInterface()->m_isUsed = false;
    GetInterface()->m_type = 0x101;    
    GetInterface()->m_rwColor = 0;
}

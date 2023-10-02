/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlaneSA.cpp
*  PURPOSE:     Plane vehicle entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlaneSA::CPlaneSA( CPlaneSAInterface *plane ) : CAutomobileSA( plane )
{
    DEBUG_TRACE("CPlaneSA::CPlaneSA( CPlaneSAInterface *plane )");
}

CPlaneSA::~CPlaneSA()
{
}

void CPlaneSA::SetLandingGearDown( bool bLandingGearDown )
{
    CPlaneSAInterface *plane = GetInterface();
    float& fTimeStep = *(float*)0xB7CB5C;
    
    if ( IsLandingGearDown() == bLandingGearDown )
        return;

    // The following code toggles the landing gear direction
    if ( plane->m_undercarriagePos == 0.0f )
    {
        for ( unsigned int n=0; n<MAX_WHEELS; n++ )
            plane->m_damage.m_wheels[n] = 0x02;

        plane->m_undercarriagePos += fTimeStep * 0.02f;
    }
    else
    {
        if ( plane->m_undercarriagePos != 1.0f )
            plane->m_undercarriagePos *= -1.0f;
        else
            plane->m_undercarriagePos = fTimeStep * 0.02f - 1.0f;
    }
}

bool CPlaneSA::IsSmokeTrailEnabled() const
{
    return GetInterface()->m_smokeTrailEnabled;
}

void CPlaneSA::SetSmokeTrailEnabled( bool enabled )
{
    GetInterface()->m_smokeTrailEnabled = enabled;
}

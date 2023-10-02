/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRadarSA.cpp
*  PURPOSE:     Game radar
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMarkerSA* Markers[MAX_MARKERS];

CRadarSA::CRadarSA()
{
    DEBUG_TRACE("CRadarSA::CRadarSA()");

    for ( unsigned int i = 0; i < MAX_MARKERS; i++ )
        Markers[i] = new CMarkerSA((CMarkerSAInterface *)(ARRAY_CMarker + i * sizeof(CMarkerSAInterface)));
}

CRadarSA::~CRadarSA()
{
    for ( unsigned int i = 0; i < MAX_MARKERS; i++ )
        delete Markers[i];
}

CMarker* CRadarSA::CreateMarker( const CVector& pos )
{
    DEBUG_TRACE("CMarker* CRadarSA::CreateMarker( const CVector& pos )");

    CMarkerSA *marker;
    marker = (CMarkerSA*)GetFreeMarker();
    if ( marker )
    {
        marker->Init();
        marker->SetPosition( pos );
    }

/*  DWORD dwFunc = FUNC_SetCoordBlip;
    FLOAT fX = vecPosition->fX; FLOAT fY = vecPosition->fY; FLOAT fZ = vecPosition->fZ;
    _asm {
        push 0
        push 2
        push 0
        push fZ
        push fY
        push fX
        push 4
        call dwFunc
        add esp, 0x1C
    }*/
    //marker->SetColor((eMarkerColor)MARKER_COLOR_PALE_GREEN);
    //marker->SetScale(MARKER_SCALE_NORMAL);
    //marker->SetSprite((eMarkerSprite)MARKER_SPRITE_BORDER);
    //marker->SetDisplay((eMarkerDisplay)MARKER_DISPLAY_MARKERONLY ); 

    return marker;
}

CMarker* CRadarSA::CreateMarker( CVehicle *vehicle )
{
    DEBUG_TRACE("CMarker* CRadarSA::CreateMarker( CVehicle *vehicle )");
    CMarkerSA * marker;
    marker = (CMarkerSA *)this->GetFreeMarker();

    marker->SetEntity(vehicle);
    marker->SetColor((eMarkerColor)MARKER_COLOR_YELLOW);
    marker->SetScale(MARKER_SCALE_NORMAL);
    marker->SetSprite((eMarkerSprite)MARKER_SPRITE_NONE);
    marker->SetDisplay((eMarkerDisplay)MARKER_DISPLAY_MARKERONLY); 

    return marker;
}

CMarker* CRadarSA::CreateMarker( CObject *object )
{
    DEBUG_TRACE("CMarker* CRadarSA::CreateMarker( CObject *object )");
    CMarkerSA * marker;
    marker = (CMarkerSA *)GetFreeMarker();

    marker->SetEntity(object);
    marker->SetColor((eMarkerColor)MARKER_COLOR_YELLOW);
    marker->SetScale(MARKER_SCALE_NORMAL);
    marker->SetSprite((eMarkerSprite)MARKER_SPRITE_NONE);
    marker->SetDisplay((eMarkerDisplay)MARKER_DISPLAY_MARKERONLY); 

    return marker;
}

CMarker* CRadarSA::CreateMarker( CPed *ped )
{
    DEBUG_TRACE("CMarker* CRadarSA::CreateMarker( CPed *ped )");
    CMarkerSA * marker;
    marker = (CMarkerSA *)GetFreeMarker();

    marker->SetEntity(ped);
    marker->SetColor((eMarkerColor)MARKER_COLOR_YELLOW);
    marker->SetScale(MARKER_SCALE_NORMAL);
    marker->SetSprite((eMarkerSprite)MARKER_SPRITE_NONE);
    marker->SetDisplay((eMarkerDisplay)MARKER_DISPLAY_MARKERONLY);

    return marker;
}

CMarker* CRadarSA::GetFreeMarker()
{
    DEBUG_TRACE("CMarker* CRadarSA::GetFreeMarker()");

    unsigned int Index = 0;

    while ((Index < MAX_MARKERS) && (Markers[Index]->GetInterface()->m_tracking))
        Index++;

    if ( Index >= MAX_MARKERS )
        return NULL;

    return Markers[Index];
}

CMarker* CRadarSA::GetMarker( unsigned short id )
{
    DEBUG_TRACE("CMarker* CRadarSA::GetMarker( unsigned short id )");
    return Markers[id];
}

void CRadarSA::ClearMarkerForEntity( CVehicle *vehicle )
{
    DEBUG_TRACE("void CRadarSA::ClearMarkerForEntity( CVehicle *vehicle )");
    CMarkerSA * marker;

    for(int i = 0; i < MAX_MARKERS; i++)
    {
        marker = (CMarkerSA *)(ARRAY_CMarker + i * sizeof(CMarkerSA));
        if(marker->GetEntity() == (CEntity *)vehicle)
        {
            marker->Remove();
        }
    }
}

void CRadarSA::ClearMarkerForEntity( CObject *object )
{
    DEBUG_TRACE("void CRadarSA::ClearMarkerForEntity( CObject *object )");
    CMarkerSA * marker;

    for(int i = 0; i < MAX_MARKERS; i++)
    {
        marker = (CMarkerSA *)(ARRAY_CMarker + i * sizeof(CMarkerSA));
        if(marker->GetEntity() == (CEntity *)object)
        {
            marker->Remove();
        }
    }
}

void CRadarSA::ClearMarkerForEntity( CPed *ped )
{
    DEBUG_TRACE("void CRadarSA::ClearMarkerForEntity( CPed *ped )");
    CMarkerSA * marker;

    for(int i = 0; i < MAX_MARKERS; i++)
    {
        marker = (CMarkerSA *)(ARRAY_CMarker + i * sizeof(CMarkerSA));
        if(marker->GetEntity() == (CEntity *)ped)
        {
            marker->Remove();
        }
    }
}

void CRadarSA::DrawAreaOnRadar( float fX1, float fY1, float fX2, float fY2, const SColor color )
{
    // Convert color to required abgr at the last moment
    unsigned long abgr = color.A << 24 | color.B << 16 | color.G << 8 | color.R;
    CRect myRect;
    myRect.fX1 = fX1;
    myRect.fY1 = fY1;
    myRect.fX2 = fX2;
    myRect.fY2 = fY2;
    DWORD dwFunc = FUNC_DrawAreaOnRadar;
    _asm
    {
        push    eax

        push    0
        lea     eax, abgr
        push    eax
        lea     eax, myRect
        push    eax
        call    dwFunc
        add     esp, 12

        pop     eax
    }
}

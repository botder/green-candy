/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CMarkerSA.cpp
*  PURPOSE:     Marker entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define COORD_BLIP_APPEARANCE_NORMAL 0

void CMarkerSA::Init()
{
    m_interface->m_position = CVector( 0, 0, 0 );
    m_interface->m_blipType = (BYTE)MARKER_TYPE_COORDS;
    m_interface->m_tracking = true;
    m_interface->m_blipRemain = FALSE;
//  internalInterface->nColour = MARKER_COLOR_BLUE;
//  internalInterface->bBright = TRUE;
    m_interface->m_shortRange = false;
    m_interface->m_friendly = false;
    m_interface->m_markerScale = 20.0f;
    m_interface->m_poolIndex = 0;
    m_interface->m_blipScale = 2;
    m_interface->m_blipFade = false;
    m_interface->m_displayFlag = MARKER_DISPLAY_BLIPONLY;
    m_interface->m_sprite = RADAR_SPRITE_NONE;

    m_interface->m_coordBlipAppearance = COORD_BLIP_APPEARANCE_NORMAL;
    m_interface->m_entryExit = NULL;
}

/**
 * Set the size of the sprite
 * @param wScale the relative size of the sprite. 1 is default.
 */
void CMarkerSA::SetScale( unsigned short scale )
{
    DEBUG_TRACE("void CMarkerSA::SetScale( unsigned short scale )");

    m_interface->m_blipScale = scale;
    m_interface->m_markerScale = 20.0f;
}

/**
 * Sets the color of the marker when MARKER_SPRITE_NONE is used
 * @param color eMarkerColor containing a valid colour id
 */
void CMarkerSA::SetColor( eMarkerColor color )
{
    DEBUG_TRACE("void CMarkerSA::SetColor( eMarkerColor color )");

    if( color < MARKER_COLOR_PLUM || color > MARKER_COLOR_DARK_TURQUOISE )
        return;

    if( color >= MARKER_COLOR_RED )
    {
        m_interface->m_bright = true;
        m_interface->m_colorID = color - MARKER_COLOR_RED;
        return;
    }

    m_interface->m_bright = false;
    m_interface->m_colorID = color;
}

/**
 * Sets the color of the marker when MARKER_SPRITE_NONE is used
 * @param color RGBA containing a valid colour in RGBA format.
 */
void CMarkerSA::SetColor( const SColor color )
{
    DEBUG_TRACE("void CMarkerSA::SetColor( const SColor color )");

    // Convert to required rgba at the last moment
    m_interface->m_colorID = color.R << 24 | color.G << 16 | color.B << 8 | color.A;
}

void CMarkerSA::Remove()
{
    DEBUG_TRACE("void CMarkerSA::Remove()");

    m_interface->m_blipType = MARKER_TYPE_UNUSED;
    m_interface->m_displayFlag = MARKER_DISPLAY_NEITHER;
    m_interface->m_sprite = MARKER_SPRITE_NONE;
    m_interface->m_tracking = false;
}

void CMarkerSA::SetEntity( CVehicle *vehicle )
{   
    DEBUG_TRACE("void CMarkerSA::SetEntity( CVehicle *vehicle )");

    m_interface->m_poolIndex = vehicle->GetPoolIndex();
    m_interface->m_blipType = (BYTE)MARKER_TYPE_CAR;
}

void CMarkerSA::SetEntity( CPed *ped )
{
    DEBUG_TRACE("void CMarkerSA::SetEntity( CPed *ped )");

    m_interface->m_poolIndex = ped->GetPoolIndex();
    m_interface->m_blipType = (BYTE)MARKER_TYPE_CHAR;
}

void CMarkerSA::SetEntity( CObject *object )
{
    DEBUG_TRACE("void CMarkerSA::SetEntity( CObject *object )");

    m_interface->m_poolIndex = object->GetPoolIndex();
    m_interface->m_blipType = (eMarkerType)MARKER_TYPE_OBJECT;
}

CEntity* CMarkerSA::GetEntity() const
{
    DEBUG_TRACE("CEntity* CMarkerSA::GetEntity() const");

    switch( m_interface->m_blipType )
    {
    case MARKER_TYPE_CAR:       return pGame->GetPools()->GetVehicleFromRef( m_interface->m_poolIndex );
    case MARKER_TYPE_CHAR:      return pGame->GetPools()->GetPedFromRef( m_interface->m_poolIndex );
    case MARKER_TYPE_OBJECT:    return pGame->GetPools()->GetObjectFromRef( m_interface->m_poolIndex );
    }
    return NULL;
}
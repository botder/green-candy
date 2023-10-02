/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCivilianPedSA.cpp
*  PURPOSE:     Civilian ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/**
 * Constructor for CCivilianPedSA
 */
CCivilianPedSA::CCivilianPedSA( CCivilianPedSAInterface *ped, modelId_t modelID ) : CPedSA( ped )
{
    DEBUG_TRACE("CCivilianPedSA::CCivilianPedSA( CCivilianPedSAInterface *ped, unsigned short modelID )");

    SetType( CIVILIAN_PED );
    SetModelIndex( modelID );

    BOOL_FLAG( GetInterface()->m_entityFlags, ENTITY_DISABLESTREAMING, true );
    BOOL_FLAG( GetInterface()->m_entityFlags, ENTITY_NOSTREAM, true );
}

CCivilianPedSA::~CCivilianPedSA( void )
{
    DEBUG_TRACE("CCivilianPedSA::~CCivilianPedSA( void )");
}

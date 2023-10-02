/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CFireSA.cpp
*  PURPOSE:     Fire
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CFireSA::Extinguish()
{
    DEBUG_TRACE("void CFireSA::Extinguish()");

    DWORD dwFunction = FUNC_Extinguish;
    DWORD dwPointer = (DWORD)m_interface;
    _asm
    {
        mov     ecx, dwPointer
        call    dwFunction
    }
    m_interface->m_active = false;
}

/**
 * Set the position the fire is burning at. This won't have any effect if this fire has a target set
 * @param vecPosition CVector * containing the desired position for the fire.
 * @see CFireSA::SetTarget
 */
void CFireSA::SetPosition( const CVector& pos )
{
    DEBUG_TRACE("void CFireSA::SetPosition( const CVector& pos )");
    
    m_interface->m_target = NULL;
    m_interface->m_position = pos;
}

CEntity* CFireSA::GetCreator() const
{
    DEBUG_TRACE("CEntity* CFireSA::GetCreator() const");

    if ( !m_interface->m_creator )
        return NULL;

    switch( m_interface->m_creator->nType )
    {
    case ENTITY_TYPE_PED:       return pGame->GetPools()->GetPed( m_interface->m_creator );
    case ENTITY_TYPE_VEHICLE:   return pGame->GetPools()->GetVehicle( m_interface->m_creator );
    }

    return NULL;
}

CEntity* CFireSA::GetEntityOnFire() const
{
    DEBUG_TRACE("CEntity* CFireSA::GetEntityOnFire() const");

    if ( !m_interface->m_target )
        return NULL;

    switch( m_interface->m_target->nType )
    {
    case ENTITY_TYPE_PED:       return pGame->GetPools()->GetPed( m_interface->m_target );
    case ENTITY_TYPE_VEHICLE:   return pGame->GetPools()->GetVehicle( m_interface->m_target );
    }
    return NULL;
}

void CFireSA::SetTarget( CEntity *entity )
{
    DEBUG_TRACE("void CFireSA::SetTarget( CEntity *entity )");

    if ( !entity )
    {
        m_interface->m_target = NULL;
        return;
    }

    m_interface->m_target = dynamic_cast <CEntitySA*> ( entity )->GetInterface();
}

void CFireSA::Ignite()
{
    DEBUG_TRACE("void CFireSA::Ignite()");

    m_interface->m_active = TRUE;

    const CVector& pos = GetPosition();
    DWORD dwFunc = FUNC_CreateFxSysForStrength;
    DWORD dwThis = (DWORD)m_interface;
    _asm
    {
        mov     ecx, dwThis
        push    0
        push    pos
        call    dwFunc
    }

    m_interface->m_beingExtinguished = false;
    m_interface->m_fresh = true;
    m_interface->m_numGenerationsAllowed = 100;
}
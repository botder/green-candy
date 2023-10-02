/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPadSA.cpp
*  PURPOSE:     Controller pad input logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CPadSAInterface::SetState( const CControllerState& cs )
{
    m_prev = m_new;
    m_new = cs;
}

// Hack for hax
void CPadSAInterface::InjectCurrent( const CControllerState& cs )
{
    m_new = cs;
}

void CPadSAInterface::SetHornHistory( bool state )
{
    m_numHorn = ( m_numHorn + 1 ) % MAX_HORN_HISTORY;

    m_hornHistory[m_numHorn] = state;
}

CPadSA::CPadSA( CPadSAInterface *pad )
{
    m_interface = pad;
}

CPadSA::~CPadSA()
{
}

const CControllerState& CPadSA::GetState()
{
    DEBUG_TRACE("CControllerState& CPadSA::GetState()");
    
    return m_interface->m_new;
}

const CControllerState& CPadSA::GetPreviousState()
{
    DEBUG_TRACE("CControllerState& CPadSA::GetPreviousState()");
    
    return m_interface->m_prev;
}

void CPadSA::SetState( const CControllerState& cs )
{
    DEBUG_TRACE("void CPadSA::SetState( const CControllerState& cs )");
    
    m_interface->SetState( cs );
}

void CPadSA::InjectCurrent( const CControllerState& cs )
{
    DEBUG_TRACE("void CPadSA::InjectCurrent( const CControllerState& cs )");

    m_interface->InjectCurrent( cs );
}

void CPadSA::Store()
{
    DEBUG_TRACE("void CPadSA::Store()");

    m_stored = *m_interface;
}

void CPadSA::Restore()
{
    DEBUG_TRACE("void CPadSA::Restore()");

    *m_interface = m_stored;
}

bool CPadSA::IsEnabled()
{
    return *(unsigned char*)FUNC_CPad_UpdatePads == 0x56;
}

// Actually, this disables ALL pads!
void CPadSA::Disable( bool bDisable )
{
    if ( bDisable )
        *(unsigned char*)FUNC_CPad_UpdatePads = 0xC3;
    else
        *(unsigned char*)FUNC_CPad_UpdatePads = 0x56;

    //this->internalInterface->DisablePlayerControls = bDisable;
}

void CPadSA::Clear()
{
    m_interface->m_new = CControllerState();
    m_interface->m_prev = CControllerState();
}

void CPadSA::SetHornHistoryValue( bool value )
{
    m_interface->SetHornHistory( value );
}

long CPadSA::GetAverageWeapon()
{
    return m_interface->m_averageWeapon;
}

void CPadSA::SetLastTimeTouched( long time )
{
    m_interface->m_lastControl = time;
}

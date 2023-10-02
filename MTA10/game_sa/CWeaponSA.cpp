/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponSA.cpp
*  PURPOSE:     Weapon class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CWeaponSA::CWeaponSA( CWeaponSAInterface *weapon, CPedSA *ped, eWeaponSlot weaponSlot )
{
    DEBUG_TRACE("CWeaponSA::CWeaponSA( CWeaponSAInterface * weaponInterface, CPed * ped, DWORD dwSlot )");

    m_owner = ped;
    m_weaponSlot = weaponSlot;

    m_interface = weapon;   
}

eWeaponType CWeaponSA::GetType()
{
    DEBUG_TRACE("eWeaponType CWeaponSA::GetType()");

    return m_interface->m_eWeaponType;
};

void CWeaponSA::SetType( eWeaponType type )
{
    DEBUG_TRACE("VOID CWeaponSA::SetType( eWeaponType type )");

    m_interface->m_eWeaponType = type;
}

eWeaponState CWeaponSA::GetState()
{
    DEBUG_TRACE("eWeaponState CWeaponSA::GetState(  )");

    return m_interface->m_eState;
}

void CWeaponSA::SetState( eWeaponState state )
{
    DEBUG_TRACE("void CWeaponSA::SetState( eWeaponState state )");

    m_interface->m_eState = state;
}

unsigned int CWeaponSA::GetAmmoInClip()
{
    DEBUG_TRACE("unsigned int CWeaponSA::GetAmmoInClip()");

    return m_interface->m_nAmmoInClip;
}

void CWeaponSA::SetAmmoInClip( unsigned int ammo )
{
    DEBUG_TRACE("void CWeaponSA::SetAmmoInClip( unsigned int ammo )");

    m_interface->m_nAmmoInClip = ammo;
}

unsigned int CWeaponSA::GetAmmoTotal()
{
    DEBUG_TRACE("unsigned int CWeaponSA::GetAmmoTotal()");

    return m_interface->m_nAmmoTotal;
}

void CWeaponSA::SetAmmoTotal( unsigned int ammo )
{
    DEBUG_TRACE("void CWeaponSA::SetAmmoTotal( unsigned int ammo )");

    m_interface->m_nAmmoTotal = ammo;
}

CPed* CWeaponSA::GetPed()
{
    DEBUG_TRACE("CPed * CWeaponSA::GetPed()");

    return m_owner;
}

CPedSA* CWeaponSA::GetPedInternal()
{
    return m_owner;
}

eWeaponSlot CWeaponSA::GetSlot()
{
    DEBUG_TRACE("eWeaponSlot CWeaponSA::GetSlot()");

    return m_weaponSlot;
}

void CWeaponSA::SetAsCurrentWeapon()
{
    DEBUG_TRACE("VOID CWeaponSA::SetAsCurrentWeapon()");

    m_owner->SetCurrentWeaponSlot( m_weaponSlot );
}

void CWeaponSA::Remove()
{
    DEBUG_TRACE("void CWeaponSA::Remove ()");

    DWORD dwFunc = FUNC_Shutdown;
    DWORD dwThis = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    // If the removed weapon was the currently active weapon, switch to empty-handed
    if ( m_owner->GetCurrentWeaponSlot() == m_weaponSlot )
    {
        CWeaponInfoSA* pInfo = pGame->GetWeaponInfo( m_interface->m_eWeaponType );

        if ( pInfo )
            m_owner->RemoveWeaponModel( pInfo->GetModel() );

        m_owner->SetCurrentWeaponSlot( WEAPONSLOT_TYPE_UNARMED );
    }
}

CWeaponInfo* CWeaponSA::GetInfo()
{
    return pGame->GetWeaponInfo( m_interface->m_eWeaponType );
}

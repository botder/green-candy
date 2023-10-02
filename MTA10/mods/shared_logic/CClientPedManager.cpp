/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedManager.cpp
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CClientPedManager::CClientPedManager( CClientManager* pManager )
{
    m_pManager = pManager;
}

CClientPedManager::~CClientPedManager()
{
    DeleteAll();
}

void CClientPedManager::DeleteAll()
{
    luaRefs refs;
    peds_t::iterator iter = m_List.begin();

    for ( ; iter != m_List.end(); iter++ )
    {
        CClientPed *ped = *iter;
        ped->Reference( refs );
        ped->Destroy();
    }

    m_List.clear();
}

void CClientPedManager::DoPulse()
{
    // Loop through our streamed-in peds
    peds_t List = m_StreamedIn;

    // We have to reference all peds/players
    for ( peds_t::iterator iter = List.begin(); iter != List.end(); iter++ )
        (*iter)->IncrementMethodStack();

    for ( peds_t::iterator iter = List.begin(); iter != List.end(); iter++ )
    {
        CClientPed *ped = *iter;

        // We should have a game ped here
        assert( ped->GetGamePlayer() );

        ped->StreamedInPulse();
    }

    for ( peds_t::iterator iter = List.begin(); iter != List.end(); iter++ )
        (*iter)->DecrementMethodStack();
}

CClientPed* CClientPedManager::Get ( ElementID ID, bool bCheckPlayers )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );

    if ( pEntity && ( pEntity->GetType () == CCLIENTPED || ( bCheckPlayers && pEntity->GetType () == CCLIENTPLAYER ) ) )
    {
        return static_cast < CClientPed* > ( pEntity );
    }

    return NULL;
}

CClientPed* CClientPedManager::Get ( CPlayerPed* pPlayer, bool bValidatePointer, bool bCheckPlayers )
{
    if ( !pPlayer )
        return NULL;

    if ( bValidatePointer )
    {
        peds_t::const_iterator iter = m_StreamedIn.begin();

        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGamePlayer () == pPlayer )
            {
                return *iter;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast < CClientPed* > ( pPlayer->GetStoredPointer () );

        if ( pPed->GetType () == CCLIENTPED || bCheckPlayers )
        {
            return pPed;
        }
    }
    return NULL;
}

CClientPed* CClientPedManager::GetSafe ( CEntity * pEntity, bool bCheckPlayers )
{
    if ( !pEntity )
        return NULL;

    for ( peds_t::const_iterator iter = m_StreamedIn.begin(); iter != m_StreamedIn.end (); iter++ )
    {
        if ( dynamic_cast < CEntity * > ( (*iter)->GetGamePlayer () ) == pEntity )
        {
            return *iter;
        }
    }
    return NULL;
}

void CClientPedManager::RemoveFromList ( CClientPed* pPed )
{
    ListRemove ( m_List, pPed );
}

void CClientPedManager::OnCreation ( CClientPed * pPed )
{
    // Check not already in the list to avoid multiple calls to pPed->StreamedInPulse() later
    if ( !ListContains ( m_StreamedIn, pPed ) )
        m_StreamedIn.push_back ( pPed );
}

void CClientPedManager::OnDestruction ( CClientPed * pPed )
{
    ListRemove ( m_StreamedIn, pPed );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIManager.cpp
*  PURPOSE:     GUI entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::map;


CClientGUIManager::CClientGUIManager()
{
    m_safeKeep = NULL;
}

CClientGUIManager::~CClientGUIManager()
{
}

void CClientGUIManager::Begin()
{
    m_safeKeep = new luaRefs;

    // Reference all GUI entities
    for ( elements_t::iterator iter = m_Elements.begin(); iter != m_Elements.end(); iter++ )
        (*iter)->Reference( *m_safeKeep );
}

bool CClientGUIManager::RemoveActivityLock( CClientGUIElement *element )
{
    if ( !m_safeKeep )
        return true;

    luaRefs::const_iterator elemIter;

    if ( m_safeKeep->FindClass( element->GetClass(), elemIter ) )
    {
        m_safeKeep->RemoveClass( elemIter );
        return true;
    }

    return false;
}

void CClientGUIManager::End()
{
    delete m_safeKeep;

    m_safeKeep = NULL;
}

bool CClientGUIManager::Exists( CClientGUIElement *element ) const
{
    return std::find( m_Elements.begin(), m_Elements.end(), element ) != m_Elements.end();
}

bool CClientGUIManager::Exists( CGUIElement *element ) const
{
    // Find the object in the list
    elements_t::const_iterator iter = m_Elements.begin();

    for ( ; iter != m_Elements.end(); iter++ )
    {
        if ( (*iter)->GetCGUIElement() == element )
            return true;
    }

    return false;
}

CClientGUIElement* CClientGUIManager::Get( CGUIElement* pCGUIElement )
{
    // Find the object in the list
    elements_t::const_iterator iter = m_Elements.begin();

    for ( ; iter != m_Elements.end(); iter++ )
    {
        if ( (*iter)->GetCGUIElement() == pCGUIElement )
            return *iter;
    }

    return NULL;
}

void CClientGUIManager::Add( CClientGUIElement *element )
{
    m_Elements.push_back( element );
}

void CClientGUIManager::Remove( CClientGUIElement *element )
{
    m_Elements.remove( element );
}

void CClientGUIManager::DoPulse()
{
    FlushQueuedUpdates();
}

void CClientGUIManager::QueueGridListUpdate( CClientGUIElement *element )
{
    ElementID ID = element->GetID();

    if ( m_QueuedGridListUpdates.find( ID ) == m_QueuedGridListUpdates.end() )
        m_QueuedGridListUpdates[ID] = true;
}

void CClientGUIManager::FlushQueuedUpdates()
{
    map < ElementID, bool >::iterator iter = m_QueuedGridListUpdates.begin ();
    for ( ; iter != m_QueuedGridListUpdates.end () ; ++iter )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( iter->first );
        if ( pEntity && pEntity->GetType () == CCLIENTGUI )
        {
            CClientGUIElement* pGUIElement = static_cast < CClientGUIElement* > ( pEntity );
            if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
            {
                CGUIGridList* pGUIGridList = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () );
                pGUIGridList->ForceUpdate ();
            }
        }
    }
    m_QueuedGridListUpdates.clear();
}

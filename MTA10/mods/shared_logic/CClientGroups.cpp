/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGroups.cpp
*  PURPOSE:     Dummy grouping class
*  DEVELOPERS:  Chris McArthur <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

void CClientGroups::DeleteAll()
{
    luaRefs refs;

    for ( dummies_t::iterator iter = m_List.begin(); iter != m_List.end(); ++iter )
    {
        CClientDummy *dummy = *iter;
        dummy->Reference( refs );
        dummy->Delete();
    }

    m_List.clear();
}

void CClientGroups::RemoveFromList( CClientDummy *pDummy )
{
    for ( dummies_t::iterator iter = m_List.begin(); iter != m_List.end(); )
    {
        CClientDummy* pCurrentDummy = *iter;

        if ( pCurrentDummy == pDummy )
        {
            iter = m_List.erase ( iter );
        }
        else
            ++iter;
    }
}

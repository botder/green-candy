/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGroups.h
*  PURPOSE:     Dummy grouping class
*  DEVELOPERS:  Chris McArthur <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTGROUPS_H
#define __CCLIENTGROUPS_H

#include "CClientDummy.h"
#include "CClientEntity.h"

class CClientGroups
{
    friend class CClientDummy;
public:
    inline                                  CClientGroups()                             {}
    inline                                  ~CClientGroups()                            { DeleteAll(); }

    void                                    DeleteAll();

    typedef std::list <CClientDummy*> dummies_t;

private:
    inline void                             AddToList( CClientDummy *dummy )            { m_List.push_back( dummy ); }
    void                                    RemoveFromList( CClientDummy *pDummy );

    dummies_t                               m_List;
};

#endif

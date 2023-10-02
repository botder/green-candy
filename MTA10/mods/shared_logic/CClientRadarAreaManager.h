/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarAreaManager.h
*  PURPOSE:     Radar area entity manager class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Chris McArthur <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTRADARAREAMANAGER_H
#define __CCLIENTRADARAREAMANAGER_H

#include "CClientRadarArea.h"
#include <list>

class CClientRadarAreaManager
{
    friend class CClientManager;
    friend class CClientRadarArea;
    friend class CClientGame;
public:
                                                            CClientRadarAreaManager( CClientManager* pManager );
                                                            ~CClientRadarAreaManager();

    CClientRadarArea*                                       Create( ElementID ID, lua_State *L, bool system );

    void                                                    Delete( CClientRadarArea* pRadarArea );
    void                                                    DeleteAll();

    typedef std::list <CClientRadarArea*> areaList_t;

    inline areaList_t::const_iterator                       IterBegin()                                         { return m_List.begin(); }
    inline areaList_t::const_iterator                       IterEnd()                                           { return m_List.end(); }

    static CClientRadarArea*                                Get( ElementID ID );

    unsigned short                                          GetDimension( void )                                { return m_usDimension; }
    void                                                    SetDimension( unsigned short usDimension );

private:
    void                                                    DoPulse( void );
    void                                                    DoPulse( bool bRender );

    inline void                                             AddToList( CClientRadarArea* pRadarArea )           { m_List.push_back( pRadarArea ); }
    void                                                    RemoveFromList( CClientRadarArea* pRadarArea );

private:
    CClientManager*                                         m_pManager;

    areaList_t                                              m_List;
    bool                                                    m_bDontRemoveFromList;
    unsigned short                                          m_usDimension;
};

#endif

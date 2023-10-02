/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIManager.h
*  PURPOSE:     GUI entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientGUIManager;
struct SGUIManagerListEntry;

#ifndef __CCLIENTGUIMANAGER_H
#define __CCLIENTGUIMANAGER_H

#include "CClientManager.h"
#include "CClientGUIElement.h"
#include <gui/CGUI.h>

class CClientGUIManager
{
    friend class CClientManager;
    friend class CClientGUIElement;

public:
                                            CClientGUIManager();
                                            ~CClientGUIManager();

    void                                    Begin();
    bool                                    RemoveActivityLock( CClientGUIElement *element );
    void                                    End();

    bool                                    Exists( CClientGUIElement *element ) const;
    bool                                    Exists( CGUIElement *element ) const;
    inline unsigned int                     Count() const                               { return m_Elements.size(); }

    CClientGUIElement*                      Get( CGUIElement *element );

    void                                    DoPulse();
    void                                    QueueGridListUpdate( CClientGUIElement *element );

private:
    void                                    Add( CClientGUIElement *element );
    void                                    Remove( CClientGUIElement *element );
    void                                    FlushQueuedUpdates();

private:
    typedef std::list <CClientGUIElement*> elements_t;

    luaRefs*                                m_safeKeep;

    elements_t                              m_Elements;
    std::map <ElementID, bool>              m_QueuedGridListUpdates;
};

#endif

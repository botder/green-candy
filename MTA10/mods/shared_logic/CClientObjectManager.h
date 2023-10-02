/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObjectManager.h
*  PURPOSE:     Physical object entity manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientObjectManager;

#ifndef __CCLIENTOBJECTMANAGER_H
#define __CCLIENTOBJECTMANAGER_H

#include "CClientCommon.h"

class CClientManager;
class CClientObject;
class CObject;

class CClientObjectManager
{
    friend class CClientManager;
    friend class CClientObject;

public:
    void                                    DoPulse();

    void                                    DeleteAll();

    inline unsigned int                     Count() const                                               { return m_Objects.size(); }
    inline unsigned int                     CountCreatedObjects() const                                 { return g_pGame->GetPools()->GetNumberOfUsedSpaces( OBJECT_POOL ); }
    static CClientObject*                   Get( ElementID ID );
    CClientObject*                          Get( CObject *object, bool bValidatePointer );
    CClientObject*                          GetSafe( CEntity *entity );

    static bool                             IsValidModel( unsigned short id );
    static bool                             IsBreakableModel( unsigned short id );
    bool                                    Exists( CClientObject *object ) const;
    bool                                    ObjectsAroundPointLoaded( const CVector& pos, float radius, unsigned short dimension, SString *pstrStatus = NULL );

    static bool                             IsObjectLimitReached();

    void                                    RestreamObjects( unsigned short id );

    typedef std::list <CClientObject*> objects_t;

    objects_t::const_iterator               IterGet( CClientObject *object ) const;
    objects_t::const_reverse_iterator       IterGetReverse( CClientObject *object ) const;
    objects_t::const_iterator               IterBegin() const                                           { return m_Objects.begin (); }
    objects_t::const_iterator               IterEnd() const                                             { return m_Objects.end (); }
    objects_t::const_reverse_iterator       IterReverseEnd() const                                      { return m_Objects.rend (); }

private:
                                            CClientObjectManager( class CClientManager *manager );
                                            ~CClientObjectManager();

    inline void                             AddToList( CClientObject* pObject )                         { m_Objects.push_back ( pObject ); }
    void                                    RemoveFromList( CClientObject* pObject );

    void                                    OnCreation( CClientObject * pObject );
    void                                    OnDestruction( CClientObject * pObject );

    void                                    UpdateLimitInfo();

    static unsigned int                     m_iEntryInfoNodeEntries;
    static unsigned int                     m_iPointerNodeSingleLinkEntries;
    static unsigned int                     m_iPointerNodeDoubleLinkEntries;

    class CClientManager*                   m_pManager;
    bool                                    m_bCanRemoveFromList;
    objects_t                               m_Objects;
    std::vector <CClientObject*>            m_StreamedIn;
    objects_t                               m_Attached;
};

#endif

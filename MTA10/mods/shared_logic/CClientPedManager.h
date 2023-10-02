/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPedManager.h
*  PURPOSE:     Ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CClientPedManager_H
#define __CClientPedManager_H

class CClientManager;
class CClientPed;
class CPlayerPed;
class CEntity;

class CClientPedManager
{
    friend class CClientManager;
    friend class CClientPed;

public:
    void                            DeleteAll();

    void                            DoPulse();
    CClientPed*                     Get( ElementID ID, bool bCheckPlayers = false );    
    CClientPed*                     Get( CPlayerPed *pPlayer, bool bValidatePointer, bool bCheckPlayers );
    CClientPed*                     GetSafe( CEntity *pEntity, bool bCheckPlayers );

    typedef std::vector <CClientPed*> peds_t;

    peds_t::const_iterator          IterBegin()                                 { return m_List.begin(); }
    peds_t::const_iterator          IterEnd()                                   { return m_List.end(); }
    peds_t::const_iterator          StreamedBegin()                             { return m_StreamedIn.begin(); }
    peds_t::const_iterator          StreamedEnd()                               { return m_StreamedIn.end(); }

protected:
                                    CClientPedManager( class CClientManager* pManager );
                                    ~CClientPedManager();

    inline void                     AddToList( CClientPed *pPed )               { m_List.push_back( pPed ); }
    void                            RemoveFromList( CClientPed *pPed );

    void                            OnCreation( CClientPed *pPed );
    void                            OnDestruction( CClientPed *pPed );

    CClientManager*                 m_pManager;
    peds_t                          m_List;    
    peds_t                          m_StreamedIn;
};

#endif
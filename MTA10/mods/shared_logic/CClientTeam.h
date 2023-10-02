/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTeam.h
*  PURPOSE:     Team entity class header
*  DEVELOPERS:  Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientTeam;

#ifndef __CCLIENTTEAM_H
#define __CCLIENTTEAM_H

#include "CClientEntity.h"
#include "CClientPlayer.h"
#include "CClientTeamManager.h"

#define LUACLASS_TEAM   56

class CClientTeam : public CClientEntity
{
    friend class CClientTeamManager;
public:    
                                CClientTeam( CClientManager* pManager, ElementID ID, lua_State *L, bool system, char* szName = NULL, unsigned char ucRed = 0, unsigned char ucGreen = 0, unsigned char ucBlue = 0 );
                                ~CClientTeam();

    eClientEntityType           GetType() const                                 { return CCLIENTTEAM; }

    void                        Unlink();

    inline unsigned char        GetID() const                                   { return m_ucID; }

    inline const char*          GetTeamName() const                             { return m_szTeamName; }
    void                        SetTeamName( const char *szName );

    void                        GetColor( unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue ) const;
    void                        SetColor( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue );

    inline bool                 GetFriendlyFire() const                         { return m_bFriendlyFire; }
    inline void                 SetFriendlyFire( bool bFriendlyFire )           { m_bFriendlyFire = bFriendlyFire; }

    void                        AddPlayer( CClientPlayer* pPlayer, bool bChangePlayer = false );
    void                        RemovePlayer( CClientPlayer* pPlayer, bool bChangePlayer = false );
    void                        RemoveAll();

    bool                        Exists( CClientPlayer* pPlayer );

    typedef std::list <CClientPlayer*> players_t;

    players_t::const_iterator   IterBegin() const                               { return m_List.begin(); }
    players_t::const_iterator   IterEnd() const                                 { return m_List.end(); }

    void                        GetPosition( CVector& pos ) const               { pos = m_vecPosition; }
    void                        SetPosition( const CVector& pos )               { m_vecPosition = pos; }

    inline unsigned int         CountPlayers() const                            { return m_List.size(); }

protected:                        
    CClientTeamManager*         m_pTeamManager;

    unsigned char               m_ucID;
    char*                       m_szTeamName;

    unsigned char               m_ucRed;
    unsigned char               m_ucGreen;
    unsigned char               m_ucBlue;

    bool                        m_bFriendlyFire;

    players_t                   m_List;  

    CVector                     m_vecPosition;
};

#endif
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayer.h
*  PURPOSE:     Player entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Oliver Brown <>
*               Kent Simon <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientPlayer;

#ifndef __CCLIENTPLAYER_H
#define __CCLIENTPLAYER_H

#include <game/CPlayerPed.h>

#include "CClientCommon.h"
#include "CClientPed.h"
#include "CClientPlayerVoice.h"

#define LUACLASS_PLAYER     66

#define NUM_PLAYER_STATS 343

class CClientPlayerManager;
class CClientTeam;

class CClientPlayer : public CClientPed
{
    friend class CClientPlayerManager;

public:
                                    CClientPlayer( CClientManager* pManager, ElementID ID, bool bIsLocalPlayer = false );
    virtual                         ~CClientPlayer();

    void                            Unlink();

    eClientEntityType               GetType() const                                                     { return CCLIENTPLAYER; }

    const char*                     GetNick() const                                                     { return m_szNick; }
    void                            SetNick( const char* szNick );

    inline unsigned int             GetPing() const                                                     { return ( m_bIsLocalPlayer ) ? g_pNet->GetPing() : m_uiPing; }
    inline void                     SetPing( unsigned int uiPing )                                      { m_uiPing = uiPing; }

    void                            GetNametagColor( unsigned char& ucR, unsigned char& ucG, unsigned char& ucB ) const;
    void                            SetNametagOverrideColor( unsigned char ucR, unsigned char ucG, unsigned char ucB );
    void                            RemoveNametagOverrideColor();
    inline bool                     IsNametagColorOverridden() const                                    { return m_bNametagColorOverridden; }

    inline const char*              GetNametagText() const                                              { return m_strNametag.c_str (); }
    inline void                     SetNametagText( const char* szText );
    inline bool                     IsNametagShowing() const                                            { return m_bNametagShowing; }
    inline void                     SetNametagShowing( bool bShowing )                                  { m_bNametagShowing = bShowing; }
    inline CGUIStaticImage*         GetStatusIcon()                                                     { return m_pStatusIcon; }
    inline unsigned long            GetLastNametagShow() const                                          { return m_ulLastNametagShow; }
    inline void                     SetLastNametagShow( unsigned long ulTime )                          { m_ulLastNametagShow = ulTime; }

    inline void                     SetIsExtrapolatingAim( bool m_bExtrap )                             { m_bDoExtrapolatingAim = m_bExtrap; }
    inline bool                     IsExtrapolatingAim() const                                          { return m_bDoExtrapolatingAim; }
    void                            UpdateAimPosition( const CVector &vecAim );

    inline unsigned short           GetLatency() const                                                  { return m_usLatency; }
    inline void                     SetLatency( unsigned short usLatency )                              { m_usLatency = (m_usLatency + usLatency) / 2; }

    inline unsigned long            GetLastPuresyncTime() const                                         { return m_ulLastPuresyncTime; }
    inline void                     SetLastPuresyncTime( unsigned long ulLastPuresyncTime )             { m_ulLastPuresyncTime = ulLastPuresyncTime; }
    inline const CVector&           GetLastPuresyncPosition() const                                     { return m_vecLastPuresyncPosition; }
    inline void                     SetLastPuresyncPosition( const CVector& vecPosition )               { m_vecLastPuresyncPosition = vecPosition; }
    inline bool                     HasConnectionTrouble() const                                        { return m_bHasConnectionTrouble; }
    inline void                     SetHasConnectionTrouble( bool bHasTrouble )                         { m_bHasConnectionTrouble = bHasTrouble; }

    inline void                     IncrementPlayerSync()                                               { ++m_uiPlayerSyncCount; }
    inline void                     IncrementKeySync()                                                  { ++m_uiKeySyncCount; }
    inline void                     IncrementVehicleSync()                                              { ++m_uiVehicleSyncCount; }

    inline unsigned int             GetPlayerSyncCount() const                                          { return m_uiPlayerSyncCount; }
    inline unsigned int             GetKeySyncCount() const                                             { return m_uiKeySyncCount; }
    inline unsigned int             GetVehicleSyncCount() const                                         { return m_uiVehicleSyncCount; }
    
    inline CClientTeam*             GetTeam()                                                           { return m_pTeam; }
    void                            SetTeam( CClientTeam *team, bool bChangeTeam = false );
    bool                            IsOnMyTeam( CClientPlayer *player ) const;

    CClientPlayerVoice*             GetVoice()                                                          { return m_voice; }

    inline float                    GetNametagDistance() const                                          { return m_fNametagDistance; }
    inline void                     SetNametagDistance( float fDistance )                               { m_fNametagDistance = fDistance; }

    inline bool                     IsDeadOnNetwork() const                                             { return m_bNetworkDead; }
    inline void                     SetDeadOnNetwork( bool bDead )                                      { m_bNetworkDead = bDead; }

    void                            Reset();

    inline CClientManager*          GetManager()                                                        { return m_pManager; }

private:
    bool                            m_bIsLocalPlayer;
    char                            m_szNick[MAX_PLAYER_NICK_LENGTH + 1];

    unsigned int                    m_uiPing;

    CVector                         m_vecTargetPosition;
    CVector                         m_vecTargetMoveSpeed;
    CVector                         m_vecTargetTurnSpeed;
    CVector                         m_vecTargetIncrements;
    unsigned int                    m_uiFramesSincePositionUpdate;

    unsigned short                  m_usLatency;
    CVector                         m_vecAimSpeed;
    CVector                         m_vecOldAim;
    CVector                         m_vecCurrentAim;
    unsigned long                   m_ulOldAimTime;
    unsigned long                   m_ulCurrentAimTime;
    unsigned long                   m_ulTick;
    bool                            m_bDoExtrapolatingAim;

    bool                            m_bForce;
    CVector                         m_vecForcedMoveSpeed;
    CVector                         m_vecForcedTurnSpeed;    

    unsigned long                   m_ulLastPuresyncTime;
    CVector                         m_vecLastPuresyncPosition;
    bool                            m_bHasConnectionTrouble;

    CClientTeam*                    m_pTeam;

    CGUIStaticImage*                m_pStatusIcon;
    bool                            m_bNametagShowing;
    unsigned long                   m_ulLastNametagShow;
    unsigned char                   m_ucNametagColorR, m_ucNametagColorG, m_ucNametagColorB;
    bool                            m_bNametagColorOverridden;
    std::string                     m_strNametag;

    unsigned int                    m_uiPlayerSyncCount;
    unsigned int                    m_uiKeySyncCount;
    unsigned int                    m_uiVehicleSyncCount;

    float                           m_fNametagDistance;

    bool                            m_bNetworkDead;

    CClientPlayerVoice*             m_voice;

#ifdef MTA_DEBUG

private:
    bool                            m_bShowingWepdata;

public:
    inline void                     SetShowingWepdata( bool bState )                                    { m_bShowingWepdata = bState; }
    inline bool                     IsShowingWepdata() const                                            { return m_bShowingWepdata; }

#endif
};

#endif

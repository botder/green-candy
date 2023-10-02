/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayer.cpp
*  PURPOSE:     Player entity class
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

#include <StdInc.h>

// TODO: The server is acting unexpected, as it reparents the players to resource dynamic root elements.
// This behaviour does not match the MTA:GREEN system, so we have to temporarily fix it here.
// We must write our own server!
static LUA_DECLARE( setParent )
{
    luaL_checktype( L, 1, LUA_TCLASS );

    CClientEntity *entity;

    if ( !lua_refclass( L, 1 )->GetTransmit( LUACLASS_ENTITY, (void*&)entity ) || entity != g_pClientGame->GetRootEntity() )
    {
        // Make sure we only accept the root element as parent!
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushmethodsuper( L );
    lua_pushvalue( L, 1 );
    lua_call( L, 1, 1 );
    return 1;
}

static const luaL_Reg player_interface_trans[] =
{
    LUA_METHOD( setParent ),
    { NULL, NULL }
};

static int luaconstructor_player( lua_State *L )
{
    CClientPlayer *player = (CClientPlayer*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_PLAYER, player );

    j.RegisterInterfaceTrans( L, player_interface_trans, 0, LUACLASS_PLAYER );

    lua_pushlstring( L, "player", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientPlayer::CClientPlayer( CClientManager *pManager, ElementID ID, bool bIsLocalPlayer ) : CClientPed( pManager, 0, ID, resMan->GetVM(), true, bIsLocalPlayer )
{
    // Lua instancing
    lua_State *L = resMan->GetVM();

    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_player, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Initialize
    m_pManager = pManager;
    m_bIsLocalPlayer = bIsLocalPlayer;

    SetTypeName( "player" );

    m_bNametagColorOverridden = false;
    m_uiPing = 0;

    m_szNick[0] = '\0';

    m_usLatency = 0;

    m_uiPlayerSyncCount = 0;
    m_uiKeySyncCount = 0;
    m_uiVehicleSyncCount = 0;

    m_ulLastPuresyncTime = 0;
    m_bHasConnectionTrouble = false;    

    m_bForce = false;
    m_bDoExtrapolatingAim = false;

    m_bNetworkDead = false;

    m_voice = NULL;

    // If it's the local player, add the model
    if ( m_bIsLocalPlayer )
    {
        // Set us as the local player in the player manager
        if ( !pManager->GetPlayerManager()->GetLocalPlayer() )
            pManager->GetPlayerManager()->SetLocalPlayer( this );

#ifdef VOICE_DEBUG_LOCAL_PLAYBACK
        if ( g_pClientGame->GetVoiceRecorder()->IsEnabled() )
            m_voice = new CClientPlayerVoice( this, g_pClientGame->GetVoiceRecorder() );
#endif
    }
    else
    {
        // Enable weapon processing for players
        m_remoteDataStorage->SetProcessPlayerWeapon( true );

        // Enable voice playback for remote players
        if ( g_pClientGame->GetVoiceRecorder()->IsEnabled() ) //If voice is enabled
            m_voice = new CClientPlayerVoice( this, g_pClientGame->GetVoiceRecorder() );
    }

    // Set all our default stats
    m_pTeam = NULL;

    m_bNametagShowing = true;
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_ulLastNametagShow = 0;
    SetNametagText( m_szNick );
    
    // Create the static icon (defaults to a warning icon for network trouble)
    m_pStatusIcon = g_pCore->GetGUI()->CreateStaticImage();
    m_pStatusIcon->SetSize( CVector2D( 16, 16 ) );
    m_pStatusIcon->SetVisible( false );

    // Could a connection trouble texture be loaded? Load it into the status icon.
    CGUITexture* pTexture = m_pManager->GetConnectionTroubleTexture();
    if ( pTexture )
    {
        m_pStatusIcon->LoadFromTexture( pTexture );
    }   

    // Add us to the player list
    m_pManager->GetPlayerManager()->AddToList( this );

#ifdef MTA_DEBUG
    m_bShowingWepdata = false;
#endif
}

CClientPlayer::~CClientPlayer()
{
    // Remove us from the team
    if ( m_pTeam )
        m_pTeam->RemovePlayer( this );

    // Remove us from the player list
    Unlink();

    // Remove the icon
    if ( m_pStatusIcon )
    {
        m_pStatusIcon->SetVisible( false );

        delete m_pStatusIcon;
        m_pStatusIcon = NULL;
    }

    if ( m_voice )
        delete m_voice;
}

void CClientPlayer::Unlink()
{
    m_pManager->GetPlayerManager()->RemoveFromList( this );
}

void CClientPlayer::UpdateAimPosition( const CVector &vecAim )
{ 
    m_vecOldAim = m_vecCurrentAim;
    m_vecCurrentAim = vecAim;
    m_ulOldAimTime = m_ulCurrentAimTime;
    m_ulCurrentAimTime = GetTickCount32();
    m_vecAimSpeed = GetExtrapolatedSpeed( m_vecOldAim, m_ulOldAimTime, m_vecCurrentAim, m_ulCurrentAimTime );
}

void CClientPlayer::SetNick( const char *szNick )
{
    strncpy( m_szNick, szNick, MAX_PLAYER_NICK_LENGTH );
    m_szNick[MAX_PLAYER_NICK_LENGTH] = '\0';

    if ( m_strNametag.empty() )
        m_strNametag = szNick;
}

void CClientPlayer::GetNametagColor( unsigned char& r, unsigned char& g, unsigned char& b ) const
{
    // If the nametag color is overridden, use the override color
    if ( m_bNametagColorOverridden )
    {
        r = m_ucNametagColorR;
        g = m_ucNametagColorG;
        b = m_ucNametagColorB;
        return;
    }

    // Otherwize take the color from the team if any
    if ( m_pTeam )
        m_pTeam->GetColor( r, g, b );
    else
    {
        // No team and no override? Default color is white
        r = 255;
        g = 255;
        b = 255;
    }
}

void CClientPlayer::SetNametagOverrideColor( unsigned char ucR, unsigned char ucG, unsigned char ucB )
{
    m_ucNametagColorR = ucR;
    m_ucNametagColorG = ucG;
    m_ucNametagColorB = ucB;
    m_bNametagColorOverridden = true;
}

void CClientPlayer::RemoveNametagOverrideColor()
{
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_bNametagColorOverridden = false;
}

void CClientPlayer::SetTeam( CClientTeam *pTeam, bool bChangeTeam )
{
    if ( pTeam == m_pTeam )
        return;

    if ( m_pTeam && bChangeTeam )
        m_pTeam->RemovePlayer( this, false );

    m_pTeam = pTeam;

    if ( m_pTeam && bChangeTeam )
        m_pTeam->AddPlayer( this, false );
}

bool CClientPlayer::IsOnMyTeam( CClientPlayer *player ) const
{
    return m_pTeam && m_pTeam == player->GetTeam();
}

void CClientPlayer::Reset()
{
    // stats
    for ( unsigned short us = 0; us <= NUM_PLAYER_STATS; us++ )
    {
        if ( us == MAX_HEALTH )
            SetStat( us, 569.0f );
        else
            SetStat( us, 0.0f );
    }

    // model
    SetModel( 0 );      
    GetClothes()->DefaultClothes( false );
    SetHasJetPack( false );
    SetFightingStyle( STYLE_GRAB_KICK );

    RebuildModel( true );

    SetNametagText( m_szNick );
    m_bNametagShowing = true;

    // Otherwize default to white
    m_ucNametagColorR = 255;
    m_ucNametagColorG = 255;
    m_ucNametagColorB = 255;
    m_bNametagColorOverridden = false;

    SetAlpha( 255 );

    if ( m_pStatusIcon )
        m_pStatusIcon->SetVisible( false );
}

void CClientPlayer::SetNametagText( const char *szText )
{
    m_strNametag = szText;
}
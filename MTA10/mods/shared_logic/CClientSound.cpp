/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientSound.cpp
*  PURPOSE:     Sound entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               arc_
*               Florian Busse <flobu@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CBassAudio.h"

static const luaL_Reg sound_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_sound( lua_State *L )
{
    CClientSound *sound = (CClientSound*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_SOUND, sound );

    j.RegisterInterfaceTrans( L, sound_interface, 0, LUACLASS_SOUND );

    lua_pushlstring( L, "sound", 5 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientSound::CClientSound( CClientManager* pManager, ElementID ID, lua_State *L ) : CClientEntity( ID, false, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_sound, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_pSoundManager = pManager->GetSoundManager();
    m_pAudio = NULL;

    SetTypeName( "sound" );

    m_pSoundManager->AddToList( this );

    m_fVolume = 1.0f;
    m_fMinDistance = 5.0f;
    m_fMaxDistance = 20.0f;
    m_fPlaybackSpeed = 1.0f;

    // Set up the effects array
    for ( unsigned char n = 0; n < NUMELMS( m_EnabledEffects ); n++ )
        m_EnabledEffects[n] = 0;
}

CClientSound::~CClientSound ( void )
{
    Destroy ();
    m_pSoundManager->RemoveFromList ( this );
}


////////////////////////////////////////////////////////////
//
// CClientSound::GetWorldBoundingSphere
//
// For spatial database
//
////////////////////////////////////////////////////////////
CSphere CClientSound::GetWorldBoundingSphere ( void )
{
    return CSphere ( m_vecPosition, m_fMaxDistance );
}


////////////////////////////////////////////////////////////
//
// CClientSound::DistanceStreamIn
//
// Sound is now close enough to be heard, so must be activated
//
////////////////////////////////////////////////////////////
void CClientSound::DistanceStreamIn ( void )
{
    if ( !m_pAudio )
    {
        Create ();
        m_pSoundManager->OnDistanceStreamIn ( this );

        // Call Stream In event
        CallEvent( "onClientElementStreamIn", m_lua, 0 );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::DistanceStreamOut
//
// Sound is now far enough away to not be heard, so can be deactivated
//
////////////////////////////////////////////////////////////
void CClientSound::DistanceStreamOut ( void )
{
    if ( m_pAudio )
    {
        m_pSoundManager->OnDistanceStreamOut ( this );
        Destroy ();

        // Call Stream Out event
        CallEvent( "onClientElementStreamOut", m_lua, 0 );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::Create
//
// Create underlying audio
//
////////////////////////////////////////////////////////////
bool CClientSound::Create()
{
    if ( m_pAudio )
        return false;

    // Initial state
    m_pAudio = new CBassAudio ( m_bStream, m_strPath, m_bLoop, m_b3D );
    m_bDoneCreate = true;

    // Load file/start connect
    if ( !m_pAudio->BeginLoadingMedia () )
        return false;

    // Get and save length
    m_dLength = m_pAudio->GetLength ();

    // Transfer dynamic state
    m_pAudio->SetVolume ( m_fVolume );
    m_pAudio->SetPlaybackSpeed ( m_fPlaybackSpeed );
    m_pAudio->SetPosition ( m_vecPosition );
    m_pAudio->SetVelocity ( m_vecVelocity );
    m_pAudio->SetMinDistance ( m_fMinDistance );
    m_pAudio->SetMaxDistance ( m_fMaxDistance );
    m_pAudio->SetFxEffects ( m_EnabledEffects, NUMELMS( m_EnabledEffects ) );

    // Transfer play position if it was being simulated
    EndSimulationOfPlayPositionAndApply ();

    //
    // Note:
    //   m_pAudio does not actually start until the next call to m_pAudio->DoPulse.
    //   This is to allow for settings to be changed before playback, avoiding sound pops etc.
    //
    return true;
}


////////////////////////////////////////////////////////////
//
// CClientSound::Destroy
//
// Destroy underlying audio
//
////////////////////////////////////////////////////////////
void CClientSound::Destroy()
{
    if ( !m_pAudio )
        return;

    BeginSimulationOfPlayPosition();

    delete m_pAudio;
    m_pAudio = NULL;
}


////////////////////////////////////////////////////////////
//
// CClientSound::BeginSimulationOfPlayPosition
//
//
//
////////////////////////////////////////////////////////////
void CClientSound::BeginSimulationOfPlayPosition()
{
    // Only 3d sounds will be distance streamed in and out. Also streams can't be seeked.
    // So only non-streamed 3D sounds need the play position simulated.
    if ( m_b3D && !m_bStream )
    {
        m_SimulatedPlayPosition.SetLooped ( m_bLoop );
        m_SimulatedPlayPosition.SetLength ( m_dLength );
        m_SimulatedPlayPosition.SetPaused ( m_bPaused );
        m_SimulatedPlayPosition.SetPlaybackSpeed( GetPlaybackSpeed () );
        m_SimulatedPlayPosition.SetPlayPositionNow ( GetPlayPosition () );
        m_SimulatedPlayPosition.SetValid ( true );
    }
}


////////////////////////////////////////////////////////////
//
// CClientSound::EndSimulationOfPlayPositionAndApply
//
//
//
////////////////////////////////////////////////////////////

void CClientSound::EndSimulationOfPlayPositionAndApply()
{
    if ( m_SimulatedPlayPosition.IsValid() )
    {
        m_SimulatedPlayPosition.SetLength( m_dLength );
        m_pAudio->SetPlayPosition( m_SimulatedPlayPosition.GetPlayPositionNow() );
        m_SimulatedPlayPosition.SetValid( false );
    }
}

////////////////////////////////////////////////////////////
//
// CClientSound::Play and pals
//
//
//
////////////////////////////////////////////////////////////

bool CClientSound::Play ( const SString& strPath, bool bLoop )
{
    m_bStream = false;
    m_b3D = false;
    m_strPath = strPath;
    m_bLoop = bLoop;

    // Instant distance-stream in
    return Create ();
}

bool CClientSound::Play3D ( const SString& strPath, bool bLoop )
{
    m_bStream = false;
    m_b3D = true;
    m_strPath = strPath;
    m_bLoop = bLoop;

    BeginSimulationOfPlayPosition ();

    return true;
}

void CClientSound::PlayStream ( const SString& strURL, bool bLoop, bool b3D )
{
    m_bStream = true;
    m_b3D = b3D;
    m_strPath = strURL;
    m_bLoop = bLoop;

    // Instant distance-stream in if not 3D
    if ( !m_b3D )
        Create ();
}

void CClientSound::SetPlayPosition( double dPosition )
{
    if ( m_pAudio )
    {
        // Use actual audio if active
        m_pAudio->SetPlayPosition ( dPosition );
    }
    else
    {
        // Use simulation if not active
        m_SimulatedPlayPosition.SetPlayPositionNow ( dPosition );
    }
}

double CClientSound::GetPlayPosition() const
{
    if ( m_pAudio )
    {
        // Use actual audio if active
        return m_pAudio->GetPlayPosition();
    }
    else if ( m_SimulatedPlayPosition.IsValid() )
    {
        // Use simulation if not active
        return m_SimulatedPlayPosition.GetPlayPositionNow();
    }

    return 0;
}

double CClientSound::GetLength()
{
    if ( !m_bDoneCreate && !m_bStream )
    {
        // If never loaded, do a create and destroy to get the length
        Create();
        Destroy();
    }
    return m_dLength;
}

float CClientSound::GetVolume() const
{
    return m_fVolume;
}

void CClientSound::SetVolume( float fVolume, bool bStore )
{
   m_fVolume = fVolume;

    if ( m_pAudio )
        m_pAudio->SetVolume( m_fVolume );
}

float CClientSound::GetPlaybackSpeed() const
{
    return m_fPlaybackSpeed;
}

void CClientSound::SetPlaybackSpeed( float fSpeed )
{
    m_fPlaybackSpeed = fSpeed;
    m_SimulatedPlayPosition.SetPlaybackSpeed( fSpeed );

    if ( m_pAudio )
        m_pAudio->SetPlaybackSpeed ( m_fPlaybackSpeed );
}

void CClientSound::SetPosition( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
    UpdateSpatialData();

    if ( m_pAudio )
        m_pAudio->SetPosition( m_vecPosition );
}

void CClientSound::GetPosition( CVector& vecPosition ) const
{
    vecPosition = m_vecPosition;
}

void CClientSound::SetVelocity( const CVector& vecVelocity )
{
    m_vecVelocity = vecVelocity;
    if ( m_pAudio )
        m_pAudio->SetVelocity( m_vecVelocity );
}

void CClientSound::GetVelocity( CVector& vecVelocity ) const
{
    vecVelocity = m_vecVelocity;
}

void CClientSound::SetPaused( bool bPaused )
{
    m_bPaused = bPaused;

    m_SimulatedPlayPosition.SetPaused ( bPaused );

    if ( m_pAudio )
        m_pAudio->SetPaused( m_bPaused );
}

bool CClientSound::IsPaused() const
{
    return m_bPaused;
}

void CClientSound::SetMinDistance( float fDistance )
{
    m_fMinDistance = fDistance;
    if ( m_pAudio )
        m_pAudio->SetMinDistance ( m_fMinDistance );
}

float CClientSound::GetMinDistance() const
{
    return m_fMinDistance;
}

void CClientSound::SetMaxDistance( float fDistance )
{
    bool bChanged = m_fMaxDistance != fDistance;

    m_fMaxDistance = fDistance;
    if ( m_pAudio )
        m_pAudio->SetMaxDistance( m_fMaxDistance );

    if ( bChanged )
        UpdateSpatialData ();
}

float CClientSound::GetMaxDistance() const
{
    return m_fMaxDistance;
}

////////////////////////////////////////////////////////////
//
// CClientSound::GetMetaTags
//
// If the stream is not active, this may not work correctly
//
////////////////////////////////////////////////////////////

SString CClientSound::GetMetaTags( const SString& strFormat )
{
    SString strMetaTags = "";

    if ( m_pAudio )
    {
        strMetaTags = m_pAudio->GetMetaTags( strFormat );
        m_SavedTags[ strFormat ] = strMetaTags;
    }
    else
    {
        // Search previously found tags for this stream when it is not active
        // This may not be such a good idea btw
        if ( SString* pstrMetaTags = MapFind( m_SavedTags, strFormat ) )
            strMetaTags = *pstrMetaTags;
    }

    return strMetaTags;
}

////////////////////////////////////////////////////////////
//
// CClientSound::SetFxEffect
//
// TODO and test
//
////////////////////////////////////////////////////////////

bool CClientSound::SetFxEffect( uint uiFxEffect, bool bEnable )
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;

    m_EnabledEffects[uiFxEffect] = bEnable;

    if ( m_pAudio )
        m_pAudio->SetFxEffects ( m_EnabledEffects, NUMELMS( m_EnabledEffects ) );

    return true;
}

bool CClientSound::IsFxEffectEnabled( uint uiFxEffect ) const
{
    if ( uiFxEffect >= NUMELMS( m_EnabledEffects ) )
        return false;

    return m_EnabledEffects[uiFxEffect] ? true : false;
}

////////////////////////////////////////////////////////////
//
// CClientSound::Process3D
//
// Update position and velocity and pass on the BASS for processing.
// m_pAudio->DoPulse needs to be called for non-3D sounds also.
//
////////////////////////////////////////////////////////////
void CClientSound::Process3D( const CVector& vecPlayerPosition, const CVector& vecCameraPosition, const CVector& vecLookAt )
{
    // If the sound isn't active, we don't need to process it
    if ( !m_pAudio )
        return;

    // Update 3D things if required
    if ( m_b3D )
    {
        // Update our position and velocity if we're attached
        CClientEntity* pAttachedToEntity = GetAttachedTo ();
        if ( pAttachedToEntity )
        {
            DoAttaching ();
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity ( *pAttachedToEntity, vecVelocity ) )
                SetVelocity ( vecVelocity );
        }
    }

    m_pAudio->DoPulse ( vecPlayerPosition, vecCameraPosition, vecLookAt );

    // Trigger script events for things
    SSoundEventInfo eventInfo;

    while ( m_pAudio->GetQueuedEvent ( eventInfo ) )
    {
        if ( eventInfo.type == SOUND_EVENT_FINISHED_DOWNLOAD )
        {
            lua_pushnumber( m_lua, eventInfo.dNumber );
            CallEvent( "onClientSoundFinishedDownload", m_lua, 1 );
            OutputDebugLine ( SString ( "onClientSoundFinishedDownload %f", eventInfo.dNumber ) );
        }
        else if ( eventInfo.type == SOUND_EVENT_CHANGED_META )
        {
            lua_pushlstring( m_lua, eventInfo.strString.c_str(), eventInfo.strString.size() );
            CallEvent( "onClientSoundChangedMeta", m_lua, 1 );
            OutputDebugLine ( SString ( "onClientSoundChangedMeta %s", *eventInfo.strString ) );
        }
        else if ( eventInfo.type == SOUND_EVENT_STREAM_RESULT )
        {
            // Call onClientSoundStream LUA event
            lua_pushboolean( m_lua, eventInfo.bBool );
            lua_pushnumber( m_lua, eventInfo.dNumber );

            if ( !eventInfo.strString.empty() )
                lua_pushlstring( m_lua, eventInfo.strString.c_str(), eventInfo.strString.size() );
            else
                lua_pushnil( m_lua );

            CallEvent( "onClientSoundStream", m_lua, 3 );
            OutputDebugLine ( SString ( "onClientSoundStream %d %f %s", eventInfo.bBool, eventInfo.dNumber, *eventInfo.strString ) );
        }
    }
}
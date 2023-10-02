/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCamera.cpp
*  PURPOSE:     Camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define PI_2 6.283185307179586476925286766559f
#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif

static LUA_DECLARE( isSphereVisible )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 4 );

    RwSphere sphere;
    sphere.pos = CVector( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) );
    sphere.radius = (float)lua_tonumber( L, 4 );

    lua_pushboolean( L, ((CClientCamera*)lua_getmethodtrans( L ))->IsSphereVisible( sphere ) );
    return 1;
}

static const luaL_Reg camera_interface[] =
{
    LUA_METHOD( isSphereVisible ),
    { NULL, NULL }
};

static int luaconstructor_camera( lua_State *L )
{
    CClientCamera *cam = (CClientCamera*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_CAMERA, cam );

    j.RegisterInterfaceTrans( L, camera_interface, 0, LUACLASS_CAMERA );

    lua_pushlstring( L, "camera", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientCamera::CClientCamera( CClientManager* pManager ) : CClientEntity( INVALID_ELEMENT_ID, true, resMan->GetVM() )
{
    lua_State *L = resMan->GetVM();

    // Setup Lua instance
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_camera, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientCamera" ), &m_pFocusedPlayer, &m_pFocusedEntity, NULL );

    // Init
    m_pManager = pManager;
    m_pPlayerManager = m_pManager->GetPlayerManager ();

    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bInvalidated = false;
    m_bFixed = false;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;

    SetTypeName( "camera" );

    m_pCamera = g_pGame->GetCamera ();

    // Hook handler for the fixed camera
    g_pMultiplayer->SetProcessCamHandler ( CClientCamera::ProcessFixedCamera );
}

CClientCamera::~CClientCamera()
{
    m_pManager->m_pCamera = NULL;

    // We need to be ingame
    if ( g_pGame->GetSystemState() == GS_PLAYING_GAME )
    {
        // Restore the camera to the local player
        SetFocusToLocalPlayerImpl ();
    }

    CClientEntityRefManager::RemoveEntityRefs( 0, &m_pFocusedPlayer, &m_pFocusedEntity, NULL );
}

void CClientCamera::DoPulse ( void )
{   
    // If we're fixed, force the target vector
    if ( m_bFixed )
    {
        // Make sure the world center/rotation is where the camera is
        CVector vecRotation;
        RwMatrix matTemp;
        GetMatrix( matTemp );
        matTemp.GetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );    
        g_pGame->SetCenterOfWorld ( NULL, &m_matrix.vPos, (float)M_PI - vecRotation.fZ );
    }
    else
    {
        // If we aren't invalidated
        if ( !m_bInvalidated )
        {
            // If we got the camera behind a player but no focused entity
            if ( m_pFocusedPlayer )
            {
                if ( m_pFocusedEntity )
                {
                    // Is the focused entity a vehicle, but the player doesn't have any occupied?
                    CClientVehicle* pVehicle = m_pFocusedPlayer->GetOccupiedVehicle ();
                    if ( m_pFocusedEntity->GetType () == CCLIENTVEHICLE )
                    {
                        if ( !pVehicle )
                        {
                            SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                        }
                    }
                    else if ( pVehicle )
                    {
                        SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                    }
                }
                else
                {
                    SetFocus ( m_pFocusedPlayer, MODE_BEHINDCAR );
                }
            }

            // Make sure the world center is where the camera is
            if ( m_pFocusedGameEntity )
            {
                // Make sure we have the proper rotation for what we're spectating
                float fRotation = 0;
                if ( m_pFocusedEntity )
                {
                    eClientEntityType eType = m_pFocusedEntity->GetType ();
                    if ( eType == CCLIENTVEHICLE )
                    {
                        CVector vecVehicleRotation;
                        static_cast < CClientVehicle* > ( m_pFocusedEntity )->GetRotationRadians ( vecVehicleRotation );
                        fRotation = vecVehicleRotation.fZ * 3.14159f / 180;
                    }
                    else if ( eType == CCLIENTPED || eType == CCLIENTPLAYER )
                    {
                        fRotation = static_cast < CClientPed* > ( m_pFocusedEntity )->GetCurrentRotation ();
                    }
                }

                // Set the new world center/rotation
                CVector pos;
                m_pFocusedGameEntity->GetPosition( pos );

                g_pGame->SetCenterOfWorld ( NULL, &pos, fRotation );
            }
        }
    }
}

void CClientCamera::AssignFixedMode()
{
    if ( !IsInFixedMode() )
        ToggleCameraFixedMode( true );

    // Make sure that's where the world center is
    CVector vecRotation;
    m_matrix.GetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    g_pGame->SetCenterOfWorld( NULL, &m_matrix.vPos, PI - vecRotation.fZ );
}

bool CClientCamera::SetMatrix( const RwMatrix& mat )
{
    m_matrix = mat;

    // Update into fixed mode
    AssignFixedMode();
    return true;
}

bool CClientCamera::GetMatrix( RwMatrix& Matrix ) const
{
    if ( !IsInFixedMode() )
        Matrix = m_pCamera->GetMatrix();
    else
        Matrix = m_matrix;

    return true;
}

void CClientCamera::GetPosition( CVector& vecPosition ) const
{
    if ( m_bFixed )
        vecPosition = m_matrix.vPos;
    else
        vecPosition = m_pCamera->GetMatrix().vPos;
}

void CClientCamera::SetPosition( const CVector& vecPosition )
{
    // Store the position so it can be updated from our hook
    m_matrix.vPos = vecPosition;

    // Switch into fixed mode
    AssignFixedMode();
}

void CClientCamera::GetRotation( CVector& vecRotation ) const
{
    CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
    m_pCamera->GetMatrix().GetRotation( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    vecRotation += CVector ( 180.0f, 180.0f, 180.0f );

    if ( vecRotation.fX > 360.0f ) vecRotation.fX -= 360.0f;
    if ( vecRotation.fY > 360.0f ) vecRotation.fY -= 360.0f;
    if ( vecRotation.fZ > 360.0f ) vecRotation.fZ -= 360.0f;
}

void CClientCamera::SetRotation ( const CVector& vecRotation )
{
    // Rotate a 1000,0,0 vector with the given rotation vector
    CVector vecRotationCopy = vecRotation;
    vecRotationCopy.fX = 0.0f;
    CVector vecNormal = CVector ( 1000.0f, 0.0f, 0.0f );
    RotateVector ( vecNormal, vecRotationCopy );

    // Add it with our current position
    CVector vecPosition;
    GetPosition ( vecPosition );
    vecPosition += vecNormal;

    // Set the calculated vector as the target
    SetTarget ( vecPosition );
}

void CClientCamera::GetTarget ( CVector & vecTarget ) const
{
    CCam* pCam = m_pCamera->GetCam( m_pCamera->GetActiveCam() );
    vecTarget = m_pCamera->GetMatrix().vPos + *pCam->GetFront();
}

void CClientCamera::SetTarget( const CVector& vecTarget )
{
    // Calculate the front vector, target - position. If its length is 0 we'll get problems
    // (i.e. if position and target are the same), so make a new vector when looking horizontally
    m_matrix.vFront = vecTarget - m_matrix.vPos;
    if ( m_matrix.vFront.Length() < FLOAT_EPSILON )
        m_matrix.vFront = CVector( 0.0, 1.0f, 0.0f );
    else
        m_matrix.vFront.Normalize();

    // Grab the right vector
    m_matrix.vRight = CVector( m_matrix.vFront.fY, -m_matrix.vFront.fX, 0 );
    if ( m_matrix.vRight.Length() < FLOAT_EPSILON )
        m_matrix.vRight = CVector( 1.0f, 0.0f, 0.0f );
    else
        m_matrix.vRight.Normalize();

    // Calculate the up vector from this
    m_matrix.vUp = m_matrix.vRight;
    m_matrix.vUp.CrossProduct( &m_matrix.vFront );
    m_matrix.vUp.Normalize();

    // Apply roll if needed
    if ( m_fRoll != 0.0f )
    {
        float fRoll = ConvertDegreesToRadiansNoWrap( m_fRoll );
        m_matrix.vUp = m_matrix.vUp * cos( fRoll ) - m_matrix.vRight * sin( fRoll );
    }
}

void CClientCamera::FadeIn( float fTime )
{
    m_pCamera->Fade ( fTime, FADE_IN );
}

void CClientCamera::FadeOut( float fTime, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    m_pCamera->SetFadeColor( ucRed, ucGreen, ucBlue );
    m_pCamera->Fade( fTime, FADE_OUT );
}

void CClientCamera::SetFocus ( CClientEntity* pEntity, eCamMode eMode, bool bSmoothTransition )
{
    // Should we attach to an entity, or set the camera to free?
    if ( pEntity )
    {
        eClientEntityType eType = pEntity->GetType ();

        // Remove stream reference from the previous target
        if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

        // Add stream reference for our new target
        if ( pEntity && pEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( pEntity)->AddStreamReference ();

        CEntity* pGameEntity = pEntity->GetGameEntity (); 
        if ( pGameEntity )
        {
            // Set the switch style
            int iSwitchStyle = 2;
            if ( bSmoothTransition )
            {
                iSwitchStyle = 1;
            }

            // Hacky, used to follow peds
            //if ( eMode == MODE_CAM_ON_A_STRING && eType == CCLIENTPLAYERMODEL )
            if ( eMode == MODE_BEHINDCAR && ( eType == CCLIENTPED || eType == CCLIENTPLAYER ) )
                eMode = MODE_FOLLOWPED;

            // Do it
            m_pCamera->TakeControl ( pGameEntity, eMode, iSwitchStyle );            

            // Store
            m_pFocusedEntity = pEntity;
            m_pFocusedGameEntity = pGameEntity;
            m_bFixed = false;
        }
    }
    else
    {
        // ## TODO ## : Don't attach to anything (free camera controlled by SetPosition and SetRotation)
        // Restore the camera to the local player at the moment
        SetFocusToLocalPlayer ();
    }

    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
}

void CClientCamera::SetFocus ( CClientPlayer* pPlayer, eCamMode eMode, bool bSmoothTransition )
{
    // Should we attach to a player, or set the camera to free?
    if ( pPlayer )
    {
        // Attach the camera to the player vehicle if in a vehicle. To the player model otherwize
        CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        if ( pVehicle )
        {
            SetFocus ( pVehicle, eMode, bSmoothTransition );
        }
        else
        {
            // Cast it so we don't call ourselves...
            SetFocus ( static_cast < CClientEntity* > ( pPlayer ), eMode, bSmoothTransition );
        }
    }
    else
    {
        SetFocus ( reinterpret_cast < CClientEntity* > ( NULL ), MODE_NONE, bSmoothTransition );
    }

    // Store the player we focused
    m_pFocusedPlayer = pPlayer;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
    m_bFixed = false;
}

void CClientCamera::SetFocus ( CVector& vecTarget, bool bSmoothTransition )
{
    // Grab the camera
    if ( m_pCamera )
    {
        // Set the switch style
        int iSwitchStyle = 2;
        if ( bSmoothTransition )
        {
            iSwitchStyle = 1;
        }
        // Do it
        m_pCamera->TakeControl ( vecTarget, iSwitchStyle );
        m_bFixed = true;
    }
}

void CClientCamera::SetFocusToLocalPlayer()
{
    // Restore the camera
    SetFocusToLocalPlayerImpl ();

    // Remove stream reference from the previous target
    if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
        static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

    // Reset
    m_pFocusedPlayer = NULL;
    m_pFocusedEntity = NULL;
    m_pFocusedGameEntity = NULL;
    m_bFixed = false;
    m_fRoll = 0.0f;
    m_fFOV = 70.0f;
}

void CClientCamera::SetFocusToLocalPlayerImpl()
{
    // Restore the camera
    m_pCamera->RestoreWithJumpCut ();
    g_pGame->SetCenterOfWorld ( NULL, NULL, NULL );
}

void CClientCamera::UnreferenceEntity( CClientEntity* pEntity )
{
    if ( m_pFocusedEntity == pEntity )
    {
        SetFocusToLocalPlayerImpl ();

        // Remove stream reference from the previous target
        if ( m_pFocusedEntity && m_pFocusedEntity->IsStreamingCompatibleClass () )
            static_cast < CClientStreamElement* > ( m_pFocusedEntity )->RemoveStreamReference ();

        m_pFocusedEntity = NULL;
        m_pFocusedGameEntity = NULL;
        m_bInvalidated = false;
    }
}

void CClientCamera::UnreferencePlayer( CClientPlayer* pPlayer )
{
    if ( m_pFocusedPlayer == pPlayer )
    {
        SetFocusToLocalPlayerImpl ();
        m_pFocusedPlayer = NULL;
    }
}

bool CClientCamera::IsSphereVisible( const RwSphere& sphere ) const
{
    return m_pCamera->IsSphereVisible( sphere );
}

void CClientCamera::InvalidateEntity ( CClientEntity* pEntity )
{
    if ( !m_bInvalidated )
    {
        if ( m_pFocusedEntity && m_pFocusedEntity == pEntity )
        {
            SetFocusToLocalPlayerImpl ();
            m_bInvalidated = true;
        }
    }
}

void CClientCamera::RestoreEntity ( CClientEntity* pEntity )
{
    if ( m_bInvalidated )
    {
        if ( m_pFocusedEntity && m_pFocusedEntity == pEntity )
        {
            SetFocus ( pEntity, MODE_BEHINDCAR );
            m_bInvalidated = false;
        }
    }
}

void CClientCamera::SetCameraViewMode ( eVehicleCamMode eMode )
{
    m_pCamera->SetCameraViewMode ( eMode );
}

eVehicleCamMode CClientCamera::GetCameraViewMode ()
{
    return (eVehicleCamMode)m_pCamera->GetCameraViewMode();
}

void CClientCamera::SetCameraClip ( bool bObjects, bool bVehicles )
{
    m_pCamera->SetCameraClip ( bObjects, bVehicles );
}

void CClientCamera::SetVehicleInterpolationSource( CClientVehicle *veh )
{
    m_pCamera->GetCam( 0 )->SetVehicleInterpolationSource( dynamic_cast <CVehicle*> ( veh->GetGameEntity() ) );
}

void CClientCamera::ToggleCameraFixedMode ( bool bEnabled )
{    
    if ( bEnabled )
    {
        CClientPlayer* pLocalPlayer = m_pManager->GetPlayerManager()->GetLocalPlayer ();
        CClientVehicle* pLocalVehicle = NULL;

        // Get the local vehicle, if any
        if ( pLocalPlayer )
            pLocalVehicle = pLocalPlayer->GetOccupiedVehicle ();

        // Use the local vehicle, otherwise use the local player
        if ( pLocalVehicle )
            SetFocus ( pLocalVehicle, MODE_FIXED, false );
        else
            SetFocus ( pLocalPlayer, MODE_FIXED, false );

        // Set the target position
        SetFocus( m_matrix.vPos, false );
    }
    else
    {
        g_pGame->SetCenterOfWorld ( NULL, NULL, NULL );
        SetFocusToLocalPlayer();

        m_fRoll = 0.0f;
        m_fFOV = 70.0f;
    }
}

CClientEntity* CClientCamera::GetTargetEntity()
{
    CClientEntity* pReturn = NULL;
    if ( m_pCamera )
    {
        CCam* pCam = m_pCamera->GetCam ( m_pCamera->GetActiveCam () );
        CEntity * pEntity = pCam->GetTargetEntity ();
        if ( pEntity )
        {
            pReturn = m_pManager->FindEntitySafe ( pEntity );            
        }
    }
    return pReturn;
}

bool CClientCamera::ProcessFixedCamera( CCam* pCam )
{
    // The purpose of this handler function is changing the Source, Front and Up vectors in CCam
    // when called by GTA. This is called when we are in fixed camera mode.
    CClientCamera* pThis = g_pClientGame->GetManager ()->GetCamera ();

    // Make sure we actually want to apply our custom camera position/lookat
    // (this handler could also be called from cinematic mode)
    if ( !pThis->m_bFixed )
        return false;

    // Update the display parameters
    *pCam->GetSource() = pThis->m_matrix.vPos;
    *pCam->GetFront() = pThis->m_matrix.vFront;
    *pCam->GetUp() = pThis->m_matrix.vUp;

    // Set the zoom
    pCam->SetFOV( pThis->m_fFOV );
    return true;
}

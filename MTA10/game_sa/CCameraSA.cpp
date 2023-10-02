/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCameraSA.cpp
*  PURPOSE:     Camera rendering
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
/**
 * \todo Need to add other TakeControl functions
 */

static bool bCameraClipObjects;
static bool bCameraClipVehicles;

#define VAR_CameraClipVehicles              0x8A5B14
#define VAR_CameraClipDynamicObjects        0x8A5B15
#define VAR_CameraClipStaticObjects         0x8A5B16

#define HOOKPOS_Camera_CollisionDetection   0x520190
DWORD RETURN_Camera_CollisionDetection =    0x520195;
void HOOK_Camera_CollisionDetection ();

bool CCameraSAInterface::IsSphereVisible( const CVector& pos, float radius, const RwMatrix& transform )
{
    DWORD dwFunc = 0x00420C40;

    __asm
    {
        push transform
        push radius
        push pos
        mov ecx,this
        call dwFunc
    }
}

float CCameraSAInterface::GetGroundLevel( unsigned int type )
{
    static CVector lastUpdatePosition;
    static float unk1 = 0;
    static float unk2 = 0;
    static float unk3 = 0;

    const CVector& camPos = Placeable.GetPosition();

    if ( abs( lastUpdatePosition[0] - camPos[0] ) > 20 ||
         abs( lastUpdatePosition[1] - camPos[1] ) > 20 ||
         abs( lastUpdatePosition[2] - camPos[2] ) > 20 )
    {
        CColPointSAInterface hitPos;
        CEntitySAInterface *hitEntity;
        CVector startPos( camPos.fX, camPos.fY, 1000.0f );

        bool hit = pGame->GetWorld()->ProcessVerticalLine( startPos, -1000, hitPos, &hitEntity, true, false, false, false, true, false, false );

        if ( hit )
        {
            unk3 = hitPos.Position[2];

            const CVector& entityPos = hitEntity->Placeable.GetPosition();
            
            CColModelSAInterface *col = hitEntity->GetColModel();

            unk2 = col->m_bounds.vecBoundMax[2] + entityPos.fZ;
            unk1 = (float)std::max( (float)0,
                ( col->m_bounds.vecBoundMax[0] - col->m_bounds.vecBoundMin[0] <= 120.0f &&
                 col->m_bounds.vecBoundMax[1] - col->m_bounds.vecBoundMin[1] <= 120.0f )
                 ? ( entityPos[2] + col->m_bounds.vecBoundMax[2] ) : ( unk2 ) );
        }

        lastUpdatePosition = camPos;
    }

    if ( type == 2 )
        return unk2;

    if ( type == 1 )
        return unk3;

    return unk1;
}

/*=========================================================
    CCameraSAInterface::GetMusicFadeType

    Purpose:
        Returns the fading status as described by the music
        fade status.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00536F80
=========================================================*/
unsigned int __thiscall CCameraSAInterface::GetMusicFadeType( void ) const
{
    if ( m_fTimeToFadeMusic == 0 )
        return 0;

    return ( m_fTimeToFadeMusic == 255.0 ) ? 2 : 1;
}

/*=========================================================
    CCameraSAInterface::SetFadeColor

    Arguments:
        red - red color component
        green - green color component
        blue - blue color component
    Purpose:
        Sets the fading color of the game.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0050BF00
=========================================================*/
void __thiscall CCameraSAInterface::SetFadeColor( unsigned char red, unsigned char green, unsigned char blue )
{
    m_bMusicFadedOut = ( red == 2 && green == 2 && blue == 2 );
    
    *(unsigned char*)0x00C3EFA8 = red;
    *(unsigned char*)0x00C3EFA9 = green;
    *(unsigned char*)0x00C3EFAA = blue;
}

/*=========================================================
    CCameraSAInterface::Fade

    Arguments:
        fadeDuration - time required to fade the screen to black
        direction - fade in or fade out
    Purpose:
        Performs a fading screen animation by the parameters
        given.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0050AC25
=========================================================*/
void __thiscall CCameraSAInterface::Fade( float fadeDuration, unsigned short direction )
{
    m_fTimeToFadeOut = fadeDuration;
    m_bFading = true;
    m_iFadingDirection = direction;

    m_uiFadeTimeStarted = *(unsigned int*)0x00B7CB84;

    if ( m_bJustInitalised && direction != 1 )
        return;

    m_bMusicFading = true;
    m_iMusicFadingDirection = direction;

    float musicFadeDuration = (float)Clamp( 0.3f, std::max( fadeDuration, 1.0f ) * 0.3f, fadeDuration );

    m_fTimeToFadeMusic = musicFadeDuration;

    if ( direction == 0 )
    {
        m_fTimeToWaitToFadeMusic = fadeDuration - musicFadeDuration;
        m_fTimeToFadeMusic = std::max( musicFadeDuration - 0.1f, 0.0f );
    }
    else
    {
        m_fTimeToWaitToFadeMusic = 0;
    }

    m_uiFadeTimeStartedMusic = *(unsigned int*)0x00B7CB84;
}

/*=========================================================
    CCameraSAInterface::GetFadeDirection

    Purpose:
        Returns the logical fade direction of the camera at
        this time.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0050ADF0
=========================================================*/
int __thiscall CCameraSAInterface::GetFadeDirection( void ) const
{
    return ( m_bFading ) ? m_iFadingDirection == 1 : 2;
}

/*=========================================================
    CCameraSAInterface::GetActiveCamLookDirection

    Purpose:
        Returns the direction the active cam is looking to.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0050AE90
=========================================================*/
int __thiscall CCameraSAInterface::GetActiveCamLookDirection( void )
{
    CCamSAInterface& cam = GetActiveCam();

    if ( cam.Mode == 18 || cam.Mode == 16 || cam.Mode == 22 || cam.Mode == 4 )
        return cam.DirectionWasLooking;

    return 3;
}

void Camera_Init( void )
{
    HookInstall( 0x00514B80, h_memFunc( &CCameraSAInterface::GetGroundLevel ), 5 );
}

void Camera_Shutdown( void )
{
}

CCameraSA::CCameraSA( CCameraSAInterface *cam )
{ 
    DEBUG_TRACE("CCameraSA::CCameraSA(CCameraSAInterface * cameraInterface)");

    m_interface = cam;

    for( unsigned int i = 0; i<MAX_CAMS; i++ )
        m_cams[i] = new CCamSA( &m_interface->Cams[i] );

    bCameraClipObjects = true;
    bCameraClipVehicles = true;

    HookInstall( HOOKPOS_Camera_CollisionDetection, (DWORD)HOOK_Camera_CollisionDetection, 5 );
}

CCameraSA::~CCameraSA( void )
{
    for ( unsigned int i = 0; i < MAX_CAMS; i++ )
        delete m_cams[i];
}

void CCameraSA::Restore( void )
{
    DEBUG_TRACE("void CCameraSA::Restore( void )");

    DWORD dwFunc = FUNC_Restore;
    CCameraSAInterface * cameraInterface = this->GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
}

void CCameraSA::RestoreWithJumpCut( void )
{
    DEBUG_TRACE("void CCameraSA::RestoreWithJumpCut( void )");

    CCameraSAInterface * cameraInterface = this->GetInterface();
    DWORD dwFunc = 0x50BD40;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
    dwFunc = 0x50BAB0;
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
    }
}

/**
 * \todo Find out what the last two paramters are
 */
void CCameraSA::TakeControl( CEntity *entity, eCamMode CamMode, int switchStyle )
{
    DEBUG_TRACE("VOID CCameraSA::TakeControl(CEntity * entity, eCamMode CamMode, int CamSwitchStyle)");

    CEntitySA *pEntitySA = dynamic_cast <CEntitySA*> ( entity );

    if ( !pEntitySA )
        return;

    CEntitySAInterface *itent = pEntitySA->GetInterface();
    CCameraSAInterface *cmint = GetInterface();
    // __thiscall

    DWORD CCamera__TakeControl = FUNC_TakeControl;
    _asm 
    {
        mov ecx, cmint
        push 1
        push switchStyle
        push CamMode
        push itent
        call CCamera__TakeControl
    }
}

void CCameraSA::TakeControl( const CVector& pos, int switchStyle )
{
    DEBUG_TRACE("void CCameraSA::TakeControl( const CVector& pos, int switchStyle )");

    CCameraSAInterface *cmint = GetInterface();
    // __thiscall
    CVector vecOffset;
/*  vecOffset.fZ = 0.5f;
    vecOffset.fY = 0.5f;
    vecOffset.fX = 0.5f;*/
/*  DWORD dwFunc = 0x50BEC0;
    _asm
    {
        mov ecx, cameraInterface
        lea     eax, vecOffset
        push    eax
        push    position
        call    dwFunc
    }*/

    DWORD CCamera__TakeControlNoEntity = FUNC_TakeControlNoEntity;
    _asm 
    {
        mov ecx, cmint
        push 1
        push switchStyle
        push pos
        call CCamera__TakeControlNoEntity
    }

    DWORD dwFunc = 0x50BEC0;
    _asm
    {
        mov ecx, cmint
        lea     eax, vecOffset
        push    eax
        push    pos
        call    dwFunc
    }

}

// vecOffset: an offset from 0,0,0
// vecLookAt: an offset from 0,0,0

void CCameraSA::TakeControlAttachToEntity( CEntity *target, CEntity *attach, const CVector& off, const CVector& lookAt, float tilt, int switchStyle )
{
    DEBUG_TRACE("void CCameraSA::TakeControlAttachToEntity( CEntity *target, CEntity *attach, const CVector& off, const CVector& lookAt, float tilt, int switchStyle )");

    char szDebug[255] = {'\0'};
    CEntitySAInterface *teint = 0;
    CEntitySAInterface *aeint = 0;

    if( target )
    {
        CEntitySA* pTargetEntitySA = dynamic_cast <CEntitySA*> ( target );
        if ( pTargetEntitySA )
            teint = pTargetEntitySA->GetInterface();
    }

    CEntitySA* pAttachEntitySA = dynamic_cast <CEntitySA*> ( attach );

    if ( !pAttachEntitySA )
        return;

    aeint = pAttachEntitySA->GetInterface();

    CCameraSAInterface *cmint = GetInterface();
    // __thiscall

    //  void    TakeControlAttachToEntity(CEntity* NewTarget, CEntity* AttachEntity, 
    //  CVector& vecOffset, CVector& vecLookAt, float fTilt, short CamSwitchStyle, 
    //  int WhoIsTakingControl=SCRIPT_CAM_CONTROL);
    DWORD CCamera__TakeControlAttachToEntity = FUNC_TakeControlAttachToEntity;

    _asm 
    {
        mov ecx,cmint
        push 1
        push switchStyle
        push tilt
        push lookAt
        push off
        push aeint
        push teint
        call CCamera__TakeControlAttachToEntity
    }
}

const RwMatrix& CCameraSA::GetMatrix( void ) const
{
    return m_interface->m_cameraMatrix; // ->Placeable.matrix;
}

void CCameraSA::SetMatrix( const RwMatrix& mat )
{
    m_interface->m_cameraMatrixOld = mat;
}

void CCameraSA::SetCamPositionForFixedMode( const CVector& pos, const CVector& upOffset )
{
    DWORD dwFunc = FUNC_SetCamPositionForFixedMode;
    CCameraSAInterface * cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    upOffset
        push    pos
        call    dwFunc
    }
}

void CCameraSA::Find3rdPersonCamTargetVector( float fDistance, const CVector& gunMuzzle, const CVector& vecSource, CVector& vecTarget ) const
{
    DEBUG_TRACE("void CCameraSA::Find3rdPersonCamTargetVector( float fDistance, const CVector& gunMuzzle, const CVector& vecSource, CVector& vecTarget ) const");
    float x = gunMuzzle.fX;
    float y = gunMuzzle.fY;
    float z = gunMuzzle.fZ;
    DWORD dwFunc = FUNC_Find3rdPersonCamTargetVector;
    const CCameraSAInterface * cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    vecTarget
        push    vecSource
        push    z
        push    y
        push    x
        push    fDistance
        call    dwFunc
    }
}

float CCameraSA::Find3rdPersonQuickAimPitch( void ) const
{
    DEBUG_TRACE("float CCameraSA::Find3rdPersonQuickAimPitch( void ) const");

    float fReturn;
    DWORD dwFunc = FUNC_Find3rdPersonQuickAimPitch;
    const CCameraSAInterface *cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        call    dwFunc
        fstp    fReturn
    }
    return fReturn;
}

unsigned char CCameraSA::GetActiveCam( void ) const
{
    DEBUG_TRACE("unsigned char CCameraSA::GetActiveCam( void ) const");

    return m_interface->ActiveCam;
}

CCamSA* CCameraSA::GetCam( unsigned char id )
{
    DEBUG_TRACE("CCamSA* CCameraSA::GetCam( unsigned char id )");

    if ( id > MAX_CAMS-1 )
        return NULL;
    
    return m_cams[id];
}

CCamSA* CCameraSA::GetCam( CCamSAInterface *cam )
{
    for ( unsigned int i = 0; i < MAX_CAMS; i++ )
    {
        if ( m_cams[i] && m_cams[i]->GetInterface() == cam )
        {
            return m_cams[i];
        }
    }

    return NULL;
}

/**
 * \todo Rewrite these functions to use m_nCarZoom presumeably
 */
float CCameraSA::GetCarZoom( void ) const
{
    DEBUG_TRACE("float CCameraSA::GetCarZoom( void ) const");
/*  CCameraSAInterface * cameraInterface = this->GetInterface();
    char szDebug[255] = {'\0'};
    sprintf(szDebug, "%d", (DWORD)&cameraInterface->CarZoomIndicator - (DWORD)cameraInterface);
    OutputDebugString(szDebug);
    return cameraInterface->CarZoomIndicator;   */
    return NULL;
}
/**
 * \todo Check if this ever works?
 */
void CCameraSA::SetCarZoom( float val )
{
    DEBUG_TRACE("void CCameraSA::SetCarZoom( float val )");
    /*CCameraSAInterface * cameraInterface = this->GetInterface();
    cameraInterface->CarZoomIndicator = fCarZoom;   */
}

/**
 * \todo does this need an enum?
 * This eventually calls TakeControl if its a valid cam mode at that time
 */
bool CCameraSA::TryToStartNewCamMode( unsigned char mode )
{
    DEBUG_TRACE("bool CCameraSA::TryToStartNewCamMode( unsigned char mode )");
    DWORD dwFunc = FUNC_TryToStartNewCamMode;
    bool bReturn;
    CCameraSAInterface *cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    mode
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CCameraSA::ConeCastCollisionResolve( const CVector& pos, const CVector& lookAt, const CVector& dest, float rad, float minDist, float& dist )
{
    DWORD dwFunc = FUNC_ConeCastCollisionResolve;
    bool bReturn;
    CCameraSAInterface *cameraInterface = GetInterface();
    _asm
    {
        mov     ecx, cameraInterface
        push    dist
        push    minDist
        push    rad
        push    dest
        push    lookAt
        push    pos
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CCameraSA::VectorTrackLinear( const CVector& to, const CVector& from, float time, bool smoothEnds )
{
    DWORD dwFunc = FUNC_VectorTrackLinear;
    CCameraSAInterface *cameraInterface = GetInterface();
    _asm
    {
        mov     ecx,cameraInterface
        push    smoothEnds
        push    time
        push    from
        push    to
        call    dwFunc
    }
}

bool CCameraSA::IsFading( void ) const
{
    return Camera::GetInterface().m_bFading;
}

int CCameraSA::GetFadingDirection( void ) const
{
    return Camera::GetInterface().GetFadeDirection();
}

void CCameraSA::Fade( float fFadeOutTime, int iOutOrIn )
{
    Camera::GetInterface().Fade( fFadeOutTime, iOutOrIn );
}

void CCameraSA::SetFadeColor( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
{
    Camera::GetInterface().SetFadeColor( ucRed, ucGreen, ucBlue );
}

float CCameraSA::GetCameraRotation( void ) const
{
    return *(float*)VAR_CameraRotation;
}

const RwMatrix& CCameraSA::GetLTM( void ) const
{
    return m_interface->m_pRwCamera->parent->GetLTM();
}

CEntity* CCameraSA::GetTargetEntity() const
{
    CEntitySAInterface* pInterface = GetInterface()->pTargetEntity;

    switch( pInterface->nType )
    {
    case ENTITY_TYPE_PED:
        return pGame->GetPools()->GetPed( pInterface );
    case ENTITY_TYPE_VEHICLE:
        return pGame->GetPools()->GetVehicle( pInterface );
    case ENTITY_TYPE_OBJECT:
        return pGame->GetPools()->GetObject( pInterface );
    }
    return NULL;
}

void CCameraSA::SetCameraClip( bool bObjects, bool bVehicles )
{
    bCameraClipObjects = bObjects;
    bCameraClipVehicles = bVehicles;
}

void _cdecl DoCameraCollisionDetectionPokes()
{
    if ( !bCameraClipObjects )
    {
        *(bool*)VAR_CameraClipDynamicObjects = false;
        *(bool*)VAR_CameraClipStaticObjects = false;
    }
    else
        *(bool*)VAR_CameraClipStaticObjects = true;

    if ( !bCameraClipVehicles )
        *(bool*)VAR_CameraClipVehicles = false;
}

void _declspec(naked) HOOK_Camera_CollisionDetection()
{
    _asm
    {
        pushad
        call DoCameraCollisionDetectionPokes
        popad
        sub         esp,24h
        push        ebx 
        push        ebp
        jmp         RETURN_Camera_CollisionDetection
    }
}

unsigned char CCameraSA::GetCameraViewMode() const
{
    // TODO: Add support for ped camera view, this will only work on vehicles for now.
    return *(unsigned char*)VAR_VehicleCameraView;
}

void CCameraSA::SetCameraViewMode( unsigned char mode )
{
    *(unsigned char*)VAR_VehicleCameraView = mode;
}

bool CCameraSA::IsSphereVisible( const RwSphere& sphere ) const
{
    return m_interface->IsSphereVisible( sphere.pos, sphere.radius, Camera::GetInterface().m_matInverse );
}
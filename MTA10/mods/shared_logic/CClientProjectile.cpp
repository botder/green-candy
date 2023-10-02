/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientProjectile.cpp
*  PURPOSE:     Projectile entity class
*  DEVELOPERS:  Jax <>
*               Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define AIRBOMB_MAX_LIFETIME 60000

static const luaL_Reg projectile_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_projectile( lua_State *L )
{
    CClientProjectile *proj = (CClientProjectile*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_PROJECTILE, proj );

    j.RegisterInterfaceTrans( L, projectile_interface, 0, LUACLASS_PROJECTILE );

    lua_pushlstring( L, "projectile", 10 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

/* An instance of this class is created when GTA creates a projectile, it automatically
   destroys itself when GTA is finished with the projectile, this could/should eventually be
   used as a server created element and streamed.
*/
CClientProjectile::CClientProjectile( class CClientManager *pManager, CProjectile *pProjectile, CProjectileInfo *pProjectileInfo, CClientEntity *pCreator, CClientEntity *pTarget, eWeaponType weaponType, CVector *pvecOrigin, CVector *pvecTarget, float fForce, bool bLocal, lua_State *L ) : CClientEntity( INVALID_ELEMENT_ID, false, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_projectile, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    CClientEntityRefManager::AddEntityRefs ( ENTITY_REF_DEBUG ( this, "CClientProjectile" ), &m_pCreator, &m_pTarget, NULL );

    m_pManager = pManager;
    m_pProjectileManager = pManager->GetProjectileManager ();
    m_pProjectile = pProjectile;
    m_pProjectileInfo = pProjectileInfo;   

    SetTypeName ( "projectile" );

    m_pCreator = pCreator;
    m_pTarget = pTarget;
    m_weaponType = weaponType;

    if ( pvecOrigin ) m_pvecOrigin = new CVector( *pvecOrigin );
    else m_pvecOrigin = NULL;
    if ( pvecTarget ) m_pvecTarget = new CVector( *pvecTarget );
    else m_pvecTarget = NULL;

    m_fForce = fForce;
    m_bLocal = bLocal;
    m_llCreationTime = GetTickCount64_();

    m_pInitiateData = NULL;
    m_bInitiate = true;

    m_pProjectileManager->AddToList( this );
    m_bLinked = true;

    if ( pCreator )
    {
        switch( pCreator->GetType() )
        {
        case CCLIENTPLAYER:
        case CCLIENTPED:
            static_cast <CClientPed*> ( pCreator )->AddProjectile( this );
            break;
        case CCLIENTVEHICLE:
            static_cast <CClientVehicle*> ( pCreator )->AddProjectile( this );
            break;
        }
    }
}

CClientProjectile::~CClientProjectile()
{   
    // Make sure we're destroyed
    Destroy();

    // If our creator is getting destroyed, this should be null
    if ( m_pCreator )
    {
        switch( m_pCreator->GetType() )
        {
            case CCLIENTPLAYER:
            case CCLIENTPED:
                static_cast <CClientPed*> ( m_pCreator )->RemoveProjectile( this );
                break;
            case CCLIENTVEHICLE:
                static_cast <CClientVehicle*> ( m_pCreator )->RemoveProjectile( this );
                break;
            default: break;
        }
    }

    delete m_pvecOrigin;
    delete m_pvecTarget;

    delete m_pInitiateData;

    Unlink();

    CClientEntityRefManager::RemoveEntityRefs( 0, &m_pCreator, &m_pTarget, NULL );
}

void CClientProjectile::Unlink()
{
    // Are we still linked? (this bool will be set to false when our manager is being destroyed)
    if ( m_bLinked )
    {
        m_pProjectileManager->RemoveFromList( this ); 
        m_bLinked = false;
    }
}

void CClientProjectile::DoPulse()
{
    // We use initiate data to set values on creation (as it doesn't exist until a frame after our projectile hook)
    if ( m_bInitiate )
    {
        if ( m_pInitiateData )
        {
            if ( m_pInitiateData->pvecPosition ) SetPosition( *m_pInitiateData->pvecPosition );
            if ( m_pInitiateData->pvecRotation ) SetRotation( *m_pInitiateData->pvecRotation );
            if ( m_pInitiateData->pvecVelocity ) SetVelocity( *m_pInitiateData->pvecVelocity );
            if ( m_pInitiateData->usModel ) SetModel( m_pInitiateData->usModel );
        }

        // Let our manager know we've been initiated
        m_pProjectileManager->OnInitiate( this );             
        m_bInitiate = false;
    }

    // Update our position/rotation if we're attached
    DoAttaching ();
}

void CClientProjectile::Initiate( CVector *pvecPosition, CVector *pvecRotation, CVector *pvecVelocity, unsigned short usModel )
{
#ifdef MTA_DEBUG
    if ( m_pInitiateData ) _asm int 3
#endif

    // Store our initiation data
    m_pInitiateData = new CProjectileInitiateData;
    if ( pvecPosition ) m_pInitiateData->pvecPosition = new CVector( *pvecPosition );
    else m_pInitiateData->pvecPosition = NULL;
    if ( pvecRotation ) m_pInitiateData->pvecRotation = new CVector( *pvecRotation );
    else m_pInitiateData->pvecRotation = NULL;
    if ( pvecVelocity ) m_pInitiateData->pvecVelocity = new CVector( *pvecVelocity );
    else m_pInitiateData->pvecVelocity = NULL;
    m_pInitiateData->usModel = usModel;
}

void CClientProjectile::Destroy()
{
    delete m_pProjectile;
}

bool CClientProjectile::IsActive() const
{
    // Ensure airbomb is cleaned up
    if ( m_weaponType == WEAPONTYPE_FREEFALL_BOMB && GetTickCount64_() - m_llCreationTime > AIRBOMB_MAX_LIFETIME )
        return false;

    return ( m_pProjectile && m_pProjectileInfo->IsActive() );
}

static __forceinline void PrepareMatrixForGame( RwMatrix& matrix )
{
    matrix.vFront.fX = -matrix.vFront.fX;
    matrix.vFront.fY = -matrix.vFront.fY;
    matrix.vUp.fX = -matrix.vUp.fX;
    matrix.vUp.fY = -matrix.vUp.fY;
}

bool CClientProjectile::GetMatrix( RwMatrix& matrix ) const
{
    if ( m_pProjectile )
    {
        m_pProjectile->GetMatrix( matrix );

        // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertMatrixToEulerAngles
        if ( m_pCreator && IS_PED( m_pCreator ) )
            PrepareMatrixForGame( matrix );

        return true;
    }
    return false;
}

bool CClientProjectile::SetMatrix( const RwMatrix& matrix_ )
{
    // Jax: If the creator is a ped, we need to invert X and Y on Direction and Was for CMultiplayer::ConvertEulerAnglesToMatrix
    if ( m_pCreator && IS_PED( m_pCreator ) )
    {
        RwMatrix matrix( matrix_ );
        PrepareMatrixForGame( matrix );

        m_pProjectile->SetMatrix( matrix );
        return true;
    }

    m_pProjectile->SetMatrix( matrix_ );
    return true;
}

void CClientProjectile::GetPosition( CVector & vecPosition ) const
{
    m_pProjectile->GetPosition( vecPosition );
}

void CClientProjectile::SetPosition( const CVector & vecPosition )
{
    m_pProjectile->SetPosition( vecPosition );
}

void CClientProjectile::GetRotation( CVector& rot ) const
{
    RwMatrix matrix;
    GetMatrix( matrix );
    matrix.GetRotationRad( rot.fX, rot.fY, rot.fZ );
}

void CClientProjectile::GetRotationDegrees( CVector& rot ) const
{
    GetRotation( rot );
    ConvertRadiansToDegrees( rot );
}

void CClientProjectile::SetRotation( const CVector& rot )
{
    RwMatrix matrix;
    GetPosition( matrix.vPos );
    matrix.SetRotationRad( rot.fX, rot.fY, rot.fZ );
    SetMatrix( matrix );
}

void CClientProjectile::SetRotationDegrees( const CVector& rot )
{
    CVector radRot = rot;
    ConvertDegreesToRadians( radRot );
    SetRotation( radRot );
}

void CClientProjectile::GetVelocity( CVector& vel ) const
{
    m_pProjectile->GetMoveSpeed( vel );
}

void CClientProjectile::SetVelocity( CVector& vecVelocity )
{
    m_pProjectile->SetMoveSpeed( vecVelocity );
}

void CClientProjectile::SetModel( unsigned short usModel )
{
    m_pProjectile->SetModelIndex( usModel );
}
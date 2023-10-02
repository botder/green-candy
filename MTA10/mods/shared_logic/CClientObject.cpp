/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientObject.cpp
*  PURPOSE:     Physical object entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#define MTA_BUILDINGS
#define CCLIENTOBJECT_MAX 250

CClientObject::CClientObject( CClientManager* pManager, ElementID ID, lua_State *L, bool system, unsigned short usModel ) : CClientGameEntity( pManager->GetObjectStreamer(), ID, L, system )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_object, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Init
    m_pManager = pManager;
    m_pObjectManager = pManager->GetObjectManager ();
    m_pModelRequester = pManager->GetModelRequestManager ();

    m_pObject = NULL;
    m_usModel = usModel;

    SetTypeName ( "object" );

    m_usModel = usModel;
    m_bIsVisible = true;
    m_bIsStatic = false;
    m_bUsesCollision = true;
    m_ucAlpha = 255;
    m_fScale = 1.0f;
    m_fHealth = 1000.0f;
    m_bBreakable = true;

    m_pModelInfo = g_pGame->GetModelInfo ( usModel );

    // Add this object to the list
    m_pObjectManager->AddToList ( this );
}

CClientObject::~CClientObject()
{
    // Unrequest whatever we've requested or we'll crash in unlucky situations
    m_pModelRequester->Cancel( this, false );  

    // Detach us from anything
    AttachTo( NULL );

    // Destroy the object
    Destroy();

    // Remove us from the list
    Unlink();
}

void CClientObject::Unlink()
{
    m_pObjectManager->RemoveFromList( this );
    m_pObjectManager->m_Attached.remove( this );
    ListRemove( m_pObjectManager->m_StreamedIn, this );
}

void CClientObject::GetPosition( CVector& vecPosition ) const
{
    if ( m_pObject )
    {
        m_pObject->GetPosition( vecPosition );
    }
    else if ( m_pAttachedToEntity )
    {
        m_pAttachedToEntity->GetPosition ( vecPosition );
        vecPosition += m_vecAttachedPosition;
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}

void CClientObject::SetPosition( const CVector& pos )
{   
    // Move the object
    if ( m_pObject )
    {
        CVector vecRot;
        GetRotationRadians( vecRot );
        m_pObject->Teleport( pos.fX, pos.fY, pos.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
        SetRotationRadians( vecRot );
    }

    if ( m_vecPosition != pos )
    {
        // Store the position in our datastruct
        m_vecPosition = pos;

        // Update our streaming position
        UpdateStreamPosition( pos );
    }
}

void CClientObject::GetRotationDegrees( CVector& vecRotation ) const
{
    GetRotationRadians( vecRotation );
    ConvertRadiansToDegrees( vecRotation );
}

void CClientObject::GetRotationRadians( CVector& vecRotation ) const
{
    if ( m_pObject && m_pAttachedToEntity )// Temp fix for static objects->
    {
        // We've been returning the rotation that got set last so far (::m_vecRotation)..
        //   but we need to get the real rotation for when the game moves the objects..
        //  (eg: physics/attaching), the code below returns wrong values, see #2732
        RwMatrix matTemp;
        m_pObject->GetMatrix( matTemp );
        matTemp.GetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
    }
    else
    {
        vecRotation = m_vecRotation;
    }
}

void CClientObject::SetRotationDegrees( const CVector & vecRotation )
{
    // Convert from degrees to radians
    CVector vecTemp;
    vecTemp.fX = vecRotation.fX * (float)M_PI / 180.0f;
    vecTemp.fY = vecRotation.fY * (float)M_PI / 180.0f;
    vecTemp.fZ = vecRotation.fZ * (float)M_PI / 180.0f;

    SetRotationRadians( vecTemp );
}

void CClientObject::SetRotationRadians( const CVector& rot )
{
    if ( m_pObject )
    {
        m_pObject->SetOrientation( rot.fX, rot.fY, rot.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
    }

    // Store it in our datastruct
    m_vecRotation = rot;
}

void CClientObject::GetOrientation( CVector& pos, CVector& rotRadians ) const
{
    GetPosition( pos );
    GetRotationRadians( rotRadians );
}

void CClientObject::SetOrientation( const CVector& pos, const CVector& rotRadians )
{
    if ( m_vecPosition != pos )
    {
        // Store the position in our datastruct
        m_vecPosition = pos;

        // Update our streaming position
        UpdateStreamPosition( pos );
    }

    // Update our internal data
    m_vecPosition = pos;
    m_vecRotation = rotRadians;

    // Eventually move the object
    if ( m_pObject )
    {
        m_pObject->Teleport( pos.fX, pos.fY, pos.fZ );
        m_pObject->SetOrientation( rotRadians.fX, rotRadians.fY, rotRadians.fZ );
#ifndef MTA_BUILDINGS
        m_pObject->ProcessCollision();
#endif
        m_pObject->SetupLighting();
    }
}

void CClientObject::ModelRequestCallback ( CModelInfo* pModelInfo )
{
    // Create our object
    Create ();
}

float CClientObject::GetDistanceFromCentreOfMassToBaseOfModel() const
{
    if ( m_pObject )
        return m_pObject->GetBasingDistance();

    return 0;
}

void CClientObject::SetVisible( bool bVisible )
{
    if ( m_pObject )
    {
        m_pObject->SetVisible ( bVisible );
    }

    m_bIsVisible = bVisible;
}

void CClientObject::SetModel( unsigned short usModel )
{
    // Valid model ID?
    if ( !CClientObjectManager::IsValidModel( usModel ) )
        return;

    // Destroy current model
    Destroy();

    // Set the new model ID and recreate the model
    m_usModel = usModel;
    m_pModelInfo = g_pGame->GetModelInfo( usModel );
    UpdateSpatialData();

    // Request the new model so we can recreate when it's done
    if ( m_pModelRequester->Request( usModel, this ) )
    {
        Create();
    }
}

void CClientObject::Render()
{
    if ( m_pObject )
        m_pObject->Render();
}

void CClientObject::SetStatic( bool bStatic )
{
    m_bIsStatic = bStatic;
    StreamOutForABit();
}

void CClientObject::SetAlpha( unsigned char ucAlpha )
{
    if ( m_pObject )
        m_pObject->SetAlpha( ucAlpha );

    m_ucAlpha = ucAlpha;
}

void CClientObject::SetScale( float fScale )
{
    if ( m_pObject )
        m_pObject->SetScaleValue( fScale );

    m_fScale = fScale;
}

void CClientObject::SetCollisionEnabled( bool bCollisionEnabled )
{
    if ( m_pObject )
        m_pObject->SetUsesCollision( bCollisionEnabled );

    m_bUsesCollision = bCollisionEnabled;
}

float CClientObject::GetHealth() const
{
    if ( m_pObject )
        return m_pObject->GetHealth();

    return m_fHealth;
}

void CClientObject::SetHealth( float fHealth )
{
    if ( m_pObject )
        m_pObject->SetHealth( fHealth );

    m_fHealth = fHealth;
}

void CClientObject::StreamIn( bool bInstantly )
{
    // We need to load now?
    if ( bInstantly )
    {
        // Request the model blocking
        if ( m_pModelRequester->RequestBlocking( m_usModel ) )
        {
            // Create us
            Create();
        }
        else
            NotifyUnableToCreate();
    }
    else
    {
        // Request the model async
        if ( m_pModelRequester->Request( m_usModel, this ) )
        {
            // Create us now if we already had it loaded
            Create();
        }
        else
            NotifyUnableToCreate();
    }
}

void CClientObject::StreamOut()
{
    // Save the health
    if ( m_pObject )
        m_fHealth = m_pObject->GetHealth();

    // Destroy the object.
    Destroy();

    // Cancel anything we might've requested.
    m_pModelRequester->Cancel( this, true );
}

void CClientObject::ReCreate()
{
    if ( m_pObject )
    {
        Destroy();
        Create();
    }
}

void CClientObject::Create()
{
    // Not already created an object?
    if ( !m_pObject )
    {
        // Check again that the limit isn't reached. We are required to do so because
        // we load async. The streamer isn't always aware of our limits.
        if ( !CClientObjectManager::IsObjectLimitReached() )
        {
            // Add a reference to the object
            m_pModelInfo->AddRef( true );

            // If the new object is not breakable, allow it into the vertical line test
            g_pMultiplayer->AllowCreatedObjectsInVerticalLineTest( !CClientObjectManager::IsBreakableModel( m_usModel ) );

            // Create the object
            m_pObject = g_pGame->GetPools()->AddObject( m_usModel );

            // Restore default behaviour
            g_pMultiplayer->AllowCreatedObjectsInVerticalLineTest( false );

            if ( !m_pObject )
            {
                // Remove our reference to the object again
                m_pModelInfo->RemoveRef();
                goto failed;
            }
            // Put our pointer in its stored pointer
            m_pObject->SetStoredPointer( this );

            // If set to true,this has the effect of forcing the object to be static at all times
            m_pObject->SetStaticWaitingForCollision( m_bIsStatic );

            // Apply our data to the object
            m_pObject->Teleport( m_vecPosition.fX, m_vecPosition.fY, m_vecPosition.fZ );
            m_pObject->SetOrientation( m_vecRotation.fX, m_vecRotation.fY, m_vecRotation.fZ );
            m_pObject->SetupLighting();

            if ( !m_bIsVisible )
                SetVisible( false );
            if ( !m_bUsesCollision )
                SetCollisionEnabled( false );
            if ( m_fScale != 1.0f )
                SetScale( m_fScale );

            m_pObject->SetAreaCode( m_ucInterior );
            SetAlpha( m_ucAlpha );
            m_pObject->SetHealth( m_fHealth );

            // Reattach to an entity + any entities attached to this
            ReattachEntities();

            // Validate this entity again
            m_pManager->RestoreEntity( this );

            // Tell the streamer we've created this object
            NotifyCreate();
            return;
        }

failed:
        // Tell the streamer we could not create it
        NotifyUnableToCreate();
    }
}

void CClientObject::Destroy()
{
    // If the object exists
    if ( m_pObject )
    {
        // Invalidate
        m_pManager->InvalidateEntity( this );

        // Destroy the object
        delete m_pObject;
        m_pObject = NULL;

        // Remove our reference to its model
        m_pModelInfo->RemoveRef();

        NotifyDestroy();
    }
}

void CClientObject::NotifyCreate()
{
    m_pObjectManager->OnCreation( this );
    CClientGameEntity::NotifyCreate();
}

void CClientObject::NotifyDestroy()
{
    m_pObjectManager->OnDestruction( this );
}

void CClientObject::StreamedInPulse()
{
    // Are we not a static object (allowed to move by physics)
    if ( !m_bIsStatic )
    {
        // Grab our actual position (as GTA moves it too)
        CVector pos;
        m_pObject->GetPosition( pos );

        // Has it moved without MTA knowing?
        if ( pos != m_vecPosition )
        {
            m_vecPosition = pos;

            // Update our streaming position
            UpdateStreamPosition( pos );
        }
    }
}

void CClientObject::AttachTo( CClientEntity* pEntity )
{
    // Add/remove us to/from our managers attached list
    if ( m_pAttachedToEntity && !pEntity )
        m_pObjectManager->m_Attached.remove( this );
    else if ( !m_pAttachedToEntity && pEntity )
        m_pObjectManager->m_Attached.push_back( this );

    CClientEntity::AttachTo( pEntity );
}

void CClientObject::GetMoveSpeed( CVector& vecMoveSpeed ) const
{
    if ( m_pObject )
    {
        m_pObject->GetMoveSpeed( vecMoveSpeed );
        return;
    }

    vecMoveSpeed = m_vecMoveSpeed;
}

void CClientObject::SetMoveSpeed( const CVector& speed )
{
    if ( m_pObject )
        m_pObject->SetMoveSpeed( speed );

    m_vecMoveSpeed = speed;
}

CSphere CClientObject::GetWorldBoundingSphere() const
{
    CSphere sphere;
    CModelInfo *pModelInfo = g_pGame->GetModelInfo( GetModel() );

    if ( const CBoundingBox *bounds = pModelInfo->GetBoundingBox() )
    {
        sphere.vecPosition = bounds->vecBoundOffset;
        sphere.fRadius = bounds->fRadius;
    }

    sphere.vecPosition += GetStreamPosition();
    return sphere;
}
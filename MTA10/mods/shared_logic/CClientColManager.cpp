/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColManager.cpp
*  PURPOSE:     Collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

//
// This is now a mess due to backward compatibilty.
// ** Review before v1.1 release **
//

CClientColManager::~CClientColManager()
{
}

void CClientColManager::DoPulse()
{
    colshapes_t::const_iterator iter = m_List.begin();

    for ( ; iter != m_List.end(); ++iter )
        (*iter)->DoPulse();
}

void CClientColManager::DoHitDetection( const CVector& pos, float radius, CClientEntity *entity, CClientColShape *pJustThis, bool bChildren )
{
    if ( pJustThis )
        DoHitDetectionForColShape( pJustThis );
    else
        DoHitDetectionForEntity( pos, 0.0f, entity );
}

//
// Handle the changing state of collision between one colshape and any entity
//
void CClientColManager::DoHitDetectionForColShape ( CClientColShape* pShape )
{
    // Ensure colshape is enabled
    if ( !pShape->IsEnabled() )
        return;

    std::map < CClientEntity*, int > entityList;

    // Get all entities within the sphere
    CSphere querySphere = pShape->GetWorldBoundingSphere ();
    CClientEntityResult result;
    GetClientSpatialDatabase()->SphereQuery ( result, querySphere );
 
    // Extract relevant types
    for ( CClientEntityResult::const_iterator it = result.begin(); it != result.end(); ++it )
    {
        CClientEntity *pEntity = *it;

        switch( pEntity->GetType() )
        {
        case CCLIENTRADARMARKER:
        case CCLIENTRADARAREA:
        case CCLIENTTEAM:
        case CCLIENTGUI:
        case CCLIENTCOLSHAPE:
        case CCLIENTDUMMY:
        case SCRIPTFILE:
        case CCLIENTDFF:
        case CCLIENTCOL:
        case CCLIENTTXD:
        case CCLIENTSOUND:
            break;
        default:
            if ( pEntity->GetParent() )
                entityList[ pEntity ] = 1;

            break;
        }
    }

    // Add existing colliders, so they can be disconnected if required
    for ( list < CClientEntity* > ::const_iterator it = pShape->CollidersBegin () ; it != pShape->CollidersEnd (); ++it )
    {
        entityList[ *it ] = 1;
    }

    // We wanna reference our collider
    pShape->IncrementMethodStack();

    // Test each entity against the colshape
    for ( std::map < CClientEntity*, int > ::const_iterator it = entityList.begin () ; it != entityList.end (); ++it )
    {
        CClientEntity *pEntity = it->first;
        CVector vecPosition;
        pEntity->GetPosition( vecPosition );

        // Reference as we will extensively use it
        pEntity->IncrementMethodStack();

        HandleHitDetectionResult( pShape->DoHitDetection( vecPosition, 0.0f ), pShape, pEntity );

        // The entity can be destroyed
        pEntity->DecrementMethodStack();
    }

    pShape->DecrementMethodStack();
}


//
// Handle the changing state of collision between one entity and any colshape
//
void CClientColManager::DoHitDetectionForEntity ( const CVector& vecNowPosition, float fRadius, CClientEntity* pEntity )
{
    typedef std::map < CClientColShape*, int > shortType;
    shortType shortList;

    // Get all entities within the sphere
    CClientEntityResult queryResult;
    GetClientSpatialDatabase()->SphereQuery ( queryResult, CSphere ( vecNowPosition, fRadius ) );

    // Extract colshapes
    for ( CClientEntityResult ::const_iterator it = queryResult.begin () ; it != queryResult.end (); ++it )
        if ( (*it)->GetType () == CCLIENTCOLSHAPE )
            shortList[ (CClientColShape*)*it ] = 1;

    // Add existing collisions, so they can be disconnected if required
    for ( list < CClientColShape* > ::const_iterator it = pEntity->CollisionsBegin () ; it != pEntity->CollisionsEnd (); ++it )
        shortList[ *it ] = 1;

    // Keep the entity alive
    pEntity->IncrementMethodStack();

    // Prevent destruction
    for ( shortType::const_iterator iter = shortList.begin(); iter != shortList.end(); iter++ )
        (*iter).first->IncrementMethodStack();

    // Test each colshape against the entity
    for ( shortType::const_iterator it = shortList.begin () ; it != shortList.end (); ++it )
    {
        CClientColShape* pShape = it->first;

        // Enabled?
        if ( !pShape->IsEnabled() )
            continue;

        HandleHitDetectionResult( pShape->DoHitDetection( vecNowPosition, fRadius ), pShape, pEntity );
    }

    // Release method lock
    for ( shortType::const_iterator iter = shortList.begin(); iter != shortList.end(); iter++ )
        (*iter).first->DecrementMethodStack();

    pEntity->DecrementMethodStack();
}


//
// Handle the changing state of collision between one colshape and one entity
//
void CClientColManager::HandleHitDetectionResult ( bool bHit, CClientColShape* pShape, CClientEntity* pEntity )
{
    if ( bHit )
    {              
        // If they havn't collided yet
        if ( !pEntity->CollisionExists ( pShape ) )
        {
#ifdef SPATIAL_DATABASE_TESTS
            testResult.newWay.onList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );
            if ( !testResult.bCountOnly )
#endif
            {
                // Add the collision and the collider
                pShape->AddCollider ( pEntity );
                pEntity->AddCollision ( pShape );

                // Can we call the event?
                if ( pShape->GetAutoCallEvent () )
                {
                    // Call the event
                    lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();
                    pEntity->PushStack( L );
                    lua_pushboolean( L, pShape->GetDimension() == pEntity->GetDimension() );
                    pShape->CallEvent( "onClientColShapeHit", L, 2 );

                    pShape->PushStack( L );
                    lua_pushboolean( L, pShape->GetDimension() == pEntity->GetDimension() );
                    pEntity->CallEvent( "onClientElementColShapeHit", L, 2 );
                }

                // Run whatever callback the collision item might have attached
                pShape->CallHitCallback ( *pEntity );
            }
        }
    }
    else
    {
        // If they collided before
        if ( pEntity->CollisionExists ( pShape ) )
        {

#ifdef SPATIAL_DATABASE_TESTS
            testResult.newWay.offList.push_back ( CTestSet::CTestResult::CEntityPair ( pShape, pEntity ) );
            if ( !testResult.bCountOnly )
#endif
            {
                // Remove the collision and the collider
                pShape->RemoveCollider ( pEntity );
                pEntity->RemoveCollision ( pShape );

                // Call the event
                lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();
                pEntity->PushStack( L );
                lua_pushboolean( L, pShape->GetDimension() == pEntity->GetDimension() );
                pShape->CallEvent( "onClientColShapeLeave", L, 2 );

                pShape->PushStack( L );
                lua_pushboolean( L, pShape->GetDimension() == pEntity->GetDimension() );
                pEntity->CallEvent( "onClientElementColShapeLeave", L, 2 );

                pShape->CallLeaveCallback( *pEntity );
            }
        }
    }
}

bool CClientColManager::Exists( CClientColShape* pShape )
{
    // Return true if it exists
    return ListContains( m_List, pShape );
}

void CClientColManager::RemoveFromList( CClientColShape* pShape )
{
    ListRemove( m_List, pShape );
}

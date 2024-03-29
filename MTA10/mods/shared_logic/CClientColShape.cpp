/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColShape.cpp
*  PURPOSE:     Shaped collision entity base class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

static const luaL_Reg colshape_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_colshape( lua_State *L )
{
    CClientColShape *col = (CClientColShape*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_COLSHAPE, col );

    j.RegisterInterfaceTrans( L, colshape_interface, 0, LUACLASS_COLSHAPE );

    lua_pushlstring( L, "colshape", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientColShape::CClientColShape( CClientManager* pManager, ElementID ID, lua_State *L, bool system ) : CClientEntity( ID, system, L )
{
    // Setup Lua
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_colshape, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    CClientEntityRefManager::AddEntityRefs( ENTITY_REF_DEBUG( this, "CClientColShape" ), &m_pOwningMarker, &m_pOwningPickup, NULL );

    // Init
    m_pManager = pManager;
    m_bIsEnabled = true;
    m_bAutoCallEvent = true;
    m_pCallback = NULL;
    m_pOwningMarker = NULL;
    m_pOwningPickup = NULL;

    SetTypeName( "colshape" );

    // Add it to our manager's list
    m_pColManager = pManager->GetColManager();
    m_pColManager->AddToList( this );
}

CClientColShape::~CClientColShape()
{
    if ( m_pOwningMarker && m_pOwningMarker->m_pCollision == this )
        m_pOwningMarker->m_pCollision = NULL;
    else if ( m_pOwningPickup && m_pOwningPickup->m_pCollision == this )
        m_pOwningPickup->m_pCollision = NULL;

    RemoveAllColliders( true );
    Unlink();
    CClientEntityRefManager::RemoveEntityRefs( 0, &m_pOwningMarker, &m_pOwningPickup, NULL );
}

void CClientColShape::Unlink()
{
    m_pColManager->RemoveFromList( this );
}

void CClientColShape::SizeChanged()
{
    UpdateSpatialData();
    // Maybe queue RefreshColliders for v1.1
}

void CClientColShape::DoPulse()
{
    // Update our position/rotation if we're attached
    DoAttaching();

#ifdef SPATIAL_DATABASE_TESTS
    if ( !GetClientSpatialDatabase ()->IsEntityPresent ( this ) )
    {
        CSphere sphere = GetWorldBoundingSphere ();
        CLogger::ErrorPrintf ( "Spatial problem - ColShape %08x Type %d not in new  sphere: %2.2f,%2.2f,%2.2f   %2.2f"
                                            ,this
                                            ,this->GetShapeType ()
                                            ,sphere.vecPosition.fX
                                            ,sphere.vecPosition.fY
                                            ,sphere.vecPosition.fZ
                                            ,sphere.fRadius
                                            );
    }
#endif
}

bool CClientColShape::IsAttachable() const
{
    return ( !m_pOwningPickup && !m_pOwningMarker );
}

void CClientColShape::SetPosition( const CVector& vecPosition )
{
    m_vecPosition = vecPosition;
    UpdateSpatialData();
    CStaticFunctionDefinitions::RefreshColShapeColliders( this );
};

void CClientColShape::CallHitCallback( CClientEntity& Entity )
{
    // Call the callback with us as the shape if it exists
    if ( m_pCallback )
        m_pCallback->Callback_OnCollision( *this, Entity );
}

void CClientColShape::CallLeaveCallback( CClientEntity& Entity )
{
    // Call the callback with us as the shape if it exists
    if ( m_pCallback )
        m_pCallback->Callback_OnLeave( *this, Entity );
}

bool CClientColShape::ColliderExists( CClientEntity *pEntity ) const
{
    entities_t::const_iterator iter = m_Colliders.begin();

    for ( ; iter != m_Colliders.end(); iter++ )
    {
        if ( *iter == pEntity )
            return true;
    }    
    return false;
}

void CClientColShape::RemoveAllColliders( bool bNotify )
{
    if ( bNotify )
    {
        entities_t::iterator iter = m_Colliders.begin();

        for ( ; iter != m_Colliders.end(); iter++ )
            (*iter)->RemoveCollision( this );
    }
    m_Colliders.clear();
}

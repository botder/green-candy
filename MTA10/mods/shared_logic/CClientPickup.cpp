/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPickup.cpp
*  PURPOSE:     Pickup entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CClientGame* g_pClientGame;

static const luaL_Reg pickup_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_pickup( lua_State *L )
{
    CClientPickup *pickup = (CClientPickup*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_PICKUP, pickup );

    j.RegisterInterfaceTrans( L, pickup_interface, 0, LUACLASS_PICKUP );

    lua_pushlstring( L, "pickup", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientPickup::CClientPickup( CClientManager *pManager, ElementID ID, lua_State *L, bool system, unsigned short usModel, CVector vecPosition ) : CClientStreamElement( pManager->GetPickupStreamer(), ID, L, system )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_pickup, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Initialize
    m_pManager = pManager;
    m_pPickupManager = pManager->GetPickupManager();
    m_usModel = usModel;    
    m_bVisible = true;
    m_pPickup = NULL;
    m_pObject = NULL;
    m_pCollision = NULL;

    // Add us to pickup manager's list
    m_pPickupManager->m_List.push_back( this );

    SetTypeName( "pickup" );
    m_ucType = CClientPickup::WEAPON;
    m_ucWeaponType = CClientPickup::WEAPON_BRASSKNUCKLE;
    m_fAmount = 0.0f;
    m_usAmmo = 0;

    // Make sure our streamer knows where we are to start with
    UpdateStreamPosition ( m_vecPosition = vecPosition );
}

CClientPickup::~CClientPickup()
{
    AttachTo( NULL );

    // Make sure our pickup is destroyed
    Destroy();

    // Remove us from the pickup manager's list
    Unlink();
}

void CClientPickup::Unlink()
{
    m_pPickupManager->RemoveFromList( this );
}

void CClientPickup::SetPosition( const CVector& vecPosition )
{
    // Different from our current position?
    if ( m_vecPosition != vecPosition )
    {
        // Set the new position and recreate it
        m_vecPosition = vecPosition;
        ReCreate ();

        // Update our streaming position
        UpdateStreamPosition( vecPosition );
    }
}

void CClientPickup::SetModel( unsigned short usModel )
{
    // Different from our current id?
    if ( m_usModel != usModel )
    {
        // Set the model and recreate the pickup
        m_usModel = usModel;
        UpdateSpatialData();
        ReCreate();
    }
}

void CClientPickup::SetVisible( bool bVisible )
{
    // Only update visible state if we're streamed in
    if ( IsStreamedIn() )
    {
        if ( bVisible )
            Create();
        else
            Destroy();
    }

    // Update the flag
    m_bVisible = bVisible;
}

void CClientPickup::StreamIn( bool bInstantly )
{
    // Create it
    Create();

    // Notify the streamer we've created it
    NotifyCreate();
}

void CClientPickup::StreamOut()
{
    // Destroy it
    Destroy();
}

void CClientPickup::Create()
{
    if ( !m_pPickup )
    {
        // Create the pickup
        m_pPickup = g_pGame->GetPickups()->CreatePickup( &m_vecPosition, m_usModel, PICKUP_ONCE );

        if ( !m_pPickup )
            return;

        // Grab the attributes from the MTA interface for this pickup
        m_pObject = NULL;
        unsigned char ucAreaCode = GetInterior();
        unsigned short usDimension = GetDimension();

        // Make sure we have an object
        if ( !m_pPickup->GetObject() )
            m_pPickup->GiveUsAPickUpObject();

        // Store our pickup's object
        m_pObject = m_pPickup->GetObject();

        // Create our collision
        m_pCollision = new CClientColSphere( g_pClientGame->GetManager(), NULL, m_lua, IsSystemEntity(), m_vecPosition, 1.0f );
        m_pCollision->m_pOwningPickup = this;
        m_pCollision->SetHitCallback( this );
        m_pCollision->SetRoot( this );

        // Increment pickup counter
        ++m_pPickupManager->m_uiPickupCount;

        // Restore the attributes
        SetInterior( ucAreaCode );
        SetDimension( usDimension );
    }
}

void CClientPickup::Destroy()
{
    if ( m_pCollision )
    {
        m_pCollision->Delete();

        m_pCollision = NULL;
    }

    if ( m_pPickup )
    {
        // Delete the pickup
        m_pPickup->Remove();
        m_pPickup = NULL;
        m_pObject = NULL;

        // Decrement pickup counter
        --m_pPickupManager->m_uiPickupCount;
    }
}

void CClientPickup::ReCreate()
{
    // If we had a pickup, destroy and recreate it
    if ( m_pPickup )
    {
        Destroy();
        Create();
    }
}

void CClientPickup::Callback_OnCollision( CClientColShape& Shape, CClientEntity& Entity )
{
    if ( Entity.IsTransmit( LUACLASS_PLAYER ) )
    {
        // Call the pickup hit event
        Entity.PushStack( m_lua );            // player that hit it
        lua_pushboolean( m_lua, GetDimension() == Entity.GetDimension() ); // matching dimension?
        CallEvent( "onClientPickupHit", m_lua, 2 );
    }
}

void CClientPickup::Callback_OnLeave( CClientColShape& Shape, CClientEntity& Entity )
{
    if ( Entity.IsTransmit( LUACLASS_PLAYER ) )
    {
        // Call the pickup leave event
        Entity.PushStack( m_lua );            // player that hit it
        lua_pushboolean( m_lua, GetDimension() == Entity.GetDimension() ); // matching dimension?
        CallEvent( "onClientPickupLeave", m_lua, 2 );
    }
}
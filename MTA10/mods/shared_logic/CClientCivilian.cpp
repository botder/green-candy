/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilian.cpp
*  PURPOSE:     Civilian ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*

*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg civilian_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_civilian( lua_State *L )
{
    CClientCivilian *cam = (CClientCivilian*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_CIVILIAN, cam );

    j.RegisterInterfaceTrans( L, civilian_interface, 0, LUACLASS_CIVILIAN );

    lua_pushlstring( L, "civilian", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void CClientCivilian::InstanceLua( lua_State *L )
{
    // Lua boot
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_civilian, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );
}

CClientCivilian::CClientCivilian( CClientManager* pManager, int iModel, ElementID ID, lua_State *L ) : CClientEntity( ID, false, resMan->GetVM() )
{
    InstanceLua( L );

    // Initialize members
    m_pManager = pManager;
    m_pCivilianManager = pManager->GetCivilianManager ();
    m_iModel = iModel;

    m_pCivilianPed = NULL;

    // Add this civilian to the civilian list
    m_pCivilianManager->AddToList ( this );

    // Show us by default
    Create ();
}

CClientCivilian::CClientCivilian( CClientManager* pManager, ElementID ID, lua_State *L, CCivilianPed *pCivilianPed ) : CClientEntity( ID, false, resMan->GetVM() )
{
    InstanceLua( L );

    // Initialize members
    m_pManager = pManager;
    m_pCivilianManager = pManager->GetCivilianManager ();
    m_iModel = pCivilianPed->GetModelIndex();

    // Store the given civilian. Also make sure we set our pointer in its stored pointer data.
    m_pCivilianPed = pCivilianPed;
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetStoredPointer ( this );
    }

    // Add our pointer to the manager list
    m_pCivilianManager->AddToList ( this );
}

CClientCivilian::~CClientCivilian( void )
{
    // Reset the stored pointer in the civilian ped if neccessary
    if ( m_pCivilianPed )
    {
        m_pCivilianPed->SetStoredPointer ( NULL );
    }

    // Destroy the civilian
    Destroy ();

    // Remove us from the civilian list
    Unlink ();
}

void CClientCivilian::Unlink()
{
    m_pCivilianManager->RemoveFromList( this );
}

float CClientCivilian::GetDistanceFromCentreOfMassToBaseOfModel ( void )
{
    if ( m_pCivilianPed )
    {
        return m_pCivilianPed->GetBasingDistance();
    }
    return 0.0f;
}

bool CClientCivilian::GetMatrix( RwMatrix& Matrix ) const
{
    m_pCivilianPed->GetMatrix( Matrix );
    return true;
}

bool CClientCivilian::SetMatrix ( const RwMatrix& Matrix )
{
    m_pCivilianPed->SetMatrix ( Matrix );
    return true;
}

void CClientCivilian::GetMoveSpeed ( CVector& vecMoveSpeed ) const
{
    m_pCivilianPed->GetMoveSpeed ( vecMoveSpeed );
}

void CClientCivilian::SetMoveSpeed ( const CVector& vecMoveSpeed )
{
    m_pCivilianPed->SetMoveSpeed ( vecMoveSpeed );
}

void CClientCivilian::GetTurnSpeed ( CVector& vecTurnSpeed ) const
{
    m_pCivilianPed->GetTurnSpeed ( vecTurnSpeed );
}

void CClientCivilian::SetTurnSpeed ( const CVector& vecTurnSpeed )
{
    m_pCivilianPed->SetTurnSpeed ( vecTurnSpeed );
}

bool CClientCivilian::IsVisible ( void )
{
    return m_pCivilianPed->IsVisible ();
}

void CClientCivilian::SetVisible ( bool bVisible )
{
    m_pCivilianPed->SetVisible ( bVisible );
    m_pCivilianPed->SetUsesCollision ( bVisible );
}

float CClientCivilian::GetHealth ( void ) const
{
    return m_pCivilianPed->GetHealth ();
}

void CClientCivilian::SetHealth ( float fHealth )
{
    // Set the health
    m_pCivilianPed->SetHealth ( fHealth );
}

void CClientCivilian::Create ( void )
{
    if ( !m_pCivilianPed )
    {
        m_pCivilianPed = g_pGame->GetPools()->AddCivilianPed( m_iModel );
    }
}

void CClientCivilian::Destroy ( void )
{
    if ( m_pCivilianPed )
    {
        delete m_pCivilianPed;

        m_pCivilianPed = NULL;
    }
}

void CClientCivilian::ReCreate ( void )
{
    // Re-create the civilian.
    Destroy ();
    Create ();
}

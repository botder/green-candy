/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDummy.cpp
*  PURPOSE:     Dummy entity class
*  DEVELOPERS:  Chris McArthur <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static int luaconstructor_dummy( lua_State *L )
{
    CClientDummy *dummy = (CClientDummy*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_DUMMY, dummy );

    lua_pushvalue( L, lua_upvalueindex( 2 ) );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientDummy::CClientDummy ( CClientManager* pManager, ElementID ID, const char *szTypeName, lua_State *L, bool system ) : CClientEntity( ID, system, L )
{
    // Basic lua business
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushstring( L, szTypeName );
    lua_pushcclosure( L, luaconstructor_dummy, 2 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SetTypeName( szTypeName );

    m_pManager = pManager;

    if ( m_pManager )
    {
        m_pGroups = pManager->GetGroups();
        m_pGroups->AddToList( this );
    }
    else
        m_pGroups = NULL;
}

CClientDummy::~CClientDummy()
{
    Unlink();
}

void CClientDummy::Unlink()
{
    if ( m_pGroups )
        m_pGroups->RemoveFromList( this );
}


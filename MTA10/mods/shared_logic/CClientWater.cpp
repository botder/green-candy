/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientWater.cpp
*  PURPOSE:     Water entity class
*  DEVELOPERS:  arc_
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

static const luaL_Reg water_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_water( lua_State *L )
{
    CClientWater *water = (CClientWater*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_WATER, water );

    j.RegisterInterfaceTrans( L, water_interface, 0, LUACLASS_WATER );

    lua_pushlstring( L, "water", 5 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void CClientWater::InstanceLua( bool system )
{
    PushStack( m_lua );
    lua_pushlightuserdata( m_lua, this );
    lua_pushcclosure( m_lua, luaconstructor_water, 1 );
    luaJ_extend( m_lua, -2, 0 );
    lua_pop( m_lua, 1 );
}

CClientWater::CClientWater( CClientManager *pManager, ElementID ID, lua_State *L, bool system, CVector& vecBL, CVector& vecBR, CVector& vecTL, CVector& vecTR, bool bShallow ) : CClientEntity( ID, system, L )
{
    InstanceLua( system );

    m_pManager = pManager;
    m_pWaterManager = pManager->GetWaterManager();
    m_pPoly = g_pGame->GetWaterManager()->CreateQuad( vecBL, vecBR, vecTL, vecTR, bShallow );
    SetTypeName( "water" );

    m_pWaterManager->AddToList( this );
}

CClientWater::CClientWater( CClientManager *pManager, ElementID ID, lua_State *L, bool system, CVector& vecL, CVector& vecR, CVector& vecTB, bool bShallow ) : CClientEntity( ID, system, L )
{
    InstanceLua( system );

    m_pManager = pManager;
    m_pWaterManager = pManager->GetWaterManager();
    m_pPoly = g_pGame->GetWaterManager()->CreateTriangle( vecL, vecR, vecTB, bShallow );
    SetTypeName( "water" );

    m_pWaterManager->AddToList( this );
}

CClientWater::~CClientWater()
{
    Unlink();

    if ( m_pPoly )
        g_pGame->GetWaterManager()->DeletePoly( m_pPoly );
}

size_t CClientWater::GetNumVertices() const
{
    if ( !m_pPoly )
        return 0;
    
    return m_pPoly->GetNumVertices();
}

void CClientWater::GetPosition( CVector& vecPosition ) const
{
    // Calculate the average of the vertex positions
    vecPosition.fX = 0.0f;
    vecPosition.fY = 0.0f;
    vecPosition.fZ = 0.0f;

    if ( !m_pPoly )
        return;

    CVector vecVertexPos;
    for ( unsigned int i = 0; i < GetNumVertices(); i++ )
    {
        m_pPoly->GetVertex( i )->GetPosition( vecVertexPos );
        vecPosition += vecVertexPos;
    }
    vecPosition /= (float)m_pPoly->GetNumVertices();
}

bool CClientWater::GetVertexPosition( unsigned int idx, CVector& pos ) const
{
    if ( !m_pPoly )
        return false;

    CWaterVertex *pVertex = m_pPoly->GetVertex( idx );

    if ( !pVertex )
        return false;

    pVertex->GetPosition( pos );
    return true;
}

void CClientWater::SetPosition( const CVector& pos )
{
    if ( !m_pPoly )
        return;

    CVector vecCurrentPosition;
    GetPosition( vecCurrentPosition );
    CVector vecDelta = pos - vecCurrentPosition;

    CVector vecVertexPos;

    for ( unsigned int i = 0; i < GetNumVertices(); i++ )
    {
        m_pPoly->GetVertex( i )->GetPosition( vecVertexPos );
        vecVertexPos += vecDelta;
        m_pPoly->GetVertex( i )->SetPosition( vecVertexPos );
    }

    g_pGame->GetWaterManager()->RebuildIndex();
}

bool CClientWater::SetVertexPosition( unsigned int idx, const CVector& pos, void *pChangeSource )
{
    if ( !m_pPoly )
        return false;

    CWaterVertex *pVertex = m_pPoly->GetVertex( idx );

    if ( !pVertex )
        return false;

    return pVertex->SetPosition( pos, pChangeSource );
}

void CClientWater::Unlink()
{
    m_pWaterManager->RemoveFromList( this );
}

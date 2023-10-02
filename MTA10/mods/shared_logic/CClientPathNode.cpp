/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathNode.cpp
*  PURPOSE:     Path node entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

static const luaL_Reg pathnode_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_pathnode( lua_State *L )
{
    CClientPathNode *node = (CClientPathNode*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_PATHNODE, node );

    j.RegisterInterfaceTrans( L, pathnode_interface, 0, LUACLASS_PATHNODE );

    lua_pushlstring( L, "pathnode", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientPathNode::CClientPathNode( CClientManager* pManager, CVector& vecPosition,
                                  CVector& vecRotation, int iTime, ElementID ID, lua_State *L, ePathNodeStyle PathStyle,
                                  CClientPathNode* pNextNode ) : CClientEntity( ID, true, L )
{
    // Lua interfacing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_pathnode, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_pManager = pManager;
    m_pPathManager = pManager->GetPathManager();

    m_vecPosition = vecPosition;
    m_vecRotation = vecRotation;
    m_iTime = iTime;
    m_Style = PathStyle;
    m_NextNodeID = INVALID_ELEMENT_ID;
    m_pPreviousNode = NULL;
    m_pNextNode = pNextNode;

    m_pPathManager->AddToList( this );
}

CClientPathNode::~CClientPathNode()
{
    Unlink();

    if ( m_pPreviousNode )
        m_pPreviousNode->SetNextNode( NULL );
    if ( m_pNextNode )
        m_pNextNode->SetPreviousNode( NULL );
}

void CClientPathNode::Unlink()
{
    m_pPathManager->RemoveFromList( this );
}

void CClientPathNode::DoPulse()
{
    attached_t::iterator iter = m_List.begin();
    for ( ; iter != m_List.end(); iter++ )
    {
        // TEST!
        if ( (*iter)->GetType() == CCLIENTVEHICLE )
        {
            CClientVehicle *pVehicle = (CClientVehicle*)*iter;
            CVector vecMoveSpeed;
            pVehicle->GetMoveSpeed( vecMoveSpeed );
            vecMoveSpeed.fZ = 0.0f;
            pVehicle->SetMoveSpeed( vecMoveSpeed );   
        }
        (*iter)->SetPosition( m_vecPosition );        
    }
}

bool CClientPathNode::IsEntityAttached( CClientEntity* pEntity ) const
{
    attached_t::const_iterator iter = m_List.begin();
    for ( ; iter != m_List.end(); iter++ )
    {
        if ( *iter == pEntity )
            return true;
    }

    return false;
}



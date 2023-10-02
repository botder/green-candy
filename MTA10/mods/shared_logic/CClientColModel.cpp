/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModel.cpp
*  PURPOSE:     Model collision (.col file) entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( replace )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientColModel*)lua_getmethodtrans( L ))->Replace( model ) );
    return 1;
}

static LUA_DECLARE( isReplaced )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientColModel*)lua_getmethodtrans( L ))->HasReplaced( model ) );
    return 1;
}

static LUA_DECLARE( restore )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientColModel*)lua_getmethodtrans( L ))->Restore( model ) );
    return 1;
}

static LUA_DECLARE( restoreAll )
{
    ((CClientColModel*)lua_getmethodtrans( L ))->RestoreAll();
    return 0;
}

static const luaL_Reg colmodel_interface[] =
{
    LUA_METHOD( replace ),
    LUA_METHOD( isReplaced ),
    LUA_METHOD( restore ),
    LUA_METHOD( restoreAll ),
    { NULL, NULL }
};

static int luaconstructor_colmodel( lua_State *L )
{
    CClientColModel *col = (CClientColModel*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_COLMODEL, col );

    j.RegisterInterfaceTrans( L, colmodel_interface, 0, LUACLASS_COLMODEL );

    lua_pushlstring( L, "colmodel", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientColModel::CClientColModel( lua_State *L ) : LuaElement( L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_colmodel, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_pColModel = NULL;
}

CClientColModel::~CClientColModel()
{
    delete m_pColModel;
}

bool CClientColModel::LoadCol( CFile *file )
{
    // Not already got a col model?
    if ( m_pColModel )
        return false;

    // Load the collision file
    m_pColModel = g_pGame->GetRenderWare()->ReadCOL( file );

    return m_pColModel != NULL;
}

bool CClientColModel::Replace( unsigned short usModel )
{
    return m_pColModel->Replace( usModel );
}

bool CClientColModel::Restore( unsigned short usModel )
{
    return m_pColModel->Restore( usModel );
}

void CClientColModel::RestoreAll()
{
    m_pColModel->RestoreAll();
}

bool CClientColModel::HasReplaced( unsigned short usModel ) const
{
    return m_pColModel->IsReplaced( usModel );
}
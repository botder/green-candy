/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAtomic.cpp
*  PURPOSE:     Atomic mesh class
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( clone )
{
    CClientAtomic *atom = new CClientAtomic( L, NULL, *((CClientAtomic*)lua_getmethodtrans( L ))->m_atomic.Clone() );
    atom->PushStack( L );
    atom->DisableKeepAlive();
    return 1;
}

static LUA_DECLARE( setClump )
{
    CClientDFF *model;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( model, LUACLASS_DFF, NULL );
    LUA_ARGS_END;

    CClientAtomic *atom = (CClientAtomic*)lua_getmethodtrans( L );

    // Force the atomic into the clump's hierarchy
    // If model is nil, we remove the connection
    if ( model )
    {
        atom->SetModel( model );
        atom->m_atomic.AddToModel( &model->m_model );   // do the internal magic :)

        atom->SetParent( model->m_parent );
        atom->SetRoot( model->m_parent );
        model->m_atomics.insert( model->m_atomics.begin(), atom );
    }
    else if ( atom->m_clump )
    {
        atom->m_clump->m_atomics.remove( atom );
        atom->SetRoot( NULL );

        atom->m_atomic.RemoveFromModel();
        atom->SetModel( NULL );
    }

    LUA_SUCCESS;
}

static LUA_DECLARE( getClump )
{
    CClientDFF *model = ((CClientAtomic*)lua_getmethodtrans( L ))->m_clump;

    if ( !model )
        return 0;

    model->PushStack( L );
    return 1;
}

static LUA_DECLARE( render )
{
    ((CClientAtomic*)lua_getmethodtrans( L ))->m_atomic.Render();
    return 0;
}

static LUA_DECLARE( getWorldSphere )
{
    RwSphere sphere;
    ((CClientAtomic*)lua_getmethodtrans( L ))->m_atomic.GetWorldSphere( sphere );

    lua_pushnumber( L, sphere.pos[0] ); lua_pushnumber( L, sphere.pos[1] ); lua_pushnumber( L, sphere.pos[2] );
    lua_pushnumber( L, sphere.radius );
    return 4;
}

static LUA_DECLARE( replaceModel )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientAtomic*)lua_getmethodtrans( L ))->ReplaceModel( model ) );
    return 1;
}

static LUA_DECLARE( isReplaced )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientAtomic*)lua_getmethodtrans( L ))->HasReplaced( model ) );
    return 1;
}

static LUA_DECLARE( getReplaced )
{
    lua_settop( L, 0 );

    const CRpAtomic::imports_t& impList = ((CClientAtomic*)lua_getmethodtrans( L ))->m_atomic.GetImportList();
    CRpAtomic::imports_t::const_iterator iter = impList.begin();
    int n = 1;

    lua_createtable( L, impList.size(), 0 );

    for ( ; iter != impList.end(); iter++ )
    {
        lua_pushnumber( L, *iter );
        lua_rawseti( L, 1, n++ );
    }

    return 1;
}

static LUA_DECLARE( restoreModel )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    ((CClientAtomic*)lua_getmethodtrans( L ))->RestoreModel( model );
    LUA_SUCCESS;
}

static LUA_DECLARE( restoreAll )
{
    ((CClientAtomic*)lua_getmethodtrans( L ))->RestoreModels();
    return 0;
}

static luaL_Reg atomic_interface_trans[] =
{
    LUA_METHOD( clone ),
    LUA_METHOD( setClump ),
    LUA_METHOD( getClump ),
    LUA_METHOD( render ),
    LUA_METHOD( getWorldSphere ),
    LUA_METHOD( replaceModel ),
    LUA_METHOD( isReplaced ),
    LUA_METHOD( getReplaced ),
    LUA_METHOD( restoreModel ),
    LUA_METHOD( restoreAll ),
    { NULL, NULL }
};

static int luaconstructor_atomic( lua_State *L )
{
    CClientAtomic *atom = (CClientAtomic*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_ATOMIC, atom );

    j.RegisterInterfaceTrans( L, atomic_interface_trans, 0, LUACLASS_ATOMIC );

    lua_pushlstring( L, "atomic", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientAtomic::CClientAtomic( lua_State *L, CClientDFF *model, CRpAtomic& atom ) : CClientRwObject( L, atom ), m_atomic( atom )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_atomic, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_clump = NULL;

    // Maybe assign us to a model
    SetModel( model );
}

CClientAtomic::~CClientAtomic()
{
    SetModel( NULL );
}

void CClientAtomic::SetModel( CClientDFF *model )
{
    if ( model == m_clump )
        return;

    if ( m_clump )
        m_clump->m_atomics.remove( this );

    m_clump = model;

    if ( model )
        model->m_atomics.insert( model->m_atomics.begin(), this );
}

bool CClientAtomic::ReplaceModel( unsigned short id )
{
    if ( !m_atomic.Replace( id ) )
        return false;

    g_pClientGame->GetManager()->Restream( id );
    return true;
}

bool CClientAtomic::HasReplaced( unsigned short id ) const
{
    return m_atomic.IsReplaced( id );
}

void CClientAtomic::RestoreModel( unsigned short id )
{
    if ( !m_atomic.Restore( id ) )
        return;

    g_pClientGame->GetManager()->Restream( id );
}

void CClientAtomic::RestoreModels()
{
    RestreamAll();

    m_atomic.RestoreAll();
}

void CClientAtomic::RestreamAll() const
{
    std::vector <unsigned short> impList = m_atomic.GetImportList();
    std::vector <unsigned short>::iterator iter = impList.begin();

    for ( ; iter != impList.end(); iter++ )
        g_pClientGame->GetManager()->Restream( *iter );
}

void CClientAtomic::MarkGC( lua_State *L )
{
    if ( m_atomic.GetImportList().size() != 0 )
    {
        LuaClass::MarkGC( L );
        return;
    }

    CClientRwObject::MarkGC( L );
}
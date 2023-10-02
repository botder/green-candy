/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicleComponent.cpp
*  PURPOSE:     Vehicle component class export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( getName )
{
    lua_pushstring( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->GetName() );
    return 1;
}

static LUA_DECLARE( setPosition )
{
    CVector pos;

    LUA_ARGS_BEGIN;
    argStream.ReadVector( pos );
    LUA_ARGS_END;

    ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->SetPosition( pos );
    LUA_SUCCESS;
}

static LUA_DECLARE( getPosition )
{
    const CVector& pos = ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->GetPosition();

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static LUA_DECLARE( getWorldPosition )
{
    const CVector& pos = ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->GetWorldPosition();

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static LUA_DECLARE( setMatrix )
{
    RwMatrix *mat;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( mat, LUACLASS_MATRIX );
    LUA_ARGS_END;

    ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->SetMatrix( *mat );
    LUA_SUCCESS;
}

static LUA_DECLARE( getMatrix )
{
    lua_creatematrix( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->GetMatrix() );
    return 1;
}

static LUA_DECLARE( getWorldMatrix )
{
    lua_creatematrix( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->GetWorldMatrix() );
    return 1;
}

static LUA_DECLARE( setActive )
{
    bool active;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( active );
    LUA_ARGS_END;

    ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->SetActive( active );
    LUA_SUCCESS;
}

static LUA_DECLARE( isActive )
{
    lua_pushboolean( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->IsActive() );
    return 1;
}

static LUA_DECLARE( addAtomic )
{
    CClientAtomic *atomic;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( atomic, LUACLASS_ATOMIC );
    LUA_ARGS_END;

    lua_pushnumber( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->AddAtomic( atomic ) );
    return 1;
}

static LUA_DECLARE( getAtomicCount )
{
    lua_pushnumber( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->GetAtomicCount() );
    return 1;
}

static LUA_DECLARE( cloneAtomic )
{
    unsigned int idx;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( idx, 0 );
    LUA_ARGS_END;

    CRpAtomic *inst = ((CClientVehicleComponent*)lua_getmethodtrans( L ))->m_component->CloneAtomic( idx );

    LUA_CHECK( inst );

    CClientAtomic *atom = new CClientAtomic( L, NULL, *inst );
    atom->PushStack( L );
    atom->DisableKeepAlive();
    return 1;
}

static LUA_DECLARE( removeAtomic )
{
    unsigned int idx;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( idx );
    LUA_ARGS_END;

    lua_pushboolean( L, ((CClientVehicleComponent*)lua_getmethodtrans( L ))->RemoveAtomic( idx ) );
    return 1;
}

static luaL_Reg component_interface[] =
{
    LUA_METHOD( getName ),
    LUA_METHOD( setPosition ),
    LUA_METHOD( getPosition ),
    LUA_METHOD( getWorldPosition ),
    LUA_METHOD( setMatrix ),
    LUA_METHOD( getMatrix ),
    LUA_METHOD( getWorldMatrix ),
    LUA_METHOD( setActive ),
    LUA_METHOD( isActive ),
    LUA_METHOD( addAtomic ),
    LUA_METHOD( getAtomicCount ),
    LUA_METHOD( cloneAtomic ),
    LUA_METHOD( removeAtomic ),
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_component )
{
    CClientVehicleComponent *comp = (CClientVehicleComponent*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_VEHICLECOMPONENT, comp );

    j.RegisterInterfaceTrans( L, component_interface, 0, LUACLASS_VEHICLECOMPONENT );

    lua_pushlstring( L, "vehicle-component", 17 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientVehicleComponent::CClientVehicleComponent( CClientVehicle *veh, CVehicleComponent *comp ) : LuaElement( veh->GetVM() )
{
    // Lua instancing
    lua_State *L = veh->GetVM();

    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_component, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    m_vehicle = veh;
    m_component = comp;

    // Link to vehicle
    veh->m_compContainer[ comp->GetName() ] = this;
}

CClientVehicleComponent::~CClientVehicleComponent()
{
    // Unlink from vehicle
    GetVehicle()->m_compContainer.erase( m_component->GetName() );

    delete m_component;
}

unsigned int CClientVehicleComponent::AddAtomic( CClientAtomic *atom )
{
    return m_component->AddAtomic( &atom->m_atomic );
}

unsigned int CClientVehicleComponent::GetAtomicCount() const
{
    return m_component->GetAtomicCount();
}

bool CClientVehicleComponent::RemoveAtomic( unsigned int idx )
{
    return m_component->RemoveAtomic( idx );
}
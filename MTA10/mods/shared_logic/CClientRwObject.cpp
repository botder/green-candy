/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRwObject.cpp
*  PURPOSE:     RenderWare object virtualization
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static LUA_DECLARE( setParent )
{
    // Make sure that we stay in the resource tree!
    CClientRwObject& element = *(CClientRwObject*)lua_getmethodtrans( L );

    if ( lua_type( L, 1 ) != LUA_TCLASS )
    {
        lua_pushmethodsuper( L );

        // If we have a root, we want to reparent to it
        if ( element.m_root )
            element.m_root->PushStack( L );
        else
            lua_pushnil( L );

        lua_call( L, 1, 1 );
        return 1;
    }

    // We can stay without a root
    if ( !element.m_root )
        goto parent;

    // Check if the new parent is inside our root
    element.m_root->PushStack( L );

    if ( lua_refclass( L, 1 )->IsRootedIn( L, -1 ) )
        goto parent;

    // Failed (i.e. we may not be the child of an outside resource class)
    lua_pushboolean( L, false );
    return 1;

parent:
    lua_pushmethodsuper( L );
    lua_pushvalue( L, 1 );
    lua_call( L, 1, 1 );
    return 1;
}

static LUA_DECLARE( getFrame )
{
    CClientRwFrame *frame = ((CClientRwObject*)lua_getmethodtrans( L ))->m_parent;

    if ( !frame )
        return 0;

    frame->PushStack( L );
    return 1;
}

static LUA_DECLARE( isValidChild )
{
    // RenderWare objects have a different type of hierarchy.
    // Some objects - like RpClump - establish connections using the parenthood to a RwFrame.
    // This frame would hold all objects references in the RpClump!
    // By default, objects cannot have children; objects should be held by their parent frame.
    return 0;
}

static luaL_Reg object_interface_trans[] =
{
    LUA_METHOD( setParent ),
    LUA_METHOD( getFrame ),
    LUA_METHOD( isValidChild ),
    { NULL, NULL }
};

static LUA_DECLARE( destroy )
{
	delete (CClientRwObject*)lua_touserdata( L, lua_upvalueindex( 1 ) );

	return 0;
}

static luaL_Reg object_interface[] =
{
	LUA_METHOD( destroy ),
    { NULL, NULL }
};

static LUA_DECLARE( luaconstructor_object )
{
    CClientRwObject *obj = (CClientRwObject*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RWOBJECT, obj );

    j.RegisterInterfaceTrans( L, object_interface_trans, 0, LUACLASS_RWOBJECT );

	lua_pushvalue( L, lua_upvalueindex( 1 ) );
	j.RegisterInterface( L, object_interface, 1 );

    lua_pushlstring( L, "rwobject", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static inline ILuaClass* _trefget( lua_State *L, CClientRwObject *obj )
{
    // Lua instancing
    lua_pushlightuserdata( L, obj );
    lua_pushcclosure( L, luaconstructor_object, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );

	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

CClientRwObject::CClientRwObject( lua_State *L, CRwObject& object ) : LuaClass( L, _trefget( L, this ) ), m_object( object )
{
    m_parent = NULL;
    m_owner = NULL;
    m_root = NULL;

	m_keepAlive = true;
}

CClientRwObject::~CClientRwObject()
{
    UnlinkOwner();

    delete &m_object;
}

void CClientRwObject::UnlinkOwner()
{
    if ( !m_owner )
        return;

    LIST_REMOVE( m_ownerObjects );

    m_owner = NULL;
}

void CClientRwObject::SetOwner( CResource *res )
{
    UnlinkOwner();

    if ( !res )
        return;

    m_owner = res;

    LIST_INSERT( res->m_rwObjects.root, m_ownerObjects );
}

void CClientRwObject::MarkGC( lua_State *L )
{
	if ( m_keepAlive )
		LuaClass::MarkGC( L );
}
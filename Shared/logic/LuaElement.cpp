/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaElement.cpp
*  PURPOSE:     Lua internalization management entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static LUA_DECLARE( setParent )
{
    // Make sure that we stay in the resource tree!
    LuaElement& element = *(LuaElement*)lua_getmethodtrans( L );

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

    if ( lua_equal( L, 1, -1 ) || lua_refclass( L, 1 )->IsRootedIn( L, -1 ) )
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

static LUA_DECLARE( getRoot )
{
    LuaClass *root = ((LuaElement*)lua_touserdata( L, lua_upvalueindex( 1 ) ))->m_root;

    if ( !root )
        return 0;

    root->PushStack( L );
    return 1;
}

static LUA_DECLARE( destroy )
{
    delete (LuaElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg element_interface[] =
{
    LUA_METHOD( destroy ),
    { NULL, NULL }
};

static const luaL_Reg element_interface_trans[] =
{
    LUA_METHOD( setParent ),
    LUA_METHOD( getRoot ),
    { NULL, NULL }
};

static int luaconstructor_element( lua_State *L )
{
    LuaElement *element = (LuaElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_ELEMENT, element );

    // Put basic protection against modification from scripts
    lua_basicprotect( L );

    // Register the element interface
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, element_interface, 1 );

    // Light interfaces take way less memory
    j.RegisterInterfaceTrans( L, element_interface_trans, 0, LUACLASS_ELEMENT );

    // Define our type
    lua_pushlstring( L, "element", 7 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static inline ILuaClass* _trefget( lua_State *L, LuaElement *el )
{
    lua_pushlightuserdata( L, el );
    lua_pushcclosure( L, luaconstructor_element, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );

	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

LuaElement::LuaElement( lua_State *L ) : LuaClass( L, _trefget( L, this ) )
{
    // Root has to be initialized after the object's construction
    m_root = NULL;
}

LuaElement::~LuaElement()
{
}

void LuaElement::SetRoot( LuaClass *root )
{
    // We can also have no root
    // It is unnecessary to remove the parent if no more root
    if ( !root )
    {
        m_root = NULL;
        return;
    }

    // Check whether we are in the root already
    // If so, we do not need to be reparented
    if ( IsRootedIn( root ) )
        return;

    // Reparent it so we are in the correct tree
    m_class->PushMethod( m_lua, "setParent" );
    root->PushStack( m_lua );
    lua_call( m_lua, 1, 1 );

    // Only set if it worked!
    if ( lua_toboolean( m_lua, -1 ) )
        m_root = root;

    lua_pop( m_lua, 1 );
}
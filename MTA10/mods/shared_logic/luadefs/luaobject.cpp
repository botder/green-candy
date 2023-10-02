/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/luaobject.cpp
*  PURPOSE:     Object Lua instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static LUA_DECLARE( setHealth )
{
    float health;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( health );
    LUA_ARGS_END;

    ((CClientObject*)lua_getmethodtrans( L ))->SetHealth( health );
    LUA_SUCCESS;
}

static LUA_DECLARE( getHealth )
{
    lua_pushnumber( L, ((CClientObject*)lua_getmethodtrans( L ))->GetHealth() );
    return 1;
}

static LUA_DECLARE( setRotation )
{
    float rx, ry, rz;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( rx );
    argStream.ReadNumber( ry );
    argStream.ReadNumber( rz );
    LUA_ARGS_END;

    ((CClientObject*)lua_getmethodtrans( L ))->SetRotationDegrees( CVector( rx, ry, rz ) );
    LUA_SUCCESS;
}

static LUA_DECLARE( getRotation )
{
    CVector rot;
    ((CClientObject*)lua_getmethodtrans( L ))->GetRotationDegrees( rot );

    lua_pushnumber( L, rot[0] );
    lua_pushnumber( L, rot[1] );
    lua_pushnumber( L, rot[2] );
    return 3;
}

static LUA_DECLARE( setModel )
{
    unsigned short model;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( model );
    LUA_ARGS_END;

    LUA_CHECK( CClientObjectManager::IsValidModel( model ) );

    ((CClientObject*)lua_getmethodtrans( L ))->SetModel( model );
    LUA_SUCCESS;
}

static LUA_DECLARE( getModel )
{
    lua_pushnumber( L, ((CClientObject*)lua_getmethodtrans( L ))->GetModel() );
    return 1;
}

static LUA_DECLARE( cloneAtomic )
{
    CObject *obj = ((CClientObject*)lua_getmethodtrans( L ))->GetGameObject();

    if ( !obj )
        return 0;

    CClientAtomic *atom = new CClientAtomic( L, NULL, *obj->CloneAtomic() );
    atom->PushStack( L );
    atom->DisableKeepAlive();
    return 1;
}

static LUA_DECLARE( setObjectStatic )
{
    bool enabled;

    LUA_ARGS_BEGIN;
    argStream.ReadBool( enabled );
    LUA_ARGS_END;

    ((CClientObject*)lua_getmethodtrans( L ))->SetStatic( enabled );
    LUA_SUCCESS;
}

static LUA_DECLARE( isObjectStatic )
{
    lua_pushboolean( L, ((CClientObject*)lua_getmethodtrans( L ))->IsStatic() );
    return 1;
}

static LUA_DECLARE( setObjectScale )
{
    float scale;

    LUA_ARGS_BEGIN;
    argStream.ReadNumber( scale );
    LUA_ARGS_END;

    ((CClientObject*)lua_getmethodtrans( L ))->SetScale( scale );
    LUA_SUCCESS;
}

static LUA_DECLARE( getObjectScale )
{
    lua_pushnumber( L, ((CClientObject*)lua_getmethodtrans( L ))->GetScale() );
    return 1;
}

static const luaL_Reg object_interface_trans[] =
{
    LUA_METHOD( setHealth ),
    LUA_METHOD( getHealth ),
    LUA_METHOD( setRotation ),
    LUA_METHOD( getRotation ),
    LUA_METHOD( setModel ),
    LUA_METHOD( getModel ),
    LUA_METHOD( cloneAtomic ),
    LUA_METHOD( setObjectStatic ),
    LUA_METHOD( isObjectStatic ),
    LUA_METHOD( setObjectScale ),
    LUA_METHOD( getObjectScale ),
    { NULL, NULL }
};

LUA_DECLARE( luaconstructor_object )
{
    CClientObject *obj = (CClientObject*)lua_touserdata( L, lua_upvalueindex( 1 ));

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_OBJECT, obj );

    j.RegisterInterfaceTrans( L, object_interface_trans, 0, LUACLASS_OBJECT );

    lua_pushlstring( L, "object", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}
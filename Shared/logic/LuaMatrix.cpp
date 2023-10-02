/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaMatrix.cpp
*  PURPOSE:     Lua RwMatrix instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#if 0

static LUA_DECLARE( clone )
{
    lua_creatematrix( L, *(RwMatrix*)lua_getmethodtrans( L ) );
    return 1;
}

static LUA_DECLARE( add )
{
    RwMatrix *with;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( with, LUACLASS_MATRIX );
    LUA_ARGS_END;

    RwMatrix *mat = (RwMatrix*)lua_getmethodtrans( L );

    mat->Add( *with, *mat );
    LUA_SUCCESS;
}

static LUA_DECLARE( sub )
{
    RwMatrix *with;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( with, LUACLASS_MATRIX );
    LUA_ARGS_END;

    RwMatrix *mat = (RwMatrix*)lua_getmethodtrans( L );

    mat->Sub( *with, *mat );
    LUA_SUCCESS;
}

static LUA_DECLARE( multiply )
{
    RwMatrix *with;

    LUA_ARGS_BEGIN;
    argStream.ReadClass( with, LUACLASS_MATRIX );
    LUA_ARGS_END;

    RwMatrix *mat = (RwMatrix*)lua_getmethodtrans( L );

    mat->Multiply( *with, *mat );
    LUA_SUCCESS;
}

inline static void lua_pushvector3D( lua_State *L, CVector& vec )
{
    lua_createtable( L, 3, 0 );
    lua_pushnumber( L, vec[2] );
    lua_pushnumber( L, vec[1] );
    lua_pushnumber( L, vec[0] );
    lua_rawseti( L, -4, 1 );
    lua_rawseti( L, -3, 2 );
    lua_rawseti( L, -2, 3 );
}

static int matrix_index( lua_State *L )
{
    if ( !lua_isnumber( L, 2 ) )
    {
        lua_getfield( L, LUA_ENVIRONINDEX, "super" );
        lua_insert( L, 1 );
        lua_call( L, 2, 1 );
        return 1;
    }

    RwMatrix *mat = (RwMatrix*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    switch( lua_tointeger( L, 2 ) )
    {
    case 1:     lua_pushvector3D( L, mat->vRight ); break;
    case 2:     lua_pushvector3D( L, mat->vFront ); break;
    case 3:     lua_pushvector3D( L, mat->vUp ); break;
    case 4:     lua_pushvector3D( L, mat->vPos ); break;
    default:    return 0;
    }

    return 1;
}

static int matrix_setPosition( lua_State *L )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 3 );

    ((RwMatrix*)lua_getmethodtrans( L ))->vPos = CVector( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) );
    return 0;
}

static int matrix_getPosition( lua_State *L )
{
    CVector& pos = ((RwMatrix*)lua_getmethodtrans( L ))->vPos;

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static int matrix_setEulerAngles( lua_State *L )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 3 );

    ((RwMatrix*)lua_getmethodtrans( L ))->SetRotation( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) );
    return 0;
}

static int matrix_setEulerAnglesRad( lua_State *L )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 3 );

    ((RwMatrix*)lua_getmethodtrans( L ))->SetRotationRad( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) );
    return 0;
}

static int matrix_getEulerAngles( lua_State *L )
{
    float x, y, z;

    ((RwMatrix*)lua_getmethodtrans( L ))->GetRotation( x, y, z );

    lua_pushnumber( L, x );
    lua_pushnumber( L, y );
    lua_pushnumber( L, z );
    return 3;
}

static int matrix_getEulerAnglesRad( lua_State *L )
{
    float x, y, z;

    ((RwMatrix*)lua_getmethodtrans( L ))->GetRotationRad( x, y, z );

    lua_pushnumber( L, x );
    lua_pushnumber( L, y );
    lua_pushnumber( L, z );
    return 3;
}

static int matrix_fromQuat( lua_State *L )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 4 );

    CQuat quat;
    quat.x = (float)lua_tonumber( L, 1 );
    quat.y = (float)lua_tonumber( L, 2 );
    quat.z = (float)lua_tonumber( L, 3 );
    quat.w = (float)lua_tonumber( L, 4 );

    CQuat::ToMatrix( quat, *(RwMatrix*)lua_getmethodtrans( L ));
    return 0;
}

static int matrix_offset( lua_State *L )
{
    luaL_checktyperange( L, 1, LUA_TNUMBER, 3 );

    CVector pos;
    ((RwMatrix*)lua_getmethodtrans( L ))->Transform( CVector( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) ), pos );

    lua_pushnumber( L, pos[0] );
    lua_pushnumber( L, pos[1] );
    lua_pushnumber( L, pos[2] );
    return 3;
}

static int matrix_destroy( lua_State *L )
{
    delete (RwMatrix*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg matrix_interface[] =
{
    { "__index", matrix_index },
    { "destroy", matrix_destroy },
    { NULL, NULL }
};

static const luaL_Reg matrix_interface_trans[] =
{
    LUA_METHOD( clone ),
    LUA_METHOD( add ),
    LUA_METHOD( sub ),
    LUA_METHOD( multiply ),
    { "setPosition", matrix_setPosition },
    { "getPosition", matrix_getPosition },
    { "setEulerAngles", matrix_setEulerAngles },
    { "setEulerAnglesRad", matrix_setEulerAnglesRad },
    { "getEulerAngles", matrix_getEulerAngles },
    { "getEulerAnglesRad", matrix_getEulerAnglesRad },
    { "fromQuat", matrix_fromQuat },
    { "offset", matrix_offset },
    { NULL, NULL }
};

static int luaconstructor_matrix( lua_State *L )
{
    RwMatrix *matrix = (RwMatrix*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_MATRIX, matrix );

    lua_basicprotect( L );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, matrix_interface, 1 );
    j.RegisterInterfaceTrans( L, matrix_interface_trans, 0, LUACLASS_MATRIX );

    lua_pushlstring( L, "matrix", 6 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void lua_creatematrix( lua_State *L, const RwMatrix& matrix )
{
    lua_pushlightuserdata( L, new RwMatrix( matrix ) );
    lua_pushcclosure( L, luaconstructor_matrix, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );
}

#endif //0
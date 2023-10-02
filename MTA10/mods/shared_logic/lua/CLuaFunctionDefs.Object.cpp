/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Object.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( createObject )
    {
    //  object createObject ( int modelid, float x, float y, float z, [float rx, float ry, float rz] )
        int iModelID; CVector vecPosition; CVector vecRotation;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iModelID );
        argStream.ReadNumber ( vecPosition.fX );
        argStream.ReadNumber ( vecPosition.fY );
        argStream.ReadNumber ( vecPosition.fZ );
        argStream.ReadNumber ( vecRotation.fX, 0 );
        argStream.ReadNumber ( vecRotation.fY, 0 );
        argStream.ReadNumber ( vecRotation.fZ, 0 );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();

            CClientObject* pObject = CStaticFunctionDefinitions::CreateObject( *res, iModelID, vecPosition, vecRotation );
            if ( pObject )
            {
                CElementGroup * pGroup = res->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pObject );
                }

                pObject->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isObjectStatic )
    {
    //  bool isObjectStatic ( object theObject )
        CClientObject* pObject;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pObject, LUACLASS_OBJECT );

        if ( !argStream.HasErrors () )
        {
            lua_pushboolean( L, pObject->IsStatic() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "isObjectStatic" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getObjectScale )
    {
    //  float getObjectScale ( object theObject )
        CClientObject *pObject;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pObject, LUACLASS_OBJECT );

        if ( !argStream.HasErrors() )
        {
            lua_pushnumber( L, pObject->GetScale() );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( moveObject )
    {
    //  bool moveObject ( object theObject, int time, float targetx, float targety, float targetz,
    //      [ float moverx, float movery, float moverz, string strEasingType, float fEasingPeriod, float fEasingAmplitude, float fEasingOvershoot ] )
        CClientEntity* pEntity; int iTime; CVector vecTargetPosition; CVector vecTargetRotation;
        CEasingCurve::eType easingType; float fEasingPeriod; float fEasingAmplitude; float fEasingOvershoot;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadNumber ( iTime );
        argStream.ReadNumber ( vecTargetPosition.fX );
        argStream.ReadNumber ( vecTargetPosition.fY );
        argStream.ReadNumber ( vecTargetPosition.fZ );
        argStream.ReadNumber ( vecTargetRotation.fX, 0 );
        argStream.ReadNumber ( vecTargetRotation.fY, 0 );
        argStream.ReadNumber ( vecTargetRotation.fZ, 0 );
        argStream.ReadEnumString ( easingType, CEasingCurve::Linear );
        argStream.ReadNumber ( fEasingPeriod, 0.3f );
        argStream.ReadNumber ( fEasingAmplitude, 1.0f );
        argStream.ReadNumber ( fEasingOvershoot, 1.70158f );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::MoveObject( *pEntity, iTime, vecTargetPosition, vecTargetRotation, easingType, fEasingPeriod, fEasingAmplitude, fEasingOvershoot ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "moveObject", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( stopObject )
    {
    //  bool stopObject ( object theobject )
        CClientEntity *pEntity;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );

        if ( !argStream.HasErrors () )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::StopObject( *pEntity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setObjectScale )
    {
    //  bool setObjectScale ( object theObject, float scale )
        CClientEntity* pEntity; float fScale;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadNumber ( fScale );

        if ( !argStream.HasErrors () )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetObjectScale( *pEntity, fScale ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setObjectScale", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setObjectStatic )
    {
    //  bool setObjectStatic ( object theObject, bool toggle )
        CClientEntity *pEntity; bool bStatic;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadBool ( bStatic );

        if ( !argStream.HasErrors () )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetObjectStatic( *pEntity, bStatic ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "setObjectStatic", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
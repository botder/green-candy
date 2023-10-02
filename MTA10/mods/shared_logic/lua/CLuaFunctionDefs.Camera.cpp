/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Camera.cpp
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
    LUA_DECLARE( getCameraViewMode )
    {
        unsigned short ucMode;
        if ( CStaticFunctionDefinitions::GetCameraViewMode ( ucMode ) )
        {
            lua_pushnumber ( L, ucMode );
            return 1;
        }

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( getCameraMatrix )
    {
        CVector vecPosition, vecLookAt;
        float fRoll, fFOV;
        if ( CStaticFunctionDefinitions::GetCameraMatrix ( vecPosition, vecLookAt, fRoll, fFOV ) )
        {
            lua_pushnumber ( L, vecPosition.fX );
            lua_pushnumber ( L, vecPosition.fY );
            lua_pushnumber ( L, vecPosition.fZ );
            lua_pushnumber ( L, vecLookAt.fX );
            lua_pushnumber ( L, vecLookAt.fY );
            lua_pushnumber ( L, vecLookAt.fZ );
            lua_pushnumber ( L, fRoll );
            lua_pushnumber ( L, fFOV );
            return 8;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getCameraTarget )
    {
        CClientEntity* pTarget = CStaticFunctionDefinitions::GetCameraTarget ();
        if ( pTarget )
        {
            pTarget->PushStack( L );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getCameraInterior )
    {
        unsigned char ucInterior;
        if ( CStaticFunctionDefinitions::GetCameraInterior ( ucInterior ) )
        {
            lua_pushnumber ( L, ucInterior );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getCameraGoggleEffect )
    {
        if ( g_pMultiplayer->IsNightVisionEnabled() )
            lua_pushstring( L, "nightvision" );
        else if ( g_pMultiplayer->IsThermalVisionEnabled() )
            lua_pushstring( L, "thermalvision" );
        else
            lua_pushstring( L, "normal" );

        return 1;
    }

    LUA_DECLARE( setCameraMatrix )
    {
        if ( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) && lua_isnumber( L, 3 ) )
        {
            // Grab the parameters
            CVector vecPosition;
            vecPosition.fX = (float)lua_tonumber( L, 1 );
            vecPosition.fY = (float)lua_tonumber( L, 2 );
            vecPosition.fZ = (float)lua_tonumber( L, 3 );

            CVector vecLookAt;
            CVector * pvecLookAt = NULL;
            float fRoll = 0.0f;
            float fFOV = 70.0f;

            if ( lua_isnumber( L, 4 ) && lua_isnumber( L, 5 ) && lua_isnumber( L, 6 ) )
            {
                vecLookAt.fX = (float)lua_tonumber( L, 4 );
                vecLookAt.fY = (float)lua_tonumber( L, 5 );
                vecLookAt.fZ = (float)lua_tonumber( L, 6 );
                pvecLookAt = &vecLookAt;

                if ( lua_isnumber( L, 7 ) )
                {
                    fRoll = (float)lua_tonumber( L, 7 );

                    if ( lua_isnumber( L, 8 ) )
                    {
                        fFOV = (float)lua_tonumber( L, 8 );

                        if ( fFOV <= 0.0f || fFOV >= 180.0f )
                            fFOV = 70.0f;
                    }
                }
            }

            lua_pushboolean( L, CStaticFunctionDefinitions::SetCameraMatrix( vecPosition, pvecLookAt, fRoll, fFOV ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setCameraMatrix" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setCameraTarget )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetCameraTarget( entity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setCameraTarget" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setCameraInterior )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetCameraInterior( (unsigned char)lua_tonumber( L, 1 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setCameraInterior" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( fadeCamera )
    {
        CScriptArgReader argStream( L );

        bool fadeIn;
        float time;
        unsigned char r, g, b;

        argStream.ReadBool( fadeIn );
        argStream.ReadNumber( time, 1 );
        argStream.ReadNumber( r, 0 ); argStream.ReadNumber( g, 0 ); argStream.ReadNumber( b, 0 );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::FadeCamera( fadeIn, time, r, g, b ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "fadeCamera", *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( setCameraClip )
    {
        m_pManager->GetCamera()->SetCameraClip( lua_toboolean( L, 1 ) == 1, lua_toboolean( L, 2 ) == 1 );
        lua_pushboolean( L, true );
        return 1;
    }

    LUA_DECLARE( setCameraViewMode )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetCameraViewMode( (unsigned short)lua_tonumber( L, 1 ) ) );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setCameraGoggleEffect )
    {
        if ( lua_type( L, 1 ) == LUA_TSTRING )
        {
            SString strMode = lua_getstring( L, 1 );

            if ( strMode.compare ( "nightvision" ) == 0 )
            {
                g_pMultiplayer->SetNightVisionEnabled   ( true );
                g_pMultiplayer->SetThermalVisionEnabled ( false );
                
                lua_pushboolean( L, true );
                return 1;
            }
            else if ( strMode.compare ( "thermalvision" ) == 0 )
            {
                g_pMultiplayer->SetNightVisionEnabled   ( false );
                g_pMultiplayer->SetThermalVisionEnabled ( true );

                lua_pushboolean( L, true );
                return 1;
            }
            else if ( strMode.compare ( "normal" ) == 0 )
            {
                g_pMultiplayer->SetNightVisionEnabled   ( false );
                g_pMultiplayer->SetThermalVisionEnabled ( false );

                lua_pushboolean( L, true );
                return 1;
            }
        }

        lua_pushboolean ( L, false );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Blip.cpp
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
    LUA_DECLARE( createBlip )
    {
        CVector pos;
        unsigned char ucIcon, ucSize;
        SColor color;
        short sOrdering;
        unsigned short usVisibleDistance;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( pos.fX ); argStream.ReadNumber( pos.fY ); argStream.ReadNumber( pos.fZ );
        argStream.ReadNumber( ucIcon, 0 );
        argStream.ReadNumber( ucSize, 2 );
        argStream.ReadNumber( color.R, 255 );
        argStream.ReadNumber( color.G, 0 );
        argStream.ReadNumber( color.B, 0 );
        argStream.ReadNumber( color.A, 255 );
        argStream.ReadNumber( sOrdering, 0 );
        argStream.ReadNumber( usVisibleDistance, 16383 );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource ();
            CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlip ( *pResource, pos, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
            if ( pMarker )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( pMarker );
                }

                pMarker->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "createBlip" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createBlipAttachedTo )
    {
        CClientEntity *entity;
        unsigned char ucIcon, ucSize;
        SColor color;
        short sOrdering;
        unsigned short usVisibleDistance;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( ucIcon, 0 );
        argStream.ReadNumber( ucSize, 2 );
        argStream.ReadNumber( color.R, 255 );
        argStream.ReadNumber( color.G, 0 );
        argStream.ReadNumber( color.B, 0 );
        argStream.ReadNumber( color.A, 255 );
        argStream.ReadNumber( sOrdering, 0 );
        argStream.ReadNumber( usVisibleDistance, 16383 );

        CLuaMain* pLuaMain = lua_readcontext( L );
        CResource* pResource = pLuaMain->GetResource ();

        if ( !argStream.HasErrors() )
        {
            // Create the blip
            CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo( *pResource, *entity, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
            if ( pMarker )
            {
                CElementGroup *pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( pMarker );
                }
                pMarker->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "createBlipAttachedTo", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getBlipIcon )
    {
        if ( CClientRadarMarker *marker = lua_readclass <CClientRadarMarker> ( L, 1, LUACLASS_RADARMARKER ) )
        {
            lua_pushnumber ( L, (unsigned char)marker->GetSprite() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "getBlipIcon" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getBlipSize )
    {
        if ( CClientRadarMarker *marker = lua_readclass <CClientRadarMarker> ( L, 1, LUACLASS_RADARMARKER ) )
        {
            lua_pushnumber( L, (unsigned char)marker->GetScale() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "getBlipSize" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getBlipColor )
    {
        if ( CClientRadarMarker *marker = lua_readclass <CClientRadarMarker> ( L, 1, LUACLASS_RADARMARKER ) )
        {
            SColor color = marker->GetColor();
            lua_pushnumber( L, color.R );
            lua_pushnumber( L, color.G );
            lua_pushnumber( L, color.B );
            lua_pushnumber( L, color.A );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadType( "getBlipColor" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getBlipOrdering )
    {
        if ( CClientRadarMarker *marker = lua_readclass <CClientRadarMarker> ( L, 1, LUACLASS_RADARMARKER ) )
        {
            lua_pushnumber( L, marker->GetOrdering() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "getBlipOrdering" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getBlipVisibleDistance )
    {
        if ( CClientRadarMarker *marker = lua_readclass <CClientRadarMarker> ( L, 1, LUACLASS_RADARMARKER ) )
        {
            lua_pushnumber ( L, marker->GetVisibleDistance() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "getBlipVisibleDistance" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setBlipIcon )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, lua_isnumber( L, 2 ) &&  CStaticFunctionDefinitions::SetBlipIcon( *entity, (unsigned char)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setBlipIcon" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setBlipSize )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, lua_isnumber( L, 2 ) && CStaticFunctionDefinitions::SetBlipSize( *entity, (unsigned char)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setBlipSize" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setBlipColor )
    {
        CClientEntity *entity;
        SColor color;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( color.R );
        argStream.ReadNumber( color.G );
        argStream.ReadNumber( color.B );
        argStream.ReadNumber( color.A );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetBlipColor( *entity, color ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "setBlipColor", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setBlipOrdering )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, lua_isnumber( L, 2 ) && CStaticFunctionDefinitions::SetBlipOrdering( *entity, (unsigned short)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setBlipOrdering" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setBlipVisibleDistance )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, lua_isnumber( L, 2 ) && CStaticFunctionDefinitions::SetBlipVisibleDistance( *entity, (unsigned short)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setBlipVisibleDistance" );

        lua_pushboolean ( L, false );
        return 1;
    }
}
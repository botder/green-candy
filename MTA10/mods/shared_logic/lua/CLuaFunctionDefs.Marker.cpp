/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Marker.cpp
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
    LUA_DECLARE( createMarker )
    {
        CVector pos;
        SString type;
        float size;
        SColor color;

        CScriptArgReader argStream( L );

        argStream.ReadVector( pos );
        argStream.ReadString( type, "default" );
        argStream.ReadNumber( size, 4.0f );
        argStream.ReadNumber( color.R, 0 ); argStream.ReadNumber( color.G, 0 ); argStream.ReadNumber( color.B, 0xFF ); argStream.ReadNumber( color.A, 0xFF );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource();

            // Create it
            CClientMarker *pMarker = CStaticFunctionDefinitions::CreateMarker( *pResource, pos, type, size, color );
            if ( pMarker )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( ( CClientEntity* ) pMarker );
                }

                pMarker->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMarkerCount )
    {
        lua_pushnumber( L, m_pMarkerManager->Count() );
        return 1;
    }

    LUA_DECLARE( getMarkerType )
    {
        if ( CClientMarker *marker = lua_readclass <CClientMarker> ( L, 1, LUACLASS_MARKER ) )
        {
            char szMarkerType [64];
            if ( CClientMarker::TypeToString( marker->GetMarkerType(), szMarkerType ) )
            {
                lua_pushstring( L, szMarkerType );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getMarkerType", "marker", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMarkerSize )
    {
        if ( CClientMarker *marker = lua_readclass <CClientMarker> ( L, 1, LUACLASS_MARKER ) )
        {
            lua_pushnumber( L, (float)marker->GetSize() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getMarkerSize", "marker", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMarkerColor )
    {
        if ( CClientMarker *marker = lua_readclass <CClientMarker> ( L, 1, LUACLASS_MARKER ) )
        {
            SColor color = marker->GetColor();
            lua_pushnumber ( L, color.R );
            lua_pushnumber ( L, color.G );
            lua_pushnumber ( L, color.B );
            lua_pushnumber ( L, color.A );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getMarkerColor", "marker", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMarkerTarget )
    {
        if ( CClientMarker *marker = lua_readclass <CClientMarker> ( L, 1, LUACLASS_MARKER ) )
        {
            CVector vecTarget;
            if ( CStaticFunctionDefinitions::GetMarkerTarget( *marker, vecTarget ) )
            {
                lua_pushnumber( L, vecTarget.fX );
                lua_pushnumber( L, vecTarget.fY );
                lua_pushnumber( L, vecTarget.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getMarkerTarget", "marker", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMarkerIcon )
    {
        if ( CClientMarker *marker = lua_readclass <CClientMarker> ( L, 1, LUACLASS_MARKER ) )
        {
            char szMarkerIcon [64];
            CClientCheckpoint* pCheckpoint = marker->GetCheckpoint ();
            if ( pCheckpoint )
            {
                CClientCheckpoint::IconToString ( pCheckpoint->GetIcon (), szMarkerIcon );
                lua_pushstring ( L, szMarkerIcon );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getMarkerIcon", "marker", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setMarkerType )
    {
        CClientEntity *entity;
        SString typeName;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetMarkerType( *entity, typeName ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setMarkerSize )
    {
        CClientEntity *entity;
        float size;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( size );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetMarkerSize( *entity, size ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

#define IS_COLOR(x) ( x >= 0 && x <= 255 )

    LUA_DECLARE( setMarkerColor )
    {
        CClientEntity *entity;
        double r, g, b, a;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( r ), argStream.ReadNumber( g ); argStream.ReadNumber( b ); argStream.ReadNumber( a, 255 );

        if ( !argStream.HasErrors() )
        {
            // Set the new color
            lua_pushboolean( L,
                IS_COLOR(r) && IS_COLOR(g) && IS_COLOR(b) && IS_COLOR(a) &&
                CStaticFunctionDefinitions::SetMarkerColor( *entity, 
                    SColorRGBA( (unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setMarkerTarget )
    {
        CClientEntity *entity;
        CVector pos;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );

        if ( !argStream.HasErrors() )
        {
            // Do it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetMarkerTarget( *entity, argStream.ReadVector( pos ) ? &pos : NULL ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setMarkerIcon )
    {
        CClientEntity *entity;
        SString iconName;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( iconName );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetMarkerIcon( *entity, iconName ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
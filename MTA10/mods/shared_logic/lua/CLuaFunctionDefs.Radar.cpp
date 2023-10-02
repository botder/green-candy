/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Radar.cpp
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
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( createRadarArea )
    {
    //  radararea createRadarArea ( float leftX, float bottomY, float sizeX, float sizeY, [ int r = 255, int g = 0, int b = 0, int a = 255, element visibleTo = getRootElement() ] )
        CVector2D vecPosition; CVector2D vecSize; unsigned char r, g, b, a;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( vecPosition.fX );
        argStream.ReadNumber( vecPosition.fY );
        argStream.ReadNumber( vecSize.fX );
        argStream.ReadNumber( vecSize.fY );
        argStream.ReadColor( r, 255 );
        argStream.ReadColor( g, 0 );
        argStream.ReadColor( b, 0 );
        argStream.ReadColor( a, 255 );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();

            // Create it
            CClientRadarArea *pRadarArea = CStaticFunctionDefinitions::CreateRadarArea( *res, vecPosition, vecSize, SColorARGB( a, r, g, b ) );
            if ( pRadarArea )
            {
                CElementGroup *pGroup = res->GetElementGroup();

                if ( pGroup )
                    pGroup->Add( pRadarArea );

                pRadarArea->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getRadarAreaColor )
    {
    //  int, int, int, int getRadarAreaColor( radararea theRadararea )
        CClientRadarArea *pRadarArea;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );

        if ( !argStream.HasErrors() )
        {
            SColor color = pRadarArea->GetColor();

            lua_pushnumber( L, color.R );
            lua_pushnumber( L, color.G );
            lua_pushnumber( L, color.B );
            lua_pushnumber( L, color.A );
            return 4;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getRadarAreaSize )
    {
    //  float, float getRadarAreaSize( radararea theRadararea )
        CClientRadarArea* pRadarArea;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );

        if ( !argStream.HasErrors() )
        {
            const CVector2D& vecSize = pRadarArea->GetSize();

            lua_pushnumber( L, vecSize.fX );
            lua_pushnumber( L, vecSize.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getRadarAreaSize", *argStream.GetErrorMessage () ) );


        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isRadarAreaFlashing )
    {
    //  bool isRadarAreaFlashing ( radararea theRadararea )
        CClientRadarArea* pRadarArea;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, pRadarArea->IsFlashing() );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "isRadarAreaFlashing", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setRadarAreaColor )
    {
    //  bool setRadarAreaColor ( radararea theRadarArea, int r, int g, int b, int a )
        CClientRadarArea* pRadarArea; unsigned char r, g, b, a;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );
        argStream.ReadColor( r );
        argStream.ReadColor( g );
        argStream.ReadColor( b );
        argStream.ReadColor( a, 255 );

        if ( !argStream.HasErrors() )
        {
            pRadarArea->SetColor( SColorRGBA( r, g, b, a ) );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "setRadarAreaColor", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( setRadarAreaSize )
    {
    //  bool setRadarAreaSize ( radararea theRadararea, float x, float y )
        CClientRadarArea *pRadarArea; CVector2D vecSize;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );
        argStream.ReadNumber( vecSize.fX );
        argStream.ReadNumber( vecSize.fY );

        if ( !argStream.HasErrors() )
        {
            pRadarArea->SetSize( vecSize );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "setRadarAreaSize", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setRadarAreaFlashing )
    {
    //  bool setRadarAreaFlashing ( radararea theRadarArea, bool flash )
        CClientRadarArea *pRadarArea; bool bFlash;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );
        argStream.ReadBool( bFlash );

        if ( !argStream.HasErrors() )
        {
            pRadarArea->SetFlashing( bFlash );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "setRadarAreaFlashing", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isInsideRadarArea )
    {
    //  bool isInsideRadarArea ( radararea theArea, float posX, float posY )
        CClientRadarArea *pRadarArea; CVector2D vecPosition;

        CScriptArgReader argStream( L );
        argStream.ReadClass( pRadarArea, LUACLASS_RADARAREA );
        argStream.ReadNumber( vecPosition.fX );
        argStream.ReadNumber( vecPosition.fY );

        if ( !argStream.HasErrors() )
        {
            bool bInside;
            CStaticFunctionDefinitions::IsInsideRadarArea( pRadarArea, vecPosition, bInside );

            lua_pushboolean( L, bInside );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isInsideRadarArea", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
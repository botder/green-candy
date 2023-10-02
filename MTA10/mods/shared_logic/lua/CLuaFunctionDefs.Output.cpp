/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Output.cpp
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
    LUA_DECLARE( outputConsole )
    {
        // Valid string argument?
        if ( lua_isstring( L, 1 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::OutputConsole( lua_tostring( L, 1 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "outputConsole" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( outputChatBox )
    {
        SString output;
        unsigned char r, g, b;
        bool colorCoded;

        CScriptArgReader argStream( L );

        argStream.ReadString( output );
        argStream.ReadColor( r, 235 ); argStream.ReadColor( g, 221 ); argStream.ReadColor( b, 178 );
        argStream.ReadBool( colorCoded, false );

        // Valid string argument?
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::OutputChatBox( output, r, g, b, colorCoded ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setClipboard )
    {
        // Valid string argument?
        if ( lua_isstring( L, 1 ) )
        {
            // set to clip board
            lua_pushboolean( L, CStaticFunctionDefinitions::SetClipboard( SString( lua_getstring( L, 1 ) ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setClipboard" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getClipboard )
    {
        SString strText;
        if ( CStaticFunctionDefinitions::GetClipboard( strText ) )
            lua_pushstring ( L, strText.c_str() );
        else
            lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( enableChat )
    {
        bool enable;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enable );

        if ( !argStream.HasErrors() )
        {
            g_pCore->SetChatEnabled( enable );
            
            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( isChatEnabled )
    {
        lua_pushboolean( L, g_pCore->IsChatEnabled() );
        return 1;
    }

    LUA_DECLARE( showChat )
    {
        // Verify arguments
        if ( lua_istype ( L, 1, LUA_TBOOLEAN ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::ShowChat( lua_toboolean( L, 1 ) == 1 ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "showChat" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Commands.cpp
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
*		        The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( addCommandHandler )
    {
    //  bool addCommandHandler ( string commandName, function handlerFunction, [bool caseSensitive = true] )
        SString strKey; LuaFunctionRef iLuaFunction; bool bCaseSensitive;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strKey );
        argStream.ReadFunction ( iLuaFunction );
        argStream.ReadBool ( bCaseSensitive, true );
        argStream.ReadFunctionComplete ();

        if ( !argStream.HasErrors () )
        {
            // Add them to our list over command handlers
            lua_pushboolean( L, m_pRegisteredCommands->Add( *lua_readcontext( L ), strKey, iLuaFunction, bCaseSensitive ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "addCommandHandler", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( removeCommandHandler )
    {
    //  bool removeCommandHandler ( string commandName )
        SString strKey;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strKey );

        if ( !argStream.HasErrors () )
        {
            // Remove it from our list
            lua_pushboolean( L, m_pRegisteredCommands->Remove( lua_readcontext( L ), strKey ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "removeCommandHandler", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getCommandHandler )
    {
        SString strKey;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strKey );

        if ( !argStream.HasErrors () )
        {
            Command *cmd = m_pRegisteredCommands->Get( strKey.c_str(), lua_readcontext( L ) );

            if ( cmd )
            {
                cmd->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( executeCommandHandler )
    {
    //  bool executeCommandHandler ( string commandName, [ string args ] )
        SString strKey; SString strArgs;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strKey );
        argStream.ReadString ( strArgs, "" );

        if ( !argStream.HasErrors () )
        {
            Command *cmd = m_pRegisteredCommands->Get( strKey.c_str() );

            if ( cmd )
            {
                std::vector <std::string> args;

                CommandlineTools::strsplit( strArgs.c_str(), args );

                lua_pushboolean( L, cmd->Execute( args ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "executeCommandHandler", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Team.cpp
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
    LUA_DECLARE( getTeamFromName )
    {
        if ( lua_isstring( L, 1 ) )
        {
            CClientTeam *pTeam = m_pTeamManager->GetTeam( lua_tostring ( L, 1 ) );
            if ( pTeam )
            {
                pTeam->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getTeamFromName" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTeamName )
    {
        if ( CClientTeam *team = lua_readclass <CClientTeam> ( L, 1, LUACLASS_TEAM ) )
        {
            const char *szName = team->GetTeamName();
            if ( szName )
            {
                lua_pushstring( L, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getTeamName", "team", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTeamColor )
    {
        if ( CClientTeam *team = lua_readclass <CClientTeam> ( L, 1, LUACLASS_TEAM ) )
        {
            unsigned char ucRed, ucGreen, ucBlue;
            team->GetColor( ucRed, ucGreen, ucBlue );

            lua_pushnumber( L, ucRed );
            lua_pushnumber( L, ucGreen );
            lua_pushnumber( L, ucBlue );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getTeamColor", "team", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTeamFriendlyFire )
    {
        if ( CClientTeam *team = lua_readclass <CClientTeam> ( L, 1, LUACLASS_TEAM ) )
        {
            lua_pushboolean( L, team->GetFriendlyFire() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getTeamFriendlyFire", "team", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayersInTeam )
    {
        if ( CClientTeam *team = lua_readclass <CClientTeam> ( L, 1, LUACLASS_TEAM ) )
        {
            lua_newtable ( L );

            unsigned int uiIndex = 0;

            std::list <CClientPlayer*>::const_iterator iter = team->IterBegin();
            for ( ; iter != team->IterEnd(); iter++ )
            {
                lua_pushnumber( L, ++uiIndex );
                (*iter)->PushStack( L );
                lua_settable( L, -3 );
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getPlayersInTeam", "team", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( countPlayersInTeam )
    {
        if ( CClientTeam *team = lua_readclass <CClientTeam> ( L, 1, LUACLASS_TEAM ) )
        {
            lua_pushnumber( L, team->CountPlayers() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "countPlayersInTeam", "team", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }
}
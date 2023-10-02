/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Player.cpp
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
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( getLocalPlayer )
    {
        CStaticFunctionDefinitions::GetLocalPlayer()->PushStack( L );
        return 1;
    }

    LUA_DECLARE( getPlayerName )
    {
    //  string getPlayerName ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNick ();
            if ( szName )
            {
                lua_pushstring ( L, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerName", *argStream.GetErrorMessage () ) );

        // Error
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerFromName )
    {
    //  player getPlayerFromName ( string playerName )
        SString strNick;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strNick );

        if ( !argStream.HasErrors () )
        {
            // Grab the player with that nick
            CClientPlayer* pPlayer = CStaticFunctionDefinitions::GetPlayerFromName ( strNick );
            if ( pPlayer )
            {
                // Return the player
                pPlayer->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerFromName", *argStream.GetErrorMessage () ) );

        // Doesn't exist
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerNametagText )
    {
    //  string getPlayerNametagText ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his nametag text and return it
            const char* szName = pPlayer->GetNametagText ();
            if ( szName )
            {
                lua_pushstring ( L, szName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerNametagText", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerNametagColor )
    {
    //  int, int, int getPlayerNametagColor ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his nametag color and return it
            unsigned char ucR, ucG, ucB;
            pPlayer->GetNametagColor ( ucR, ucG, ucB );

            lua_pushnumber ( L, ucR );
            lua_pushnumber ( L, ucG );
            lua_pushnumber ( L, ucB );
            return 3;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerNametagColor", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPlayerNametagShowing )
    {
    //  bool isPlayerNametagShowing ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            bool bIsNametagShowing = pPlayer->IsNametagShowing ();
            lua_pushboolean ( L, bIsNametagShowing );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isPlayerNametagShowing", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerPing )
    {
    //  int getPlayerPing ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his ping
            unsigned int uiPing = pPlayer->GetPing ();
            lua_pushnumber ( L, uiPing );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerPing", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerTeam )
    {
    //  team getPlayerTeam ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his team and return it
            CClientTeam* pTeam = pPlayer->GetTeam ();
            if ( pTeam )
            {
                pTeam->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPlayerTeam", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPlayerDead )
    {
    //  bool isPlayerDead ( player thePlayer )
        CClientPlayer* pPlayer;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );

        if ( !argStream.HasErrors () )
        {
            // Grab his dead state and return it
            bool bDead = pPlayer->IsDead ();
            lua_pushboolean ( L, bDead );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isPlayerDead", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getPlayerMoney )
    {
        long lMoney;
        if ( CStaticFunctionDefinitions::GetPlayerMoney ( lMoney ) )
        {
            lua_pushnumber ( L, lMoney );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerWantedLevel )
    {
        char cWanted;
        if ( CStaticFunctionDefinitions::GetPlayerWantedLevel ( cWanted ) )
        {
            lua_pushnumber ( L, cWanted );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( showPlayerHudComponent )
    {
    //  bool showPlayerHudComponent ( string component, bool show )
        eHudComponent component; bool bShow;

        CScriptArgReader argStream ( L );
        argStream.ReadEnumString ( component );
        argStream.ReadBool ( bShow );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::ShowPlayerHudComponent ( component, bShow ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "showPlayerHudComponent", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPlayerMoney )
    {
    //  bool setPlayerMoney ( int amount )
        int lMoney;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( lMoney );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetPlayerMoney ( lMoney ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }        
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setPlayerMoney", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( givePlayerMoney )
    {
    //  bool givePlayerMoney ( int amount )
        int lMoney;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( lMoney );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::GivePlayerMoney ( lMoney ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }        
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "givePlayerMoney", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( takePlayerMoney )
    {
    //  bool takePlayerMoney ( int amount )
        int lMoney;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( lMoney );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::TakePlayerMoney ( lMoney ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }        
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "takePlayerMoney", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPlayerNametagText )
    {
    //  bool setPlayerNametagText ( player thePlayer, string text )
        CClientEntity* pPlayer; SString strText;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );
        argStream.ReadString ( strText );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetPlayerNametagText ( *pPlayer, strText ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setPlayerNametagText", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPlayerNametagColor )
    {
        CScriptArgReader argStream ( L );
        if ( !argStream.NextIsBool ( 1 ) )
        {
            // Call type 1
            //  bool setPlayerNametagColor ( player thePlayer, int r, int g, int b )
            CClientEntity* pPlayer; int iR; int iG; int iB;

            argStream.ReadClass( pPlayer, LUACLASS_PLAYER );
            argStream.ReadNumber ( iR );
            argStream.ReadNumber ( iG );
            argStream.ReadNumber ( iB );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, false, iR, iG, iB ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }
        else
        {
            // Call type 2
            //  bool setPlayerNametagColor ( player thePlayer, false )
            CClientEntity* pPlayer; bool bFalse;

            argStream.ReadClass( pPlayer, LUACLASS_PLAYER );
            argStream.ReadBool ( bFalse );

            if ( !argStream.HasErrors () )
            {
                if ( bFalse == false )
                {
                    if ( CStaticFunctionDefinitions::SetPlayerNametagColor ( *pPlayer, true, 255, 255, 255 ) )
                    {
                        lua_pushboolean ( L, true );
                        return 1;
                    }
                }
            }
        }

        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setPlayerNametagColor", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setPlayerNametagShowing )
    {
    //  bool setPlayerNametagShowing ( player thePlayer, bool showing )
        CClientEntity* pPlayer; bool bShowing;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pPlayer, LUACLASS_PLAYER );
        argStream.ReadBool ( bShowing );

        if ( !argStream.HasErrors () )
        {
            // Set the new rotation
            if ( CStaticFunctionDefinitions::SetPlayerNametagShowing ( *pPlayer, bShowing ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setPlayerNametagShowing", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    // Community
    LUA_DECLARE( getPlayerUserName )
    {
        std::string strUser;

        g_pCore->GetCommunity ()->GetUsername ( strUser );
        if ( !strUser.empty () )
        {
            lua_pushstring ( L, strUser.c_str () );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerSerial )
    {
        char szSerial [ 64 ];
        g_pCore->GetNetwork ()->GetSerial ( szSerial, sizeof ( szSerial ) );

        if ( szSerial )
        {
            lua_pushstring ( L, szSerial );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    // Player Map
    LUA_DECLARE( isPlayerMapForced )
    {    
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerMapForced ( bForced ) )
        {
            lua_pushboolean ( L, bForced );
            return 1;
        }    

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isPlayerMapVisible )
    {    
        bool bForced;
        if ( CStaticFunctionDefinitions::IsPlayerMapVisible ( bForced ) )
        {
            lua_pushboolean ( L, bForced );
            return 1;
        }    

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPlayerMapBoundingBox )
    {    
        // Grab the bounding box and return it
        CVector vecMin, vecMax;
        if ( CStaticFunctionDefinitions::GetPlayerMapBoundingBox ( vecMin, vecMax ) )
        {
            lua_pushnumber ( L, vecMin.fX );
            lua_pushnumber ( L, vecMin.fY );
            lua_pushnumber ( L, vecMax.fX );
            lua_pushnumber ( L, vecMax.fY );
            return 4;
        }
        //The map is invisible
        lua_pushboolean ( L, false );
        return 1;
    }
}
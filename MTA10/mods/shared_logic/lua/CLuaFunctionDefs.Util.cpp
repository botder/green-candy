/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Util.cpp
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
    LUA_DECLARE( gettok )
    {
        if ( ( lua_type ( L, 1 ) != LUA_TSTRING ) || ( lua_type ( L, 2 ) != LUA_TNUMBER ) || ( lua_type ( L, 3 ) != LUA_TNUMBER ) && ( lua_type ( L, 3 ) != LUA_TSTRING ) )
        {
            m_pScriptDebugging->LogBadType( "gettok" );

            lua_pushboolean ( L, false );
            return 1;
        }

        SString strDelimiter;
        if ( lua_type ( L, 3 ) == LUA_TNUMBER )
        {
            unsigned int uiCharacter = static_cast < unsigned int > ( lua_tonumber ( L, 3 ) );
            wchar_t wUNICODE[2] = { uiCharacter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            strDelimiter = lua_tostring ( L, 3 );

        const char* szText = lua_tostring ( L, 1 );
        int iToken = static_cast < int > ( lua_tonumber ( L, 2 ) );
        unsigned int uiCount = 0;

        if ( iToken > 0 && iToken < 1024 )
        {
            // Copy the string
            char* strText = new char [ strlen ( szText ) + 1 ];
            strcpy ( strText, szText );

            unsigned int uiCount = 1;
            char* szToken = strtok ( strText, strDelimiter );

            // We're looking for the first part?
            if ( iToken != 1 )
            {
                // strtok count number of times
                do
                {
                    uiCount++;
                    szToken = strtok ( NULL, strDelimiter );
                }
                while ( uiCount != iToken );
            }

            // Found it?
            if ( szToken )
            {
                // Return it
                lua_pushstring ( L, szToken );
                delete [] strText;
                return 1;
            }

            // Delete the text
            delete [] strText;
        }
        else
            m_pScriptDebugging->LogWarning( "Token parameter sent to split must be greater than 0 and smaller than 1024" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( split )
    {
        if ( ( lua_type ( L, 1 ) != LUA_TSTRING ) || ( lua_type ( L, 2 ) != LUA_TNUMBER && ( lua_type ( L, 2 ) != LUA_TSTRING ) ) )
        {
            m_pScriptDebugging->LogBadType( "split" );

            lua_pushboolean ( L, false );
            return 1;
        }

        SString strDelimiter;
        if ( lua_type ( L, 2 ) == LUA_TNUMBER )
        {
            unsigned int uiCharacter = static_cast < unsigned int > ( lua_tonumber ( L, 2 ) );
            wchar_t wUNICODE[2] = { uiCharacter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            strDelimiter = lua_tostring ( L, 2 );

        const char* szText = lua_tostring ( L, 1 );

        // Copy the string
        char* strText = new char [ strlen ( szText ) + 1 ];
        strcpy ( strText, szText );

        unsigned int uiCount = 0;
        char* szToken = strtok ( strText, strDelimiter );

        // Create a new table
        lua_newtable ( L );

        // Add our first token
        lua_pushnumber ( L, ++uiCount );
        lua_pushstring ( L, szToken );
        lua_settable ( L, -3 );

        // strtok until we're out of tokens
        while ( szToken = strtok ( NULL, strDelimiter ) )
        {
            // Add the token to the table
            lua_pushnumber ( L, ++uiCount );
            lua_pushstring ( L, szToken );
            lua_settable ( L, -3 );
        }

        // Delete the text
        delete [] strText;

        return 1;
    }

    LUA_DECLARE( tocolor )
    {
    //  int tocolor ( int red, int green, int blue [, int alpha = 255] )
        int iRed; int iGreen; int iBlue; int iAlpha;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iRed );
        argStream.ReadNumber ( iGreen );
        argStream.ReadNumber ( iBlue );
        argStream.ReadNumber ( iAlpha, 255 );

        if ( !argStream.HasErrors () )
        {
            // Make it into an unsigned long
            unsigned long ulColor = COLOR_RGBA ( iRed, iGreen, iBlue, iAlpha );
            lua_pushinteger ( L, static_cast < lua_Integer > ( ulColor ) );
            return 1;
        }

        // Make it black so funcs dont break
        unsigned long ulColor = COLOR_RGBA ( 0, 0, 0, 255 );
        lua_pushnumber ( L, static_cast < lua_Number > ( ulColor ) );
        return 1;
    }

    LUA_DECLARE( getColorFromString )
    {
    //  int int int int getColorFromString ( string theColor )
        SString strColor ;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strColor );

        if ( !argStream.HasErrors () )
        {
            unsigned char ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha;
            if ( XMLColorToInt ( strColor, ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
            {
                lua_pushnumber ( L, ucColorRed );
                lua_pushnumber ( L, ucColorGreen );
                lua_pushnumber ( L, ucColorBlue );
                lua_pushnumber ( L, ucColorAlpha );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getDistanceBetweenPoints2D", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }


    LUA_DECLARE( getValidPedModels )
    {
        int iIndex = 0;
        lua_newtable ( L );
        for( int i = 0; i < 289; i++)
        {
            if ( CClientPlayerManager::IsValidModel(i) )
            {
                lua_pushnumber ( L , ++iIndex);
                lua_pushnumber ( L , i);
                lua_settable ( L , -3);
            }
        }

        return 1;
    }

    LUA_DECLARE( getNetworkUsageData )
    {
        SPacketStat m_PacketStats [ 2 ] [ 256 ];
        memcpy ( m_PacketStats, g_pNet->GetPacketStats (), sizeof ( m_PacketStats ) );

        lua_createtable ( L, 0, 2 );

        lua_pushstring ( L, "in" );
        lua_createtable ( L, 0, 2 );
        {
            lua_pushstring ( L, "bits" );
            lua_createtable ( L, 255, 1 );
            for ( unsigned int i = 0; i < 256; ++i )
            {
                const SPacketStat& statIn = m_PacketStats [ CNet::STATS_INCOMING_TRAFFIC ] [ i ];
                lua_pushnumber ( L, statIn.iTotalBytes * 8 );
                lua_rawseti ( L, -2, i );
            }
            lua_rawset ( L, -3 );

            lua_pushstring ( L, "count" );
            lua_createtable ( L, 255, 1 );
            for ( unsigned int i = 0; i < 256; ++i )
            {
                const SPacketStat& statIn = m_PacketStats [ CNet::STATS_INCOMING_TRAFFIC ] [ i ];
                lua_pushnumber ( L, statIn.iCount );
                lua_rawseti ( L, -2, i );
            }
            lua_rawset ( L, -3 );
        }
        lua_rawset ( L, -3 );

        lua_pushstring ( L, "out" );
        lua_createtable ( L, 0, 2 );
        {
            lua_pushstring ( L, "bits" );
            lua_createtable ( L, 255, 1 );
            for ( unsigned int i = 0; i < 256; ++i )
            {
                const SPacketStat& statOut = m_PacketStats [ CNet::STATS_OUTGOING_TRAFFIC ] [ i ];
                lua_pushnumber ( L, statOut.iTotalBytes * 8 );
                lua_rawseti ( L, -2, i );
            }
            lua_rawset ( L, -3 );

            lua_pushstring ( L, "count" );
            lua_createtable ( L, 255, 1 );
            for ( unsigned int i = 0; i < 256; ++i )
            {
                const SPacketStat& statOut = m_PacketStats [ CNet::STATS_OUTGOING_TRAFFIC ] [ i ];
                lua_pushnumber ( L, statOut.iCount );
                lua_rawseti ( L, -2, i );
            }
            lua_rawset ( L, -3 );
        }
        lua_rawset ( L, -3 );

        return 1;
    }

    LUA_DECLARE( getNetworkStats )
    {
        NetStatistics stats;
        if ( g_pNet->GetNetworkStatistics ( &stats ) )
        {
            uint uiNumMessagesInSendBuffer = 0;

            lua_createtable ( L, 0, 11 );

            lua_pushstring ( L, "bytesReceived" );
            lua_pushnumber ( L, static_cast < double > ( 0 ) );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "bytesSent" );
            lua_pushnumber ( L, static_cast < double > ( 0 ) );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "packetsReceived" );
            lua_pushnumber ( L, stats.packetsReceived );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "packetsSent" );
            lua_pushnumber ( L, stats.packetsSent );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "packetlossTotal" );
            lua_pushnumber ( L, stats.packetlossTotal );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "packetlossLastSecond" );
            lua_pushnumber ( L, stats.packetlossLastSecond );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "messagesInSendBuffer" );
            lua_pushnumber ( L, uiNumMessagesInSendBuffer );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "messagesInResendBuffer" );
            lua_pushnumber ( L, stats.messagesInResendBuffer );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "isLimitedByCongestionControl" );
            lua_pushnumber ( L, stats.isLimitedByCongestionControl ? 1 : 0 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "isLimitedByOutgoingBandwidthLimit" );
            lua_pushnumber ( L, stats.isLimitedByOutgoingBandwidthLimit ? 1 : 0 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "encryptionStatus" );
            lua_pushnumber ( L, stats.encryptionStatus );
            lua_settable   ( L, -3 );

            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getVersion )
    {
        lua_createtable ( L, 0, 8 );

        lua_pushstring ( L, "number" );
        lua_pushnumber ( L, CStaticFunctionDefinitions::GetVersion () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "mta" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetVersionString () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "name" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetVersionName () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "netcode" );
        lua_pushnumber ( L, CStaticFunctionDefinitions::GetNetcodeVersion () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "os" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetOperatingSystemName () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "type" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetVersionBuildType () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "tag" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetVersionBuildTag () );
        lua_settable   ( L, -3 );

        lua_pushstring ( L, "sortable" );
        lua_pushstring ( L, CStaticFunctionDefinitions::GetVersionSortable () );
        lua_settable   ( L, -3 );

        return 1;
    }

    LUA_DECLARE( getPerformanceStats )
    {
    //  table getPerformanceStats ( string category, string options, string filter )
        SString strCategory, strOptions, strFilter;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strCategory );
        argStream.ReadString ( strOptions, "" );
        argStream.ReadString ( strFilter, "" );

        if ( !argStream.HasErrors () )
        {
            CClientPerfStatResult Result;
            CClientPerfStatManager::GetSingleton ()->GetStats ( &Result, strCategory, strOptions, strFilter );

            lua_newtable ( L );
            for ( int c = 0; c < Result.ColumnCount () ; c++ )
            {
                const SString& name = Result.ColumnName ( c );
                lua_pushnumber ( L, c+1 );                      // row index number (starting at 1, not 0)
                lua_pushlstring ( L, (char *)name.c_str (), name.length() );
                lua_settable ( L, -3 );
            }

            lua_newtable ( L );
            for ( int r = 0; r < Result.RowCount () ; r++ )
            {
                lua_newtable ( L );                             // new table
                lua_pushnumber ( L, r+1 );                      // row index number (starting at 1, not 0)
                lua_pushvalue ( L, -2 );                        // value
                lua_settable ( L, -4 );                         // refer to the top level table

                for ( int c = 0; c < Result.ColumnCount () ; c++ )
                {
                    SString& cell = Result.Data ( c, r );
                    lua_pushnumber ( L, c+1 );
                    lua_pushlstring ( L, (char *)cell.c_str (), cell.length () );
                    lua_settable ( L, -3 );
                }
                lua_pop ( L, 1 );                               // pop the inner table
            }
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getPerformanceStats", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getPerformanceTimer )
    {
        // You might wanna be careful if your machine is running for days :p
        LONGLONG counterFrequency, currentCount;

        LUA_CHECK( 
            QueryPerformanceFrequency( (LARGE_INTEGER*)&counterFrequency ) == TRUE &&
            QueryPerformanceCounter( (LARGE_INTEGER*)&currentCount ) == TRUE
        );

        // Section of high precision math!
        lua_Number ext_perfCount;
        {
            HighPrecisionMathWrap mathWrap;

            long double _currentCount = (long double)currentCount;
            long double _counterFrequency = (long double)counterFrequency;

            long double perfCount = _currentCount / _counterFrequency;

            ext_perfCount = (lua_Number)perfCount;
        }

        lua_pushnumber( L, ext_perfCount );
        return 1;
    }
}
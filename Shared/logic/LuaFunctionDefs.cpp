/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaFunctionDefs.cpp
*  PURPOSE:     Shared Lua function definitions
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

namespace LuaFunctionDefs
{
    ScriptDebugging *debug;
    RegisteredCommands *cmds;
    ResourceManager *resManager;

    void SetResourceManager( ResourceManager& man )
    {
        resManager = &man;
    }

    void SetRegisteredCommands( RegisteredCommands& _cmds )
    {
        cmds = &_cmds;
    }

    void SetDebugging( ScriptDebugging& _debug )
    {
        debug = &_debug;
    }

    LUA_DECLARE( DisabledFunction )
    {
        debug->LogError( "Unsafe function was called." );

        lua_pushboolean( L, false );
        return 1;
    }

#ifdef MTA_CLIENT

    LUA_DECLARE( matrixNew )
    {
        lua_creatematrix( L, RwMatrix() );
        return 1;
    }

    LUA_DECLARE( matrixFromQuat )
    {
        CQuat quat;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( quat.x );
        argStream.ReadNumber( quat.y );
        argStream.ReadNumber( quat.z );
        argStream.ReadNumber( quat.w );

        if ( !argStream.HasErrors() )
        {
            RwMatrix mat;
            CQuat::ToMatrix( quat, mat );

            lua_creatematrix( L, mat );
            return 1;
        }
        else
            debug->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

#endif

    LUA_DECLARE( newmd5hasher )
    {
        CMD5Hasher *hasher = new CMD5Hasher();
        hasher->Init();

        lua_createmd5hasher( L, hasher );
        return 1;
    }

    LUA_DECLARE( md5 )
    {
        std::string buf;

        CScriptArgReader argStream( L );
        argStream.ReadString( buf );

        if ( !argStream.HasErrors() )
        {
            MD5 md5bytes;
            char szResult[33];
            CMD5Hasher hasher;

            hasher.Calculate( buf.c_str(), buf.size(), md5bytes );
            hasher.ConvertToHex( md5bytes, szResult );
            lua_pushstring( L, szResult );
            return 1;
        }
        else
        {
            std::string err_msg( "Bad argument @ '" );
            err_msg += __FUNCTION__;
            err_msg += "' [";
            err_msg += argStream.GetErrorMessage();
            err_msg += "]";
            debug->LogCustom( err_msg.c_str() );
        }

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( hashString )
    {
        std::string buf;

        LUA_ARGS_BEGIN;
        argStream.ReadString( buf );
        LUA_ARGS_END;

        lua_pushnumber( L, TumblerHash( buf.c_str(), buf.size() ) );
        return 1;
    }

    LUA_DECLARE( fileCreateDir )
    {
        const char *path;

        LUA_ARGS_BEGIN;
        argStream.ReadString( path );
        LUA_ARGS_END;

        lua_pushboolean( L, resManager->CreateDir( lua_readresource( L ), path ) );
        return 1;
    }

    LUA_DECLARE( fileCreate )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        CFile *file = resManager->OpenStream( lua_readresource( L ), lua_tostring( L, 1 ), "wb+" );

        if ( !file )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        lua_pushlightuserdata( L, file );
        lua_pushcclosure( L, luaconstructor_file, 1 );
        lua_newclass( L );
        return 1;
    }

    LUA_DECLARE( fileExists )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        lua_pushboolean( L, resManager->FileExists( lua_readresource( L ), lua_tostring( L, 1 ) ) );
        return 1;
    }

    LUA_DECLARE( fileOpen )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        CFile *file = resManager->OpenStream( lua_readresource( L ), lua_tostring( L, 1 ), "rb+" );

        if ( !file )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        lua_pushlightuserdata( L, file );
        lua_pushcclosure( L, luaconstructor_file, 1 );
        lua_newclass( L );
        return 1;
    }

    LUA_DECLARE( fileIsEOF )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );

        j.PushMethod( L, "eof" );
        lua_call( L, 0, 1 );
        return 1;
    }

    LUA_DECLARE( fileGetPos )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );

        j.PushMethod( L, "tell" );
        lua_call( L, 0, 1 );
        return 1;
    }

    LUA_DECLARE( fileSetPos )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );
        luaL_checktype( L, 2, LUA_TNUMBER );

        j.PushMethod( L, "seek" );
        lua_pushvalue( L, 2 );
        lua_pushlstring( L, "set", 3 );
        lua_call( L, 2, 1 );
        return 1;
    }

    LUA_DECLARE( fileGetSize )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );

        j.PushMethod( L, "size" );
        lua_call( L, 0, 1 );
        return 1;
    }

    LUA_DECLARE( fileRead )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );
        luaL_checktype( L, 2, LUA_TNUMBER );

        j.PushMethod( L, "read" );
        lua_pushvalue( L, 2 );
        lua_call( L, 1, 1 );
        return 1;
    }

    LUA_DECLARE( fileWrite )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );
        luaL_checktype( L, 2, LUA_TSTRING );

        j.PushMethod( L, "write" );
        lua_pushvalue( L, 2 );
        lua_call( L, 1, 1 );
        return 1;
    }

    LUA_DECLARE( fileFlush )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );

        j.PushMethod( L, "flush" );
        lua_call( L, 0, 1 );
        return 1;
    }

    LUA_DECLARE( fileClose )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_FILE );

        j.PushMethod( L, "destroy" );
        lua_call( L, 0, 0 );

        lua_pushboolean( L, true );
        return 1;
    }

    LUA_DECLARE( fileDelete )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        lua_pushboolean( L, resManager->FileDelete( lua_readresource( L ), lua_tostring( L, 1 ) ) );
        return 1;
    }

    LUA_DECLARE( fileRename )
    {
        luaL_checktype( L, 1, LUA_TSTRING );
        luaL_checktype( L, 2, LUA_TSTRING );

        lua_pushboolean( L, resManager->FileRename( lua_readresource( L ), lua_tostring( L, 1 ), lua_tostring( L, 2 ) ) );
        return 1;
    }

    LUA_DECLARE( fileCopy )
    {
        luaL_checktype( L, 1, LUA_TSTRING );
        luaL_checktype( L, 2, LUA_TSTRING );

        lua_pushboolean( L, resManager->FileCopy( lua_readresource( L ), lua_tostring( L, 1 ), lua_tostring( L, 2 ) ) );
        return 1;
    }

    LUA_DECLARE( utfLen )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        lua_pushwideinteger( L, (lua_WideInteger)MbUTF8ToUTF16( lua_getstring( L, 1 ) ).size() );
        return 1;
    }

    LUA_DECLARE( utfSeek )
    {
        luaL_checktype( L, 1, LUA_TSTRING );
        luaL_checktype( L, 2, LUA_TNUMBER );

        int iPos = (int)lua_tonumber( L, 2 );
        std::wstring utf = MbUTF8ToUTF16( lua_getstring( L, 1 ) );

        if ( iPos > (int)utf.size() || iPos < 0 )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        utf = utf.substr( 0, iPos );
        lua_pushwideinteger( L, (lua_WideInteger)UTF16ToMbUTF8( utf ).size() );
        return 1;
    }

    LUA_DECLARE( utfSub )
    {
        luaL_checktype( L, 1, LUA_TSTRING );
        luaL_checktype( L, 2, LUA_TNUMBER );

        //Ripped and modded Lua source.  It's pretty disgusting, i know.

        const char *s = lua_tostring(L, 1);
        std::wstring strUTF = MbUTF8ToUTF16(s);
        size_t l = static_cast < int > ( strUTF.size() );

        ptrdiff_t start = luaL_checkinteger(L, 2);
        ptrdiff_t end = luaL_optinteger(L, 3, -1);

        //posrelat them both
        if (start < 0) start += (ptrdiff_t)l + 1;
            start = (start >= 0) ? start : 0;

        if (end < 0) end += (ptrdiff_t)l + 1;
            end = (end >= 0) ? end : 0;

        if (start < 1) start = 1;
        if (end > (ptrdiff_t)l) end = (ptrdiff_t)l;
        if (start <= end)
        {
            strUTF = strUTF.substr(start-1, end-start+1);
            lua_pushstring(L, UTF16ToMbUTF8(strUTF).c_str());
        }
        else lua_pushliteral(L, "");
        return 1;
    }

    LUA_DECLARE( utfChar )
    {
        luaL_checktype( L, 1, LUA_TNUMBER );

        int iChar = (int)lua_tonumber( L, 1 );

        if ( iChar > 65534 || iChar < 32 )
            throw lua_exception( L, LUA_ERRRUN, "invalid character to utfChar" );

        // Generate a null-terminating string for our character
        wchar_t wUNICODE[2] = { (wchar_t)iChar, '\0' };

        // Return the ANSI representation
        lua_pushlstring( L, UTF16ToMbUTF8( wUNICODE ).c_str(), 1 );
        return 1;
    }

    LUA_DECLARE( utfCode )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        std::wstring utf = MbUTF8ToUTF16( lua_getstring( L, 1 ) );

        lua_pushnumber( L, utf[0] );
        return 1;
    }

    LUA_DECLARE( getDistanceBetweenPoints2D )
    {
        luaL_checktyperange( L, 1, LUA_TNUMBER, 4 );

        CVector2D a( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ) );
        CVector2D b( (float)lua_tonumber( L, 3 ), (float)lua_tonumber( L, 4 ) );
        a -= b;

        lua_pushnumber( L, a.Length() );
        return 1;
    }

    LUA_DECLARE( getDistanceBetweenPoints3D )
    {
        luaL_checktyperange( L, 1, LUA_TNUMBER, 6 );

        CVector a( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) );
        CVector b( (float)lua_tonumber( L, 4 ), (float)lua_tonumber( L, 5 ), (float)lua_tonumber( L, 6 ) );
        a -= b;

        lua_pushnumber( L, a.Length() );
        return 1;
    }

    LUA_DECLARE( getTickCount )
    {
        lua_pushnumber( L, GetSecondCount() * 1000.0 );
        return 1;
    }

    LUA_DECLARE( getCTime )
    {
        time_t timer;
        time( &timer );

        int top = lua_gettop( L );

        if ( top >= 1 )
        {
            switch( lua_type( L, 1 ) )
            {
            case LUA_TNUMBER:
            case LUA_TSTRING:
                timer = (time_t)lua_tonumber( L, 1 );

                if ( timer < 0 )
                {
                    lua_pushboolean( L, false );
                    return 1;
                }

                break;
            }

            lua_settop( L, 0 );
        }

        tm *time = localtime( &timer );

        lua_newtable( L );
        lua_pushlstring( L, "second", 6 );
        lua_pushnumber( L, time->tm_sec );
        lua_pushlstring( L, "minute", 6 );
        lua_pushnumber( L, time->tm_min );
        lua_pushlstring( L, "hour", 4 );
        lua_pushnumber( L, time->tm_hour );
        lua_pushlstring( L, "monthday", 8 );
        lua_pushnumber( L, time->tm_mday );
        lua_pushlstring( L, "month", 5 );
        lua_pushnumber( L, time->tm_mon );
        lua_pushlstring( L, "year", 4 );
        lua_pushnumber( L, time->tm_year );
        lua_pushlstring( L, "weekday", 7 );
        lua_pushnumber( L, time->tm_wday );
        lua_pushlstring( L, "yearday", 7 );
        lua_pushnumber( L, time->tm_yday );
        lua_pushlstring( L, "isdst", 5 );
        lua_pushboolean( L, time->tm_isdst == 1 );
        lua_pushlstring( L, "timestamp", 9 );
        lua_pushnumber( L, (double)timer );

        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        lua_rawset( L, 1 );
        return 1;
    }

#if 0
    LUA_DECLARE( split )
    {
        if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER && ( lua_type ( luaVM, 2 ) != LUA_TSTRING ) ) )
        {
            m_pScriptDebugging->LogBadType ( luaVM, "split" );

            lua_pushboolean ( luaVM, false );
            return 1;
        }

        SString strDelimiter;
        if ( lua_type ( luaVM, 2 ) == LUA_TNUMBER )
        {
            unsigned int uiCharacter = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
            wchar_t wUNICODE[2] = { uiCharacter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            strDelimiter = lua_tostring ( luaVM, 2 );

        const char* szText = lua_tostring ( luaVM, 1 );

        // Copy the string
        char* strText = new char [ strlen ( szText ) + 1 ];
        strcpy ( strText, szText );

        unsigned int uiCount = 0;
        char* szToken = strtok ( strText, strDelimiter );

        // Create a new table
        lua_newtable ( luaVM );

        // Add our first token
        lua_pushnumber ( luaVM, ++uiCount );
        lua_pushstring ( luaVM, szToken );
        lua_settable ( luaVM, -3 );

        // strtok until we're out of tokens
        while ( ( szToken = strtok ( NULL, strDelimiter ) ) )
        {
            // Add the token to the table
            lua_pushnumber ( luaVM, ++uiCount );
            lua_pushstring ( luaVM, szToken );
            lua_settable ( luaVM, -3 );
        }

        // Delete the text
        delete [] strText;

        return 1;
    }

    int CLuaFunctionDefinitions::GetTok ( lua_State* luaVM )
    {
        if ( ( lua_type ( luaVM, 1 ) != LUA_TSTRING ) || ( lua_type ( luaVM, 2 ) != LUA_TNUMBER ) || ( ( lua_type ( luaVM, 3 ) != LUA_TNUMBER ) && ( lua_type ( luaVM, 3 ) != LUA_TSTRING ) ) )
        {
            m_pScriptDebugging->LogBadType ( luaVM, "gettok" );

            lua_pushboolean ( luaVM, false );
            return 1;
        }

        SString strDelimiter;
        if ( lua_type ( luaVM, 3 ) == LUA_TNUMBER )
        {
            unsigned int uiCharacter = static_cast < unsigned int > ( lua_tonumber ( luaVM, 3 ) );
            wchar_t wUNICODE[2] = { uiCharacter, '\0' };
            strDelimiter = UTF16ToMbUTF8(wUNICODE);
        }
        else  // It's already a string
            strDelimiter = lua_tostring ( luaVM, 3 );

        const char* szText = lua_tostring ( luaVM, 1 );
        int iToken = static_cast < int > ( lua_tonumber ( luaVM, 2 ) );

        if ( iToken > 0 && iToken < 1024 )
        {
            // Copy the string
            char* strText = new char [ strlen ( szText ) + 1 ];
            strcpy ( strText, szText );

            int iCount = 1;
            char* szToken = strtok ( strText, strDelimiter );

            // We're looking for the first part?
            if ( iToken != 1 )
            {
                // strtok count number of times
                do
                {
                    iCount++;
                    szToken = strtok ( NULL, strDelimiter );
                }
                while ( iCount != iToken );
            }

            // Found it?
            if ( szToken )
            {
                // Return it
                lua_pushstring ( luaVM, szToken );
                delete [] strText;
                return 1;
            }

            // Delete the text
            delete [] strText;
        }
        else
            m_pScriptDebugging->LogWarning ( luaVM, "Token parameter sent to split must be greater than 0 and smaller than 1024" );

        lua_pushboolean ( luaVM, false );
        return 1;
    }
#endif

    LUA_DECLARE( setTimer )
    {
        luaL_checktype( L, 1, LUA_TFUNCTION );
        luaL_checktype( L, 2, LUA_TNUMBER );
        luaL_checktype( L, 3, LUA_TNUMBER );

        LuaMain& main = *lua_readcontext( L );
        unsigned int count = (unsigned int)lua_tonumber( L, 3 );
        double interval = lua_tonumber( L, 2 );

        // Check for the minimum interval
        if ( interval < LUA_TIMER_MIN_INTERVAL )
            throw lua_exception( L, LUA_ERRRUN, "interval is below 50" );

        LuaFunctionRef ref = main.CreateReference( L, 1 );

        LuaTimer& timer = *main.GetTimerManager().AddTimer( L, ref, (std::chrono::milliseconds)( (std::int64_t)interval ), count );
        timer.ObtainArguments( L, 4 );
        timer.PushStack( L );
        return 1;
    }

    LUA_DECLARE( killTimer )
    {
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_TIMER );

        j.PushMethod( L, "destroy" );
        lua_call( L, 0, 0 );
        return 0;
    }

    LUA_DECLARE( resetTimer )
    {
    //  bool resetTimer ( timer theTimer )
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_TIMER );

        j.PushMethod( L, "reset" );
        lua_call( L, 0, 0 );

        lua_pushboolean( L, true );
        return 1;
    }

    LUA_DECLARE( isTimer )
    {
        return lua_type( L, 1 ) == LUA_TCLASS && lua_refclass( L, 1 )->IsTransmit( LUACLASS_TIMER );
    }

    LUA_DECLARE( getTimers )
    {
    //  table getTimers ( [ time ] )
        LuaMain& main = *lua_readcontext( L );

        lua_newtable( L );

        double time;

        if ( lua_gettop( L ) > 1 )
            time = lua_tonumber( L, 1 );
        else
            time = 0;

        // Add all the timers with less than ulTime left
        main.GetTimerManager().GetTimers(
            (std::chrono::milliseconds)(std::int64_t)time,
            &main
        );
        return 1;
    }

    LUA_DECLARE( getTimerDetails )
    {
    //  int, int, int getTimerDetails ( timer theTimer )
        ILuaClass& j = lua_classobtain( L, 1, LUACLASS_TIMER );

        j.PushMethod( L, "getDetails" );
        lua_call( L, 0, 3 );
        return 3;
    }

#if 0
    LUA_DECLARE( getColorFromString )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        unsigned char r, g, b;

        if ( !XMLColorToInt( lua_tostring( L, 1 ), ucColorRed, ucColorGreen, ucColorBlue, ucColorAlpha ) )
        {
            lua_pushnumber ( luaVM, ucColorRed );
            lua_pushnumber ( luaVM, ucColorGreen );
            lua_pushnumber ( luaVM, ucColorBlue );
            lua_pushnumber ( luaVM, ucColorAlpha );
            return 4;
        }
    }

    int CLuaFunctionDefinitions::GetValidPedModels ( lua_State* luaVM )
    {
        int iIndex = 0;
        lua_newtable ( luaVM );
        for( int i = 0; i < 289; i++)
        {
            if ( CPlayerManager::IsValidPlayerModel(i) )
            {
                lua_pushnumber ( luaVM , ++iIndex);
                lua_pushnumber ( luaVM , i);
                lua_settable ( luaVM , -3);
            }
        }

        return 1;
    }
#endif

    LUA_DECLARE( outputDebugString )
    {
        SString output;
        unsigned int level;
        unsigned char r, g, b;

        LUA_ARGS_BEGIN;
        argStream.ReadString( output );
        argStream.ReadNumber( level, 3 );
        if ( level == 0 )
        {
            argStream.ReadColor( r, 0xFF ); argStream.ReadColor( g, 0xFF ); argStream.ReadColor( b, 0xFF );
        }
        LUA_ARGS_END;

        // Output it
        switch( level )
        {
        case 1:     debug->LogError( "%s", output.c_str() ); break;
        case 2:     debug->LogWarning( "%s", output.c_str() ); break;
        case 3:     debug->LogInformation( "%s", output.c_str() ); break;
        case 0:     debug->LogCustom( r, g, b, "%s", output.c_str() ); break;
        default:
            debug->LogWarning( "Bad level argument sent to outputDebugString (0-3)" );

            lua_pushboolean( L, false );
            return 1;
        }

        // Success
        lua_pushboolean( L, true );
        return 1;
    }

    LUA_DECLARE( print )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        debug->LogInformation( "%s", lua_tostring( L, 1 ) );
        return 0;
    }
};

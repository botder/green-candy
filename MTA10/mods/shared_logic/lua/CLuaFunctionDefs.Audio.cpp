/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Audio.cpp
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
    LUA_DECLARE( playSound )
    {
        filePath strSound;
        bool loop;

        CScriptArgReader argStream( L );

        argStream.ReadString( strSound );
        argStream.ReadBool( loop, false );

        if ( !argStream.HasErrors() )
        {
            filePath strSound = lua_getstring( L, 1 ).c_str();
            CLuaMain *luaMain = lua_readcontext( L );
            CResource *resource = luaMain->GetResource();

            // Check if it is a URL
            bool isURL = strnicmp( strSound.c_str(), "http:", 5 ) == 0 || strnicmp( strSound.c_str(), "www.", 4 ) == 0;
            const char *meta;

            if ( !isURL )
            {
                filePath strFilename;

                if ( !m_pResourceManager->ParseResourceFullPath( (Resource*&)resource, strSound.c_str(), meta, strFilename ) )
                    goto fail;

                strSound = strFilename;
            }

            CClientSound *sound = CStaticFunctionDefinitions::PlaySound( resource, strSound.convert_ansi(), isURL, loop );

            if ( sound )
            {
                sound->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

fail:
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( playSound3D )
    {
        if ( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) && lua_isnumber( L, 3 ) )
        {
            CVector vecPosition ( static_cast < float > ( lua_tonumber ( L, 2 ) ),
                static_cast < float > ( lua_tonumber ( L, 3 ) ),
                static_cast < float > ( lua_tonumber ( L, 4 ) ) );

            SString strSound = lua_tostring ( L, 1 );

            CLuaMain * luaMain = lua_readcontext( L );
            CResource* pResource = luaMain->GetResource();
            const char *meta;
            filePath strFilename;
            bool bIsURL = !m_pResourceManager->ParseResourceFullPath( (Resource*&)pResource, strSound, meta, strFilename );

            if ( !bIsURL )
                strSound = strFilename.convert_ansi();

            bool bLoop = false;
            if ( lua_istype ( L, 5, LUA_TBOOLEAN ) )
            {
                bLoop = ( lua_toboolean ( L, 5 ) ) ? true : false;
            }

            CClientSound* pSound = CStaticFunctionDefinitions::PlaySound3D ( pResource, strSound, bIsURL, vecPosition, bLoop );
            if ( pSound )
            {
                pSound->PushStack( L );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( stopSound )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            sound->Delete();

            lua_pushboolean( L, true );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundPosition )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_isnumber( L, 2 ) )
            {
                sound->SetPlayPosition( lua_tonumber( L, 2 ) );
                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundPosition )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetPlayPosition() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundLength )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetLength() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundPaused )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_istype( L, 2, LUA_TBOOLEAN ) )
            {
                sound->SetPaused( lua_toboolean( L, 2 ) == 1 );
                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isSoundPaused )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushboolean( L, sound->IsPaused() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundVolume )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_isnumber( L, 2 ) )
            {
                sound->SetVolume( (float)lua_tonumber( L, 2 ) );
                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundVolume )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetVolume() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundSpeed )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_isnumber( L, 2 ) )
            {
                sound->SetPlaybackSpeed( (float)lua_tonumber( L, 2 ) );

                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundSpeed )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetPlaybackSpeed() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundMinDistance )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_isnumber( L, 2 ) )
            {
                sound->SetMinDistance( (float)lua_tonumber( L, 2 ) );

                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundMinDistance )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetMinDistance() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundMaxDistance )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            if ( lua_isnumber( L, 2 ) )
            {
                sound->SetMaxDistance( (float)lua_tonumber( L, 2 ) );

                lua_pushboolean( L, true );
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundMaxDistance )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushnumber( L, sound->GetMaxDistance() );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundMetaTags )
    {
        if ( CClientSound *pSound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            SString strMetaTags = "";
            if ( lua_istype ( L, 2, LUA_TSTRING ) )
            {
                SString strFormat = lua_tostring ( L, 2 );
                if ( CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, strFormat, strMetaTags ) )
                {
                    if ( !strMetaTags.empty() )
                    {
                        lua_pushstring ( L, strMetaTags );
                        return 1;
                    }
                }
            }
            else
            {
                lua_newtable ( L );
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%TITL", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "title" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%ARTI", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "artist" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%ALBM", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "album" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%GNRE", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "genre" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%YEAR", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "year" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%CMNT", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "comment" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%TRCK", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "track" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%COMP", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "composer" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%COPY", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "copyright" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%SUBT", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "subtitle" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "%AART", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "album_artist" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "streamName", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "stream_name" );
                }
                CStaticFunctionDefinitions::GetSoundMetaTags ( *pSound, "streamTitle", strMetaTags );
                if ( !strMetaTags.empty() )
                {
                    lua_pushstring ( L, strMetaTags );
                    lua_setfield ( L, -2, "stream_title" );
                }
                return 1;
            }
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setSoundEffectEnabled )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            lua_pushboolean( L, lua_isstring( L, 2 ) && CStaticFunctionDefinitions::SetSoundEffectEnabled( *sound, lua_tostring( L, 2 ), lua_toboolean( L, 3 ) == 1 ) );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSoundEffects )
    {
        if ( CClientSound *sound = lua_readclass <CClientSound> ( L, 1, LUACLASS_SOUND ) )
        {
            std::map <std::string, int> iFxEffects = m_pManager->GetSoundManager()->GetFxEffects();
            lua_newtable( L );
            for ( std::map <std::string, int>::const_iterator iter = iFxEffects.begin(); iter != iFxEffects.end(); ++iter )
            {
                lua_pushboolean( L, sound->IsFxEffectEnabled ( (*iter).second ) );
                lua_setfield( L, -2, (*iter).first.c_str () );
            }
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( playMissionAudio )
    {
        if ( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) && lua_isnumber( L, 3 ) && lua_isnumber( L, 4 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::PlayMissionAudio(
                CVector( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ), (float)lua_tonumber( L, 3 ) ),
                (unsigned short)lua_tonumber( L, 4 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "playMissionAudio" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( playSoundFrontEnd )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            unsigned char ucSound = static_cast < unsigned char > ( lua_tonumber ( L, 1 ) );
            if ( ucSound <= 101 )
            {
                lua_pushboolean( L, CStaticFunctionDefinitions::PlaySoundFrontEnd ( ucSound ) );
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "Invalid sound ID specified. Valid sound IDs are 0 - 101." );
        }
        else
            m_pScriptDebugging->LogBadType( "playSoundFrontEnd" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( preloadMissionAudio )
    {
        if ( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::PreloadMissionAudio( (unsigned short)lua_tonumber( L, 1 ), (unsigned short)lua_tonumber( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "preloadMissionAudio" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setAmbientSoundEnabled )
    {
        eAmbientSoundType eType; bool bEnabled;

        CScriptArgReader argStream ( L );
        argStream.ReadEnumString ( eType, AMBIENT_SOUND_GENERAL );
        argStream.ReadBool ( bEnabled );

        if ( !argStream.HasErrors () )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetAmbientSoundEnabled ( eType, bEnabled ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setAmbientSoundEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isAmbientSoundEnabled )
    {
        eAmbientSoundType eType;

        CScriptArgReader argStream ( L );
        argStream.ReadEnumString ( eType );

        if ( !argStream.HasErrors() )
        {
            bool bResultEnabled;
            if ( CStaticFunctionDefinitions::IsAmbientSoundEnabled( eType, bResultEnabled ) )
            {
                lua_pushboolean( L, bResultEnabled );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isAmbientSoundEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetAmbientSounds )
    {
        lua_pushboolean( L, CStaticFunctionDefinitions::ResetAmbientSounds () );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.GUI.cpp
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
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( guiGetInputEnabled )
    {
        lua_pushboolean ( L, CStaticFunctionDefinitions::GUIGetInputEnabled() );
        return 1;
    }

    LUA_DECLARE( guiSetInputEnabled )
    {
    //  bool guiSetInputEnabled ( bool enabled )
        bool enabled;

        CScriptArgReader argStream ( L );
        argStream.ReadBool ( enabled );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetInputMode ( enabled ? INPUTMODE_NO_BINDS : INPUTMODE_ALLOW_BINDS );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetInputEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetInputMode )
    {
    //  bool guiSetInputMode ( string mode )
        eInputMode mode;

        CScriptArgReader argStream ( L );
        argStream.ReadEnumString ( mode );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetInputMode ( mode );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetInputMode", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetInputMode )
    {
        eInputMode mode = CStaticFunctionDefinitions::GUIGetInputMode ();
        lua_pushstring ( L, EnumToString ( mode ) );
        return 1;
    }

    LUA_DECLARE( isChatBoxInputActive )
    {
        lua_pushboolean ( L, g_pCore->IsChatInputEnabled () );
        return 1;
    }

    LUA_DECLARE( isConsoleActive )
    {
        lua_pushboolean ( L, g_pCore->GetConsole ()->IsVisible () );
        return 1;
    }

    LUA_DECLARE( isDebugViewActive )
    {
        lua_pushboolean ( L, g_pCore->IsDebugVisible () );
        return 1;
    }

    LUA_DECLARE( isMainMenuActive )
    {
        lua_pushboolean ( L, g_pCore->IsMenuVisible () );
        return 1;
    }

    LUA_DECLARE( isMTAWindowActive )
    {
        bool bActive = ( g_pCore->IsChatInputEnabled () ||
            g_pCore->IsMenuVisible () ||
            g_pCore->GetConsole ()->IsVisible () ||
            g_pClientGame->GetTransferBox ()->IsVisible () );

        lua_pushboolean ( L, bActive );
        return 1;
    }

    LUA_DECLARE( isTransferBoxActive )
    {
        lua_pushboolean ( L, g_pClientGame->GetTransferBox ()->IsVisible () );
        return 1;
    }

    LUA_DECLARE( guiCreateWindow )
    {
    //  element guiCreateWindow ( float x, float y, float width, float height, string titleBarText, bool relative )
        float x; float y; float width; float height; SString titleBarText; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( titleBarText );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateWindow ( *pLuaMain, x, y, width, height, titleBarText, relative );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateWindow", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateLabel )
    {
    //  element guiCreateLabel ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateLabel ( *pLuaMain, x, y, width, height, text, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateLabel", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateStaticImage )
    {
    //  element guiCreateStaticImage ( float x, float y, float width, float height, string path, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString path; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( path );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource();
            filePath strPath;
            const char *meta;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pResource, path, meta, strPath ) )
            {
                CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateStaticImage ( *pLuaMain, x, y, width, height, strPath.c_str(), relative, parent );

                if ( pGUIElement )
                {
                    pGUIElement->PushStack( L );
                    return 1;
                }
            }
            else
            {
                std::string ansiPath = strPath.convert_ansi();

                m_pScriptDebugging->LogCustom( SString ( "Bad file path @ '%s' [%s]", "guiStaticImage", ansiPath.c_str() ) );
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiStaticImage", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateButton )
    {
    //  element guiCreateButton ( float x, float y, float width, float height, string text, bool relative, [ element parent = nil ] )
        float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateButton ( *pLuaMain, x, y, width, height, text, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateButton", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateProgressBar )
    {
    //  element guiCreateProgressBar ( float x, float y, float width, float height, bool relative, [element parent = nil] )
        float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateProgressBar ( *pLuaMain, x, y, width, height, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateProgressBar", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateCheckBox )
    {
    //  element guiCreateCheckBox ( float x, float y, float width, float height, string text, bool selected, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString text; bool selected; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( selected );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateCheckBox ( *pLuaMain, x, y, width, height, text, selected, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateCheckBox", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateRadioButton )
    {
    //  element guiCreateRadioButton ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateRadioButton ( *pLuaMain, x, y, width, height, text, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateRadioButton", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateEdit )
    {
    //  gui-edit guiCreateEdit ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateEdit ( *pLuaMain, x, y, width, height, text, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateEdit", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateMemo )
    {
    //  gui-memo guiCreateMemo ( float x, float y, float width, float height, string text, bool relative, [element parent = nil] )
        float x; float y; float width; float height; SString text; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( text );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateMemo ( *pLuaMain, x, y, width, height, text, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateMemo", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateGridList )
    {
    //  element guiCreateGridList ( float x, float y, float width, float height, bool relative, [ element parent = nil ] )
        float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateGridList ( *pLuaMain, x, y, width, height, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateGridList", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateScrollPane )
    {
    //  element guiCreateScrollPane( float x, float y, float width, float height, bool relative, [gui-element parent = nil])
        float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollPane ( *pLuaMain, x, y, width, height, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateScrollPane", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateScrollBar )
    {
    //  gui-scrollbar guiCreateScrollBar ( float x, float y, float width, float height, bool horizontal, bool relative, [gui-element parent = nil])
        float x; float y; float width; float height; bool horizontal; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( horizontal );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateScrollBar ( *pLuaMain, x, y, width, height, horizontal, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateScrollBar", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateTabPanel )
    {
    //  element guiCreateTabPanel ( float x, float y, float width, float height, bool relative, [element parent = nil ] )
        float x; float y; float width; float height; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTabPanel ( *pLuaMain, x, y, width, height, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateTabPanel", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiStaticImageLoadImage )
    {
    //  bool guiStaticImageLoadImage ( element theElement, string filename )
        CClientGUIElement* theElement; SString filename;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUISTATICIMAGE );
        argStream.ReadString( filename );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource =  pLuaMain->GetResource();
            filePath strPath;
            const char *meta;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pResource, filename, meta, strPath ) )
            {
                if ( CStaticFunctionDefinitions::GUIStaticImageLoadImage ( *theElement, strPath.c_str() ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
                else
                {
                    std::string ansiPath = strPath.convert_ansi();

                    m_pScriptDebugging->LogCustom( SString ( "Bad news @ '%s' [%s '%s']", "guiStaticImageLoadImage", "Problem loading image", ansiPath.c_str() ) );
                }
            }
            else
            {
                std::string ansiPath = strPath.c_str();

                m_pScriptDebugging->LogCustom( SString ( "Bad file path @ '%s' [%s]", "guiStaticImageLoadImage", ansiPath.c_str() ) );
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiStaticImageLoadImage", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateTab )
    {
    //  element guiCreateTab ( string text, element parent )
        SString text; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( text );
        argStream.ReadClass( parent, LUACLASS_GUITABPANEL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateTab ( *pLuaMain, text, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateTab", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetSelectedTab )
    {
    //  element guiGetSelectedTab ( element tabPanel )
        CClientGUIElement* tabPanel;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( tabPanel, LUACLASS_GUITABPANEL );

        if ( !argStream.HasErrors () )
        {
            CClientGUIElement *pTab = CStaticFunctionDefinitions::GUIGetSelectedTab ( *tabPanel );
            pTab->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetSelectedTab", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetSelectedTab )
    {
    //  bool guiSetSelectedTab ( element tabPanel, element theTab )
        CClientGUIElement* tabPanel; CClientGUIElement* theTab;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( tabPanel, LUACLASS_GUITABPANEL );
        argStream.ReadClass( theTab, LUACLASS_GUITAB );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::GUISetSelectedTab ( *tabPanel, *theTab ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetSelectedTab", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiDeleteTab )
    {
    //  bool guiDeleteTab ( element tabToDelete, element tabPanel )
        CClientGUIElement* tabToDelete; CClientGUIElement* tabPanel;

        CScriptArgReader argStream( L );
        argStream.ReadClass( tabToDelete, LUACLASS_GUITAB );
        argStream.ReadClass( tabPanel, LUACLASS_GUITABPANEL );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::GUIDeleteTab( *lua_readcontext( L ), tabToDelete, tabPanel ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "guiDeleteTab", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetText )
    {
    //  bool guiSetText ( element guiElement, string text )
        CClientGUIElement* guiElement; SString text;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadString ( text );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetText ( *guiElement, text );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetText", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetFont )
    {
    //  bool guiSetFont ( element guiElement, mixed font )
        CClientGUIElement* guiElement; SString strFontName; CClientGuiFont* pGuiFontElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        MixedReadGuiFontString ( argStream, strFontName, "default-normal", pGuiFontElement );

        if ( !argStream.HasErrors () )
        {
            if ( guiElement->SetFont ( strFontName, pGuiFontElement ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetFont", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiBringToFront )
    {
    //  bool guiBringToFront ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::GUIBringToFront ( *guiElement ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiBringToFront", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiMoveToBack )
    {
    //  bool guiMoveToBack( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIMoveToBack ( *guiElement );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiMoveToBack", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiRadioButtonSetSelected )
    {
    //  bool guiRadioButtonSetSelected ( element guiRadioButton, bool state )
        CClientGUIElement* guiRadioButton; bool state;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiRadioButton, LUACLASS_GUIELEMENT );
        argStream.ReadBool ( state );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIRadioButtonSetSelected ( *guiRadioButton, state );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiRadioButtonSetSelected", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCheckBoxSetSelected )
    {
    //  bool guiCheckBoxSetSelected ( element theCheckbox, bool state )
        CClientGUIElement* theCheckbox; bool state;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theCheckbox, LUACLASS_GUICHECKBOX );
        argStream.ReadBool ( state );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUICheckBoxSetSelected ( *theCheckbox, state );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCheckBoxSetSelected", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiRadioButtonGetSelected )
    {
    //  bool guiRadioButtonGetSelected( element guiRadioButton )
        CClientGUIElement* guiRadioButton;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiRadioButton, LUACLASS_GUIRADIOBUTTON );

        if ( !argStream.HasErrors () )
        {
            bool bResult = static_cast < CGUIRadioButton* > ( guiRadioButton->GetCGUIElement () ) -> GetSelected ();
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiRadioButtonGetSelected", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCheckBoxGetSelected )
    {
    //  bool guiCheckBoxGetSelected ( element theCheckbox )
        CClientGUIElement* theCheckbox;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theCheckbox, LUACLASS_GUICHECKBOX );

        if ( !argStream.HasErrors () )
        {
            bool bResult = static_cast < CGUICheckBox* > ( theCheckbox->GetCGUIElement () ) -> GetSelected ();
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCheckBoxGetSelected", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiProgressBarSetProgress )
    {
    //  bool guiProgressBarSetProgress ( progressBar theProgressbar, float progress )
        CClientGUIElement* theProgressbar; float progress;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theProgressbar, LUACLASS_GUIPROGRESSBAR );
        argStream.ReadNumber ( progress );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIProgressBarSetProgress ( *theProgressbar, static_cast < int > ( progress ) );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiProgressBarSetProgress", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollBarSetScrollPosition )
    {
    //  bool guiScrollBarSetScrollPosition ( gui-scrollBar theScrollBar, float amount )
        CClientGUIElement* theScrollBar; float amount;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollBar, LUACLASS_GUISCROLLBAR );
        argStream.ReadNumber ( amount );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIScrollBarSetScrollPosition ( *theScrollBar, static_cast < int > ( amount ) );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollBarSetScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollPaneSetHorizontalScrollPosition )
    {
    //  bool guiScrollPaneSetHorizontalScrollPosition ( gui-scrollPane theScrollPane, float amount )
        CClientGUIElement* theScrollPane; float amount;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollPane, LUACLASS_GUISCROLLBAR );
        argStream.ReadNumber ( amount );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetHorizontalScrollPosition ( *theScrollPane, static_cast < int > ( amount ) );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollPaneSetHorizontalScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollPaneSetVerticalScrollPosition )
    {
    //  bool guiScrollPaneSetVerticalScrollPosition ( gui-scrollPane theScrollPane, float amount )
        CClientGUIElement* theScrollPane; float amount;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollPane, LUACLASS_GUISCROLLPANE );
        argStream.ReadNumber ( amount );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetVerticalScrollPosition ( *theScrollPane, static_cast < int > ( amount ) );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollPaneSetVerticalScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiProgressBarGetProgress )
    {
    //  float guiProgressBarGetProgress ( progressBar theProgressbar );
        CClientGUIElement* theProgressbar;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theProgressbar, LUACLASS_GUIPROGRESSBAR );

        if ( !argStream.HasErrors () )
        {
            int iProgress = ( int ) ( static_cast < CGUIProgressBar* > ( theProgressbar->GetCGUIElement () ) -> GetProgress () * 100.0f + 0.5f );
            lua_pushnumber ( L, iProgress );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiProgressBarGetProgress", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollPaneGetHorizontalScrollPosition )
    {
    //  float guiScrollPaneGetHorizontalScrollPosition ( gui-scrollPane theScrollPane  )
        CClientGUIElement* theScrollPane;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollPane, LUACLASS_GUISCROLLPANE );

        if ( !argStream.HasErrors () )
        {
            float fPos = static_cast < CGUIScrollPane* > ( theScrollPane->GetCGUIElement () ) -> GetHorizontalScrollPosition () * 100.0f;
            lua_pushnumber ( L, fPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollPaneGetHorizontalScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollPaneGetVerticalScrollPosition )
    {
    //  float guiScrollPaneGetVerticalScrollPosition ( gui-scrollPane theScrollPane  )
        CClientGUIElement* theScrollPane;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollPane, LUACLASS_GUISCROLLPANE );

        if ( !argStream.HasErrors () )
        {
            float fPos = static_cast < CGUIScrollPane* > ( theScrollPane->GetCGUIElement () ) -> GetVerticalScrollPosition () * 100.0f;
            lua_pushnumber ( L, fPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollPaneGetVerticalScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollBarGetScrollPosition )
    {
    //  float guiScrollBarGetScrollPosition ( gui-scrollBar theScrollBar )
        CClientGUIElement* theScrollBar;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theScrollBar, LUACLASS_GUISCROLLBAR );

        if ( !argStream.HasErrors () )
        {
            int iPos = ( int ) ( static_cast < CGUIScrollBar* > ( theScrollBar->GetCGUIElement () ) -> GetScrollPosition () * 100.0f );
            lua_pushnumber ( L, iPos );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollBarGetScrollPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetText )
    {
    //  string guiGetText ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            SString strText = guiElement->GetCGUIElement ()->GetText ();
            lua_pushstring ( L, strText );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetText", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetFont )
    {
    //  string,font guiGetFont ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            CClientGuiFont* pGuiFontElement;
            SString strFontName = guiElement->GetFont ( &pGuiFontElement );

            if ( strFontName != "" )
                lua_pushstring ( L, strFontName );
            else
                lua_pushnil ( L );

            if ( pGuiFontElement )
            {
                pGuiFontElement->PushStack( L );
                return 2;
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetFont", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetSize )
    {
    //  float float guiGetSize ( element theElement, bool relative )
        CClientGUIElement* theElement; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIELEMENT );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CVector2D Size;
            theElement->GetCGUIElement ()->GetSize ( Size, relative );

            lua_pushnumber ( L, Size.fX );
            lua_pushnumber ( L, Size.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetSize", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetScreenSize )
    {
        const CVector2D Size = CStaticFunctionDefinitions::GUIGetScreenSize ();

        lua_pushnumber ( L, Size.fX );
        lua_pushnumber ( L, Size.fY );
        return 2;
    }

    LUA_DECLARE( guiGetPosition )
    {
    //  float, float guiGetPosition ( element guiElement, bool relative )
        CClientGUIElement* guiElement; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CVector2D Pos;
            guiElement->GetCGUIElement ()->GetPosition ( Pos, relative );

            lua_pushnumber ( L, Pos.fX );
            lua_pushnumber ( L, Pos.fY );
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetAlpha )
    {
    //  bool guiSetAlpha ( element guielement, float alpha )
        CClientGUIElement* guiElement; float alpha;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadNumber ( alpha );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetAlpha ( *guiElement, Clamp ( 0.0f, alpha, 1.0f ) );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetAlpha", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetAlpha )
    {
    //  int guiGetAlpha ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            float fAlpha = guiElement->GetCGUIElement ()->GetAlpha ();
            lua_pushnumber ( L, fAlpha );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetAlpha", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetVisible )
    {
    //  bool guiSetVisible ( element guiElement, bool state )
        CClientGUIElement* guiElement; bool state;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadBool ( state );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetVisible ( *guiElement, state );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetVisible", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetEnabled )
    {
    //  bool guiSetEnabled ( element guiElement, bool enabled )
        CClientGUIElement* guiElement; bool enabled;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadBool ( enabled );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetEnabled ( *guiElement, enabled );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetVisible", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetProperty )
    {
    //  bool guiSetProperty ( element guiElement, string property, string value )
        CClientGUIElement* guiElement; SString property; SString value;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadString ( property );
        argStream.ReadString ( value );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetProperty ( *guiElement, property, value );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetProperty", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetVisible )
    {
    //  bool guiGetVisible ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            bool bResult = guiElement->GetCGUIElement ()->IsVisible ();
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetVisible", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetEnabled )
    {
    //  bool guiGetEnabled ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            bool bResult = guiElement->GetCGUIElement ()->IsEnabled ();
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetProperty )
    {
    //  string guiGetProperty ( element guiElement, string property )
        CClientGUIElement* guiElement; SString property;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadString ( property );

        if ( !argStream.HasErrors () )
        {
            SString strValue = guiElement->GetCGUIElement ()->GetProperty ( property );
            lua_pushstring ( L, strValue );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetProperty", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetProperties )
    {
    //  table guiGetProperties ( element guiElement )
        CClientGUIElement* guiElement;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );

        if ( !argStream.HasErrors () )
        {
            // Create a new table
            lua_newtable ( L );

            // Add all our properties to the table on top of the given lua main's stack
            unsigned int uiIndex = 0;
            CGUIPropertyIter iter = guiElement->GetCGUIElement ()->GetPropertiesBegin ();
            CGUIPropertyIter iterEnd = guiElement->GetCGUIElement ()->GetPropertiesEnd ();
            for ( ; iter != iterEnd; ++iter )
            {
                char * szKey = (*iter)->szKey;
                char * szValue = (*iter)->szValue;

                // Add it to the table
                lua_pushstring ( L, szKey );
                lua_pushstring ( L, szValue );
                lua_settable ( L, -3 );
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGetProperties", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetSize )
    {
    //  bool guiSetSize ( element guiElement, float width, float height, bool relative )
        CClientGUIElement* guiElement; float width; float height; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetSize ( *guiElement, CVector2D ( width, height ), relative );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetSize", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiSetPosition )
    {
    //  bool guiSetPosition ( element guiElement, float x, float y, bool relative )
        CClientGUIElement* guiElement; float x; float y; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiElement, LUACLASS_GUIELEMENT );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUISetPosition ( *guiElement, CVector2D ( x, y ), relative );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiSetPosition", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetSortingEnabled )
    {
    //  bool guiGridListSetSortingEnabled ( element guiGridlist, bool enabled )
        CClientGUIElement* guiGridlist; bool enabled;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadBool ( enabled );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetSortingEnabled ( *guiGridlist, enabled );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetSortingEnabled", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListAddColumn )
    {
    //  int guiGridListAddColumn ( element gridList, string title, float width )
        CClientGUIElement* guiGridlist; SString title; float width;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadString ( title );
        argStream.ReadNumber ( width );

        if ( !argStream.HasErrors () )
        {
            uint id = CStaticFunctionDefinitions::GUIGridListAddColumn ( *guiGridlist, title, width );
            lua_pushnumber ( L, id );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListAddColumn", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListRemoveColumn )
    {
    //  bool guiGridListRemoveColumn ( element guiGridlist, int columnIndex )
        CClientGUIElement* guiGridlist; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListRemoveColumn ( *guiGridlist, columnIndex );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListRemoveColumn", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetColumnWidth )
    {
    //  bool guiGridListSetColumnWidth ( element gridList, int columnIndex, number width, bool relative )
        CClientGUIElement* guiGridlist; int columnIndex; float width; bool relative;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( columnIndex );
        argStream.ReadNumber ( width );
        argStream.ReadBool ( relative );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetColumnWidth ( *guiGridlist, columnIndex, width, relative );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "GUIGridListSetColumnWidth", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListAddRow )
    {
    //  int guiGridListAddRow ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            int iRet = CStaticFunctionDefinitions::GUIGridListAddRow ( *guiGridlist, true );
            if ( iRet >= 0 )
            {
                m_pGUIManager->QueueGridListUpdate ( guiGridlist );
                lua_pushnumber ( L, iRet );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListAddRow", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListInsertRowAfter )
    {
    //  int guiGridListInsertRowAfter ( element gridList, int rowIndex )
        CClientGUIElement* guiGridlist; int rowIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );

        if ( !argStream.HasErrors () )
        {
            int iRet = CStaticFunctionDefinitions::GUIGridListInsertRowAfter ( *guiGridlist, rowIndex );
            if ( iRet >= 0 ) 
            {
                lua_pushnumber ( L, iRet );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListInsertRowAfter", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListAutoSizeColumn )
    {
    //  bool guiGridListAutoSizeColumn ( element gridList, int columnIndex )
        CClientGUIElement* guiGridlist; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListAutoSizeColumn ( *guiGridlist, columnIndex );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListAutoSizeColumn", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListClear )
    {
    //  bool guiGridListClear ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListClear ( *guiGridlist );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListClear", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetSelectionMode )
    {
    //  bool guiGridListSetSelectionMode ( guiElement gridlist, int mode )
        CClientGUIElement* guiGridlist; int mode;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( mode );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetSelectionMode ( *guiGridlist, mode );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetSelectionMode", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetSelectedItem )
    {
    //  int, int guiGridListGetSelectedItem ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            int iRow = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedItemRow ();
            int iColumn = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedItemColumn ();
            lua_pushnumber ( L, iRow );
            lua_pushnumber ( L, iColumn + 1 );  // columns start at 1
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetSelectedItem", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetSelectedItems )
    {
    //  table guiGridListGetSelectedItems ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            CGUIGridList* pList = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () );
            CGUIListItem* pItem = NULL;

            lua_newtable ( L );

            for ( int i = 1; i <= pList->GetSelectedCount(); i++ )
            {
                pItem = pList->GetNextSelectedItem ( pItem );
                if ( !pItem ) break;

                lua_pushnumber ( L, i );
                lua_newtable ( L );

                // column
                lua_pushstring ( L, "column" );
                lua_pushnumber ( L, pList->GetItemColumnIndex ( pItem ) );
                lua_settable ( L, -3 );

                // row
                lua_pushstring ( L, "row" );
                lua_pushnumber ( L, pList->GetItemRowIndex ( pItem ) );
                lua_settable ( L, -3 );

                // push to main table
                lua_settable ( L, -3 );
            }

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetSelectedItems", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetSelectedCount )
    {
    //  int guiGridListGetSelectedCount ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            int iCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetSelectedCount ();
            lua_pushnumber ( L, iCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetSelectedCount", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetSelectedItem )
    {
    //  bool guiGridListSetSelectedItem ( element gridList, int rowIndex, int columnIndex )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetSelectedItem ( *guiGridlist, rowIndex, columnIndex, true );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetSelectedItem", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListRemoveRow )
    {
    //  bool guiGridListRemoveRow ( element gridList, int rowIndex )
        CClientGUIElement* guiGridlist; int rowIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListRemoveRow ( *guiGridlist, rowIndex );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListRemoveRow", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetItemText )
    {
    //  string guiGridListGetItemText ( element gridList, int rowIndex, int columnIndex )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors () )
        {
            const char* szText = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetItemText ( rowIndex, columnIndex );
            if ( szText )
                lua_pushstring ( L, szText );
            else
                lua_pushnil ( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetItemText", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetItemData )
    {
    //  string guiGridListGetItemData ( element gridList, int rowIndex, int columnIndex )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors() )
        {
            LuaTypeExport *exp = (LuaTypeExport*)((CGUIGridList*)guiGridlist->GetCGUIElement())->GetItemData( rowIndex, columnIndex );

            if ( exp )
                exp->Push( L );
            else
                lua_pushnil( L );

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetItemData", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetItemColor )
    {
    //  int int int int guiGridListGetItemColor ( element gridList, int rowIndex, int columnIndex )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );

        if ( !argStream.HasErrors () )
        {
            unsigned char ucRed = 255, ucGreen = 255, ucBlue = 255, ucAlpha = 255;
            if ( static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetItemColor ( rowIndex, columnIndex, ucRed, ucGreen, ucBlue, ucAlpha ) )
            {
                lua_pushnumber ( L, ucRed );
                lua_pushnumber ( L, ucGreen );
                lua_pushnumber ( L, ucBlue );
                lua_pushnumber ( L, ucAlpha );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetItemColor", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetItemText )
    {
    //  bool guiGridListSetItemText ( element gridList, int rowIndex, int columnIndex, string text, bool section, bool number )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; SString text; bool section; bool number;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );
        argStream.ReadString ( text );
        argStream.ReadBool ( section );
        argStream.ReadBool ( number );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetItemText ( *guiGridlist, rowIndex, columnIndex, text, section, number, true );
            m_pGUIManager->QueueGridListUpdate ( guiGridlist );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetItemText", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetItemData )
    {
    //  bool guiGridListSetItemData ( element gridList, int rowIndex, int columnIndex, string data )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber( rowIndex );
        argStream.ReadNumber( columnIndex );

        if ( !argStream.HasErrors() )
        {
            LuaTypeExport *exp;

            if ( Lua_ReadExportType( L, 4, exp ) )
            {
                CStaticFunctionDefinitions::GUIGridListSetItemData( *guiGridlist, rowIndex, columnIndex, exp );
                lua_pushboolean( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetItemData", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetItemColor )
    {
    //  bool guiGridListSetItemColor ( element gridList, int rowIndex, int columnIndex, int red, int green, int blue[, int alpha = 255 ] )
        CClientGUIElement* guiGridlist; int rowIndex; int columnIndex; int red; int green; int blue; int alpha;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadNumber ( rowIndex );
        argStream.ReadNumber ( columnIndex );
        argStream.ReadNumber ( red );
        argStream.ReadNumber ( green );
        argStream.ReadNumber ( blue );
        argStream.ReadNumber ( alpha, 255 );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetItemColor( *guiGridlist, rowIndex, columnIndex, red, green, blue, alpha );
            m_pGUIManager->QueueGridListUpdate ( guiGridlist );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetItemColor", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListSetScrollBars )
    {
    //  bool guiGridListSetScrollBars ( element guiGridlist, bool horizontalBar, bool verticalBar )
        CClientGUIElement* guiGridlist; bool horizontalBar; bool verticalBar;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );
        argStream.ReadBool ( horizontalBar );
        argStream.ReadBool ( verticalBar );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIGridListSetScrollBars ( *guiGridlist, horizontalBar, verticalBar );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListSetScrollBars", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiScrollPaneSetScrollBars )
    {
    //  bool guiScrollPaneSetScrollBars ( element scrollPane, bool horizontal, bool vertical )
        CClientGUIElement* scrollPane; bool horizontalBar; bool verticalBar;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( scrollPane, LUACLASS_GUISCROLLPANE );
        argStream.ReadBool ( horizontalBar );
        argStream.ReadBool ( verticalBar );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIScrollPaneSetScrollBars ( *scrollPane, horizontalBar, verticalBar );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiScrollPaneSetScrollBars", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetRowCount )
    {
    //  int guiGridListGetRowCount ( element theList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            int iRowCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetRowCount ();
            lua_pushnumber ( L, iRowCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetRowCount", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGridListGetColumnCount )
    {
    //  int guiGridListGetColumnCount ( element gridList )
        CClientGUIElement* guiGridlist;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( guiGridlist, LUACLASS_GUIGRIDLIST );

        if ( !argStream.HasErrors () )
        {
            int iColumnCount = static_cast < CGUIGridList* > ( guiGridlist->GetCGUIElement () ) -> GetColumnCount ();
            lua_pushnumber ( L, iColumnCount );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiGridListGetColumnCount", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiEditSetReadOnly )
    {
    //  bool guiEditSetReadOnly ( element editField, bool status )
        CClientGUIElement* editField; bool status;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( editField, LUACLASS_GUIEDIT );
        argStream.ReadBool ( status );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIEditSetReadOnly ( *editField, status );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiEditSetReadOnly", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiMemoSetReadOnly )
    {
    //  bool guiMemoSetReadOnly ( gui-memo theMemo, bool status )
        CClientGUIElement* theMemo; bool status;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theMemo, LUACLASS_GUIMEMO );
        argStream.ReadBool ( status );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIMemoSetReadOnly ( *theMemo, status );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiMemoSetReadOnly", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiEditSetMasked )
    {
    //  bool guiEditSetMasked ( element theElement, bool status )
        CClientGUIElement* theElement; bool status;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIEDIT );
        argStream.ReadBool ( status );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIEditSetMasked ( *theElement, status );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiEditSetMasked", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiEditSetMaxLength )
    {
    //  bool guiEditSetMaxLength ( element theElement, int length )
        CClientGUIElement* theElement; int length;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIEDIT );
        argStream.ReadNumber ( length );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIEditSetMaxLength ( *theElement, length );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiEditSetMaxLength", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiEditSetCaretIndex )
    {
    //  bool guiEditSetCaratIndex ( element theElement, int index )
        CClientGUIElement* theElement; int index;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIEDIT );
        argStream.ReadNumber ( index );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIEditSetCaratIndex ( *theElement, index );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiEditSetCaratIndex", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiMemoSetCaratIndex )
    {
    //  bool guiMemoSetCaratIndex ( gui-memo theMemo, int index )
        CClientGUIElement* theMemo; int index;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theMemo, LUACLASS_GUIMEMO );
        argStream.ReadNumber ( index );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIMemoSetCaratIndex ( *theMemo, index );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiMemoSetCaratIndex", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiWindowSetMovable )
    {
    //  bool guiWindowSetMovable ( element theElement, bool status )
        CClientGUIElement* theElement; bool status;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIWINDOW );
        argStream.ReadBool ( status );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIWindowSetMovable ( *theElement, status );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiWindowSetMovable", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiWindowSetSizable )
    {
    //  bool guiWindowSetSizable ( element theElement, bool status )
        CClientGUIElement* theElement; bool status;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUIWINDOW );
        argStream.ReadBool ( status );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUIWindowSetSizable ( *theElement, status );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiWindowSetSizable", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiLabelGetTextExtent )
    {
    //  float guiLabelGetTextExtent ( element theLabel )
        CClientGUIElement* theLabel;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theLabel, LUACLASS_GUILABEL );

        if ( !argStream.HasErrors () )
        {
            float fExtent = static_cast < CGUILabel* > ( theLabel->GetCGUIElement () ) -> GetTextExtent ();
            lua_pushnumber ( L, fExtent );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiLabelGetTextExtent", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiLabelGetFontHeight )
    {
    //  float guiLabelGetFontHeight ( element theLabel )
        CClientGUIElement* theLabel;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theLabel, LUACLASS_GUILABEL );

        if ( !argStream.HasErrors () )
        {
            float fHeight = static_cast < CGUILabel* > ( theLabel->GetCGUIElement () ) -> GetFontHeight ();
            lua_pushnumber ( L, fHeight );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiLabelGetFontHeight", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiLabelSetColor )
    {
    //  bool guiLabelSetColor ( element theElement, int red, int green, int blue )
        CClientGUIElement* theElement; int red; int green; int blue;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theElement, LUACLASS_GUILABEL );
        argStream.ReadNumber ( red );
        argStream.ReadNumber ( green );
        argStream.ReadNumber ( blue );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUILabelSetColor ( *theElement, red, green, blue );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiLabelSetColor", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiLabelSetVerticalAlign )
    {
    //  bool guiLabelSetVerticalAlign ( element theLabel, string align )
        CClientGUIElement* theLabel; CGUIVerticalAlign align;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theLabel, LUACLASS_GUILABEL );
        argStream.ReadEnumString ( align );

        if ( !argStream.HasErrors () )
        {
            CStaticFunctionDefinitions::GUILabelSetVerticalAlign ( *theLabel, align );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiLabelSetVerticalAlign", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiLabelSetHorizontalAlign )
    {
    //  bool guiLabelSetHorizontalAlign ( element theLabel, string align, [ bool wordwrap = false ] )
        CClientGUIElement* theLabel; CGUIHorizontalAlign align; bool wordwrap;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( theLabel, LUACLASS_GUILABEL );
        argStream.ReadEnumString ( align );
        argStream.ReadBool ( wordwrap, false );

        if ( !argStream.HasErrors () )
        {
            if ( wordwrap )
                align = (CGUIHorizontalAlign)(align + 4);
            CStaticFunctionDefinitions::GUILabelSetHorizontalAlign ( *theLabel, align );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiLabelSetHorizontalAlign", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiGetChatboxLayout )
    {
        //* chat_font - Returns the chatbox font
        //* chat_lines - Returns how many lines the chatbox has
        //* chat_color - Returns the background color of the chatbox
        //* chat_text_color - Returns the chatbox text color
        //* chat_input_color - Returns the background color of the chatbox input
        //* chat_input_prefix_color - Returns the color of the input prefix text
        //* chat_input_text_color - Returns the color of the text in the chatbox input
        //* chat_scale - Returns the scale of the text in the chatbox
        //* chat_width - Returns the scale of the background width
        //* chat_css_style_text - Returns whether text fades out over time
        //* chat_css_style_background - Returns whether the background fades out over time
        //* chat_line_life - Returns how long it takes for text to start fading out
        //* chat_line_fade_out - Returns how long takes for text to fade out
        //* chat_use_cegui - Returns whether CEGUI is used to render the chatbox
        //* text_scale - Returns text scale

        CCVarsInterface* pCVars = g_pCore->GetCVars ();
        float fNumber;
        pCVars->Get("chat_font", fNumber);
        lua_newtable ( L );
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_font" );
        pCVars->Get("chat_lines", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_lines" );
        pCVars->Get("chat_width", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_width" );
        pCVars->Get("chat_css_style_text", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_css_style_text" );
        pCVars->Get("chat_css_style_background", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_css_style_background" );
        pCVars->Get("chat_line_life", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_line_life" );
        pCVars->Get("chat_line_fade_out", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "chat_line_fade_out" );
        pCVars->Get("text_scale", fNumber);
        lua_pushnumber ( L, fNumber );
        lua_setfield ( L, -2, "text_scale" );
        pCVars->Get("chat_use_cegui", fNumber);
        lua_pushboolean ( L, fNumber ? true : false );
        lua_setfield ( L, -2, "chat_use_cegui" );
        std::string strCVar = pCVars->Get("chat_color");
        std::stringstream ss;
        int iR, iG, iB, iA;
        if ( !strCVar.empty() )
        {
            ss.str ( strCVar );
            ss >> iR >> iG >> iB >> iA;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, iR );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, iG );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, iB );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 4 );
            lua_pushnumber ( L, iA );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_color" );
        }
        strCVar = pCVars->Get("chat_text_color");
        if ( !strCVar.empty() )
        {
            ss.clear();
            ss.str ( strCVar );
            ss >> iR >> iG >> iB >> iA;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, iR );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, iG );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, iB );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 4 );
            lua_pushnumber ( L, iA );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_text_color" );
        }
        strCVar = pCVars->Get("chat_input_color");
        if ( !strCVar.empty() )
        {
            ss.clear();
            ss.str ( strCVar );
            ss >> iR >> iG >> iB >> iA;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, iR );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, iG );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, iB );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 4 );
            lua_pushnumber ( L, iA );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_input_color" );
        }
        strCVar = pCVars->Get("chat_input_prefix_color");
        if ( !strCVar.empty() )
        {
            ss.clear();
            ss.str ( strCVar );
            ss >> iR >> iG >> iB >> iA;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, iR );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, iG );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, iB );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 4 );
            lua_pushnumber ( L, iA );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_input_prefix_color" );
        }
        strCVar = pCVars->Get("chat_input_text_color");
        if ( !strCVar.empty() )
        {
            ss.clear();
            ss.str ( strCVar );
            ss >> iR >> iG >> iB >> iA;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, iR );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, iG );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 3 );
            lua_pushnumber ( L, iB );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 4 );
            lua_pushnumber ( L, iA );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_input_text_color" );
        }
        strCVar = pCVars->Get("chat_scale");
        if ( !strCVar.empty() )
        {
            float fX, fY;
            ss.clear();
            ss.str ( strCVar );
            ss >> fX >> fY;
            lua_newtable ( L );
            lua_pushnumber ( L, 1 );
            lua_pushnumber ( L, fX );
            lua_settable( L, -3 );
            lua_pushnumber ( L, 2 );
            lua_pushnumber ( L, fY );
            lua_settable( L, -3 );
            lua_setfield ( L, -2, "chat_scale" );
        }
        return 1;
    }

    LUA_DECLARE( guiCreateComboBox )
    {
    //  element guiCreateComboBox ( float x, float y, float width, float height, string caption, bool relative, [ element parent = nil ] )
        float x; float y; float width; float height; SString caption; bool relative; CClientGUIElement* parent;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( x );
        argStream.ReadNumber ( y );
        argStream.ReadNumber ( width );
        argStream.ReadNumber ( height );
        argStream.ReadString ( caption );
        argStream.ReadBool ( relative );
        argStream.ReadClass( parent, LUACLASS_GUIELEMENT, NULL );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CClientGUIElement* pGUIElement = CStaticFunctionDefinitions::GUICreateComboBox ( *pLuaMain, x, y, width, height, caption, relative, parent );
            pGUIElement->PushStack( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateComboBox", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxAddItem )
    {
        // int guiComboBoxAddItem( element comboBox, string value )
        CClientGUIElement* comboBox; SString value;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );
        argStream.ReadString ( value );

        if ( !argStream.HasErrors () )
        {
            int newId = CStaticFunctionDefinitions::GUIComboBoxAddItem ( *comboBox, value );
            lua_pushnumber( L, newId );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxAddItem", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxRemoveItem )
    {
    //  bool guiComboBoxRemoveItem( element comboBox, int itemId )
        CClientGUIElement* comboBox; int itemId;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );
        argStream.ReadNumber ( itemId );

        if ( !argStream.HasErrors () )
        {
            bool ret = CStaticFunctionDefinitions::GUIComboBoxRemoveItem ( *comboBox, itemId );
            lua_pushboolean( L, ret );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxRemoveItem", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxClear )
    {
    //  bool guiComboBoxClear ( element comboBox )
        CClientGUIElement* comboBox;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );

        if ( !argStream.HasErrors () )
        {
            bool ret = CStaticFunctionDefinitions::GUIComboBoxClear ( *comboBox );
            lua_pushboolean( L, ret );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxClear", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxGetSelected )
    {
    //  int guiComboBoxGetSelected ( element comboBox )
        CClientGUIElement* comboBox;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );

        if ( !argStream.HasErrors () )
        {
            int selected = CStaticFunctionDefinitions::GUIComboBoxGetSelected( *comboBox );
            lua_pushnumber ( L, selected );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxGetSelected", *argStream.GetErrorMessage () ) );

        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( guiComboBoxSetSelected )
    {
    //  bool guiComboBoxSetSelected ( element comboBox, int itemIndex )
        CClientGUIElement* comboBox; int itemIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );
        argStream.ReadNumber ( itemIndex );

        if ( !argStream.HasErrors () )
        {
            bool ret = CStaticFunctionDefinitions::GUIComboBoxSetSelected( *comboBox, itemIndex );
            lua_pushboolean ( L, ret );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxSetSelected", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxGetItemText )
    {
    //  string guiComboBoxGetItemText ( element comboBox, int itemId )
        CClientGUIElement* comboBox; int itemId;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );
        argStream.ReadNumber ( itemId );

        if ( !argStream.HasErrors () )
        {
            SString ret = CStaticFunctionDefinitions::GUIComboBoxGetItemText( *comboBox, itemId );
            lua_pushstring ( L, ret );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxGetItemText", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiComboBoxSetItemText )
    {
    //  bool guiComboBoxSetItemText ( element comboBox, int itemId, string text )
        CClientGUIElement* comboBox; int itemId; SString text;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( comboBox, LUACLASS_GUICOMBOBOX );
        argStream.ReadNumber ( itemId );
        argStream.ReadString ( text );

        if ( !argStream.HasErrors () )
        {
            bool ret = CStaticFunctionDefinitions::GUIComboBoxSetItemText( *comboBox, itemId, text );
            lua_pushboolean ( L, ret );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiComboBoxSetItemText", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( guiCreateFont )
    {
    //  element guiCreateFont( string filepath [, int size=9 ] )
        SString strFilePath; int iSize;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strFilePath );
        argStream.ReadNumber ( iSize, 9 );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            const char *meta;
            filePath strPath;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pParentResource, strFilePath, meta, strPath ) )
            {
                if ( pParentResource->FileExists( meta ) )
                {
                    SString strUniqueName = SString ( "%s*%s*%s", pParentResource->GetName (), pFileResource->GetName (), meta ).Replace ( "\\", "/" );
                    CClientGuiFont* pGuiFont = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateGuiFont ( strPath.c_str(), strUniqueName, iSize, *pParentResource->GetResourceDynamicEntity() );
                    if ( pGuiFont )
                    {
                        // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                    }
                    pGuiFont->PushStack( L );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer( "guiCreateFont", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer( "guiCreateFont", "file-path", 1 );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "guiCreateFont", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Drawing.cpp
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
    LUA_DECLARE( dxDrawLine )
    {
        float x, y, toX, toY;
        unsigned long color;
        float width;
        bool postGUI;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( x );
        argStream.ReadNumber( y );
        argStream.ReadNumber( toX );
        argStream.ReadNumber( toY );
        argStream.ReadNumber( color );
        argStream.ReadNumber( width, 1.0f );
        argStream.ReadBool( postGUI, false );

        // Check required arguments. Should all be numbers.
        if ( argStream.HasErrors() )
        {
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '%s' [%s]", "dxDrawLine", *argStream.GetErrorMessage() ) );
            lua_pushboolean( L, false );
            return 1;
        }

        // Draw it
        g_pCore->GetGraphics()->DrawLineQueued( x, y, toX, toY, width, color, postGUI );

        // Success
        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( dxDrawLine3D )
    {
        // dxDrawLine3D ( float x,float y,float z,float x2,float y2,float z2,int color, [float width,bool postGUI,float zBuffer] )

        CVector begin, end;
        unsigned long color;
        float width;
        bool postGUI;

        CScriptArgReader argStream( L );

        argStream.ReadVector( begin );
        argStream.ReadVector( end );
        argStream.ReadNumber( color );
        argStream.ReadNumber( width, 1 );
        argStream.ReadBool( postGUI, false );

        // Check required arguments. Should all be numbers.
        if ( argStream.HasErrors() )
        {
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );
            lua_pushboolean ( L, false );
            return 1;
        }

        // Draw it
        g_pCore->GetGraphics()->DrawLine3DQueued( begin, end, width, color, postGUI );

        lua_pushboolean( L, true );
        return 1;
    }

    LUA_DECLARE( dxDrawText )
    {
    //  bool dxDrawText ( string text, int left, int top [, int right=left, int bottom=top, int color=white, float scale=1, mixed font="default", 
    //      string alignX="left", string alignY="top", bool clip=false, bool wordBreak=false, bool postGUI] )
        SString strText; int iLeft; int iTop; int iRight; int iBottom; ulong ulColor; float fScaleX; float fScaleY; SString strFontName; CClientDxFont* pDxFontElement;
        eDXHorizontalAlign alignX; eDXVerticalAlign alignY; bool bClip; bool bWordBreak; bool bPostGUI;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strText );
        argStream.ReadNumber ( iLeft );
        argStream.ReadNumber ( iTop );
        argStream.ReadNumber ( iRight, iLeft );
        argStream.ReadNumber ( iBottom, iTop );
        argStream.ReadNumber ( ulColor, 0xFFFFFFFF );
        argStream.ReadNumber ( fScaleX, 1 );
        if ( argStream.NextIsNumber () )
            argStream.ReadNumber ( fScaleY );
        else
            fScaleY = fScaleX;
        MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );
        argStream.ReadEnumString ( alignX, DX_ALIGN_LEFT );
        argStream.ReadEnumString ( alignY, DX_ALIGN_TOP );
        argStream.ReadBool ( bClip, false );
        argStream.ReadBool ( bWordBreak, false );
        argStream.ReadBool ( bPostGUI, false );

        if ( !argStream.HasErrors () )
        {
            // Get DX font
            ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

            // Make format flag
            ulong ulFormat = alignX | alignY;
            //if ( ulFormat & DT_BOTTOM ) ulFormat |= DT_SINGLELINE;        MS says we should do this. Nobody tells me what to do.
            if ( bWordBreak )           ulFormat |= DT_WORDBREAK;
            if ( !bClip )               ulFormat |= DT_NOCLIP;

            CStaticFunctionDefinitions::DrawText ( iLeft, iTop, iRight, iBottom, ulColor, strText, fScaleX, fScaleY, ulFormat, pD3DXFont, bPostGUI );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxDrawRectangle )
    {
        // dxDrawRectangle ( int x,int y,float width,float height,[int color=0xffffffff] )

        float x, y;
        float width, height;
        unsigned long color;
        bool postGUI;

        CScriptArgReader argStream( L );
        
        argStream.ReadNumber( x );
        argStream.ReadNumber( y );
        argStream.ReadNumber( width );
        argStream.ReadNumber( height );
        argStream.ReadNumber( color, 0xFFFFFFFF );
        argStream.ReadBool( postGUI, false );

        if ( !argStream.HasErrors() )
        {
            g_pCore->GetGraphics()->DrawRectQueued( x, y, width, height, color, postGUI );
            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( dxDrawImage )
    {
    //  bool dxDrawImage ( float posX, float posY, float width, float height, string filepath [, float rotation = 0, float rotationCenterOffsetX = 0, 
    //      float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )
        float fPosX; float fPosY; float fWidth; float fHeight; CClientMaterial* pMaterialElement; float fRotation;
            float fRotCenOffX; float fRotCenOffY; uint ulColor; bool bPostGUI;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fPosX );
        argStream.ReadNumber ( fPosY );
        argStream.ReadNumber ( fWidth );
        argStream.ReadNumber ( fHeight );
        MixedReadMaterialString ( argStream, pMaterialElement );
        argStream.ReadNumber ( fRotation, 0 );
        argStream.ReadNumber ( fRotCenOffX, 0 );
        argStream.ReadNumber ( fRotCenOffY, 0 );
        argStream.ReadNumber ( ulColor, 0xffffffff );
        argStream.ReadBool ( bPostGUI, false );

        if ( !argStream.HasErrors () )
        {
            if ( pMaterialElement )
            {
                g_pCore->GetGraphics ()->DrawTextureQueued ( fPosX, fPosY, fWidth, fHeight, 0, 0, 1, 1, true, pMaterialElement->GetMaterialItem (), fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI );
                lua_pushboolean ( L, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "dxDrawImage cannot resolve material" );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxDrawImageSection )
    {
    //  bool dxDrawImageSection ( float posX, float posY, float width, float height, float u, float v, float usize, float vsize, string filepath, 
    //      [ float rotation = 0, float rotationCenterOffsetX = 0, float rotationCenterOffsetY = 0, int color = white, bool postGUI = false ] )
        float fPosX; float fPosY; float fWidth; float fHeight; float fU; float fV; float fSizeU; float fSizeV; CClientMaterial* pMaterialElement;
             float fRotation; float fRotCenOffX; float fRotCenOffY; uint ulColor; bool bPostGUI;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fPosX );
        argStream.ReadNumber ( fPosY );
        argStream.ReadNumber ( fWidth );
        argStream.ReadNumber ( fHeight );
        argStream.ReadNumber ( fU );
        argStream.ReadNumber ( fV );
        argStream.ReadNumber ( fSizeU );
        argStream.ReadNumber ( fSizeV );
        MixedReadMaterialString ( argStream, pMaterialElement );
        argStream.ReadNumber ( fRotation, 0 );
        argStream.ReadNumber ( fRotCenOffX, 0 );
        argStream.ReadNumber ( fRotCenOffY, 0 );
        argStream.ReadNumber ( ulColor, 0xffffffff );
        argStream.ReadBool ( bPostGUI, false );

        if ( !argStream.HasErrors () )
        {
            if ( pMaterialElement )
            {
                g_pCore->GetGraphics ()->DrawTextureQueued ( fPosX, fPosY, fWidth, fHeight, fU, fV, fSizeU, fSizeV, false, pMaterialElement->GetMaterialItem (), fRotation, fRotCenOffX, fRotCenOffY, ulColor, bPostGUI );
                lua_pushboolean ( L, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "dxDrawImageSection can't load file" );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxGetTextWidth )
    {
    //  float dxGetTextWidth ( string text, [float scale=1, mixed font="default"] )
        SString strText; float fScale; SString strFontName; CClientDxFont* pDxFontElement;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strText );
        argStream.ReadNumber ( fScale, 1 );
        MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );

        if ( !argStream.HasErrors () )
        {
            ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

            // Retrieve the longest line's extent
            std::stringstream ssText ( strText );
            std::string sLineText;
            float fWidth = 0.0f, fLineExtent = 0.0f;

            while( std::getline ( ssText, sLineText ) )
            {
                fLineExtent = g_pCore->GetGraphics ()->GetDXTextExtent ( sLineText.c_str ( ), fScale, pD3DXFont );
                if ( fLineExtent > fWidth )
                    fWidth = fLineExtent;
            }

            // Success
            lua_pushnumber ( L, fWidth );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxGetFontHeight )
    {
    //  int dxGetFontHeight ( [float scale=1, mixed font="default"] )
        float fScale; SString strFontName; CClientDxFont* pDxFontElement;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fScale, 1 );
        MixedReadDxFontString ( argStream, strFontName, "default", pDxFontElement );

        if ( !argStream.HasErrors () )
        {
            ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( strFontName, pDxFontElement );

            // Success
            lua_pushnumber ( L, g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pD3DXFont ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxCreateTexture )
    {
    //  element dxCreateTexture( string filepath )
        SString strFilePath;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strFilePath );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            const char *meta;
            filePath strPath;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pFileResource, strFilePath, meta, strPath ) )
            {
                if ( pFileResource->FileExists( meta ) )
                {
                    CClientTexture* pTexture = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateTexture ( strPath.c_str(), *pParentResource->GetResourceDynamicEntity() );

                    // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **

                    pTexture->PushStack( L );
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer( "dxCreateTexture", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer( "dxCreateTexture", "file-path", 1 );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxCreateShader )
    {
    //  element dxCreateShader( string filepath [, bool debug] )
        SString strFilePath; bool bDebug;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strFilePath );
        argStream.ReadBool ( bDebug, false );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            const char *meta;
            filePath strPath;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pFileResource, strFilePath, meta, strPath ) )
            {
                if ( pFileResource->FileExists( meta ) )
                {
                    filePath strRootPath = filePath( strPath.c_str(), strPath.size() - strlen( meta ) );
                    SString strStatus;
                    CClientShader* pShader = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateShader ( strPath.convert_ansi(), strRootPath.convert_ansi(), strStatus, bDebug, *pParentResource->GetResourceDynamicEntity() );
                    if ( pShader )
                    {
                        // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                        pShader->PushStack( L );
                        lua_pushstring ( L, strStatus );    // String containing name of technique being used.
                        return 2;
                    }
                    else
                    {
                        // Replace any path in the error message with our own one
                        SString path = filePath( strRootPath );
                        SString strRootPathWithoutResource = path.Left( path.TrimEnd ( "\\" ).length() - pFileResource->GetName().size() );
                        strStatus = strStatus.ReplaceI ( strRootPathWithoutResource, "" );
                        m_pScriptDebugging->LogCustom( SString ( "Problem @ '%s' [%s]", "dxCreateShader", *strStatus ) );
                    }
                }
                else
                    m_pScriptDebugging->LogCustom( SString ( "Missing file @ '%s' [%s]", "dxCreateShader", strPath.c_str() ) );
            }
            else
                m_pScriptDebugging->LogCustom( SString ( "Bad file-path @ '%s' [%s]", "dxCreateShader", *strFilePath ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxCreateRenderTarget )
    {
    //  element dxCreateRenderTarget( int sizeX, int sizeY [, int withAlphaChannel = false ] )
        uint uiSizeX; uint uiSizeY; bool bWithAlphaChannel;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( uiSizeX );
        argStream.ReadNumber ( uiSizeY );
        argStream.ReadBool ( bWithAlphaChannel, false );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;

            CClientRenderTarget* pRenderTarget = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateRenderTarget ( uiSizeX, uiSizeY, bWithAlphaChannel, *pParentResource->GetResourceDynamicEntity() );
            if ( pRenderTarget )
            {
                pRenderTarget->PushStack( L );

                // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            }
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxCreateScreenSource )
    {
    //  element dxCreateScreenSource( int sizeX, int sizeY )
        uint uiSizeX; uint uiSizeY;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( uiSizeX );
        argStream.ReadNumber ( uiSizeY );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pParentResource = pLuaMain->GetResource ();

            CClientScreenSource* pScreenSource = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateScreenSource ( uiSizeX, uiSizeY, *pParentResource->GetResourceDynamicEntity() );
            if ( pScreenSource )
            {
                pScreenSource->PushStack( L );

                // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            }
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "dxCreateScreenSource", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxGetMaterialSize )
    {
    //  int, int dxGetMaterialSize( element material )
        CClientMaterial* pMaterial; SString strName;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pMaterial, LUACLASS_RENDERELEMENT );

        if ( !argStream.HasErrors () )
        {
            lua_pushnumber ( L, pMaterial->GetMaterialItem ()->m_uiSizeX );
            lua_pushnumber ( L, pMaterial->GetMaterialItem ()->m_uiSizeY );
            return 2;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxSetShaderValue )
    {
    //  bool dxSetShaderValue( element shader, string name, mixed value )
        CClientShader* pShader; SString strName;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pShader, LUACLASS_SHADER );
        argStream.ReadString( strName );

        if ( !argStream.HasErrors () )
        {
            // Try each mixed type in turn
            int iArgument = lua_type ( L, argStream.m_iIndex );

            if ( iArgument == LUA_TCLASS )
            {
                // Texture
                CClientTexture* pTexture;
                if ( argStream.ReadClass( pTexture, LUACLASS_CORETEXTURE ) )
                {
                    bool bResult = pShader->GetShaderItem()->SetValue( strName, pTexture->GetTextureItem() );
                    lua_pushboolean ( L, bResult );
                    return 1;
                }
            }
            else if ( iArgument == LUA_TBOOLEAN )
            {
                // bool
                bool bValue;
                if ( argStream.ReadBool ( bValue ) )
                {
                    bool bResult = pShader->GetShaderItem()->SetValue( strName, bValue );
                    lua_pushboolean ( L, bResult );
                    return 1;
                }
            }
            else if ( lua_isnumber( L, argStream.m_iIndex ) )
            {
                // float(s)
                float fBuffer[16];
                uint i;
                for ( i = 0 ; i < NUMELMS(fBuffer); i++ )
                {
                    fBuffer[i] = static_cast < float > ( lua_tonumber ( L, argStream.m_iIndex++ ) );
                    iArgument = lua_type ( L, argStream.m_iIndex );
                    if ( iArgument != LUA_TNUMBER && iArgument != LUA_TSTRING )
                        break;
                }
                bool bResult = pShader->GetShaderItem()->SetValue( strName, fBuffer, i + 1 );
                lua_pushboolean ( L, bResult );
                return 1;
            }
            else if ( iArgument == LUA_TTABLE )
            {
                // table (of floats)
                float fBuffer[16];
                for ( uint i = 0 ; i < NUMELMS(fBuffer); i++ )
                {
                    // TODO
                }
            }
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "dxSetShaderValue", "Expected number, bool, table or texture at argument 3" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "dxSetShaderValue", *argStream.GetErrorMessage () ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxSetRenderTarget )
    {
    //  bool setRenderTaget( element renderTarget [, bool clear = false ] )
        CClientRenderTarget* pRenderTarget; bool bClear;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pRenderTarget, LUACLASS_CORETEXTURE, NULL );
        argStream.ReadBool( bClear, false );

        if ( !argStream.HasErrors () )
        {
            bool bResult;
            if ( pRenderTarget )
                bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->SetRenderTarget ( pRenderTarget->GetRenderTargetItem (), bClear );
            else
                bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->RestoreDefaultRenderTarget ();

            if ( bResult )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogCustom( SString ( "Bad usage @ '%s' [%s]", "dxSetRenderTarget", "dxSetRenderTarget can only be used inside rendering events" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxUpdateScreenSource )
    {
        CClientScreenSource* pScreenSource;
        bool resampleNow;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pScreenSource, LUACLASS_CORESCREENSOURCE );
        argStream.ReadBool( resampleNow, false );

        if ( !argStream.HasErrors () )
        {
            g_pCore->GetGraphics ()->GetRenderItemManager ()->UpdateScreenSource ( pScreenSource->GetScreenSourceItem (), resampleNow );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxCreateFont )
    {
    //  element dxCreateFont( string filepath [, int size=9, bool bold=false ] )
        SString strFilePath; int iSize; bool bBold;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strFilePath );
        argStream.ReadNumber ( iSize, 9 );
        argStream.ReadBool ( bBold, false );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pParentResource = pLuaMain->GetResource ();
            CResource* pFileResource = pParentResource;
            const char *meta;
            filePath strPath;
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pFileResource, strFilePath, meta, strPath ) )
            {
                if ( pFileResource->FileExists( meta ) )
                {
                    CClientDxFont* pDxFont = g_pClientGame->GetManager ()->GetRenderElementManager ()->CreateDxFont ( strPath.convert_ansi(), iSize, bBold, *pParentResource->GetResourceDynamicEntity() );
                    if ( pDxFont )
                    {
                        pDxFont->PushStack( L );

                        // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
                    }
                    return 1;
                }
                else
                    m_pScriptDebugging->LogBadPointer( "dxCreateFont", "file-path", 1 );
            }
            else
                m_pScriptDebugging->LogBadPointer( "dxCreateFont", "file-path", 1 );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxSetBlendMode )
    {
        EBlendModeType mode;

        CScriptArgReader argStream( L );
        argStream.ReadEnumString( mode );

        if ( !argStream.HasErrors() )
        {
            g_pCore->GetGraphics()->SetBlendMode( mode );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( dxGetBlendMode )
    {
        lua_pushstring( L, EnumToString( g_pCore->GetGraphics()->GetBlendMode() ) );
        return 1;
    }

    LUA_DECLARE( dxSetTestMode )
    {
    //  bool dxSetTestMode( string testMode )
        eDxTestMode testMode;

        CScriptArgReader argStream ( L );
        argStream.ReadEnumString ( testMode, DX_TEST_MODE_NONE );

        if ( !argStream.HasErrors () )
        {
            g_pCore->GetGraphics ()->GetRenderItemManager ()->SetTestMode ( testMode );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( dxGetStatus )
    {
    //  table dxGetStatus()

        CScriptArgReader argStream ( L );

        if ( !argStream.HasErrors () )
        {
            SDxStatus dxStatus;
            g_pCore->GetGraphics ()->GetRenderItemManager ()->GetDxStatus ( dxStatus );

            lua_createtable ( L, 0, 13 );

            lua_pushstring ( L, "TestMode" );
            lua_pushstring ( L, EnumToString ( dxStatus.testMode ) );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoCardName" );
            lua_pushstring ( L, dxStatus.videoCard.strName );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoCardRAM" );
            lua_pushnumber ( L, dxStatus.videoCard.iInstalledMemoryKB / 1024 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoCardPSVersion" );
            lua_pushstring ( L, dxStatus.videoCard.strPSVersion );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoMemoryFreeForMTA" );
            lua_pushnumber ( L, dxStatus.videoMemoryKB.iFreeForMTA / 1024 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoMemoryUsedByFonts" );
            lua_pushnumber ( L, dxStatus.videoMemoryKB.iUsedByFonts / 1024 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoMemoryUsedByTextures" );
            lua_pushnumber ( L, dxStatus.videoMemoryKB.iUsedByTextures / 1024 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "VideoMemoryUsedByRenderTargets" );
            lua_pushnumber ( L, dxStatus.videoMemoryKB.iUsedByRenderTargets / 1024 );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "SettingWindowed" );
            lua_pushboolean ( L, dxStatus.settings.bWindowed );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "SettingFXQuality" );
            lua_pushnumber ( L, dxStatus.settings.iFXQuality );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "SettingDrawDistance" );
            lua_pushnumber ( L, dxStatus.settings.iDrawDistance );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "SettingVolumetricShadows" );
            lua_pushboolean ( L, dxStatus.settings.bVolumetricShadows );
            lua_settable   ( L, -3 );

            lua_pushstring ( L, "SettingStreamingVideoMemoryForGTA" );
            lua_pushnumber ( L, dxStatus.settings.iStreamingMemory );
            lua_settable   ( L, -3 );

            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // error: bad arguments
        lua_pushboolean ( L, false );
        return 1;
    }
}
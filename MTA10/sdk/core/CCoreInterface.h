/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CCoreInterface.h
*  PURPOSE:     Core interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOREINTERFACE_H
#define __CCOREINTERFACE_H

#include <core/interface.h>

#include "CConsoleInterface.h"
#include "CCommandsInterface.h"
#include "CCommunityInterface.h"
#include "CRenderItemManagerInterface.h"
#include "CPixelsManagerInterface.h"
#include "CScreenGrabberInterface.h"
#include "CGraphicsInterface.h"
#include "CModManagerInterface.h"
#include "CServerInterface.h"
#include "CKeyBindsInterface.h"
#include "CCVarsInterface.h"
#include "CJoystickManagerInterface.h"
#include "xml/CXML.h"
#include <gui/CGUI.h>

typedef bool (*pfnProcessMessage) ( HWND, UINT, WPARAM, LPARAM );

class CMultiplayer;
class CNet;
class CGame;

namespace ChatFonts
{
    enum eChatFont { CHAT_FONT_DEFAULT,CHAT_FONT_CLEAR,CHAT_FONT_BOLD,CHAT_FONT_ARIAL,CHAT_FONT_MAX };
}

using ChatFonts::eChatFont;

enum eCoreVersion
{
    MTACORE_20 = 1,
    MTACORE_21
};

#ifndef WITH_TIMING_CHECKPOINTS
    #define WITH_TIMING_CHECKPOINTS 0     // Comment this line to remove timing checkpoint code
#endif

#if WITH_TIMING_CHECKPOINTS
    #define TIMING_CHECKPOINT(x)        g_pCore->OnTimingCheckpoint ( x )
    #define TIMING_DETAIL(x)            g_pCore->OnTimingDetail ( x )
    #define TIMING_DETAIL_FORCE(x)      g_pCore->OnTimingDetail ( x, true )
#else
    #define TIMING_CHECKPOINT(x)        {}
    #define TIMING_DETAIL(x)            {}
    #define TIMING_DETAIL_FORCE(x)      {}
#endif

class CCoreInterface
{
public:
    // Note: Leave this on top for compatibility reasons!
    virtual eCoreVersion                GetVersion() = 0;

    virtual CConsoleInterface*          GetConsole() = 0;
    virtual CCommandsInterface*         GetCommands() = 0;
    virtual CGame*                      GetGame() = 0;
    virtual CGraphicsInterface*         GetGraphics() = 0;
    virtual CGUI*                       GetGUI() = 0;
    virtual CModManagerInterface*       GetModManager() = 0;
    virtual CServerInterface*           GetServer() = 0;
    virtual CMultiplayer*               GetMultiplayer() = 0;
    virtual CNet*                       GetNetwork() = 0;
    virtual CXML*                       GetXML() = 0;
    virtual CKeyBindsInterface*         GetKeyBinds() = 0;
    virtual CXMLNode*                   GetConfig() = 0;
    virtual CCVarsInterface*            GetCVars() = 0;
    virtual CCommunityInterface*        GetCommunity() = 0;
    virtual CFileSystemInterface*       GetFileSystem() = 0;
    virtual CJoystickManagerInterface*  GetJoystickManager() = 0;
    
    // Temporary functions for r1
    virtual void                    DebugEcho( const char* szText ) = 0;
    virtual void                    DebugPrintf( const char* szFormat, ... ) = 0;
    virtual void                    SetDebugVisible( bool bVisible ) = 0;
    virtual bool                    IsDebugVisible() = 0;
    virtual void                    DebugEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B ) = 0;
    virtual void                    DebugPrintfColor( const char* szFormat, unsigned char R, unsigned char G, unsigned char B, ... ) = 0;
    virtual void                    DebugClear() = 0;
    virtual void                    ChatEcho( const char* szText, bool bColorCoded = false ) = 0;
    virtual void                    ChatEchoColor( const char* szText, unsigned char R, unsigned char G, unsigned char B, bool bColorCoded = false ) = 0;
    virtual void                    ChatPrintf( const char* szFormat, bool bColorCoded, ... ) = 0;
    virtual void                    ChatPrintfColor( const char* szFormat, bool bColorCoded, unsigned char R, unsigned char G, unsigned char B, ... ) = 0;
    virtual void                    SetChatEnabled( bool enabled ) = 0;
    virtual bool                    IsChatEnabled() const = 0;
    virtual void                    SetChatVisible( bool bVisible ) = 0;
    virtual bool                    IsChatVisible() = 0;
    virtual void                    TakeScreenShot() = 0;
    virtual void                    EnableChatInput( char* szCommand, DWORD dwColor ) = 0;
    virtual bool                    IsChatInputEnabled() = 0;
    virtual bool                    IsSettingsVisible() = 0;
    virtual bool                    IsMenuVisible() = 0;
    virtual bool                    IsCursorForcedVisible() = 0;
    virtual bool                    IsCursorControlsToggled() = 0;
    virtual void                    CallSetCursorPos( int X, int Y ) = 0;

    virtual void                    SetConnected( bool bConnected ) = 0;
    virtual void                    SetOfflineMod( bool bOffline ) = 0;

    virtual bool                    IsConnected() = 0;
    virtual bool                    Reconnect( const char *szHost, unsigned short usPort, const char *szPassword, bool bSave = true ) = 0;

    virtual const char*             GetModInstallRoot( const char *szModName ) = 0;
    virtual CFileTranslator*        GetModRoot() = 0;
    virtual CFileTranslator*        GetMTARoot() = 0;
    virtual CFileTranslator*        GetDataRoot() = 0;

    virtual void                    ShowServerInfo( unsigned int WindowType ) = 0;

    virtual void                    ForceCursorVisible( bool bVisible, bool bToggleControls = true ) = 0;
    virtual void                    SetMessageProcessor( pfnProcessMessage pfnMessageProcessor ) = 0;
    virtual void                    ShowMessageBox( const char* szTitle, const char* szText, unsigned int uiFlags, GUI_CALLBACK * ResponseHandler = NULL ) = 0;
    virtual void                    RemoveMessageBox( bool bNextFrame = false ) = 0;
    virtual void                    HideMainMenu() = 0;
    virtual HWND                    GetHookedWindow() = 0;
    virtual bool                    IsFocused() = 0;
    virtual bool                    IsWindowMinimized() = 0;

    virtual void                    SaveConfig() = 0;
    virtual void                    UpdateRecentlyPlayed() = 0;

    virtual void                    SwitchRenderWindow( HWND hWnd, HWND hWndInput ) = 0;
    virtual void                    SetCenterCursor( bool bEnabled ) = 0;

    // CGUI Callbacks
    virtual bool                    OnMouseClick( CGUIMouseEventArgs Args ) = 0;
    virtual bool                    OnMouseDoubleClick( CGUIMouseEventArgs Args ) = 0;

    virtual void                    Quit( bool bInstantly = true) = 0;
    virtual void                    InitiateUpdate( const char* szType, const char* szData, const char* szHost ) = 0;
    virtual bool                    IsOptionalUpdateInfoRequired( const char* szHost ) = 0;
    virtual void                    InitiateDataFilesFix() = 0;

    virtual unsigned int            GetFrameRateLimit() = 0;
    virtual void                    RecalculateFrameRateLimit( uint uiServerFrameRateLimit = -1 ) = 0;
    virtual void                    ApplyFrameRateLimit( uint uiOverrideRate = -1 ) = 0;
    virtual void                    EnsureFrameRateLimitApplied() = 0;

    virtual void                    OnPreHUDRender() = 0;
    virtual unsigned int            GetMinStreamingMemory() = 0;
    virtual unsigned int            GetMaxStreamingMemory() = 0;
};

#endif

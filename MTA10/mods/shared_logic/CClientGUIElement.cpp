/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIElement.cpp
*  PURPOSE:     GUI wrapper entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

static const luaL_Reg gui_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_gui( lua_State *L )
{
    CClientGUIElement *gui = (CClientGUIElement*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_GUIELEMENT, gui );

    j.RegisterInterfaceTrans( L, gui_interface, 0, LUACLASS_GUIELEMENT );

    lua_pushlstring( L, "gui-element", 11 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientGUIElement::CClientGUIElement( CClientManager *pManager, CLuaMain *pLuaMain, CGUIElement *pCGUIElement, bool system, ElementID ID ) : CClientEntity( ID, system, pLuaMain->GetVirtualMachine() )
{
    // Advance the Lua interface
    lua_State *L = pLuaMain->GetVirtualMachine();

    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_gui, 1 );
    luaJ_extend( L, -2, 0 );

    // Set root entity
    SetRoot( pLuaMain->GetResource()->GetResourceGUIEntity() );

    m_pManager = pManager;
    m_pGUIManager = pManager->GetGUIManager();
    m_pCGUIElement = pCGUIElement;
    m_pLuaMain = pLuaMain;
    m_pFontElement = NULL;

    memset( &_szCallbackFunc1[0], NULL, sizeof( _szCallbackFunc1 ) );
    memset( &_szCallbackFunc2[0], NULL, sizeof( _szCallbackFunc2 ) );

    // Store the this-pointer in the userdata variable
    CGUI_SET_CCLIENTGUIELEMENT( pCGUIElement, this );

    // Generate the CGUI type name variable
    // Inherit a custom GUI interface, too
    lua_pushlightuserdata( L, this );

    switch( m_pCGUIElement->GetType() )
    {
    case CGUI_BUTTON:
        lua_pushcclosure( L, luaconstructor_guibutton, 1 );
        m_szCGUITypeName = "button";
        break;
    case CGUI_CHECKBOX:
        lua_pushcclosure( L, luaconstructor_guicheckbox, 1 );
        m_szCGUITypeName = "checkbox";
        break;
    case CGUI_EDIT:
        lua_pushcclosure( L, luaconstructor_guiedit, 1 );
        m_szCGUITypeName = "edit";
        break;
    case CGUI_GRIDLIST:
        lua_pushcclosure( L, luaconstructor_guigridlist, 1 );
        m_szCGUITypeName = "gridlist";
        break;
    case CGUI_LABEL:
        lua_pushcclosure( L, luaconstructor_guilabel, 1 );
        m_szCGUITypeName = "label";
        break;
    case CGUI_MEMO:
        lua_pushcclosure( L, luaconstructor_guimemo, 1 );
        m_szCGUITypeName = "memo";
        break;
    case CGUI_PROGRESSBAR:
        lua_pushcclosure( L, luaconstructor_guiprogressbar, 1 );
        m_szCGUITypeName = "progressbar";
        break;
    case CGUI_RADIOBUTTON:
        lua_pushcclosure( L, luaconstructor_guiradiobutton, 1 );
        m_szCGUITypeName = "radiobutton";
        break;
    case CGUI_STATICIMAGE:
        lua_pushcclosure( L, luaconstructor_guistaticimage, 1 );
        m_szCGUITypeName = "staticimage";
        break;
    case CGUI_TAB:
        lua_pushcclosure( L, luaconstructor_guitab, 1 );
        m_szCGUITypeName = "tab";
        break;
    case CGUI_TABPANEL:
        lua_pushcclosure( L, luaconstructor_guitabpanel, 1 );
        m_szCGUITypeName = "tabpanel";
        break;
    case CGUI_WINDOW:
        lua_pushcclosure( L, luaconstructor_guiwindow, 1 );
        m_szCGUITypeName = "window";
        break;
    case CGUI_SCROLLPANE:
        lua_pushcclosure( L, luaconstructor_guiscrollpane, 1 );
        m_szCGUITypeName = "scrollpane";
        break;
    case CGUI_SCROLLBAR:
        lua_pushcclosure( L, luaconstructor_guiscrollbar, 1 );
        m_szCGUITypeName = "scrollbar";
        break;
    case CGUI_COMBOBOX:
        lua_pushcclosure( L, luaconstructor_guicombobox, 1 );
        m_szCGUITypeName = "combobox";
        break;
    default:
        assert( 0 );    // We cannot afford this
    }

    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    SString typeName = SString( "gui-%s", m_szCGUITypeName );

    SetTypeName( typeName );

    // Add us to the list in the manager
    m_pGUIManager->Add ( this );
}

CClientGUIElement::~CClientGUIElement()
{
    // Remove us from the list in the manager
    Unlink();

    delete m_pCGUIElement;
}

void CClientGUIElement::Unlink()
{
    // Detach from any custom font
    if ( m_pFontElement )
        SetFont( "", NULL );

    m_pGUIManager->Remove( this );
}

void CClientGUIElement::SetEvents( const char *szFunc1, const char *szFunc2 )
{
    if ( szFunc1 && strlen( szFunc1 ) < MAX_EVENT_NAME )
        strncpy( &_szCallbackFunc1[0], szFunc1, strlen ( szFunc1 ) );

    if ( szFunc2 && strlen( szFunc2 ) < MAX_EVENT_NAME )
        strncpy( &_szCallbackFunc2[0], szFunc2, strlen ( szFunc2 ) );    
}

bool CClientGUIElement::_CallbackEvent1( CGUIElement *pCGUIElement )
{
    CClientGUIElement *pElement = m_pGUIManager->Get( pCGUIElement );
    if ( pElement )
    {
        lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();
        pElement->PushStack( L );
        pElement->CallEvent( _szCallbackFunc1, L, 1 );
        return true;
    }

    return false;
}

bool CClientGUIElement::_CallbackEvent2( CGUIElement *pCGUIElement )
{
    CClientGUIElement* pElement = m_pGUIManager->Get ( pCGUIElement );
    if ( pElement )
    {
        lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();
        pElement->PushStack( L );
        pElement->CallEvent( _szCallbackFunc2, L, 1 );
        return true;
    }

    return false;
}

//
// Get which font name and font element we are using now
//
SString CClientGUIElement::GetFont( CClientGuiFont** ppFontElement )
{
    *ppFontElement = m_pFontElement;
    return GetCGUIElement()->GetFont();
}

//
// Change font
//
bool CClientGUIElement::SetFont( const SString& strInFontName, CClientGuiFont* pFontElement )
{
    SString strFontName = strInFontName;

    if ( pFontElement )
        strFontName = pFontElement->GetCEGUIFontName();
    else if ( strFontName.empty() )
        strFontName = "default-normal";

    if ( GetCGUIElement ()->SetFont( strFontName ) )
    {
        if ( m_pFontElement )
            m_pFontElement->NotifyGUIElementDetach( this );

        m_pFontElement = pFontElement;

        if ( m_pFontElement )
            m_pFontElement->NotifyGUIElementAttach( this );

        return true;
    }
    return false;
}
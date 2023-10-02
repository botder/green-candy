/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/mods/shared_logic/lua/CLuaFunctionParseHelpers.h
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#ifndef _LUA_FUNCTION_PARSE_HELPERS_
#define _LUA_FUNCTION_PARSE_HELPERS_

// Forward declare enum reflection stuff

enum eLuaType { };
DECLARE_ENUM( eLuaType );

#ifdef MTA_CLIENT
#include <gui/CGUI.h>
DECLARE_ENUM( CGUIVerticalAlign );
DECLARE_ENUM( CGUIHorizontalAlign );
DECLARE_ENUM( eInputMode );
DECLARE_ENUM( TrafficLight::EColor );
DECLARE_ENUM( TrafficLight::EState );
DECLARE_ENUM( eAccessType );
DECLARE_ENUM( CEasingCurve::eType );
DECLARE_ENUM( eAmbientSoundType );
DECLARE_ENUM( eCGUIType );
DECLARE_ENUM( eDxTestMode );
DECLARE_ENUM( EBlendModeType );
DECLARE_ENUM( RpLightType );
DECLARE_ENUM( eWorldRenderMode );

enum eDXHorizontalAlign
{
    DX_ALIGN_LEFT = DT_LEFT,
    DX_ALIGN_HCENTER = DT_CENTER,
    DX_ALIGN_RIGHT = DT_RIGHT,
};
DECLARE_ENUM( eDXHorizontalAlign );

enum eDXVerticalAlign
{
    DX_ALIGN_TOP = DT_TOP,
    DX_ALIGN_VCENTER = DT_VCENTER,
    DX_ALIGN_BOTTOM = DT_BOTTOM,
};
DECLARE_ENUM( eDXVerticalAlign );

enum eHudComponent
{
    HUD_AMMO,
    HUD_WEAPON,
    HUD_HEALTH,
    HUD_BREATH,
    HUD_ARMOUR,
    HUD_MONEY,
    HUD_VEHICLE_NAME,
    HUD_AREA_NAME,
    HUD_RADAR,
    HUD_CLOCK,
    HUD_RADIO,
    HUD_WANTED,
    HUD_CROSSHAIR,
    HUD_ALL,
};
DECLARE_ENUM( eHudComponent );



// class -> class type
inline eCGUIType GetClassType ( CGUIButton* )      { return CGUI_BUTTON; }
inline eCGUIType GetClassType ( CGUICheckBox* )    { return CGUI_CHECKBOX; }
inline eCGUIType GetClassType ( CGUIEdit* )        { return CGUI_EDIT; }
inline eCGUIType GetClassType ( CGUIGridList* )    { return CGUI_GRIDLIST; }
inline eCGUIType GetClassType ( CGUILabel* )       { return CGUI_LABEL; }
inline eCGUIType GetClassType ( CGUIMemo* )        { return CGUI_MEMO; }
inline eCGUIType GetClassType ( CGUIProgressBar* ) { return CGUI_PROGRESSBAR; }
inline eCGUIType GetClassType ( CGUIRadioButton* ) { return CGUI_RADIOBUTTON; }
inline eCGUIType GetClassType ( CGUIStaticImage* ) { return CGUI_STATICIMAGE; }
inline eCGUIType GetClassType ( CGUITab* )         { return CGUI_TAB; }
inline eCGUIType GetClassType ( CGUITabPanel* )    { return CGUI_TABPANEL; }
inline eCGUIType GetClassType ( CGUIWindow* )      { return CGUI_WINDOW; }
inline eCGUIType GetClassType ( CGUIScrollPane* )  { return CGUI_SCROLLPANE; }
inline eCGUIType GetClassType ( CGUIScrollBar* )   { return CGUI_SCROLLBAR; }
inline eCGUIType GetClassType ( CGUIComboBox* )    { return CGUI_COMBOBOX; }


// class -> class name
inline SString GetClassTypeName ( CClientEntity* )          { return "element"; }
inline SString GetClassTypeName ( CClientCamera* )          { return "camera"; }
inline SString GetClassTypeName ( CClientPlayer* )          { return "player"; }
inline SString GetClassTypeName ( CClientVehicle* )         { return "vehicle"; }
inline SString GetClassTypeName ( CClientRadarMarker* )     { return "blip"; }
inline SString GetClassTypeName ( CClientObject* )          { return "object"; }
inline SString GetClassTypeName ( CClientCivilian* )        { return "civilian"; }
inline SString GetClassTypeName ( CClientPickup* )          { return "pickup"; }
inline SString GetClassTypeName ( CClientRadarArea* )       { return "radararea"; }
inline SString GetClassTypeName ( CClientMarker* )          { return "marker"; }
inline SString GetClassTypeName ( CClientTeam* )            { return "team"; }
inline SString GetClassTypeName ( CClientPed* )             { return "ped"; }
inline SString GetClassTypeName ( CClientProjectile* )      { return "projectile"; }
inline SString GetClassTypeName ( CClientGUIElement* )      { return "gui-element"; }
inline SString GetClassTypeName ( CClientColShape* )        { return "colshape"; }
inline SString GetClassTypeName ( CClientDummy* )           { return "dummy"; }
inline SString GetClassTypeName ( CClientDFF* )             { return "dff"; }
inline SString GetClassTypeName ( CClientColModel* )        { return "col-model"; }
inline SString GetClassTypeName ( CClientTXD* )             { return "txd"; }
inline SString GetClassTypeName ( CClientSound* )           { return "sound"; }
inline SString GetClassTypeName ( CClientWater* )           { return "water"; }
inline SString GetClassTypeName ( CClientDxFont* )          { return "dx-font"; }
inline SString GetClassTypeName ( CClientGuiFont* )         { return "gui-font"; }
inline SString GetClassTypeName ( CClientMaterial* )        { return "material"; }
inline SString GetClassTypeName ( CClientTexture* )         { return "texture"; }

inline SString GetClassTypeName ( CGUIButton* )      { return "gui-button"; }
inline SString GetClassTypeName ( CGUICheckBox* )    { return "gui-checkbox"; }
inline SString GetClassTypeName ( CGUIEdit* )        { return "gui-edit"; }
inline SString GetClassTypeName ( CGUIGridList* )    { return "gui-gridlist"; }
inline SString GetClassTypeName ( CGUILabel* )       { return "gui-label"; }
inline SString GetClassTypeName ( CGUIMemo* )        { return "gui-memo"; }
inline SString GetClassTypeName ( CGUIProgressBar* ) { return "gui-progressbar"; }
inline SString GetClassTypeName ( CGUIRadioButton* ) { return "gui-radiobutton"; }
inline SString GetClassTypeName ( CGUIStaticImage* ) { return "gui-staticimage"; }
inline SString GetClassTypeName ( CGUITab* )         { return "gui-tab"; }
inline SString GetClassTypeName ( CGUITabPanel* )    { return "gui-tabpanel"; }
inline SString GetClassTypeName ( CGUIWindow* )      { return "gui-window"; }
inline SString GetClassTypeName ( CGUIScrollPane* )  { return "gui-scrollpane"; }
inline SString GetClassTypeName ( CGUIScrollBar* )   { return "gui-scrollbar"; }
inline SString GetClassTypeName ( CGUIComboBox* )    { return "gui-combobox"; }

inline SString GetClassTypeName ( CResource* )              { return "resource-data"; }
inline SString GetClassTypeName ( CXMLNode* )               { return "xml-node"; }
inline SString GetClassTypeName ( CLuaTimer* )              { return "lua-timer"; }
inline SString GetClassTypeName ( CEntity* )                { return "entity"; }


//
// CXMLNode from userdata
//
template < class T >
CXMLNode* UserDataCast ( CXMLNode*, void* ptr, lua_State* )
{
    return g_pCore->GetXML ()->GetNodeFromID ( reinterpret_cast < unsigned long > ( ptr ) );
}

//
// Reading mixed types
//
class CScriptArgReader;
bool MixedReadDxFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientDxFont*& pFontElement );
bool MixedReadGuiFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientGuiFont*& pFontElement );
bool MixedReadMaterialString ( CScriptArgReader& argStream, CClientMaterial*& pMaterialElement );

#endif //MTA_CLIENT

#endif //_LUA_FUNCTION_PARSE_HELPERS_
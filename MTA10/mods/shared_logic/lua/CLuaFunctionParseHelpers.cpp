/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        MTA10/mods/shared_logic/lua/CLuaFunctionParseHelpers.cpp
*  PURPOSE:
*  DEVELOPERS:  Nobody knows
*
*****************************************************************************/

#include "StdInc.h"

//
// enum values <-> script strings
//

IMPLEMENT_ENUM_BEGIN( eLuaType )
    ADD_ENUM ( LUA_TNONE,           "none" )
    ADD_ENUM ( LUA_TNIL,            "nil" )
    ADD_ENUM ( LUA_TBOOLEAN,        "boolean" )
    ADD_ENUM ( LUA_TLIGHTUSERDATA,  "lightuserdata" )
    ADD_ENUM ( LUA_TNUMBER,         "number" )
    ADD_ENUM ( LUA_TSTRING,         "string" )
    ADD_ENUM ( LUA_TTABLE,          "table" )
    ADD_ENUM ( LUA_TFUNCTION,       "function" )
    ADD_ENUM ( LUA_TUSERDATA,       "userdata" )
    ADD_ENUM ( LUA_TCLASS,          "class" )
    ADD_ENUM ( LUA_TDISPATCH,       "dispatch" )
    ADD_ENUM ( LUA_TTHREAD,         "thread" )
IMPLEMENT_ENUM_END( "lua-type" )

#ifdef MTA_CLIENT

IMPLEMENT_ENUM_BEGIN( CGUIVerticalAlign )
    ADD_ENUM ( CGUI_ALIGN_TOP,              "top" )
    ADD_ENUM ( CGUI_ALIGN_BOTTOM,           "bottom" )
    ADD_ENUM ( CGUI_ALIGN_VERTICALCENTER,   "center" )
IMPLEMENT_ENUM_END( "vertical-align" )


IMPLEMENT_ENUM_BEGIN( CGUIHorizontalAlign )
    ADD_ENUM ( CGUI_ALIGN_LEFT,             "left" )
    ADD_ENUM ( CGUI_ALIGN_RIGHT,            "right" )
    ADD_ENUM ( CGUI_ALIGN_HORIZONTALCENTER, "center" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eInputMode )
    ADD_ENUM ( INPUTMODE_ALLOW_BINDS,       "allow_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS,          "no_binds" )
    ADD_ENUM ( INPUTMODE_NO_BINDS_ON_EDIT,  "no_binds_when_editing" )
IMPLEMENT_ENUM_END( "input-mode" )


IMPLEMENT_ENUM_BEGIN( eAccessType )
    ADD_ENUM ( ACCESS_PUBLIC,               "public" )
    ADD_ENUM ( ACCESS_PRIVATE,              "private" )
IMPLEMENT_ENUM_END( "access-type" )


IMPLEMENT_ENUM_BEGIN( eDXHorizontalAlign )
    ADD_ENUM ( DX_ALIGN_LEFT,           "left" )
    ADD_ENUM ( DX_ALIGN_HCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_RIGHT,          "right" )
IMPLEMENT_ENUM_END( "horizontal-align" )


IMPLEMENT_ENUM_BEGIN( eDXVerticalAlign )
    ADD_ENUM ( DX_ALIGN_TOP,            "top" )
    ADD_ENUM ( DX_ALIGN_VCENTER,        "center" )
    ADD_ENUM ( DX_ALIGN_BOTTOM,         "bottom" )
IMPLEMENT_ENUM_END( "vertical-align" )


IMPLEMENT_ENUM_BEGIN( TrafficLight::EColor )
    ADD_ENUM ( TrafficLight::GREEN,           "green" )
    ADD_ENUM ( TrafficLight::YELLOW,          "yellow" )
    ADD_ENUM ( TrafficLight::RED,             "red" )
IMPLEMENT_ENUM_END( "traffic-light-color" )


IMPLEMENT_ENUM_BEGIN( TrafficLight::EState )
    ADD_ENUM ( TrafficLight::AUTO,            "auto" )
    ADD_ENUM ( TrafficLight::DISABLED,        "disabled" )
IMPLEMENT_ENUM_END( "traffic-light-state" )


IMPLEMENT_ENUM_BEGIN( CEasingCurve::eType )
    ADD_ENUM ( CEasingCurve::Linear,           "Linear" )
    ADD_ENUM ( CEasingCurve::InQuad,           "InQuad" )
    ADD_ENUM ( CEasingCurve::OutQuad,          "OutQuad" )
    ADD_ENUM ( CEasingCurve::InOutQuad,        "InOutQuad" )
    ADD_ENUM ( CEasingCurve::OutInQuad,        "OutInQuad" )
    ADD_ENUM ( CEasingCurve::InElastic,        "InElastic" )
    ADD_ENUM ( CEasingCurve::OutElastic,       "OutElastic" )
    ADD_ENUM ( CEasingCurve::InOutElastic,     "InOutElastic" )
    ADD_ENUM ( CEasingCurve::OutInElastic,     "OutInElastic" )
    ADD_ENUM ( CEasingCurve::InBack,           "InBack" )
    ADD_ENUM ( CEasingCurve::OutBack,          "OutBack" )
    ADD_ENUM ( CEasingCurve::InOutBack,        "InOutBack" )
    ADD_ENUM ( CEasingCurve::OutInBack,        "OutInBack" )
    ADD_ENUM ( CEasingCurve::InBounce,         "InBounce" )
    ADD_ENUM ( CEasingCurve::OutBounce,        "OutBounce" )
    ADD_ENUM ( CEasingCurve::InOutBounce,      "InOutBounce" )
    ADD_ENUM ( CEasingCurve::OutInBounce,      "OutInBounce" )
    ADD_ENUM ( CEasingCurve::SineCurve,        "SineCurve" )
    ADD_ENUM ( CEasingCurve::CosineCurve,      "CosineCurve" )
IMPLEMENT_ENUM_END_DEFAULTS( "easing-type", CEasingCurve::EASING_INVALID, "Invalid" )

IMPLEMENT_ENUM_BEGIN( eHudComponent )
    ADD_ENUM ( HUD_AMMO,            "ammo" )
    ADD_ENUM ( HUD_WEAPON,          "weapon" )
    ADD_ENUM ( HUD_HEALTH,          "health" )
    ADD_ENUM ( HUD_BREATH,          "breath" )
    ADD_ENUM ( HUD_ARMOUR,          "armour" )
    ADD_ENUM ( HUD_MONEY,           "money" )
    ADD_ENUM ( HUD_VEHICLE_NAME,    "vehicle_name" )
    ADD_ENUM ( HUD_AREA_NAME,       "area_name" )
    ADD_ENUM ( HUD_RADAR,           "radar" )
    ADD_ENUM ( HUD_CLOCK,           "clock" )
    ADD_ENUM ( HUD_RADIO,           "radio" )
    ADD_ENUM ( HUD_WANTED,          "wanted" )
    ADD_ENUM ( HUD_CROSSHAIR,       "crosshair" )
    ADD_ENUM ( HUD_ALL,             "all" )
IMPLEMENT_ENUM_END( "hud-component" )

IMPLEMENT_ENUM_BEGIN( eAmbientSoundType )
    ADD_ENUM ( AMBIENT_SOUND_GENERAL,           "general" )
    ADD_ENUM ( AMBIENT_SOUND_GUNFIRE,           "gunfire" )
IMPLEMENT_ENUM_END( "ambient-sound-type" )

IMPLEMENT_ENUM_BEGIN( eCGUIType )
    ADD_ENUM ( CGUI_BUTTON,         "gui-button" )
    ADD_ENUM ( CGUI_CHECKBOX,       "gui-checkbox" )
    ADD_ENUM ( CGUI_EDIT,           "gui-edit" )
    ADD_ENUM ( CGUI_GRIDLIST,       "gui-gridlist" )
    ADD_ENUM ( CGUI_LABEL,          "gui-label" )
    ADD_ENUM ( CGUI_MEMO,           "gui-memo" )
    ADD_ENUM ( CGUI_PROGRESSBAR,    "gui-progressbar" )
    ADD_ENUM ( CGUI_RADIOBUTTON,    "gui-radiobutton" )
    ADD_ENUM ( CGUI_STATICIMAGE,    "gui-staticimage" )
    ADD_ENUM ( CGUI_TAB,            "gui-tab" )
    ADD_ENUM ( CGUI_TABPANEL,       "gui-tabpanel" )
    ADD_ENUM ( CGUI_WINDOW,         "gui-window" )
    ADD_ENUM ( CGUI_SCROLLPANE,     "gui-scrollpane" )
    ADD_ENUM ( CGUI_SCROLLBAR,      "gui-scrollbar" )
    ADD_ENUM ( CGUI_COMBOBOX,       "gui-combobox" )
IMPLEMENT_ENUM_END( "gui-type" )

IMPLEMENT_ENUM_BEGIN( EBlendModeType )
    ADD_ENUM( BLEND,            "blend" )
    ADD_ENUM( ADD,              "add" )
    ADD_ENUM( MODULATE_ADD,     "modulate_add" )
IMPLEMENT_ENUM_END( "dx-blend-mode" )

IMPLEMENT_ENUM_BEGIN( eDxTestMode )
    ADD_ENUM ( DX_TEST_MODE_NONE,           "none" )
    ADD_ENUM ( DX_TEST_MODE_NO_MEM,         "no_mem" )
    ADD_ENUM ( DX_TEST_MODE_LOW_MEM,        "low_mem" )
    ADD_ENUM ( DX_TEST_MODE_NO_SHADER,      "no_shader" )
IMPLEMENT_ENUM_END( "dx-test-mode" )

IMPLEMENT_ENUM_BEGIN( RpLightType )
    ADD_ENUM( LIGHT_TYPE_DIRECTIONAL,   "directional" )
    ADD_ENUM( LIGHT_TYPE_AMBIENT,       "ambient" )
    ADD_ENUM( LIGHT_TYPE_POINT,         "point" )
    ADD_ENUM( LIGHT_TYPE_SPOT_1,        "spot" )
    ADD_ENUM( LIGHT_TYPE_SPOT_2,        "spot_soft" )
IMPLEMENT_ENUM_END( "rwlight-type" )

IMPLEMENT_ENUM_BEGIN( eWorldRenderMode )
    ADD_ENUM( WORLD_RENDER_ORIGINAL,            "original" )
    ADD_ENUM( WORLD_RENDER_MESHLOCAL_ALPHAFIX,  "meshlocal_alphafix" )
    ADD_ENUM( WORLD_RENDER_SCENE_ALPHAFIX,      "scene_alphafix" )
IMPLEMENT_ENUM_END( "world-render-mode" )

//
// Reading mixed types
//

//
// DxFont/string
//
bool MixedReadDxFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientDxFont*& pDxFontElement )
{
    pDxFontElement = NULL;
    if ( argStream.NextIsString () || argStream.NextIsNone () )
        return argStream.ReadString ( strFontName, szDefaultFontName );
    else
        return argStream.ReadClass( pDxFontElement, LUACLASS_DXFONT );
}


//
// GuiFont/string
//
bool MixedReadGuiFontString ( CScriptArgReader& argStream, SString& strFontName, const char* szDefaultFontName, CClientGuiFont*& pGuiFontElement )
{
    pGuiFontElement = NULL;
    if ( argStream.NextIsString () || argStream.NextIsNone () )
        return argStream.ReadString ( strFontName, szDefaultFontName );
    else
        return argStream.ReadClass( pGuiFontElement, LUACLASS_GUIFONT );
}


//
// Material/string
//
bool MixedReadMaterialString ( CScriptArgReader& argStream, CClientMaterial*& pMaterialElement )
{
    pMaterialElement = NULL;
    if ( !argStream.NextIsString () )
        return argStream.ReadClass( pMaterialElement, LUACLASS_MATERIAL );
    else
    {
        SString strFilePath;
        argStream.ReadString ( strFilePath );

        // If no element, auto find/create one
        CLuaMain* pLuaMain = CLuaFunctionDefs::lua_readcontext ( argStream.m_luaVM );
        CResource* pParentResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pParentResource )
        {
            CResource* pFileResource = pParentResource;
            filePath strPath;
            const char *meta;
            if ( g_pClientGame->GetResourceManager()->ParseResourceFullPath( (Resource*&)pFileResource, strFilePath, meta, strPath ) )
            {
                SString strUniqueName = SString( "%s*%s*%s", pParentResource->GetName().c_str(), pFileResource->GetName().c_str(), meta ).Replace ( "\\", "/" );
                pMaterialElement = g_pClientGame->GetManager()->GetRenderElementManager()->FindAutoTexture( strPath.c_str(), strUniqueName, *pParentResource->GetResourceDynamicEntity() );

                // ** CHECK  Should parent be pFileResource, and element added to pParentResource's ElementGroup? **
            }
        }
        return pMaterialElement != NULL;
    }
}

#endif //MTA_CLIENT
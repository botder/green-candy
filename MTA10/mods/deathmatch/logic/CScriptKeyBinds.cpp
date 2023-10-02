/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptKeyBinds.cpp
*  PURPOSE:     Key binds manager
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

SScriptBindableKey g_bkKeys [ NUMBER_OF_KEYS ] = 
{ 
    { "mouse1" }, { "mouse2" }, { "mouse3" }, { "mouse_wheel_up" }, { "mouse_wheel_down" },
    { "mouse4" }, { "mouse5" }, { "backspace" }, { "tab" }, { "lshift" }, { "rshift" },
    { "lctrl" }, { "rctrl" }, { "lalt" }, { "ralt" }, { "pause" }, { "capslock" },
    { "enter" }, { "space" }, { "pgup" }, { "pgdn" }, { "end" }, { "home" }, { "arrow_l" },
    { "arrow_u" }, { "arrow_r" }, { "arrow_d" }, { "insert" }, { "delete" }, { "0" }, 
    { "1" }, { "2" }, { "3" }, { "4" }, { "5" }, { "6" }, { "7" }, { "8" }, { "9" },
    { "a" }, { "b" }, { "c" }, { "d" }, { "e" }, { "f" }, { "g" }, { "h" }, { "i" }, 
    { "j" }, { "k" }, { "l" }, { "m" }, { "n" }, { "o" }, { "p" }, { "q" }, { "r" }, 
    { "s" }, { "t" }, { "u" }, { "v" }, { "w" }, { "x" }, { "y" }, { "z" }, { "num_0" }, 
    { "num_1" }, { "num_2" }, { "num_3" }, { "num_4" }, { "num_5" }, { "num_6" },
    { "num_7" }, { "num_8" }, { "num_9" }, { "num_mul" }, { "num_add" },
    { "num_sep" }, { "num_sub" }, { "num_dec" }, { "num_div" }, { "F1" }, { "F2" }, 
    { "F3" }, { "F4" }, { "F5" }, { "F6" }, { "F7" }, /* { "F8" },  * Used for console */
    { "F9" }, { "F10" }, { "F11" }, { "F12" }, { "scroll" }, { ";" }, { "=" }, { "," }, { "-" },
    { "." }, { "/" }, { "'" }, { "[" }, { "\\" }, { "]" },  { "#" },
    { "" }
};

SScriptBindableGTAControl g_bcControls[] =
{
    { "fire" }, { "next_weapon" }, { "previous_weapon" }, { "forwards" }, { "backwards" },
    { "left" }, { "right" }, { "zoom_in" }, { "zoom_out" }, { "enter_exit" },
    { "change_camera" }, { "jump" }, { "sprint" }, { "look_behind" }, { "crouch" },
    { "action" }, { "walk" }, { "vehicle_fire" }, { "vehicle_secondary_fire" },
    { "vehicle_left" }, { "vehicle_right" }, { "steer_forward" }, { "steer_back" },
    { "accelerate" }, { "brake_reverse" }, { "radio_next" }, { "radio_previous" },
    { "radio_user_track_skip" }, { "horn" }, { "sub_mission" }, { "handbrake" },
    { "vehicle_look_left" }, { "vehicle_look_right" }, { "vehicle_look_behind" },
    { "vehicle_mouse_look" }, { "special_control_left"  }, { "special_control_right" },
    { "special_control_down" }, { "special_control_up" }, { "aim_weapon" },
    { "conversation_yes" }, { "conversation_no" }, { "group_control_forwards" },
    { "group_control_back" }, { "" }
};

CScriptKeyFunctionBind::CScriptKeyFunctionBind( CLuaMain *lua, SScriptBindableKey *key ) : CScriptKeyBind( lua )
{
    m_key = key;

    g_pClientGame->GetScriptKeyBinds()->m_keyBinds.push_back( this );
}

CScriptKeyFunctionBind::~CScriptKeyFunctionBind()
{
    g_pClientGame->GetScriptKeyBinds()->m_keyBinds.remove( this );
}

CScriptControlFunctionBind::CScriptControlFunctionBind( CLuaMain *lua, SScriptBindableGTAControl *control ) : CScriptKeyBind( lua )
{
    m_control = control;

    g_pClientGame->GetScriptKeyBinds()->m_controlBinds.push_back( this );
}

CScriptControlFunctionBind::~CScriptControlFunctionBind()
{
    g_pClientGame->GetScriptKeyBinds()->m_controlBinds.remove( this );
}

CScriptKeyBinds::CScriptKeyBinds()
{
}

CScriptKeyBinds::~CScriptKeyBinds()
{
    Clear();
}

SScriptBindableKey* CScriptKeyBinds::GetBindableFromKey( const char *key )
{
    for ( unsigned int i = 0; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SScriptBindableKey *temp = &g_bkKeys[i];

        if ( stricmp( temp->szKey, key ) == 0 )
            return temp;
    }

    return NULL;
}

SScriptBindableGTAControl* CScriptKeyBinds::GetBindableFromControl( const char *control )
{
    for ( unsigned int i = 0 ; *g_bcControls[i].szControl != NULL; i++ )
    {
        SScriptBindableGTAControl *temp = &g_bcControls[i];

        if ( stricmp( temp->szControl, control ) == 0 )
            return temp;
    }

    return NULL;
}

bool CScriptKeyBinds::GetBindTypeFromName( const char *type, eBindStateType& bindType )
{
    if ( stricmp( type, "down" ) == 0 )
        bindType = STATE_DOWN;
    else if ( stricmp( type, "up" ) == 0 )
        bindType = STATE_UP;
    else if ( stricmp( type, "both" ) == 0 )
        bindType = STATE_BOTH;
    else
        return false;

    return true;
}

void CScriptKeyBinds::Clear( eScriptKeyBindType bindType )
{
    binds_t::iterator iter = m_List.begin();

    while ( iter != m_List.end() )
    {
        CScriptKeyBind *keyBind = *iter++;

        if ( bindType == SCRIPT_KEY_BIND_UNDEFINED || keyBind->GetType() == bindType )
            keyBind->Delete();
    }
}

bool CScriptKeyBinds::ProcessKey( const char *key, bool state, eScriptKeyBindType type )
{
    bool executed = false;

    if ( type == SCRIPT_KEY_BIND_FUNCTION )
    {
        SScriptBindableKey *info = GetBindableFromKey( key );

        if ( !info )
            return false;

        for ( keyBinds_t::iterator iter = m_keyBinds.begin(); iter != m_keyBinds.end(); ++iter )
        {
            if ( (*iter)->m_key != info )
                continue;

            if ( !(*iter)->CanCaptureState( state ) )
                continue;

            (*iter)->Execute( state );

            executed = true;
        }
    }
    else if ( type == SCRIPT_KEY_BIND_CONTROL_FUNCTION )
    {
        SScriptBindableGTAControl *info = GetBindableFromControl( key );

        if ( !info )
            return false;

        for ( controlBinds_t::iterator iter = m_controlBinds.begin(); iter != m_controlBinds.end(); ++iter )
        {
            if ( (*iter)->m_control != info )
                continue;

            if ( !(*iter)->CanCaptureState( state ) )
                continue;

            (*iter)->Execute( state );

            executed = true;
        }
    }

    return executed;
}

CScriptKeyBind* CScriptKeyBinds::AddKeyFunction( SScriptBindableKey *key, eBindStateType bindType, CLuaMain *lua, int argCount )
{
    CScriptKeyFunctionBind *keyBind = new CScriptKeyFunctionBind( lua, key );
    keyBind->m_bindType = bindType;
    keyBind->m_ref = lua->CreateReference( -argCount - 1 );
    keyBind->AcquireArguments( **lua, argCount );
    lua_pop( **lua, 1 );
    return keyBind;
}

CScriptKeyBind* CScriptKeyBinds::AddControlFunction( SScriptBindableGTAControl *control, eBindStateType bindType, CLuaMain *lua, int argCount )
{
    CScriptControlFunctionBind *ctrlBind = new CScriptControlFunctionBind( lua, control );
    ctrlBind->m_bindType = bindType;
    ctrlBind->m_ref = lua->CreateReference( -argCount - 1 );
    ctrlBind->AcquireArguments( **lua, argCount );
    lua_pop( **lua, 1 );
    return ctrlBind;
}

CScriptKeyBind* CScriptKeyBinds::GetKeyFunction( SScriptBindableKey *key, CLuaMain *lua, eBindStateType bindType, const void *routine )
{
    for ( keyBinds_t::iterator iter = m_keyBinds.begin(); iter != m_keyBinds.end(); ++iter )
    {
        CScriptKeyFunctionBind *bind = *iter;

        if ( bind->m_key == key && bind->m_lua == lua && bind->m_bindType == bindType && bind->m_ref.GetPointer() == routine )
            return bind;
    }

    return NULL;
}

CScriptKeyBind* CScriptKeyBinds::GetControlFunction( SScriptBindableGTAControl *control, CLuaMain *lua, eBindStateType bindType, const void *routine )
{
    for ( controlBinds_t::iterator iter = m_controlBinds.begin(); iter != m_controlBinds.end(); ++iter )
    {
        CScriptControlFunctionBind *bind = *iter;

        if ( bind->m_control == control && bind->m_lua == lua && bind->m_bindType == bindType && bind->m_ref.GetPointer() == routine )
            return bind;
    }

    return NULL;
}

void CScriptKeyBinds::RemoveAllKeys( CLuaMain *lua )
{
    binds_t::iterator iter = m_List.begin();

    while ( iter != m_List.end() )
    {
        CScriptKeyBind *keyBind = *iter++;

        if ( keyBind->m_lua != lua )
            continue;

        keyBind->Delete();
    }
}
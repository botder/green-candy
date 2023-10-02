/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CScriptKeyBinds.h
*  PURPOSE:     Header for script key binds class
*  DEVELOPERS:  Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CScriptKeyBinds_H
#define __CScriptKeyBinds_H

#define NUMBER_OF_KEYS 123

#include "luadefs/luakeybind.h"

struct SScriptBindableKey
{
    char szKey[20];
};

struct SScriptBindableGTAControl
{
    char szControl[25];
};

class CScriptKeyFunctionBind : public CScriptKeyBind
{
public:
                                    CScriptKeyFunctionBind( CLuaMain *lua, SScriptBindableKey *key );
                                    ~CScriptKeyFunctionBind();

    inline eScriptKeyBindType       GetType() const                                         { return SCRIPT_KEY_BIND_FUNCTION; }

    const char*                     GetKeyName() const                                      { return m_key->szKey; }

    SScriptBindableKey*             m_key;
};

class CScriptControlFunctionBind : public CScriptKeyBind
{
public:
                                    CScriptControlFunctionBind( CLuaMain *lua, SScriptBindableGTAControl *control );
                                    ~CScriptControlFunctionBind();

    inline eScriptKeyBindType       GetType() const                                         { return SCRIPT_KEY_BIND_CONTROL_FUNCTION; }

    const char*                     GetKeyName() const                                      { return m_control->szControl; }

    SScriptBindableGTAControl*      m_control;
};

class CScriptKeyBinds
{
    friend class CScriptKeyBind;
    friend class CScriptKeyFunctionBind;
    friend class CScriptControlFunctionBind;
public:
                                    CScriptKeyBinds();
                                    ~CScriptKeyBinds();

    // Shared functions for inline function access
    static SScriptBindableKey*          GetBindableFromKey( const char *key );
    static SScriptBindableGTAControl*   GetBindableFromControl( const char *control );
    static bool                         GetBindTypeFromName( const char *type, eBindStateType& bindType );

    // Basic funcs
    void                            Clear( eScriptKeyBindType bindType = SCRIPT_KEY_BIND_UNDEFINED );
    bool                            ProcessKey( const char *key, bool state, eScriptKeyBindType type );

    typedef std::list <CScriptKeyBind*> binds_t;
    typedef std::list <CScriptKeyFunctionBind*> keyBinds_t;
    typedef std::list <CScriptControlFunctionBind*> controlBinds_t;

    CScriptKeyBind*                 AddKeyFunction( SScriptBindableKey *key, eBindStateType bindType, CLuaMain *lua, int argCount );
    CScriptKeyBind*                 AddControlFunction( SScriptBindableGTAControl *control, eBindStateType bindType, CLuaMain *lua, int argCount );

    CScriptKeyBind*                 GetKeyFunction( SScriptBindableKey *key, CLuaMain *lua, eBindStateType bindType, const void *routine );
    CScriptKeyBind*                 GetControlFunction( SScriptBindableGTAControl *control, CLuaMain *lua, eBindStateType bindType, const void *routine );

    void                            RemoveAllKeys( CLuaMain *lua );

    binds_t                         m_List;
    keyBinds_t                      m_keyBinds;
    controlBinds_t                  m_controlBinds;
};

#endif

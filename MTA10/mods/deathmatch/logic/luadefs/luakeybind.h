/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/luakeybind.h
*  PURPOSE:     Keybind Lua instancing
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_KEYBIND_
#define _LUA_KEYBIND_

#define LUACLASS_KEYBIND    100

enum eScriptKeyBindType
{
    SCRIPT_KEY_BIND_FUNCTION = 0,
    SCRIPT_KEY_BIND_CONTROL_FUNCTION,
    SCRIPT_KEY_BIND_UNDEFINED,
};

enum eBindStateType : unsigned char
{
    STATE_DOWN,
    STATE_UP,
    STATE_BOTH
};

class CScriptKeyBind abstract : public LuaClass
{
public:
                                    CScriptKeyBind( CLuaMain *lua );
    virtual                         ~CScriptKeyBind();

    virtual const char*             GetKeyName() const = 0;
    virtual eScriptKeyBindType      GetType() const = 0;

    void                            AcquireArguments( lua_State *L, int argCount );
    void                            Execute( bool state );

    inline bool CanCaptureState( bool state ) const
    {
        switch( m_bindType )
        {
        case STATE_DOWN:
            return state;
        case STATE_UP:
            return !state;
        }

        return true;
    }

    eBindStateType                  m_bindType;
    CLuaMain*                       m_lua;

    LuaFunctionRef                  m_ref;
};

#endif //_LUA_KEYBIND_
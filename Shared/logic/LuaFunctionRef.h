/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaFunctionRef.h
*  PURPOSE:     Lua function reference
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_FUNCTIONREF_
#define _BASE_LUA_FUNCTIONREF_

#define LUA_REFNIL              (-1)
#define VERIFY_FUNCTION(func)   ( (func).ToInt () != LUA_REFNIL )
#define IS_REFNIL(func)         ( (func).ToInt () == LUA_REFNIL )

class LuaMain;
class LuaManager;

class LuaFunctionRef
{
    friend class LuaMain;
    friend class LuaManager;
public:
                        LuaFunctionRef();
                        LuaFunctionRef( LuaMain *lua, int ref, const void *call );
                        LuaFunctionRef( const LuaFunctionRef& other );
                        ~LuaFunctionRef();

    int                 ToInt() const;
    const void*         GetPointer() const    { return m_call; }
    void                Push( lua_State *L );

    LuaFunctionRef&     operator = ( const LuaFunctionRef& other );
    bool                operator == ( const LuaFunctionRef& other ) const;
    bool                operator != ( const LuaFunctionRef& other ) const;

protected:
    class LuaMain*      m_lua;
    int                 m_ref;
    const void*         m_call;
};

#endif //_BASE_LUA_FUNCTIONREF_

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaElement.h
*  PURPOSE:     Lua internalization management entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_ELEMENT_
#define _BASE_LUA_ELEMENT_

#define LUACLASS_ELEMENT 120

class Resource;

class LuaElement : public LuaClass
{
                            // Do not allow copy
                            LuaElement( const LuaElement& elem );
public:
                            LuaElement( lua_State *L );
                            ~LuaElement();

    void                    SetRoot( LuaClass *root );

    LuaClass*               m_root;
};

#endif //_BASE_LUA_ELEMENT_
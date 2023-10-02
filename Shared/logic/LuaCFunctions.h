/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaCFunctions.h
*  PURPOSE:     Base Lua function management
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASIC_LUA_FUNCTIONS_
#define _BASIC_LUA_FUNCTIONS_

class LuaCFunction
{
public:
                                LuaCFunction( const char *name, lua_CFunction proto, bool restrict );

    inline lua_CFunction        GetAddress() const                      { return m_proto; }

    const std::string&          GetName() const                         { return m_name; }
    void                        SetName( const std::string& name )      { m_name = name; }

    bool                        IsRestricted() const                    { return m_restrict; }

private:
    lua_CFunction               m_proto;
    std::string                 m_name;
    bool                        m_restrict;
};

namespace LuaCFunctions
{
    void            Init();
    void            Destroy();

    LuaCFunction*   AddFunction( const char *name, lua_CFunction proto, bool restrict = false );
    LuaCFunction*   GetFunction( lua_CFunction proto );
    LuaCFunction*   GetFunction( const char *name );

    void            RegisterFunctionsWithVM( lua_State* luaVM );

    void            RemoveAllFunctions();

    void            InitializeHashMaps();
};

#endif //_BASIC_LUA_FUNCTIONS_

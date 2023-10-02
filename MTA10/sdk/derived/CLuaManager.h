/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CLuaManager.h
*  PURPOSE:     Derived lua hyperstructure management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_MANAGER_
#define _LUA_MANAGER_

class CLuaManager : public LuaManager
{
public:
                                    CLuaManager();
                                    ~CLuaManager();

    void                            GarbageCollect( lua_State *L );
    bool                            OnLuaClassDeallocationFail( lua_State *L, ILuaClass *j );

    CLuaMain*                       Create( const std::string& name, CFileTranslator& fileRoot );
    bool                            Remove( LuaMain *lua );

    inline CScriptDebugging&        GetDebug()          { return m_debug; }
    inline CRegisteredCommands&     GetCommands()       { return m_commands; }

    const CLuaMain*                 GetStatus( int *line, std::string *src, std::string *proto_name )    { return (const CLuaMain*)LuaManager::GetStatus( line, src, proto_name ); }

protected:
    CRegisteredCommands m_commands;
    CScriptDebugging    m_debug;
};

#endif //_LUA_MANAGER_
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/RegisteredCommands.h
*  PURPOSE:     Console command registry
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_REGISTERED_COMMANDS_
#define _BASE_REGISTERED_COMMANDS_

class LuaMain;
class LuaManager;
class RegisteredCommands;

class Command : public LuaElement
{
public:
    Command( RegisteredCommands& cmds, lua_State *L ) : LuaElement( L ), manager( cmds )
    {
    }

    ~Command();

    virtual bool Execute( std::vector <std::string>& args );

    RegisteredCommands& manager;
    LuaMain*        lua;
    std::string     key;
    LuaFunctionRef  ref;
    bool            caseSensitive;
};

class RegisteredCommands
{
    friend class Command;
public:
                                        RegisteredCommands( LuaManager& manager );
    virtual                             ~RegisteredCommands();

    bool                                Remove( LuaMain *lua, const std::string& key );
    Command*                            Get( const char *key, LuaMain *lua = NULL );
    void                                Clear();
    void                                CleanUp( LuaMain *lua );

    bool                                Exists( const char *key, LuaMain *lua = NULL );

    typedef std::list <Command*> commandList_t;
    std::list <Command*>    m_commands;

    static int luaconstructor_command( lua_State *L );

protected:    
    LuaManager&     m_system;
};

namespace CommandlineTools
{
    static inline bool stradjust( const char*& msg, char delim )
    {
        if ( *msg == 0 )
            return false;

        while ( *msg != delim )
        {
            if ( *msg == 0 )
                return true;

            msg++;
        }

        return true;
    }

    static inline bool strgettok( const char*& msg, char delim, std::string& out )
    {
        const char *dpass = msg;

        if ( !stradjust( msg, delim ) )
            return false;

        out = std::string( dpass, (size_t)( msg - dpass ) );

        if ( *msg != 0 )
            msg++;

        return true;
    }

    static inline void strsplit( const char *msg, std::vector <std::string>& out, const char seperator = ' ' )
    {
        std::string item;

        while ( strgettok( msg, seperator, item ) )
        {
            if ( item.size() == 0 )
                continue;

            out.push_back( item );
        }
    }
}

#endif //_BASE_REGISTERED_COMMANDS_
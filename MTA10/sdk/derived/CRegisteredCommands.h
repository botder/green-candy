/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CRegisteredCommands.h
*  PURPOSE:     Extended Console command registry
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _REGISTERED_COMMANDS_
#define _REGISTERED_COMMANDS_

class CCommand : public Command
{
public:
    CCommand( class CRegisteredCommands& cmds, lua_State *L );
    ~CCommand();

    bool Execute( std::vector <std::string>& args );
};

class CRegisteredCommands : public RegisteredCommands
{
public:
                                CRegisteredCommands( class CLuaManager& manager );
                                ~CRegisteredCommands();

    bool                        Add( LuaMain& lua, const std::string& key, const LuaFunctionRef& ref, bool caseSensitive );
};

#endif //_REGISTERED_COMMANDS_
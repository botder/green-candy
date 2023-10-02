/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTaskDefs.cpp
*  PURPOSE:     Lua task definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

namespace CLuaFunctionDefs
{
    void LoadTaskFunctions()
    {
        // ChrML: Disabled for dp3
        /*
        CLuaCFunctions::AddFunction ( "createTaskInstance", CLuaTaskDefs::createTaskInstance );

        CLuaCFunctions::AddFunction ( "getTaskName", CLuaTaskDefs::getTaskName );

        CLuaCFunctions::AddFunction ( "getTaskParameter", CLuaTaskDefs::getTaskParameter );
        CLuaCFunctions::AddFunction ( "getTaskParameters", CLuaTaskDefs::getTaskParameters );
        CLuaCFunctions::AddFunction ( "setTaskParameters", CLuaTaskDefs::setTaskParameters );
        CLuaCFunctions::AddFunction ( "clearTaskParameters", CLuaTaskDefs::clearTaskParameters );

        CLuaCFunctions::AddFunction ( "runTaskFunction", CLuaTaskDefs::runTaskFunction );

        CLuaCFunctions::AddFunction ( "setPlayerTask", CLuaTaskDefs::setPlayerTask );
        */
    }

#if 0

    LUA_DECLARE( createTaskInstance )
    {
        // taskinstance createTaskInstance ( string taskname, table parameters )
        // returns taskinstance or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TSTRING ) &&
             argtype ( 2, LUA_TTABLE ) )
        {
            // Grab the task name
            CClientTask Task ( m_pManager );
            const char* szTaskName = lua_tostring ( L, 1 );
            Task.SetTaskName ( szTaskName );

            // Generate an unique identifier
            Task.SetUniqueIdentifier ( CClientTask::GenerateUniqueIdentifier () );

            // Read out the task parameters
            if ( Task.ReadParameters ( L, 2, true ) )
            {
                // Just return the task data as a table
                lua_newtable ( L );
                Task.Write ( L, -1 );
                return 1;
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTaskName )
    {
        // string getTaskName ( taskinstance task )
        // returns a string or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TTABLE ) )
        {
            // Read out the task data
            CClientTask Task ( m_pManager );
            if ( Task.Read ( L, 1, true ) )
            {
                // Return it
                lua_pushstring ( L, Task.GetTaskName () );
                return 1;
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTaskParameter )
    {
        // string getTaskParameter ( taskinstance task, string key )
        // returns a string or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TTABLE ) &&
             argtype ( 2, LUA_TSTRING ) )
        {
            // Read out the task data
            CClientTask Task ( m_pManager );
            if ( Task.Read ( L, 1, true ) )
            {
                // Read out the key string
                const char* szKey = lua_tostring ( L, 2 );

#if 0
                // Grab the parameter
                CLuaArgument* pValue = Task.GetParameter ( szKey );
                if ( pValue )
                {
                    // Return it
                    pValue->Push ( L );
                    return 1;
                }
#endif
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTaskParameters )
    {
        // table getTaskParameters ( taskinstance task )
        // returns a table or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TTABLE ) )
        {
            // Read out the task data
            CClientTask Task ( m_pManager );
            if ( Task.Read ( L, 1, true ) )
            {
                // Write the parameters and return
                lua_newtable ( L );
                Task.WriteParameters ( L, -1 );
                return 1;
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTaskParameters )
    {
        // bool setTaskParameters ( taskinstance task, table newparameters )
        // returns true on success or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TTABLE ) &&
             argtype ( 2, LUA_TTABLE ) )
        {
            // Read out the task data
            CClientTask Task ( m_pManager );
            if ( Task.Read ( L, 1, true ) )
            {
                // Read the new parameters into it in addition to the old
                Task.ReadParameters ( L, 2, false );

                // Write them back to the table
                Task.Write ( L, 1 );

                // Success
                lua_pushboolean ( L, true );
                return 1;
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( clearTaskParameters )
    {
        // bool clearTaskParameters ( taskinstance task )
        // returns true on success or false on failure

        // Verify types
        if ( argtype ( 1, LUA_TTABLE ) )
        {
            // Read out the task data
            CClientTask Task ( m_pManager );
            if ( Task.Read ( L, 1, true ) )
            {
                // Read the new parameters into it in addition to the old
                // and write them back into the table.
                Task.ClearParameters ();
                Task.Write ( L, 1 );

                // Success
                lua_pushboolean ( L, true );
                return 1;
            }
        }

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( runTaskFunction )
    {
        return 0;
    }

    LUA_DECLARE( getPlayerTaskInstance )
    {
        return 0;
    }

    LUA_DECLARE( setPlayerTask )
    {
        // bool setPlayerTask ( ped thePed, taskinstance task )
        // returns true on success or false on failure

        CClientPed *ped;

        CScriptArgReader argStream( L );

        argStream.ReadClass( ped, LUACLASS_PED );

        // Verify types
        if ( !argStream.HasErrors() )
        {
            if ( lua_type( L, 2 ) == LUA_TTABLE )
            {
                // Read out the task data
                CClientTask Task( m_pManager );

                // Apply it on the player
                lua_pushboolean( L, Task.Read( L, 2, true ) && Task.ApplyTask( *ped ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

#endif //0
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CRegisteredCommands.cpp
*  PURPOSE:     Registered (lua) command manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Jax <>
*               Oliver Brown <>
*               Alberto Alonso <rydencillo@gmail.com>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CRegisteredCommands::CRegisteredCommands( LuaManager& manager, CAccessControlListManager* pACLManager ) : RegisteredCommands( manager )
{
    m_pACLManager = pACLManager;
    m_bIteratingList = false;
}

CRegisteredCommands::~CRegisteredCommands()
{
    ClearCommands();
}

bool CRegisteredCommands::AddCommand ( CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bRestricted, bool bCaseSensitive )
{
    // Check if we already have this key and handler
    SCommand* pCommand = GetCommand ( szKey, pLuaMain );
    if ( pCommand &&
         iLuaFunction == pCommand->iLuaFunction )
    {
        return false;
    }

    // Create the entry
    pCommand = new SCommand;
    pCommand->pLuaMain = pLuaMain;
    strncpy ( pCommand->szKey, szKey, MAX_REGISTERED_COMMAND_LENGTH );
    pCommand->szKey [MAX_REGISTERED_COMMAND_LENGTH] = 0;
    pCommand->iLuaFunction = iLuaFunction;
    pCommand->bRestricted = bRestricted;
    pCommand->bCaseSensitive = bCaseSensitive;

    // Add it to our list
    m_Commands.push_back ( pCommand );
    return true;
}

bool CRegisteredCommands::ProcessCommand ( const char* szKey, const char* szArguments, CClient* pClient )
{
    // Call the handler for every virtual machine that matches the given key
    bool bHandled = false;
    m_bIteratingList = true;
    list < SCommand* > ::const_iterator iter = m_Commands.begin ();
    int iCompareResult;

    for ( ; iter != m_Commands.end (); iter++ )
    {
        if ( (*iter)->bCaseSensitive )
            iCompareResult = strcmp ( (*iter)->szKey, szKey );
        else
            iCompareResult = stricmp ( (*iter)->szKey, szKey );

        // Matching names?
        if ( iCompareResult == 0 )
        {
            if ( m_pACLManager->CanObjectUseRight ( pClient->GetAccount ()->GetName ().c_str (),
                                                    CAccessControlListGroupObject::OBJECT_TYPE_USER,
                                                    szKey,
                                                    CAccessControlListRight::RIGHT_TYPE_COMMAND,
                                                    !(*iter)->bRestricted ) )   // If this command is restricted, the default access should be false unless granted specially
            {
                // Call it
                CallCommandHandler ( (*iter)->pLuaMain, (*iter)->iLuaFunction, szKey, szArguments, pClient );
                bHandled = true;
            }
        }
    }
    m_bIteratingList = false;
    TakeOutTheTrash ();

    // Return whether some handler was called or not
    return bHandled;
}

void CRegisteredCommands::CallCommandHandler ( CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments, CClient* pClient )
{
    CLuaArguments Arguments;
    // First, try to call a handler with the same number of arguments
    if ( pClient )
    {
        switch ( pClient->GetClientType () )
        {
            case CClient::CLIENT_PLAYER:
            {
                Arguments.PushElement ( static_cast < CPlayer* > ( pClient ) );
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                Arguments.PushElement ( static_cast < CConsoleClient* > ( pClient ) );
                break;
            }
            default:
            {
                Arguments.PushBoolean ( false );
                break;
            }
        }
    }
    else
    {
        Arguments.PushBoolean ( false );
    }

    Arguments.PushString ( szKey );

    if ( szArguments )
    {
        // Create a copy and strtok modifies the string
        char * szTempArguments = new char[strlen(szArguments) + 1];
        strcpy ( szTempArguments, szArguments );
        char * arg;
        arg = strtok ( szTempArguments, " " );
        while ( arg )
        {
            Arguments.PushString ( arg );
            arg = strtok ( NULL, " " );
        }
        delete [] szTempArguments;
    }

    // Call the handler with the arguments we pushed
    Arguments.Call ( pLuaMain, iLuaFunction );
}

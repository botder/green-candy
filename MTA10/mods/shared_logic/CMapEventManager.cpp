/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEventManager.cpp
*  PURPOSE:     Map event manager class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapEventManager::CMapEventManager( CClientEntity *owner )
{
    m_owner = owner;
}

CMapEventManager::~CMapEventManager()
{
    // Delete all eventhandlers
    DeleteAll();
}

bool CMapEventManager::Add( CLuaMain *main, const char *name, const LuaFunctionRef& ref, bool propagated )
{
    // Check for max name length
    if ( strlen( name ) > MAPEVENT_MAX_LENGTH_NAME )
        return false;

    // Make a new event
    CMapEvent *pEvent = new CMapEvent( main, *this, name, ref, propagated );

    LIST_APPEND( m_list.root, pEvent->m_node );
    return true;
}

bool CMapEventManager::Delete( CLuaMain *main, const char *name, const LuaFunctionRef *ref )
{
    luaRefs refs;
    RwListEntry <CMapEvent> *iter = m_list.root.next;

    for ( ; iter != &m_list.root; iter = iter->next )
    {
        CMapEvent *pMapEvent = LIST_GETITEM( CMapEvent, iter, m_node );

        if ( main != pMapEvent->GetVM() )
            continue;

        // If we have a name, delete only events which match it
        if ( name && strcmp( pMapEvent->GetName(), name ) != 0 )
            continue;

        if ( ref && *ref != pMapEvent->GetLuaFunction() )
            continue;

        // Reference and mark it for deletion
        pMapEvent->Reference( refs );
        pMapEvent->Delete();
    }

    // Return whether we actually destroyed someone or not
    return !refs.empty();
}

void CMapEventManager::DeleteAll()
{
    // Delete all the events
    // We should not be referencing the event here
    // Otherwise the freeze is expected
    RwListEntry <CMapEvent> *iter;

    while ( ( iter = m_list.root.next ) != &m_list.root )
        LIST_GETITEM( CMapEvent, iter, m_node )->Delete();
}

CMapEvent* CMapEventManager::Get( const char *name )
{
    // Return it if we find it in the list
    LIST_FOREACH_BEGIN( CMapEvent, m_list.root, m_node )
        // Compare the names
        if ( strcmp( item->GetName(), name ) == 0 )
            return item;
    LIST_FOREACH_END

    return NULL;
}

bool CMapEventManager::Call( lua_State *callee, unsigned int argCount, const char *name, CClientEntity *source )
{
    luaRefs refs;

    // Call all the events with matching names
    LIST_FOREACH_BEGIN( CMapEvent, m_list.root, m_node )
        // Compare the names
        if ( strcmp( item->GetName(), name ) == 0 )
        {
            // Call if propagated?
            if ( source == m_owner || item->IsPropagated() )
            {
                // Grab the event's executive environment
                CLuaMain *main = item->GetVM();
                lua_State *L = **main;
                int top = lua_gettop( L );

                TIMEUS startTime = GetTimeUs();

                // Make sure our stack has got enough size
                lua_checkstack( L, 6 + argCount );

                // Store globals
                lua_getfield( L, LUA_GLOBALSINDEX, "source" );
                lua_getfield( L, LUA_GLOBALSINDEX, "this" );
                lua_getfield( L, LUA_GLOBALSINDEX, "sourceResource" );
                lua_getfield( L, LUA_GLOBALSINDEX, "sourceResourceRoot" );
                lua_getfield( L, LUA_GLOBALSINDEX, "eventName" );

                // Init globals
                source->PushStack( L );                                                             lua_setglobal( L, "source" );
                m_owner->PushStack( L );                                                            lua_setglobal( L, "this" );
                main->GetResource()->PushStack( L );                                                lua_setglobal( L, "sourceResource" );
                main->GetResource()->GetResourceDynamicEntity()->PushStack( L );                    lua_setglobal( L, "sourceResourceRoot" );
                lua_pushstring( L, name );                                                          lua_setglobal( L, "eventName" );

                // Reference it to prevent destruction
                item->Reference( refs );

                // Get it's function
                main->PushReference( item->m_funcRef );

                // Grab the arguments
                if ( callee != L )
                    lua_xcopy( callee, L, argCount );   // perform a quick copy
                else
                {
                    // We need to snatch the arguments from below
                    unsigned int n = argCount;

                    while ( n-- )
                        lua_pushvalue( L, top - n );
                }

                // Call it
                main->PCallStackVoid( argCount );

                // Reset globals
                lua_setfield( L, LUA_GLOBALSINDEX, "eventName" );
                lua_setfield( L, LUA_GLOBALSINDEX, "sourceResourceRoot" );            
                lua_setfield( L, LUA_GLOBALSINDEX, "sourceResource" );
                lua_setfield( L, LUA_GLOBALSINDEX, "this" );
                lua_setfield( L, LUA_GLOBALSINDEX, "source" );

#if MTA_DEBUG
                assert( lua_gettop( L ) == top );
#endif

                CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming( main, name, GetTimeUs() - startTime );
            }
        }
    LIST_FOREACH_END

    // Return whether we called atleast one func or not
    return !refs.empty();
}
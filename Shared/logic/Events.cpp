/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/Events.cpp
*  PURPOSE:     Lua events management
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

Events::Events()
{
    m_wasCancelled = false;
    m_eventCancelled = false;
}

bool Events::Add( const char *name, const char *arguments, LuaMain *lua, bool allowRemote )
{
    // If it already exists, return
    if ( Get( name ) )
        return false;

    // Create and add the event
    Event& evt = *new Event;
    evt.name = name;
    evt.arguments = arguments;
    evt.lua = lua;
    evt.allowRemote = allowRemote;

    m_events.push_back( &evt );
    return true;
}

void Events::Remove( Event *evt )
{
    // Remove it and delete it
    m_events.remove( evt );

    delete evt;
}

void Events::Remove( const char *name )
{
    // Find it
    Event *evt = Get( name );

    if ( !evt )
        return;

    Remove( evt );
}

void Events::RemoveAll( LuaMain *lua )
{
    // Delete all items
    eventList_t::iterator iter = m_events.begin();

    while ( iter != m_events.end() )
    {
        // If they match, delete it null it and set the bool
        if ( (*iter)->lua != lua )
        {
            ++iter;
            continue;
        }

        // Delete the object
        delete *iter;

        // Remove from list
        iter = m_events.erase( iter );
    }
}

void Events::RemoveAll()
{
    // Delete all items
    eventList_t::const_iterator iter = m_events.begin();

    for ( ; iter != m_events.end(); ++iter )
        delete *iter;

    // Clear the list
    m_events.clear();
}

Event* Events::Get( const char *name )
{
    // Find a matching name
    eventList_t::const_iterator iter = m_events.begin();

    for ( ; iter != m_events.end(); ++iter )
    {
        if ( (*iter)->name != name )
            continue;

        return *iter;
    }

    // None matches
    return NULL;
}

void Events::PrePulse()
{
    m_cancelList.push_back( m_eventCancelled );

    m_eventCancelled = false;
    m_wasCancelled = false;
    m_lastError = "";
}

void Events::PostPulse()
{
    m_wasCancelled = m_eventCancelled;
    m_eventCancelled = m_cancelList.back();

    m_cancelList.pop_back();
}

void Events::Cancel( bool cancel )
{
    m_eventCancelled = cancel;
}

void Events::Cancel( bool cancel, const char *reason )
{
    Cancel( cancel );

    m_lastError = reason;
}

bool Events::WasCancelled()
{
    return m_wasCancelled;
}

const char* Events::GetLastError()
{
    return m_lastError.c_str();
}


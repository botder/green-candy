/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/Events.h
*  PURPOSE:     Lua events management
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_EVENTS_
#define _BASE_EVENTS_

class LuaMain;

struct Event
{
    LuaMain*            lua;
    std::string         name;
    std::string         arguments;
    bool                allowRemote;
};

class Events
{
public:
                                Events();
    inline                      ~Events()                   { RemoveAll(); };

    bool                        Add( const char *name, const char *args, LuaMain *lua, bool allowRemote );
    void                        Remove( Event *evt );
    void                        Remove( const char *name );
    void                        RemoveAll( LuaMain *lua );
    void                        RemoveAll();

    inline bool                 Exists( const char *name )  { return Get( name ) != NULL; };
    Event*                      Get( const char *name );

    typedef std::list <Event*> eventList_t;

    eventList_t::const_iterator   IterBegin()               { return m_events.begin(); };
    eventList_t::const_iterator   IterEnd()                 { return m_events.end(); };

    void                        PrePulse();
    void                        PostPulse();

    void                        Cancel( bool cancel = true );
    void                        Cancel( bool cancel, const char *reason );
    bool                        WasCancelled();
    const char*                 GetLastError();

private:
    eventList_t                 m_events;

    std::vector <bool>          m_cancelList;
    bool                        m_eventCancelled;
    bool                        m_wasCancelled;

    std::string                 m_lastError;
};

#endif //_BASE_EVENTS_
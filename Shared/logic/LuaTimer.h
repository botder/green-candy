/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaTimer.h
*  PURPOSE:     Lua timed execution
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Florian Busse <flobu@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_TIMER_
#define _BASE_LUA_TIMER_

#include <chrono>

#define LUACLASS_TIMER  1

#define LUA_TIMER_MIN_INTERVAL      50

class LuaTimer : public LuaClass
{
public:
    typedef std::chrono::system_clock::time_point time_point;
    typedef std::chrono::system_clock::duration duration;

                            LuaTimer( lua_State *L, class LuaTimerManager *manager, const LuaFunctionRef& ref );
    virtual                 ~LuaTimer();

    time_point              GetStartTime() const                        { return m_startTime; };
    inline void             SetStartTime( time_point startTime )        { m_startTime = startTime; };

    duration                GetDelay() const                            { return m_delay; };
    inline void             SetDelay( duration delay )                  { m_delay = delay; };

    inline unsigned int     GetRepeats() const                          { return m_repCount; };
    inline void             SetRepeats( unsigned int repCount )         { m_repCount = repCount; }

    void                    ObtainArguments( lua_State *L, int idx = 1 );
    void                    Execute( class LuaMain& main );

    duration                GetTimeLeft();

private:
    LuaTimerManager*        m_manager;
    LuaFunctionRef          m_ref;
    time_point              m_startTime;
    duration                m_delay;
    unsigned int            m_repCount;
};

#endif //_BASE_LUA_TIMER_
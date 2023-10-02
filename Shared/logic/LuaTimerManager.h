/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaTimerManager.h
*  PURPOSE:     Lua timer manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_TIMER_MANAGER_
#define _BASE_LUA_TIMER_MANAGER_

#include <chrono>

class LuaTimerManager
{
    friend class LuaTimer;
public:
    typedef std::chrono::system_clock::time_point time_point;
    typedef std::chrono::system_clock::duration duration;
    
    inline                      LuaTimerManager()                       {}
    inline                      ~LuaTimerManager()                      { RemoveAllTimers(); };

    void                        DoPulse( LuaMain& main );

    bool                        Exists( LuaTimer *timer );
    LuaTimer*                   GetTimer( unsigned int id );

    LuaTimer*                   AddTimer( lua_State *L, const LuaFunctionRef& ref, duration delay, unsigned int repCount );
    void                        RemoveTimer( LuaTimer *timer );
    void                        RemoveAllTimers();
    size_t                      GetTimerCount() const                   { return m_list.size(); }

    void                        ResetTimer( LuaTimer *timer );

    void                        GetTimers( duration timeLeft, LuaMain *main );

private:
    std::list <LuaTimer*>     m_list;
};

#endif //_BASE_LUA_TIMER_MANAGER
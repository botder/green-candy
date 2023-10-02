/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CLuaTimer.h
*  PURPOSE:     Lua timed execution
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUA_TIMER_
#define _LUA_TIMER_

class CLuaTimer : public LuaTimer
{
public:
    CLuaTimer( lua_State *L, LuaTimerManager *manager, const LuaFunctionRef& ref ) : LuaTimer( L, manager, ref );
    {
    }

    ~CLuaTimer()
    {
    }
};

#endif //_LUA_TIMER_
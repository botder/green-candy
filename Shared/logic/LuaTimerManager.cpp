/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaTimerManager.cpp
*  PURPOSE:     Lua timer manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Marcus Bauer <mabako@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

void LuaTimerManager::DoPulse( LuaMain& main )
{
    LuaTimer::time_point curTime = std::chrono::system_clock::now();
    luaRefs refs;

    std::list <LuaTimer*>::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        LuaTimer *timer = *iter;

        // We have to reference the timer to keep it
        timer->Reference( refs );

        if ( curTime < ( timer->GetStartTime() + timer->GetDelay() ) )
            continue;

        timer->Execute( main );
        timer->SetStartTime( curTime );
    }

    // the "trash" will be taken out by lua here, in case the timer is not being used anywhere else
}

void LuaTimerManager::RemoveTimer( LuaTimer *timer )
{
    // This can either destroy it right away, or wait till unreferenced
    timer->Destroy();
}

void LuaTimerManager::RemoveAllTimers()
{
    luaRefs refs;
    std::list <LuaTimer*>::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        // We may not delete it right away, as it clears itself from list
        (*iter)->Reference( refs );
        (*iter)->Destroy();
    }
}

void LuaTimerManager::ResetTimer( LuaTimer *timer )
{
    timer->SetStartTime( std::chrono::system_clock::now() );
}

bool LuaTimerManager::Exists( LuaTimer *timer )
{
    auto findIter = std::find( m_list.begin(), m_list.end(), timer );

    return ( findIter != m_list.end() );
}

LuaTimer* LuaTimerManager::AddTimer( lua_State *L, const LuaFunctionRef& ref, duration delay, unsigned int repCount )
{
    // Check for the minimum interval
    if ( delay.count() < LUA_TIMER_MIN_INTERVAL )
        return NULL;

    if ( !VERIFY_FUNCTION( ref ) )
        return NULL;

    // Add the timer
    LuaTimer *timer = new LuaTimer( L, this, ref );
    timer->SetStartTime( std::chrono::system_clock::now() );
    timer->SetDelay( delay );
    timer->SetRepeats( repCount );
    m_list.push_back( timer );
    return timer;
}

void LuaTimerManager::GetTimers( duration timeLeft, LuaMain *main )
{
    // We expect a table on the stack
    time_point curTime = std::chrono::system_clock::now();
    std::list <LuaTimer*>::const_iterator iter = m_list.begin();
    size_t n = 0;

    for ( ; iter != m_list.end(); ++iter )
    {
        LuaTimer *currentTimer = *iter;

        // If the time left is less than the time specified, or the time specifed is 0
        if ( timeLeft.count() != 0 && ( currentTimer->GetStartTime() - curTime ) + currentTimer->GetDelay() > timeLeft )
            continue;

        lua_State *L = main->GetVirtualMachine();
        lua_pushwideinteger( L, (lua_WideInteger)( ++n ) );
        (*iter)->PushStack( L );
        lua_settable( L, -3 );
    }
}

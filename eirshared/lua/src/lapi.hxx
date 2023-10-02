#ifndef _LUA_API_INTERNALS_
#define _LUA_API_INTERNALS_

#include "lstate.h"

inline bool hascallingenv( lua_State *L )
{
    return ( L->ciStack.GetUsageCount( L ) != 1 );
}

inline GCObject *getcurrenv( lua_State *L )
{
    if ( !hascallingenv( L ) )  /* no enclosing function? */
    {
        return gcvalue(gt(L));  /* use global table as environment */
    }

    return curr_func( L )->env;
}

#endif //_LUA_API_INTERNALS_
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaFunctionDefs.Resources.cpp
*  PURPOSE:     Shared Lua resource definitions
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

namespace LuaFunctionDefs
{
    LUA_DECLARE( getResources )
    {
        lua_settop( L, 0 );
        lua_newtable( L );

        ResourceManager::resourceList_t::const_iterator iter = resManager->IterBegin();
        unsigned int n = 1;

        for ( ; iter != resManager->IterEnd(); ++iter, n++ )
        {
            (*iter)->PushStack( L );
            lua_rawseti( L, 1, n );
        }

        return 1;
    }

    LUA_DECLARE( getRuntimeResource )
    {
        lua_readcontext( L )->GetResource()->PushStack( L );
        return 1;
    }
}
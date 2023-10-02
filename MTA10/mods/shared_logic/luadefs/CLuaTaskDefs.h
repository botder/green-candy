/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaTaskDefs.h
*  PURPOSE:     Lua task definitions class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CLUATASKDEFS_H
#define __CLUATASKDEFS_H

#include "CLuaDefs.h"
#include "lua/CLuaCommonUtils.h"

namespace CLuaFunctionDefs
{
    void LoadTaskFunctions();

    LUA_DECLARE( createTaskInstance );

    LUA_DECLARE( getTaskName );

    LUA_DECLARE( getTaskParameter );
    LUA_DECLARE( getTaskParameters );
    LUA_DECLARE( setTaskParameters );
    LUA_DECLARE( clearTaskParameters );

    LUA_DECLARE( runTaskFunction );

    // Should be in player defs
    LUA_DECLARE( getPlayerTaskInstance );
    LUA_DECLARE( setPlayerTask );
};

#endif

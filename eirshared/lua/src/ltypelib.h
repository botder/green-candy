#ifndef _LUA_TYPE_LIBRARY_
#define _LUA_TYPE_LIBRARY_

#include "lobject.h"

// Type helper routines.
void* luaTL_checktype( lua_State *L, int stackidx, LuaTypeSystem::typeInfoBase *typeInfo );

#endif //_LUA_TYPE_LIBRARY_
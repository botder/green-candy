// Lua type abstraction system.
#ifndef _LUA_TYPE_ABSTRACTION_SYSTEM_
#define _LUA_TYPE_ABSTRACTION_SYSTEM_

#include <sdk/DynamicTypeSystem.h>

#include "lmem.h"

// Type sentry struct of the Lua type system.
// It notes the programmer that the struct has RTTI.
// All types that are constructed using the DynamicTypeSystem automatically inherit from this.
typedef GenericRTTI LuaRTTI;

// This class manages type information of the Lua runtime.
// It allows for dynamic C++ class extension depending on runtime conditions.
// Main purpose for its creation are the tight memory requirements of Lua VMs.
// NOTE: we are using conditional struct flavor because Lua types are VERY complicated!
typedef DynamicTypeSystem <GeneralMemoryAllocator, global_State> LuaTypeSystem;

#endif //_LUA_TYPE_ABSTRACTION_SYSTEM_

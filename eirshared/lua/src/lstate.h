/*
** $Id: lstate.h,v 2.24.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lapi.h"
#include "lobject.h"
#include "ltm.h"

// Helper functions to turn stack items into value addresses.
FASTAPI ValueAddress stackitem2adr( lua_State *L, RtCtxItem& stackItem )
{
    return ValueAddress( L, L->rtStack.GetNewVirtualStackItem( L, stackItem ) );
}
FASTAPI ConstValueAddress stackitem2constadr( lua_State *L, RtCtxItem& stackItem )
{
    return ConstValueAddress( L, L->rtStack.GetNewVirtualStackItemConst( L, stackItem ) );
}

// Opaque type.
typedef void Fiber;

// Internal functions.
LUAI_FUNC ValueAddress index2adr (lua_State *L, int idx);
LUAI_FUNC ConstValueAddress index2constadr( lua_State *L, int idx );

// Since we use advanced memory allocation techniques, we depend on custom
// memory management templates.
#include <sdk/MemoryUtils.h>

// Put plugin ids here!
#define GLOBAL_STATE_PLUGIN_ALLOC_HOLD          0x00000000
#define GLOBAL_STATE_PLUGIN_MAIN_STATE          0x00000001
#define GLOBAL_STATE_PLUGIN_STRING_META         0x00000002

enum eLuaThreadStatus : unsigned char
{
	THREAD_RUNNING,
	THREAD_SUSPENDED,
	THREAD_TERMINATED
};

class lua_Thread : public lua_State
{
public:
    lua_Thread( global_State *g, void *construction_params );
    ~lua_Thread();

    void    SetMainThread( bool enable )        { isMain = enable; }
    bool    IsThread()                          { return !isMain; }

    bool    AllocateRuntime();
    bool    IsRuntimeAllocated() const          { return fiber != nullptr; }

    void    SetYieldDisabled( bool disable )    { yieldDisabled = disable; }
    bool    IsYieldDisabled()                   { return yieldDisabled; }

    void resume();
    void yield();

    bool isMain;
    bool yieldDisabled;
    Fiber *callee;
    Fiber *fiber;
    eLuaThreadStatus status;
	int errorCode;

    RwListEntry <lua_Thread> threadNode;
};

/* macros to convert a GCObject into a specific value */
#define rawgco2ts(o)	check_exp((o)->tt == LUA_TSTRING, (TString*)(o))
#define gco2ts(o)	(rawgco2ts(o))
#define rawgco2u(o)	check_exp((o)->tt == LUA_TUSERDATA, (Udata*)(o))
#define gco2u(o)	(rawgco2u(o))
#define gco2cl(o)	check_exp((o)->tt == LUA_TFUNCTION, (Closure*)(o))
#define gco2h(o)	check_exp((o)->tt == LUA_TTABLE, (Table*)(o))
#define gco2j(o)    check_exp((o)->tt == LUA_TCLASS, (Class*)(o))
#define gco2p(o)	check_exp((o)->tt == LUA_TOBJECT, (Proto*)(o))  // TODO: maybe re-add security checks more thoroughly.
#define gco2uv(o)	check_exp((o)->tt == LUA_TOBJECT, (UpVal*)(o))
#define ngcotouv(o) check_exp((o) == nullptr || (o)->tt == LUA_TOBJECT, (UpVal*)(o))
#define gco2th(o)	check_exp((o)->tt == LUA_TTHREAD, (lua_State*)(o))


LUAI_FUNC lua_Thread* luaE_newthread (lua_State *L);
LUAI_FUNC void luaE_newenvironment( lua_State *L );
LUAI_FUNC LuaTypeSystem::typeInfoBase* luaE_getmainthreadtype( global_State *g );
LUAI_FUNC void luaE_terminate( lua_Thread *L );
LUAI_FUNC void luaE_freethread (lua_State *L, lua_State *L1);

// Library initialization.
LUAI_FUNC void luaE_libraryinit( void );
LUAI_FUNC void luaE_libraryshutdown( void );

#endif


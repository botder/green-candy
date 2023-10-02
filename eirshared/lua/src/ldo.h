/*
** $Id: ldo.h,v 2.7.1.1 2007/12/27 13:02:25 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#ifndef ldo_h
#define ldo_h


#include "lobject.h"
#include "lstate.h"
#include "lzio.h"


FASTAPI void luaD_checkstack( lua_State *L, stackOffset_t n )
{
    RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

    RtStackView& currentStackFrame = L->GetCurrentStackFrame();

    stackOffset_t currentTopOffset = currentStackFrame.GetTopOffset( L, *rtStack );

    currentStackFrame.EnsureOutboundsSlots( L, *rtStack, currentTopOffset + n );
}

FASTAPI stackOffset_t savestack( lua_State *L, StkId_const p )
{
    return L->rtStack.GetStackOffset( L, p );
}
FASTAPI RtCtxItem restorestack( lua_State *L, stackOffset_t n )
{
    return L->rtStack.GetStackItem( L, n );
}

FASTAPI stackOffset_t saveci( lua_State *L, CiCtxItem p )
{
    return L->ciStack.GetStackOffset( L, p.Pointer() );
}
FASTAPI CiCtxItem restoreci( lua_State *L, stackOffset_t n )
{
    return L->ciStack.GetStackItem( L, n );
}

/* results from luaD_precall */
#define PCRLUA		0	/* initiated a call to a Lua function */
#define PCRC		1	/* did a call to a C function */
#define PCRYIELD	2	/* C funtion yielded */


/* type of protected functions, to be ran by `runprotected' */
typedef void (*Pfunc) (lua_State *L, void *ud);

LUAI_FUNC int luaD_protectedparser (lua_State *L, ZIO *z, const char *name);
LUAI_FUNC void luaD_callhook (lua_State *L, int event, int line);
LUAI_FUNC int luaD_precall (lua_State *L, RtCtxItem& func, int nresults, stackOffset_t topOffset, eCallFrameModel callFrameBehavior);
LUAI_FUNC void luaD_call (lua_State *L, RtCtxItem& func, int nResults, eCallFrameModel callFrameBehavior, stackOffset_t argTopOffset);
LUAI_FUNC int luaD_pcall (lua_State *L, Pfunc func, void *u, stackOffset_t oldtop, stackOffset_t ef, lua_Debug *debug);
LUAI_FUNC void luaD_leaveframe (lua_State *L);
LUAI_FUNC int luaD_poscall (lua_State *L, RtCtxItem *firstResult, eCallFrameModel callFrameBehavior, stackOffset_t resultTopOffset);
LUAI_FUNC void luaD_reallocCI (lua_State *L, stackOffset_t newsize);
LUAI_FUNC void luaD_reallocstack (lua_State *L, stackOffset_t newsize);
LUAI_FUNC void luaD_growstack (lua_State *L, stackOffset_t n);

LUAI_FUNC int luaD_rawrunprotected( lua_State *L, Pfunc f, void *ud, std::string& err, lua_Debug *debug );

LUAI_FUNC void luaD_seterrorobj (lua_State *L, int errcode);

#endif


/*
** $Id: lgc.h,v 2.15.1.1 2007/12/27 13:02:25 roberto Exp $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#ifndef lgc_h
#define lgc_h


#include "lobject.h"


/*
** Possible states of the Garbage Collector
*/
#define GCSpause	0
#define GCSpropagate	1
#define GCSsweepstring	2
#define GCSsweep	3
#define GCSfinalize	4

LUAI_FUNC void luaC_checkGC( lua_State *L );

LUAI_FUNC int luaC_getstate( lua_State *L );
LUAI_FUNC void luaC_setthreshold( lua_State *L, lu_mem threshold );
LUAI_FUNC lu_byte luaC_white( global_State *g );
LUAI_FUNC void luaC_markobject( global_State *g, GCObject *o );
LUAI_FUNC void luaC_callGCTM (lua_State *L);
LUAI_FUNC void luaC_freeall (lua_State *L);
LUAI_FUNC void luaC_step (lua_State *L);
LUAI_FUNC void luaC_finish( lua_State *L );
LUAI_FUNC void luaC_fullgc (lua_State *L);
LUAI_FUNC bool luaC_isready (lua_State *L);
LUAI_FUNC void luaC_register( global_State *g, GCObject *o, lu_byte tt );
LUAI_FUNC void luaC_link( global_State *g, GCObject *o, lu_byte tt );
LUAI_FUNC void luaC_unlink( global_State *g, GCObject *o );
LUAI_FUNC void luaC_linktmu( global_State *g, GCObject *o, lu_byte tt );
LUAI_FUNC void luaC_unlinktmu( global_State *g, GCObject *o );
LUAI_FUNC void luaC_paycost( global_State *g, class lua_Thread *L, lu_mem cost );
LUAI_FUNC void luaC_barrierf (lua_State *L, GCObject *o, GCObject *v);
LUAI_FUNC void luaC_barrier( lua_State *L, GCObject *o, const TValue *v );
LUAI_FUNC void luaC_objbarrier( lua_State *L, GCObject *p, GCObject *o );
LUAI_FUNC void luaC_stringmark( lua_State *L, TString *string );
LUAI_FUNC void luaC_forceupdatef( lua_State *L, GCObject *o );
LUAI_FUNC void luaC_barrierback( lua_State *L, GrayObject *o );
LUAI_FUNC void luaC_barriert( lua_State *L, Table *t, const TValue *v );
LUAI_FUNC void luaC_objbarriert( lua_State *L, Table *t, GCObject *o );
LUAI_FUNC void luaC_init( global_State *g );
LUAI_FUNC void luaC_initthread( global_State *g );
LUAI_FUNC void luaC_shutdown( global_State *g );

// Module initialization.
LUAI_FUNC void luaC_moduleinit( lua_config *cfg );
LUAI_FUNC void luaC_moduleshutdown( lua_config *cfg );

#endif

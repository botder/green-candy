/*
** $Id: ldebug.h,v 2.3.1.1 2007/12/27 13:02:25 roberto Exp $
** Auxiliary functions from Debug Interface module
** See Copyright Notice in lua.h
*/

#ifndef ldebug_h
#define ldebug_h


#include "lstate.h"


FASTAPI int pcRel( const Instruction *pc, const Proto *p )              { return (cast(int, (pc) - (p)->code) - 1); }

FASTAPI int getline( const Proto *f, size_t pc )                        { return (((f)->lineinfo) ? (f)->lineinfo[pc] : 0); }

FASTAPI void resethookcount( lua_State *L )                             { (L->hookcount = L->basehookcount); }


LUAI_FUNC void luaG_typeerror (lua_State *L, ConstValueAddress& o, const char *opname);
LUAI_FUNC void luaG_concaterror (lua_State *L, ConstValueAddress o, ConstValueAddress p2);
LUAI_FUNC void luaG_aritherror (lua_State *L, ConstValueAddress p1, ConstValueAddress p2);
LUAI_FUNC int luaG_ordererror (lua_State *L, ConstValueAddress& p1, ConstValueAddress& p2);
LUAI_FUNC void luaG_runerror (lua_State *L, const char *fmt, ...);
LUAI_FUNC void luaG_errormsg (lua_State *L);
LUAI_FUNC bool luaG_checkcode (const Proto *pt);
LUAI_FUNC bool luaG_checkopenop (Instruction i);
LUAI_FUNC std::list <std::string> luaG_protodump(lua_State *L, const Proto *p);
LUAI_FUNC std::list <std::string> luaG_stackdump(lua_State *L);

class callstack_ref
{
    friend class lua_State;
public:
    callstack_ref( lua_State& L ) : m_lua( L )
    {
        if ( L.nCcalls == LUAI_MAXCCALLS )
            throw lua_exception( &L, LUA_ERRRUN, "C stack overflow" );
        else if ( L.nCcalls >= (LUAI_MAXCCALLS + (LUAI_MAXCCALLS>>3)) )
            throw lua_exception( &L, LUA_ERRERR, "stack handling error" );

        L.nCcalls++;
    }

    ~callstack_ref()
    {
        m_lua.nCcalls--;
    }

    lua_State& m_lua;
};

class debughook_shield
{
public:
    debughook_shield( lua_State& L ) : m_lua( L )
    {
        allowhook = L.allowhook;
        L.allowhook = false;
    }

    ~debughook_shield()
    {
        m_lua.allowhook = allowhook;
    }

    lua_State& m_lua;
    bool allowhook;
};

#endif

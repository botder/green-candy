/*
** $Id: ldebug.c,v 2.29.1.6 2008/05/08 16:56:26 roberto Exp $
** Debug Interface
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "lapi.h"
#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"



static const char *getfuncname (lua_State *L, CallInfo *ci, const char **name);


static inline int currentpc (lua_State *L, CallInfo *ci)
{
    if (!isLua(ci)) return -1;  /* function is not a Lua function? */
    if (ci == L->ciStack.Top())
    {
        ci->savedpc = L->savedpc;
    }
    return pcRel(ci->savedpc, ci_func(ci)->GetLClosure()->p);
}


static int currentline (lua_State *L, CallInfo *ci) {
  int pc = currentpc(L, ci);
  if (pc < 0)
    return -1;  /* only active lua functions have current-line information */
  else
    return getline(ci_func(ci)->GetLClosure()->p, pc);
}


/*
** this function can be called asynchronous (e.g. during a signal)
*/
LUA_API int lua_sethook (lua_State *L, lua_Hook func, int mask, int count) {
  if (func == nullptr || mask == 0) {  /* turn off hooks? */
    mask = 0;
    func = nullptr;
  }
  L->hook = func;
  L->basehookcount = count;
  resethookcount(L);
  L->hookmask = cast_byte(mask);
  return 1;
}


LUA_API lua_Hook lua_gethook (lua_State *L) {
  return L->hook;
}


LUA_API int lua_gethookmask (lua_State *L) {
  return L->hookmask;
}


LUA_API int lua_gethookcount (lua_State *L) {
  return L->basehookcount;
}


LUA_API int lua_getstack (lua_State *L, int level, lua_Debug *ar)
{
    int status = 0;

    lua_lock(L);

    CallInfo *ci;

    for ( ci = L->ciStack.Top(); level > 0 && ci > L->ciStack.Base(); ci-- )
    {
        level--;

        if ( f_isLua(ci) )  /* Lua function? */
        {
            level -= ci->tailcalls;  /* skip lost tail calls */
        }
    }
    if ( level == 0 && ci > L->ciStack.Base() )
    {  /* level found? */
        status = 1;
        ar->i_ci = (int)L->ciStack.GetStackOffset( L, ci );
    }
    else if ( level < 0 )
    {  /* level is of a lost tail call? */
        status = 1;
        ar->i_ci = 0;
    }

    lua_unlock(L);

    return status;
}


static Proto *getluaproto (CallInfo *ci) {
  return (isLua(ci) ? ci_func(ci)->GetLClosure()->p : nullptr);
}


static const char *findlocal (lua_State *L, CallInfo *ci, int n)
{
    const char *name;
    Proto *fp = getluaproto(ci);

    if ( fp && (name = luaF_getlocalname(fp, n, currentpc(L, ci))) != nullptr )
    {
        return name;  /* is a local variable in a Lua function */
    }

    stackOffset_t ciUsageCount = ci->stack.GetUsageCount( L, L->rtStack );

    if ( n > 0 && n < ciUsageCount )  /* is 'n' inside 'ci' stack? */
    {
        return "(*temporary)";
    }

    // No idea.
    return nullptr;
}


LUA_API const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n)
{
    CiCtxItem ciCtx = L->ciStack.GetStackItem( L, ar->i_ci );
    CallInfo *ci = ciCtx.Pointer();

    const char *name = findlocal(L, ci, n);

    lua_lock(L);

    if ( name )
    {
        RtCtxItem readFromValue = ci->stack.GetStackItem( L, L->rtStack, n - 1 );

        pushtvalue(L, readFromValue.Pointer());
    }

    lua_unlock(L);

    return name;
}


LUA_API const char *lua_setlocal (lua_State *L, const lua_Debug *ar, int n)
{
    CiCtxItem ciCtx = L->ciStack.GetStackItem( L, ar->i_ci );
    CallInfo *ci = ciCtx.Pointer();

    const char *name = findlocal(L, ci, n);

    lua_lock(L);

    if ( name )
    {
        ConstValueAddress topItem = index2constadr( L, -1 );

        RtCtxItem writeToValue = index2stackadr( L, n - 1 );

        writeToValue.Set( *topItem.Pointer( L ) );
    }
    popstack( L, 1 );  /* pop value */

    lua_unlock(L);

    return name;
}


static void funcinfo (lua_Debug *ar, Closure *cl)
{
    if (cl->isC)
    {
        ar->source = "=[C]";
        ar->linedefined = -1;
        ar->lastlinedefined = -1;
        ar->what = "C";
    }
    else
    {
        LClosure *lcl = cl->GetLClosure();
        ar->source = getstr(lcl->p->source);
        ar->linedefined = lcl->p->linedefined;
        ar->lastlinedefined = lcl->p->lastlinedefined;
        ar->what = (ar->linedefined == 0) ? "main" : "Lua";
    }
    luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
}


static void info_tailcall (lua_Debug *ar) {
  ar->name = ar->namewhat = "";
  ar->what = "tail";
  ar->lastlinedefined = ar->linedefined = ar->currentline = -1;
  ar->source = "=(tail call)";
  luaO_chunkid(ar->short_src, ar->source, LUA_IDSIZE);
  ar->nups = 0;
}


static void collectvalidlines (lua_State *L, Closure *f)
{
    if ( f == nullptr || f->isC )
    {
        pushnilvalue(L);
    }
    else
    {
        Table *t = luaH_new(L, 0, 0);

        try
        {
            LClosure *lcl = f->GetLClosure();
            int *lineinfo = lcl->p->lineinfo;

            for ( size_t i = 0; i < lcl->p->sizelineinfo; i++ )
            {
                ValueAddress lineinfoVar = luaH_setnum( L, t, lineinfo[i] );

                setbvalue( lineinfoVar, 1 );
            }

            pushhvalue(L, t);
        }
        catch( ... )
        {
            t->DereferenceGC( L );
            throw;
        }

        // Since the table is now on the stack, we can dereference it.
        t->DereferenceGC( L );
    }
}


static int auxgetinfo (lua_State *L, const char *what, lua_Debug *ar, Closure *f, CallInfo *ci)
{
  int status = 1;
  if (f == nullptr) {
    info_tailcall(ar);
    return status;
  }
  for (; *what; what++) {
    switch (*what) {
      case 'S': {
        funcinfo(ar, f);
        break;
      }
      case 'l': {
        ar->currentline = (ci) ? currentline(L, ci) : -1;
        break;
      }
      case 'u': {
        ar->nups = f->nupvalues;
        break;
      }
      case 'n': {
        ar->namewhat = (ci) ? getfuncname(L, ci, &ar->name) : nullptr;
        if (ar->namewhat == nullptr) {
          ar->namewhat = "";  /* not found */
          ar->name = nullptr;
        }
        break;
      }
      case 'L':
      case 'f':  /* handled by lua_getinfo */
        break;
      default: status = 0;  /* invalid option */
    }
  }
  return status;
}

LUA_API int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar)
{
    int status;
    Closure *f = nullptr;
    CallInfo *ci = nullptr;

    lua_lock(L);

    if ( *what == '>' )
    {
        {
            ConstValueAddress topItem = index2constadr( L, -1 );

            luai_apicheck(L, ttisfunction(topItem));

            what++;  /* skip the '>' */
            f = clvalue(topItem);
        }
        popstack( L, 1 );  /* pop function */
    }
    else if ( ar->i_ci != 0 )
    {  /* no tail call? */
        ci = L->ciStack.GetStackItem( L, ar->i_ci ).Pointer();

        lua_assert(ttisfunction(ci->func));

        f = clvalue(ci->func);
    }

    status = auxgetinfo(L, what, ar, f, ci);

    if ( strchr(what, 'f') )
    {
        if ( f == nullptr )
        {
            pushnilvalue(L);
        }
        else
        {
            pushclvalue(L, f);
        }
    }

    if ( strchr(what, 'L') )
    {
        collectvalidlines(L, f);
    }

    lua_unlock(L);

    return status;
}


/*
** {======================================================
** Symbolic Execution and code checker
** =======================================================
*/

#define check(x)		if (!(x)) return 0;

#define checkjump(pt,pc)	check(0 <= pc && pc < pt->sizecode)

#define checkreg(pt,reg)	check((reg) < (pt)->maxstacksize)



static int precheck (const Proto *pt)
{
    check(pt->maxstacksize <= MAXSTACK);
    check(pt->numparams+(pt->is_vararg & VARARG_HASARG) <= pt->maxstacksize);
    check(!(pt->is_vararg & VARARG_NEEDSARG) || (pt->is_vararg & VARARG_HASARG));
    check(pt->sizeupvalues <= pt->nups);
    check(pt->sizelineinfo == pt->sizecode || pt->sizelineinfo == 0);
    check(pt->sizecode > 0 && GET_OPCODE(pt->code[pt->sizecode-1]) == OP_RETURN);
    return 1;
}


#define checkopenop(pt,pc)	luaG_checkopenop((pt)->code[(pc)+1])

bool luaG_checkopenop (Instruction i)
{
    switch (GET_OPCODE(i))
    {
    case OP_CALL:
    case OP_TAILCALL:
    case OP_RETURN:
    case OP_SETLIST: {
        check(GETARG_B(i) == 0);
        return true;
    }
    default: return false;  /* invalid instruction after an open call */
    }
}


static int checkArgMode (const Proto *pt, unsigned int r, enum OpArgMask mode) {
  switch (mode) {
    case OpArgN: check(r == 0); break;
    case OpArgU: break;
    case OpArgR: checkreg(pt, r); break;
    case OpArgK:
      check(ISK(r) ? INDEXK(r) < pt->sizek : r < pt->maxstacksize);
      break;
  }
  return 1;
}


static Instruction symbexec (const Proto *pt, size_t lastpc, stackOffset_t reg)
{
    size_t last;  /* stores position of last instruction that changed `reg' */
    last = pt->sizecode-1;  /* points to final return (a `neutral' instruction) */
    check(precheck(pt));

    for (size_t pc = 0; pc < lastpc; pc++)
    {
        Instruction i = pt->code[pc];
        OpCode op = GET_OPCODE(i);

        int a = GETARG_A(i);
        int b = 0;
        int c = 0;

        check(op < NUM_OPCODES);
        checkreg(pt, a);

        switch (getOpMode(op))
        {
        case iABC:
        {
            b = GETARG_B(i);
            c = GETARG_C(i);
            check(checkArgMode(pt, b, getBMode(op)));
            check(checkArgMode(pt, c, getCMode(op)));
            break;
        }
        case iABx:
        {
            b = GETARG_Bx(i);
            if (getBMode(op) == OpArgK) check((unsigned int)b < pt->sizek);
                break;
        }
        case iAsBx:
        {
            b = GETARG_sBx(i);

            if (getBMode(op) == OpArgR)
            {
                check(b >= 0 || pc + 1 >= (size_t)-b);

                size_t dest = pc+1+b;
                check(dest < pt->sizecode);

                if (dest > 0)
                {
                    size_t j;

                    /* check that it does not jump to a setlist count; this
                    is tricky, because the count from a previous setlist may
                    have the same value of an invalid setlist; so, we must
                    go all the way back to the first of them (if any) */
                    for (j = 0; j < dest; j++)
                    {
                        Instruction d = pt->code[dest-1-j];

                        if (!(GET_OPCODE(d) == OP_SETLIST && GETARG_C(d) == 0))
                            break;
                    }
                    /* if 'j' is even, previous value is not a setlist (even if
                    it looks like one) */
                    check((j&1) == 0);
                }
            }
            break;
            }
        }

        if (testAMode(op))
        {
            if (a == reg)
            {
                last = pc;  /* change register `a' */
            }
        }

        if (testTMode(op))
        {
            check(pc+2 < pt->sizecode);  /* check skip */
            check(GET_OPCODE(pt->code[pc+1]) == OP_JMP);
        }

        switch (op)
        {
        case OP_LOADBOOL:
        {
            if (c == 1)
            {  /* does it jump? */
                check(pc+2 < pt->sizecode);  /* check its jump */
                check(GET_OPCODE(pt->code[pc+1]) != OP_SETLIST ||
                GETARG_C(pt->code[pc+1]) != 0);
            }
            break;
        }
        case OP_LOADNIL:
        {
            if (a <= reg && reg <= b)
            {
                last = pc;  /* set registers from `a' to `b' */
            }
            break;
        }
        case OP_GETUPVAL:
        case OP_SETUPVAL:
        {
            check(b < pt->nups);
            break;
        }
        case OP_GETGLOBAL:
        case OP_SETGLOBAL:
        {
            check(ttisstring(&pt->k[b]));
            break;
        }
        case OP_SELF: {
            checkreg(pt, a+1);

            if (reg == a+1)
            {
                last = pc;
            }

            break;
        }
        case OP_CONCAT: {
            check(b < c);  /* at least two operands */
            break;
        }
        case OP_TFORLOOP:
        {
            check(c >= 1);  /* at least one result (control variable) */
            checkreg(pt, a+2+c);  /* space for results */

            if (reg >= a+2)
            {
                last = pc;  /* affect all regs above its base */
            }

            break;
        }
        case OP_FORLOOP:
        case OP_FORPREP:
            checkreg(pt, a+3);

            /* fallthrough */
        case OP_JMP:
        {
            check(b >= 0 || pc + 1 >= (size_t)-b);

            size_t dest = pc + 1 + b;

            /* not full check and jump is forward and do not skip `lastpc'? */
            if (reg != NO_REG && pc < (unsigned int)dest && (unsigned int)dest <= lastpc)
            {
                pc += b;  /* do the jump */
            }
            break;
        }
        case OP_CALL:
        case OP_TAILCALL:
        {
            if (b != 0)
            {
                checkreg(pt, a+b-1);
            }

            c--;  /* c = num. returns */

            if (c == LUA_MULTRET)
            {
                check(checkopenop(pt, pc));
            }
            else if (c != 0)
            {
                checkreg(pt, a+c-1);
            }

            if (reg >= a)
            {
                last = pc;  /* affect all registers above base */
            }
            break;
        }
        case OP_RETURN:
        {
            b--;  /* b = num. returns */

            if (b > 0)
            {
                checkreg(pt, a+b-1);
            }
            break;
        }
        case OP_SETLIST:
        {
            if (b > 0)
            {
                checkreg(pt, a + b);
            }
            if (c == 0)
            {
                check(pc < pt->sizecode);
                pc++;
            }
            break;
        }
        case OP_CLOSURE:
        {
            lua_assert(b >= 0);

            check((unsigned int)b < pt->sizep);

            unsigned int nup = pt->p[b]->nups;

            check(pc + nup < pt->sizecode);

            for (unsigned int j = 1; j <= nup; j++)
            {
                OpCode op1 = GET_OPCODE(pt->code[pc + j]);

                check(op1 == OP_GETUPVAL || op1 == OP_MOVE);
            }
            if (reg != NO_REG)  /* tracing? */
            {
                pc += nup;  /* do not 'execute' these pseudo-instructions */
            }
            break;
        }
        case OP_VARARG:
        {
            check((pt->is_vararg & VARARG_ISVARARG) && !(pt->is_vararg & VARARG_NEEDSARG));

            b--;

            if (b == LUA_MULTRET)
            {
                check(checkopenop(pt, pc));
            }

            checkreg(pt, a+b-1);
            break;
        }
        default:
            break;
        }
    }
    return pt->code[last];
}

#undef check
#undef checkjump
#undef checkreg

/* }====================================================== */


bool luaG_checkcode (const Proto *pt) {
  return (symbexec(pt, pt->sizecode, NO_REG) != 0);
}


static const char *kname (Proto *p, int c) {
  if (ISK(c) && ttisstring(&p->k[INDEXK(c)]))
    return svalue(&p->k[INDEXK(c)]);
  else
    return "?";
}


static const char *getobjname (lua_State *L, CallInfo *ci, stackOffset_t stackpos, const char **name)
{
    if (!isLua(ci))  /* a Lua function? */
        return nullptr;

    Proto *p = ci_func(ci)->GetLClosure()->p;
    int pc = currentpc(L, ci);
    Instruction i;
    *name = luaF_getlocalname(p, stackpos+1, pc);

    if (*name)  /* is a local? */
        return "local";

    i = symbexec(p, pc, stackpos);  /* try symbolic execution */
    lua_assert(pc != -1);

    switch (GET_OPCODE(i))
    {
    case OP_GETGLOBAL: {
        int g = GETARG_Bx(i);  /* global index */
        lua_assert(ttisstring(&p->k[g]));
        *name = svalue(&p->k[g]);
        return "global";
    }
    case OP_MOVE: {
        int a = GETARG_A(i);
        int b = GETARG_B(i);  /* move from `b' to `a' */

        if (b < a)
            return getobjname(L, ci, b, name);  /* get name for `b' */

        break;
    }
    case OP_GETTABLE: {
        int k = GETARG_C(i);  /* key index */
        *name = kname(p, k);
        return "field";
    }
    case OP_GETUPVAL: {
        int u = GETARG_B(i);  /* upvalue index */
        *name = p->upvalues ? getstr(p->upvalues[u]) : "?";
        return "upvalue";
    }
    case OP_SELF: {
        int k = GETARG_C(i);  /* key index */
        *name = kname(p, k);
        return "method";
    }
    default: break;
    }
    return nullptr;
}


static const char *getfuncname (lua_State *L, CallInfo *ci, const char **name)
{
  Instruction i;
  if ((isLua(ci) && ci->tailcalls > 0) || !isLua(ci - 1))
    return nullptr;  /* calling function is not Lua (or is unknown) */
  ci--;  /* calling function */
  i = ci_func(ci)->GetLClosure()->p->code[currentpc(L, ci)];
  if (GET_OPCODE(i) == OP_CALL || GET_OPCODE(i) == OP_TAILCALL ||
      GET_OPCODE(i) == OP_TFORLOOP)
    return getobjname(L, ci, GETARG_A(i), name);
  else
    return nullptr;  /* no useful name can be found */
}

void luaG_typeerror (lua_State *L, ConstValueAddress& o, const char *op)
{
    const char *name = nullptr;
    const char *t = luaT_gettypename( L, ttype(o) );
    const char *kind = nullptr;

    CallInfo *current_ci = L->ciStack.Top();

    if ( L->GetCurrentStackFrame().IsInStack( L, L->rtStack, o ) )
    {
        stackOffset_t objOffset = L->ciStack.Top()->stack.GetStackOffset( L, L->rtStack, o );

        kind = getobjname(L, current_ci, objOffset, &name);
    }

    if ( kind )
        luaG_runerror(L, "attempt to %s %s " LUA_QS " (a %s value)", op, kind, name, t);
    else
        luaG_runerror(L, "attempt to %s a %s value", op, t);
}

void luaG_concaterror (lua_State *L, ConstValueAddress p1, ConstValueAddress p2)
{
    if ( ttisstring(p1) || ttisnumber(p1) )
    {
        p1 = p2;
    }

    lua_assert(!ttisstring(p1) && !ttisnumber(p1));

    luaG_typeerror(L, p1, "concatenate");
}

void luaG_aritherror (lua_State *L, ConstValueAddress p1, ConstValueAddress p2)
{
    LocalValueAddress tmp;

    if ( luaV_tonumber(p1, tmp) == nullptr )
    {
        p2 = std::move( p1 );  /* first operand is wrong */
    }

    luaG_typeerror(L, p2, "perform arithmetic on");
}

int luaG_ordererror (lua_State *L, ConstValueAddress& p1, ConstValueAddress& p2)
{
    const char *t1 = luaT_gettypename( L, ttype(p1) );
    const char *t2 = luaT_gettypename( L, ttype(p2) );

    if ( t1[2] == t2[2] )
    {
        luaG_runerror(L, "attempt to compare two %s values", t1);
    }
    else
    {
        luaG_runerror(L, "attempt to compare %s with %s", t1, t2);
    }

    return 0;
}

#if 0
static void addinfo (lua_State *L, const char *msg)
{
    CallInfo *ci = L->ciStack.Top();

    if ( isLua(ci) )
    {  /* is Lua code? */
        char buff[LUA_IDSIZE];  /* add file:line information */
        int line = currentline(L, ci);
        luaO_chunkid(buff, getstr(getluaproto(ci)->source), LUA_IDSIZE);
        luaO_pushfstring(L, "%s:%d: %s", buff, line, msg);
    }
}
#endif //0

void luaG_errormsg (lua_State *L)
{
#if 0
  if (L->errfunc != 0) {  /* is there an error handling function? */
    StkId errfunc = restorestack(L, L->errfunc);
    if (!ttisfunction(errfunc)) luaD_throw(L, LUA_ERRERR);
    setobjs2s(L, L->top, L->top - 1);  /* move argument */
    setobjs2s(L, L->top - 1, errfunc);  /* push function */
    incr_top(L);
    luaD_call(L, L->top - 2, 1);  /* call it */
  }
#endif
    throw lua_exception( L, LUA_ERRRUN, lua_tostring( L, -1 ) );
}


void luaG_runerror (lua_State *L, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    luaO_pushvfstring(L, fmt, argp);
    va_end(argp);
    luaG_errormsg( L );
}

std::list <std::string> luaG_protodump(lua_State *L, const Proto *p)
{
    std::list <std::string> protoDump;

    size_t sizecode = p->sizecode;

    for ( size_t n = 0; n < sizecode; n++ )
    {
        Instruction inst = p->code[ n ];

        OpCode opcode = GET_OPCODE( inst );

        const char *opName = luaP_getopname( opcode );

        std::string instDescriptor = opName;

        protoDump.push_back( instDescriptor );
    }

    return protoDump;
}

std::list <std::string> luaG_stackdump(lua_State *L)
{
    std::list <std::string> stackDump;

    // Create a list of strings that represents the stack, with the most recent item at the top.
    RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );
    {
        RtStackView& currentFrame = L->GetCurrentStackFrame();

        stackOffset_t itemCount = currentFrame.GetUsageCount( L, *rtStack );

        for ( stackOffset_t n = 0; n < itemCount; n++ )
        {
            RtCtxItem stackItem = currentFrame.GetStackItem( L, *rtStack, itemCount - n - 1 );

            StkId_const iter = stackItem.Pointer();

            // Put an info into the string list.
            std::string debugString;

            int type = ttype( iter );

            const char *native_type_name = lua_typename( L, type );

            // Add the type name.
            if ( native_type_name )
            {
                debugString += native_type_name;
                debugString += ' ';
            }

            if ( iscollectable( iter ) )
            {
                debugString += '[';

                GCObject *gcobj = gcvalue(iter);

                LuaRTTI *rttiObj = LuaTypeSystem::GetTypeStructFromObject( gcobj );

                LuaTypeSystem::typeInfoBase *typeInfo = LuaTypeSystem::GetTypeInfoFromTypeStruct( rttiObj );

                if ( const char *typeName = typeInfo->name )
                {
                    debugString += typeName;
                }

                debugString += ']';
            }

            stackDump.push_back( debugString );
        }
    }

    return stackDump;
}

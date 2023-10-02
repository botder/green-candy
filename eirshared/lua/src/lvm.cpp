/*
** $Id: lvm.c,v 2.63.1.4 2009/07/01 21:10:33 roberto Exp $
** Lua virtual machine
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"

#include "lfunc.upval.hxx"


/* limit for table tag-method chains (to avoid loops) */
#define MAXTAGLOOP	100


ConstValueAddress luaV_tonumber (ConstValueAddress& obj, ValueAddress& n)
{
    lua_Number num;

    if ( ttisnumber(obj) )
    {
        return obj;
    }

    if ( ttisstring(obj) && luaO_str2d(svalue(obj), &num) )
    {
        setnvalue(n, num);
        return n.ConstCast();
    }

    return ConstValueAddress( nullptr );
}


int luaV_tostring (lua_State *L, ValueAddress& obj)
{
    if ( !ttisnumber(obj) )
        return 0;
    else
    {
        char s[LUAI_MAXNUMBER2STR];
        lua_Number n = nvalue(obj);
        lua_number2str(s, n);

        TString *numberString = luaS_new(L, s);

        setsvalue(L, obj, numberString);

        // Since the number string is now on the stack, we can dereference it.
        numberString->DereferenceGC( L );

        return 1;
    }
}


static void traceexec( lua_State *L, const Instruction *pc )
{
    lu_byte mask = L->hookmask;
    const Instruction *oldpc = L->savedpc;

    L->savedpc = pc;

    if ( (mask & LUA_MASKCOUNT) && L->hookcount == 0 )
    {
        resethookcount(L);
        luaD_callhook(L, LUA_HOOKCOUNT, -1);
    }
    if ( mask & LUA_MASKLINE )
    {
        Proto *p = ci_func( L->ciStack.Top() )->GetLClosure()->p;
        int npc = pcRel(pc, p);
        int newline = getline(p, npc);

        /* call linehook when enter a new function, when jump back (loop),
           or when enter a new line */
        if ( npc == 0 || pc <= oldpc || newline != getline(p, pcRel(oldpc, p)) )
            luaD_callhook(L, LUA_HOOKLINE, newline);
    }
}


static void callTMres (lua_State *L, ValueAddress& res, ConstValueAddress& f, ConstValueAddress& p1, ConstValueAddress& p2)
{
    pushtvalue(L, f);  /* push function */
    pushtvalue(L, p1);  /* 1st argument */
    pushtvalue(L, p2);  /* 2nd argument */
    luaD_checkstack(L, 3);
    lua_call(L, 2, 1);
    popstkvalue( L, res );
}

// WARNING: stack reallocating function
void luaV_handle_index( lua_State *L, ConstValueAddress& obj, ConstValueAddress& tm, ConstValueAddress& key, ValueAddress& val )
{
    if ( ttisnil( tm ) )
        luaG_typeerror( L, obj, "index" );

    if ( ttisfunction( tm ) )
    {
        // Reallocate a new stack
        luaD_checkstack( L, 3 );

        // Work with the new stack
        pushclvalue( L, clvalue( tm ) );
        pushtvalue( L, obj );
        pushtvalue( L, key );
        lua_call( L, 2, 1 );

        // Save result at appropriate position
        popstkvalue( L, val );
    }
    else
    {
        callstack_ref indexRef( *L );

        luaV_gettable( L, tm, key, val );
    }
}

// WARNING: stack reallocating function
void luaV_gettable (lua_State *L, ConstValueAddress& t, ConstValueAddress& key, ValueAddress& val)
{
    if ( iscollectable( t ) )
        gcvalue( t )->Index( L, key, val );
    else
    {
        ConstValueAddress tmobjval = luaT_gettmbyobj( L, t, TM_INDEX );

        luaV_handle_index( L, t, tmobjval, key, val );
    }
}

// WARNING: stack reallocating function
void luaV_handle_newindex( lua_State *L, ConstValueAddress& obj, ConstValueAddress& tm, ConstValueAddress& key, ConstValueAddress& val )
{
    if ( ttisnil( tm ) )
        luaG_typeerror( L, obj, "index" );

    if ( ttisfunction( tm ) )
    {
        luaD_checkstack( L, 4 );    // allocate a new stack

        // Work with the new stack
        pushclvalue( L, clvalue( tm ) );
        pushtvalue( L, obj );
        pushtvalue( L, key );
        pushtvalue( L, val );
        lua_call( L, 3, 0 );
    }
    else
    {
        // Repeat the process with the index object
        // For security reasons, we should increment the callstack depth
        callstack_ref indexRef( *L );

        luaV_settable( L, tm, key, val );
    }
}

// WARNING: stack reallocating function
void luaV_settable (lua_State *L, ConstValueAddress& t, ConstValueAddress& key, ConstValueAddress& val)
{
    if ( iscollectable( t ) )
        gcvalue( t )->NewIndex( L, key, val );
    else
    {
        ConstValueAddress tmobjval = luaT_gettmbyobj( L, t, TM_NEWINDEX );

        luaV_handle_newindex( L, t, tmobjval, key, val );
    }
}


static int call_binTM (lua_State *L, ConstValueAddress& p1, ConstValueAddress& p2, ValueAddress& res, TMS event)
{
    ConstValueAddress tm = luaT_gettmbyobj( L, p1, event );  /* try first operand */

    if ( ttisnil(tm) )
    {
        tm = luaT_gettmbyobj( L, p2, event );  /* try second operand */

        if ( ttisnil(tm) )
        {
            return 0;
        }
    }

    callTMres(L, res, tm, p1, p2);
    return 1;
}

static ConstValueAddress get_compTM (lua_State *L, Table *mt1, Table *mt2, TMS event)
{
    global_State *g = G(L);

    ConstValueAddress tm1 = gfasttm(g, mt1, event);

    if (tm1 == nullptr) return ConstValueAddress( nullptr );  /* no metamethod */
    if (mt1 == mt2) return tm1;  /* same metatables => same metamethods */

    ConstValueAddress tm2 = gfasttm(g, mt2, event);

    if (tm2 == nullptr) return ConstValueAddress( nullptr );  /* no metamethod */
    if (luaO_rawequalObj(tm1, tm2))  /* same metamethods? */
        return tm1;

    return ConstValueAddress( nullptr );
}

static int call_orderTM (lua_State *L, ConstValueAddress& p1, ConstValueAddress& p2, TMS event)
{
    ConstValueAddress tm1 = luaT_gettmbyobj(L, p1, event);

    if ( ttisnil(tm1) )
        return -1;  /* no metamethod? */

    ConstValueAddress tm2 = luaT_gettmbyobj(L, p2, event);

    if ( !luaO_rawequalObj(tm1, tm2) )  /* different metamethods? */
        return -1;

    LocalValueAddress tmp;

    callTMres( L, tmp, tm1, p1, p2 );
    return !l_isfalse(tmp);
}


static int l_strcmp (const TString *ls, const TString *rs) {
  const char *l = getstr(ls);
  size_t ll = ls->len;
  const char *r = getstr(rs);
  size_t lr = rs->len;
  for (;;) {
    int temp = strcoll(l, r);
    if (temp != 0) return temp;
    else {  /* strings are equal up to a `\0' */
      size_t len = strlen(l);  /* index of first `\0' in both strings */
      if (len == lr)  /* r is finished? */
        return (len == ll) ? 0 : 1;
      else if (len == ll)  /* l is finished? */
        return -1;  /* l is smaller than r (because r is not finished) */
      /* both strings longer than `len'; go on comparing (after the `\0') */
      len++;
      l += len; ll -= len; r += len; lr -= len;
    }
  }
}


int luaV_lessthan (lua_State *L, ConstValueAddress& l, ConstValueAddress& r)
{
    int res;

    if (ttype(l) != ttype(r))
    {
        return luaG_ordererror(L, l, r);
    }
    else if (ttisnumber(l))
    {
        return luai_numlt(nvalue(l), nvalue(r));
    }
    else if (ttisstring(l))
    {
        return l_strcmp(tsvalue(l), tsvalue(r)) < 0;
    }
    else if ((res = call_orderTM(L, l, r, TM_LT)) != -1)
    {
        return res;
    }

    return luaG_ordererror(L, l, r);
}


static int lessequal (lua_State *L, ConstValueAddress& l, ConstValueAddress& r)
{
    int res;

    if (ttype(l) != ttype(r))
    {
        return luaG_ordererror(L, l, r);
    }
    else if (ttisnumber(l))
    {
        return luai_numle(nvalue(l), nvalue(r));
    }
    else if (ttisstring(l))
    {
        return l_strcmp(tsvalue(l), tsvalue(r)) <= 0;
    }
    else if ((res = call_orderTM(L, l, r, TM_LE)) != -1)  /* first try `le' */
    {
        return res;
    }
    else if ((res = call_orderTM(L, r, l, TM_LT)) != -1)  /* else try `lt' */
    {
        return !res;
    }

    return luaG_ordererror(L, l, r);
}


int luaV_equalval (lua_State *L, ConstValueAddress& t1, ConstValueAddress& t2)
{
    ConstValueAddress tm;
    lua_assert(ttype(t1) == ttype(t2));

    switch (ttype(t1))
    {
    case LUA_TNIL: return 1;
    case LUA_TNUMBER: return luai_numeq(nvalue(t1), nvalue(t2));
    case LUA_TBOOLEAN: return bvalue(t1) == bvalue(t2);  /* true must be 1 !! */
    case LUA_TLIGHTUSERDATA: return pvalue(t1) == pvalue(t2);
    case LUA_TUSERDATA:
        if (uvalue(t1) == uvalue(t2))
            return 1;

        tm = get_compTM(L, uvalue(t1)->metatable, uvalue(t2)->metatable, TM_EQ);
        break;  /* will try TM */
    case LUA_TTABLE:
        if (hvalue(t1) == hvalue(t2))
            return 1;

        tm = get_compTM(L, hvalue(t1)->metatable, hvalue(t2)->metatable, TM_EQ);
        break;  /* will try TM */
    default: return gcvalue(t1) == gcvalue(t2);
    }

    if (tm == nullptr)
        return 0;  /* no TM? */

    LocalValueAddress tmp;

    callTMres(L, tmp, tm, t1, t2);  /* call TM */
    return !l_isfalse(tmp);
}


void luaV_concat (lua_State *L, stackOffset_t total, stackOffset_t last)
{
    lua_assert( total >= 2 );

    do
    {
        stackOffset_t topOffset = last + 1;

        ValueAddress firstOp = fetchstackadr( L, topOffset - 2 );
        ValueAddress secondOp = fetchstackadr( L, topOffset - 1 );

        int n = 2;  /* number of elements handled in this pass (at least 2) */

        if (!(ttisstring(firstOp) || ttisnumber(firstOp)) || !tostring(L, secondOp))
        {
            if (!call_binTM(L, firstOp.ConstCast(), secondOp.ConstCast(), firstOp, TM_CONCAT))
            {
                luaG_concaterror(L, firstOp.ConstCast(), secondOp.ConstCast());
            }
        }
        else if (tsvalue(secondOp)->len == 0)  /* second op is empty? */
        {
            (void)tostring(L, firstOp);  /* result is first op (as string) */
        }
        else
        {
            n = luaS_concat( L, (int)topOffset, (int)total );
        }

        total -= n-1;  /* got `n' strings to create 1 new */
        last -= n-1;
    }
    while (total > 1);  /* repeat until only 1 result left */
}


static void Arith (lua_State *L, ValueAddress& ra, ConstValueAddress& rb, ConstValueAddress& rc, TMS op)
{
    LocalValueAddress tempb, tempc;
    ConstValueAddress b, c;

    if ( (b = luaV_tonumber(rb, tempb)) != nullptr && (c = luaV_tonumber(rc, tempc)) != nullptr )
    {
        lua_Number nb = nvalue(b), nc = nvalue(c);

        switch (op)
        {
        case TM_ADD: setnvalue(ra, luai_numadd(nb, nc)); break;
        case TM_SUB: setnvalue(ra, luai_numsub(nb, nc)); break;
        case TM_MUL: setnvalue(ra, luai_nummul(nb, nc)); break;
        case TM_DIV: setnvalue(ra, luai_numdiv(nb, nc)); break;
        case TM_MOD: setnvalue(ra, luai_nummod(nb, nc)); break;
        case TM_POW: setnvalue(ra, luai_numpow(nb, nc)); break;
        case TM_UNM: setnvalue(ra, luai_numunm(nb)); break;
        default: lua_assert(0); break;
        }
    }
    else if ( !call_binTM(L, rb, rc, ra, op) )
    {
        luaG_aritherror(L, rb, rc);
    }
}



/*
** some macros for common tasks in `luaV_execute'
*/

FASTAPI void bytecode_exception( lua_State *L, const char *errmsg )
{
    std::string finalError( "bytecode exception: " );
    finalError += errmsg;

    throw lua_exception( L, LUA_ERRRUN, finalError.c_str() );
}

struct bytecode_frame
{
    lua_State *L;
    rtStack_t *rtStack;
    Proto *runningProto;

    inline bytecode_frame( lua_State *L, LClosure *cl )
    {
        this->L = L;
        this->rtStack = &L->rtStack;
        this->runningProto = cl->p;
    }

    FASTAPI stackOffset_t RA_getStackOffset( const Instruction i )
    {
        return GETARG_A(i);
    }
    FASTAPI RtCtxItem RActx_offset( const Instruction i, int offset )
    {
        return L->GetCurrentStackFrame().GetStackItem( L, *rtStack, RA_getStackOffset(i) + offset );
    }
    FASTAPI ValueAddress RA_offset( const Instruction i, int offset )
    {
        RtCtxItem stackItem = RActx_offset( i, offset );

        return ValueAddress( L, rtStack->GetNewVirtualStackItem( L, std::move( stackItem ) ) );
    }
    FASTAPI ValueAddress RA( const Instruction i )
    {
        return RA_offset( i, 0 );
    }
    FASTAPI ValueAddress RB( const Instruction i )
    {
        //check_exp(getBMode(GET_OPCODE(i)) == OpArgR, )
        RtCtxItem stackItem = L->GetCurrentStackFrame().GetStackItem( L, *rtStack, GETARG_B(i) );

        return ValueAddress( L, rtStack->GetNewVirtualStackItem( L, std::move( stackItem ) ) );
    }
    FASTAPI ValueAddress RC( const Instruction i )
    {
        //check_exp(getCMode(GET_OPCODE(i)) == OpArgR, )
        RtCtxItem stackItem = L->GetCurrentStackFrame().GetStackItem( L, *rtStack, GETARG_C(i) );

        return ValueAddress( L, rtStack->GetNewVirtualStackItem( L, std::move( stackItem ) ) );
    }
    FASTAPI ConstValueAddress RKB( const Instruction i )
    {
        //check_exp(getBMode(GET_OPCODE(i)) == OpArgK, )

        ConstValueAddress retAddr;

        if ( ISK(GETARG_B(i)) )
        {
            retAddr = luaF_getprotoconstaddress( L, this->runningProto, INDEXK(GETARG_B(i)) );
        }
        else
        {
            retAddr = RB(i);
        }

        return retAddr;
    }
    FASTAPI ConstValueAddress RKC( const Instruction i )
    {
        //check_exp(getCMode(GET_OPCODE(i)) == OpArgK, )

        ConstValueAddress retAddr;

        if ( ISK(GETARG_C(i)) )
        {
            retAddr = luaF_getprotoconstaddress( L, this->runningProto, INDEXK(GETARG_C(i)) );
        }
        else
        {
            retAddr = RC(i);
        }

        return retAddr;
    }
    FASTAPI ConstValueAddress KBx( const Instruction i )
    {
        //check_exp(getBMode(GET_OPCODE(i)) == OpArgK, )
        return luaF_getprotoconstaddress( L, this->runningProto, GETARG_Bx(i) );
    }
};


#define dojump(L,pc,i)	{(pc) += (i); luai_threadyield(L);}


#define Protect(x)	{ L->savedpc = pc; {x;}; }


#define arith_op(op,tm) \
{ \
    ConstValueAddress rb = bcframe.RKB(i); \
    ConstValueAddress rc = bcframe.RKC(i); \
    if (ttisnumber(rb) && ttisnumber(rc)) \
    { \
        lua_Number nb = nvalue(rb), nc = nvalue(rc); \
        setnvalue(ra, op(nb, nc)); \
    } \
    else \
    { \
      Protect(Arith(L, ra, rb, rc, tm)); \
    } \
}

void lua_debugLClosureStack( lua_State *L, CallInfo *currentFrame )
{
    stackOffset_t currentStackTop = currentFrame->stack.GetUsageCount( L, L->rtStack );
    (void)currentStackTop;

    LClosure *lcl = clvalue( currentFrame->func )->GetLClosure();

    stackOffset_t requiredStackTop = lcl->p->maxstacksize;
    (void)requiredStackTop;

    lua_assert( currentStackTop == requiredStackTop );
}

void luaV_execute (lua_State *L, int nexeccalls, eCallFrameModel callFrameModel)
{
reentry:  /* entry point */
    CiCtxItem currentFrameCtx = L->ciStack.GetStackItem( L, -1 );

    lua_assert(isLua(currentFrameCtx.Pointer()));

    const Instruction *pc = L->savedpc;
    LClosure *cl = clvalue( currentFrameCtx.Pointer()->func )->GetLClosure();

    bytecode_frame bcframe( L, cl );

    /* main loop of interpreter */
    for (;;)
    {
        const Instruction i = *pc++;

        if ( (L->hookmask & (LUA_MASKLINE | LUA_MASKCOUNT)) && (--L->hookcount == 0 || L->hookmask & LUA_MASKLINE) )
        {
            traceexec( L, pc );
        }

        OpCode opcode = GET_OPCODE(i);

        // Debug beforehand.
        lua_debugLClosureStack( L, currentFrameCtx.Pointer() );

        // Is the opcode heavily stack dependant?
        if ( opcode == OP_RETURN ||
             opcode == OP_TAILCALL ||
             opcode == OP_CALL ||
             opcode == OP_CLOSE ||
             opcode == OP_TFORLOOP ||
             opcode == OP_SETLIST ||
             opcode == OP_VARARG ||
             opcode == OP_FORLOOP )
        {
            RtCtxItem ra = bcframe.RActx_offset(i, 0);

            switch( opcode )
            {
            case OP_CALL: {
                int b = GETARG_B(i);
                int nresults = GETARG_C(i) - 1;

                stackOffset_t theTopOffset;

                {
                    CallInfo *currentFrame = currentFrameCtx.Pointer();

                    if ( b != 0 )
                    {
                        stackOffset_t raOffset = bcframe.RA_getStackOffset( i );

                        theTopOffset = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, raOffset + b - 1 );    /* else previous instruction set top */
                    }
                    else
                    {
                        lua_assert( currentFrame->hasVirtualStackTop == true );

                        // Make sure top offset is absolute.
                        theTopOffset = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, currentFrame->virtualStackTop, true );
                    }
                }

                L->savedpc = pc;

                int result = luaD_precall(L, ra, nresults, theTopOffset, CALLFRAME_STATIC);

                switch( result )
                {
                case PCRLUA: {
                    nexeccalls++;
                    goto reentry;  /* restart luaV_execute over new Lua function */
                }
                case PCRC: {
                    /* it was a C function (`precall' called it); adjust results */
                    if ( nresults == -1 )
                    {
                        CallInfo *currentFrame = currentFrameCtx.Pointer();

                        currentFrame->hasVirtualStackTop = true;

                        currentFrame->virtualStackTop = currentFrame->stack.GetRelativeStackOffset( L, *bcframe.rtStack, currentFrame->lastResultTop );
                    }
                    break;
                }
                default: {
                    return;  /* yield */
                }
                }

                break;
            }
            case OP_TAILCALL: {
                int b = GETARG_B(i);

                stackOffset_t theTopOffset;

                {
                    CallInfo *currentFrame = currentFrameCtx.Pointer();

                    if ( b != 0 )
                    {
                        stackOffset_t raOffset = bcframe.RA_getStackOffset( i );

                        stackOffset_t rebasingOffset = ( raOffset + b - 1 );

                        theTopOffset = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, 0 ) + rebasingOffset;  /* else previous instruction set top */
                    }
                    else
                    {
                        lua_assert( currentFrame->hasVirtualStackTop == true );

                        // Make sure top offset is absolute.
                        theTopOffset = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, currentFrame->virtualStackTop, true );
                    }
                }

                L->savedpc = pc;

                lua_assert( GETARG_C(i) - 1 == -1 );

                int precallResult = luaD_precall(L, ra, LUA_MULTRET, theTopOffset, CALLFRAME_STATIC);

                switch( precallResult )
                {
                case PCRLUA: {
                    /* tail call: put new frame in place of previous one */
                    CallInfo *ciPrev = currentFrameCtx.Pointer();

                    CallInfo *newCurrentFrame = L->ciStack.Top();

                    rtStack_t& rtStack = L->rtStack;

                    // Cannot throw exceptions in this code region.
                    rtStack.Lock( L );
                    {
                        StkId func = ciPrev->func;
                        StkId_const pfunc = newCurrentFrame->func;  /* previous function index */

                        if ( !L->openupval.IsEmpty() )
                        {
                            luaF_close( L, ciPrev->stack.Base( L, rtStack ) );
                        }

                        stackOffset_t newBaseOffset =
                            bcframe.rtStack->GetStackOffset( L,
                                func + rtStack_t::GetDeferredStackOffset( pfunc, newCurrentFrame->stack.Base( L, rtStack ) )
                            );

                        // Rebase this stack frame.
                        ciPrev->stack.InitializeEx( L, *bcframe.rtStack, newBaseOffset, clvalue(pfunc)->GetLClosure()->p->maxstacksize );

                        // TODO: properly implement this following behavior in the stack logic.
                        // It moves a variable amount of values to another position on the stack.

                        // Move down the stack frame.

                        RtStackView& curStackFrame = newCurrentFrame->stack;

                        StkId_const stackTop = curStackFrame.Top( L, rtStack );
                        {
                            stackOffset_t prevFuncOffset = bcframe.rtStack->GetStackOffset( L, pfunc );
                            stackOffset_t curFuncOffset = bcframe.rtStack->GetStackOffset( L, func );

                            stackOffset_t moveCount = rtStack_t::GetDeferredStackOffset( pfunc, stackTop ) + 1;

                            for ( stackOffset_t n = 0; n < moveCount; n++ )
                            {
                                RtCtxItem srcItem = bcframe.rtStack->GetStackItem( L, prevFuncOffset + n );
                                RtCtxItem dstItem = bcframe.rtStack->GetStackItem( L, curFuncOffset + n );

                                // Move the item down.
                                setobj( L, dstItem.Pointer(), srcItem.Pointer() );
                            }
                        }
                    }
                    rtStack.Unlock( L );

                    ciPrev->savedpc = L->savedpc;
                    ciPrev->tailcalls++;  /* one more call lost */

                    L->ciStack.Pop( L );  /* remove new frame */
                    goto reentry;
                }
                case PCRC: {  /* it was a C function (`precall' called it) */
                    CallInfo *currentFrame = currentFrameCtx.Pointer();

                    currentFrame->hasVirtualStackTop = true;

                    currentFrame->virtualStackTop = currentFrame->stack.GetRelativeStackOffset( L, *bcframe.rtStack, currentFrame->lastResultTop );
                    break;
                }
                default: {
                    return;  /* yield */
                }
                }

                break;
            }
            case OP_RETURN: {
                int b = GETARG_B(i);

                stackOffset_t resultTopOffsetAbsolute;

                CallInfo *currentFrame = currentFrameCtx.Pointer();

                if ( b != 0 )
                {
                    stackOffset_t raOffset = bcframe.RA_getStackOffset( i );

                    stackOffset_t rebasingOffset = ( raOffset + b - 2 );

                    resultTopOffsetAbsolute = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, 0 ) + rebasingOffset;
                }
                else
                {
                    lua_assert( currentFrame->hasVirtualStackTop == true );

                    resultTopOffsetAbsolute = currentFrame->stack.GetAbsoluteStackOffset( L, *bcframe.rtStack, currentFrame->virtualStackTop, true );
                }

                if ( !L->openupval.IsEmpty() )
                {
                    luaF_close(L, currentFrame->stack.Base( L, *bcframe.rtStack ));
                }

                L->savedpc = pc;

                // Decide how to handle return.
                eCallFrameModel returnCIModel = CALLFRAME_STATIC;

                if ( nexeccalls == 1 )
                {
                    returnCIModel = callFrameModel;
                }

                bool shouldSetVirtualStackTop = ( currentFrame->nresults == -1 );

                b = luaD_poscall(L, &ra, returnCIModel, resultTopOffsetAbsolute);

                CallInfo *newCurrentFrame = L->ciStack.Top();

                if ( shouldSetVirtualStackTop == true )
                {
                    newCurrentFrame->hasVirtualStackTop = true;

                    newCurrentFrame->virtualStackTop = newCurrentFrame->stack.GetRelativeStackOffset( L, *bcframe.rtStack, newCurrentFrame->lastResultTop );
                }

                if ( --nexeccalls == 0 )  /* was previous function running `here'? */
                {
                    return;  /* no: return */
                }
                else
                {  /* yes: continue its execution */
                    lua_assert(isLua(newCurrentFrame));
                    lua_assert(GET_OPCODE(*((newCurrentFrame)->savedpc - 1)) == OP_CALL);

                    goto reentry;
                }

                break;
            }
            case OP_CLOSE: {
                luaF_close(L, ra.Pointer());
                break;
            }
            case OP_TFORLOOP: {
                RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );
                {
                    RtCtxItem srcStateCtx = bcframe.RActx_offset(i, 1);
                    RtCtxItem srcIndexCtx = bcframe.RActx_offset(i, 2);

                    RtCtxItem cb = bcframe.RActx_offset(i, 3);  /* call base */
                    {
                        RtCtxItem stateCtx = bcframe.RActx_offset(i, 4);
                        RtCtxItem indexCtx = bcframe.RActx_offset(i, 5);

                        setobjs2s(L, indexCtx.Pointer(), srcIndexCtx.Pointer());
                        setobjs2s(L, stateCtx.Pointer(), srcStateCtx.Pointer());
                        setobjs2s(L, cb.Pointer(), ra.Pointer());
                    }

                    // Adjust the stack top.
                    stackOffset_t callTopOffset;
                    {
                        stackOffset_t raOffset = bcframe.RA_getStackOffset(i);
                        stackOffset_t cbOffset = ( raOffset + 3 );

                        callTopOffset = currentFrameCtx.Pointer()->stack.GetAbsoluteStackOffset( L, *rtStack, cbOffset + 3 - 1 );   /* func. + 2 args (state and index) */
                    }

                    Protect(luaD_call(L, cb, GETARG_C(i), CALLFRAME_STATIC, callTopOffset));

                    if ( !ttisnil(cb.Pointer()) )
                    {  /* continue loop? */
                        setobj(L, srcIndexCtx.Pointer(), cb.Pointer());  /* save control variable */
                        dojump(L, pc, GETARG_sBx(*pc));  /* jump back */
                    }
                }

                pc++;
                break;
            }
            case OP_SETLIST: {
                stackOffset_t n = GETARG_B(i);
                int c = GETARG_C(i);

                CallInfo *currentFrame = currentFrameCtx.Pointer();

                if ( n == 0 )
                {
                    lua_assert( currentFrame->hasVirtualStackTop == true );

                    stackOffset_t raOffset = currentFrame->stack.GetStackOffset( L, *bcframe.rtStack, ra.Pointer() );

                    stackOffset_t offsetTopFromRA = ( currentFrame->virtualStackTop - raOffset );

                    n = offsetTopFromRA;
                }
                if ( c == 0 )
                {
                    c = cast_int(*pc++);
                }

                if ( !ttistable(ra.Pointer()) )
                {
                    bytecode_exception( L, "expected table in ARG_A" );
                }

                Table *h = hvalue(ra.Pointer());

                stackOffset_t last = ((c-1)*LFIELDS_PER_FLUSH) + n;

                if ( last > 0 )
                {
                    luaH_ensureslots( L, h, (size_t)last );
                }

                stackOffset_t raAbsoluteOffset = currentFrame->stack.GetStackOffsetAbsolute( L, *bcframe.rtStack, ra.Pointer() );

                for ( ; n > 0; n-- )
                {
                    stackOffset_t nextIdx = last--;

                    RtCtxItem valCtx = bcframe.rtStack->GetStackItem( L, raAbsoluteOffset + n );

                    ValueAddress numObj = luaH_setnum(L, h, (int)nextIdx);

                    setobj2t(L, numObj, valCtx.Pointer());
                }
                break;
            }
            case OP_VARARG: {
                CallInfo *currentFrame = currentFrameCtx.Pointer();

                stackOffset_t b = GETARG_B(i) - 1;
                stackOffset_t funcBaseOffset = rtStack_t::GetDeferredStackOffset( currentFrame->func, currentFrame->stack.Base( L, *bcframe.rtStack ) );
                stackOffset_t actualPushedParams = ( funcBaseOffset - 1 );
                stackOffset_t n = ( actualPushedParams - cl->p->numparams );

                if ( b == -1 )
                {
                    Protect( luaD_checkstack( L, n ) );

                    stackOffset_t raOffset = bcframe.RA_getStackOffset(i);

                    b = n;

                    // Store the stack top.
                    stackOffset_t requiredStackOffset = ( raOffset + n - 1 );

                    currentFrame->hasVirtualStackTop = true;

                    currentFrame->virtualStackTop = requiredStackOffset;
                }

                stackOffset_t raOffset = bcframe.RA_getStackOffset( i );

                for ( stackOffset_t j = 0; j < b; j++ )
                {
                    RtCtxItem targetStackItem = currentFrame->stack.GetStackItemOutbounds( L, *bcframe.rtStack, raOffset + j );

                    // Sort of hacky, but it works.
                    if ( j < n )
                    {
                        // Hack into the vararg params on of the previous frame.
                        StkId_const srcStackItem = currentFrame->stack.Base( L, *bcframe.rtStack ) - n + j;

                        setobjs2s(L, targetStackItem.Pointer(), srcStackItem);
                    }
                    else
                    {
                        setnilvalue(targetStackItem.Pointer());
                    }
                }
                break;
            }
            case OP_FORLOOP: {
                RtCtxItem stepItem = bcframe.RActx_offset(i, 2);
                RtCtxItem limitItem = bcframe.RActx_offset(i, 1);

                if ( ttype( stepItem.Pointer() ) != LUA_TNUMBER )
                {
                    bytecode_exception( L, "step item in OP_FORLOOP is not a number" );
                }

                if ( ttype( limitItem.Pointer() ) != LUA_TNUMBER )
                {
                    bytecode_exception( L, "limit item in OP_FORLOOP is not a number" );
                }

                if ( ttype( ra.Pointer() ) != LUA_TNUMBER )
                {
                    bytecode_exception( L, "index item in OP_FORLOOP is not a number" );
                }

                lua_Number step = nvalue( stepItem.Pointer() );
                lua_Number idx = luai_numadd(nvalue( ra.Pointer() ), step); /* increment index */
                lua_Number limit = nvalue( limitItem.Pointer() );

                if ( luai_numlt(0, step) ? luai_numle(idx, limit) : luai_numle(limit, idx) )
                {
                    RtCtxItem extidxItem = bcframe.RActx_offset(i, 3);

                    dojump(L, pc, GETARG_sBx(i));  /* jump back */
                    setnvalue(ra.Pointer(), idx);  /* update internal index... */
                    setnvalue(extidxItem.Pointer(), idx);  /* ...and external index */
                }
                break;
            }
            default:
                // GCC is retarded.
                break;
            }
        }
        else
        {
            ValueAddress ra = bcframe.RA(i);
            //lua_assert(L->top == L->ci->top || luaG_checkopenop(i));
            lua_assert(ra != nullptr);

            switch( opcode )
            {
            case OP_MOVE: {
                ValueAddress b = bcframe.RB(i);

                setobjs2s(L, ra, b);
                break;
            }
            case OP_LOADK: {
                ConstValueAddress kb = bcframe.KBx( i );

                setobj2s(L, ra, kb);
                break;
            }
            case OP_LOADBOOL: {
                setbvalue( ra, ( GETARG_B(i) != 0 ) );

                if ( GETARG_C(i) )
                {
                    pc++;  /* skip next instruction (if C) */
                }
                break;
            }
            case OP_LOADNIL: {
                bcframe.rtStack->Lock( L );
                {
                    TValue *rb = bcframe.RB(i);

                    do
                    {
                        setnilvalue(rb--);
                    }
                    while (rb >= ra);
                }
                bcframe.rtStack->Unlock( L );
                break;
            }
            case OP_GETUPVAL: {
                int b = GETARG_B(i);
                setobj2s(L, ra, cl->upvals[b]->v);
                break;
            }
            case OP_GETGLOBAL: {
                LocalValueAddress g;
                ConstValueAddress rb = bcframe.KBx(i);
                setgcvalue(L, g, cl->env);
                lua_assert(ttisstring(rb));
                Protect(luaV_gettable(L, g.ConstCast(), rb, ra));
                break;
            }
            case OP_GETTABLE: {
                ConstValueAddress tableAddr = bcframe.RB(i);
                ConstValueAddress keyAddr = bcframe.RKC(i);

                Protect(luaV_gettable(L, tableAddr, keyAddr, ra));
                break;
            }
            case OP_SETGLOBAL: {
                LocalValueAddress g;
                setgcvalue(L, g, cl->env);
                ConstValueAddress keyAddr = bcframe.KBx(i);
                lua_assert(ttisstring(keyAddr));
                Protect(luaV_settable(L, g.ConstCast(), keyAddr, ra.ConstCast()));
                break;
            }
            case OP_SETUPVAL: {
                UpVal *uv = cl->upvals[ GETARG_B(i) ];
                setobj(L, uv->v, ra);
                luaC_barrier(L, uv, ra);
                break;
            }
            case OP_SETTABLE: {
                ConstValueAddress keyAddr = bcframe.RKB(i);
                ConstValueAddress valAddr = bcframe.RKC(i);

                Protect(luaV_settable(L, ra.ConstCast(), keyAddr, valAddr));
                break;
            }
            case OP_NEWTABLE: {
                int b = GETARG_B(i);
                int c = GETARG_C(i);

                Table *newTable = luaH_new(L, luaO_fb2int(b), luaO_fb2int(c));

                sethvalue(L, ra, newTable);

                // Since the table is now living on the stack, we can dereference it.
                newTable->DereferenceGC( L );

                Protect(luaC_checkGC(L));
                break;
            }
            case OP_SELF: {
                ValueAddress ra_offseted = bcframe.RA_offset( i, 1 );
                ConstValueAddress rb = bcframe.RB(i);

                setobjs2s(L, ra_offseted, rb);

                ConstValueAddress keyAddr = bcframe.RKC(i);

                Protect(luaV_gettable(L, rb, keyAddr, ra));
                break;
            }
            case OP_ADD: {
                arith_op(luai_numadd, TM_ADD);
                break;
            }
            case OP_SUB: {
                arith_op(luai_numsub, TM_SUB);
                break;
            }
            case OP_MUL: {
                arith_op(luai_nummul, TM_MUL);
                break;
            }
            case OP_DIV: {
                arith_op(luai_numdiv, TM_DIV);
                break;
            }
            case OP_MOD: {
                arith_op(luai_nummod, TM_MOD);
                break;
            }
            case OP_POW: {
                arith_op(luai_numpow, TM_POW);
                break;
            }
            case OP_UNM: {
                ConstValueAddress rb = bcframe.RB(i);

                if ( ttisnumber(rb) )
                {
                    lua_Number nb = nvalue(rb);
                    setnvalue(ra, luai_numunm(nb));
                }
                else
                {
                    Protect(Arith(L, ra, rb, rb, TM_UNM));
                }
                break;
            }
            case OP_NOT: {
                ConstValueAddress rb = bcframe.RB(i);
                bool res = l_isfalse(rb);  /* next assignment may change this value */
                setbvalue(ra, res);
                break;
            }
            case OP_LEN: {
                ConstValueAddress rb = bcframe.RB(i);

                switch( ttype(rb) )
                {
                case LUA_TTABLE: {
                    setnvalue(ra, cast_num(luaH_getn(L, hvalue(rb))));
                    break;
                }
                case LUA_TSTRING: {
                    setnvalue(ra, cast_num(tsvalue(rb)->len));
                    break;
                }
                default: {  /* try metamethod */
                    ConstValueAddress niladdr = luaO_getnilcontext( L );

                    Protect(
                        if ( !call_binTM(L, rb, niladdr, ra, TM_LEN) )
                        {
                            luaG_typeerror(L, rb, "get length of");
                        }
                    )
                }
                }
                break;
            }
            case OP_CONCAT: {
                int b = GETARG_B(i);
                int c = GETARG_C(i);

                Protect(luaV_concat(L, c-b+1, c); luaC_checkGC(L));

                ConstValueAddress outAddr = index2constadr( L, b + 1 );

                setobjs2s(L, ra, outAddr);
                break;
            }
            case OP_JMP: {
                dojump(L, pc, GETARG_sBx(i));
                break;
            }
            case OP_EQ: {
                ConstValueAddress rb = bcframe.RKB(i);
                ConstValueAddress rc = bcframe.RKC(i);

                Protect(
                    if ( equalobj(L, rb, rc) == GETARG_A(i) )
                    {
                        dojump(L, pc, GETARG_sBx(*pc));
                    }
                );

                pc++;
                break;
            }
            case OP_LT: {
                ConstValueAddress rb = bcframe.RKB(i);
                ConstValueAddress rc = bcframe.RKC(i);

                Protect(
                    if ( luaV_lessthan(L, rb, rc) == GETARG_A(i) )
                    {
                        dojump(L, pc, GETARG_sBx(*pc));
                    }
                );
                pc++;
                break;
            }
            case OP_LE: {
                ConstValueAddress rb = bcframe.RKB(i);
                ConstValueAddress rc = bcframe.RKC(i);

                Protect(
                    if ( lessequal(L, rb, rc) == GETARG_A(i) )
                    {
                        dojump(L, pc, GETARG_sBx(*pc));
                    }
                );
                pc++;
                break;
            }
            case OP_TEST: {
                if ( l_isfalse(ra) != ( GETARG_C(i) != 0 ) )
                {
                    dojump(L, pc, GETARG_sBx(*pc));
                }
                pc++;
                break;
            }
            case OP_TESTSET: {
                ConstValueAddress rb = bcframe.RB(i);

                if ( l_isfalse(rb) != ( GETARG_C(i) != 0 ) )
                {
                    setobjs2s(L, ra, rb);
                    dojump(L, pc, GETARG_sBx(*pc));
                }

                pc++;
                break;
            }
            case OP_FORPREP: {
                ValueAddress init = ra;
                ValueAddress plimit = bcframe.RA_offset(i, 1);
                ValueAddress pstep = bcframe.RA_offset(i, 2);

                L->savedpc = pc;  /* next steps may throw errors */

                if ( !tonumber(init.ConstCast(), init) )
                {
                    luaG_runerror(L, LUA_QL("for") " initial value must be a number");
                }
                else if ( !tonumber(plimit.ConstCast(), plimit) )
                {
                    luaG_runerror(L, LUA_QL("for") " limit must be a number");
                }
                else if ( !tonumber(pstep.ConstCast(), pstep) )
                {
                    luaG_runerror(L, LUA_QL("for") " step must be a number");
                }
                setnvalue(ra, luai_numsub(nvalue(ra), nvalue(pstep)));
                dojump(L, pc, GETARG_sBx(i));
                break;
            }
            case OP_CLOSURE: {
                Proto *p = cl->p->p[ GETARG_Bx(i) ];
                int nup = p->nups;

                LClosure *lcl = luaF_newLclosure( L, nup, cl->env );

                try
                {
                    Closure *ncl = lcl;
                    lcl->p = p;

                    for ( int j = 0; j < nup; j++, pc++ )
                    {
                        if ( GET_OPCODE(*pc) == OP_GETUPVAL )
                        {
                            lcl->upvals[j] = cl->upvals[ GETARG_B(*pc) ];
                        }
                        else
                        {
                            lua_assert(GET_OPCODE(*pc) == OP_MOVE);

                            RtCtxItem stackItem = index2stackadr( L, GETARG_B(*pc) + 1 );

                            lcl->upvals[j] = luaF_findupval(L, stackItem.Pointer());
                        }
                    }
                    setclvalue(L, ra, ncl);
                }
                catch( ... )
                {
                    // Even bytecode can throw exceptions, so account for that.
                    lcl->DereferenceGC( L );
                    throw;
                }

                // Since the Lua closure is now on the stack, we can dereference it.
                lcl->DereferenceGC( L );

                Protect(luaC_checkGC(L));
                break;
            }
            default:
                // GCC is stupid.
                break;
            }
        }

        // Debug stuff.
        lua_debugLClosureStack( L, currentFrameCtx.Pointer() );
    }
}

// Module initialization.
void luaV_init( lua_config *cfg )
{
    return;
}

void luaV_shutdown( lua_config *cfg )
{
    return;
}

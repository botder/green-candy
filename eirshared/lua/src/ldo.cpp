/*
** $Id: ldo.c,v 2.38.1.3 2008/01/18 22:31:22 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"
#include "lzio.h"

#include "lfunc.upval.hxx"

/*
** {======================================================
** Error-recovery functions
** =======================================================
*/


void luaD_seterrorobj (lua_State *L, int errcode)
{
    RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

    switch( errcode )
    {
    case LUA_ERRMEM: {
        TString *memErrorString = luaS_newliteral(L, MEMERRMSG);

        try
        {
            pushsvalue(L, memErrorString);
        }
        catch( ... )
        {
            memErrorString->DereferenceGC( L );
            throw;
        }

        // Since the error string is on the stack, we can free it now.
        memErrorString->DereferenceGC( L );
        break;
    }
    case LUA_ERRERR: {
        TString *errorErrorString = luaS_newliteral(L, "error in error handling");

        try
        {
            pushsvalue(L, errorErrorString);
        }
        catch( ... )
        {
            errorErrorString->DereferenceGC( L );
            throw;
        }

        // Since the error string is on the stack now, we can dereference it.
        errorErrorString->DereferenceGC( L );
        break;
    }
    case LUA_ERRSYNTAX:
    case LUA_ERRRUN: {
        pushtvalue(L, L->GetCurrentStackFrame().Top( L, *rtStack ));  /* error message on current top */
        break;
    }
    }
}


static void restore_stack_limit (lua_State *L)
{
    L->rtStack.VerifyIntegrity( L );
    L->ciStack.VerifyIntegrity( L );

    if ( L->ciStack.GetSize() > LUAI_MAXCALLS )
    {  /* there was an overflow? */
        stackOffset_t inuse = L->ciStack.GetUsageCount( L );

        if ( inuse + 1 < LUAI_MAXCALLS )  /* can `undo' overflow? */
        {
            luaD_reallocCI( L, LUAI_MAXCALLS );
        }
    }
}

#if 0
static void resetstack (lua_State *L, int status)
{
    L->ciStack.SetTopOffset( L, 0 );
    {
        RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

        StkId_const newBase = L->GetCurrentStackFrame().Base( L, *rtStack );
        luaF_close(L, newBase);  /* close eventual pending closures */
        L->rtStack.SetTopOffset( L, 0 );
        luaD_seterrorobj(L, status);
        L->allowhook = true;
        restore_stack_limit(L);
    }
    L->errfunc = 0;
}
#endif //0

int luaD_rawrunprotected( lua_State *L, Pfunc f, void *ud, std::string& err, lua_Debug *debug )
{
    try
    {
        (*f)(L, ud);
    }
    catch( lua_exception& e )
    {
        if ( debug )
            e.getDebug( *debug );

        err = e.what();
        return e.status();
    }

    return 0;
}

/* }====================================================== */

void lua_State::rtStackExp_t::OnRebasing( lua_State *L, stackItem_t oldStack )
{
    // Call the super function!
    growingStack::OnRebasing( L, oldStack );

    if ( oldStack != nullptr )
    {
        lua_State *rtThread = this->runtimeThread;

        for ( gcObjList_t::iterator iter = rtThread->openupval.GetIterator(); !iter.IsEnd(); iter.Increment() )
        {
            GCObject *up = (GCObject*)iter.Resolve();

            gco2uv(up)->v = GetDeferredStackItem( oldStack, gco2uv(up)->v );
        }

        if ( !rtThread->ciStack.IsEmpty( rtThread ) )
        {
            CallInfo *base_ci = rtThread->ciStack.Base();
            CallInfo *top_ci = rtThread->ciStack.Top();

            for ( CallInfo *ci = base_ci; ci <= top_ci; ci++ )
            {
                ci->stack.OnRebasing( rtThread, *this, oldStack );

                ci->func = GetDeferredStackItem( oldStack, ci->func );
            }
        }
    }
}

void lua_State::rtStackExp_t::OnResizing( lua_State *L, stackOffset_t oldSize )
{
    // Call the super function!
    growingStack::OnResizing( L, oldSize );
}

void lua_State::rtStackExp_t::OnChangeTop( lua_State *L, stackItem_t oldTop )
{
    // Call the super function!
    growingStack::OnChangeTop( L, oldTop );

    stackItem_t curTop = this->Top();

    if ( oldTop > curTop )
    {
        assert( 0 );
    }

    stackOffset_t topOffset;

    bool hasTopOffset = this->GetTopOffset( L, topOffset );

#if 0
    lua_State *runtimeThread = this->runtimeThread;
#endif

    if ( hasTopOffset )
    {
#if 0
        // Update the current call-frame top (if present).
        if ( !runtimeThread->ciStack.IsEmpty( L ) )
        {
            RtStackView& currentTopInfo = runtimeThread->GetCurrentStackFrame();
            {
                currentTopInfo.SetTopOffset( L, runtimeThread->rtStack, topOffset );

#if 0
                // Catch illegal changes of the top stack item.
                if ( currentCallFrame->func && ttisfunction( currentCallFrame->func ) )
                {
                    LClosure *cl = clvalue( currentCallFrame->func )->GetLClosure();

                    if ( cl )
                    {
                        stackOffset_t requiredStackSpace = cl->p->maxstacksize;
                        stackOffset_t currentStackSpace = this->GetUsageCountBasedOn( L, L->base );

                        lua_assert( requiredStackSpace <= currentStackSpace );
                    }
                }
#endif
            }
        }
#endif
    }
}


void luaD_reallocstack (lua_State *L, stackOffset_t newsize)
{
    StkId oldstack = L->rtStack.Base();
    UNUSED(oldstack);

    stackOffset_t realsize = newsize + 1 + EXTRA_STACK;

    L->rtStack.VerifyIntegrity( L );

    L->rtStack.SetSize( L, realsize );
}

void luaD_reallocCI (lua_State *L, stackOffset_t newsize)
{
    L->ciStack.SetSize( L, newsize );
}


void luaD_growstack (lua_State *L, stackOffset_t n)
{
    L->rtStack.Grow( L, n );
}


static CiCtxItem growCI (lua_State *L)
{
    if ( L->ciStack.GetSize() + 1 > LUAI_MAXCALLS )
    {
        luaG_runerror( L, "callinfo stack overflow" );
    }

    L->ciStack.Lock( L );

    CiCtxItem retCi = L->ciStack.ObtainItemLocked( L );

    L->ciStack.Unlock( L );

    return retCi;
}


void luaD_callhook (lua_State *L, int event, int line)
{
    lua_Hook hook = L->hook;

    if ( !hook || !L->allowhook )
        return;

    CallInfo *topCI = L->ciStack.Top();

    stackOffset_t topOffset = L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack );

    lua_Debug ar;

    ar.event = event;
    ar.currentline = line;

    if ( event == LUA_HOOKTAILRET )
    {
        ar.i_ci = 0;  /* tail call; no debug information about it */
    }
    else
    {
        ar.i_ci = cast_int( L->ciStack.GetStackOffset( L, topCI ) );
    }

    // Protect from more hook triggers
    {
        debughook_shield shield( *L );

        lua_unlock(L);
        (*hook)(L, &ar);
        lua_lock(L);
    }

    L->GetCurrentStackFrame().SetTopOffset( L, L->rtStack, topOffset );
}

static inline void tryfuncTM (lua_State *L, RtCtxItem& funcCtx)
{
    ConstValueAddress tm = luaT_gettmbyobj(L, funcCtx.Pointer(), TM_CALL);

    if ( !ttisfunction(tm) )
    {
        ConstValueAddress stackAddr = stackitem2constadr( L, funcCtx );

        luaG_typeerror( L, stackAddr, "call" );
    }

    // TODO: fix this

    // Get the absolute offset of the function on the stack.
    stackOffset_t funcOffset = L->rtStack.GetStackOffset( L, funcCtx.Pointer() );

    // We want to call a tag method, so insert it after the function.
    L->rtStack.InsertItemAtExternal( L, funcOffset + 1, funcCtx.Pointer() );
}


FASTAPI CiCtxItem inc_ci( lua_State *L )
{
    return growCI( L );
}


int luaD_precall (lua_State *L, RtCtxItem& func, int nresults, stackOffset_t prevTopOffset, eCallFrameModel ci_model)
{
    int precallResult = -1;

    if (!ttisfunction(func.Pointer())) /* `func' is not a function? */
    {
        tryfuncTM(L, func);  /* check the `function' tag method */
    }

    ciStack_t& ciStack = L->ciStack;
    rtStack_t& rtStack = L->rtStack;

    // Store the last bytecode position.
    ciStack.Top()->savedpc = L->savedpc;

    stackOffset_t funcOffsetAbsolute = L->GetCurrentStackFrame().GetStackOffsetAbsolute( L, rtStack, func.Pointer() );

    Closure *basic_closure = clvalue(func.Pointer());

    LClosure *lcl = basic_closure->GetLClosure();
    CClosure *ccl = nullptr;

    // First prepare the call stack entry.
    stackOffset_t newBaseOffsetAbsolute;
    stackOffset_t newStackTopCount = 0;

    if ( lcl )
    {  /* Lua function? prepare its call */
        Proto *p = lcl->p;

        luaD_checkstack(L, p->maxstacksize);

        if (!p->is_vararg)
        {  /* no varargs? */
            newBaseOffsetAbsolute = funcOffsetAbsolute + 1;
        }
        else
        {  /* vararg function */
            newBaseOffsetAbsolute = prevTopOffset + 1;
        }

        newStackTopCount = p->maxstacksize;
    }
    else if ( ( ccl = basic_closure->GetCClosure() ) != nullptr )
    {
        /* if it's a C function, call it */
        luaD_checkstack(L, LUA_MINSTACK);  /* ensure minimum stack size */

        // Get the current top offset.
        // We want to transform this top into a new top for the new callframe.
        stackOffset_t oldTopOffsetAbsolute = prevTopOffset;

        // This is the new base offset of the function.
        newBaseOffsetAbsolute = ( funcOffsetAbsolute + 1 );

        newStackTopCount = ( oldTopOffsetAbsolute - newBaseOffsetAbsolute + 1 );
    }
    else
    {
        lua_assert( 0 );
    }

    CiCtxItem ciCtx = inc_ci(L);  /* now `enter' new function */
    {
        CallInfo *ci = ciCtx.Pointer();

        ci->func = func.Pointer();
        ci->stack.InitializeEx( L, rtStack, newBaseOffsetAbsolute, newStackTopCount );
        ci->tailcalls = 0;
        ci->nresults = nresults;
        ci->hasVirtualStackTop = false;
        ci->virtualStackTop = -1;
        ci->lastResultTop = -1;
    }

    // Perform post-frame-creation building steps, if necessary.
    if ( lcl != nullptr )
    {
        Proto *p = lcl->p;

        if ( !p->is_vararg )
        {
            // Make sure that we zero out the available stack slots that have no valid arguments.
            stackOffset_t actuallyGivenParams = ( prevTopOffset - newBaseOffsetAbsolute + 1 );

            for ( stackOffset_t n = 0; n < newStackTopCount; n++ )
            {
                if ( n >= actuallyGivenParams )
                {
                    RtCtxItem clearItem = ciCtx.Pointer()->stack.GetStackItem( L, rtStack, n );

                    setnilvalue( clearItem.Pointer() );
                }
            }
        }
        else
        {
            // Post vararg arguments into visibility.
            int nargs = cast_int(prevTopOffset - funcOffsetAbsolute);
            {
                int actual = nargs;

                stackOffset_t nfixargs = p->numparams;

                // Cannot throw exceptions in this code region.
                rtStack.Lock( L );

                for ( ; actual < nfixargs; ++actual )
                {
                    if ( actual < newStackTopCount )
                    {
                        RtCtxItem newStackArgument = ciCtx.Pointer()->stack.GetStackItem( L, rtStack, actual );

                        setnilvalue( newStackArgument.Pointer() );
                    }
                }

                //StkId stackTop = ciCtx.Pointer()->stack.TopMutable( L, rtStack );

                stackOffset_t realArgOffset = ( funcOffsetAbsolute + 1 );

                /* move fixed parameters to final position */
                for ( stackOffset_t i = 0; i < nfixargs; i++ )
                {
                    if ( i < newStackTopCount )
                    {
                        RtCtxItem newStackArgument = ciCtx.Pointer()->stack.GetStackItem( L, rtStack, i );

                        RtCtxItem srcOldStackArg = rtStack.GetStackItem( L, realArgOffset + i );

                        setobj( L, newStackArgument.Pointer(), srcOldStackArg.Pointer() );
                        setnilvalue( srcOldStackArg.Pointer() );
                    }
                }

                rtStack.Unlock( L );
            }
        }
    }

    if ( lcl != nullptr )
    {
        Proto *p = lcl->p;

        L->savedpc = p->code;  /* starting point */

        if (L->hookmask & LUA_MASKCALL)
        {
            L->savedpc++;  /* hooks assume 'pc' is already incremented */

            luaD_callhook(L, LUA_HOOKCALL, -1);

            L->savedpc--;  /* correct 'pc' */
        }

        precallResult = PCRLUA;
    }
    else if ( ccl != nullptr )
    {
        // Return value of the C closure.
        int n = 0;

        if ( L->hookmask & LUA_MASKCALL )
        {
            luaD_callhook(L, LUA_HOOKCALL, -1);
        }

        {
            lua_CFunction cfunc = ccl->f;

            lua_unlock(L);
            n = cfunc(L);  /* do the actual call */
            lua_lock(L);
        }

        // Get the current on-stack count (as absolute offset).
        stackOffset_t resultTopOffsetAbsolute = ciCtx.Pointer()->stack.GetTopOffset( L, rtStack );

        if ( n < 0 )
        {
            RtCtxItem firstResultCtx = index2stackadr( L, -1 );

            luaD_poscall( L, &firstResultCtx, ci_model, resultTopOffsetAbsolute );
            lua_yield( L, 0 );
        }
        else
        {
            if ( n == 0 )
            {
                luaD_poscall( L, nullptr, ci_model, resultTopOffsetAbsolute );
            }
            else
            {
                RtCtxItem firstResultCtx = index2stackadr( L, -n );

                luaD_poscall(L, &firstResultCtx, ci_model, resultTopOffsetAbsolute);
            }
        }

        precallResult = PCRC;
    }

    return precallResult;
}


static inline void callrethooks (lua_State *L)
{
    luaD_callhook(L, LUA_HOOKRET, -1);

    if ( f_isLua(L->ciStack.Top()) )
    {  /* Lua function? */
        while ( (L->hookmask & LUA_MASKRET) && L->ciStack.Top()->tailcalls-- ) /* tail calls */
        {
            luaD_callhook(L, LUA_HOOKTAILRET, -1);
        }
    }
}

int luaD_poscall (lua_State *L, RtCtxItem *firstResultCtx, eCallFrameModel callFrameBehavior, stackOffset_t resultTopOffset)
{
    if ( L->hookmask & LUA_MASKRET )
    {
        callrethooks(L);
    }

    int wanted;
    {
        RtStackAddr rtStack = L->rtStack.LockedAcquisition( L );

        StkId res;
        stackOffset_t oldOnStackCount = 0;
        {
            CallInfo *lastCI = L->ciStack.Top();
            res = lastCI->func;  /* res == final position of 1st result */
            wanted = lastCI->nresults;
            oldOnStackCount = lastCI->stack.GetRelativeStackOffset( L, *rtStack, resultTopOffset ) + 1;
        }

        // Remove the stack frame.
        L->ciStack.Pop( L );

        // Get the pointer to the callinfo that we restored to.
        CallInfo *restoredFrame = L->ciStack.Top();

        L->savedpc = restoredFrame->savedpc;  /* restore savedpc */

        /* move results to correct place */
        RtStackView& currentStackFrame = restoredFrame->stack;

        stackOffset_t actualResultOffsetAbsolute = currentStackFrame.GetStackOffsetAbsolute( L, *rtStack, res );

        // When we leave this function, we want to have the top point to the last
        // argument that we moved to the correct place. The count that we need to increment
        // the top with is given by "wanted".

        StkId firstResult = ( firstResultCtx ) ? firstResultCtx->Pointer() : nullptr;

        stackOffset_t firstResultOffsetAbsolute;

        stackOffset_t reallyThereCount = 0;

        if ( firstResult != nullptr )
        {
            firstResultOffsetAbsolute = currentStackFrame.GetStackOffsetAbsolute( L, *rtStack, firstResult );

            reallyThereCount = ( resultTopOffset - firstResultOffsetAbsolute ) + 1;
        }

        stackOffset_t givenParams = 0;

        if ( wanted == LUA_MULTRET )
        {
            // If wanted is LUA_MULTRET, then we return as many values as the function provided us with.
            if ( firstResult != nullptr )
            {
                givenParams = reallyThereCount;
            }
        }
        else
        {
            givenParams = wanted;
        }

        stackOffset_t preparedResultCount = 0;
        {
            if ( firstResult != nullptr )
            {
                stackOffset_t moveCount = givenParams;

                // Make sure we do not move more than is on the stack.
                if ( moveCount > oldOnStackCount )
                {
                    moveCount = oldOnStackCount;
                }

                // Perform manual movement of stack items.
                // This has to be secured properly!
                lua_assert( firstResultOffsetAbsolute > actualResultOffsetAbsolute );

                for ( stackOffset_t n = 0; n < moveCount; n++ )
                {
                    RtCtxItem dstItem = rtStack->GetStackItem( L, actualResultOffsetAbsolute + n );

                    if ( n < reallyThereCount )
                    {
                        RtCtxItem srcItem = rtStack->GetStackItem( L, firstResultOffsetAbsolute + n );

                        setobj( L, dstItem.Pointer(), srcItem.Pointer() );
                    }
                    else
                    {
                        setnilvalue( dstItem.Pointer() );
                    }
                }

                preparedResultCount += moveCount;
            }
        }

        // Prepare the remainder by setting them to nil.
        for ( stackOffset_t n = preparedResultCount; n < givenParams; n++ )
        {
            RtCtxItem toBeNil = rtStack->GetStackItem( L, actualResultOffsetAbsolute + n );

            setnilvalue( toBeNil.Pointer() );
        }

        // Set the top to where it is supposed to be.
        // This is done if we are in the dynamic stack model.
        stackOffset_t requiredStackTop = ( actualResultOffsetAbsolute + givenParams - 1 );

        if ( callFrameBehavior == CALLFRAME_DYNAMIC )
        {
            currentStackFrame.SetTopOffset( L, *rtStack, requiredStackTop );
        }

        // Store the required stack top in the current stack frame.
        restoredFrame->lastResultTop = requiredStackTop;
    }

    return ( wanted - LUA_MULTRET );  /* return 0 if wanted == LUA_MULTRET */
}


/*
** Call a function (C or Lua). The function to be called is at *func.
** The arguments are on the stack, right after the function.
** When returns, all the results are on the stack, starting at the original
** function position.
*/
void luaD_call (lua_State *L, RtCtxItem& func, int nResults, eCallFrameModel callFrameBehavior, stackOffset_t topOffset)
{
    callstack_ref ref( *L );
    stackOffset_t cioff; L->ciStack.GetTopOffset( L, cioff );

    try
    {
        if ( luaD_precall( L, func, nResults, topOffset, callFrameBehavior ) == PCRLUA )  /* is a Lua function? */
        {
            luaV_execute( L, 1, callFrameBehavior );  /* call it */
        }
    }
    catch( ... )
    {
        // Restore basic business
        L->ciStack.SetTopOffset( L, cioff );
        //L->base = L->ciStack.Top()->base;
        throw;
    }

    luaC_checkGC(L);
}

inline static int resume_error (lua_State *L, const char *msg)
{
    L->GetCurrentStackFrame().Clear( L, L->rtStack );

    TString *errorString = luaS_new(L, msg);

    try
    {
        pushsvalue(L, errorString);
    }
    catch( ... )
    {
        errorString->DereferenceGC( L );
        throw;
    }

    // Since the error string is now on the stack, we can dereference it.
    errorString->DereferenceGC( L );

    lua_unlock(L);
    return LUA_ERRRUN;
}

LUA_API int lua_resume (lua_State *_L, int nargs)
{
    lua_lock(L);

	if ( !_L->IsThread() )
        return resume_error(_L, "not a thread");

	lua_Thread *L = (lua_Thread*)_L;

    if ( L->status != THREAD_SUSPENDED )
        return resume_error(L, "cannot resume non-suspended coroutine");

    if ( L->nCcalls >= LUAI_MAXCCALLS )
        return resume_error(L, "C stack overflow");

    // Allocate OS resources
    if ( !L->AllocateRuntime() )
        return resume_error( L, "failed to allocate OS resources (too many coroutines running?)" );

    lua_State *main = G(L)->mainthread;

    lua_checkstack( main, 2 );

    luai_userstateresume( L, nargs );

    if ( Closure *evtCall = G(main)->events[LUA_EVENT_THREAD_CONTEXT_PUSH] )
    {
        pushclvalue( main, evtCall );
        lua_pushthreadex( main, L );
        lua_call( main, 1, 0 );
    }

    callstack_ref ref( *L );

    // The OS thread is saved on it's position
    ((lua_Thread*)L)->resume();

    if ( Closure *evtCall = G(main)->events[LUA_EVENT_THREAD_CONTEXT_POP] )
    {
        pushclvalue( main, evtCall );
        lua_call( main, 0, 0 );
    }

    lua_unlock(L);
    luai_userstateyield(L, nresults);
    return L->status;
}

LUA_API int lua_yield( lua_State *L, int nresults )
{
    if ( !L->IsThread() )
        throw lua_exception( L, LUA_ERRRUN, "cannot yield from main thread" );

    if ( L->IsYieldDisabled() )
        throw lua_exception( L, LUA_ERRRUN, "cannot yield at current runtime" );

    // TODO: I really have to decide here whether code execution without prior runtime
    // resumation should be possible. Imagine that the user runs code on a coroutine
    // which suddenly yields. If there is no previous Lua thread to take the OS yield-back
    // query, Lua will deadlock. This is prevented by the simple rule that only main threads
    // may execute code in such a way. Otherwise it is the programmer's mistake.

    lua_lock(L);
    {
        RtStackView::baseProtect baseProt = L->GetCurrentStackFrame().ProtectSlots( L, L->rtStack, -nresults ); /* protect stack slots below */

        // Give back control to the previous thread
        ((lua_Thread*)L)->yield();
    }
    lua_unlock(L);
    return 0;
}

int luaD_pcall (lua_State *L, Pfunc func, void *u, stackOffset_t old_top, stackOffset_t ef, lua_Debug *debug)
{
    int status;
    ptrdiff_t old_errfunc = L->errfunc;
    std::string errmsg;
    L->errfunc = ef;
    status = luaD_rawrunprotected(L, func, u, errmsg, debug);

    if (status != 0)
    {  /* an error occurred? */
        RtCtxItem oldtop = restorestack(L, old_top);
        luaF_close(L, oldtop.Pointer());  /* close eventual pending closures */
        // We must set the top just below the called function.
        {
            RtStackView& curStackFrame = L->GetCurrentStackFrame();
            stackOffset_t subFrameOffset = curStackFrame.GetStackOffset( L, L->rtStack, oldtop.Pointer() );
            curStackFrame.SetTopOffset( L, L->rtStack, subFrameOffset );
            curStackFrame.PopCount( L, L->rtStack, 1 );
        }
        lua_pushlstring( L, errmsg.c_str(), errmsg.size() );
        L->savedpc = L->ciStack.Top()->savedpc;
        restore_stack_limit(L);
    }

    L->errfunc = old_errfunc;
    return status;
}


/*
** Execute a protected parser.
*/
struct SParser {  /* data to `f_parser' */
  ZIO *z;
  Mbuffer buff;  /* buffer to be used by the scanner */
  const char *name;
};

static void f_parser (lua_State *L, void *ud)
{
    struct SParser *p = cast(struct SParser *, ud);
    int c = luaZ_lookahead(p->z);

    luaC_checkGC(L);

    Proto *tf = ((c == LUA_SIGNATURE[0]) ? luaU_undump : luaY_parser)(L, p->z, &p->buff, p->name);

    try
    {
        LClosure *cl = luaF_newLclosure(L, tf->nups, gcvalue(gt(L)));

        cl->p = tf;

        try
        {

            for ( int i = 0; i < tf->nups; i++ )  /* initialize eventual upvalues */
            {
                UpVal *theUpValue = luaF_newupval(L);

                cl->upvals[i] = theUpValue;

                // Since the upvalue is now stored in a reference Lua closure, we can dereference it.
                theUpValue->DereferenceGC( L );
            }

            pushclvalue(L, cl);
        }
        catch( ... )
        {
            // Pushing on the stack failure or memory allocation failure.
            // Everything has to be accounted for.
            cl->DereferenceGC( L );
            throw;
        }

        // Since the Lua closure is now on the stack, we can dereference it.
        cl->DereferenceGC( L );
    }
    catch( ... )
    {
        tf->DereferenceGC( L );
        throw;
    }

    // Dereference the proto aswell.
    tf->DereferenceGC( L );

    lua_assert( tf->GetGCRefCount() == 0 );
}

int luaD_protectedparser (lua_State *L, ZIO *z, const char *name)
{
    struct SParser p;
    p.z = z; p.name = name;
    luaZ_initbuffer(L, &p.buff);

    lua_Debug deb;

    stackOffset_t top_offset;

    top_offset = L->GetCurrentStackFrame().GetTopOffset( L, L->rtStack );

    int status = luaD_pcall(L, f_parser, &p, top_offset, L->errfunc, &deb);

    if ( status != 0 && deb.currentline != -1 )
    {
        lua_pushstring( L, deb.short_src );
        lua_pushlstring( L, ":", 1 );
        lua_pushnumber( L, deb.currentline );
        lua_pushlstring( L, ": ", 2 );
        lua_concat( L, 4 );
        lua_insert( L, -2 );
        lua_concat( L, 2 );
    }

    luaZ_freebuffer(L, &p.buff);
    return status;
}

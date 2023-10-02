/*
** $Id: lfunc.c,v 2.12.1.2 2007/12/28 14:58:43 roberto Exp $
** Auxiliary functions to manipulate prototypes and closures
** See Copyright Notice in lua.h
*/


#include "luacore.h"

#include "lapi.hxx"
#include "lfunc.hxx"

// Global state plugin definitions.
closureEnvConnectingBridge_t closureEnvConnectingBridge;

// Closure type information plugin.
closureTypeInfo_t closureTypeInfo;

TValue* luaF_getcurraccessor( lua_State *L )
{
    if ( hascallingenv( L ) )
    {
        if ( CClosure *cl = curr_func( L )->GetCClosure() )
        {
            setgcvalue( L, &L->env, cl->accessor );
            return &L->env;
        }
    }

    return &L->storage;
}

CClosureMethodRedirect::CClosureMethodRedirect( global_State *g, void *construction_params ) : CClosure( g, construction_params )
{
    this->redirect = nullptr;
    this->m_class = nullptr;

    this->nupvalues = 0;

    this->isC = true;
    this->isEnvLocked = true;
}

CClosureMethodRedirect::~CClosureMethodRedirect( void )
{
    return;
}

CClosureMethodRedirect* luaF_newCmethodredirect( lua_State *L, GCObject *e, Closure *redirect, Class *j )
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = 0;
        params.runtimeThread = L;

        CClosureMethodRedirect *c = lua_new <CClosureMethodRedirect> ( L, typeInfo->cclosureMethodRedirectTypeInfo, &params );

        if ( c )
        {
            c->env = e;
            c->redirect = redirect;
            c->m_class = j;
        }

        return c;
    }
    return nullptr;
}

TValue* CClosureMethodRedirect::ReadUpValue( unsigned char index )
{
    return (TValue*)luaO_nilobject;
}

CClosureMethodRedirectSuper::CClosureMethodRedirectSuper( global_State *g, void *construction_params ) : CClosure( g, construction_params )
{
    this->redirect = nullptr;
    this->m_class = nullptr;
    this->super = nullptr;

    this->nupvalues = 0;

    this->isC = true;
    this->isEnvLocked = true;
}

CClosureMethodRedirectSuper::~CClosureMethodRedirectSuper( void )
{
    return;
}

CClosureMethodRedirectSuper* luaF_newCmethodredirectsuper( lua_State *L, GCObject *e, Closure *redirect, Class *j, Closure *super )
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = 0;
        params.runtimeThread = L;

        CClosureMethodRedirectSuper *c = lua_new <CClosureMethodRedirectSuper> ( L, typeInfo->cclosureMethodRedirectSuperTypeInfo, &params );

        if ( c )
        {
            c->env = e;
            c->redirect = redirect;
            c->m_class = j;
            c->super = super;
        }
        return c;
    }
    return nullptr;
}

TValue* CClosureMethodRedirectSuper::ReadUpValue( unsigned char index )
{
    return (TValue*)luaO_nilobject;
}

CClosureBasic::CClosureBasic( global_State *g, void *construction_params ) : CClosure( g, construction_params )
{
    cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

    int nelems = 0;

    if ( params )
    {
        nelems = params->nelems;
    }

    this->nupvalues = nelems;

    this->isC = true;
    this->isEnvLocked = false;
}

CClosureBasic::~CClosureBasic( void )
{
    return;
}

CClosureBasic* luaF_newCclosure (lua_State *L, int nelems, GCObject *e)
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = nelems;
        params.runtimeThread = L;

        CClosureBasic *c = lua_new <CClosureBasic> ( L, typeInfo->cclosureBasicTypeInfo, &params );

        if ( c )
        {
            c->env = e;
        }
        return c;
    }
    return nullptr;
}

TValue* CClosureBasic::ReadUpValue( unsigned char index )
{
    if ( index >= nupvalues )
        return (TValue*)luaO_nilobject; // we trust the programmer that he will not write into the nil object (possibly through upvalues)

    return &upvalues[index];
}

CClosureMethod::CClosureMethod( global_State *g, void *construction_params ) : CClosureMethodBase( g, construction_params )
{
    cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

    int nelems = 0;

    if ( params )
    {
        nelems = params->nelems;
    }

    this->nupvalues = nelems;

    this->isC = true;
    this->isEnvLocked = true;

    // TODO: maybe nil the upvalue slots.
}

CClosureMethod::~CClosureMethod( void )
{
    return;
}

CClosureMethod* luaF_newCmethod( lua_State *L, int nelems, GCObject *e, Class *j )
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = nelems;
        params.runtimeThread = L;

        CClosureMethod *c = lua_new <CClosureMethod> ( L, typeInfo->cclosureMethodTypeInfo, &params );

        if ( c )
        {
            c->env = e;
            c->m_class = j;
        }
        return c;
    }
    return nullptr;
}

TValue* CClosureMethod::ReadUpValue( unsigned char index )
{
    if ( index >= nupvalues )
        return (TValue*)luaO_nilobject; // we trust the programmer that he will not write into the nil object (possibly through upvalues)

    return &upvalues[index];
}

CClosureMethodTrans::CClosureMethodTrans( global_State *g, void *construction_params ) : CClosureMethodBase( g, construction_params )
{
    cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

    int nelems = 0;

    if ( params )
    {
        nelems = params->nelems;
    }

    this->nupvalues = nelems;
    this->trans = 0;
    this->data = nullptr;

    this->isC = true;
    this->isEnvLocked = true;
}

CClosureMethodTrans::~CClosureMethodTrans( void )
{
    return;
}

CClosureMethodTrans* luaF_newCmethodtrans( lua_State *L, int nelems, GCObject *e, Class *j, int trans )
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = nelems;
        params.runtimeThread = L;

        CClosureMethodTrans *c = lua_new <CClosureMethodTrans> ( L, typeInfo->cclosureMethodTransTypeInfo, &params );

        if ( c )
        {
            c->env = e;
            c->m_class = j;
            c->trans = trans;
        }
        return c;
    }
    return nullptr;
}

TValue* CClosureMethodTrans::ReadUpValue( unsigned char index )
{
    if ( index >= nupvalues )
        return (TValue*)luaO_nilobject; // we trust the programmer that he will not write into the nil object (possibly through upvalues)

    return &upvalues[index];
}

CClosureMethodBase::CClosureMethodBase( global_State *g, void *construction_params ) : CClosure( g, construction_params )
{
    this->m_class = nullptr;
    this->method = nullptr;
    this->super = nullptr;
}

CClosureMethodBase::~CClosureMethodBase( void )
{
    return;
}

CClosure::CClosure( global_State *g, void *construction_params ) : Closure( g )
{
    cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

    lua_State *L = g->mainthread;

    if ( params )
    {
        L = params->runtimeThread;
    }

    this->f = nullptr;
    this->accessor = gcvalue( luaF_getcurraccessor( L ) );
}

CClosure::~CClosure( void )
{
}

LClosure::LClosure( global_State *g, void *construction_params ) : Closure( g )
{
    cclosureSharedConstructionParams *params = (cclosureSharedConstructionParams*)construction_params;

    int nelems = 0;

    if ( params )
    {
        nelems = params->nelems;
    }

    this->nupvalues = nelems;
    this->p = nullptr;

    this->nupvalues = cast_byte(nelems);

    for ( int n = 0; n < nelems; n++ )
    {
        this->upvals[ n ] = nullptr;
    }
}

LClosure::~LClosure( void )
{
    return;
}

LClosure *luaF_newLclosure (lua_State *L, int nelems, GCObject *e)
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        cclosureSharedConstructionParams params;
        params.nelems = nelems;
        params.runtimeThread = L;

        LClosure *c = lua_new <LClosure> ( L, typeInfo->lclosureTypeInfo, &params );

        if ( c )
        {
            c->env = e;
        }

        return c;
    }
    return nullptr;
}

Closure::Closure( global_State *g ) : GrayObject( g )
{
    this->nupvalues = 0;
    this->env = nullptr;
    this->genFlags = 0;

    luaC_link( g, this, LUA_TFUNCTION );
}

Closure::Closure( const Closure& right ) : GrayObject( right )
{
    throw lua_exception( this->gstate->mainthread, LUA_ERRRUN, "attempt to clone a closure", 1 );
}

Closure::~Closure( void )
{
    global_State *g = this->gstate;

    luaC_unlink( g, this );
}

void luaF_freeclosure (lua_State *L, Closure *c)
{
    lua_delete <Closure> ( L, c );
}

TValue* LClosure::ReadUpValue( unsigned char index )
{
    if ( index > nupvalues )
        return (TValue*)luaO_nilobject;

    return upvals[index]->v;
}

UpVal *luaF_newupval (lua_State *L)
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        UpVal *uv = lua_new <UpVal> ( L, typeInfo->upvalueTypeInfo );

        return uv;
    }
    return nullptr;
}

void luaF_freeupval (lua_State *L, UpVal *u)
{
    lua_delete <UpVal> ( L, u );
}

// WARNING: do not rellocate stack in this function!
UpVal *luaF_findupval (lua_State *L, StkId level)
{
    global_State *g = G(L);
    gcObjList_t::removable_iterator iter = L->openupval.GetRemovableIterator();
    UpVal *uv = nullptr;

    {
        UpVal *p;

        while (!iter.IsEnd() && (p = ngcotouv((GCObject*)iter.Resolve()))->v >= level)
        {
            lua_assert(p->v != &p->u.value);

            if (p->v == level)
            {  /* found a corresponding upvalue? */
                if (isdead(g, p))  /* is it dead? */
                {
                    // TODO: this does not belong here.
                    changewhite(p);  /* ressurect it */
                }

                return p;
            }
            iter.Increment();
        }
    }

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        uv = lua_new <UpVal> ( L, typeInfo->upvalueTypeInfo );    /* not found: create a new one */

        if ( uv )
        {
            uv->v = level;  /* current value lives in the stack */

            iter.Insert( uv );  /* chain it in the proper position */

            {
                globalStateClosureEnvPlugin *closureEnv = closureEnvConnectingBridge.GetPluginStruct( g->config, g );

                if ( closureEnv )
                {
                    LIST_INSERT( closureEnv->uvhead.root, uv->u.l ); /* double link it in `uvhead' list */

                    lua_assert( LIST_ISVALID( uv->u.l ) == true );
                }
                else
                {
                    // Hack for no closure env, since the node must be valid.
                    LIST_CLEAR( uv->u.l );
                }
            }

            // We can now dereference it from GC.
            uv->DereferenceGC( L );
        }
    }
    return uv;
}

static void unlinkupval (UpVal *uv)
{
    lua_assert( LIST_ISVALID( uv->u.l ) == true );

    LIST_REMOVE( uv->u.l );  /* remove from `uvhead' list */
}

UpVal::UpVal( global_State *g, void *construction_params ) : GCObject( g )
{
    luaC_register( g, this, LUA_TOBJECT );

    this->v = &this->u.value;       // initially the upvalue is closed.
    setnilvalue( &this->u.value );
}

UpVal::~UpVal( void )
{
    if ( v != &u.value )  /* is it open? */
    {
        unlinkupval( this );  /* remove from open list */
    }
    else
    {
        global_State *g = this->gstate;

        lua_State *L = g->mainthread;

        luaC_unlinkupval( L, this );
    }
}

// WARNING: no stack rellocation in this function supported!
void luaF_close (lua_State *L, StkId_const level)
{
    UpVal *uv;
    global_State *g = G(L);

    while (L->openupval.GetFirst() != nullptr && (uv = ngcotouv(L->openupval.GetFirst()))->v >= level)
    {
        GCObject *o = uv;

        lua_assert(!isblack(o) && uv->v != &uv->u.value);

        L->openupval.RemoveFirst();  /* remove from `open' list */

        if (isdead(g, o))
        {
            luaF_freeupval( L, uv ); /* free upvalue */
        }
        else
        {
            unlinkupval(uv);

            setobj(L, &uv->u.value, uv->v);

            uv->v = &uv->u.value;  /* now current value lives here */
            luaC_linkupval(L, uv);  /* link upvalue into `gcroot' list */
        }
    }
}

Proto *luaF_newproto (lua_State *L)
{
    global_State *g = G(L);

    closureTypeInfoPlugin_t *typeInfo = closureTypeInfo.GetPluginStruct( g->config );

    if ( typeInfo )
    {
        Proto *f = lua_new <Proto> ( L, typeInfo->protoTypeInfo );

        return f;
    }
    return nullptr;
}

Proto::Proto( global_State *g, void *construction_params ) : GrayObject( g )
{
    // Initialize ourselves.
    luaC_link(g, this, LUA_TOBJECT);

    this->k = nullptr;
    this->sizek = 0;
    this->p = nullptr;
    this->sizep = 0;
    this->code = nullptr;
    this->sizecode = 0;
    this->sizelineinfo = 0;
    this->sizeupvalues = 0;
    this->nups = 0;
    this->upvalues = nullptr;
    this->numparams = 0;
    this->is_vararg = 0;
    this->maxstacksize = 0;
    this->lineinfo = nullptr;
    this->sizelocvars = 0;
    this->locvars = nullptr;
    this->linedefined = 0;
    this->lastlinedefined = 0;
    this->source = nullptr;
}

Proto::Proto( const Proto& right ) : GrayObject( right )
{
    global_State *g = this->gstate;

    luaC_link( g, this, LUA_TOBJECT );

    // Clone everything.
    this->source = right.source;

    size_t sizek = right.sizek;
    size_t sizecode = right.sizecode;
    size_t sizelineinfo = right.sizelineinfo;
    size_t sizep = right.sizep;
    size_t sizelocvars = right.sizelocvars;
    size_t sizeupvalues = right.sizeupvalues;

    this->sizek = sizek;
    this->sizecode = sizecode;
    this->sizelineinfo = sizelineinfo;
    this->sizep = sizep;
    this->sizelocvars = sizelocvars;
    this->sizeupvalues = sizeupvalues;

    this->linedefined = right.linedefined;
    this->lastlinedefined = right.lastlinedefined;
    this->nups = right.nups;
    this->numparams = right.numparams;
    this->is_vararg = right.is_vararg;
    this->maxstacksize = right.maxstacksize;

    // Clone data arrays.
    lua_State *L = g->mainthread;

    TValue *k = luaM_clonevector( L, right.k, sizek );

    try
    {
        Instruction *code = luaM_clonevector( L, right.code, sizecode );

        try
        {
            int *lineinfo = luaM_clonevector( L, right.lineinfo, sizelineinfo );

            try
            {
                Proto **p = luaM_clonevector( L, right.p, sizep );

                try
                {
                    LocVar *locvars = luaM_clonevector( L, right.locvars, sizelocvars );

                    try
                    {
                        this->upvalues = luaM_clonevector( L, right.upvalues, sizeupvalues );
                    }
                    catch( ... )
                    {
                        if ( locvars )
                        {
                            luaM_freearray( L, locvars, sizelocvars );
                        }

                        throw;
                    }

                    this->locvars = locvars;
                }
                catch( ... )
                {
                    if ( p )
                    {
                        luaM_freearray( L, p, sizep );
                    }

                    throw;
                }

                this->p = p;
            }
            catch( ... )
            {
                if ( lineinfo )
                {
                    luaM_freearray( L, lineinfo, sizelineinfo );
                }

                throw;
            }

            this->lineinfo = lineinfo;
        }
        catch( ... )
        {
            if ( code )
            {
                luaM_freearray( L, code, sizecode );
            }

            throw;
        }

        this->code = code;
    }
    catch( ... )
    {
        if ( k )
        {
            luaM_freearray( L, k, sizek );
        }

        throw;
    }

    this->k = k;
}

Proto::~Proto( void )
{
    global_State *g = this->gstate;

    lua_State *L = g->mainthread;

    // Free all used resources.
    luaM_freearray(L, this->code, this->sizecode);
    luaM_freearray(L, this->p, this->sizep);
    luaM_freearray(L, this->k, this->sizek);
    luaM_freearray(L, this->lineinfo, this->sizelineinfo);
    luaM_freearray(L, this->locvars, this->sizelocvars);
    luaM_freearray(L, this->upvalues, this->sizeupvalues);

    luaC_unlink( g, this );
}

void luaF_freeproto (lua_State *L, Proto *p)
{
    lua_delete <Proto> ( L, p );
}

ConstValueAddress luaF_getprotoconstaddress (lua_State *L, Proto *p, int idx)
{
    ConstValueAddress theAddr;
    {
        global_State *g = G(L);

        globalStateClosureEnvPlugin *closureEnv = closureEnvConnectingBridge.GetPluginStruct( g->config, g );

        if ( closureEnv )
        {
            globalStateClosureEnvPlugin::ProtoConstValueAccessContext *valCtx = closureEnv->NewProtoConstValueAccessContext( L, p );

            valCtx->theValue = &p->k[ idx ];

            theAddr = ConstValueAddress( L, valCtx );
        }
    }
    return theAddr;
}

/*
** Look for n-th local variable at line `line' in function `func'.
** Returns NULL if not found.
*/
const char *luaF_getlocalname( const Proto *f, stackOffset_t local_number, unsigned int pc )
{
    for ( size_t i = 0; i<f->sizelocvars && f->locvars[i].startpc <= pc; i++ )
    {
        if ( pc < f->locvars[i].endpc )
        {  /* is variable active? */
            local_number--;

            if (local_number == 0)
            {
                return getstr(f->locvars[i].varname);
            }
        }
    }

    return nullptr;  /* not found */
}

// Module initialization.
void luaF_init( lua_config *cfg )
{
    return;
}

void luaF_shutdown( lua_config *cfg )
{
    return;
}

void luaF_libraryinit( void )
{
    closureEnvConnectingBridge.RegisterPluginStruct( namespaceFactory );
    closureTypeInfo.RegisterPlugin( namespaceFactory );
}

void luaF_libraryshutdown( void )
{
    closureTypeInfo.UnregisterPlugin();
    closureEnvConnectingBridge.UnregisterPluginStruct();
}

// Runtime initialization.
void luaF_runtimeinit( global_State *g )
{
    return;
}

void luaF_runtimeshutdown( global_State *g )
{
    // Clear memory.
    globalStateClosureEnvPlugin *closureEnv = closureEnvConnectingBridge.GetPluginStruct( g->config, g );

    if ( closureEnv )
    {
        closureEnv->ClearMemory( g );
    }
}

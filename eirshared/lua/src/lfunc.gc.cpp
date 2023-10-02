#include "luacore.h"

#include "lfunc.hxx"

#include "lfunc.class.hxx"
#include "lfunc.upval.hxx"

// Include garbage collector definitions.
#include "lgc.interface.hxx"

void luaF_gcruntime( lua_State *L )
{
    global_State *g = G(L);

    globalStateClosureEnvPlugin *closureEnv = closureEnvConnectingBridge.GetPluginStruct( g->config, g );

    if ( closureEnv )
    {
        /* remark occasional upvalues of (maybe) dead threads */
        LIST_FOREACH_BEGIN( UpVal, closureEnv->uvhead.root, u.l )

            lua_assert( LIST_ISVALID( *iter ) == true );

            if ( isgray(item) )
            {
                markvalue(g, item->v);
            }

        LIST_FOREACH_END
    }
}

void UpVal::MarkGC( global_State *g )
{
    markvalue( g, v );

    if ( v == &u.value )  /* closed? */
    {
        gray2black( this );  /* open upvalues are never black */
    }
}

/*
** All marks are conditional because a GC may happen while the
** prototype is still being created
*/
int Proto::TraverseGC( global_State *g )
{
    if ( source )
        stringmark( source );

    for ( size_t i = 0; i < sizek; i++ )  /* mark literals */
    {
        markvalue( g, &k[i] );
    }

    for ( size_t i = 0; i < sizeupvalues; i++ )
    {  /* mark upvalue names */
        if ( TString *upval_name = upvalues[i] )
        {
            stringmark( upval_name );
        }
    }

    for ( size_t i = 0; i < sizep; i++ )
    {  /* mark nested protos */
        if ( Proto *curr_p = p[i] )
        {
            markobject( g, curr_p );
        }
    }

    for ( size_t i = 0; i < sizelocvars; i++)
    {  /* mark local-variable names */
        if ( TString *locvar_name = locvars[i].varname )
        {
            stringmark( locvar_name );
        }
    }

    return 0;
}

int Closure::TraverseGC( global_State *g )
{
    markobject( g, env );
    return 0;
}

int LClosure::TraverseGC( global_State *g )
{
    lu_byte nupvalues = this->nupvalues;

    if ( Proto *p = this->p )
    {
        lua_assert( nupvalues == p->nups );

        markobject( g, p );
    }

    for ( unsigned int i = 0; i < nupvalues; i++ )  /* mark its upvalues */
    {
        markobject( g, upvals[i] );
    }

    return Closure::TraverseGC( g );
}

int CClosure::TraverseGC( global_State *g )
{
    markobject( g, accessor );

    return Closure::TraverseGC( g );
}

size_t CClosureMethodRedirect::Propagate( global_State *g )
{
    markobject( g, redirect );
    markobject( g, m_class );

    return CClosure::Propagate( g ) + sizeof(*this);
}

size_t CClosureMethodRedirectSuper::Propagate( global_State *g )
{
    markobject( g, redirect );
    markobject( g, m_class );
    markobject( g, super );

    return CClosure::Propagate( g ) + sizeof(*this);
}

size_t CClosureBasic::Propagate( global_State *g )
{
    for ( unsigned int i = 0; i < nupvalues; i++ )  /* mark its upvalues */
    {
        markvalue( g, &upvalues[i] );
    }

    return CClosure::Propagate( g ) + sizeCclosure( nupvalues );
}

size_t CClosureMethodBase::Propagate( global_State *g )
{
    markobject( g, m_class );

    if ( super )
        markobject( g, super );

    return CClosure::Propagate( g );
}

size_t CClosureMethod::Propagate( global_State *g )
{
    for ( unsigned int i = 0; i < nupvalues; i++ )  /* mark its upvalues */
    {
        markvalue( g, &upvalues[i] );
    }

    return CClosureMethodBase::Propagate( g ) + sizeCmethod( nupvalues );
}

size_t CClosureMethodTrans::Propagate( global_State *g )
{
    for ( unsigned int i = 0; i < nupvalues; i++ )  /* mark its upvalues */
    {
        markvalue( g, &upvalues[i] );
    }

    return CClosureMethodBase::Propagate( g ) + sizeCmethodt( nupvalues );
}

size_t LClosure::Propagate( global_State *g )
{
    TraverseGC( g );
    return sizeLclosure( nupvalues );
}

size_t CClosure::Propagate( global_State *g )
{
    TraverseGC( g );
    return 0;
}

size_t Proto::Propagate( global_State *g )
{
    TraverseGC( g );

    return sizeof(Proto) + sizeof(Instruction) * sizecode +
        sizeof(Proto *) * sizep +
        sizeof(TValue) * sizek + 
        sizeof(int) * sizelineinfo +
        sizeof(LocVar) * sizelocvars +
        sizeof(TString *) * sizeupvalues;
}
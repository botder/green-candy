// Lua Class type GC runtime.
#include "luacore.h"

#include "lclass.hxx"

#include "lgc.interface.hxx"
#include "lgc.internal.hxx"

void luaJ_gcruntime( lua_State *L )
{
    global_State *g = G(L);

    globalStateClassEnvPlugin *classEnv = GetGlobalClassEnv( g );

    if ( classEnv )
    {
        // Mark special global values that are used by the class runtime.
        if ( TString *superCached = classEnv->superCached )
        {
            stringmark( superCached );   // 'super' used by classes
        }
    }
}

template <class typeinfo>
void DynamicStringTable <typeinfo>::TraverseGC( global_State *g )
{
    LIST_FOREACH_BEGIN( STableItemHeader, m_list.root, node )
        stringmark( item->key );
        typeinfo::MarkValue( g, GetNodeValue( item ) );
    LIST_FOREACH_END
}

int Class::TraverseGC( global_State *g )
{
    if ( destroyed )
        return 0;

    if ( destructor )
        markobject( g, destructor );

    if ( Dispatch *classEnv = this->env )
    {
        markobject( g, classEnv );
    }
    if ( Dispatch *classOutEnv = this->outenv )
    {
        markobject( g, classOutEnv );
    }
    if ( Table *classStorage = this->storage )
    {
        markobject( g, classStorage );
    }
    if ( Table *classInternStorage = this->internStorage )
    {
        markobject( g, classInternStorage );
    }

    if ( parent )
    {
        markobject( g, parent );
        markobject( g, childAPI );
    }

    globalStateGCEnv *gcEnv = GetGlobalGCEnvironment( g );

    if ( gcEnv )
    {
        lua_State *L = gcEnv->GCthread;

        LIST_FOREACH_BEGIN( Class, children.root, child_iter )
            // Be able to garbage collect children if they set themselves weak.
            const TValue *weakMeth = item->GetEnvValueString( L, "isWeakChildLink" );
            bool markChild = true;

            if ( ttype( weakMeth ) == LUA_TFUNCTION )
            {
                pushtvalue( L, weakMeth );
                bool success = lua_pcall( L, 0, 1, 0 ) == 0;

                markChild = ( success && ( lua_toboolean( L, -1 ) == false ) );
                popstack( L, 1 );   // we can pop the error message or the boolean with this.
            }

            if ( markChild )
            {
                markobject( g, item );
            }
        LIST_FOREACH_END
    }

    if ( Class::ForceSuperTable *forceSuper = this->forceSuper )
    {
        forceSuper->TraverseGC( g );
    }

    if ( methodCache )
    {
        methodCache->TraverseGC( g );
    }

    for ( envList_t::const_iterator iter = envInherit.begin(); iter != envInherit.end(); iter++ )
    {
        markobject( g, *iter );
    }

    return 0;
}

// User-mode function
void Class::Propagate( lua_State *L )
{
    markobject( G(L), this );
}
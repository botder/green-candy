#ifndef _LUA_DISPATCH_CLASS_HANDLERS_
#define _LUA_DISPATCH_CLASS_HANDLERS_

class ClassDispatch abstract : public Dispatch
{
public:
    inline ClassDispatch( global_State *g ) : Dispatch( g )
    {
        this->m_class = nullptr;
    }

    size_t                  Propagate( global_State *g );

    bool                    GCRequiresBackBarrier( void ) const     { return true; }

    Class*                  GetClass()      { return m_class; }

    Class*                  m_class;
};

class ClassEnvDispatch : public ClassDispatch
{
public:
    inline ClassEnvDispatch( global_State *g, void *construct_params ) : ClassDispatch( g )
    {
        return;
    }

    void                    Index( lua_State *L, ConstValueAddress& key, ValueAddress& val );
    void                    NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );
};

class ClassOutEnvDispatch : public ClassDispatch
{
public:
    inline ClassOutEnvDispatch( global_State *g, void *construct_params ) : ClassDispatch( g )
    {
        return;
    }

    void                    Index( lua_State *L, ConstValueAddress& key, ValueAddress& val );
    void                    NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );
};

class ClassMethodDispatch : public ClassDispatch
{
public:
    inline ClassMethodDispatch( global_State *g, void *construct_params ) : ClassDispatch( g )
    {
        this->m_prevEnv = nullptr;
    }

    size_t                  Propagate( global_State *g );

    void                    Index( lua_State *L, ConstValueAddress& key, ValueAddress& val );
    void                    NewIndex( lua_State *L, ConstValueAddress& key, ConstValueAddress& val );

    GCObject*               m_prevEnv;
};

// Private class-env dispatch API.
LUAI_FUNC ClassEnvDispatch*     luaQ_newclassenv( lua_State *L, Class *j );
LUAI_FUNC ClassOutEnvDispatch*  luaQ_newclassoutenv( lua_State *L, Class *j );
LUAI_FUNC ClassMethodDispatch*  luaQ_newclassmethodenv( lua_State *L, Class *j, GCObject *prevEnv );

#endif //_LUA_DISPATCH_CLASS_HANDLERS_
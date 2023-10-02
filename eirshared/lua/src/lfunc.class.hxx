#ifndef _LUA_CLASS_FUNCTION_ABSTRACTION_
#define _LUA_CLASS_FUNCTION_ABSTRACTION_

class CClosureMethodRedirect : public CClosure
{
public:
    CClosureMethodRedirect( global_State *g, void *construction_params );

    ~CClosureMethodRedirect( void );

    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char index );

    Closure* redirect;
    Class* m_class;
};

class CClosureMethodRedirectSuper : public CClosure
{
public:
    CClosureMethodRedirectSuper( global_State *g, void *construction_params );

    ~CClosureMethodRedirectSuper( void );

    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char index );

    Closure *redirect;
    Class *m_class;
    Closure *super;
};

class CClosureMethodBase abstract : public CClosure
{
public:
    CClosureMethodBase( global_State *g, void *construction_params );

    ~CClosureMethodBase( void );

    size_t Propagate( global_State *g );

    Class*          m_class;
    lua_CFunction   method;
    Closure*        super;
};

class CClosureMethod : public CClosureMethodBase
{
public:
    CClosureMethod( global_State *g, void *construction_params );

    ~CClosureMethod( void );

    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char index );

    TValue  upvalues[1];
};

class CClosureMethodTrans : public CClosureMethodBase
{
public:
    CClosureMethodTrans( global_State *g, void *construction_params );

    ~CClosureMethodTrans( void );

    size_t Propagate( global_State *g );

    TValue* ReadUpValue( unsigned char index );

    unsigned char trans;
    void *data;
    TValue  upvalues[1];
};

FASTAPI size_t sizeCmethod( int n )             { return sizeof(CClosureMethod) + ( sizeof(TValue)*((n)-1) ); }
FASTAPI size_t sizeCmethodt( int n )            { return sizeof(CClosureMethodTrans) + ( sizeof(TValue)*((n)-1) ); }

// Private class method API.
LUAI_FUNC CClosureMethodRedirect* luaF_newCmethodredirect( lua_State *L, GCObject *e, Closure *redirect, Class *j );
LUAI_FUNC CClosureMethodRedirectSuper* luaF_newCmethodredirectsuper( lua_State *L, GCObject *e, Closure *redirect, Class *j, Closure *super );
LUAI_FUNC CClosureMethod *luaF_newCmethod( lua_State *L, int nelems, GCObject *e, Class *j );
LUAI_FUNC CClosureMethodTrans *luaF_newCmethodtrans( lua_State *L, int nelems, GCObject *e, Class *j, int trans );

#endif //_LUA_CLASS_FUNCTION_ABSTRACTION_
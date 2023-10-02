// Lua global header with utilities that are not as important.
#ifdef __cplusplus
#include <list>

// Helper class for class referencing
class lua_class_reference
{
public:
    lua_class_reference( void )
    {
    }

    void init( lua_State *L, ILuaClass *j )
    {
        m_lua = L;
        m_class = j;

        j->IncrementMethodStack( L );
    }

    lua_class_reference( lua_State *L, int idx )
    {
        if ( lua_type( L, idx ) != LUA_TCLASS )
            throw lua_exception( L, LUA_ERRRUN, "invalid type @ class reference" );

        init( L, lua_refclass( L, idx ) );
    }

    lua_class_reference( lua_State *L, ILuaClass *j )
    {
        init( L, j );
    }

    ~lua_class_reference( void )
    {
        m_class->DecrementMethodStack( m_lua );
    }

    ILuaClass* GetClass( void )
    {
        return m_class;
    }

private:
    lua_State*  m_lua;
    ILuaClass*  m_class;
};

class luaRefs : public std::list <lua_class_reference*>
{
public:
    ~luaRefs( void )
    {
        luaRefs::const_iterator iter = begin();

        for ( ; iter != end(); iter++ )
            delete *iter;
    }

    bool FindClass( ILuaClass *j, luaRefs::const_iterator& out_iter )
    {
        for ( luaRefs::const_iterator iter = begin(); iter != end(); iter++ )
        {
            if ( (*iter)->GetClass() == j )
            {
                out_iter = iter;
                return true;
            }
        }

        return false;
    }

    void RemoveClass( luaRefs::const_iterator& remIter )
    {
        delete *remIter;

        this->erase( remIter );
    }
};

class lua_yield_shield
{
public:
    lua_yield_shield( lua_State *L ) : m_api( lua_getstateapi( L ) )
    {
        m_prev = m_api.IsYieldDisabled();

        m_api.SetYieldDisabled( true );
    }

    ~lua_yield_shield()
    {
        m_api.SetYieldDisabled( m_prev );
    }

    bool        m_prev;
    ILuaState&  m_api;
};

#endif
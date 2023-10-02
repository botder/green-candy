/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaCommon.h
*  PURPOSE:     Master lua header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_COMMON_
#define _BASE_LUA_COMMON_

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <SString.h>

#define TO_ELEMENTID(x) ((ElementID) reinterpret_cast < unsigned long > (x) )

template <class type, int t, int idx>
static inline type* lua_readuserdata( lua_State *L )
{
    int top = lua_gettop( L );

    lua_rawgeti( L, t, idx );

    type *ud = (type*)lua_touserdata( L, top + 1 );

    lua_settop( L, top );
    return ud;
}

template <class type, int t, int idx>
inline type* lua_readuserdata_assert( lua_State *L )
{
    type *data = lua_readuserdata <type, t, idx> ( L );

    if ( !data )
        throw lua_exception( L, LUA_ERRRUN, "attempted to browse a destroyed resource environment" );

    return data;
}

static inline std::string lua_getstring( lua_State *L, int idx )
{
    size_t len;
    const char *msg = lua_tolstring( L, idx, &len );

    return std::string( msg, len );
}

static inline void lua_unpack( lua_State *L )
{
    int idx = lua_gettop( L );

    lua_pushnil( L );

    while ( lua_next( L, idx ) )
        lua_insert( L, -2 );

    lua_remove( L, idx );
}

static inline void lua_stack2table( lua_State *L, int tidx, int n )
{
    while ( n )
        lua_rawseti( L, tidx, n-- );
}

// Inline class for saving basic types
class LuaTypeExport abstract
{
public:
    LuaTypeExport( int type )
    {
        m_type = type;
    }

    virtual ~LuaTypeExport()    {}

    int GetType() const         { return m_type; }

    virtual lua_Number GetNumber() const = 0;
    virtual bool GetBoolean() const = 0;
    virtual void GetString( std::string& buf ) const = 0;

    virtual void Push( lua_State *L ) const = 0;

private:
    int m_type;
};

class LuaTypeBoolean : public LuaTypeExport
{
public:
    LuaTypeBoolean( bool boolean ) : LuaTypeExport( LUA_TBOOLEAN )
    {
        m_boolean = boolean;
    }

    lua_Number GetNumber() const
    {
        return m_boolean ? 1.0 : 0.0;
    }

    bool GetBoolean() const
    {
        return m_boolean;
    }

    void GetString( std::string& buf ) const
    {
        buf = m_boolean ? "true" : "false";
    }

    void Push( lua_State *L ) const
    {
        lua_pushboolean( L, m_boolean );
    }

private:
    bool m_boolean;
};

class LuaTypeNumber : public LuaTypeExport
{
public:
    LuaTypeNumber( lua_Number num ) : LuaTypeExport( LUA_TNUMBER )
    {
        m_num = num;
    }

    lua_Number GetNumber() const        { return m_num; }

    bool GetBoolean() const
    {
        return m_num != 0;
    }

    void GetString( std::string& buf ) const
    {
        std::stringstream stream;

        stream.precision( 16 );
        stream << m_num;

        buf = stream.str();
    }

    void Push( lua_State *L ) const
    {
        lua_pushnumber( L, m_num );
    }

private:
    lua_Number m_num;
};

class LuaTypeString : public LuaTypeExport
{
public:
    LuaTypeString( const std::string& string ) : LuaTypeExport( LUA_TSTRING )
    {
        m_string = string;
    }

    lua_Number GetNumber() const
    {
        std::stringstream stream( m_string );

        stream.precision( 16 );

        double num;
        stream >> num;

        return num;
    }

    bool GetBoolean() const
    {
        if ( m_string == "true" )
            return true;

        return GetNumber() != 0;
    }

    void GetString( std::string& buf ) const
    {
        buf = m_string;
    }

    void Push( lua_State *L ) const
    {
        lua_pushlstring( L, m_string.c_str(), m_string.size() );
    }

private:
    std::string m_string;
};

static inline bool Lua_ReadExportType( lua_State *L, int idx, LuaTypeExport*& exp )
{
    switch( lua_type( L, idx ) )
    {
    case LUA_TBOOLEAN:
        exp = new LuaTypeBoolean( lua_toboolean( L, idx ) != 0 );
        return true;
    case LUA_TNUMBER:
        exp = new LuaTypeNumber( lua_tonumber( L, idx ) );
        return true;
    case LUA_TSTRING:
        exp = new LuaTypeString( lua_getstring( L, idx ) );
        return true;
    }

    return false;
}

// Global functions for convenience
void lua_createEnvProxy( lua_State *L );
unsigned int TumblerHash( const char *str, size_t len );

//#define LUA_EXCEPTION_SAFETY

#define LUA_METHOD(x)       { #x, x }
#define LUA_CHECK(x)        if ( !(x) ) \
                            { \
                                lua_pushboolean( L, false ); \
                                return 1; \
                            }
#define LUA_CHECK_S(x, msg) if ( !(x) ) \
                            { \
                                g_pClientGame->GetScriptDebugging()->LogCustom( (msg) ); \
                                lua_pushboolean( L, false ); \
                                return 1; \
                            }
#define LUA_ARGS_BEGIN      CScriptArgReader argStream( L )

#ifndef LUA_EXCEPTION_SAFETY
#define LUA_ARGS_END        if ( argStream.HasErrors() ) \
                            { \
                                std::string err_msg( "Bad argument @ '" ); \
                                err_msg += __FUNCTION__; \
                                err_msg += "' ["; \
                                err_msg += argStream.GetErrorMessage(); \
                                err_msg += "]"; \
                                throw lua_exception( L, LUA_ERRRUN, err_msg.c_str() ); \
                            }
#define LUA_ASSERT(x, msg)  if ( !(x) ) \
                                throw lua_exception( L, LUA_ERRRUN, (msg) )
#define LUA_SUCCESS         return 0;
#else
#define LUA_ARGS_END        if ( argStream.HasErrors() ) \
                            { \
                                std::string err_msg( "Bad argument @ '" ); \
                                err_msg += __FUNCTION__; \
                                err_msg += "' ["; \
                                err_msg += argStream.GetErrorMessage(); \
                                err_msg += "]"; \
                                g_pClientGame->GetScriptDebugging()->LogCustom( err_msg.c_str() ); \
                                lua_pushboolean( L, false ); \
                                return 1; \
                            }
#define LUA_ASSERT(x, msg)  if ( !(x) ) \
                            { \
                                g_pClientGame->GetScriptDebugging()->LogCustom( (msg) ); \
                                lua_pushboolean( L, false ); \
                                return 1; \
                            }
#define LUA_SUCCESS         lua_pushboolean( L, true ); \
                            return 1;
#endif

#endif //_BASE_LUA_COMMON

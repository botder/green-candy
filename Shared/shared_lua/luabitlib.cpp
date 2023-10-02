/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luabitlib.cpp
*  PURPOSE:     Lua bitwise operations 32bit integer library
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

typedef size_t bitwiseNumber;

static int bitwise_not( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );

    lua_pushnumber( L, (lua_Number)( ~( (bitwiseNumber)lua_tonumber( L, 1 ) ) ) );
    return 1;
}

template <class opFunc>
int bitwise_operation( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );

    bitwiseNumber num = (bitwiseNumber)lua_tonumber( L, 1 );
    int top = lua_gettop( L );

    for ( int n = 2; n <= top; n++ )
    {
        luaL_checktype( L, 2, LUA_TNUMBER );
        
        num = opFunc::perform( num, (bitwiseNumber)lua_tonumber( L, n ) );
    }

    lua_pushnumber( L, (lua_Number)num );
    return 1;
}

#define BITWISE_OP( name, exec ) \
    struct lua_bit_##name \
    { \
        static bitwiseNumber perform( bitwiseNumber op1, bitwiseNumber op2 ) \
        { \
            return exec; \
        } \
    };

BITWISE_OP( and, op1 & op2 );
BITWISE_OP( or, op1 | op2 );
BITWISE_OP( xor, op1 ^ op2 );

static int bitwise_lshift( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );
    luaL_checktype( L, 2, LUA_TNUMBER );

    lua_pushnumber( L, (lua_Number)( (bitwiseNumber)lua_tonumber( L, 1 ) << (bitwiseNumber)lua_tonumber( L, 2 ) ) );
    return 1;
}

static int bitwise_rshift( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TNUMBER );
    luaL_checktype( L, 2, LUA_TNUMBER );

    lua_pushnumber( L, (lua_Number)( (bitwiseNumber)lua_tonumber( L, 1 ) >> (bitwiseNumber)lua_tonumber( L, 2 ) ) );
    return 1;
}

static const luaL_Reg bit32_interface[] =
{
    { "bnot", bitwise_not },
    { "band", bitwise_operation <lua_bit_and> },
    { "bor", bitwise_operation <lua_bit_or> },
    { "bxor", bitwise_operation <lua_bit_xor> },
    { "lshift", bitwise_lshift },
    { "rshift", bitwise_rshift },
    { NULL, NULL }
};

void luabitwise_open( lua_State *L )
{
    luaL_openlib( L, "bit32", bit32_interface, 0 );
    lua_pop( L, 1 );
}
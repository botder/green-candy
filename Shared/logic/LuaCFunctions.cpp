/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaCFunctions.cpp
*  PURPOSE:     Shared Lua functions
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#include <unordered_map>

typedef std::unordered_map <lua_CFunction, LuaCFunction*> funcList;
static funcList luaFunctions;
typedef std::unordered_map <std::string, LuaCFunction*> protoNameList;
static protoNameList luaProtosByName;
static bool mapsInitialized = false;

LuaCFunction::LuaCFunction( const char *name, lua_CFunction proto, bool restrict )
{
    if ( name )
        m_name = name;

    m_proto = proto;
    m_restrict = restrict;
}

namespace LuaCFunctions
{
    void Init()
    {
    }

    void Destroy()
    {
        RemoveAllFunctions();
    }

    void InitializeHashMaps()
    {
        if ( !mapsInitialized )
        {
            mapsInitialized = true;
        }
    }

    LuaCFunction* AddFunction( const char *name, lua_CFunction cProto, bool restrict )
    {
        // Already have a function by this name?
        LuaCFunction *proto = GetFunction( name );

        if ( proto )
            return proto;

        // Already have a function by this address?
        proto = GetFunction( cProto );

        if ( !proto )
        {
            luaFunctions[ cProto ] = proto;
        }
        luaProtosByName[ name ] = proto;
        return proto;
    }

    LuaCFunction* GetFunction( lua_CFunction proto )
    {
        funcList::iterator it = luaFunctions.find( proto );

        if ( it == luaFunctions.end() )
            return NULL;

        return it->second;
    }

    LuaCFunction* GetFunction( const char *name )
    {
        protoNameList::iterator it = luaProtosByName.find( name );

        if ( it == luaProtosByName.end() )
            return NULL;

        return it->second;
    }

    void RegisterFunctionsWithVM( lua_State *lua )
    {
        // Register all our functions to a lua VM
        protoNameList::iterator it;

        for ( it = luaProtosByName.begin(); it != luaProtosByName.end(); it++ )
            lua_register( lua, it->first.c_str(), it->second->GetAddress() );
    }

    void RemoveAllFunctions()
    {
        // Delete all functions
        funcList::iterator it;

        for ( it = luaFunctions.begin(); it != luaFunctions.end(); it++ )
            delete it->second;

        luaFunctions.clear();
        luaProtosByName.clear();
    }
}

const static unsigned int mod_lookup[] =
{
    0x85465933, 0x74183274, 0x19374935, 0x75639274, 0x82471783,
    0x78580324, 0x24754925, 0x75327563, 0x47762487, 0x65378454,
    0x13162964, 0x98145783, 0x76478235, 0x78364872, 0x87637562,
    0x47234927, 0x87648723, 0x57435293, 0x93462398, 0x43748932,
    0x74637424, 0x87428744, 0x47247826, 0x27462374, 0x78326428,
    0x37841841, 0x42749237, 0x23492749, 0x41984414, 0x62389468,
    0x28946144, 0x46298472, 0x74672364, 0x27452139, 0x73629793,
    0x23674629, 0x76597823, 0x61928461, 0x76491624, 0x76732548,
    0x72641649, 0x85378959, 0x23568234, 0x15634789, 0x53462592,
    0x78645783, 0x89259684, 0x35873424, 0x73578653, 0x64274674,
    0x35893275, 0x37583089, 0x89725398, 0x98237534, 0x97853298,
    0x84732980, 0x23572843, 0x98327424, 0x98327424, 0x55852803,
    0x28394752, 0x56234434, 0x97213948, 0x91479344, 0x89749198,
    0x12701273, 0x09747124, 0x74128471, 0x18734879, 0x80748187,
    0x98714944, 0x89759327, 0x87695487, 0x76491413, 0x28740917,
    0x80941478, 0x97632947, 0x98701355, 0x97532944, 0x97834974,
    0x89743918, 0x45378543, 0x50475345, 0x74534385, 0x87401247,
    0x87529075, 0x87534794, 0x79278409, 0x42374084, 0x09782304,
    0x47297402, 0x98274238, 0x87098704, 0x43298478, 0x57529434,
    0x32947298, 0x98749344, 0x76349524, 0x21436149, 0x43267491,
    0x76498442, 0x97649144, 0x19284714, 0x97649143, 0x87414780,
    0x78494720, 0x91649344, 0x76941443, 0x93497442, 0x99483223
};

unsigned int TumblerHash( const char *str, size_t len )
{
    unsigned int hash = 0xABBADEED;
    unsigned char lastChar = 0xCC;
    const size_t tabLen = NUMELMS(mod_lookup);
    size_t tab_off = 0;
    size_t tab_len = tabLen;

    while ( len != 0 )
    {
        len--;

        unsigned char curChar = (unsigned char)str[len];

        hash ^= mod_lookup[(curChar + lastChar + tab_off) % tab_len];

        tab_off = (tab_off + 1) % tabLen;
        tab_len = tabLen - std::min( tabLen - tab_off - 1, curChar & (~len) );

        lastChar = curChar;
    }

    return hash;
}
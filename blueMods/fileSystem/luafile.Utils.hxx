/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.cpp
*  PURPOSE:     Lua filesystem utils
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _LUAFILE_UTILS_
#define _LUAFILE_UTILS_

static inline void luafile_pushStats( lua_State *L, fsOffsetNumber_t fileSize, const filesysStats& stats )
{
    lua_newtable( L );

    int top = lua_gettop( L );

    lua_pushnumber( L, (lua_Number)stats.atime );
    lua_setfield( L, top, "accessTime" );
    lua_pushnumber( L, (lua_Number)stats.ctime );;
    lua_setfield( L, top, "creationTime" );
    lua_pushnumber( L, (lua_Number)stats.mtime );
    lua_setfield( L, top, "modTime" );
    lua_pushnumber( L, (lua_Number)fileSize );
    lua_setfield( L, top, "size" );
}

#define LUAFILE_GUARDFSCALL_BEGIN \
    try {
#define LUAFILE_GUARDFSCALL_END \
    } \
    catch( FileSystem::TerminatedObjectException& ) \
    { \
        lua_pushboolean( L, false ); \
        return 1; \
    } \
    catch( FileSystem::filesystem_exception& except ) \
    { \
        lua_pushboolean( L, false ); \
        FileSystem::eGenExceptCode code = except.get_code(); \
        if ( code == FileSystem::eGenExceptCode::RESOURCE_UNAVAILABLE ) \
        { \
            lua_pushstring( L, "resource unavailable" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::MEMORY_INSUFFICIENT ) \
        { \
            lua_pushstring( L, "memory insufficient" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::INVALID_SYSPARAM ) \
        { \
            lua_pushstring( L, "invalid sysparam" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::INVALID_PARAM ) \
        { \
            lua_pushstring( L, "invalid param" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::ILLEGAL_PATHCHAR ) \
        { \
            lua_pushstring( L, "illegal pathchar" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::UNSUPPORTED_OPERATION ) \
        { \
            lua_pushstring( L, "unsupported operation" ); \
        } \
        else if ( code == FileSystem::eGenExceptCode::INVALID_OBJECT_STATE ) \
        { \
            lua_pushstring( L, "invalid object state" ); \
        } \
        else \
        { \
            lua_pushstring( L, "unknown fs error" ); \
        } \
        return 2; \
    }

#endif //_LUAFILE_UITLS_

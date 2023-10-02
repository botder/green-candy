/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafilesystem.cpp
*  PURPOSE:     Lua implementation of the FileSystem CFileTranslator class
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "luafile.Utils.hxx"

extern CFileSystemInterface *pubFileSystem;

static int filesystem_open( lua_State *L )
{
    // TODO: maybe allow access to the full feature-set of Eir FileSystem by enabling
    // the non ANSI-C fopen API with struct as open-descriptor.

    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );
    luaL_checktype( L, 3, LUA_TSTRING );

    lua_settop( L, 3 );

    CFile *file;

    LUAFILE_GUARDFSCALL_BEGIN
        file = trans->Open( lua_tostring( L, 2 ), lua_tostring( L, 3 ) );
    LUAFILE_GUARDFSCALL_END

    if ( !file )
    {
        lua_pushboolean( L, false );

        eFileOpenFailure open_fail = pubFileSystem->GetLastTranslatorOpenFailure();

        if ( open_fail == eFileOpenFailure::NONE )
        {
            lua_pushlstring( L, "none", 4 );
        }
        else if ( open_fail == eFileOpenFailure::UNKNOWN_ERROR )
        {
            lua_pushlstring( L, "unknown error", 13 );
        }
        else if ( open_fail == eFileOpenFailure::PATH_OUT_OF_SCOPE )
        {
            lua_pushlstring( L, "path out of scope", 17 );
        }
        else if ( open_fail == eFileOpenFailure::INVALID_PARAMETERS )
        {
            lua_pushlstring( L, "invalid parameters", 18 );
        }
        else if ( open_fail == eFileOpenFailure::RESOURCES_EXHAUSTED )
        {
            lua_pushlstring( L, "resources exhausted", 19 );
        }
        else if ( open_fail == eFileOpenFailure::ACCESS_DENIED )
        {
            lua_pushlstring( L, "access denied", 13 );
        }
        else if ( open_fail == eFileOpenFailure::NOT_FOUND )
        {
            lua_pushlstring( L, "not found", 9 );
        }
        else if ( open_fail == eFileOpenFailure::ALREADY_EXISTS )
        {
            lua_pushlstring( L, "already exists", 14 );
        }
        else
        {
            lua_pushlstring( L, "invalid error mapping (?)", 25 );
        }

        return 2;
    }

    fslib_config *cfg = fsLuaGetConfig( L );

    if ( cfg->doBufferAllRaw )
    {
        try
        {
            file = pubFileSystem->WrapStreamBuffered( file, true );
        }
        catch( ... )
        {}
    }

    fsRegisterFile( file );
    luaclass_pushshim( L, file );
    return 1;
}

static int filesystem_exists( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    bool exists;

    LUAFILE_GUARDFSCALL_BEGIN
        exists = trans->Exists( lua_tostring( L, 2 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, exists );
    return 1;
}

static int filesystem_createDir( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    bool success;
    
    LUAFILE_GUARDFSCALL_BEGIN
        success = trans->CreateDir( lua_tostring( L, 2 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int filesystem_chdir( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = trans->ChangeDirectory( lua_tostring( L, 2 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int filesystem_delete( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = trans->Delete( lua_tostring( L, 2 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int filesystem_copy( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );
    luaL_checktype( L, 3, LUA_TSTRING );

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = trans->Copy( lua_tostring( L, 2 ), lua_tostring( L, 3 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int filesystem_rename( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );
    luaL_checktype( L, 3, LUA_TSTRING );

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = trans->Rename( lua_tostring( L, 2 ), lua_tostring( L, 3 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int filesystem_size( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    size_t fileSize;

    LUAFILE_GUARDFSCALL_BEGIN
        fileSize = trans->Size( lua_tostring( L, 2 ) );
    LUAFILE_GUARDFSCALL_END

    lua_pushnumber( L, fileSize );
    return 1;
}

static int filesystem_stat( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    filesysStats stats;

    LUAFILE_GUARDFSCALL_BEGIN
        if ( !trans->QueryStats( lua_tostring( L, 2 ), stats ) )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        size_t fileSize = trans->Size( lua_tostring( L, 2 ) );

        luafile_pushStats( L, fileSize, stats );
        return 1;
    LUAFILE_GUARDFSCALL_END
}

static int filesystem_relPath( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    const char *src = lua_tostring( L, 2 );
    filePath path;

    if ( !src )
        src = "";

    if ( !trans->GetRelativePath( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    return 1;
}

static int filesystem_relPathRoot( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    const char *src = lua_tostring( L, 2 );
    filePath path;

    if ( !src )
        src = "";

    if ( !trans->GetRelativePathFromRoot( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    return 1;
}

static int filesystem_absPath( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    const char *src = lua_tostring( L, 2 );
    filePath path;

    if ( !src )
        src = "";

    if ( !trans->GetFullPath( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    return 1;
}

static int filesystem_absPathRoot( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    const char *src = lua_tostring( L, 2 );
    filePath path;

    if ( !src )
        src = "";

    if ( !trans->GetFullPathFromRoot( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    return 1;
}

static void lua_findScanCallback( const filePath& path, void *ud )
{
    auto ansiFilePath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_pushlstring( (lua_State*)ud, ansiFilePath.GetConstString(), ansiFilePath.GetLength() );
    luaL_ref( (lua_State*)ud, -2 );
}

static int filesystem_scanDir( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    const char *path = lua_tostring( L, 2 );
    const char *wildcard;
    bool recursive;

    int top = lua_gettop( L );

    if ( top > 1 )
    {
        wildcard = lua_tostring( L, 3 );

        if ( top > 2 )
            recursive = ( lua_toboolean( L, 4 ) != 0 );
        else
            recursive = false;
    }
    else
    {
        wildcard = "*";
        recursive = false;
    }

    lua_newtable( L );

    LUAFILE_GUARDFSCALL_BEGIN
        trans->ScanDirectory( path, wildcard, recursive, lua_findScanCallback, lua_findScanCallback, L );
    LUAFILE_GUARDFSCALL_END

    return 1;
}

static int filesystem_getFiles( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    const char *path = lua_tostring( L, 2 );
    const char *wildcard;
    bool recursive;

    int top = lua_gettop( L );

    if ( top > 1 )
    {
        wildcard = lua_tostring( L, 3 );

        if ( top > 2 )
            recursive = ( lua_toboolean( L, 4 ) != 0 );
        else
            recursive = false;
    }
    else
    {
        wildcard = "*";
        recursive = false;
    }

    lua_newtable( L );

    LUAFILE_GUARDFSCALL_BEGIN
        trans->ScanDirectory( path, wildcard, recursive, nullptr, lua_findScanCallback, L );
    LUAFILE_GUARDFSCALL_END

    return 1;
}

static int filesystem_getDirs( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    const char *path = lua_tostring( L, 2 );
    bool recursive;

    if ( lua_gettop( L ) > 1 )
        recursive = ( lua_toboolean( L, 3 ) != 0 );
    else
        recursive = false;

    lua_newtable( L );

    LUAFILE_GUARDFSCALL_BEGIN
        trans->ScanDirectory( path, "*", recursive, lua_findScanCallback, 0, L );
    LUAFILE_GUARDFSCALL_END

    return 1;
}

static void filesystem_exfilecb( const filePath& path, void *ud )
{
    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_State *L = (lua_State*)ud;
    lua_pushvalue( L, 4 );
    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    lua_call( L, 1, 0 );
}

static void filesystem_exdircb( const filePath& path, void *ud )
{
    auto ansiPath = path.convert_ansi <FileSysCommonAllocator> ();

    lua_State *L = (lua_State*)ud;
    lua_pushvalue( L, 3 );
    lua_pushlstring( L, ansiPath.GetConstString(), ansiPath.GetLength() );
    lua_call( L, 1, 0 );
}

static int filesystem_scanDirEx( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );
    luaL_checktype( L, 3, LUA_TSTRING );

    LUAFILE_GUARDFSCALL_BEGIN
        trans->ScanDirectory(
            lua_tostring( L, 2 ),
            lua_tostring( L, 3 ),
            ( lua_toboolean( L, 6 ) != 0 ),
            lua_type( L, 4 ) == LUA_TFUNCTION ? filesystem_exdircb : nullptr,
            lua_type( L, 5 ) == LUA_TFUNCTION ? filesystem_exfilecb : nullptr,
            L
        );
    LUAFILE_GUARDFSCALL_END

    return 0;
}

int filesystem_setOutbreakEnabled( lua_State *L )
{
    // This is only secure in the case that we are not on the MTA client.
    // Since Lua modules do not exist on the client-side, we are fine.

    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TBOOLEAN );
    
    bool enabled = lua_toboolean( L, 2 );

    trans->SetOutbreakEnabled( enabled );
    return 0;
}

int filesystem_getOutbreakEnabled( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    bool enabled = trans->IsOutbreakEnabled();

    lua_pushboolean( L, enabled );
    return 1;
}

int filesystem_setPathProcessMode( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );

    const char *ppm = lua_tostring( L, 2 );

    filesysPathProcessMode fs_ppm;

    if ( StringEqualToZero( ppm, "distinguished", false ) )
    {
        fs_ppm = filesysPathProcessMode::DISTINGUISHED;
    }
    else if ( StringEqualToZero( ppm, "ambivalent_file", false ) )
    {
        fs_ppm = filesysPathProcessMode::AMBIVALENT_FILE;
    }
    else
    {
        lua_pushboolean( L, false );
        return 1;
    }

    trans->SetPathProcessMode( fs_ppm );

    lua_pushboolean( L, true );
    return 1;
}

int filesystem_getPathProcessMode( lua_State *L )
{
    CFileTranslator *trans = fsLuaGetTranslator( L, 1 );

    filesysPathProcessMode fs_ppm = trans->GetPathProcessMode();

    if ( fs_ppm == filesysPathProcessMode::DISTINGUISHED )
    {
        lua_pushstring( L, "distinguished" );
    }
    else if ( fs_ppm == filesysPathProcessMode::AMBIVALENT_FILE )
    {
        lua_pushstring( L, "ambivalent_file" );
    }
    else
    {
        lua_pushboolean( L, false );
    }
    return 1;
}

static const luaL_Reg ftranslator_methods[] =
{
    { "open", filesystem_open },
    { "exists", filesystem_exists },
    { "createDir", filesystem_createDir },
    { "chdir", filesystem_chdir },
    { "delete", filesystem_delete },
    { "copy", filesystem_copy },
    { "rename", filesystem_rename },
    { "size", filesystem_size },
    { "stat", filesystem_stat },
    { "relPath", filesystem_relPath },
    { "relPathRoot", filesystem_relPathRoot },
    { "absPath", filesystem_absPath },
    { "absPathRoot", filesystem_absPathRoot },
    { "scanDir", filesystem_scanDir },
    { "scanDirEx", filesystem_scanDirEx },
    { "getDirs", filesystem_getDirs },
    { "getFiles", filesystem_getFiles },
    { "setOutbreakEnabled", filesystem_setOutbreakEnabled },
    { "getOutbreakEnabled", filesystem_getOutbreakEnabled },
    { "setPathProcessMode", filesystem_setPathProcessMode },
    { "getPathProcessMode", filesystem_getPathProcessMode },
    { nullptr, nullptr }
};

int luafsys_createTranslator( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );

    CFileTranslator *root;

    LUAFILE_GUARDFSCALL_BEGIN
        root = pubFileSystem->CreateTranslator( lua_tostring( L, 1 ) );
    LUAFILE_GUARDFSCALL_END

    if ( !root )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    fsRegisterTranslator( root );
    luaclass_pushshim( L, root );
    return 1;
}

void luaftrans_makemeta( lua_State *L, int ridx )
{
    lua_newtable( L );
    lua_pushvalue( L, ridx - 1 );
    luaL_openlib( L, nullptr, ftranslator_methods, 1 );
}
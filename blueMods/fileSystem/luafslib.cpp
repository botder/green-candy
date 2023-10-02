/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafslib.cpp
*  PURPOSE:     Lua Eir FileSystem module main implementation file
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#include "luafile.Utils.hxx"

#include <cstring>

extern CFileSystemInterface *pubFileSystem;

#ifndef FU_CLASS
static int archive_save( lua_State *L )
{
    ILuaClass *j = lua_refclass( L, lua_upvalueindex( 1 ) );

    CFile *file;

    LUA_CHECK( j->GetTransmit( LUACLASS_FILE, (void*&)file ) && file->IsWriteable() );

    ((CArchiveTranslator*)lua_touserdata( L, lua_upvalueindex( 2 ) ))->Save();
    lua_pushboolean( L, true );
    return 1;
}

static int archive_destroy( lua_State *L )
{
    // Decrement the file reference count
    ILuaClass *j = lua_refclass( L, lua_upvalueindex( 1 ) );
    j->DecrementMethodStack( L );

    return 0;
}

static const luaL_Reg archiveLib[] =
{
    { "save", archive_save },
    { "destroy", archive_destroy },
    { NULL, NULL }
};

static int archive_constructor( lua_State *L )
{
    lua_pushvalue( L, LUA_ENVIRONINDEX );
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    lua_pushvalue( L, lua_upvalueindex( 2 ) );
    luaL_openlib( L, NULL, archiveLib, 2 );
    return 0;
}

template <typename archiveHandler>
static AINLINE int _archiveTranslatorFunctor( lua_State *L, archiveHandler& cb )
{
    luaL_checktype( L, 1, LUA_TCLASS );

    ILuaClass *j = lua_refclass( L, 1 );

    // Grab the file interface
    CFile *file;

    if ( !j->GetTransmit( LUACLASS_FILE, (void*&)file ) )
        throw lua_exception( L, LUA_ERRRUN, "expected file at archive creation" );

    // Attempt to read an archive.
    CArchiveTranslator *root = cb.CreateTranslator( file );

    // Check that we actually suceeded in creating the archive
    LUA_CHECK( root );

    // Keep the file alive during archive business
    j->IncrementMethodStack( L );

    luafsys_pushroot( L, root );

    // Extend the fileTranslator class
    lua_pushvalue( L, 1 );
    lua_pushlightuserdata( L, root );
    lua_pushcclosure( L, archive_constructor, 2 );
    luaJ_extend( L, 2, 0 );
    return 1;
}

struct archiveTranslatorOpener
{
    AINLINE CArchiveTranslator* CreateTranslator( CFile *file )
    {
        return pubFileSystem->OpenArchive( *file );
    }
};

int luafsys_createArchiveTranslator( lua_State *L )
{
    archiveTranslatorOpener opener;

    return _archiveTranslatorFunctor( L, opener );
}

struct zipArchiveTranslatorCreator
{
    AINLINE CArchiveTranslator* CreateTranslator( CFile *file )
    {
        return pubFileSystem->CreateZIPArchive( *file );
    }
};

int luafsys_createZIPArchive( lua_State *L )
{
    zipArchiveTranslatorCreator creator;

    return _archiveTranslatorFunctor( L, creator );
}
#endif //FU_CLASS

static int luafsys_copyFile( lua_State *L )
{
    CFileTranslator *srcTranslator = fsLuaGetTranslator( L, 1 );
    const char *srcPath = lua_tostring( L, 2 );
    CFileTranslator *dstTranslator = fsLuaGetTranslator( L, 3 );
    const char *dstPath = lua_tostring( L, 4 );

    LUA_CHECK( srcTranslator && srcPath && dstTranslator && dstPath );

    bool success;

    LUAFILE_GUARDFSCALL_BEGIN
        success = FileSystem::FileCopy( srcTranslator, srcPath, dstTranslator, dstPath );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, success );
    return 1;
}

static int luafsys_copyStream( lua_State *L )
{
    CFile *srcStream = fsLuaGetFile( L, 1 );
    CFile *dstStream = fsLuaGetFile( L, 2 );

    LUA_CHECK( srcStream && dstStream );

    LUAFILE_GUARDFSCALL_BEGIN
        FileSystem::StreamCopy( *srcStream, *dstStream );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, true );
    return 1;
}

static int luafsys_copyStreamCount( lua_State *L )
{
    CFile *srcStream = fsLuaGetFile( L, 1 );
    CFile *dstStream = fsLuaGetFile( L, 2 );
    int count = (int)lua_tonumber( L, 3 );

    LUA_CHECK( srcStream && dstStream );

    LUA_CHECK( count > 0 );

    LUAFILE_GUARDFSCALL_BEGIN
        FileSystem::StreamCopyCount( *srcStream, *dstStream, count );
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, true );
    return 1;
}

static int luafsys_pathToFilename( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    luaL_checktype( L, 2, LUA_TBOOLEAN );

    const char *path = lua_tostring( L, 1 );
    bool includeExtension = ( lua_toboolean( L, 2 ) != 0 );

    filePath directoryOut;

    filePath fileName = FileSystem::GetFileNameItem( path, includeExtension, &directoryOut );

    int iRet = 1;

    lua_pushlstring( L, fileName.c_str(), fileName.size() );

    if ( directoryOut.size() != 0 )
    {
        lua_pushlstring( L, directoryOut.c_str(), directoryOut.size() );

        iRet++;
    }

    return iRet;
}

static int luafsys_streamCompare( lua_State *L )
{
    CFile *srcFile = fsLuaGetFile( L, 1 );
    CFile *dstFile = fsLuaGetFile( L, 2 );

    LUA_CHECK( srcFile && dstFile );

    char sourceBuf[2048];
    char targetBuf[2048];

    bool isEqual = true;

    LUAFILE_GUARDFSCALL_BEGIN
        while ( !srcFile->IsEOF() )
        {
            size_t sourceReadCount = srcFile->Read( sourceBuf, sizeof( sourceBuf ) );
            size_t destReadCount = dstFile->Read( targetBuf, sizeof( targetBuf ) );

            if ( sourceReadCount != destReadCount )
            {
                isEqual = false;
                break;
            }

            if ( sourceReadCount != 0 && memcmp( sourceBuf, targetBuf, sourceReadCount ) != 0 )
            {
                isEqual = false;
                break;
            }
        }
    LUAFILE_GUARDFSCALL_END

    lua_pushboolean( L, isEqual );
    return 1;
}

int luafsys_getRoot( lua_State *L )
{
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    return 1;
}

int luafsys_createTranslator( lua_State *L );

int luafsys_createRAMDisk( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TBOOLEAN );

    bool case_sensitive = lua_toboolean( L, 1 );

    CFileTranslator *ramDisk = pubFileSystem->CreateRamdisk( case_sensitive );

    if ( ramDisk == nullptr )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    fsRegisterTranslator( ramDisk );
    luaclass_pushshim( L, ramDisk );
    return 1;
}

int luafsys_createMemoryFile( lua_State *L )
{
    CFile *memFile = pubFileSystem->CreateMemoryFile();

    if ( memFile == nullptr )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    fsRegisterFile( memFile );
    luaclass_pushshim( L, memFile );
    return 1;
}

int luafsys_createFileIterative( lua_State *L )
{
    CFileTranslator *target = fsLuaGetTranslator( L, 1 );
    luaL_checktype( L, 2, LUA_TSTRING );
    luaL_checktype( L, 3, LUA_TSTRING );
    luaL_checkinteger( L, 4 );

    const char *prefix = lua_tostring( L, 2 );
    const char *suffix = lua_tostring( L, 3 );
    int i_max_dupl = lua_tointeger( L, 4 );

    if ( i_max_dupl < 0 )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    unsigned int max_dupl = (unsigned int)i_max_dupl;

    CFile *result;

    LUAFILE_GUARDFSCALL_BEGIN
        result = pubFileSystem->CreateFileIterative( target, prefix, suffix, max_dupl );
    LUAFILE_GUARDFSCALL_END

    if ( result == nullptr )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    fsRegisterFile( result );
    luaclass_pushshim( L, result );
    return 1;
}

int luafsys_topointer( lua_State *L )
{
    void *ptr = luaclass_getpointer( L, 1 );

    lua_pushlightuserdata( L, ptr );
    return 1;

    // TODO: maybe allow conversion from pointer back to object?
}

int luafsys_type( lua_State *L )
{
    void *obj = luaclass_getpointer( L, 1 );

    if ( fsIsFile( obj ) )
    {
        lua_pushlstring( L, "file", 4 );
        return 1;
    }
    if ( fsIsTranslator( obj ) )
    {
        lua_pushlstring( L, "file-translator", 15 );
        return 1;
    }

    lua_pushboolean( L, false );
    return 1;
}

int luafsys_setDoBufferAllRaw( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TBOOLEAN );

    bool enable = lua_toboolean( L, 1 );

    fslib_config *cfg = fsLuaGetConfig( L );

    cfg->doBufferAllRaw = enable;
    return 0;
}

int luafsys_getDoBufferAllRaw( lua_State *L )
{
    fslib_config *cfg = fsLuaGetConfig( L );

    lua_pushboolean( L, cfg->doBufferAllRaw );
    return 1;
}

static const luaL_Reg fsysLib[] =
{
    { "createTranslator", luafsys_createTranslator },
    { "createRAMDisk", luafsys_createRAMDisk },
    { "createMemoryFile", luafsys_createMemoryFile },
    { "createFileIterative", luafsys_createFileIterative },
#ifndef FU_CLASS
    { "createArchiveTranslator", luafsys_createArchiveTranslator },
    { "createZIPArchive", luafsys_createZIPArchive },
#endif //FU_CLASS
    { "copyFile", luafsys_copyFile },
    { "copyStream", luafsys_copyStream },
    { "copyStreamCount", luafsys_copyStreamCount },
    { "pathToFilename", luafsys_pathToFilename },
    { "streamCompare", luafsys_streamCompare },
    { "topointer", luafsys_topointer },
    { "type", luafsys_type },
    { "setDoBufferAllRaw", luafsys_setDoBufferAllRaw },
    { "getDoBufferAllRaw", luafsys_getDoBufferAllRaw },
    { nullptr, nullptr }
};

int luafsys_init( lua_State *L )
{
    // Specify the root fileTranslator
    CFileTranslator *rootTranslator = pubFileSystem->CreateTranslator( "" ); // use the current directory.

    // We could fail to obtain the handle to the translator if the directory is handle-locked.
    // In that case, return false.
    LUA_CHECK( rootTranslator != nullptr );

    // Return the Lua representation of the root translator.
    fsRegisterTranslator( rootTranslator );
    luaclass_pushshim( L, rootTranslator );
    return 1;
}

void luaclass_makemeta( lua_State *L, int ridx );
void luafile_makemeta( lua_State *L, int ridx );
void luaftrans_makemeta( lua_State *L, int ridx );
void luaimgarch_makemeta( lua_State *L, int ridx );
void luaziparch_makemeta( lua_State *L, int ridx );

void luaclassmt_make( lua_State *L, int ridx );

static int luafilesystem_ongc( lua_State *L )
{
    void **ptr_cfg = (void**)lua_touserdata( L, lua_upvalueindex( 1 ) );

    fslib_config *cfg = (fslib_config*)*ptr_cfg;

    delete cfg;

    *ptr_cfg = nullptr;

    return 0;
}

fslib_config* fsLuaGetConfig( lua_State *L )
{
    void **ptr_cfg = (void**)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return (fslib_config*)*ptr_cfg;
}

extern bool _global_doBufferAllRaw;

void luafilesystem_open( lua_State *L )
{
    // We want to store the entire Eir FileSystem Lua implementation state
    // as GC-linked userdata upvalue to all C-closures we create.
    // This strategy is safely possible because even the "debug.getupvalue" function
    // is disallowed from fetching C-closure upvalues.

    // The library we will return.
    lua_newtable( L );

    void **ptr_cfg = (void**)lua_newuserdata( L, sizeof(void*) );
    {
        fslib_config *cfg = new fslib_config;

        cfg->doBufferAllRaw = _global_doBufferAllRaw;

        *ptr_cfg = cfg;
    }

    lua_newtable( L );

    // Store as meta-table.
    lua_pushvalue( L, -1 );
    lua_setmetatable( L, -3 );

    // Create a table that should store all meta-tables for Eir FileSystem classes.
    lua_newtable( L );

    // Store it as "metas" in the maintenance table.
    lua_pushlstring( L, "metas", 5 );
    lua_pushvalue( L, -2 );
    lua_rawset( L, -4 );

    // Initialize the class metatable.
    lua_pushlstring( L, "class", 5 );
    luaclass_makemeta( L, -4 );
    lua_rawset( L, -3 );

    // Initialize the file metatable.
    lua_pushlstring( L, "file", 4 );
    luafile_makemeta( L, -4 );
    lua_rawset( L, -3 );

    // Initialize the file-translator metatable.
    lua_pushlstring( L, "ftrans", 6 );
    luaftrans_makemeta( L, -4 );
    lua_rawset( L, -3 );

    // Initialize the img-archive metatable.
    lua_pushlstring( L, "imgarch", 7 );
    luaimgarch_makemeta( L, -4 );
    lua_rawset( L, -3 );

    // Initialize the zip-archive metatable.
    lua_pushlstring( L, "ziparch", 7 );
    luaziparch_makemeta( L, -4 );
    lua_rawset( L, -3 );

    lua_pop( L, 1 );

    // Now all meta-method tables have been initialized.
    // Next get the maintenance set-up.
    lua_pushlstring( L, "__gc", 4 );
    lua_pushvalue( L, -3 );
    lua_pushcclosure( L, luafilesystem_ongc, 1 );
    lua_rawset( L, -3 );

    // Create the shim for all objects.
    lua_pushlstring( L, "classmt", 7 );
    luaclassmt_make( L, -3 );
    lua_rawset( L, -3 );

    lua_pop( L, 1 );

    // Setup the main Eir FileSystem functions.
    lua_pushvalue( L, -2 );
    lua_pushvalue( L, -2 );
    luaL_openlib( L, nullptr, fsysLib, 1 );
    lua_pop( L, 1 );

    // Cache the root translator into the lib.
    lua_pushvalue( L, -1 );
    lua_pushcclosure( L, luafsys_init, 1 );
    lua_call( L, 0, 1 );

    lua_pushlstring( L, "root", 4 );
    lua_pushvalue( L, -2 );
    lua_rawset( L, -5 );

    lua_pushlstring( L, "getRoot", 7 );
    lua_pushvalue( L, -2 );
    lua_pushcclosure( L, luafsys_getRoot, 1 );
    lua_rawset( L, -5 );

    lua_pop( L, 2 );

    // At the end we return the table that we created at the top.
}

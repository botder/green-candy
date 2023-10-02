/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafilesystem.cpp
*  PURPOSE:     Lua filesystem access
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
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
    int theTop = lua_gettop( L );

    luaL_checktype( L, 1, LUA_TSTRING );
    luaL_checktype( L, 2, LUA_TSTRING );

    lua_settop( L, 2 );

    CFile *file = ((CFileTranslator*)lua_getmethodtrans( L ))->Open( lua_tostring( L, 1 ), lua_tostring( L, 2 ) );

    if ( !file )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    lua_pushlightuserdata( L, file );
    lua_pushcclosure( L, luaconstructor_file, 1 );
#ifdef FU_CLASS
    lua_newclass( L, (unk*)file );
#else
    lua_newclass( L );

    // Register the file
    lua_getfield( L, 3, "setParent" );
    lua_getmethodclass( L )->Push( L );
    lua_call( L, 1, 0 );
#endif
    return 1;
}

static int filesystem_exists( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->Exists( lua_tostring( L, 1 ) ) );
    return 1;
}

static int filesystem_createDir( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->CreateDir( lua_tostring( L, 1 ) ) );
    return 1;
}

static int filesystem_chdir( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->ChangeDirectory( lua_tostring( L, 1 ) ) );
    return 1;
}

static int filesystem_delete( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->Delete( lua_tostring( L, 1 ) ) );
    return 1;
}

static int filesystem_copy( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    luaL_checktype( L, 2, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->Copy( lua_tostring( L, 1 ), lua_tostring( L, 2 ) ) );
    return 1;
}

static int filesystem_rename( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    luaL_checktype( L, 2, LUA_TSTRING );
    lua_pushboolean( L, ((CFileTranslator*)lua_getmethodtrans( L ))->Rename( lua_tostring( L, 1 ), lua_tostring( L, 2 ) ) );
    return 1;
}

static int filesystem_size( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );
    lua_pushwideinteger( L, (lua_WideInteger)((CFileTranslator*)lua_getmethodtrans( L ))->Size( lua_tostring( L, 1 ) ) );
    return 1;
}

static int filesystem_stat( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );

    struct stat stats;

    if ( !((CFileTranslator*)lua_getmethodtrans( L ))->Stat( lua_tostring( L, 1 ), &stats ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    luafile_pushStats( L, stats );
    return 1;
}

static int filesystem_relPath( lua_State *L )
{
    const char *src = lua_tostring( L, 1 );
    filePath path;

    if ( !src )
        src = "";

    if ( !((CFileTranslator*)lua_getmethodtrans( L ))->GetRelativePath( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    std::string ansiPath = path.convert_ansi();

    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    return 1;
}

static int filesystem_relPathRoot( lua_State *L )
{
    const char *src = lua_tostring( L, 1 );
    filePath path;

    if ( !src )
        src = "";

    if ( !((CFileTranslator*)lua_getmethodtrans( L ))->GetRelativePathFromRoot( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    std::string ansiPath = path.convert_ansi();

    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    return 1;
}

static int filesystem_absPath( lua_State *L )
{
    const char *src = lua_tostring( L, 1 );
    filePath path;

    if ( !src )
        src = "";

    if ( !((CFileTranslator*)lua_getmethodtrans( L ))->GetFullPath( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    std::string ansiPath = path.convert_ansi();

    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    return 1;
}

static int filesystem_absPathRoot( lua_State *L )
{
    const char *src = lua_tostring( L, 1 );
    filePath path;

    if ( !src )
        src = "";

    if ( !((CFileTranslator*)lua_getmethodtrans( L ))->GetFullPathFromRoot( src, true, path ) )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    std::string ansiPath = path.convert_ansi();

    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    return 1;
}

static void lua_findScanCallback( const filePath& path, void *ud )
{
    std::string ansiFilePath = path.convert_ansi();

    lua_pushlstring( (lua_State*)ud, ansiFilePath.c_str(), ansiFilePath.size() );
    luaL_ref( (lua_State*)ud, -2 );
}

static int filesystem_scanDir( lua_State *lua )
{
    luaL_checktype( lua, 1, LUA_TSTRING );

    const char *path = lua_tostring( lua, 1 );
    const char *wildcard;
    bool recursive;

    int top = lua_gettop( lua );

    if ( top > 1 )
    {
        wildcard = lua_tostring( lua, 2 );

        if ( top > 2 )
            recursive = ( lua_toboolean( lua, 3 ) != 0 );
        else
            recursive = false;
    }
    else
    {
        wildcard = "*";
        recursive = false;
    }

    lua_newtable( lua );

    ((CFileTranslator*)lua_getmethodtrans( lua ))->ScanDirectory( path, wildcard, recursive, lua_findScanCallback, lua_findScanCallback, lua );
    return 1;
}

static int filesystem_getFiles( lua_State *lua )
{
    luaL_checktype( lua, 1, LUA_TSTRING );

    const char *path = lua_tostring( lua, 1 );
    const char *wildcard;
    bool recursive;

    int top = lua_gettop( lua );

    if ( top > 1 )
    {
        wildcard = lua_tostring( lua, 2 );

        if ( top > 2 )
            recursive = ( lua_toboolean( lua, 3 ) != 0 );
        else
            recursive = false;
    }
    else
    {
        wildcard = "*";
        recursive = false;
    }

    lua_newtable( lua );

    ((CFileTranslator*)lua_getmethodtrans( lua ))->ScanDirectory( path, wildcard, recursive, 0, lua_findScanCallback, lua );
    return 1;
}

static int filesystem_getDirs( lua_State *lua )
{
    luaL_checktype( lua, 1, LUA_TSTRING );

    const char *path = lua_tostring( lua, 1 );
    bool recursive;

    if ( lua_gettop( lua ) > 1 )
        recursive = ( lua_toboolean( lua, 2 ) != 0 );
    else
        recursive = false;

    lua_newtable( lua );

    ((CFileTranslator*)lua_getmethodtrans( lua ))->ScanDirectory( path, "*", recursive, lua_findScanCallback, 0, lua );
    return 1;
}

static void filesystem_exfilecb( const filePath& path, void *ud )
{
    std::string ansiPath = path.convert_ansi();

    lua_State *L = (lua_State*)ud;
    lua_pushvalue( L, 4 );
    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    lua_call( L, 1, 0 );
}

static void filesystem_exdircb( const filePath& path, void *ud )
{
    std::string ansiPath = path.convert_ansi();

    lua_State *L = (lua_State*)ud;
    lua_pushvalue( L, 3 );
    lua_pushlstring( L, ansiPath.c_str(), ansiPath.size() );
    lua_call( L, 1, 0 );
}

static int filesystem_scanDirEx( lua_State *lua )
{
    luaL_checktype( lua, 1, LUA_TSTRING );
    luaL_checktype( lua, 2, LUA_TSTRING );

    ((CFileTranslator*)lua_getmethodtrans( lua ))->ScanDirectory(
        lua_tostring( lua, 1 ),
        lua_tostring( lua, 2 ),
        ( lua_toboolean( lua, 5 ) != 0 ),
        lua_type( lua, 3 ) == LUA_TFUNCTION ? filesystem_exdircb : NULL,
        lua_type( lua, 4 ) == LUA_TFUNCTION ? filesystem_exfilecb : NULL, lua );

    return 0;
}

static int filesystem_destroy( lua_State *L )
{
    delete (CFileTranslator*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg fsys_methods[] =
{
    { "destroy", filesystem_destroy },
#ifndef FU_CLASS
    { NULL, NULL }
};

static const luaL_Reg fsys_methods_trans[] =
{
#endif //FU_CLASS
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
    { NULL, NULL }
};

int luafsys_constructor( lua_State *L )
{
#ifndef FU_CLASS
    CFileTranslator *trans = (CFileTranslator*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass *j = lua_refclass( L, 1 );
    j->SetTransmit( LUACLASS_FILETRANSLATOR, trans );

    j->RegisterInterfaceTrans( L, fsys_methods_trans, 0, LUACLASS_FILETRANSLATOR );
#endif //FU_CLASS

    lua_pushvalue( L, LUA_ENVIRONINDEX );
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    lua_getfield( L, LUA_ENVIRONINDEX, "this" );
    luaL_openlib( L, NULL, fsys_methods, 2 );

    lua_pushlstring( L, "filesystem", 10 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

void luafsys_pushroot( lua_State *L, CFileTranslator *root )
{
    lua_pushlightuserdata( L, root );
    lua_pushcclosure( L, luafsys_constructor, 1 );
#ifdef FU_CLASS
    lua_newclass( L, (unk*)root );
#else
    lua_newclass( L );
#endif
}

int luafsys_createTranslator( lua_State *L )
{
    luaL_checktype( L, 1, LUA_TSTRING );

    CFileTranslator *root = pubFileSystem->CreateTranslator( lua_tostring( L, 1 ) );

    if ( !root )
    {
        lua_pushboolean( L, false );
        return 1;
    }

    luafsys_pushroot( L, root );
    return 1;
}

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

static int luafsys_copyFile( lua_State *L )
{
    CFileTranslator *srcTranslator = lua_readclass <CFileTranslator> ( L, 1, LUACLASS_FILETRANSLATOR );
    const char *srcPath = lua_tostring( L, 2 );
    CFileTranslator *dstTranslator = lua_readclass <CFileTranslator> ( L, 3, LUACLASS_FILETRANSLATOR );
    const char *dstPath = lua_tostring( L, 4 );

    LUA_CHECK( srcTranslator && srcPath && dstTranslator && dstPath );

    bool success = FileSystem::FileCopy( srcTranslator, srcPath, dstTranslator, dstPath );

    lua_pushboolean( L, success );
    return 1;
}

static int luafsys_copyStream( lua_State *L )
{
    CFile *srcStream = lua_readclass <CFile> ( L, 1, LUACLASS_FILE );
    CFile *dstStream = lua_readclass <CFile> ( L, 2, LUACLASS_FILE );

    LUA_CHECK( srcStream && dstStream );

    FileSystem::StreamCopy( *srcStream, *dstStream );

    lua_pushboolean( L, true );
    return 1;
}

static int luafsys_copyStreamCount( lua_State *L )
{
    CFile *srcStream = lua_readclass <CFile> ( L, 1, LUACLASS_FILE );
    CFile *dstStream = lua_readclass <CFile> ( L, 2, LUACLASS_FILE );
    int count = (int)lua_tonumber( L, 3 );

    LUA_CHECK( srcStream && dstStream );

    LUA_CHECK( count > 0 );

    FileSystem::StreamCopyCount( *srcStream, *dstStream, count );

    lua_pushboolean( L, true );
    return 1;
}
#endif //FU_CLASS

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
    CFile *srcFile = lua_readclass <CFile> ( L, 1, LUACLASS_FILE );
    CFile *dstFile = lua_readclass <CFile> ( L, 2, LUACLASS_FILE );

    LUA_CHECK( srcFile && dstFile );

    char sourceBuf[2048];
    char targetBuf[2048];

    bool isEqual = true;

    while ( !srcFile->IsEOF() )
    {
        size_t sourceReadCount = srcFile->Read( sourceBuf, 1, sizeof( sourceBuf ) );
        size_t destReadCount = dstFile->Read( targetBuf, 1, sizeof( targetBuf ) );

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

    lua_pushboolean( L, isEqual );
    return 1;
}

int luafsys_getRoot( lua_State *L )
{
    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    return 1;
}

static const luaL_Reg fsysLib[] =
{
    { "createTranslator", luafsys_createTranslator },
#ifndef FU_CLASS
    { "createArchiveTranslator", luafsys_createArchiveTranslator },
    { "createZIPArchive", luafsys_createZIPArchive },
    { "copyFile", luafsys_copyFile },
    { "copyStream", luafsys_copyStream },
    { "copyStreamCount", luafsys_copyStreamCount },
#endif //FU_CLASS
    { "pathToFilename", luafsys_pathToFilename },
    { "streamCompare", luafsys_streamCompare },
    { NULL, NULL }
};

int luafsys_init( lua_State *L )
{
    // Specify the root fileTranslator
    CFileTranslator *rootTranslator = pubFileSystem->CreateTranslator( "" ); // use the current directory.

    // We could fail to obtain the handle to the translator if the directory is handle-locked.
    // In that case, return false.
    LUA_CHECK( rootTranslator != NULL );

    // Return the Lua representation of the root translator.
    luafsys_pushroot( L, rootTranslator );
    return 1;
}

void luafilesystem_open( lua_State *L )
{
    lua_newtable( L );
    luaL_openlib( L, NULL, fsysLib, 0 );

    lua_pushlstring( L, "getRoot", 7 );
    lua_pushcclosure( L, luafsys_init, 0 );
    lua_call( L, 0, 1 );

    lua_pushlstring( L, "root", 4 );

    lua_pushvalue( L, -2 );

    lua_rawset( L, -5 );

    lua_pushcclosure( L, luafsys_getRoot, 1 );
    lua_rawset( L, -3 );
}

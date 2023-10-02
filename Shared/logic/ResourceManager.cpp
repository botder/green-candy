/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ResourceManager.cpp
*  PURPOSE:     Resource management for scripting environments
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CFileTranslator* ResourceManager::resFileRoot = NULL;

static int root_index( lua_State *L )
{
    if ( lua_type( L, 2 ) == LUA_TSTRING )
    {
        std::string key = lua_getstring( L, 2 );

        if ( key == "destroy" )
            return 0;
    }

    lua_getfield( L, LUA_ENVIRONINDEX, "super" );
    lua_insert( L, 1 );
    lua_call( L, 2, 1 );
    return 1;
}

static int root_destroy( lua_State *L )
{
    // This will be destroyed by the Lua environment!
    delete (ResourceManager*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg root_interface[] =
{
    { "__index", root_index },
    { "destroy", root_destroy },
    { NULL, NULL }
};

static int luaconstructor_root( lua_State *L )
{
    ResourceManager *root = (ResourceManager*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_ROOT, root );

    lua_basicprotect( L );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, root_interface, 1 );

    lua_pushlstring( L, "root", 4 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static inline ILuaClass* _trefget( lua_State *L, ResourceManager& manager )
{
    lua_pushlightuserdata( L, &manager );
    lua_pushcclosure( L, luaconstructor_root, 1 );
    lua_newclassex( L, LCLASS_API_LIGHT );

	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

ResourceManager::ResourceManager( lua_State *L ) : LuaClass( L, _trefget( L, *this ) )
{
}

ResourceManager::~ResourceManager()
{
}

// Inheritance has to provide it's own addition of resources

Resource* ResourceManager::Get( const char *name )
{
    Resource **res = MapFind( m_resByName, filePath( name ) );

    if ( !res )
        return NULL;

    return *res;
}

Resource* ResourceManager::Get( unsigned short id )
{
    resourceList_t::iterator iter = m_resources.begin();

    for ( ; iter != m_resources.end(); ++iter )
    {
        if ( (*iter)->GetID() == id )
            return *iter;
    }

    return NULL;
}

void ResourceManager::Remove( Resource *res )
{
    m_resources.remove( res );
    MapRemove( m_resByName, res->GetName() );
}

void ResourceManager::Remove( unsigned short id )
{
    Resource *res = Get( id );

    if ( !res )
        return;

    Remove( res );
}

bool ResourceManager::Exists( Resource *res ) const
{
    return m_resByName.find( res->GetName() ) != m_resByName.end();
}

bool ResourceManager::ParseResourcePath( Resource*& res, const char *path, const char*& meta )
{
    if ( path[0] == ':' )
    {
        path++;

        const char *delim = strchr( path, '/' );

        if ( !delim || delim == path )
            return false;

        res = Get( std::string( path, (size_t)( delim - path ) ).c_str() );

        if ( !res )
            return false;

        meta = delim + 1;
        return true;
    }

    meta = path;
    return true;
}

bool ResourceManager::ParseResourceFullPath( Resource*& res, const char *path, const char*& meta, filePath& absPath )
{
    if ( !ParseResourcePath( res, path, meta ) )
        return false;

    return res->GetFullMetaPath( meta, absPath );
}

bool ResourceManager::CreateDir( Resource *res, const char *path )
{
    if ( !ParseResourcePath( res, path, path ) )
        return false;

    return res->CreateDir( path );
}

CFile* ResourceManager::OpenStream( Resource *res, const char *path, const char *mode )
{
    if ( !ParseResourcePath( res, path, path ) )
        return NULL;

    return res->OpenStream( path, mode );
}

bool ResourceManager::FileCopy( Resource *res, const char *src, const char *dst )
{
    Resource *dstRes = res;

    if ( !ParseResourcePath( res, src, src ) || !ParseResourcePath( dstRes, dst, dst ) )
        return false;

    if ( res == dstRes )
        return res->FileCopy( src, dst );

    return resFileRoot->Copy( res->GetName() + "/" + src, dstRes->GetName() + "/" + dst );
}

bool ResourceManager::FileRename( Resource *res, const char *src, const char *dst )
{
    Resource *dstRes = res;

    if ( !ParseResourcePath( res, src, src ) || !ParseResourcePath( dstRes, dst, dst ) )
        return false;

    if ( res == dstRes )
        return res->FileRename( src, dst );

    return resFileRoot->Rename( res->GetName() + "/" + src, dstRes->GetName() + "/" + dst );
}

size_t ResourceManager::FileSize( Resource *res, const char *path )
{
    if ( !ParseResourcePath( res, path, path ) )
        return 0;

    return res->FileSize( path );
}

bool ResourceManager::FileExists( Resource *res, const char *path )
{
    return ParseResourcePath( res, path, path ) && res->FileExists( path );
}

bool ResourceManager::FileDelete( Resource *res, const char *path )
{
    return ParseResourcePath( res, path, path ) && res->FileDelete( path );
}
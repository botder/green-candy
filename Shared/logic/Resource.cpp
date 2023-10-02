/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/Resource.cpp
*  PURPOSE:     Resource management for scripting environments
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static int resource_getName( lua_State *L )
{
    Resource& res = *(Resource*)lua_touserdata( L, lua_upvalueindex( 1 ) );
    const filePath& name = res.GetName();

    lua_pushlstring( L, name.c_str(), name.size() );
    return 1;
}

static int resource_destroy( lua_State *L )
{
    delete (Resource*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    return 0;
}

static const luaL_Reg resource_interface[] =
{
    { "getName", resource_getName },
    { "destroy", resource_destroy },
    { NULL, NULL }
};

static int luaconstructor_resource( lua_State *L )
{
    Resource *res = (Resource*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RESOURCE, res );

    lua_basicprotect( L );

    lua_pushvalue( L, lua_upvalueindex( 1 ) );
    j.RegisterInterface( L, resource_interface, 1 );

    lua_pushlstring( L, "resource", 8 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

static inline ILuaClass* _trefget( lua_State *L, Resource& res )
{
    lua_pushlightuserdata( L, &res );
    lua_pushcclosure( L, luaconstructor_resource, 1 );
    lua_newclass( L );

	ILuaClass *j = lua_refclass( L, -1 );
	lua_pop( L, 1 );
	return j;
}

Resource::Resource( LuaMain& main, unsigned short id, const filePath& name, CFileTranslator& root ) : LuaClass( *main, _trefget( *main, *this ) ), m_lua( main ), m_fileRoot( root )
{
    m_name = name;
    m_id = id;
    m_active = false;

    // The Lua hyperstructure should assign us
    main.m_resource = this;

    // Register itself to the Lua environment
    lua_State *L = *main;

    PushStack( L );
    lua_setfield( L, LUA_GLOBALSINDEX, "resource" );
}

Resource::~Resource()
{
    // Delete its file root.
    if ( CFileTranslator *resRoot = &this->m_fileRoot )
    {
        delete resRoot;
    }
}

bool Resource::GetFullMetaPath( const char *path, filePath& absPath )
{
    return m_fileRoot.GetFullPath( path, true, absPath );
}

bool Resource::CreateDir( const char *path )
{
    return m_fileRoot.CreateDir( path );
}

CFile* Resource::OpenStream( const char *path, const char *mode )
{
    return m_fileRoot.Open( path, mode );
}

bool Resource::FileCopy( const char *src, const char *dst )
{
    return m_fileRoot.Copy( src, dst );
}

bool Resource::FileRename( const char *src, const char *dst )
{
    return m_fileRoot.Rename( src, dst );
}

size_t Resource::FileSize( const char *path ) const
{
    return m_fileRoot.Size( path );
}

bool Resource::FileExists( const char *path ) const
{
    return m_fileRoot.Exists( path );
}

bool Resource::FileDelete( const char *path )
{
    return m_fileRoot.Delete( path );
}

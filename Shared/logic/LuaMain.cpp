/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/LuaMain.cpp
*  PURPOSE:     Lua hyperstructure
*       This is a child environment of the LuaManager virtual machine.
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

LuaMain::LuaMain( LuaManager& manager ) : m_system( manager )
{
    m_lua = manager.m_lua;
}

LuaMain::~LuaMain()
{
    // TODO: Think about removing LuaMain out of the hyperstructure management!
}

LuaFunctionRef LuaMain::CreateReference( int stack )
{
    return CreateReference( m_lua, stack );
}

// Custom Lua 'stack argument'->'reference' routine
LuaFunctionRef LuaMain::CreateReference( lua_State *L, int stack )
{
    const void *ptr = lua_topointer( L, stack );
    CRefInfo *e_info;

    if ( !m_refStore.empty() && ( e_info = MapFind( m_refStore, ptr ) ) )
    {
        // Re-use the lua ref we already have
        e_info->refCount++;
        return LuaFunctionRef( this, e_info->idx, ptr );
    }

    // Get a lua ref
    lua_pushvalue( L, stack );
    int ref = luaL_ref( L, LUA_REGISTRYINDEX );

    // Save ref info
    CRefInfo info;
    info.refCount = 1;
    info.idx = ref;
    m_refStore[ptr] = info;

    return LuaFunctionRef( this, ref, ptr );
}

bool LuaMain::GetReference( int stack, LuaFunctionRef& ref )
{
    const void *ptr = lua_topointer( m_lua, stack );
    CRefInfo *info;

    if ( m_refStore.empty() || !( info = MapFind( m_refStore, ptr ) ) )
        return false;

    ref = LuaFunctionRef( this, info->idx, ptr );
    return true;
}

void LuaMain::Reference( const LuaFunctionRef& ref )
{
    if ( m_refStore.empty() )
        return;

    CRefInfo *iref = MapFind( m_refStore, ref.m_call );

    if ( !iref )
        return;

    iref->refCount++;
}

void LuaMain::Dereference( const LuaFunctionRef& ref )
{
    if ( m_refStore.empty() )
        return;

    CRefInfo *iref = MapFind( m_refStore, ref.m_call );

    if ( !iref )
        return;

    if ( --iref->refCount == 0 )
    {
        // Remove on last unuse
        lua_unref( m_lua, iref->idx );

        MapRemove( m_refStore, ref.m_call );
        MapRemove( m_tagStore, ref.m_ref );
    }
}

void LuaMain::PushReference( const LuaFunctionRef& ref )
{
    CRefInfo *iref = MapFind( m_refStore, ref.m_call );

    if ( !iref )
    {
        lua_pushnil( m_lua );
        return;
    }

    m_system.PushReference( m_lua, ref );
}

void LuaMain::RegisterFunction( const char *name, lua_CFunction proto )
{
    lua_pushcclosure( m_lua, proto, 0 );
    lua_setfield( m_lua, LUA_GLOBALSINDEX, name );
}

void LuaMain::CallStack( int args )
{
    m_system.AcquireContext( *this ).CallStack( args );
}

void LuaMain::CallStackVoid( int args )
{
    m_system.AcquireContext( *this ).CallStackVoid( args );
}

bool LuaMain::PCallStack( int argc )
{
    return m_system.AcquireContext( *this ).PCallStack( argc );
}

bool LuaMain::PCallStackVoid( int argc )
{
    return m_system.AcquireContext( *this ).PCallStackVoid( argc );
}

void LuaMain::InitSecurity()
{
    using namespace LuaFunctionDefs;

    // TODO: Safe implementation of these routines
    RegisterFunction( "dofile", DisabledFunction );
    RegisterFunction( "loadfile", DisabledFunction );
    RegisterFunction( "require", DisabledFunction );
    RegisterFunction( "loadlib", DisabledFunction );
    //RegisterFunction( "getfenv", DisabledFunction );
    RegisterFunction( "newproxy", DisabledFunction );
}

void LuaMain::InitVM( int structure, int meta )
{
    m_global = m_system.GetGlobalEnvironment();

    m_structure = structure;
    m_meta = meta;

    // Initialize security restrictions
    InitSecurity();

#if 0
    // Register module functions
    CLuaCFunctions::RegisterFunctionsWithVM ( m_luaVM );

    // Update global variables
    lua_pushelement ( m_luaVM, g_pClientGame->GetRootEntity () );
    lua_setglobal ( m_luaVM, "root" );

    lua_pushresource ( m_luaVM, m_pResource );
    lua_setglobal ( m_luaVM, "resource" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceEntity () );
    lua_setglobal ( m_luaVM, "resourceRoot" );

    lua_pushelement ( m_luaVM, m_pResource->GetResourceGUIEntity () );
    lua_setglobal ( m_luaVM, "guiRoot" );

    lua_pushelement ( m_luaVM, g_pClientGame->GetLocalPlayer() );
    lua_setglobal ( m_luaVM, "localPlayer" );

    // Load pre-loaded lua code
    LoadScript ( szPreloadedScript );
#endif
}

bool LuaMain::LoadScriptFromBuffer( const char *buf, size_t size, const char *path, bool utf8 )
{
    filePath relPath;

    if ( !ParseRelative( path, relPath ) )
        return false;

    std::string script;

    // Are we not marked as UTF-8 already, and not precompiled?
    if ( !utf8 && ( size < 5 || buf[0] != 27 || buf[1] != 'L' || buf[2] != 'u' || buf[3] != 'a' || buf[4] != 'Q' ) )
    {
        script = UTF16ToMbUTF8( ANSIToUTF16( std::string( buf, size ) ) );

        if ( size != script.size() )
        {
            size = script.size();

            m_system.m_debug.LogWarning( "Script '%s' is not encoded in UTF-8.  Loading as ANSI...", relPath.c_str() );
        }
    }
    else
        script = std::string( buf, size );

    // Run the script
    if ( luaL_loadbuffer( m_lua, script.c_str(), size, SString( "@%s", relPath.c_str() ) ) != 0 )
    {
        // Print the error
        std::string result = lua_getstring( m_lua, -1 );

        if ( result.length() )
        {
            Logger::LogPrintf( "SCRIPT ERROR: %s\n", result.c_str() );
            m_system.m_debug.LogWarning( "Loading script failed: %s", result.c_str() );
        }
        else
        {
            Logger::LogPrint( "SCRIPT ERROR: Unknown\n" );
            m_system.m_debug.LogInformation( "Loading script failed for unknown reason" );
        }
        return false;
    }

    return PCallStackVoid( 0 );
}

bool LuaMain::LoadScript( const char *buf )
{
    // Run the script
    if ( luaL_loadbuffer( m_lua, buf, strlen( buf ), NULL ) != 0 )
    {
        m_system.GetDebug().LogError( "Loading in-line script failed: %s", lua_tostring( m_lua, -1 ) );
        return false;
    }

    switch( lua_pcall( m_lua, 0, 0, 0 ) )
    {
    case LUA_ERRRUN:
    case LUA_ERRMEM:
        m_system.GetDebug().LogError( "Executing in-line script failed: %s", lua_tostring( m_lua, -1 ) );

        lua_pop( m_lua, 1 );
        return false;
    }

    return true;
}

void LuaMain::DoPulse()
{
    m_timers.DoPulse( *this );
}

#ifdef MTA_CLIENT

void LuaMain::DestroyXML( CXMLFile *file )
{
    m_XMLFiles.remove( file );
    delete file;
}

void LuaMain::DestroyXML( CXMLNode *root )
{
    std::list <CXMLFile*>::iterator iter = m_XMLFiles.begin();

    for ( ; iter != m_XMLFiles.end(); ++iter )
    {
        if ( (*iter)->GetRootNode() == root )
        {
            DestroyXML( *iter );
            return;
        }
    }
}

#endif //MTA_CLIENT

const SString& LuaMain::GetFunctionTag( int refID )
{
    // Find existing
    const SString* pTag = MapFind( m_tagStore, refID );
#ifndef MTA_DEBUG
    if ( !pTag )
#endif
    {
        // Create if required
        SString strText;

        lua_Debug debugInfo;
        lua_getref( m_lua, refID );

        if ( lua_getinfo( m_lua, ">nlS", &debugInfo ) )
        {
            filePath src;

            // Make sure this function isn't defined in a string
            if ( debugInfo.source[0] == '@' && ParseRelative( debugInfo.source, src ) )
            {
                std::string fileNamePart = FileSystem::GetFileNameItem( src.c_str(), true, NULL, NULL );

                strText = SString( "@%s:%d", fileNamePart.c_str(), debugInfo.currentline != -1 ? debugInfo.currentline : debugInfo.linedefined, refID );
            }
            else
            {
                strText = SString( "@func_%d %s", refID, debugInfo.short_src );
            }
        }
        else
            strText = SString( "@func_%d NULL", refID );

#ifdef MTA_DEBUG
        if ( pTag )
        {
            // Check tag remains unchanged
            assert( strText == *pTag );
            return *pTag;
        }
#endif

        m_tagStore[refID] = strText;
        pTag = &m_tagStore[refID];
    }
    return *pTag;
}
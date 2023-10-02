/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Resource.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( call )
    {
        CLuaMain *pLuaMain = lua_readcontext( L );
        CResource *pThisResource = pLuaMain->GetResource();

        if ( CResource *res = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE ) )
        {
            // Get the target Lua VM
            LuaMain& t_main = res->GetVM();
            lua_State *targetL = *t_main;
            lua_State *preserve = m_pLuaManager->GetVirtualMachine();
            int argc = lua_gettop( L ) - 2;

            std::string name = lua_getstring( L, 2 );

            // Allocate enough stack size
            lua_checkstack( targetL, std::max( 2, argc ) );

            // Store globals
            lua_getglobal( targetL, "sourceResource" );
            lua_getglobal( targetL, "sourceResourceRoot" );

            // Set globals
            pThisResource->PushStack( targetL );                            lua_setglobal( targetL, "sourceResource" );
            pThisResource->GetResourceEntity()->PushStack( targetL );       lua_setglobal( targetL, "sourceResourceRoot" );

            int top = lua_gettop( targetL );
            int rcount;

            // Call the exported function with the given name and the args
            bool rslt;
            CResource::exports_t::iterator iter = res->IterBeginExports();

            for ( ; iter != res->IterEndExports(); iter++ )
            {
                if ( (*iter)->GetFunctionName() == name )
                {
                    lua_getfield( targetL, LUA_GLOBALSINDEX, name.c_str() );

                    if ( lua_type( targetL, -1 ) == LUA_TNIL )
                    {
                        lua_pop( targetL, 1 );
                        break;
                    }

                    // Move the arguments over to the target env
                    if ( targetL != L )
                        lua_xmove( L, targetL, argc );
                    else
                    {
                        // Insert the function at the start of the arguments
                        lua_insert( L, 3 );
                    }
                    
                    rslt = t_main.PCallStack( argc );
                    rcount = lua_gettop( targetL ) - top;

                    if ( targetL != L )
                        lua_xmove( targetL, L, rcount );
                    else
                    {
                        lua_checkstack( preserve, rcount );
                        lua_xmove( targetL, preserve, rcount );
                    }

                    goto success;
                }
            }

            // We failed to call a function
            rslt = false;

success:
            // Restore globals
            lua_setglobal( targetL, "sourceResourceRoot" );
            lua_setglobal( targetL, "sourceResource" );

            if ( rslt )
            {
                if ( L == targetL && rcount )
                    lua_xmove( preserve, L, rcount );

                return rcount;
            }
            else
                m_pScriptDebugging->LogError( "call: failed to call '%s:%s'", res->GetName().c_str(), name.c_str() );
        }
        else
            m_pScriptDebugging->LogBadPointer( "call", "resource", 1 );

        // Failed
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( getThisResource )
    {
        lua_readcontext( L )->GetResource()->PushStack( L );
        return 1;
    }

    LUA_DECLARE( getResourceConfig )
    {
        if ( lua_istype ( L, 1, LUA_TLIGHTUSERDATA ) )
            m_pScriptDebugging->LogCustom( "getResourceConfig may be using an outdated syntax. Please check and update." );

        // Resource and config name
        CResource* pResource = NULL;
        const char* szInput = NULL;
        filePath strAbsPath;
        const char *meta;

        if ( lua_istype ( L, 1, LUA_TSTRING ) )
        {
            // Grab our lua main
            CLuaMain* pLuaMain = lua_readcontext( L );

            // Grab resource and the config name from arg
            pResource = pLuaMain->GetResource ();
            szInput = lua_tostring ( L, 1 );
        }

        // We have both a resource file to grab the config from and a config name?
        if ( pResource && szInput )
        {
            if ( m_pResourceManager->ParseResourceFullPath( (Resource*&)pResource, szInput, meta, strAbsPath ) )
            {
                // Loop through the configs in that resource
                std::list < CResourceConfigItem* >::iterator iter = pResource->ConfigIterBegin ();
                for ( ; iter != pResource->ConfigIterEnd (); iter++ )
                {
                    // Matching name?
                    if ( strcmp ( (*iter)->GetShortName(), meta ) == 0 )
                    {
                        // Return it
                        CResourceConfigItem* pConfig = (CResourceConfigItem*) (*iter);
                        CXMLNode* pNode = pConfig->GetRoot ();
                        if ( pNode )
                        {
                            lua_pushxmlnode ( L, pNode );
                            return 1;
                        }
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getResourceConfig" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getResourceName )
    {
        // Verify arguments
        if ( CResource *res = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE ) )
        {
            // Grab its name and return it
            std::string ansiResName = res->GetName().convert_ansi();

            lua_pushstring( L, ansiResName.c_str() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getResourceName", "resource", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getResourceFromName )
    {
        // Verify arguments
        if ( lua_isstring( L, 1 ) )
        {
            // Grab the argument
            const char* szResource = lua_tostring ( L, 1 );

            // Try to find a resource with that name
            CResource* pResource = (CResource*)m_pResourceManager->Get( szResource );
            if ( pResource )
            {
                pResource->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getResourceFromName" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getResourceRootElement )
    {
        // Resource given?
        CResource *pResource;
        if ( lua_istype( L, 1, LUA_TCLASS ) )
        {
            pResource = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE );

            if ( !pResource )
                goto error;
        }

        // No resource given, get this resource's root
        else if ( lua_istype ( L, 1, LUA_TNONE ) )
        {
            // Find our vm and get the root
            CLuaMain* pLuaMain = lua_readcontext( L );
            pResource = pLuaMain->GetResource ();
        }
        else
        {
error:
            m_pScriptDebugging->LogBadPointer( __FUNCTION__, "resource", 1 );

            lua_pushboolean( L, false );
            return 1;
        }

        // Grab the root element of it and return it if it existed
        pResource->GetResourceEntity()->PushStack( L );
        return 1;
    }

    LUA_DECLARE( getResourceGUIElement )
    {
        // Resource given?
        CResource *pResource;
        if ( lua_istype( L, 1, LUA_TCLASS ) )
        {
            pResource = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE );
        }

        // No resource given, get this resource's root
        else if ( lua_istype ( L, 1, LUA_TNONE ) )
        {
            // Find our vm and get the root
            CLuaMain* pLuaMain = lua_readcontext( L );
            pResource = pLuaMain->GetResource ();
        }
        else
        {
            m_pScriptDebugging->LogBadPointer( __FUNCTION__, "resource", 1 );
            lua_pushboolean( L, false );
            return 1;
        }

        pResource->GetResourceGUIEntity()->PushStack( L );
        return 1;
    }

    LUA_DECLARE( getResourceDynamicElementRoot )
    {
        if ( CResource *res = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE ) )
        {
            CClientEntity *pEntity = res->GetResourceDynamicEntity();
            if ( pEntity )
            {
                pEntity->PushStack( L );
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "getResourceDynamicElementRoot: Resource %s Is Not Currently Running", res->GetName() );
        }
        else
            m_pScriptDebugging->LogBadPointer( "getResourceDynamicElementRoot", "resource", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getResourceExportedFunctions )
    {
        CResource *resource = NULL;
        
        // resource
        if ( lua_istype( L, 1, LUA_TCLASS ) )
            resource = lua_readclass <CResource> ( L, 1, LUACLASS_RESOURCE );
        else if ( lua_istype( L, 1, LUA_TNONE ) )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            resource = pLuaMain->GetResource();
        }

        if ( resource )
        {
            lua_newtable( L );

            unsigned int n = 1;
            CResource::exports_t::iterator iterd = resource->IterBeginExports();

            for ( ; iterd != resource->IterEndExports(); iterd++, n++ )
            {
                lua_pushnumber( L, n );
                lua_pushstring( L, (*iterd)->GetFunctionName().c_str() );
                lua_settable( L, -3 );
            }

            return 1;
        }
        m_pScriptDebugging->LogBadType( "getResourceExportedFunctions" );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( fileCreateTranslator )
    {
        luaL_checktype( L, 1, LUA_TSTRING );

        const char *meta;

        filePath root;
        Resource *res = lua_readcontext( L )->GetResource();
        
        if ( !m_pResourceManager->ParseResourceFullPath( res, lua_tostring( L, 1 ), meta, root ) )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        CFileTranslator *fileRoot = g_pCore->GetFileSystem()->CreateTranslator( root.c_str() );

        if ( !fileRoot )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        luafsys_pushroot( L, fileRoot );
        return 1;
    }
}
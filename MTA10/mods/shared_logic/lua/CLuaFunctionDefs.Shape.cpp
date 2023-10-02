/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Shape.cpp
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
    LUA_DECLARE( createColCircle )
    {
        float x, y, radius;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( x ); argStream.ReadNumber( y );
        argStream.ReadNumber( radius, 0.1f );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource();

            // Create it and return it
            CClientColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle( *pResource, CVector( x, y, 0 ), radius );
            if ( pShape )
            {
                CElementGroup *pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createColCuboid )
    {
        CVector pos, size;

        CScriptArgReader argStream( L );

        argStream.ReadVector( pos );
        argStream.ReadVector( size );
    
        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource ();

            // Create it and return it
            CClientColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid( *pResource, pos, size );
            if ( pShape )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createColSphere )
    {
        CVector pos;
        float radius;

        CScriptArgReader argStream( L );

        argStream.ReadVector( pos );
        argStream.ReadNumber( radius );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource ();

            // Create it and return it
            CClientColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere ( *pResource, pos, radius );
            if ( pShape )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createColRectangle )
    {
        float x, y, width, height;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( x ); argStream.ReadNumber( y );
        argStream.ReadNumber( width ); argStream.ReadNumber( height );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource ();

            // Create it and return it
            CClientColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle( *pResource, CVector( x, y, 0 ), CVector( width, height, 0 ) );
            if ( pShape )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createColPolygon )
    { // Formerly createColSquare
        float x, y;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( x ); argStream.ReadNumber( y );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource();

            // Create it and return it
            CClientColPolygon *pShape = CStaticFunctionDefinitions::CreateColPolygon( *pResource, CVector( x, y, 0 ) );
            if ( pShape )
            {
                // Get the points
                while ( argStream.ReadNumber( x ) && argStream.ReadNumber( y ) )
                    pShape->AddPoint( CVector2D( x, y ) );

                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createColTube )
    {
        CVector pos;
        float radius, height;

        CScriptArgReader argStream( L );

        argStream.ReadVector( pos );
        argStream.ReadNumber( radius );
        argStream.ReadNumber( height );

        if ( !argStream.HasErrors() )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain->GetResource ();

            // Create it and return it
            CClientColTube* pShape = CStaticFunctionDefinitions::CreateColTube ( *pResource, pos, std::max( 0.1f, radius ), std::max( 0.1f, height ) );
            if ( pShape )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pShape );
                }
                pShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }
}
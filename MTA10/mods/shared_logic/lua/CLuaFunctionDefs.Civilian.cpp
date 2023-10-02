/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Civilian.cpp
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
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

namespace CLuaFunctionDefs
{
    LUA_DECLARE( createCivilian )
    {
        modelId_t ulModel;
        float x, y, z, fRotation;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( ulModel );
        argStream.ReadNumber( x );
        argStream.ReadNumber( y );
        argStream.ReadNumber( z );
        argStream.ReadNumber( fRotation, 0 );

        if ( !argStream.HasErrors() )
        {
            // Valid model?
            if ( CClientPlayerManager::IsValidModel ( ulModel ) )
            {
                // Convert the rotation to radians
                float fRotationRadians = ConvertDegreesToRadians ( fRotation );
                // Clamp it to -PI .. PI
                if ( fRotationRadians < -PI )
                {
                    do
                    {
                        fRotationRadians += PI * 2.0f;
                    } while ( fRotationRadians < -PI );
                }
                else if ( fRotationRadians > PI )
                {
                    do
                    {
                        fRotationRadians -= PI * 2.0f;
                    } while ( fRotationRadians > PI );
                }

                CResource *res = lua_readcontext( L )->GetResource();

                // Create it
                CClientCivilian *civ = new CClientCivilian( m_pManager, ulModel, INVALID_ELEMENT_ID, *res->GetVM() );
                civ->SetRoot( res->GetResourceDynamicEntity() );
                civ->SetPosition( CVector( x, y, z ) );
                civ->SetCurrentRotation( fRotationRadians );

                civ->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }
};
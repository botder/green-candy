/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Element.cpp
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
    LUA_DECLARE( getRootElement )
    {
        CStaticFunctionDefinitions::GetRootElement()->PushStack( L );
        return 1;
    }

    LUA_DECLARE( isElement )
    {
        if ( lua_type( L, 1 ) != LUA_TCLASS )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        ILuaClass& j = *lua_refclass( L, 1 );

        lua_pushboolean( L, !j.IsDestroyed() && j.IsTransmit( LUACLASS_ENTITY ) );
        return 1;
    }

    LUA_DECLARE( getElementID )
    {
        // Correct argument type specified?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushstring( L, entity->GetName() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "getElementID" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementByID )
    {
        // Eventually read out the index
        unsigned int uiIndex = 0;

        if ( lua_isnumber( L, 2 ) )
            uiIndex = (unsigned int)lua_tonumber( L, 2 );

        // Check that we got the name in place
        if ( lua_isstring( L, 1 ) )
        {
            // Try to find the element with that ID. Return it
            CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByID( lua_tostring( L, 1 ), uiIndex );
            if ( pEntity )
            {
                pEntity->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getElementByID" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementByIndex )
    {
        SString type;
        unsigned int index;

        CScriptArgReader argStream( L );

        argStream.ReadString( type );
        argStream.ReadNumber( index );

        if ( !argStream.HasErrors() )
        {
            // Try to find it. Return it if we did.
            CClientEntity* pEntity = CStaticFunctionDefinitions::GetElementByIndex( type, index );
            if ( pEntity )
            {
                pEntity->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getElementByIndex" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementData )
    {
    //  var getElementData ( element theElement, string key [, inherit = true] )
        CClientEntity* pEntity; SString strKey; bool bInherit;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadString ( strKey );
        argStream.ReadBool ( bInherit, true );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );

            if ( strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom( SString ( "Truncated argument @ '%s' [%s]", "getElementData", *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            lua_getfield( L, 1, "data" );
            lua_getfield( L, -1, strKey.c_str() );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getElementData", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementMatrix )
    {
        // Verify the argument
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the position
            RwMatrix matrix;
            if ( CStaticFunctionDefinitions::GetElementMatrix( *entity, matrix ) )
            {
                lua_creatematrix( L, matrix );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getElementMatrix" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementPosition )
    {
        // Verify the argument
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the position
            CVector vecPosition;
            entity->GetPosition( vecPosition );

            lua_pushnumber( L, vecPosition.fX );
            lua_pushnumber( L, vecPosition.fY );
            lua_pushnumber( L, vecPosition.fZ );
            return 3;
        }
        else
            m_pScriptDebugging->LogBadType( "getElementPosition" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementRotation )
    {
        // Verify the argument
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            const char* szRotationOrder = "default";

            if ( lua_isstring( L, 2 ) ) 
                szRotationOrder = lua_tostring ( L, 2 );

            // Grab the rotation
            CVector vecRotation;
            if ( CStaticFunctionDefinitions::GetElementRotation( *entity, vecRotation, szRotationOrder ) )
            {            
                // Return it
                lua_pushnumber( L, vecRotation.fX );
                lua_pushnumber( L, vecRotation.fY );
                lua_pushnumber( L, vecRotation.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementRotation", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementVelocity )
    {
        // Verify the argument
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the velocity
            CVector vecVelocity;
            if ( CStaticFunctionDefinitions::GetElementVelocity( *entity, vecVelocity ) )
            {
                // Return it
                lua_pushnumber( L, vecVelocity.fX );
                lua_pushnumber( L, vecVelocity.fY );
                lua_pushnumber( L, vecVelocity.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementVelocity", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementType )
    {
        // Check the arg type
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Get its typename
            lua_pushstring( L, entity->GetTypeName() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementType", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementChildren )
    {
        // Correct arg type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Create a new table
            lua_newtable ( L );

            // Add all the elements with a matching type to it
            entity->GetChildren ( L );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementChildren", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementChild )
    {
        CClientEntity *entity;
        unsigned int index;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( index );

        if ( !argStream.HasErrors() )
        {
            // Grab the child
            CClientEntity* pChild = CStaticFunctionDefinitions::GetElementChild( *entity, index );
            if ( pChild )
            {
                pChild->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "getElementChild" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementChildrenCount )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushnumber( L, entity->CountChildren() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementChildrenCount", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementParent )
    {
        // Correct arg type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the parent and return it
            CClientEntity *pParent = entity->GetParent();
            if ( pParent )
            {
                pParent->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementParent", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementsByType )
    {
        SString typeName;
        CClientEntity *root;
        bool streamedIn;

        CScriptArgReader argStream( L );

        argStream.ReadString( typeName );
        argStream.ReadClass( root, LUACLASS_ENTITY, m_pRootEntity );
        argStream.ReadBool( streamedIn, false );
        
        if ( !argStream.HasErrors() )
        {
            lua_newtable( L );

            // Add all the elements with a matching type to it
            root->FindAllChildrenByType( typeName, L, streamedIn );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementInterior )
    {
        // Check arg types
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushnumber( L, entity->GetInterior() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementInterior", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementWithinColShape )
    {
        CClientEntity *entity;
        CClientColShape *colShape;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( colShape, LUACLASS_COLSHAPE );

        if ( !argStream.HasErrors() )
        {
            // Check if it's within
            lua_pushboolean( L, entity->CollisionExists( colShape ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( isElementWithinMarker )
    {
        CClientEntity *entity;
        CClientMarker *marker;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( marker, LUACLASS_MARKER );

        // Check arg types
        if ( !argStream.HasErrors() )
        {
            // Check if it's within
            lua_pushboolean( L, entity->CollisionExists( marker->GetColShape() ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "isElementWithinMarker" );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getElementsWithinColShape )
    {
        // Correct arg type?
        if ( CClientColShape *colShape = lua_readclass <CClientColShape> ( L, 1, LUACLASS_COLSHAPE ) )
        {
            // Grab optional type arg
            const char* szType = NULL;
            if ( lua_isstring( L, 2 ) )
            {
                // Is it zero length? Don't use it
                szType = lua_tostring( L, 2 );

                if ( szType[ 0 ] == 0 )
                    szType = NULL;
            }

            // Create a new table
            lua_newtable( L );

            // Add all the elements within the shape to it
            unsigned int uiIndex = 0;
            std::list <CClientEntity*>::iterator iter = colShape->CollidersBegin();
            for ( ; iter != colShape->CollidersEnd(); ++iter )
            {
                if ( szType == NULL || strcmp( (*iter)->GetTypeName(), szType ) == 0 )
                {
                    lua_pushnumber( L, ++uiIndex );
                    (*iter)->PushStack( L );
                    lua_settable( L, -3 );
                }
            }

            // We're returning 1 table
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementsWithinColShape", "colshape", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementDimension )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the dimension
            lua_pushnumber( L, entity->GetDimension() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementDimension", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementBoundingBox )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the bounding box and return it
            CVector vecMin, vecMax;
            if ( CStaticFunctionDefinitions::GetElementBoundingBox( *entity, vecMin, vecMax ) )
            {
                lua_pushnumber( L, vecMin.fX );
                lua_pushnumber( L, vecMin.fY );
                lua_pushnumber( L, vecMin.fZ );
                lua_pushnumber( L, vecMax.fX );
                lua_pushnumber( L, vecMax.fY );
                lua_pushnumber( L, vecMax.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementBoundingBox", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementRadius )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab its radius and return it
            float fRadius = 0.0f;
            if ( CStaticFunctionDefinitions::GetElementRadius ( *entity, fRadius ) )
            {
                lua_pushnumber( L, fRadius );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementRadius", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementAttached )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            CClientEntity *pEntityAttachedTo = entity->GetAttachedTo();

            lua_pushboolean( L, pEntityAttachedTo && pEntityAttachedTo->IsEntityAttached( entity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementAttached", "element", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getElementAttachedTo )
    {
        // Check types
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the enity attached to it
            CClientEntity *pEntityAttachedTo = CStaticFunctionDefinitions::GetElementAttachedTo( *entity );

            if ( pEntityAttachedTo )
            {
                pEntityAttachedTo->PushStack( L );
                return 1;
            }
            else
            {
                lua_pushboolean( L, false );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementAttachedTo", "element", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getAttachedElements )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Create a new table
            lua_newtable ( L );

            // Add All Attached Elements
            unsigned int uiIndex = 0;
            std::list <CClientEntity*>::const_iterator iter = entity->AttachedEntitiesBegin();
            for ( ; iter != entity->AttachedEntitiesEnd(); ++iter )
            {
                CClientEntity *pAttached = *iter;
                if ( pAttached->GetAttachedTo() == entity )
                {
                    lua_pushnumber( L, ++uiIndex );
                    pAttached->PushStack( L );
                    lua_settable( L, -3 );
                }
            }
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getAttachedElements", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementDistanceFromCentreOfMassToBaseOfModel )
    {
        // Valid types?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the distance and return it
            float fDistance;
            if ( CStaticFunctionDefinitions::GetElementDistanceFromCentreOfMassToBaseOfModel( *entity, fDistance ) )
            {
                lua_pushnumber( L, fDistance );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementDistanceFromCentreOfMassToBaseOfModel", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementLocal )
    {
        // Check types
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Return whether it's local or not
            lua_pushboolean( L, entity->IsLocalEntity() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementLocal", "element", 1 );

        // Failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( getElementAttachedOffsets )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            CVector vecPosition, vecRotation;

            if ( CStaticFunctionDefinitions::GetElementAttachedOffsets( *entity, vecPosition, vecRotation ) )
            {
                lua_pushnumber( L, vecPosition.fX );
                lua_pushnumber( L, vecPosition.fY );
                lua_pushnumber( L, vecPosition.fZ );
                lua_pushnumber( L, vecRotation.fX );
                lua_pushnumber( L, vecRotation.fY );
                lua_pushnumber( L, vecRotation.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementAttachedOffsets", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementAlpha )
    {
        // Valid type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab its alpha level and return it
            unsigned char ucAlpha;
            if ( CStaticFunctionDefinitions::GetElementAlpha( *entity, ucAlpha ) )
            {
                lua_pushnumber( L, ucAlpha );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementAlpha", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementHealth )
    {
        // Same type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Grab the health and return it
            float fHealth;
            if ( CStaticFunctionDefinitions::GetElementHealth ( *entity, fHealth ) )
            {
                lua_pushnumber( L, fHealth );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementHealth", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementModel )
    {
        // Same type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            unsigned short usModel;
            if ( CStaticFunctionDefinitions::GetElementModel( *entity, usModel ) )
            {
                lua_pushnumber( L, usModel );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementModel", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementColShape )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            CClientEntity* pColShape = CStaticFunctionDefinitions::GetElementColShape( entity );
            if ( pColShape )
            {
                pColShape->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementColShape", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementInWater )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            bool bInWater = false;
            if ( CStaticFunctionDefinitions::IsElementInWater( *entity, bInWater ) )
            {
                lua_pushboolean( L, bInWater );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "isElementInWater" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementSyncer )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            bool bIsSyncer = false;
            if ( CStaticFunctionDefinitions::IsElementSyncer( *entity, bIsSyncer ) )
            {
                lua_pushboolean( L, bIsSyncer );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadType( "isElementSyncer" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementCollidableWith )
    {
        CClientEntity *entity;
        CClientEntity *collideWith;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( collideWith, LUACLASS_ENTITY );

        if ( !argStream.HasErrors() )
        {
            bool bCanCollide;
            if ( CStaticFunctionDefinitions::IsElementCollidableWith( *entity, *collideWith, bCanCollide ) )
            {
                lua_pushboolean( L, bCanCollide );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementDoubleSided )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, entity->IsDoubleSided() );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "isElementDoubleSided" );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getElementCollisionsEnabled )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::GetElementCollisionsEnabled( *entity ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "getElementCollisionsEnabled", "element", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementFrozen )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            bool bFrozen;
            if ( CStaticFunctionDefinitions::IsElementFrozen( *entity, bFrozen ) )
            {
                lua_pushboolean( L, bFrozen );
                return 1;
            }        
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementFrozen", "element", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isElementStreamedIn )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Is this a streaming compatible class?
            if ( entity->IsStreamingCompatibleClass() )
            {
                // Return whether or not this class is streamed in
                lua_pushboolean( L, ((CClientStreamElement*)entity)->IsStreamedIn() );
                return 1;
            }
            else if ( entity->GetType() == CCLIENTSOUND )
            {
                lua_pushboolean( L, ((CClientSound*)entity)->IsSoundStopped() );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning( "isElementStreamedIn; element is not streaming compatible\n" );
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementStreamedIn", "element", 1 );

        // We failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( isElementStreamable )
    {
        // We have a correct parameter 1?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Is this a streaming compatible class?
            if ( entity->IsStreamingCompatibleClass() )
            {
                CClientStreamElement* pStreamElement = static_cast < CClientStreamElement* > ( entity );

                // Return whether or not this element is set to never be streamed out
                lua_pushboolean( L, pStreamElement->GetTotalStreamReferences () == 0 );
                return 1;
            }
            else
                m_pScriptDebugging->LogWarning( "isElementStreamable; element is not streaming compatible\n" );

            // Return false, we're not streamable
            lua_pushboolean ( L, false );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementStreamable", "element", 1 );

        // We failed
        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( isElementOnScreen )
    {
        // Valid type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Return whether we're on the screen or not
            bool bOnScreen;
            if ( CStaticFunctionDefinitions::IsElementOnScreen( *entity, bOnScreen ) )
            {
                lua_pushboolean( L, bOnScreen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "isElementOnScreen", "element", 1 );

        lua_pushnil ( L );
        return 1;
    }

    LUA_DECLARE( createElement )
    {
        CLuaMain* pLuaMain = lua_readcontext( L );
        CResource * pResource = pLuaMain->GetResource();

        SString typeName, id;

        CScriptArgReader argStream( L );

        argStream.ReadString( typeName );
        argStream.ReadString( id, "" );

        // Verify argument types
        if ( !argStream.HasErrors() )
        {
            // Try to create
            CClientDummy* pDummy = CStaticFunctionDefinitions::CreateElement( *pResource, typeName.c_str(), id.c_str() );
            if ( pDummy )
            {
                // Add it to the element group
                // TODO: Get rid of element groups
                CElementGroup *pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add( pDummy );
                }

                // Return it
                pDummy->PushStack( L );
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "createElement; element limit reached\n" );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( destroyElement )
    {
        // Correct type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            if ( !entity->IsSystemEntity() )
            {
                // Attempt destruction
                entity->PushMethod( L, "destroy" );
                lua_call( L, 0, 0 );

                lua_pushboolean( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer( "destroyElement", "element", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementID )
    {
        CClientEntity *entity;
        SString id;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadString( id );

        // Correct type?
        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementID( *entity, id ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementData )
    {
    //  bool setElementData ( element theElement, string key, var value, [bool synchronize = true] )
        CClientEntity* pEntity; SString strKey;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadString( strKey );

        if ( !argStream.HasErrors() )
        {
            if ( strKey.length() > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom( SString ( "Truncated argument @ '%s' [%s]", "setElementData", *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            lua_getfield( L, 1, "data" );
            lua_getfield( L, -1, strKey.c_str() );

            if ( lua_isnil( L, 3 ) || !lua_equal( L, -1, 3 ) )
            {
                // Should we synchronize?
                if ( lua_toboolean( L, 4 ) )
                {
                    // Allocate a bitstream
                    NetBitStreamInterface *pBitStream = g_pNet->AllocateNetBitStream();
                    if ( pBitStream )
                    {
                        // Write element ID, name length and the name. Also write the variable.
                        pBitStream->Write( pEntity->GetID () );
                        pBitStream->WriteStringCompressed( strKey );
                        RakNet_WriteArgument( *pBitStream, L, 3 );

                        // Send the packet and deallocate
                        g_pNet->SendPacket( PACKET_ID_CUSTOM_DATA, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_CHAT );
                        g_pNet->DeallocateNetBitStream( pBitStream );

                        // Set its custom data
                        lua_pushvalue( L, 3 );
                        lua_setfield( L, -3, strKey.c_str() );
                        lua_pushboolean( L, true );
                        return 1;
                    }
                }
                else
                {
                    // Set its custom data
                    lua_pushvalue( L, 3 );
                    lua_setfield( L, -3, strKey.c_str() );
                    lua_pushboolean( L, true );
                    return 1;
                }
            }

            lua_pushboolean( L, false );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( removeElementData )
    {
    //  bool removeElementData ( element theElement, string key )
        CClientEntity* pEntity; SString strKey;;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pEntity, LUACLASS_ENTITY );
        argStream.ReadString ( strKey );

        if ( !argStream.HasErrors() )
        {
            if ( strKey.length () > MAX_CUSTOMDATA_NAME_LENGTH )
            {
                // Warn and truncate if key is too long
                m_pScriptDebugging->LogCustom( SString ( "Truncated argument @ '%s' [%s]", "removeElementData", *SString ( "string length reduced to %d characters at argument 2", MAX_CUSTOMDATA_NAME_LENGTH ) ) );
                strKey = strKey.Left ( MAX_CUSTOMDATA_NAME_LENGTH );
            }

            lua_pushboolean( L, CStaticFunctionDefinitions::RemoveElementData ( *pEntity, strKey ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "removeElementData", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementPosition )
    {
        CClientEntity *entity;
        CVector pos;
        bool warp;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( pos );
        argStream.ReadBool( warp, true );

        if ( !argStream.HasErrors() )
        {
            // Try to set the position
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementPosition( *entity, pos, warp ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Error
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementRotation )
    {
        CClientEntity *entity;
        CVector rot;
        SString rotOrder;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( rot );
        argStream.ReadString( rotOrder, "default" );

        if ( !argStream.HasErrors() )
        {
            // Try to set the position
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementRotation( *entity, rot, rotOrder ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Error
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementVelocity )
    {
        CClientEntity *entity;
        CVector vel;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadVector( vel );

        // Verify the first argument
        if ( !argStream.HasErrors() )
        {
            // Set the velocity
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementVelocity( *entity, vel ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "setElementVelocity" );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementParent )
    {
        CClientEntity *entity, *parent;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( parent, LUACLASS_ENTITY );

        // Correct args?
        if ( !argStream.HasErrors() )
        {
            // Change the parent
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementParent( *entity, *parent, lua_readcontext( L ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementInterior )
    {
        CClientEntity *entity;
        unsigned char interior;
        CVector pos;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( interior );
        bool setPos = argStream.ReadVector( pos, CVector( 0, 0, 0 ) );

        if ( !argStream.HasErrors() )
        {
            // Set the interior
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementInterior( *entity, interior, setPos, pos ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementDimension )
    {
        CClientEntity *entity;
        unsigned short dimension;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( dimension );

        if ( !argStream.HasErrors() )
        {
            // Change the dimension
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementDimension( *entity, dimension ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( attachElements )
    {
        CClientEntity *entity, *attachTo;
        CVector offsetPos;
        CVector offsetRot;

        CScriptArgReader argStream( L );
        
        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( attachTo, LUACLASS_ENTITY );
        argStream.ReadNumber( offsetPos.fX, 0 ); argStream.ReadNumber( offsetPos.fY, 0 ); argStream.ReadNumber( offsetPos.fZ, 0 );
        argStream.ReadNumber( offsetRot.fX, 0 ); argStream.ReadNumber( offsetRot.fY, 0 ); argStream.ReadNumber( offsetRot.fZ, 0 );

        // Valid arguments?
        if ( !argStream.HasErrors() )
        {
            // Try to attach them
            lua_pushboolean( L, CStaticFunctionDefinitions::AttachElements( *entity, *attachTo, offsetPos, offsetRot ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( detachElements )
    {
        // Valid arg type?
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            // Try to detach
            lua_pushboolean( L, CStaticFunctionDefinitions::DetachElements( *entity, lua_readclass <CClientEntity> ( L, 2, LUACLASS_ENTITY ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "detachElements", "element", 1 );

        // Failure
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementAttachedOffsets )
    {
        CClientEntity *entity;
        CVector pos, rot;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( pos.fX, 0 ); argStream.ReadNumber( pos.fY, 0 ); argStream.ReadNumber( pos.fZ, 0 );
        argStream.ReadNumber( rot.fX, 0 ); argStream.ReadNumber( rot.fY, 0 ); argStream.ReadNumber( rot.fZ, 0 );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementAttachedOffsets( *entity, pos, rot ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementCollisionsEnabled )
    {
        if ( CClientEntity *entity = lua_readclass <CClientEntity> ( L, 1, LUACLASS_ENTITY ) )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementCollisionsEnabled( *entity, lua_toboolean( L, 2 ) == 1 ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "setElementCollisionsEnabled", "element", 1 );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementCollidableWith )
    {
        CClientEntity *entity, *collideWith;
        bool enable;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadClass( collideWith, LUACLASS_ENTITY );
        argStream.ReadBool( enable, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementCollidableWith( *entity, *collideWith, enable ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementDoubleSided )
    {
        CClientEntity *entity;
        bool enable;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( enable, true );

        if ( !argStream.HasErrors() )
        {
            // Grab the chosen value and set it
            entity->SetDoubleSided( enable );
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failure
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementFrozen )
    {
        CClientEntity *entity;
        bool frozen;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadBool( frozen, true );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementFrozen( *entity, frozen ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failure
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementAlpha )
    {
        CClientEntity *entity;
        unsigned char alpha;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( alpha );

        if ( !argStream.HasErrors() )
        {
            // Grab the chosen alpha and set it
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementAlpha( *entity, alpha ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failure
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementHealth )
    {
        CClientEntity *entity;
        float health;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( health );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementHealth( *entity, health ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementModel )
    {
        CClientEntity *entity;
        unsigned short model;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_ENTITY );
        argStream.ReadNumber( model );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, CStaticFunctionDefinitions::SetElementModel( *entity, model ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setElementStreamable )
    {
        CClientStreamElement *entity;
        bool stream;

        CScriptArgReader argStream( L );

        argStream.ReadClass( entity, LUACLASS_STREAMELEMENT );
        argStream.ReadBool( stream, true );

        if ( !argStream.HasErrors() )
        {
            // TODO: maybe use a better VM-based reference system (rather than a boolean one)
            if ( stream && ( entity->GetStreamReferences ( true ) > 0 ) )
            {
                entity->RemoveStreamReference ( true );
            }
            else if ( !stream && ( entity->GetStreamReferences ( true ) == 0 ) )
            {
                entity->AddStreamReference ( true );
            }
            else
                goto fail;

            // Return success
            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

fail:
        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }
}
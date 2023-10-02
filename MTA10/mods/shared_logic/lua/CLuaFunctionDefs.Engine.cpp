/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Engine.cpp
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

#include "StdInc.h"

extern CGame *g_pGame;

namespace CLuaFunctionDefs
{
    LUA_DECLARE( engineLoadCOL )
    {
        CResource *res = lua_readcontext( L )->GetResource();

        CFile *file = m_pResourceManager->OpenStream( res, lua_isstring( L, 1 ) ? lua_tostring( L, 1 ) : "" , "rb" );

        if ( file )
        {
            // Create the col model
            CClientColModel *pCol = new CClientColModel( L );
            pCol->SetRoot( res->GetResourceCOLModelRoot() );

            // Attempt loading the file
            bool success = pCol->LoadCol( file );

            delete file;

            if ( success )
            {
                pCol->PushStack( L );
                return 1;
            }

            pCol->Destroy();
        }
        else
            m_pScriptDebugging->LogBadType( "engineLoadCOL" );

        // We failed for some reason
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineLoadDFF )
    {
        const char *path;
        unsigned short num;

        CScriptArgReader argStream( L );
        
        argStream.ReadString( path );
        argStream.ReadNumber( num );
        
        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();

            CFile *file = m_pResourceManager->OpenStream( res, path, "rb" );

            CModel *model = g_pGame->GetModelManager()->CreateModel( file, num );

            delete file;

            if ( model )
            {
                // Create a DFF element
                CClientDFF *dff = new CClientDFF( L, *model, res );
                dff->PushStack( L );
                dff->DisableKeepAlive();
                return 1;
            }
            else
                m_pScriptDebugging->LogError( "failed to load .dff file" );
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // We failed
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineLoadTXD )
    {
        const char *path;
        bool filtering;

        CScriptArgReader argStream( L );

        argStream.ReadString( path );
        argStream.ReadBool( filtering, true );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();

            CFile *file = res->OpenStream( path, "rb" );

            CTexDictionary *dict = g_pGame->GetTextureManager()->CreateTxd( file );

            delete file;

            if ( dict )
            {
                // Stupid filtering :P
                std::list <CTexture*>& list = dict->GetTextures();

                for ( std::list <CTexture*>::const_iterator iter = list.begin(); iter != list.end(); ++iter )
                    (*iter)->SetFiltering( filtering );

                CClientTXD *obj = new CClientTXD( L, *dict, res );
                obj->PushStack( L );
                obj->DisableKeepAlive();
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        // We failed
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetGameTextures )
    {
        CResource *res = lua_readcontext( L )->GetResource();
   
        lua_settop( L, 0 );
        lua_newtable( L );

        int n = 1;

        LIST_FOREACH_BEGIN( CClientGameTexture, res->m_gameTextures.root, m_ownerTex )
            item->PushStack( L );
            lua_rawseti( L, 1, n++ );
        LIST_FOREACH_END

        return 1;
    }

    LUA_DECLARE( engineModelInfoHasLoaded )
    {
        unsigned short model;

        CScriptArgReader argStream( L );
        
        argStream.ReadNumber( model );

        if ( !argStream.HasErrors() )
        {
            lua_pushboolean( L, g_pGame->GetStreaming()->HasModelLoaded( model ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineModelInfoCloneObject )
    {
        unsigned short model;

        CScriptArgReader argStream( L );
        
        argStream.ReadNumber( model );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();
            eRwType type;

            union
            {
                CModel *clump;
                CRpAtomic *atom;
            };

            if ( g_pGame->GetModelManager()->GetRwModelType( model, type ) )
            {
                switch( type )
                {
                case RW_CLUMP:
                    clump = g_pGame->GetModelManager()->CloneClump( model );

                    if ( clump )
                    {
                        CClientDFF *dff = new CClientDFF( L, *clump, res );
                        dff->PushStack( L );
                        dff->DisableKeepAlive();
                        return 1;
                    }
                    break;
                case RW_ATOMIC:
                    atom = g_pGame->GetModelManager()->CloneAtomic( model );

                    if ( atom )
                    {
                        CClientAtomic *obj = new CClientAtomic( L, NULL, *atom );
                        obj->SetOwner( res );
                        obj->PushStack( L );
                        obj->DisableKeepAlive();
                        return 1;
                    }
                    break;
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineCreateLight )
    {
        RpLightType type;

        CScriptArgReader argStream( L );
        argStream.ReadEnumString( type );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();
            CRpLight *light = g_pGame->GetRenderWare()->CreateLight( type );

            if ( light )
            {
                CClientLight *obj = new CClientLight( L, NULL, *light );
                obj->SetOwner( res );
                obj->PushStack( L );
                obj->DisableKeepAlive();
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineCreateFrame )
    {
        CResource *res = lua_readcontext( L )->GetResource();
        CRwFrame *frame = g_pGame->GetRenderWare()->CreateFrame();

        if ( !frame )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        CClientRwFrame *obj = new CClientRwFrame( L, *frame, res );
        obj->PushStack( L );
        return 1;
    }

    LUA_DECLARE( engineCreateCamera )
    {
        int width, height;

        CScriptArgReader argStream( L );

        argStream.ReadNumber( width );
        argStream.ReadNumber( height );

        if ( !argStream.HasErrors() )
        {
            CResource *res = lua_readcontext( L )->GetResource();
            CRwCamera *cam = g_pGame->GetRenderWare()->CreateCamera( width, height );

            if ( cam )
            {
                CClientRwCamera *obj = new CClientRwCamera( L, *cam );
                obj->SetOwner( res );
                obj->PushStack( L );
                obj->DisableKeepAlive();
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString( "Bad argument @ '" __FUNCTION__ "' [%s]", *argStream.GetErrorMessage() ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineCreateClump )
    {
        CResource *res = lua_readcontext( L )->GetResource();
        CModel *model = g_pGame->GetRenderWare()->CreateClump();

        if ( !model )
        {
            lua_pushboolean( L, false );
            return 1;
        }

        CClientDFF *dff = new CClientDFF( L, *model, res );
        dff->PushStack( L );
        dff->DisableKeepAlive();
        return 1;
    }

    LUA_DECLARE( engineIsRendering )
    {
        lua_pushboolean( L, g_pGame->GetRenderWare()->IsRendering() );
        return 1;
    }

    LUA_DECLARE( engineReplaceCOL )
    {
        // Grab the COL and model ID
        CClientColModel* pCol = lua_readclass <CClientColModel> ( L, 1, LUACLASS_COLMODEL );
        unsigned short usModel = lua_tointeger( L, 2 );

        if ( pCol )
        {
            // Replace the colmodel
            lua_pushboolean( L, pCol->Replace( usModel ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "engineReplaceCOL", "col", 1 );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineRestoreCOL )
    {
        if ( lua_isnumber( L, 1 ) )
        {
            lua_pushboolean( L, g_pGame->GetModelManager()->RestoreCollision( (unsigned short)lua_tonumber( L, 1 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType( "engineRestoreCOL" );

        // Failed.
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineImportTXD )
    {
        CClientTXD *txd = lua_readclass <CClientTXD> ( L, 1, LUACLASS_TXD );
        unsigned short usModelID = lua_tointeger( L, 2 );

        if ( txd )
        {
            // Try to import
            if ( txd->Import( usModelID ) )
            {
                // Success
                lua_pushboolean( L, true );
                return 1;
            }
            else
                m_pScriptDebugging->LogBadPointer( "engineImportTXD", "number", 2 );
        }
        else
            m_pScriptDebugging->LogBadPointer( "engineImportTXD", "txd", 1 );

        // Failed
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineReplaceModel )
    {
        CClientDFF *dff = lua_readclass <CClientDFF> ( L, 1, LUACLASS_DFF );

        if ( dff )
        {
            lua_pushboolean( L, dff->ReplaceModel( lua_tointeger( L, 2 ) ) );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer( "engineReplaceModel", "dff", 1 );

        // Failure
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineRestoreModel )
    {
        // Restore the model
        if ( g_pGame->GetModelManager()->RestoreModel( lua_tointeger( L, 1 ) ) )
        {
            // Success
            lua_pushboolean( L, true );
            return 1;
        }

        m_pScriptDebugging->LogBadType( "engineRestoreModel" );

        // Failure
        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineSetModelLODDistance )
    {
        if ( lua_isnumber( L, 1 ) && lua_isnumber( L, 2 ) )
        {
            unsigned short usModelID = (unsigned short)lua_tonumber( L, 1 );
            float fDistance = (float)lua_tonumber( L, 2 );

            CModelInfo *pModelInfo = g_pGame->GetModelInfo( usModelID );

            if ( pModelInfo && fDistance > 0.0f )
            {
                pModelInfo->SetLODDistance( fDistance );
                lua_pushboolean( L, true );
                return 1;
            }
        }

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineSetEnvMapRenderingEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetRenderWare()->EnableEnvMapRendering( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsEnvMapRenderingEnabled )
    {
        lua_pushboolean( L, g_pGame->GetRenderWare()->IsEnvMapRenderingEnabled() );
        return 1;
    }

    LUA_DECLARE( engineSetAsynchronousLoading )
    {
        if ( lua_type( L, 1 ) == LUA_TBOOLEAN )
        {
            bool bEnabled = lua_toboolean( L, 1 ) == 1;
            bool bForced = lua_toboolean( L, 2 ) == 1;

            g_pGame->SetAsyncLoadingFromScript( bEnabled, bForced );
            lua_pushboolean( L, true );
            return 1;
        }

        lua_pushboolean( L, false );
        return 1;
    }

    // TODO: int CLuaFunctionDefs::EnginePositionAtomic ( lua_State* L )
    LUA_DECLARE( enginePositionAtomic )
    {
        lua_pushboolean ( L, false );
        return 1;
    }

    // TODO: int CLuaFunctionDefs::EnginePositionSeats ( lua_State* L )
    LUA_DECLARE( enginePositionSeats )
    {
        lua_pushboolean ( L, false );
        return 1;
    }

    // TODO: int CLuaFunctionDefs::EngineAddAllAtomics ( lua_State* L )
    LUA_DECLARE( engineAddAllAtomics )
    {
        lua_pushboolean ( L, false );
        return 1;
    }

    // TODO: int CLuaFunctionDefs::EngineReplaceVehiclePart ( lua_State* L )
    LUA_DECLARE( engineReplaceVehiclePart )
    {
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineApplyShaderToWorldTexture )
    {
    //  bool engineApplyShaderToWorldTexture ( element shader, string textureName, float orderPriority )
        CClientShader* pShader; SString strTextureNameMatch; float fOrderPriority;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pShader, LUACLASS_SHADER );
        argStream.ReadString ( strTextureNameMatch );
        argStream.ReadNumber ( fOrderPriority, 0 );

        if ( !argStream.HasErrors () )
        {
            bool bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->ApplyShaderItemToWorldTexture ( pShader->GetShaderItem (), strTextureNameMatch, NULL );
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineApplyShaderToWorldTexture", *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineRemoveShaderFromWorldTexture )
    {
    //  bool engineRemoveShaderFromWorldTexture ( element shader, string textureName )
        CClientShader* pShader; SString strTextureNameMatch;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pShader, LUACLASS_SHADER );
        argStream.ReadString ( strTextureNameMatch );

        if ( !argStream.HasErrors () )
        {
            bool bResult = g_pCore->GetGraphics ()->GetRenderItemManager ()->RemoveShaderItemFromWorldTexture ( pShader->GetShaderItem (), strTextureNameMatch, NULL );
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineRemoveShaderFromWorldTexture", *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetModelNameFromID )
    {
    //  string engineGetModelNameFromID ( int modelID )
        int iModelID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iModelID );

        if ( !argStream.HasErrors () )
        {
            SString strModelName = CModelNames::GetModelName ( iModelID );
            if ( !strModelName.empty () )
            {
                lua_pushstring ( L, strModelName );
                return 1;
            }
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineGetModelNameFromID", "Expected valid model ID at argument 1" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineGetModelNameFromID", *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetModelIDFromName )
    {
        //  int engineGetModelIDFromName ( string modelName )
        SString strModelName;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strModelName );

        if ( !argStream.HasErrors () )
        {
            int iModelID = CModelNames::GetModelID ( strModelName );
            if ( iModelID )
            {
                lua_pushnumber ( L, iModelID );
                return 1;
            }
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineGetModelIDFromName", "Expected valid model name at argument 1" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "engineGetModelIDFromName", *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetModelTextureNames )
    {
    //  table engineGetModelTextureNames ( string modelName )
        SString strModelName;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strModelName, "" );

        if ( !argStream.HasErrors () )
        {
            ushort usModelID = CModelNames::ResolveModelID ( strModelName );
            if ( usModelID )
            {
                std::vector < SString > nameList;
                g_pGame->GetRenderWare()->GetModelTextureNames ( nameList, usModelID );

                lua_newtable ( L );
                for ( uint i = 0 ; i < nameList.size () ; i++ )
                {                
                    lua_pushnumber ( L, i + 1 );
                    lua_pushstring ( L, nameList [ i ] );
                    lua_settable ( L, -3 );
                }
                return 1;
            }
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, "Expected valid model ID or name at argument 1" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetVisibleTextureNames )
    {
    //  table engineGetVisibleTextureNames ( string wildcardMatch = "*" [, string modelName )
        SString strTextureNameMatch; SString strModelName;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strTextureNameMatch, "*" );
        argStream.ReadString ( strModelName, "" );

        if ( !argStream.HasErrors () )
        {
            ushort usModelID = CModelNames::ResolveModelID ( strModelName );
            if ( usModelID || strModelName == "" )
            {
                std::vector < SString > nameList;
                g_pCore->GetGraphics ()->GetRenderItemManager ()->GetVisibleTextureNames ( nameList, strTextureNameMatch, usModelID );

                lua_newtable ( L );
                for ( uint i = 0 ; i < nameList.size () ; i++ )
                {                
                    lua_pushnumber ( L, i + 1 );
                    lua_pushstring ( L, nameList [ i ] );
                    lua_settable ( L, -3 );
                }
                return 1;
            }
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, "Expected valid model ID or name at argument 1" ) );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        // We failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( engineSetInfiniteStreamingEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetInfiniteStreamingEnabled( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsInfiniteStreamingEnabled )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsInfiniteStreamingEnabled() );
        return 1;
    }

    LUA_DECLARE( engineSetStrictStreamingNodeDistributionEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetStrictNodeDistribution( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsStrictStreamingNodeDistributionEnabled )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsStrictNodeDistributionEnabled() );
        return 1;
    }

    LUA_DECLARE( engineSetStreamingGCOnDemandEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetGarbageCollectOnDemand( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsStreamingGCOnDemandEnabled )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsGarbageCollectOnDemandEnabled() );
        return 1;
    }

    LUA_DECLARE( engineAllowStreamingNodeStealing )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetStreamingNodeStealingAllowed( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsStreamingNodeStealingAllowed )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsStreamingNodeStealingAllowed() );
        return 1;
    }

    LUA_DECLARE( engineGetActiveStreamingEntityCount )
    {
        lua_pushnumber( L, g_pGame->GetStreaming()->GetActiveStreamingEntityCount() );
        return 1;
    }

    LUA_DECLARE( engineGetActiveStreamingFreeSlotCount )
    {
        lua_pushnumber( L, g_pGame->GetStreaming()->GetFreeStreamingEntitySlotCount() );
        return 1;
    }

    inline void CreateLuaTableOfEntityList( lua_State *L, CStreaming::entityList_t& list )
    {
        lua_newtable( L );
        
        unsigned int n = 0;

        for ( CStreaming::entityList_t::iterator iter = list.begin(); iter != list.end(); ++iter )
        {
            CEntity *gameEntity = *iter;

            if ( CClientEntity *clientEntity = m_pManager->FindEntity( gameEntity ) )
            {
                clientEntity->PushStack( L );
                lua_rawseti( L, -2, ++n );
            }
        }
    }

    LUA_DECLARE( engineGetActiveStreamingEntities )
    {
        // Get a table of all entities that are insideof the streaming garbage collector.
        CreateLuaTableOfEntityList( L, g_pGame->GetStreaming()->GetActiveStreamingEntities() );
        return 1;
    }

    LUA_DECLARE( engineGetStreamingFocus )
    {
        // Obtain the entity that the world is loading around.
        CEntity *streamingFocus = g_pGame->GetStreaming()->GetStreamingFocusEntity();

        LUA_CHECK( streamingFocus );

        CClientEntity *clientEntity = g_pClientGame->GetManager()->FindEntity( streamingFocus );

        LUA_CHECK( clientEntity );

        clientEntity->PushStack( L );
        return 1;
    }

    LUA_DECLARE( engineStreamingIsElementManaged )
    {
        // Returns whether the entity is inside of the streaming garbage collection system.
        // Can be used for debugging and streaming error detection.
        CClientEntity *clientEntity;

        CScriptArgReader argStream( L );
        argStream.ReadClass( clientEntity, LUACLASS_ENTITY );

        if ( !argStream.HasErrors() )
        {
            CEntity *gameEntity = clientEntity->GetGameEntity();

            if ( gameEntity )
            {
                lua_pushboolean( L, g_pGame->GetStreaming()->IsEntityGCManaged( gameEntity ) );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetModelRefCount )
    {
        modelId_t modelIndex;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( modelIndex );

        if ( !argStream.HasErrors() )
        {
            CModelInfo *info = g_pGame->GetModelInfo( modelIndex );

            if ( info )
            {
                lua_pushnumber( L, info->GetRefCount() );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineSetWorldStreamingEnabled )
    {
        // Switch world rendering on or off.
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetWorldStreamingEnabled( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsWorldStreamingEnabled )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsWorldStreamingEnabled() );
        return 1;
    }

    LUA_DECLARE( engineGetEntitiesInRenderQueue )
    {
        CreateLuaTableOfEntityList( L, g_pGame->GetEntitiesInRenderQueue() );
        return 1;
    }

    LUA_DECLARE( engineGetGamePoolLimits )
    {
        lua_newtable( L );

        CPools *pools = g_pGame->GetPools();

        for ( unsigned int n = 0; n < MAX_POOLS; n++ )
        {
            lua_newtable( L );

            lua_pushstring( L, pools->GetPoolName( (ePools)n ) );
            lua_setfield( L, -2, "name" );

            lua_pushnumber( L, pools->GetNumberOfUsedSpaces( (ePools)n ) );
            lua_setfield( L, -2, "usedCount" );

            lua_pushnumber( L, pools->GetPoolCapacity( (ePools)n ) );
            lua_setfield( L, -2, "maxCount" );

            lua_rawseti( L, -2, n + 1 );
        }

        return 1;
    }

    LUA_DECLARE( engineGetStreamingInfo )
    {
        lua_newtable( L );

        CStreaming::streamingInfo info;

        g_pGame->GetStreaming()->GetStreamingInfo( info );

        lua_pushnumber( L, info.usedMemory );
        lua_setfield( L, -2, "usedMemory" );

        lua_pushnumber( L, info.maxMemory );
        lua_setfield( L, -2, "maxMemory" );

        lua_pushnumber( L, info.numberOfRequests );
        lua_setfield( L, -2, "numberOfRequests" );

        lua_pushnumber( L, info.numberOfPriorityRequests );
        lua_setfield( L, -2, "numberOfPriorityRequests" );

        lua_pushnumber( L, info.numberOfSlicers );
        lua_setfield( L, -2, "numberOfSlicers" );

        lua_pushnumber( L, info.numberOfRequestsPerSlicer );
        lua_setfield( L, -2, "numberOfRequestsPerSlicer" );

        lua_pushnumber( L, info.activeStreamingThread );
        lua_setfield( L, -2, "activeStreamingThread" );

        lua_pushboolean( L, info.isBusy );
        lua_setfield( L, -2, "isBusy" );

        lua_pushboolean( L, info.isLoadingBigModel );
        lua_setfield( L, -2, "isLoadingBigModel" );

        return 1;
    }

    LUA_DECLARE( engineStreamingEnableFiberedLoading )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->EnableFiberedLoading( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineStreamingIsFiberedLoadingEnabled )
    {
        lua_pushboolean( L, g_pGame->GetStreaming()->IsFiberedLoadingEnabled() );
        return 1;
    }

    LUA_DECLARE( engineStreamingSetFiberedPerfMultiplier )
    {
        double mult;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( mult );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetStreaming()->SetFiberedPerfMultiplier( mult );
            
            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineStreamingGetFiberedPerfMultiplier )
    {
        lua_pushnumber( L, g_pGame->GetStreaming()->GetFiberedPerfMultiplier() );
        return 1;
    }

    LUA_DECLARE( engineSetGlobalLightingAlwaysEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetRenderWare()->SetGlobalLightingAlwaysEnabled( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsGlobalLightingAlwaysEnabled )
    {
        lua_pushboolean( L, g_pGame->GetRenderWare()->IsGlobalLightingAlwaysEnabled() );
        return 1;
    }

    LUA_DECLARE( engineSetLocalLightingAlwaysEnabled )
    {
        bool enabled;

        CScriptArgReader argStream( L );
        argStream.ReadBool( enabled );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetRenderWare()->SetLocalLightingAlwaysEnabled( enabled );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineIsLocalLightingAlwaysEnabled )
    {
        lua_pushboolean( L, g_pGame->GetRenderWare()->IsLocalLightingAlwaysEnabled() );
        return 1;
    }

    LUA_DECLARE( engineSetWorldRenderMode )
    {
        eWorldRenderMode renderMode;

        CScriptArgReader argStream( L );
        argStream.ReadEnumString( renderMode );

        if ( !argStream.HasErrors() )
        {
            g_pGame->GetRenderWare()->SetWorldRenderMode( renderMode );

            lua_pushboolean( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetWorldRenderMode )
    {
        eWorldRenderMode renderMode = g_pGame->GetRenderWare()->GetWorldRenderMode();

        SString modeString = EnumToString( renderMode );

        lua_pushlstring( L, modeString.c_str(), modeString.size() );
        return 1;
    }

    LUA_DECLARE( engineIsModelBeingUsed )
    {
        int modelIndex;

        CScriptArgReader argStream( L );
        argStream.ReadNumber( modelIndex );

        if ( !argStream.HasErrors() )
        {
            if ( modelIndex >= 0 )
            {
                CModelInfo *modelInfo = g_pGame->GetModelInfo( modelIndex );

                if ( modelInfo )
                {
                    lua_pushboolean( L, modelInfo->GetRefCount() > 0 );
                    return 1;
                }
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", __FUNCTION__, *argStream.GetErrorMessage () ) );

        lua_pushboolean( L, false );
        return 1;
    }

    LUA_DECLARE( engineGetRenderBucketStatistics )
    {
        renderBucketStats currentRenderStats;

        g_pGame->GetRenderWare()->GetRenderBucketStatistics( currentRenderStats );

        lua_newtable( L );

        lua_pushnumber( L, currentRenderStats.totalNumberOfRenderCalls );
        lua_setfield( L, -2, "totalNumberOfRenderCalls" );

        lua_pushnumber( L, currentRenderStats.totalNumberOfActiveBuckets );
        lua_setfield( L, -2, "totalNumberOfActiveBuckets" );

        lua_pushnumber( L, currentRenderStats.maxRenderCallsPerBucket );
        lua_setfield( L, -2, "maxRenderCallsPerBucket" );

        lua_pushnumber( L, currentRenderStats.bucketCacheMissCount );
        lua_setfield( L, -2, "bucketCacheMissCount" );

        lua_pushnumber( L, currentRenderStats.bucketAllocationCount );
        lua_setfield( L, -2, "bucketAllocationCount" );

        lua_pushnumber( L, currentRenderStats.bucketAdaptionCount );
        lua_setfield( L, -2, "bucketAdaptionCount" );

        // Conflict counts.
        lua_pushnumber( L, currentRenderStats.cacheNumVertexStreamConflicts );
        lua_setfield( L, -2, "cacheNumVertexStreamConflicts" );

        lua_pushnumber( L, currentRenderStats.cacheNumRenderStateConflicts );
        lua_setfield( L, -2, "cacheNumRenderStateConflicts" );

        lua_pushnumber( L, currentRenderStats.cacheNumTextureStageStateConflicts );
        lua_setfield( L, -2, "cacheNumTextureStageStateConflicts" );

        lua_pushnumber( L, currentRenderStats.cacheNumLightingStateConflicts );
        lua_setfield( L, -2, "cacheNumLightingStateConflicts" );

        lua_pushnumber( L, currentRenderStats.cacheNumTransformationStateConflicts );
        lua_setfield( L, -2, "cacheNumTransformationStateConflicts" );

        lua_pushnumber( L, currentRenderStats.cacheNumSamplerStateConflicts );
        lua_setfield( L, -2, "cacheNumSamplerStateConflicts" );

        lua_pushnumber( L, currentRenderStats.uniqueBucketUsageCount );
        lua_setfield( L, -2, "uniqueBucketUsageCount" );

        lua_pushnumber( L, currentRenderStats.bucketTerminationCount );
        lua_setfield( L, -2, "bucketTerminationCount" );

        return 1;
    }
}
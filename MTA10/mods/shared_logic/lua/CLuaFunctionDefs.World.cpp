/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.World.cpp
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
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

namespace CLuaFunctionDefs
{
    LUA_DECLARE( createExplosion )
    {
    //  bool createExplosion ( float x, float y, float z, int type [, bool makeSound = true, float camShake = -1.0, bool damaging = true ] )
        CVector vecPosition; int iType; bool bMakeSound; float fCamShake = -1.0; bool bDamaging;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecPosition.fX );
        argStream.ReadNumber ( vecPosition.fY );
        argStream.ReadNumber ( vecPosition.fZ );
        argStream.ReadNumber ( iType );
        argStream.ReadBool ( bMakeSound, true );
        argStream.ReadNumber ( fCamShake, -1.0f );
        argStream.ReadBool ( bDamaging, true );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, iType, bMakeSound, fCamShake, bDamaging ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "createExplosion", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createFire )
    {
    //  bool createFire ( float x, float y, float z [, float size = 1.8 ] )
        CVector vecPosition; float fSize;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecPosition.fX );
        argStream.ReadNumber ( vecPosition.fY );
        argStream.ReadNumber ( vecPosition.fZ );
        argStream.ReadNumber ( fSize, 1.8f );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "createFire", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTime )
    {
        // Get the time
        unsigned char ucHour, ucMinute;
        if ( CStaticFunctionDefinitions::GetTime ( ucHour, ucMinute ) )
        {
            // Return it
            lua_pushnumber ( L, static_cast < lua_Number > ( ucHour ) );
            lua_pushnumber ( L, static_cast < lua_Number > ( ucMinute ) );
            return 2;
        }

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGroundPosition )
    {
    //  float getGroundPosition ( float x, float y, float z )
        CVector vecStart;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecStart.fX );
        argStream.ReadNumber ( vecStart.fY );
        argStream.ReadNumber ( vecStart.fZ );

        if ( !argStream.HasErrors () )
        {
            // Get the ground position and return it
            float fGround = g_pGame->GetWorld ()->FindGroundZFor3DPosition ( &vecStart );
            lua_pushnumber ( L, fGround );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getGroundPosition", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( processLineOfSight )
    {
    //  bool float float float element float float float int int int processLineOfSight ( float startX, float startY, float startZ, float endX, float endY, float endZ,
    //      [ bool checkBuildings = true, bool checkVehicles = true, bool checkPlayers = true, bool checkObjects = true, bool checkDummies = true,
    //        bool seeThroughStuff = false, bool ignoreSomeObjectsForCamera = false, bool shootThroughStuff = false, element ignoredElement = nil, bool returnBuildingInfo = false ] )
        CVector vecStart; CVector vecEnd;
        SLineOfSightFlags flags; CClientEntity* pIgnoredEntity; bool bIncludeBuildingInfo;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecStart.fX );
        argStream.ReadNumber ( vecStart.fY );
        argStream.ReadNumber ( vecStart.fZ );
        argStream.ReadNumber ( vecEnd.fX );
        argStream.ReadNumber ( vecEnd.fY );
        argStream.ReadNumber ( vecEnd.fZ );
        argStream.ReadBool ( flags.bCheckBuildings, true );
        argStream.ReadBool ( flags.bCheckVehicles, true );
        argStream.ReadBool ( flags.bCheckPeds, true );
        argStream.ReadBool ( flags.bCheckObjects, true );
        argStream.ReadBool ( flags.bCheckDummies, true );
        argStream.ReadBool ( flags.bSeeThroughStuff, false );
        argStream.ReadBool ( flags.bIgnoreSomeObjectsForCamera, false );
        argStream.ReadBool ( flags.bShootThroughStuff, false );
        argStream.ReadClass( pIgnoredEntity, LUACLASS_ENTITY, NULL );
        argStream.ReadBool ( bIncludeBuildingInfo, false );

        if ( !argStream.HasErrors () )
        {
            CColPoint* pColPoint = NULL;
            CClientEntity* pColEntity = NULL;
            CEntity *ignoredGameEntity = ( pIgnoredEntity ) ? pIgnoredEntity->GetGameEntity() : NULL;
            bool bCollision;
            SLineOfSightBuildingResult buildingResult;
            if ( CStaticFunctionDefinitions::ProcessLineOfSight ( vecStart, vecEnd, bCollision, &pColPoint, &pColEntity, flags, ignoredGameEntity, bIncludeBuildingInfo ? &buildingResult : NULL ) )
            {    
                // Got a collision?
                CVector vecColPosition;
                CVector vecColNormal;

                int iMaterial = -1;
                int iLighting = -1;
                int iPiece = -1;

                if ( pColPoint )
                {
                    // Get the collision position
                    vecColPosition = *pColPoint->GetPosition ();
                    vecColNormal = *pColPoint->GetNormal();
                    iMaterial = pColPoint->GetSurfaceTypeB(); //From test, only B function return relevant data
                    iLighting = pColPoint->GetLightingB();
                    iPiece = pColPoint->GetPieceTypeB();

                    // Delete the colpoint
                    pColPoint->Destroy ();
                }

                lua_pushboolean ( L, bCollision );
                if ( bCollision )
                {
                    lua_pushnumber ( L, vecColPosition.fX );
                    lua_pushnumber ( L, vecColPosition.fY );
                    lua_pushnumber ( L, vecColPosition.fZ );

                    if ( pColEntity )
                        pColEntity->PushStack( L );
                    else
                        lua_pushnil ( L );

                    lua_pushnumber ( L, vecColNormal.fX );
                    lua_pushnumber ( L, vecColNormal.fY );
                    lua_pushnumber ( L, vecColNormal.fZ );

                    lua_pushinteger ( L, iMaterial );
                    lua_pushinteger ( L, iLighting );
                    lua_pushinteger ( L, iPiece );

                    if ( bIncludeBuildingInfo && buildingResult.bValid )
                    {
                        lua_pushnumber ( L, buildingResult.usModelID );

                        lua_pushnumber ( L, buildingResult.vecPosition.fX );
                        lua_pushnumber ( L, buildingResult.vecPosition.fY );
                        lua_pushnumber ( L, buildingResult.vecPosition.fZ );

                        lua_pushnumber ( L, ConvertRadiansToDegrees ( buildingResult.vecRotation.fX ) );
                        lua_pushnumber ( L, ConvertRadiansToDegrees ( buildingResult.vecRotation.fY ) );
                        lua_pushnumber ( L, ConvertRadiansToDegrees ( buildingResult.vecRotation.fZ ) );
                        return 18;
                    }
                    return 11;
                }
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "processLineOfSight", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isLineOfSightClear )
    {
        //bool isLineOfSightClear ( float startX, 
        //    float startY, 
        //    float startZ, 
        //    float endX, 
        //    float endY, 
        //    float endZ, 
        //    [ bool checkBuildings = true, 
        //    bool checkVehicles = true, 
        //    bool checkPeds = true, 
        //    bool checkObjects = true, 
        //    bool checkDummies = true, 
        //    bool seeThroughStuff = false, 
        //    bool ignoreSomeObjectsForCamera = false, 
        //    element ignoredElement = nil ] )
        CVector vecStart; CVector vecEnd;
        SLineOfSightFlags flags; CEntity* pIgnoredEntity;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecStart.fX );
        argStream.ReadNumber ( vecStart.fY );
        argStream.ReadNumber ( vecStart.fZ );
        argStream.ReadNumber ( vecEnd.fX );
        argStream.ReadNumber ( vecEnd.fY );
        argStream.ReadNumber ( vecEnd.fZ );
        argStream.ReadBool ( flags.bCheckBuildings, true );
        argStream.ReadBool ( flags.bCheckVehicles, true );
        argStream.ReadBool ( flags.bCheckPeds, true );
        argStream.ReadBool ( flags.bCheckObjects, true );
        argStream.ReadBool ( flags.bCheckDummies, true );
        argStream.ReadBool ( flags.bSeeThroughStuff, false );
        argStream.ReadBool ( flags.bIgnoreSomeObjectsForCamera, false );
        argStream.ReadClass( pIgnoredEntity, LUACLASS_ENTITY, NULL );

        if ( !argStream.HasErrors () )
        {
            bool bIsClear;
            if ( CStaticFunctionDefinitions::IsLineOfSightClear ( vecStart, vecEnd, bIsClear, flags, pIgnoredEntity ) )
            {        
                lua_pushboolean ( L, bIsClear );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isLineOfSightClear", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( testLineAgainstWater )
    {
    //  bool float float float testLineAgainstWater ( float startX, float startY, float startZ, float endX, float endY, float endZ )
        CVector vecStart; CVector vecEnd;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecStart.fX );
        argStream.ReadNumber ( vecStart.fY );
        argStream.ReadNumber ( vecStart.fZ );
        argStream.ReadNumber ( vecEnd.fX );
        argStream.ReadNumber ( vecEnd.fY );
        argStream.ReadNumber ( vecEnd.fZ );

        if ( !argStream.HasErrors () )
        {
            CVector vecCollision;
            if ( CStaticFunctionDefinitions::TestLineAgainstWater ( vecStart, vecEnd, vecCollision ) )
            {
                lua_pushboolean ( L, true );
                lua_pushnumber ( L, vecCollision.fX );
                lua_pushnumber ( L, vecCollision.fY );
                lua_pushnumber ( L, vecCollision.fZ );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "testLineAgainstWater", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( createWater )
    {
    //  water createWater ( float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3 [, float x4, float y4, float z4 ] )
        CVector v1; CVector v2; CVector v3; CVector v4; bool bShallow;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( v1.fX );
        argStream.ReadNumber ( v1.fY );
        argStream.ReadNumber ( v1.fZ );
        argStream.ReadNumber ( v2.fX );
        argStream.ReadNumber ( v2.fY );
        argStream.ReadNumber ( v2.fZ );
        argStream.ReadNumber ( v3.fX );
        argStream.ReadNumber ( v3.fY );
        argStream.ReadNumber ( v3.fZ );
        bool bIsQuad = argStream.NextCouldBeNumber ( 2 );   // Check for existence of v4.fZ
        if ( bIsQuad )
        {
            argStream.ReadNumber ( v4.fX );
            argStream.ReadNumber ( v4.fY );
            argStream.ReadNumber ( v4.fZ );
        }
        argStream.ReadBool ( bShallow, false );

        if ( !argStream.HasErrors () )
        {
            CLuaMain* pLuaMain = lua_readcontext( L );
            CResource* pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
            if ( pResource )
            {
                CClientWater* pWaterElement;

                if ( bIsQuad )
                    pWaterElement = CStaticFunctionDefinitions::CreateWater ( *pResource, &v1, &v2, &v3, &v4, bShallow );
                else
                    pWaterElement = CStaticFunctionDefinitions::CreateWater ( *pResource, &v1, &v2, &v3, NULL, bShallow );

                pWaterElement->PushStack( L );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "createWater", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWaterLevel )
    {
        CScriptArgReader argStream ( L );
        if ( !argStream.NextIsClass() )
        {
            // Call type 1
            //  float getWaterLevel ( float posX, float posY, float posZ [ , bool bCheckWaves = false ] )
            CVector vecPosition; bool bCheckWaves;

            argStream.ReadNumber ( vecPosition.fX );
            argStream.ReadNumber ( vecPosition.fY );
            argStream.ReadNumber ( vecPosition.fZ );
            argStream.ReadBool ( bCheckWaves, false );

            if ( !argStream.HasErrors () )
            {
                float fWaterLevel;
                CVector vecUnknown;
                if ( CStaticFunctionDefinitions::GetWaterLevel ( vecPosition, fWaterLevel, bCheckWaves, vecUnknown ) )
                {
                    lua_pushnumber ( L, fWaterLevel );
                    return 1;
                }
            }
        }
        else
        {
            // Call type 2
            //  float getWaterLevel ( water theWater )
            CClientWater* pWater;

            argStream.ReadClass( pWater, LUACLASS_WATER );

            if ( !argStream.HasErrors () )
            {
                float fLevel;
                if ( CStaticFunctionDefinitions::GetWaterLevel ( pWater, fLevel ) )
                {
                    lua_pushnumber ( L, fLevel );
                    return 1;
                }
            }
        }

        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getWaterLevel", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWaterVertexPosition )
    {
    //  float float float getWaterVertexPosition ( water theWater, int vertexIndex )
        CClientWater* pWater; int iVertexIndex;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pWater, LUACLASS_WATER );
        argStream.ReadNumber ( iVertexIndex);

        if ( !argStream.HasErrors () )
        {
            CVector vecPosition;
            if ( CStaticFunctionDefinitions::GetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
            {
                lua_pushnumber ( L, vecPosition.fX );
                lua_pushnumber ( L, vecPosition.fY );
                lua_pushnumber ( L, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getWaterVertexPosition", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWaterLevel )
    {
        CLuaMain* pLuaMain = lua_readcontext( L );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( !pResource )
        {
            lua_pushboolean ( L, false );
            return 1;
        }

        CScriptArgReader argStream ( L );
        if ( argStream.NextIsClass() )
        {
            // Call type 1
            //  bool setWaterLevel ( [water theWater,] float level )
            CClientWater* pWater; float fLevel;

            argStream.ReadClass( pWater, LUACLASS_WATER );
            argStream.ReadNumber ( fLevel );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetWaterLevel ( pWater, fLevel, pResource ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }
        else if ( argStream.NextCouldBeNumber ( 1 ) )
        {
            // Call type 2
            //  bool setWaterLevel ( [float x, float y, float z,] float level )
            CVector vecPosition; float fLevel;

            argStream.ReadNumber ( vecPosition.fX );
            argStream.ReadNumber ( vecPosition.fY );
            argStream.ReadNumber ( vecPosition.fZ );
            argStream.ReadNumber ( fLevel );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetWaterLevel ( &vecPosition, fLevel, pResource ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }
        else
        {
            // Call type 3
            //  bool setWaterLevel ( float level )
            float fLevel;

            argStream.ReadNumber ( fLevel );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetWaterLevel ( (CVector *)NULL, fLevel, pResource ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }

        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWaterLevel", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWaterVertexPosition )
    {
    //  bool setWaterVertexPosition ( water theWater, int vertexIndex, float x, float y, float z )
        CClientWater* pWater; int iVertexIndex; CVector vecPosition;

        CScriptArgReader argStream ( L );
        argStream.ReadClass( pWater, LUACLASS_WATER );
        argStream.ReadNumber ( iVertexIndex);
        argStream.ReadNumber ( vecPosition.fX );
        argStream.ReadNumber ( vecPosition.fY );
        argStream.ReadNumber ( vecPosition.fZ );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWaterVertexPosition ( pWater, iVertexIndex, vecPosition ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWaterVertexPosition", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWorldFromScreenPosition )
    {
    //  float, float, float getWorldFromScreenPosition ( float x, float y, float depth )
        CVector vecScreen;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecScreen.fX );
        argStream.ReadNumber ( vecScreen.fY );
        argStream.ReadNumber ( vecScreen.fZ );

        if ( !argStream.HasErrors () )
        {
            CVector vecWorld;
            if ( CStaticFunctionDefinitions::GetWorldFromScreenPosition ( vecScreen, vecWorld ) )
            {
                lua_pushnumber ( L, vecWorld.fX );
                lua_pushnumber ( L, vecWorld.fY );
                lua_pushnumber ( L, vecWorld.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getWorldFromScreenPosition", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getScreenFromWorldPosition )
    {
    //  float float getScreenFromWorldPosition ( float x, float y, float z, [ float edgeTolerance=0, bool relative=true ] )
        CVector vecWorld; float fEdgeTolerance; bool bRelative;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecWorld.fX );
        argStream.ReadNumber ( vecWorld.fY );
        argStream.ReadNumber ( vecWorld.fZ );
        argStream.ReadNumber ( fEdgeTolerance, 0 );
        argStream.ReadBool ( bRelative, true );

        if ( !argStream.HasErrors () )
        {
            CVector vecScreen;
            if ( CStaticFunctionDefinitions::GetScreenFromWorldPosition ( vecWorld, vecScreen, fEdgeTolerance, bRelative ) )
            {
                lua_pushnumber ( L, vecScreen.fX );
                lua_pushnumber ( L, vecScreen.fY );
                lua_pushnumber ( L, vecScreen.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getScreenFromWorldPosition", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWeather )
    {
        unsigned char ucWeather, ucWeatherBlendingTo;
        if ( CStaticFunctionDefinitions::GetWeather ( ucWeather, ucWeatherBlendingTo ) )
        {
            lua_pushnumber ( L, static_cast < lua_Number > ( ucWeather ) );

            if ( ucWeatherBlendingTo != 0xFF )
                lua_pushnumber ( L, static_cast < lua_Number > ( ucWeatherBlendingTo ) );
            else
                lua_pushnil ( L );

            return 2;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getZoneName )
    {
    //  string getZoneName ( float x, float y, float z, [bool citiesonly=false] )
        CVector vecPosition; bool bCitiesOnly;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( vecPosition.fX );
        argStream.ReadNumber ( vecPosition.fY );
        argStream.ReadNumber ( vecPosition.fZ );
        argStream.ReadBool ( bCitiesOnly, true );

        if ( !argStream.HasErrors () )
        {
            char szZoneName [ 128 ];
            if ( CStaticFunctionDefinitions::GetZoneName ( vecPosition, szZoneName, 128, bCitiesOnly ) )
            {
                lua_pushstring ( L, szZoneName );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getZoneName", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGravity )
    {
        float fGravity;
        if ( CStaticFunctionDefinitions::GetGravity ( fGravity ) )
        {
            lua_pushnumber ( L, fGravity );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGameSpeed )
    {
        float fSpeed;
        if ( CStaticFunctionDefinitions::GetGameSpeed ( fSpeed ) )
        {
            lua_pushnumber ( L, fSpeed );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getMinuteDuration )
    {
        unsigned long ulDelay;
        if ( CStaticFunctionDefinitions::GetMinuteDuration ( ulDelay ) )
        {
            lua_pushnumber ( L, ulDelay );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWaveHeight )
    {
        float fHeight;
        if ( CStaticFunctionDefinitions::GetWaveHeight ( fHeight ) )
        {
            lua_pushnumber ( L, fHeight );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isGarageOpen )
    {
    //  bool isGarageOpen ( int garageID )
        int iGarageID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iGarageID );

        if ( !argStream.HasErrors () )
        {
            bool bIsOpen;

            if ( CStaticFunctionDefinitions::IsGarageOpen ( iGarageID, bIsOpen ) )
            {
                lua_pushboolean ( L, bIsOpen );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isGarageOpen", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGaragePosition )
    {
    //  float, float, float getGaragePosition ( int garageID )
        int iGarageID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iGarageID );

        if ( !argStream.HasErrors () )
        {
            CVector vecPosition;

            if ( CStaticFunctionDefinitions::GetGaragePosition ( iGarageID, vecPosition ) )
            {
                lua_pushnumber ( L, vecPosition.fX );
                lua_pushnumber ( L, vecPosition.fY );
                lua_pushnumber ( L, vecPosition.fZ );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getGaragePosition", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGarageSize )
    {
    //  float, float, float getGarageSize ( int garageID )
        int iGarageID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iGarageID );

        if ( !argStream.HasErrors () )
        {
            float fDepth;
            float fWidth;
            float fHeight;

            if ( CStaticFunctionDefinitions::GetGarageSize ( iGarageID, fHeight, fWidth, fDepth ) )
            {
                lua_pushnumber ( L, fHeight );
                lua_pushnumber ( L, fWidth );
                lua_pushnumber ( L, fDepth );
                return 3;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getGarageSize", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getGarageBoundingBox )
    {
    //  float, float, float, float getGarageBoundingBox ( int garageID )
        int iGarageID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iGarageID );

        if ( !argStream.HasErrors () )
        {
            float fLeft;
            float fRight;
            float fFront;
            float fBack;

            if ( CStaticFunctionDefinitions::GetGarageBoundingBox ( iGarageID, fLeft, fRight, fFront, fBack ) )
            {
                lua_pushnumber ( L, fLeft );
                lua_pushnumber ( L, fRight );
                lua_pushnumber ( L, fFront );
                lua_pushnumber ( L, fBack );
                return 4;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "getGarageBoundingBox", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getTrafficLightState )
    {
        lua_pushnumber ( L, g_pMultiplayer->GetTrafficLightState () );
        return 1;
    }

    LUA_DECLARE( areTrafficLightsLocked )
    {
        lua_pushboolean ( L, g_pMultiplayer->GetTrafficLightsLocked () );
        return 1;
    }

    LUA_DECLARE( getBlurLevel )
    {
        lua_pushnumber ( L, g_pGame->GetBlurLevel () );
        return 1;
    }

    LUA_DECLARE( setBlurLevel )
    {
    //  bool setBlurLevel ( int level )
        int iLevel;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iLevel );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetBlurLevel ( static_cast < unsigned char > ( iLevel ) ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setBlurLevel", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTime )
    {
    //  bool setTime ( int hour, int minute )
        int iHour; int iMinute;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iHour );
        argStream.ReadNumber ( iMinute );

        if ( !argStream.HasErrors () )
        {
            // Set the new time
            if ( CStaticFunctionDefinitions::SetTime ( static_cast < unsigned char > ( iHour ), static_cast < unsigned char > ( iMinute ) ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setTime", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getSkyGradient )
    {
        unsigned char ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB;
        CStaticFunctionDefinitions::GetSkyGradient ( ucTopR, ucTopG, ucTopB, ucBottomR, ucBottomG, ucBottomB );

        lua_pushnumber ( L, ucTopR );
        lua_pushnumber ( L, ucTopG );
        lua_pushnumber ( L, ucTopB );
        lua_pushnumber ( L, ucBottomR );
        lua_pushnumber ( L, ucBottomG );
        lua_pushnumber ( L, ucBottomB );
        return 6;
    }

    LUA_DECLARE( setSkyGradient )
    {
        CScriptArgReader argStream ( L );

        // Set the new sky gradient
        uchar ucTopRed, ucTopGreen, ucTopBlue;
        uchar ucBottomRed, ucBottomGreen, ucBottomBlue;

        argStream.ReadNumber ( ucTopRed, 0 );
        argStream.ReadNumber ( ucTopGreen, 0 );
        argStream.ReadNumber ( ucTopBlue, 0 );
        argStream.ReadNumber ( ucBottomRed, 0 );
        argStream.ReadNumber ( ucBottomGreen, 0 );
        argStream.ReadNumber ( ucBottomBlue, 0 );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetSkyGradient ( ucTopRed, ucTopGreen, ucTopBlue, ucBottomRed, ucBottomGreen, ucBottomBlue ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setSkyGradient", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetSkyGradient )
    {
        if ( CStaticFunctionDefinitions::ResetSkyGradient () )
        {
            lua_pushboolean ( L, true );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getHeatHaze )
    {
        SHeatHazeSettings settings;
        CStaticFunctionDefinitions::GetHeatHaze ( settings );

        lua_pushnumber ( L, settings.ucIntensity );
        lua_pushnumber ( L, settings.ucRandomShift );
        lua_pushnumber ( L, settings.usSpeedMin );
        lua_pushnumber ( L, settings.usSpeedMax );
        lua_pushnumber ( L, settings.sScanSizeX );
        lua_pushnumber ( L, settings.sScanSizeY );
        lua_pushnumber ( L, settings.usRenderSizeX );
        lua_pushnumber ( L, settings.usRenderSizeY );
        lua_pushboolean ( L, settings.bInsideBuilding );
        return 9;
    }

    LUA_DECLARE( setHeatHaze )
    {
        CScriptArgReader argStream ( L );

        // Set the new heat haze settings
        SHeatHazeSettings heatHaze;
        argStream.ReadNumber ( heatHaze.ucIntensity );
        argStream.ReadNumber ( heatHaze.ucRandomShift, 0 );
        argStream.ReadNumber ( heatHaze.usSpeedMin, 12 );
        argStream.ReadNumber ( heatHaze.usSpeedMax, 18 );
        argStream.ReadNumber ( heatHaze.sScanSizeX, 75 );
        argStream.ReadNumber ( heatHaze.sScanSizeY, 80 );
        argStream.ReadNumber ( heatHaze.usRenderSizeX, 80 );
        argStream.ReadNumber ( heatHaze.usRenderSizeY, 85 );
        argStream.ReadBool ( heatHaze.bInsideBuilding, false );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetHeatHaze ( heatHaze ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setHeatHaze", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetHeatHaze )
    {
        if ( CStaticFunctionDefinitions::ResetHeatHaze () )
        {
            lua_pushboolean ( L, true );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWaterColor )
    {
        float fRed, fGreen, fBlue, fAlpha;

        CStaticFunctionDefinitions::GetWaterColor ( fRed, fGreen, fBlue, fAlpha );

        lua_pushnumber ( L, fRed );
        lua_pushnumber ( L, fGreen );
        lua_pushnumber ( L, fBlue );
        lua_pushnumber ( L, fAlpha );
        return 4;
    }

    LUA_DECLARE( setWaterColor )
    {
    //  bool setWaterColor ( int red, int green, int blue, [ int alpha = 200 ] )
        float r; float g; float b; float alpha;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( r );
        argStream.ReadNumber ( g );
        argStream.ReadNumber ( b );
        argStream.ReadNumber ( alpha, 200 );

        if ( !argStream.HasErrors () )
        {
            // Set the new water colour
            if ( CStaticFunctionDefinitions::SetWaterColor ( r, g, b, alpha ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWaterColor", *argStream.GetErrorMessage () ) );

        // Failed
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetWaterColor )
    {
        if ( CStaticFunctionDefinitions::ResetWaterColor () )
        {
            lua_pushboolean ( L, true );
            return 1;
        }
        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWeather )
    {
    //  bool setWeather ( int weatherID )
        int iWeatherID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iWeatherID );

        if ( !argStream.HasErrors () )
        {
            // Set the new time
            if ( CStaticFunctionDefinitions::SetWeather ( static_cast < unsigned char > ( iWeatherID ) ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWeather", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;        
    }

    LUA_DECLARE( setWeatherBlended )
    {
    //  bool setWeatherBlended ( int weatherID )
        int iWeatherID;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iWeatherID );

        if ( !argStream.HasErrors () )
        {
            // Set the new time
            if ( CStaticFunctionDefinitions::SetWeatherBlended ( static_cast < unsigned char > ( iWeatherID ) ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWeatherBlended", *argStream.GetErrorMessage () ) );

        // Return false
        lua_pushboolean ( L, false );
        return 1;        
    }

    LUA_DECLARE( setGravity )
    {
    //  bool setGravity ( float level )
        float fGravity;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fGravity );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetGravity ( fGravity ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setGravity", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setGameSpeed )
    {
    //  bool setGameSpeed ( float value )
        float fSpeed;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fSpeed );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetGameSpeed ( fSpeed ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setGameSpeed", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setMinuteDuration )
    {
    //  bool setMinuteDuration ( int milliseconds )
        int iMilliseconds;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iMilliseconds );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetMinuteDuration ( iMilliseconds ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setMinuteDuration", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWaveHeight )
    {
    //  bool setWaveHeight ( float height )
        float fHeight;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fHeight );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWaveHeight ( fHeight ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWaveHeight", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setGarageOpen )
    {
    //  bool setGarageOpen ( int garageID, bool open )
        int iGarageID; bool bOpen;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iGarageID );
        argStream.ReadBool ( bOpen );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetGarageOpen ( iGarageID, bOpen ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setGarageOpen", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setJetpackMaxHeight )
    {
    //  bool setJetpackMaxHeight ( float Height )
        float fHeight;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fHeight );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetJetpackMaxHeight ( fHeight ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setJetpackMaxHeight", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setAircraftMaxHeight )
    {
    //  bool setAircraftMaxHeight ( float Height )
        float fHeight;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fHeight );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetAircraftMaxHeight ( fHeight ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setAircraftMaxHeight", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( isWorldSpecialPropertyEnabled )
    {
    //  bool isWorldSpecialPropertyEnabled ( string propname )
        SString strPropName;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strPropName );

        if ( !argStream.HasErrors () )
        {
            bool bResult = CStaticFunctionDefinitions::IsWorldSpecialPropertyEnabled ( strPropName );
            lua_pushboolean ( L, bResult );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "isWorldSpecialPropertyEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWorldSpecialPropertyEnabled )
    {
    //  bool setWorldSpecialPropertyEnabled ( string propname, bool enable )
        SString strPropName; bool bEnable;

        CScriptArgReader argStream ( L );
        argStream.ReadString ( strPropName );
        argStream.ReadBool ( bEnable );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWorldSpecialPropertyEnabled ( strPropName, bEnable ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWorldSpecialPropertyEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setCloudsEnabled )
    {
    //  bool setCloudsEnabled ( bool enabled )
        bool bEnabled;

        CScriptArgReader argStream ( L );
        argStream.ReadBool ( bEnabled );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetCloudsEnabled ( bEnabled ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setCloudsEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getJetpackMaxHeight )
    {
        lua_pushnumber ( L, g_pGame->GetWorld ()->GetJetpackMaxHeight ( ) );
        return 1;
    }

    LUA_DECLARE( getAircraftMaxHeight )
    {
        lua_pushnumber ( L, g_pGame->GetWorld ()->GetAircraftMaxHeight ( ) );
        return 1;
    }

    LUA_DECLARE( getCloudsEnabled )
    {
        lua_pushboolean ( L, CStaticFunctionDefinitions::GetCloudsEnabled () );
        return 1;
    }

    LUA_DECLARE( setTrafficLightState )
    {
    //  bool setTrafficLightState ( int state )
    //  bool setTrafficLightState ( string state )
    //  bool setTrafficLightState ( string colorNS, string colorEW )

        CScriptArgReader argStream ( L );

        // Determine which version to parse
        if ( argStream.NextIsNumber () )
        {
    //  bool setTrafficLightState ( int state )
            int iState;
            argStream.ReadNumber ( iState );

            if ( !argStream.HasErrors () )
            {
                if ( CStaticFunctionDefinitions::SetTrafficLightState ( iState ) )
                {
                    lua_pushboolean ( L, true );
                    return 1;
                }
            }
        }
        else if ( !argStream.NextIsString ( 1 ) )
        {
    //  bool setTrafficLightState ( string state )
            TrafficLight::EState eState;
            argStream.ReadEnumString ( eState );

            if ( !argStream.HasErrors () )
            {
                if ( eState == TrafficLight::AUTO )
                {
                    bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( false ) &&
                               CStaticFunctionDefinitions::SetTrafficLightState ( 0 );
                    lua_pushboolean ( L, bOk );
                    return 1;
                }
                else
                {
                    bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( true ) &&
                               CStaticFunctionDefinitions::SetTrafficLightState ( 9 );
                    lua_pushboolean ( L, bOk );
                    return 1;
                }
            }
        }
        else
        {
    //  bool setTrafficLightState ( string colorNS, string colorEW )
            TrafficLight::EColor eColorNS;
            TrafficLight::EColor eColorEW;
            argStream.ReadEnumString ( eColorNS );
            argStream.ReadEnumString ( eColorEW );

            if ( !argStream.HasErrors () )
            {
                unsigned char ucState = SharedUtil::GetTrafficLightStateFromColors ( eColorNS, eColorEW );

                // Change it.
                bool bOk = CStaticFunctionDefinitions::SetTrafficLightsLocked ( true ) &&
                           CStaticFunctionDefinitions::SetTrafficLightState ( ucState );
                lua_pushboolean ( L, bOk );
                return 1;
            }
        }

        if ( argStream.HasErrors () )
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setTrafficLightState", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setTrafficLightsLocked )
    {
    //  bool setTrafficLightsLocked ( bool bLocked )
        bool bLocked;

        CScriptArgReader argStream ( L );
        argStream.ReadBool ( bLocked );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetTrafficLightsLocked ( bLocked ) )
            {
                lua_pushboolean ( L, true );
                return 1 ;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setTrafficLightsLocked", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getWindVelocity )
    {
        float fX, fY, fZ;

        if ( CStaticFunctionDefinitions::GetWindVelocity ( fX, fY, fZ ) )
        {
            lua_pushnumber ( L, fX );
            lua_pushnumber ( L, fY );
            lua_pushnumber ( L, fZ );
            return 3;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( setWindVelocity )
    {
    //  bool setWindVelocity ( float velocityX, float velocityY, float velocityZ )
        float fX; float fY; float fZ;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fX );
        argStream.ReadNumber ( fY );
        argStream.ReadNumber ( fZ );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetWindVelocity ( fX, fY, fZ ) )
            {
                lua_pushboolean ( L, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setWindVelocity", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetWindVelocity )
    {
        if ( CStaticFunctionDefinitions::RestoreWindVelocity ( ) )
        {
            lua_pushboolean ( L, true );
            return 1;
        }

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getInteriorSoundsEnabled )
    {
        lua_pushboolean ( L, g_pMultiplayer->GetInteriorSoundsEnabled ( ) );
        return 1;
    }

    LUA_DECLARE( setInteriorSoundsEnabled )
    {
    //  bool setInteriorSoundsEnabled ( bool enabled )
        bool bEnabled;

        CScriptArgReader argStream ( L );
        argStream.ReadBool ( bEnabled );

        if ( !argStream.HasErrors () )
        {
            g_pMultiplayer->SetInteriorSoundsEnabled ( bEnabled );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setInteriorSoundsEnabled", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( getRainLevel )
    {
        lua_pushnumber ( L, g_pGame->GetWeather ()->GetAmountOfRain ());
        return 1;
    }

    LUA_DECLARE( setRainLevel )
    {
    //  bool setRainLevel ( float amount )
        float fAmount;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fAmount );

        if ( !argStream.HasErrors () )
        {
            g_pGame->GetWeather ()->SetAmountOfRain ( fAmount );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setRainLevel", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetRainLevel )
    {
        g_pGame->GetWeather ()->ResetAmountOfRain ( );

        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( getFarClipDistance )
    {
        lua_pushnumber ( L, g_pMultiplayer->GetFarClipDistance ( ) );
        return 1;
    }

    LUA_DECLARE( setFarClipDistance )
    {
    //  bool setFarClipDistance ( float distance )
        float fDistance;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fDistance );

        if ( !argStream.HasErrors () )
        {
            g_pMultiplayer->SetFarClipDistance ( fDistance );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setFarClipDistance", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetFarClipDistance )
    {
        g_pMultiplayer->RestoreFarClipDistance ();

        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( getFogDistance )
    {
        lua_pushnumber ( L, g_pMultiplayer->GetFogDistance());
        return 1;
    }

    LUA_DECLARE( setFogDistance )
    {
    //  bool setFogDistance ( float distance )
        float fDistance ;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fDistance );

        if ( !argStream.HasErrors () )
        {
            g_pMultiplayer->SetFogDistance ( fDistance );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setFogDistance", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetFogDistance )
    {
        g_pMultiplayer->RestoreFogDistance ( );

        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( getSunColor )
    {
        unsigned char ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue;
        
        g_pMultiplayer->GetSunColor ( ucCoreRed, ucCoreGreen, ucCoreBlue, ucCoronaRed, ucCoronaGreen, ucCoronaBlue );

        lua_pushnumber ( L, ucCoreRed );
        lua_pushnumber ( L, ucCoreGreen );
        lua_pushnumber ( L, ucCoreBlue );
        lua_pushnumber ( L, ucCoronaRed );
        lua_pushnumber ( L, ucCoronaGreen );
        lua_pushnumber ( L, ucCoronaBlue );

        return 6;
    }

    LUA_DECLARE( setSunColor )
    {
    //  bool setSunColor ( int coreRed, int coreGreen, int coreBlue, int coronaRed, int coronaGreen, int coronaBlue )
        int iCoreRed; int iCoreGreen; int iCoreBlue; int iCoronaRed; int iCoronaGreen; int iCoronaBlue;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( iCoreRed );
        argStream.ReadNumber ( iCoreGreen );
        argStream.ReadNumber ( iCoreBlue );
        argStream.ReadNumber ( iCoronaRed, iCoreRed );
        argStream.ReadNumber ( iCoronaGreen, iCoreGreen );
        argStream.ReadNumber ( iCoronaBlue, iCoreBlue );

        if ( !argStream.HasErrors () )
        {
            g_pMultiplayer->SetSunColor ( iCoreRed, iCoreGreen, iCoreBlue, iCoronaRed, iCoronaGreen, iCoronaBlue );
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setSunColor", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( resetSunColor )
    {
        g_pMultiplayer->ResetSunColor ( );

        lua_pushboolean ( L, true );
        return 1;
    }

    LUA_DECLARE( getSunSize )
    {
        lua_pushnumber ( L, g_pMultiplayer->GetSunSize ( ) );
        return 1;
    }

    LUA_DECLARE( setSunSize )
    {
    //  bool setSunSize ( float size )
        float fSize;

        CScriptArgReader argStream ( L );
        argStream.ReadNumber ( fSize );

        if ( !argStream.HasErrors () )
        {
            g_pMultiplayer->SetSunSize ( fSize );

            lua_pushboolean ( L, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogCustom( SString ( "Bad argument @ '%s' [%s]", "setSunSize", *argStream.GetErrorMessage () ) );

        lua_pushboolean ( L, false );
        return 1;
    }

    LUA_DECLARE( resetSunSize )
    {
        g_pMultiplayer->ResetSunSize ();

        lua_pushboolean ( L, true );
        return 1;
    }
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaDefs.cpp
*  PURPOSE:     Lua definitions
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

namespace CLuaFunctionDefs
{
    CLuaManager                 *m_pLuaManager = NULL;
    CScriptDebugging            *m_pScriptDebugging = NULL;
    CClientGame                 *m_pClientGame = NULL;
    CClientManager              *m_pManager = NULL;
    CClientEntity               *m_pRootEntity = NULL;
    CClientGUIManager           *m_pGUIManager = NULL;
    CClientPlayerManager        *m_pPlayerManager = NULL;
    CClientRadarMarkerManager   *m_pRadarMarkerManager = NULL;
    CResourceManager            *m_pResourceManager = NULL;
    CClientVehicleManager       *m_pVehicleManager = NULL;
    CClientColManager           *m_pColManager = NULL;
    CClientObjectManager        *m_pObjectManager = NULL;
    CClientTeamManager          *m_pTeamManager = NULL;
    CRenderWare                 *m_pRenderWare = NULL;
    CClientMarkerManager        *m_pMarkerManager = NULL;
    CClientPickupManager        *m_pPickupManager = NULL;
    CRegisteredCommands         *m_pRegisteredCommands = NULL;

    void Initialize( CClientGame *game, CLuaManager *manager, CScriptDebugging *debug )
    {
        m_pLuaManager = manager;
        m_pScriptDebugging = debug;
        m_pClientGame = game;
        m_pManager = game->GetManager ();
        m_pRootEntity = game->GetRootEntity ();
        m_pGUIManager = m_pManager->GetGUIManager ();
        m_pPlayerManager = m_pManager->GetPlayerManager ();
        m_pRadarMarkerManager = m_pManager->GetRadarMarkerManager ();
        m_pResourceManager = m_pManager->GetResourceManager ();
        m_pVehicleManager = m_pManager->GetVehicleManager ();
        m_pColManager = m_pManager->GetColManager ();
        m_pObjectManager = m_pManager->GetObjectManager ();
        m_pTeamManager = m_pManager->GetTeamManager ();
        m_pRenderWare = g_pGame->GetRenderWare ();
        m_pMarkerManager = m_pManager->GetMarkerManager ();
        m_pPickupManager = m_pManager->GetPickupManager ();
        m_pRegisteredCommands = game->GetRegisteredCommands ();

        LuaFunctionDefs::SetResourceManager( *m_pResourceManager );
        LuaFunctionDefs::SetRegisteredCommands( *m_pRegisteredCommands );
        LuaFunctionDefs::SetDebugging( *m_pScriptDebugging );
    }
}
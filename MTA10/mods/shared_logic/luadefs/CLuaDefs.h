/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaDefs.h
*  PURPOSE:     Lua definitions header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#ifndef __CLUADEFS_H
#define __CLUADEFS_H

class CClientColManager;
class CClientEntity;
class CClientGame;
class CClientGUIManager;
class CClientManager;
class CClientMarkerManager;
class CClientObjectManager;
class CClientPickupManager;
class CClientPlayerManager;
class CClientRadarMarkerManager;
class CClientTeamManager;
class CClientVehicleManager;
//class CLuaManager;
//class CRegisteredCommands;
class CRenderWare;
class CResourceManager;
class CScriptDebugging;

#define argtype(number,type) (lua_type(L,number) == type)
#define argtype2(number,type,type2) (lua_type(L,number) == type || lua_type(L,number) == type2)

namespace CLuaFunctionDefs
{
    void Initialize( CClientGame *game,
                     CLuaManager *manager,
                     CScriptDebugging *debug );

    static inline CLuaMain* lua_readcontext( lua_State *L )
    {
        return lua_readuserdata_assert <CLuaMain, LUA_STORAGEINDEX, 2> ( L );
    }

    // This is just for the LUA funcs. Please don't public this and use it other
    // places in the client.
    extern CLuaManager*                 m_pLuaManager;
    extern CScriptDebugging*            m_pScriptDebugging;
    extern CClientGame*                 m_pClientGame;
    extern CClientManager*              m_pManager;
    extern CClientEntity*               m_pRootEntity;
    extern CClientGUIManager*           m_pGUIManager;
    extern CClientPlayerManager*        m_pPlayerManager;
    extern CClientRadarMarkerManager*   m_pRadarMarkerManager;
    extern CResourceManager*            m_pResourceManager;
    extern CClientVehicleManager*       m_pVehicleManager;
    extern CClientColManager*           m_pColManager;
    extern CClientObjectManager*        m_pObjectManager;
    extern CClientTeamManager*          m_pTeamManager;
    extern CRenderWare*                 m_pRenderWare;
    extern CClientMarkerManager*        m_pMarkerManager;
    extern CClientPickupManager*        m_pPickupManager;
    extern CRegisteredCommands*         m_pRegisteredCommands;  
}

#endif

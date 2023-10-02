/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/logic/lua/CLuaManager.h
*  PURPOSE:     Management of Lua hyperstructures
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaManager;

#ifndef __CLUAMANAGER_H
#define __CLUAMANAGER_H

#include <list>
#include "../CClientGUIManager.h"
#include "../CEvents.h"
#include "CLuaMain.h"
#include "../CRegisteredCommands.h"

class CLuaManager : public LuaManager
{
public:
                                    CLuaManager             ( class CClientGame* pClientGame );
                                    ~CLuaManager            ();

    CLuaMain*                       Create                  ( CResource* owner );

    void                            DoPulse                 ();

    void                            SetScriptDebugging      ( CScriptDebugging* pScriptDebugging );

    void                            AddToPendingDeleteList  ( lua_State* m_luaVM )     { m_PendingDeleteList.push_back ( m_luaVM ); }
    void                            ProcessPendingDeleteList();

    CClientGUIManager*              m_pGUIManager;

private:
    void                            LoadCFunctions          ();

    CEvents*                        m_pEvents;
    CRegisteredCommands*            m_pRegisteredCommands;

    std::list < CLuaMain* >         m_virtualMachines;
    std::list < lua_State* >        m_PendingDeleteList;
};

#endif

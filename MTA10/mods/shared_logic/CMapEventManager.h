/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEventManager.h
*  PURPOSE:     Map event manager class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAPEVENTMANAGER_H
#define __CMAPEVENTMANAGER_H

#include "CMapEvent.h"

class CClientEntity;

class CMapEventManager
{
    friend class CMapEvent;
public:
                            CMapEventManager( CClientEntity *owner );
                            ~CMapEventManager();

    bool                    Add( CLuaMain *main, const char *name, const LuaFunctionRef& ref, bool propagated );
    bool                    Delete( CLuaMain *main, const char *name = NULL, const LuaFunctionRef *ref = NULL );
    void                    DeleteAll();

    CMapEvent*              Get( const char *name );

    bool                    Call( lua_State *callee, unsigned int argCount, const char *name, class CClientEntity *source );

private:
    RwList <CMapEvent>          m_list;
    CClientEntity*              m_owner;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEvent.h
*  PURPOSE:     Map event class header
*  DEVELOPERS:  Jax <>
*               Oliver Brown <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMAPEVENT_H
#define __CMAPEVENT_H

#define LUACLASS_MAPEVENT   90

#define MAPEVENT_MAX_LENGTH_NAME 100

class CMapEventManager;

class CMapEvent : public LuaElement
{
    friend class CMapEventManager;
public:
    inline class CLuaMain*  GetVM()                                             { return m_main; }
    inline const char*      GetName() const                                     { return m_name; }
    inline LuaFunctionRef   GetLuaFunction()                                    { return m_funcRef; }
    inline bool             IsPropagated() const                                { return m_propagated; }

private:
                            CMapEvent( class CLuaMain* main, CMapEventManager& manager, const char *name, const LuaFunctionRef& funcRef, bool propagated );
                            ~CMapEvent();

    class CLuaMain*         m_main;
    LuaFunctionRef          m_funcRef;
    char                    m_name[MAPEVENT_MAX_LENGTH_NAME + 1];
    bool                    m_propagated;
    CMapEventManager&       m_system;

    RwListEntry <CMapEvent> m_node;
};

#endif

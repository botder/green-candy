/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.h
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CLuaMain;

#ifndef __CLUAMAIN_H
#define __CLUAMAIN_H

#include "CLuaModuleManager.h"
#include "../CTextDisplay.h"

#define MAX_SCRIPTNAME_LENGTH 64

class CBlipManager;
class CObjectManager;
class CPlayerManager;
class CRadarAreaManager;
class CVehicleManager;
class CMapManager;

class CLuaManager;
class CResourceFile;

class CLuaMain : public LuaMain
{
public:
                                    CLuaMain( CLuaManager& luaManager,
                                              CObjectManager *objectManager,
                                              CPlayerManager *playerManager,
                                              CVehicleManager *vehicleManager,
                                              CBlipManager *blipManager,
                                              CRadarAreaManager *radarManager,
                                              CMapManager *mapManager,
                                              CResource *resource );
                                    ~CLuaMain();

    void                            InitVM();

    void                            RegisterFunction( const char *name, lua_CFunction proto );

    inline CBlipManager*            GetBlipManager() const                                  { return m_blipManager; };
    inline CObjectManager*          GetObjectManager() const                                { return m_objectManager; };
    inline CPlayerManager*          GetPlayerManager() const                                { return m_playerManager; };
    inline CVehicleManager*         GetVehicleManager() const                               { return m_vehicleManager; };
    inline CMapManager*             GetMapManager() const                                   { return m_mapManager; };
    
    CXMLFile*                       CreateXML( const char *path );
    void                            SaveXML( CXMLNode *root );
    unsigned long                   GetElementCount() const                                 { return m_resource && GetResource()->GetElementGroup() ? GetResource()->GetElementGroup()->GetCount() : 0; };
    unsigned long                   GetTextDisplayCount() const                             { return m_displays.size(); };
    unsigned long                   GetTextItemCount() const                                { return m_textItems.size(); };

    CTextDisplay*                   CreateDisplay();
    void                            DestroyDisplay( CTextDisplay *display );
    CTextItem*                      CreateTextItem( const char *text, float x, float y, eTextPriority priority = PRIORITY_LOW, const SColor color = -1, float scale = 1.0f, unsigned char format = 0, unsigned char shadowAlpha = 0 );
    void                            DestroyTextItem( CTextItem *item );

    bool                            TextDisplayExists( CTextDisplay *display );
    bool                            TextItemExists( CTextItem *item );

    inline CResource*               GetResource()                                           { return (CResource*)m_resource; }

    inline void                     SetResourceFile( CResourceFile *file )                  { m_resourceFile = file; }
    inline CResourceFile*           GetResourceFile() const                                 { return m_resourceFile; }

    void                            RegisterHTMLDFunctions();

private:
    CResourceFile*                  m_resourceFile;
    CBlipManager*                   m_blipManager;
    CObjectManager*                 m_objectManager;
    CPlayerManager*                 m_playerManager;
    CRadarAreaManager*              m_radarAreaManager;
    CVehicleManager*                m_vehicleManager;
    CMapManager*                    m_mapManager;

    typedef std::list <CTextDisplay*> textDisplays_t;
    typedef std::list <CTextItem*> textItems_t;

    textDisplays_t                  m_displays;
    textItems_t                     m_textItems;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.h
*  PURPOSE:     Resource manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               Oliver Brown <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include "ehs/ehs.h"

class CResourceManager : public EHS, public ResourceManager
{
public:
    enum eResourceQueue
    {
        QUEUE_STOP,
        QUEUE_STOPALL,
        QUEUE_RESTART,
        QUEUE_RESTART2,
    };

    struct sResourceStartFlags
    {
        bool bConfigs : 1;
        bool bMaps : 1;
        bool bScripts : 1;
        bool bHTML : 1;
        bool bClientConfigs : 1;
        bool bClientScripts : 1;
        bool bClientFiles : 1;
    };

private:
    struct sResourceQueue
    {
        CResource*          pResource;
        eResourceQueue      eQueue;
        sResourceStartFlags Flags;
        std::vector <SString>   dependents;
    };

public:
                                CResourceManager();
                                ~CResourceManager();

    CResource*                  Load( bool zip, const char *path, const char *name );
    CResource*                  Create( const char *name );
    CResource*                  Copy( CResource *src, const char *dst );
    void                        Unload( CResource *resource );
    void                        UnloadRemovedResources();
    void                        CheckResourceDependencies();
    void                        ListResourcesLoaded();

    typedef std::list <CResource*> resList_t;

    bool                        Refresh( bool all = false );
    void                        Upgrade();
    inline unsigned int         GetResourceLoadedCount()                { return m_loadedCount; }
    inline unsigned int         GetResourceFailedCount()                { return m_failedCount; }
    void                        OnPlayerJoin( CPlayer& Player );

    bool                        StartResource( CResource *res, const resList_t *dependents = NULL, 
                                                bool manual = false, bool startIncludes = true,
                                                bool configs = true, bool maps = true, bool scripts = true, bool html = true, bool clientConfigs = true,
                                                bool clientScripts = true, bool clientFiles = true );
    bool                        Reload( CResource *res );
    void                        StopAll();

    void                        QueueResource( CResource *res, eResourceQueue eQueueType, const sResourceStartFlags* Flags, const resList_t *dependents = NULL );
    void                        ProcessQueue();
    void                        RemoveFromQueue( CResource *res );

    bool                        IsAResourceElement( CElement *element );

    unsigned short              GenerateID();

    CResource*                  GetResourceFromLuaState( lua_State *L );
    bool                        Install( const char *url, const char *name );

    void                        NotifyResourceVMOpen( CResource *res, CLuaMain *main );
    void                        NotifyResourceVMClose( CResource *res, CLuaMain *main );

private:
    unsigned int                m_loadedCount;
    unsigned int                m_failedCount;
    bool                        m_resListChanged;
    resList_t                   m_loadQueue;

    // Maps to speed things up
    std::map <CResource*, lua_State*>   m_ResourceLuaStateMap;
    std::map <lua_State*, CResource*>   m_LuaStateResourceMap;

    std::list <sResourceQueue>  m_resourceQueue;
};

#endif

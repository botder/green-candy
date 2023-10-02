/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.cpp
*  PURPOSE:     Resource manager class
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Kevin Whiteside <>
*               Oliver Brown <>
*               Jax <>
*               lil_Toady <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CServerInterface* g_pServerInterface;

static bool s_bNotFirstTime = false; // if true, outputs "new resource loaded" messages, doesn't do it
                                     // first time to prevent console spam on startup

// SResInfo - Item in list of potential resources - Used in Refresh()
struct SResInfo
{
    SString strAbsPath;
    SString strName;
    bool bIsDir;
    bool bPathIssue;
};

CResourceManager::CResourceManager()
{
    m_bResourceListChanged = false;
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;
}

CResourceManager::~CResourceManager()
{
    // First process the queue to make sure all queued up tasks are done
    ProcessQueue ();

    // Then kill all the running resources
    StopAllResources ();

    // Then start deleting them
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        delete (*iter);
    }
}

// Load the complete list of resources and create their objects
// DOES NOT reload already loaded resources, we need a special function for lua for that (e.g. reloadResource)
// Talidan: yes it did, noob.  Not as of r897 - use bRefreshAll to do already loaded resources.
bool CResourceManager::Refresh ( bool bRefreshAll )
{
    UnloadRemovedResources();

    // Make list of potential active resources
    std::map < SString, SResInfo > resInfoMap;

    // Initial search dir
    std::vector < SString > resourcesPathList;
    resourcesPathList.push_back ( "resources" );

    SString strModPath = g_pServerInterface->GetModManager ()->GetModPath ();
    for ( uint i = 0 ; i < resourcesPathList.size () ; i++ )
    {
        // Enumerate all files and directories
        SString strResourcesRelPath = resourcesPathList[i];
        SString strResourcesAbsPath = PathJoin ( strModPath, strResourcesRelPath, "/" );
        std::vector < SString > itemList = FindFiles ( strResourcesAbsPath, true, true );

        // Check each item
        for ( uint i = 0 ; i < itemList.size () ; i++ )
        {
            SString strName = itemList[i];

            // Ignore items that start with a dot
            if ( strName[0] == '.' )
                continue;

            bool bIsDir = DirectoryExists ( PathJoin ( strResourcesAbsPath, strName ) );

            // Recurse into [directories]
            if ( bIsDir && ( strName.BeginsWith( "#" ) || ( strName.BeginsWith( "[" ) && strName.EndsWith( "]" ) ) ) )
            {
                resourcesPathList.push_back ( PathJoin ( strResourcesRelPath, strName ) );
                continue;
            }

            // Extract file extention
            SString strExt;
            if ( !bIsDir )
                ExtractExtention ( strName, &strName, &strExt );

            // Ignore files that are not .zip
            if ( !bIsDir && strExt != "zip" )
                continue;

            // Ignore items that have dot or space in the name
            if ( strName.Contains ( "." ) || strName.Contains ( " " ) )
            {
                CLogger::LogPrintf ( "WARNING: Not loading resource '%s' as it contains illegal characters\n", *strName );
                continue;
            }

            // Ignore dir items with no meta.xml (assume it's the result of saved files from a zipped resource)
            if ( bIsDir && !FileExists ( PathJoin ( strResourcesAbsPath, strName, "meta.xml" ) ) )
                continue;

            // Add potential resource to list
            SResInfo newInfo;
            newInfo.strAbsPath = strResourcesAbsPath;
            newInfo.strName = strName;
            newInfo.bIsDir = bIsDir;
            newInfo.bPathIssue = false;

            // Check for duplicate
            if ( SResInfo* pDup = MapFind ( resInfoMap, strName ) )
            {
                // Is path the same?
                if ( newInfo.strAbsPath == pDup->strAbsPath )
                {
                    if ( newInfo.bIsDir )
                    {
                        // If non-zipped item, replace already existing zipped item on the same path
                        assert ( !pDup->bIsDir );
                        *pDup = newInfo;
                    }
                }
                else
                {
                    // Don't load resource if there are duplicates on different paths
                    pDup->bPathIssue = true;
                }
            }
            else
            {
                // No duplicate found
                MapSet ( resInfoMap, strName, newInfo );
            }
        }
    }

    // Process potential resource list
    for ( std::map < SString, SResInfo >::const_iterator iter = resInfoMap.begin () ; iter != resInfoMap.end () ; ++iter )
    {
        const SResInfo& info = iter->second;
        if ( !info.bPathIssue )
        {
            CResource* pResource = GetResource ( info.strName );

            if ( bRefreshAll || !pResource || !pResource->CheckIfStartable() )
            {
                // Add the resource
                Load ( !info.bIsDir, info.strAbsPath, info.strName );
            }
        }
    }


    CheckResourceDependencies();

    // Print important errors
    for ( std::map < SString, SResInfo >::const_iterator iter = resInfoMap.begin () ; iter != resInfoMap.end () ; ++iter )
    {
        const SResInfo& info = iter->second;
        if ( info.bPathIssue )
        {
            CLogger::ErrorPrintf ( "Not processing resource '%s' as it has duplicates on different paths\n", *info.strName );
        }
        else
        {
            CResource* pResource = GetResource ( info.strName );
            if ( pResource && !pResource->CheckIfStartable() )
            {
                CLogger::ErrorPrintf ( "Problem with resource: %s; %s\n", *info.strName, pResource->GetFailureReason ().c_str () ); 
            }
        }
    }


    if ( m_bResourceListChanged )
    {
        m_bResourceListChanged = false;
        CLogger::LogPrintf ( "Resources: %d loaded, %d failed\n", m_uiResourceLoadedCount, m_uiResourceFailedCount );
    }

    list < CResource* > ::iterator iter = m_resourcesToStartAfterRefresh.begin ();
    for ( ; iter != m_resourcesToStartAfterRefresh.end (); iter++ )
    {
        (*iter)->Start();
    }
    m_resourcesToStartAfterRefresh.clear();

    s_bNotFirstTime = true;

    return true;
}

void CResourceManager::Upgrade()
{
    // Modify source files if needed
    for ( list < CResource* > ::const_iterator iter = m_resources.begin () ; iter != m_resources.end (); iter++ )
        (*iter)->ApplyUpgradeModifications ();

    Refresh( true );
}

void CResourceManager::CheckResourceDependencies()
{
    m_uiResourceLoadedCount = 0;
    m_uiResourceFailedCount = 0;

    list <CResource*>::const_iterator iter = m_resources.begin();

    for ( ; iter != m_resources.end (); iter++ )
        (*iter)->LinkToIncludedResources();

    iter = m_resources.begin();

    for ( ; iter != m_resources.end(); iter++ )
    {
        if ( (*iter)->CheckIfStartable() )
            m_uiResourceLoadedCount++;
        else
            m_uiResourceFailedCount++;
    }
}

void CResourceManager::ListResourcesLoaded()
{
    unsigned int uiCount = 0;
    unsigned int uiFailedCount = 0;
    unsigned int uiRunningCount = 0;
    CLogger::LogPrintf ( "== Resource list ==\n" );
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource * res = (*iter);
        if ( res->IsLoaded() )
        {
            if ( res->IsActive() )
            {
                CLogger::LogPrintf ( "%-20.20s   RUNNING   (%d dependents)\n", res->GetName().c_str(), res->GetDependentCount() );
                uiRunningCount++;
            }
            else
                CLogger::LogPrintf ( "%-20.20s   STOPPED   (%d files)\n", res->GetName().c_str(), res->GetFileCount() );
            uiCount ++;
        }
        else
        {
            CLogger::LogPrintf ( "%-20.20s   FAILED    (see info command for reason)\n", res->GetName().c_str () );
            uiFailedCount ++;
        }
    }
    CLogger::LogPrintf ( "Resources: %d loaded, %d failed, %d running\n", uiCount, uiFailedCount, uiRunningCount );
}

// check all loaded resources and see if they're still valid (i.e. have a meta.xml file)
void CResourceManager::UnloadRemovedResources()
{
    list < CResource* > resourcesToDelete;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    string strPath;
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( ! (*iter)->GetFilePath ( "meta.xml", strPath ) )
        {
            if ( (*iter)->IsActive() )
                CLogger::ErrorPrintf ( "Resource '%s' has been removed while running! Stopping resource.\n", (*iter)->GetName().c_str () );
            else
                CLogger::LogPrintf ( "Resource '%s' has been removed, unloading\n", (*iter)->GetName().c_str () );
            resourcesToDelete.push_back ( (*iter) );
            m_bResourceListChanged = true;
        }
    }

    iter = resourcesToDelete.begin ();
    for ( ; iter != resourcesToDelete.end (); iter++ )
    {
        Unload ( *iter );
    }
}

void CResourceManager::Unload( CResource * resource )
{
    RemoveResourceFromLists ( resource );
    m_resourcesToStartAfterRefresh.remove ( resource );
    RemoveFromQueue ( resource );
    delete resource;
}

CResource* CResourceManager::Load( bool bIsZipped, const char * szAbsPath, const char * szResourceName )
{
    CResource * loadedResource = NULL;
    bool bStartAfterLoading = false;

    // check to see if we've already loaded this resource - we can only
    // load each resource once

    list < CResource* > resourcesToDelete;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        if ( stricmp ( (*iter)->GetName().c_str (), szResourceName ) == 0 )
        {
            if ( (*iter)->HasResourceChanged() )
            {
                if ( (*iter)->IsActive() )
                {
                    CLogger::LogPrintf ( "Resource '%s' changed while running, reloading and restarting\n", szResourceName );
                    bStartAfterLoading = true;
                }
                else
                {
                    CLogger::LogPrintf ( "Resource '%s' changed, reloading\n", szResourceName );
                }

                resourcesToDelete.push_back ( (*iter) );
            }
            else
                return (*iter);
        }
    }

    iter = resourcesToDelete.begin ();
    for ( ; iter != resourcesToDelete.end (); iter++ )
    {
        // Stop it first. This fixes bug #3729 because it isn't removed from the list before it's stopped.
        // Removing it from the resources list first caused the resource pointer to be unverifyable, and
        // the pointer wouldn't work in resource LUA functions.
        (*iter)->Stop ( true );

        RemoveResourceFromLists ( *iter );
        m_resourcesToStartAfterRefresh.remove ( *iter );
        RemoveFromQueue ( *iter );
        delete *iter;

        m_bResourceListChanged = true;
    }

    try
    {
        loadedResource = new CResource( this, bIsZipped, szAbsPath, szResourceName );
    }
    catch( std::exception& e )
    {
        CLogger::ErrorPrintf("Loading of resource '%s' failed: %s\n", szResourceName, e.what() );
        //m_resourcesToStartAfterRefresh.remove( loadedResource );
        //RemoveFromQueue( loadedResource );
        return NULL;
    }

    if ( bStartAfterLoading )
        m_resourcesToStartAfterRefresh.push_back ( loadedResource );
    if ( s_bNotFirstTime )
        CLogger::LogPrintf("New resource '%s' loaded\n", loadedResource->GetName().c_str () );
    unsigned short usID = GenerateID ();
    loadedResource->SetID ( usID );
    AddResourceToLists ( loadedResource );
    m_bResourceListChanged = true;
    return loadedResource;
}

unsigned short CResourceManager::GenerateID()
{
    // Create a map of all used IDs
    map < unsigned short, bool > idMap;
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        idMap[ ( *iter )->GetID () ] = true;
    }

    // Find an unused ID
    for ( unsigned short i = 0 ; i < 0xFFFE ; i++ )
    {
        if ( idMap.find ( i ) == idMap.end () )
            return i;
    }
    return 0xFFFF;
}


void CResourceManager::OnPlayerJoin( CPlayer& Player )
{
    // Loop through our started resources so they start in the correct order clientside
    list < CResource* > ::iterator iter = CResource::m_StartedResources.begin ();
    for ( ; iter != CResource::m_StartedResources.end (); iter++ )
    {
        ( *iter )->OnPlayerJoin ( Player );
    }
}

//
// Add resource <-> luaVM lookup mapping
//
void CResourceManager::NotifyResourceVMOpen( CResource* pResource, CLuaMain* pVM )
{
    lua_State* luaVM = pVM->GetVirtualMachine ();
    assert ( luaVM );
    assert ( !MapContains ( m_ResourceLuaStateMap, pResource ) );
    assert ( !MapContains ( m_LuaStateResourceMap, luaVM ) );
    MapSet ( m_ResourceLuaStateMap, pResource, luaVM );
    MapSet ( m_LuaStateResourceMap, luaVM, pResource );
}

void CResourceManager::NotifyResourceVMClose( CResource* pResource, CLuaMain* pVM )
{
    lua_State* luaVM = pVM->GetVirtualMachine ();
    assert ( luaVM );
    assert ( MapContains ( m_ResourceLuaStateMap, pResource ) );
    assert ( MapContains ( m_LuaStateResourceMap, luaVM ) );
    MapRemove ( m_ResourceLuaStateMap, pResource );
    MapRemove ( m_LuaStateResourceMap, luaVM );
}

void CResourceManager::AddResourceToLists( CResource* pResource )
{
    SString strResourceNameKey = SString ( pResource->GetName () ).ToUpper ();

    assert ( !m_resources.Contains ( pResource ) );
    assert ( !MapContains ( m_NameResourceMap, strResourceNameKey ) );
    assert ( !MapContains ( m_ResourceLuaStateMap, pResource ) );

    m_resources.push_back ( pResource );

    CLuaMain* pLuaMain = pResource->GetVirtualMachine ();
    assert ( !pLuaMain );
    MapSet ( m_NameResourceMap, strResourceNameKey, pResource );
}

CResource* CResourceManager::GetResourceFromLuaState( lua_State* luaVM )
{
    luaVM = lua_getmainstate ( luaVM );

    // Use lookup map
    CResource** ppResource = MapFind ( m_LuaStateResourceMap, luaVM );
    if ( ppResource )
    {
        CResource* pResource = *ppResource;
        CLuaMain* pLuaMain = pResource->GetVirtualMachine ();
        if ( pLuaMain )
        {
            assert ( luaVM == pLuaMain->GetVirtualMachine () );
            return pResource;
        }
    }
    return NULL;
}

bool CResourceManager::IsAResourceElement( CElement* pElement )
{
    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource* pResource = *iter;
        if ( pResource->IsActive () )
        {
            if ( pResource->GetResourceRootElement () == pElement || pResource->GetDynamicElementRoot () == pElement )
                return true;
            else
            {
                list < CResourceFile* > ::iterator fiter = pResource->IterBegin ();
                for ( ; fiter != pResource->IterEnd (); fiter++ )
                {
                    if ( (*fiter)->GetType () == CResourceFile::RESOURCE_FILE_TYPE_MAP )
                    {
                        CResourceMapItem* pMap = static_cast < CResourceMapItem* > ( *fiter );
                        if ( pMap->GetMapRootElement () == pElement )
                            return true;
                    }
                }
            }
        }
    }
    return false;
}


bool CResourceManager::StartResource( CResource* pResource, list < CResource* > * dependents, bool bStartedManually, bool bStartIncludedResources, bool bConfigs, bool bMaps, bool bScripts, bool bHTML, bool bClientConfigs, bool bClientScripts, bool bClientFiles )
{
    // Has resurce changed since load?
    if ( pResource->HasResourceChanged() )
    {
        // Attempt to reload it
        if ( Reload ( pResource ) )
        {
            // Start the resource
            return pResource->Start( NULL, bStartedManually, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles );
        }
        else
            return false;
    }
    else
    {
        // If it's not running yet
        if ( !pResource->IsActive() )
        {
            // Start it
            return pResource->Start ( dependents, bStartedManually, bStartIncludedResources, bConfigs, bMaps, bScripts, bHTML, bClientConfigs, bClientScripts, bClientFiles );
        }
        return false;
    }

    // Stop it again if it failed starting
    pResource->Stop ();
    return false;
}


bool CResourceManager::Reload( CResource* pResource )
{
    std::string strResourceName = pResource->GetName();

    CLogger::LogPrintf( "Resource '%s' changed, reloading and starting\n", strResourceName.c_str () );

    try
    {
        // Delete the old resource and create a new one
        pResource->Reload();
    }
    catch( std::exception& e )
    {
        CLogger::LogPrintf( "Loading of resource '%s' failed: %s\n", strResourceName.c_str(), e.what() );
        return false;
    }

    m_bResourceListChanged = true;

    pResource->SetID( GenerateID() );
    return true;
}


bool CResourceManager::StopAll()
{
    CLogger::SetMinLogLevel ( LOGLEVEL_MEDIUM );
    CLogger::LogPrint ( "Stopping resources..." );
    CLogger::ProgressDotsBegin ();

    list < CResource* > ::const_iterator iter = m_resources.begin ();
    for ( ; iter != m_resources.end (); iter++ )
    {
        CResource* pResource = *iter;
        if ( pResource->IsActive () )
        {

            if ( pResource->IsPersistent () )
                pResource->SetPersistent ( false );

            pResource->Stop ( true );

            CLogger::ProgressDotsUpdate ();
        }
    }

    CLogger::ProgressDotsEnd ();
    CLogger::SetMinLogLevel ( LOGLEVEL_LOW );
    return true;
}

void CResourceManager::QueueResource( CResource* pResource, eResourceQueue eQueueType, const sResourceStartFlags* Flags, list < CResource* > * dependents )
{
    // Make the queue item
    sResourceQueue sItem;
    sItem.pResource = pResource;
    sItem.eQueue = eQueueType;
    if ( dependents )
        for ( list < CResource* >::iterator it = dependents->begin () ; it != dependents->end () ; ++it )
           sItem.dependents.push_back ( (*it)->GetName () );

    if ( Flags )
    {
        sItem.Flags = *Flags;
    }
    else
    {
        sItem.Flags.bClientConfigs = true;
        sItem.Flags.bClientFiles = true;
        sItem.Flags.bClientScripts = true;
        sItem.Flags.bHTML = true;
        sItem.Flags.bScripts = true;
        sItem.Flags.bMaps = true;
        sItem.Flags.bConfigs = true;
    }

    // Push it to the back of the queue
    m_resourceQueue.push_back ( sItem );
}


void CResourceManager::ProcessQueue()
{
    // While we have queuestuff to process
    while ( m_resourceQueue.size () > 0 )
    {
        // Pop the first item
        sResourceQueue sItem = m_resourceQueue.front ();
        m_resourceQueue.pop_front ();

        // Supposed to stop it?
        if ( sItem.eQueue == QUEUE_STOP )
        {
            // It must be active
            if ( sItem.pResource->IsActive () )
            {
                // So we can stop it incase it is persistent
                if ( sItem.pResource->IsPersistent () )
                {
                    sItem.pResource->SetPersistent ( false );
                }

                // Stop the resource
                if ( !sItem.pResource->Stop ( true ) )
                {
                    // Failed
                    CLogger::ErrorPrintf ( "Unable to stop resource %s\n", sItem.pResource->GetName ().c_str () );
                }
            }
        }

        // Supposed to stop all resources
        else if ( sItem.eQueue == QUEUE_STOPALL )
        {
            // Do so
            StopAllResources ();
        }

        // Supposed to restart it?
        else if ( sItem.eQueue == QUEUE_RESTART )
        {
            // It must be active
            if ( sItem.pResource->IsActive () )
            {
                // So we can stop it incase it is persistent
                if ( sItem.pResource->IsPersistent() )
                {
                    sItem.pResource->SetPersistent ( false );
                }

                // Copy the dependents
                list<CResource *> * resourceList = sItem.pResource->GetDependents();
                list<CResource *> resourceListCopy;
                list<CResource *>::iterator iterd = resourceList->begin();
                for ( ; iterd != resourceList->end(); iterd++ )
                {
                    resourceListCopy.push_back ( (*iterd) );
                }

                // Stop it
                if ( sItem.pResource->Stop ( true ) )
                {
                    // Continue after the rest of the queue is processed
                    QueueResource ( sItem.pResource, QUEUE_RESTART2, &sItem.Flags, &resourceListCopy );
                }
                else
                    CLogger::ErrorPrintf ( "Unable to stop resource %s for restart\n", sItem.pResource->GetName ().c_str () );
            }
        }
        // Restart part 2
        else if ( sItem.eQueue == QUEUE_RESTART2 )
        {
            list < CResource* > resourceListCopy;
            for ( vector < SString >::iterator it = sItem.dependents.begin () ; it != sItem.dependents.end () ; ++it )
            {
                CResource* pResource = GetResource ( *it );
                if ( pResource )
                    resourceListCopy.push_back ( pResource);
            }

            // Start it again
            if ( !StartResource ( sItem.pResource, &resourceListCopy, true, true,
                                 sItem.Flags.bConfigs, sItem.Flags.bMaps,
                                 sItem.Flags.bScripts, sItem.Flags.bHTML,
                                 sItem.Flags.bClientConfigs, sItem.Flags.bClientScripts,
                                 sItem.Flags.bClientFiles ) )
            {
                // Failed
                CLogger::ErrorPrintf ( "Unable to restart resource %s\n", sItem.pResource->GetName ().c_str () );
            }
            else
                CLogger::LogPrintf ( "%s restarted successfully\n", sItem.pResource->GetName ().c_str () );
        }
    }
}

void CResourceManager::RemoveFromQueue( CResource* pResource )
{
    // Loop through our resourcequeue
    std::list < sResourceQueue > ::iterator iter = m_resourceQueue.begin ();
    while ( iter != m_resourceQueue.end () )
    {
        // Matching resource, erase it. Otherwize just increase the iterator
        if ( iter->pResource == pResource )
            iter = m_resourceQueue.erase ( iter );
        else
            iter++;
    }
}

bool CResourceManager::Install( const char *szURL, const char *szName )
{
    size_t size;
    CTCPImpl tcp;
    tcp.Initialize();

    CHTTPRequest request( szURL );
    CHTTPResponse *response = request->Send( &tcp );    // guys, why do you need to alloc on the heap? use the stack!!

    if ( !response )
        return false;

    if ( ( size = response->GetDataLength() ) == 0 )
    {
        delete response;
        return false;
    }
    resFileRoot->WriteData( SString( szName ) + ".zip", response->GetData(), size );

    delete response;
    return true;
}

CResource* CResourceManager::CreateResource( const SString& name )
{
    // Does the resource name already exist?
    if ( GetResource( name ) != NULL )
        return NULL;

    filePath path;
    
    if ( !resFileRoot->GetFullPath( name + "/meta.xml", true, path ) )
        return false;

    // Create an empty meta file for that resource
    CXMLFile *pXML = g_pServerInterface->GetXML()->CreateXML( path.c_str() );

    if ( !pXML )
        return NULL;

    pXML->CreateRootNode( "meta" );

    if ( !pXML->Write() )
    {
        delete pXML;

        return NULL;
    }

    delete pXML;

    CResource *res;

    try
    {
        // Add the resource and load it
        AddResourceToLists( res = new CResource( this, false, name ) );
    }
    catch( ... )
    {
        // Fail
        return NULL;
    }

    return res;
}

CResource* CResourceManager::CopyResource( CResource* pSourceResource, const char* szNewResourceName )
{
    return NULL;
}
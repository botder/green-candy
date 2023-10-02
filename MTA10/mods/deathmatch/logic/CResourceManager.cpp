/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceManager.cpp
*  PURPOSE:     Resource manager
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CFileTranslator *resFileRoot;
CResourceManager *resMan = NULL;

CResourceManager::CResourceManager() : ResourceManager( g_pClientGame->GetLuaManager()->GetVirtualMachine() )
{
    filePath resRoot;
    modFileRoot->GetFullPath( "/resources/", false, resRoot );

    // It has to exist :3
    modFileRoot->CreateDir( resRoot.c_str() );

    ResourceManager::resFileRoot = g_pCore->GetFileSystem()->CreateTranslator( resRoot.c_str() );
    ::resFileRoot = ResourceManager::resFileRoot;

    // The one and only resource manager!
    resMan = this;

    LuaManager::m_resMan = this;
}

CResourceManager::~CResourceManager()
{
    m_resources.clear();    // Lua takes care of deletion, since everything lives in the VM
}

CResource* CResourceManager::Add( unsigned short id, const char *name, CClientEntity *resEntity, CClientEntity *dynamicEntity )
{
    std::string extName = name;
    extName += '/';

    filePath resPath;

    if ( !resFileRoot->GetFullPathFromRoot( extName.c_str(), false, resPath ) )
        return NULL;

    // Create the folder if it does not exist
    resFileRoot->CreateDir( extName.c_str() );

    CFileTranslator *root = g_pCore->GetFileSystem()->CreateTranslator( resPath.c_str() );

    CResource *res = new CResource( id, name, *root, resEntity, dynamicEntity );

    m_resByName[name] = res;
    m_resources.push_back( res );
    return res;
}

void CResourceManager::LoadUnavailableResources()
{
    resourceList_t::const_iterator iter = m_resources.begin();

    for ( ; iter != m_resources.end(); ++iter )
    {
        if ( !(*iter)->GetActive() )
            ((CResource*)*iter)->Load();
    }
}

void CResourceManager::Remove( Resource *res )
{
    ResourceManager::Remove( res );
}

void CResourceManager::StopAll()
{
    while ( m_resources.size() != 0 )
        Remove( m_resources.front() );
}

void CResourceManager::GarbageCollect( lua_State *L )
{
}

bool CResourceManager::ParseResourcePath( Resource*& res, const char *path, std::string& meta )
{
    if ( path[0] == '@' )
    {
        meta = '@';
        path++;
    }

    if ( !ResourceManager::ParseResourcePath( res, path, path ) )
        return false;

    meta += path;
    return true;
}

bool CResourceManager::CreateDir( Resource *res, const char *path )
{
    std::string meta;

    if ( !ParseResourcePath( res, path, meta ) )
        return NULL;

    return res->CreateDir( meta.c_str() );
}

CFile* CResourceManager::OpenStream( Resource *res, const char *path, const char *mode )
{
    std::string meta;

    if ( !ParseResourcePath( res, path, meta ) )
        return NULL;

    return res->OpenStream( meta.c_str(), mode );
}

bool CResourceManager::FileCopy( Resource *res, const char *src, const char *dst )
{
    Resource *dstRes = res;
    std::string srcMeta;
    std::string dstMeta;

    if ( !ParseResourcePath( res, src, srcMeta ) || !ParseResourcePath( dstRes, dst, dstMeta ) )
        return false;

    if ( res == dstRes )
        return res->FileCopy( src, dst );

    CFile *srcFile = res->OpenStream( src, "rb" );

    if ( !srcFile )
        return false;

    CFile *dstFile = dstRes->OpenStream( dst, "wb" );

    if ( !dstFile )
    {
        delete srcFile;

        return false;
    }

    FileSystem::StreamCopy( *srcFile, *dstFile );

    delete srcFile;
    delete dstFile;

    return true;
}

bool CResourceManager::FileRename( Resource *res, const char *src, const char *dst )
{
    Resource *dstRes = res;
    std::string srcMeta;
    std::string dstMeta;

    if ( !ParseResourcePath( res, src, srcMeta ) || !ParseResourcePath( dstRes, dst, dstMeta ) )
        return false;

    if ( res == dstRes )
        return res->FileRename( src, dst );

    CFile *srcFile = res->OpenStream( src, "rb+" );

    if ( !srcFile )
        return false;

    CFile *dstFile = dstRes->OpenStream( dst, "wb" );

    if ( !dstFile )
    {
        delete srcFile;

        return false;
    }

    FileSystem::StreamCopy( *srcFile, *dstFile );

    delete srcFile;
    delete dstFile;

    return res->FileDelete( src );
}

size_t CResourceManager::FileSize( Resource *res, const char *path )
{
    std::string meta;

    if ( !ParseResourcePath( res, path, meta ) )
        return 0;

    return res->FileSize( meta.c_str() );
}

bool CResourceManager::FileExists( Resource *res, const char *path )
{
    std::string meta;

    return ParseResourcePath( res, path, meta ) && res->FileExists( meta.c_str() );
}

bool CResourceManager::FileDelete( Resource *res, const char *path )
{
    std::string meta;

    return ParseResourcePath( res, path, meta ) && res->FileDelete( meta.c_str() );
}
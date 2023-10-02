/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.h
*  PURPOSE:     Resource object class
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Chris McArthur <>
*               Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCE_H
#define CRESOURCE_H

#include "CResourceConfigItem.h"
#include "CResourceFile.h"
#include "CElementGroup.h"

class CExportedFunction
{
public:
    CExportedFunction( const std::string& name )
    {
        m_name = name;
    }

    inline const std::string&   GetFunctionName()    { return m_name; }

private:
    std::string m_name;
};

class CClientEntity;

class CResource : public Resource
{
    friend class CClientRwObject;
    friend class CClientGameTexture;
    friend class CResourceManager;
public:
                                CResource( unsigned short id, const filePath& name, CFileTranslator& root, CClientEntity *entity, CClientEntity *dynamicEntity );
                                ~CResource();

    void                        Load();

    bool                        InDownloadQueue()                                   { return m_inDownQueue; };
    bool                        SetInDownloadQueue( bool in )                       { m_inDownQueue = in; };

    CDownloadableResource*      QueueFile( CDownloadableResource::eResourceType type, const char *path, CChecksum chksm );

    typedef std::list <class CResourceConfigItem*> configList_t;

    CDownloadableResource*      AddConfigFile( const char *path, CChecksum chksum );
    configList_t::iterator      ConfigIterBegin()                                   { return m_configFiles.begin(); }
    configList_t::iterator      ConfigIterEnd()                                     { return m_configFiles.end(); }

    CElementGroup*              GetElementGroup()                                   { return m_defaultGroup; }

    void                        AddExportedFunction( const char *name );

    typedef std::list <CExportedFunction*> exports_t;

    exports_t::iterator         IterBeginExports()                                  { return m_exports.begin(); }
    exports_t::iterator         IterEndExports()                                    { return m_exports.end(); }

    class CClientEntity*        GetResourceEntity()                                 { return m_resourceEntity; }
    void                        SetResourceEntity( CClientEntity *entity );
    class CClientEntity*        GetResourceDynamicEntity()                          { return m_dynamicEntity; }
    class CClientEntity*        GetResourceGUIEntity()                              { return m_guiEntity; }
    inline CClientEntity*       GetResourceCOLModelRoot()                           { return m_colEntity; };
    inline CClientEntity*       GetResourceDFFRoot()                                { return m_dffEntity; };
    inline CClientEntity*       GetResourceTXDRoot()                                { return m_txdEntity; };

    // Use this for cursor showing/hiding
    void                        ShowCursor( bool bShow, bool bToggleControls = true );

    bool                        GetFullMetaPath( const char *path, filePath& absPath );
    bool                        CreateDir( const char *path );
    CFile*                      OpenStream( const char *path, const char *mode );
    bool                        FileCopy( const char *src, const char *dst );
    bool                        FileRename( const char *src, const char *dst );
    size_t                      FileSize( const char *path ) const;
    bool                        FileExists( const char *path ) const;
    bool                        FileDelete( const char *path );

    RwList <CClientRwObject>    m_rwObjects;
    RwList <CClientGameTexture> m_gameTextures;

private:
    CLuaManager*                m_luaManager;
    CClientEntity*              m_rootEntity;
    CClientEntity*              m_resourceEntity;
    CClientEntity*              m_dynamicEntity;
    CClientEntity*              m_colEntity;
    CClientEntity*              m_dffEntity;
    CClientEntity*              m_guiEntity;
    CClientEntity*              m_txdEntity;
    bool                        m_inDownQueue;

    // To control cursor show/hide
    static int                  m_refShowCursor;
    bool                        m_showCursor;

    CFileTranslator*            m_privateRoot;  // server id private directory

    typedef std::list <class CResourceFile*> fileList_t;
    typedef std::list <CElementGroup*> groupList_t;

    fileList_t                  m_files;
    configList_t                m_configFiles;
    groupList_t                 m_elementGroups; // stores elements created by scripts in this resource
    exports_t                   m_exports;
    CElementGroup*              m_defaultGroup;
};

#endif

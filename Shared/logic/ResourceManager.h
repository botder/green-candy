/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ResourceManager.h
*  PURPOSE:     Resource management for scripting environments
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_RESOURCE_MANAGER_
#define _BASE_RESOURCE_MANAGER_

#define LUACLASS_ROOT   23

class ResourceManager abstract : public LuaClass
{
public:
                            ResourceManager( lua_State *L );
    virtual                 ~ResourceManager();

    Resource*               Get( const char *name );
    Resource*               Get( unsigned short id );
    virtual void            Remove( Resource *res );
    void                    Remove( unsigned short id );
    bool                    Exists( Resource *res ) const;

    typedef std::list <Resource*> resourceList_t;

    resourceList_t::const_iterator  IterBegin()         { return m_resources.begin(); };
    resourceList_t::const_iterator  IterEnd()           { return m_resources.end(); };

    // File interface
    bool                    ParseResourcePath( Resource*& res, const char *path, const char*& meta );
    bool                    ParseResourceFullPath( Resource*& res, const char *path, const char*& meta, filePath& absPath );
    virtual bool            CreateDir( Resource *res, const char *path );
    virtual CFile*          OpenStream( Resource *res, const char *path, const char *mode );
    virtual bool            FileCopy( Resource *res, const char *src, const char *dst );
    virtual bool            FileRename( Resource *res, const char *src, const char *dst );
    virtual size_t          FileSize( Resource *res, const char *path );
    virtual bool            FileExists( Resource *res, const char *path );
    virtual bool            FileDelete( Resource *res, const char *path );

    // Share this value with all deriviates
    static CFileTranslator *resFileRoot;

protected:
    typedef std::map <filePath, Resource*> resNameMap_t;

    resourceList_t  m_resources;
    resNameMap_t    m_resByName;
};

#endif //_BASE_RESOURCE_MANAGER_
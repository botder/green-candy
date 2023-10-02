/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResource.h
*  PURPOSE:     Resource handler class
*  DEVELOPERS:  Ed Lyons <>
*               Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               Jax <>
*               Cecill Etheredge <>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCE_H
#define CRESOURCE_H

#include "packets/CResourceStartPacket.h"
#include "packets/CResourceStopPacket.h"
#include "packets/CEntityRemovePacket.h"

#include "CResourceFile.h"
#include "CResourceManager.h"
#include "CElementGroup.h"
#include "ehs/ehs.h"
#include <pthread.h>

struct SVersion
{
    unsigned int m_uiMajor;
    unsigned int m_uiMinor;
    unsigned int m_uiRevision;
    //unsigned int m_uiState;
};

class CExportedFunction
{
public:
    enum eExportedFunctionType
    {
        EXPORTED_FUNCTION_TYPE_CLIENT,
        EXPORTED_FUNCTION_TYPE_SERVER
    };

private:
    std::string                     m_strFunctionName;
    vector<std::string>             m_accessList;
    bool                            m_bHTTPAccess;
    bool                            m_bRestricted;

    eExportedFunctionType           m_ucType;

public:
    CExportedFunction( const std::string& strFunctionName, const std::string& access, bool bHTTPAccess, eExportedFunctionType ucType, bool bRestricted )
    {
        m_ucType = ucType;
        m_strFunctionName = strFunctionName;
        m_bHTTPAccess = bHTTPAccess;
        m_bRestricted = bRestricted;

        CommandlineTools::strsplit( access.c_str(), m_accessList, ',' );
    }

    inline eExportedFunctionType    GetType ( void ) { return m_ucType; }
    inline const std::string&       GetFunctionName ( void ) { return m_strFunctionName; }
    inline bool                     IsHTTPAccessible ( void ) { return m_bHTTPAccess; }
    inline bool                     IsRestricted ( void ) { return m_bRestricted; };
    inline bool                     IsOnAccessList ( char * strResourceName )
    {
        if ( m_accessList.size() == 0 )
            return false;
        vector < std::string > ::iterator iter = m_accessList.begin ();
        for ( ; iter != m_accessList.end (); iter++ )
        {
            if ( (*iter).compare ( "*" ) == 0 || (*iter).compare ( strResourceName ) == 0 )
                return true;
        }
        return false;
    }
};

class CIncludedResources 
{
private:
    std::string             m_strResourceName;
    unsigned int            m_uiMinimumVersion;
    unsigned int            m_uiMaximumVersion;
    SVersion                m_MinVersion;
    SVersion                m_MaxVersion;
    bool                    m_bExists;
    bool                    m_bBadVersion;
    class CResource *       m_resource; // the resource this links to
    class CResource *       m_owner; // the resource this is inside
    class CResourceManager *      m_resourceManager;

public:
    CIncludedResources ( CResourceManager * manager, const std::string& strResourceName, SVersion svMinVersion, SVersion svMaxVersion, unsigned int uiMinVersion, unsigned int uiMaxVersion, CResource * owner )
    {
        m_resourceManager = manager;
        m_owner = owner;
        m_strResourceName = strResourceName;
        m_MinVersion = svMinVersion;
        m_MaxVersion = svMaxVersion;
        m_uiMinimumVersion = uiMinVersion;
        m_uiMaximumVersion = uiMaxVersion;
        m_bExists = false;
        m_resource = NULL;
    }

    inline const std::string&   GetName             ()    { return m_strResourceName; }
    int                         GetMinimumVersion   ()    { return m_uiMinimumVersion; }
    int                         GetMaximumVersion   ()    { return m_uiMaximumVersion; }
    inline SVersion*            GetMinVersion       ()    { return &m_MinVersion; };
    inline SVersion*            GetMaxVersion       ()    { return &m_MaxVersion; };
    inline bool                 DoesExist           () { return m_bExists; }
    inline bool                 IsBadVersion        () { return m_bBadVersion; }
    inline CResource *          GetResource         () { return m_resource; }
    bool                        CreateLink          ();
    inline void                 InvalidateReference () { m_resource = NULL; m_bExists = false; };
};

class CInfoValue
{
private:
    std::string m_strName;
    std::string m_strValue;

public:
    CInfoValue ( const std::string& strName, const std::string& strValue )
    {
        m_strName = strName;
        m_strValue = strValue;
    }

    ~CInfoValue ()
    {
    }

    inline const std::string&   GetValue ()                             { return m_strValue; }
    inline void                 SetValue ( const std::string& szValue ) { m_strValue = szValue; }
    inline const std::string&   GetName  ()                             { return m_strName; }
};

class CResource : public EHS, public Resource
{
private:
    bool                    m_bHandlingHTTPRequest;

    CFileTranslator*        m_cacheRoot;                // Cached access zone
    CFileTranslator*        m_archiveRoot;              // Archived content

    // DEPRECATED
    bool                    m_isZip;
    filePath                m_zipPath;

    unsigned int            m_uiVersionMajor;
    unsigned int            m_uiVersionMinor;
    unsigned int            m_uiVersionRevision;
    unsigned int            m_uiVersionState;

    time_t                  m_timeLoaded;
    time_t                  m_timeStarted;

    class CElement*         m_pRootElement;
    class CDummy*           m_pResourceElement;
    class CDummy*           m_pResourceDynamicElementRoot;

    typedef std::list <CInfoValue*> infoList_t;
    typedef std::list <CIncludedResources*> includes_t;
    typedef std::list <CResourceFile*> fileList_t;
    typedef std::list <CResource*> resourceList_t;
    typedef std::list <CElementGroup*> groupList_t;
    typedef std::list <CExportedFunction*> exports_t;

    infoList_t              m_infoValues;
    includes_t              m_includedResources; // we store them here temporarily, then read them once all the resources are loaded
    fileList_t              m_resourceFiles;
    resourceList_t          m_dependents; // resources that have "included" or loaded this one
    groupList_t             m_elementGroups; // stores elements created by scripts in this resource
    exports_t               m_exportedFunctions;
    resourceList_t          m_temporaryIncludes; // started by startResource script command
    
    CElementGroup*          m_pDefaultElementGroup;
    
    std::string             m_strCircularInclude;
    bool                    m_bLoaded; // its been loaded successfully (i.e. meta parsed ok), included resources loaded ok
    bool                    m_bActive; // resource items are running
    bool                    m_bStarting; // the resource is starting
    bool                    m_bStopping; // the resource is stopping
    bool                    m_bIsPersistent; // if true, the resource will remain even if it has no Dependents, mainly if started by the user or the startup
    bool                    m_bLinked; // if true, the included resources are already linked to this resource
    unzFile                 m_zipfile;
    SString                 m_strFailureReason;

    bool                    m_bClientConfigs;
    bool                    m_bClientScripts;
    bool                    m_bClientFiles;
    bool                    m_bHasStarted;
    bool                    m_bDoneUpgradeWarnings;

    class CResourceManager* m_resourceManager; 

    CXMLNode*               m_pNodeSettings;        // Settings XML node, read from meta.xml and copied into it's own instance
    CXMLNode*               m_pNodeStorage;         // Dummy XML node used for temporary storage of stuff returned by CSettings::Get

    CChecksum               m_checksum; // Checksum last time this was loaded, generated by GenerateChecksum()

    bool                    m_bProtected;
    bool                    m_bStartedManually;

    void                    ThrowError( const char *msg, ... );
    bool                    CheckState();

    bool                    ReadIncludedResources( class CXMLNode * root );
    bool                    ReadIncludedMaps( CXMLNode * root );
    bool                    ReadIncludedScripts( CXMLNode * root );
    bool                    ReadIncludedConfigs( CXMLNode * root );
    bool                    ReadIncludedHTML( CXMLNode * root );
    bool                    ReadIncludedExports( CXMLNode * root );
    bool                    ReadIncludedFiles( CXMLNode * root );

    bool                    CreateVM();
    bool                    DestroyVM();
    void                    TidyUp();

    ResponseCode            HandleRequestActive( HttpRequest *request, HttpResponse *respone, class CAccount *account );
    ResponseCode            HandleRequestCall( HttpRequest *request, HttpResponse *respone, class CAccount *account );
    ResponseCode            HandleRequestInfo( HttpRequest *request, HttpResponse *respone );
    
    pthread_mutex_t         m_mutex;

    inline void             Lock()                              { pthread_mutex_lock( &m_mutex ); }
    inline void             Unlock()                            { pthread_mutex_unlock ( &m_mutex ); }
    
public:
                            CResource( CResourceManager *manager, bool zip, const char *name );
                            ~CResource();

    bool                    Load();
    bool                    Unload();
    void                    Reload();

    CXMLFile*               GrabMeta( CXMLNode*& root );

    bool                    GetDefaultSetting( const char *name, char *value, size_t sizeBuffer );
    bool                    SetDefaultSetting( const char *name, const char *value );
    bool                    RemoveDefaultSetting( const char *name );

    bool                    AddMapFile( const char *path,, int dimension );
    bool                    AddConfigFile( const char *path, int type );
    bool                    IncludedFileExists( const char *path, int type = CResourceFile::RESOURCE_FILE_TYPE_NONE );
    bool                    RemoveFile( const char *path );

    bool                    AllocateArchive();
    void                    ReleaseArchive();

    // Virtual file interface
    CFile*                  OpenStream( const char *path, const char *mode );
    bool                    FileCopy( const char *src, const char *dst );
    bool                    FileRename( const char *src, const char *dst );
    size_t                  FileSize( const char *path ) const;
    bool                    FileExists( const char *path ) const;
    bool                    FileDelete( const char *path );
    
    bool                    HasStarted()                        { return m_bHasStarted; };

    bool                    Start( list<CResource *> * dependents = NULL, bool bStartManually = false, bool bStartIncludedResources = true, bool bConfigs = true, bool bMaps = true, bool bScripts = true, bool bHTML = true, bool bClientConfigs = true, bool bClientScripts = true, bool bClientFiles = true );
    bool                    Stop( bool bStopManually = false );

    inline bool             IsClientConfigsOn()                 { return m_bClientConfigs; };
    inline bool             IsClientScriptsOn()                 { return m_bClientScripts; };
    inline bool             IsClientFilesOn()                   { return m_bClientFiles; };

    bool                    StopAllResourceItems();    

    CChecksum               GenerateChecksum();
    inline CChecksum        GetLastChecksum()                   { return m_checksum; }
    bool                    HasResourceChanged();
    void                    ApplyUpgradeModifications();

    void                    AddTemporaryInclude( CResource * resource );
    const std::string&      GetFailureReason()                  { return m_strFailureReason; }
    inline CXMLNode*        GetSettingsNode()                   { return m_pNodeSettings; }
    inline CXMLNode*        GetStorageNode()                    { return m_pNodeStorage; }

    bool                    CallExportedFunction( char *name, CLuaArguments& args, CLuaArguments& returns, CResource& caller );

    inline const resourceList_t&  GetDependents()               { return m_dependents; }
    inline size_t           GetDependentCount()                 { return m_dependents.size(); }

    inline includes_t::iterator GetIncludedResourcesBegin()     { return m_includedResources.begin(); };
    inline includes_t::iterator GetIncludedResourcesEnd()       { return m_includedResources.end(); };
    inline int              GetIncludedResourcesCount()         { return m_includedResources.size(); };

    bool                    GetInfoValue( const char * szKey, std::string& strValue );
    void                    SetInfoValue( const char * szKey, const char * szValue );

    inline unsigned int     GetVersionMajor()                   { return m_uiVersionMajor; }
    inline unsigned int     GetVersionMinor()                   { return m_uiVersionMinor; }
    inline unsigned int     GetVersionRevision()                { return m_uiVersionRevision; }
    inline unsigned int     GetVersionState()                   { return m_uiVersionState; }

    inline bool             IsLoaded()                          { return m_bLoaded; }
    inline bool             IsStarting()                        { return m_bStarting; }
    inline bool             IsStopping()						{ return m_bStopping; }

    void                    AddDependent( CResource * resource );
    void                    RemoveDependent( CResource * resource );
    bool                    IsDependentResource( CResource * resource );
    bool                    IsDependentResource( const char * szResourceName );
    bool                    IsDependentResourceRecursive( CResource *res );
    bool                    IsDependentResourceRecursive( const char *name );
	bool                    IsIncludedResourceRecursive( std::vector <CResource*> *past );
    void                    InvalidateIncludedResourceReference( CResource *res );

    inline bool             IsPersistent()                      { return m_bIsPersistent; }
    inline void             SetPersistent( bool bPersistent )   { m_bIsPersistent = bPersistent; }

    bool                    ExtractFile( const char *path );
    bool                    DoesFileExistInZip( const char *path );
    bool                    CacheFileZip( const char *path );
    bool                    GetFilePath( const char *filename, filePath& path );

    bool                    LinkToIncludedResources();
    bool                    CheckIfStartable();
    inline unsigned int     GetFileCount()                      { return m_resourceFiles.size(); }
    void                    DisplayInfo();
    void                    OutputHTMLEntry( std::string& buf );
    char*                   DisplayInfoHTML( char *info, size_t length );

    const fileList_t&       GetFiles() const                    { return m_resourceFiles; }
    CElementGroup&          GetElementGroup()                   { return *m_pDefaultElementGroup; }

    inline time_t           GetTimeStarted()                    { return m_timeStarted; }
    inline time_t           GetTimeLoaded()                     { return m_timeLoaded; }

    void                    SetID( unsigned short id )          { m_id = id };

    void                    OnPlayerJoin( CPlayer& Player );
    CDummy*                 GetResourceRootElement()            { return m_pResourceElement; };
    CDummy*                 GetDynamicElementRoot()             { return m_pResourceDynamicElementRoot; };

    void                    SetProtected( bool protect )        { m_bProtected = protect; };
    bool                    IsProtected() const                 { return m_bProtected; };

    inline bool             IsResourceZip()                     { return m_isZip; };

    ResponseCode            HandleRequest( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse );

    inline fileList_t::iterator IterBegin()                     { return m_resourceFiles.begin(); }
    inline fileList_t::iterator IterEnd()                       { return m_resourceFiles.end(); }
    inline unsigned short       IterCount()                     { return m_resourceFiles.size(); }

    inline exports_t::iterator  IterBeginExportedFunctions()    { return m_exportedFunctions.begin(); }
    inline exports_t::iterator  IterEndExportedFunctions()      { return m_exportedFunctions.end(); }

    static list <CResource*> m_StartedResources;
};


#endif

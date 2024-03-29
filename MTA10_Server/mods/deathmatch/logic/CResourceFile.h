/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceFile.h
*  PURPOSE:     Resource server-side file item class
*  DEVELOPERS:  Ed Lyons <>
*               Jax <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class controls a single resource file. This could be
// any item contained within the resource, mainly being a
// map or script.

// This is just a base class.

class CResourceFile;

#ifndef CRESOURCEFILE_H
#define CRESOURCEFILE_H
#include "ehs/ehs.h"

class CResourceFile abstract
{
public:
    enum eResourceType
    {
        RESOURCE_FILE_TYPE_MAP,
        RESOURCE_FILE_TYPE_SCRIPT,
        RESOURCE_FILE_TYPE_CLIENT_SCRIPT,
        RESOURCE_FILE_TYPE_CONFIG,
        RESOURCE_FILE_TYPE_CLIENT_CONFIG,
        RESOURCE_FILE_TYPE_HTML,
        RESOURCE_FILE_TYPE_CLIENT_FILE,
        RESOURCE_FILE_TYPE_NONE,
    }; // TODO: sort all client-side enums and use >= (instead of each individual type) on comparisons that use this enum?

protected:
    class CResource*            m_resource;
    filePath                    m_path;
    std::string                 m_name;
    eResourceType               m_type;

    class CLuaMain*             m_pVM;
    CChecksum                   m_checksum;     // Checksum last time this was loaded, generated by GenerateChecksum()
    map <string, string>        m_attributeMap; // Map of attributes from the meta.xml file
    
public:
                                CResourceFile( class CResource *resource, const filePath& path, CXMLAttributes *attr );
    virtual                     ~CResourceFile();

    virtual ResponseCode        Request( HttpRequest *ipoHttpRequest, HttpResponse *ipoHttpResponse );

    virtual bool                Start() = 0;
    virtual bool                Stop() = 0;

    inline eResourceType        GetType() const                                     { return m_type; }
    inline const char*          GetName() const                                     { return m_name.c_str(); }

    inline const filePath&      GetPath() const                                     { return m_path; }

    inline CChecksum            GetLastChecksum() const                             { return m_checksum; }
    void                        SetLastChecksum( const CChecksum& checksum )        { m_checksum = checksum; }

    virtual void                OutputHTMLEntry( std::string& buf );

    double                      GetApproxSize();    // Only used by download counters
    string                      GetMetaFileAttribute( const string& key )           { return m_attributeMap[key]; }
};

#endif

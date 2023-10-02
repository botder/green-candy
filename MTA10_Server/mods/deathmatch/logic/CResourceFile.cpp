/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceFile.cpp
*  PURPOSE:     Resource server-side file
*  DEVELOPERS:  Ed Lyons <>
*               Jax <>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CResourceFile::CResourceFile( CResource *resource, const char *name, const filePath& path, CXMLAttributes *attr ) 
{ 
    m_path = path;
    m_name = name;

    m_resource = resource;
    m_pVM = NULL;

    // Create a map of the attributes for later use
    if ( !attr )
        return;

    for ( list <CXMLAttribute*>::iterator iter = attr->ListBegin(); iter != attr->ListEnd(); iter++ )
        m_attributeMap[(*iter)->GetName()] = (*iter)->GetValue();
}

CResourceFile::~CResourceFile()
{
}

double CResourceFile::GetApproxSize()
{
    return g_pServerInterface->GetFileSystem()->Size( m_path.c_str() );
}

ResponseCode CResourceFile::Request( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse )
{
    // HACK - Use http-client-files if possible as the resources directory may have been changed since the resource was loaded.
    filePath dst;

    CFile *file = resCacheFileRoot->Open( SString( "http-client-files/", 18 ) + m_resource->GetName() + "/" + GetName(), "rb" );
    if ( !file )
        file = resCacheFileRoot->Open( m_path.c_str() );

    // its a raw page
    if ( !file )
    {
        ipoHttpResponse->SetBody( "Can't read file!", 16 );
        return HTTPRESPONSECODE_500_INTERNALSERVERERROR;
    }

    size_t size = file->GetSize();
    char *buf = (char*)malloc( size );

    if ( file->Read( buf, size, 1 ) != 1 )
        return HTTPRESPONSECODE_INVALID;

    delete file;

    ipoHttpResponse->oResponseHeaders["content-type"] = "application/octet-stream"; // not really the right mime-type
    ipoHttpResponse->SetBody( buf, size );
    
    free( buf );
    return HTTPRESPONSECODE_200_OK;
}

void CResourceFile::OutputHTMLEntry( std::string& buf )
{
    buf += m_name;
}
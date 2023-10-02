/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        xml/CXMLImpl.cpp
*  PURPOSE:     XML handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "xml"
#include <sys/stat.h>

CXMLImpl::CXMLImpl ( void )
{
    // Init array stuff
    CXMLArray::Initialize ();
}

CXMLImpl::~CXMLImpl ( void )
{
}

CXMLFile* CXMLImpl::CreateXML( const char* szFilename, bool bUseIDs )
{
    CXMLFile* xmlFile = new CXMLFileImpl ( szFilename, bUseIDs );

#ifdef _DEBUG
    // We usually want absolute paths here
    assert( szFilename[1] == ':' );
#endif

    if ( !xmlFile->IsValid() )
    {
        delete xmlFile;

        return NULL;
    }

    return xmlFile;
}

void CXMLImpl::DeleteXML ( CXMLFile* pFile )
{
    delete pFile;
}

CXMLNode* CXMLImpl::CreateDummyNode ( void )
{
    CXMLNode* xmlNode = new CXMLNodeImpl ( NULL, NULL, *new TiXmlElement ( "dummy_storage" ) );

    if ( !xmlNode->IsValid( ) )
    {
        delete xmlNode;

        return NULL;
    }

    return xmlNode;
}

CXMLAttribute* CXMLImpl::GetAttrFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );

    if ( pCommon && pCommon->GetClassType () == CXML_ATTR )
    {
        return reinterpret_cast < CXMLAttribute* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}

CXMLFile* CXMLImpl::GetFileFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );

    if ( pCommon && pCommon->GetClassType () == CXML_FILE )
    {
        return reinterpret_cast < CXMLFile* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}

CXMLNode* CXMLImpl::GetNodeFromID ( unsigned long ulID )
{
    // Grab it and verify the type
    CXMLCommon* pCommon = CXMLArray::GetEntry ( ulID );

    if ( pCommon && pCommon->GetClassType () == CXML_NODE )
    {
        return reinterpret_cast < CXMLNode* > ( pCommon );
    }

    // Doesn't exist or bad type
    return NULL;
}

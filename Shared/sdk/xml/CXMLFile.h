/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLFile.h
*  PURPOSE:     XML file interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLFILE_H
#define __CXMLFILE_H

#include "CXMLCommon.h"
#include "CXMLErrorCodes.h"

#include <string>

class CXMLNode;

class CXMLFile: public CXMLCommon
{
public:
    virtual                         ~CXMLFile           () {};

    virtual const char*             GetFilename         () = 0;
    virtual void                    SetFilename         ( const char* szFilename ) = 0;

    virtual bool                    Parse               () = 0;
    virtual bool                    Write               () = 0;
    virtual void                    Clear               () = 0;
    virtual void                    Reset               () = 0;

    virtual CXMLNode*               CreateRootNode      ( const std::string& strTagName ) = 0;
    virtual CXMLNode*               GetRootNode         () = 0;

    virtual CXMLErrorCodes::Code    GetLastError        ( std::string& strOut ) = 0;
    virtual void                    ResetLastError      () = 0;

    virtual bool                    IsValid             () = 0;
};

#endif

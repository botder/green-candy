/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CServerInterface.h
*  PURPOSE:     Core server interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERVERINTERFACE_H
#define __CSERVERINTERFACE_H

#include "net/CNetServer.h"
#include "CTCP.h"
#include "CModManager.h"
#include <xml/CXML.h>

class CServerInterface
{
public:
    virtual CNetServer*             GetNetwork          () = 0;
    virtual CModManager*            GetModManager       () = 0;
    virtual CTCP*                   GetTCP              () = 0;
    virtual CXML*                   GetXML              () = 0;
    virtual CFileSystemInterface*   GetFileSystem       () = 0;

    virtual void                    Printf              ( const char* szFormat, ... ) = 0;
};

#endif

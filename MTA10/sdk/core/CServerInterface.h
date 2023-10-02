/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CServerInterface.h
*  PURPOSE:     Server instance interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _SERVER_INSTANCE_INTERFACE_
#define _SERVER_INSTANCE_INTERFACE_

class CServerInterface
{
public:
    virtual bool                        Start( const std::string& config ) = 0;
    virtual bool                        Stop() = 0;
    virtual bool                        IsStarted() = 0;
    virtual bool                        IsRunning() = 0;
    virtual bool                        IsReady() = 0;

    virtual int                         GetLastError() = 0;

    virtual const CFileTranslator&      GetFileRoot() const = 0;

    virtual bool                        Send( const char *cmd ) = 0;
};

#endif //_SERVER_INSTANCE_INTERFACE_
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CServerBase.h
*  PURPOSE:     Core server base interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERVERBASE_H
#define __CSERVERBASE_H

#include <core/interface.h>

#include "CServerInterface.h"

class CServerBase
{
public:
    virtual void        ServerInitialize            ( CServerInterface* pServer ) = 0;
    virtual bool        ServerStartup               ( int iArgumentCount, char* szArguments [] ) = 0;
    virtual void        ServerShutdown              () = 0;

    virtual void        DoPulse                     () = 0;

    virtual void        HandleInput                 ( char* szCommand ) = 0;
    virtual void        GetTag                      ( char* szInfoTag, int iInfoTag ) = 0;

    virtual bool        IsFinished                  () = 0;
    virtual bool        PendingWorkToDo             () = 0;
};

#endif


/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/CClient.h
*  PURPOSE:     Header file for Client class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oliver Brown <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENT_H
#define __CCLIENT_H

#include <core/CClientBase.h>

class CClient : public CClientBase
{
public:
    int                 ClientInitialize( const char* szArguments, CCoreInterface* pCore );
    void                ClientShutdown();

    // Protective measures for Lua scripting
    void                BeginGUI();
    void                EndGUI();

    void                PreFrameExecutionHandler();
    void                PreHUDRenderExecutionHandler( bool bDidUnminimize, bool bDidRecreateRenderTargets );
    void                PostFrameExecutionHandler();

    bool                ProcessCommand( const char *szCommandLine );

    bool                HandleException( CExceptionInformation* pExceptionInformation );
};

extern CFileTranslator *modFileRoot;
extern CFileTranslator *mtaFileRoot;

#endif
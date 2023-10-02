/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManagerImpl.h
*  PURPOSE:     Mod manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oli <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CModManagerImpl;

#ifndef __CMODMANAGERIMPL_H
#define __CMODMANAGERIMPL_H

#include <core/CModManager.h>
#include <core/CServerBase.h>

#include "CServerImpl.h"

typedef CServerBase* (InitServer)();

class CModManagerImpl : public CModManager
{
public:
                        CModManagerImpl( CServerImpl* pServer );
                        ~CModManagerImpl();

    bool                RequestLoad( const char* szModName );

    CFileTranslator*    GetModRoot()                                            { return m_modRoot; };

    bool                IsModLoaded();
    CServerBase*        GetCurrentMod();

    bool                Load( const char* szModName, int iArgumentCount, char* szArguments [] );
    void                Unload();

    void                HandleInput( const char* szCommand );
    void                GetTag( char* szInfoTag, int iInfoTag );

    void                DoPulse();

    bool                IsFinished();

    bool                PendingWorkToDo();

private:
    CServerImpl*        m_pServer;

    CServerBase*        m_pBase;
    CDynamicLibrary     m_Library;

    filePath            m_modPath;
    CFileTranslator*    m_modRoot;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerImpl.h
*  PURPOSE:     Server class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Oli <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerImpl;

#ifndef __CSERVERIMPL_H
#define __CSERVERIMPL_H

#include "MTAPlatform.h"
#include <string>

#include <core/CServerInterface.h>
#include <net/CNetServer.h>

#include "CModManagerImpl.h"
#include "sockets/CTCPImpl.h"
#include "sockets/CTCPServerSocketImplManager.h"
#include <xml/CXML.h>
#include "CThreadCommandQueue.h"

#ifndef WIN32
#include <ncursesw/curses.h>
#endif

#define SERVER_RESET_RETURN 500

typedef CXML* (*InitXMLInterface) ();
typedef CNetServer* (*InitNetServerInterface) ();

#ifdef WIN32
typedef void (FClientFeedback) ( const char* szText );
#endif

class CServerImpl : public CServerInterface
{
public:
#ifdef WIN32
                        CServerImpl( CThreadCommandQueue* pThreadCommandQueue );
#else
                        CServerImpl();
#endif

                        ~CServerImpl();

    CNetServer*         GetNetwork();
    CModManager*        GetModManager();
    CTCP*               GetTCP();
    CXML*               GetXML();
    CFileSystem*        GetFileSystem()                         { return m_fileSystem; }

    void                Printf( const char* szText, ... );

    int                 Run( int iArgumentCount, char* szArguments [] );
#ifndef WIN32
    void                Daemonize() const;
#endif

private:
    void                MainLoop();

    bool                ParseArguments( int iArgumentCount, char* szArguments [] );
    bool                IsKeyPressed( int iKey );
    void                WaitForKey( int iKey );

    void                ShowInfoTag( char *szTag );
    void                HandleInput();

    void                SleepMs( unsigned long ulMs );
    void                DestroyWindow();

    CDynamicLibrary     m_NetworkLibrary;
    CDynamicLibrary     m_XMLLibrary;
    CNetServer*         m_pNetwork;
    CModManagerImpl*    m_pModManager;
    CTCPImpl*           m_pTCP;
    CXML*               m_pXML;
    CFileSystem*        m_fileSystem;

#ifdef WIN32
    FClientFeedback*    m_fClientFeedback;
#endif

    bool                m_bRequestedQuit;
    bool                m_bRequestedReset;

    wchar_t             m_szInputBuffer[255];
    unsigned int        m_uiInputCount;
    
    char                m_szTag[80];

#ifdef WIN32
    HANDLE              m_hConsole;
    CHAR_INFO           m_ScrnBuffer[256];

    CThreadCommandQueue*    m_pThreadCommandQueue;
#else
    WINDOW*             m_wndMenu;
    WINDOW*             m_wndInput;
#endif
};

extern CFileTranslator *modFileRoot;

#endif

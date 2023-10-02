/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServer.h
*  PURPOSE:     Header for server class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSERVER_H
#define __CSERVER_H

class CServer : public CServerInterface
{
public:
                                CServer( const filePath& mtaRoot );
                                ~CServer();

    void                        DoPulse();

    bool                        Start( const std::string& config );
    bool                        Stop();
    bool                        IsStarted();
    inline bool                 IsRunning()                             { return m_lib != NULL; };
    inline bool                 IsReady()                               { return m_ready; };

    int                         GetLastError()                          { return m_lastError; };

    const CFileTranslator&      GetFileRoot() const                     { return *m_serverRoot; };

    bool                        Send( const char *cmd );

private:
    static DWORD WINAPI         Thread_EntryPoint( LPVOID pThis );
    unsigned long               Thread_Run();

    CFileTranslator*            m_serverRoot;

    bool                        m_ready;
    HANDLE                      m_thread;
    CDynamicLibrary* volatile   m_lib;
    CCriticalSection            m_criticalSection;

    std::string                 m_config;

    int                         m_lastError;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServer.cpp
*  PURPOSE:     Local server instancing class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static volatile bool isStarted = false;
extern CCore* g_pCore;

static CCriticalSection         outputCC;
static std::list <std::string>  outputQueue;

#ifdef MTA_DEBUG
#define SERVER_DLL_PATH "core_d.dll"
#else
#define SERVER_DLL_PATH "core.dll"
#endif

#define ERROR_NO_ERROR 0
#define ERROR_NO_NETWORK_LIBRARY 1
#define ERROR_NETWORK_LIBRARY_FAILED 2
#define ERROR_LOADING_MOD 3

char* szServerErrors[4] =
{
    "Server stopped",
    "Could not load network library",
    "Loading network library failed",
    "Error loading mod"
};

CServer::CServer( const filePath& mtaRoot )
{
    // Initialize
    m_ready = false;
    m_lib = NULL;
    m_thread = INVALID_HANDLE_VALUE;
    m_lastError = ERROR_NO_ERROR;

    // Create the server root translator
    filePath rootPath = mtaRoot + "server/";

    m_serverRoot = g_pCore->GetFileSystem()->CreateTranslator( rootPath.c_str() );
}

CServer::~CServer()
{
    Stop();
}

void CServer::DoPulse()
{
    if ( !IsRunning() )
    {
        // not running, errored?
        if ( GetLastError() != ERROR_NO_ERROR )
        {
            Stop ();
        }
    }
    
    // Make sure the server doesn't happen to be adding anything right now
    outputCC.Lock();

    // Anything to output to console?
    if ( outputQueue.size() > 0 )
    {
        // Loop through our output queue and echo it to console
        std::list <std::string>::const_iterator iter = outputQueue.begin();

        for ( ; iter != outputQueue.end(); ++iter )
        {
            // Echo it
            const char *echo = iter->c_str();
            g_pCore->GetConsole()->Echo( echo );

            // Does the message end with "Server started and is ready to accept connections!\n"?
            size_t size = iter->length();

            if ( size >= 51 &&
                stricmp( echo + size - 51, "Server started and is ready to accept connections!\n" ) == 0 )
            {
                m_ready = true;
            }
        }

        // Clear the list
        outputQueue.clear();
    }

    // Unlock again
    outputCC.Unlock();
}

bool CServer::Start( const std::string& config )
{
    if ( isStarted )
        return false;

    m_config = config;

    // Check that the DLL exists
    if ( !m_serverRoot->Exists( SERVER_DLL_PATH ) )
    {
        g_pCore->GetConsole()->Printf( "Unable to find: '%s'", SERVER_DLL_PATH );
        return false;
    }

    isStarted = true;
    m_ready = false;

    // Close the previous thread?
    if ( m_thread != INVALID_HANDLE_VALUE )
        CloseHandle( m_thread );

    // Create a thread to run the server
    DWORD temp;
    m_thread = CreateThread( NULL, 0, Thread_EntryPoint, this, 0, &temp );
    return m_thread != INVALID_HANDLE_VALUE;
}

bool CServer::IsStarted()
{
    return isStarted;
}

bool CServer::Stop()
{
    // Started?
    if ( isStarted )
    {
        // Wait for the library to come true or is started to go false
        // This is so a call to Start then fast call to Stop will work. Otherwize it might not
        // get time to start the server thread before we terminate it and we will end up
        // starting it after this call to Stop.
        while ( isStarted && !m_lib )
            Sleep( 1 );

        // Lock
        m_criticalSection.Lock();

        // Is the server running?
        if ( m_lib )
        {
            // Send the exit message
            Send( "exit" );
        }

        // Unlock it so we won't deadlock
        m_criticalSection.Unlock();
    }

    // If we have a thread, wait for it to finish
    if ( m_thread != INVALID_HANDLE_VALUE )
    {
        // Let the thread finish
        WaitForSingleObject( m_thread, INFINITE );

        // If we can get an exit code, see if it's non-zero
        DWORD exitCode = 0;

        if ( GetExitCodeThread( m_thread, &exitCode ) )
        {
            // Non-zero, output error
            if ( exitCode != 0 )
            {
                g_pCore->ShowMessageBox( "Error", "Could not start the local server. See console for details.", MB_BUTTON_OK | MB_ICON_ERROR );
                g_pCore->GetConsole()->Printf( "Error: Could not start local server. [%s]", szServerErrors[ GetLastError() ] );
            }
        }

        // Close it
        CloseHandle( m_thread );
        m_thread = INVALID_HANDLE_VALUE;
    }

    m_lastError = ERROR_NO_ERROR;
    return true;
}

bool CServer::Send( const char *cmd )
{
    // Server running?
    bool ret = false;

    if ( isStarted )
    {
        // Wait for the library to come true or is started to go false
        while ( isStarted && !m_lib )
            Sleep( 1 );

        // Lock
        m_criticalSection.Lock();

        // Are we running the server
        if ( m_lib )
        {
            // Grab the SendServerCommand function pointer
            typedef bool ( SendServerCommand_t )( const char* );
            SendServerCommand_t *pfnSendServerCommand = (SendServerCommand_t*)m_lib->GetProcedureAddress( "SendServerCommand" );

            if ( pfnSendServerCommand )
            {
                // Call it with the command
                ret = pfnSendServerCommand( cmd );
            }
        }

        // Unlock
        m_criticalSection.Unlock ();
    }

    // Return
    return ret;
}


DWORD WINAPI CServer::Thread_EntryPoint ( LPVOID pThis )
{
    return reinterpret_cast < CServer* > ( pThis )->Thread_Run ();
}

static void addServerOutput( const char *msg )
{
    // Make sure the client doesn't process the queue right now
    outputCC.Lock();

    // Add the string to the queue
    outputQueue.push_back( msg );

    outputCC.Unlock();
}

unsigned long CServer::Thread_Run()
{
    // Enter critical section
    m_criticalSection.Lock();

    if ( m_lib )
    {
        m_criticalSection.Unlock();
        return 0;
    }

    filePath path;
    m_serverRoot->GetFullPath( "/" SERVER_DLL_PATH, true, path );

    // Load the DLL
    m_lib = new CDynamicLibrary;
    if ( m_lib->Load( path.c_str() ) )
    {
        // Grab the entrypoint
        typedef int (Main_t)( int, const char* [] );
        Main_t* pfnEntryPoint = (Main_t*)m_lib->GetProcedureAddress( "Run" );

        if ( pfnEntryPoint )
        {
            // Populate the arguments array
            const char *args[7];

            filePath rootDir;
            m_serverRoot->GetFullPathFromRoot( "/", false, rootDir );

            args[0] = "-D";
            args[1] = rootDir.c_str();
            args[2] = "--config";
            args[3] = m_config.c_str();
            args[4] = "-s";

            // Give the server our function to output stuff to the console
            args[5] = "--clientfeedback";
            args[6] = (const char*)addServerOutput;

            // We're now running the server
            m_criticalSection.Unlock();

            // Call it and grab what it returned
            m_lastError = pfnEntryPoint( 7, args );

            // Lock again
            m_criticalSection.Lock();
        }
        else
            m_lastError = 1;
    }

    // Delete the library
    delete m_lib;
    m_lib = NULL;

    m_criticalSection.Unlock();
    isStarted = false;
    return m_lastError;
}
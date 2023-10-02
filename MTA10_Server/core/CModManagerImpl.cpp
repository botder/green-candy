/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CModManagerImpl.cpp
*  PURPOSE:     Mod manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Oli <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CModManagerImpl.h"
#include "MTAPlatform.h"
#include <cstdio>

using namespace std;

CModManagerImpl::CModManagerImpl( CServerImpl* pServer )
{
    // Init
    m_pServer = pServer;
    m_pBase = NULL;
}

CModManagerImpl::~CModManagerImpl()
{
    // Make sure the mod is unloaded
    Unload ();
}

bool CModManagerImpl::RequestLoad ( const char* szModName )
{
    // TODO
    return false;
}

bool CModManagerImpl::IsModLoaded()
{
    return m_pBase != NULL;
}

CServerBase* CModManagerImpl::GetCurrentMod()
{
    return m_pBase;
}

bool CModManagerImpl::Load( const char *szModName, int iArgumentCount, char* szArguments [] )
{
    filePath path;
    
    if ( !modFileRoot->GetFullPath( szModName, false, path ) )
        return false;

    path += szModName;

#if defined(_WIN32)
#ifdef _DEBUG
    path += "_d.dll";
#else
    path += ".dll";
#endif //_DEBUG
#else
    path += ".so";
#endif

    // Attempt to load it
    if ( !m_Library.Load( path.c_str() ) )
    {
        // Failed
        Print( "\nERROR: Loading mod (%s) failed!\n", path.c_str() );
        return false;
    }

    // Set mod file root
    m_modRoot = m_pServer->GetFileSystem()->CreateTranslator( path.c_str() );

    // Grab the initialization procedure
    InitServer* pfnInitServer = (InitServer*)m_Library.GetProcedureAddress( "InitServer" );

    if ( !pfnInitServer )
    {
        // Unload the library
        m_Library.Unload();

        // Report the error
        Print( "\nERROR: Bad file: %s!\n", path.c_str() );
        return false;
    }

    // Call the InitServer procedure to get the interface
    m_pBase = pfnInitServer();

    if ( !m_pBase )
    {
        // Unload the library
        m_Library.Unload();

        // Report the error
        Print( "\nERROR: Failed initializing '%s'!\n", path.c_str() );
        return false;
    }

    // Call the initialization procedure in the interface
    m_pBase->ServerInitialize( m_pServer );

    // Start the mod up
    if ( !m_pBase->ServerStartup( iArgumentCount, szArguments ) )
    {
        // Unload the mod again
        Unload ();
        return false;
    }

    // Success
    return true;
}


void CModManagerImpl::Unload ( void )
{
    // Got a mod loaded?
    if ( m_pBase )
    {
        // Call the mod's shutdown procedure
        m_pBase->ServerShutdown ();
        m_pBase = NULL;

        // Unload the library
        m_Library.Unload ();
    }
}


void CModManagerImpl::DoPulse ( void )
{
    // Got a mod loaded?
    if ( m_pBase )
    {
        // Pulse the mod
        m_pBase->DoPulse ();
    }
}


bool CModManagerImpl::IsFinished ( void )
{
    if ( m_pBase )
    {
        return m_pBase->IsFinished ();
    }

    return true;
}

bool CModManagerImpl::PendingWorkToDo ( void )
{
    if ( m_pBase )
    {
        return m_pBase->PendingWorkToDo ();
    }

    return false;
}


void CModManagerImpl::GetTag ( char *szInfoTag, int iInfoTag )
{
    if ( m_pBase )
    {
        m_pBase->GetTag ( szInfoTag, iInfoTag );
    }
}


void CModManagerImpl::HandleInput ( const char* szCommand )
{
    if ( m_pBase )
    {
        m_pBase->HandleInput ( (char*)szCommand );
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/CServer.cpp
*  PURPOSE:     Server interface handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define ALLOC_STATS_MODULE_NAME "deathmatch"
#include "SharedUtil.hpp"

CServerInterface *g_pServerInterface = NULL;
CNetServer *g_pNetServer = NULL;

CFileTranslator *modFileRoot;

CServer::CServer()
{
    // Init
    m_pServerInterface = NULL;
    m_pGame = NULL;

    // Grab the file root for us
    modFileRoot = g_pServerInterface->GetModManager()->GetModRoot();
}

CServer::~CServer()
{
}

void CServer::ServerInitialize( CServerInterface* pServerInterface )
{
    m_pServerInterface = pServerInterface;
    g_pServerInterface = pServerInterface;
    g_pNetServer = pServerInterface->GetNetwork ();
}

bool CServer::ServerStartup( int iArgumentCount, char* szArguments [] )
{
    if ( !m_pGame )
    {
        m_pGame = new CGame;
        return m_pGame->Start ( iArgumentCount, szArguments );
    }

    return false;
}

void CServer::ServerShutdown()
{
    if ( m_pGame )
    {
        delete m_pGame;
        m_pGame = NULL;
    }
}

void CServer::GetTag( char* szInfoTag, int iInfoTag )
{
    if ( !m_pGame )
        return;

    m_pGame->GetTag ( szInfoTag, iInfoTag );
}

void CServer::HandleInput( char* szCommand )
{
    if ( !m_pGame )
        return;

    m_pGame->HandleInput ( szCommand );
}

void CServer::DoPulse()
{
    if ( !m_pGame )
        return;

    m_pGame->DoPulse ();
}

bool CServer::IsFinished()
{
    return m_pGame && m_pGame->IsFinished();
}

bool CServer::PendingWorkToDo()
{
    return g_pNetServer->GetPendingPacketCount() != 0;
}
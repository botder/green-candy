/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEvent.cpp
*  PURPOSE:     Map event class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Chris McArthur <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CMapEvent::CMapEvent( CLuaMain* pMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated )
{
    // Init
    m_pMain = pMain;
    m_bDestroyFunction = false;
    m_iLuaFunction = iLuaFunction;
    m_bPropagated = bPropagated;
    m_bBeingDestroyed = false;

    strncpy ( m_szName, szName, MAPEVENT_MAX_LENGTH_NAME );
    m_szName [ MAPEVENT_MAX_LENGTH_NAME ] = '\0';
}

CMapEvent::~CMapEvent()
{
}

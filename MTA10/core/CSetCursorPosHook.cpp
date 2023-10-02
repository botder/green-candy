/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSetCursorPosHook.cpp
*  PURPOSE:     Cursor position setting hook
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "deviare_inproc/Include/NktHookLib.h"

extern CNktHookLib g_hookingLib;

template<> CSetCursorPosHook * CSingleton< CSetCursorPosHook >::m_pSingleton = NULL;

CSetCursorPosHook::CSetCursorPosHook ( void ) 
{
    WriteDebugEvent ( "CSetCursorPosHook::CSetCursorPosHook" );

    m_bCanCall          = true;
    m_pfnSetCursorPos   = NULL;
}


CSetCursorPosHook::~CSetCursorPosHook ( void ) 
{
    WriteDebugEvent ( "CSetCursorPosHook::~CSetCursorPosHook" );

    if ( m_pfnSetCursorPos != NULL )
    {
        RemoveHook ( );
    }
}

void CSetCursorPosHook::ApplyHook ( void ) 
{
    // Hook SetCursorPos
    g_hookingLib.Hook( &m_SetCursorPos_hookid, (LPVOID*)&m_SetCursorPos_hookid, GetProcAddress( GetModuleHandleW( L"User32.dll" ), "SetCursorPos" ), API_SetCursorPos );
}


void CSetCursorPosHook::RemoveHook ( void ) 
{
    // Remove hook
    if ( m_pfnSetCursorPos )
    {
        g_hookingLib.Unhook( m_SetCursorPos_hookid );

        m_pfnSetCursorPos = NULL;
    }

    // Reset variables
    m_pfnSetCursorPos = NULL;
    m_bCanCall = true;
}


void CSetCursorPosHook::DisableSetCursorPos ( void ) 
{
    m_bCanCall = false;
}


void CSetCursorPosHook::EnableSetCursorPos ( void ) 
{
    m_bCanCall = true;
}


bool CSetCursorPosHook::IsSetCursorPosEnabled ( void ) 
{
    return m_bCanCall;
}


BOOL CSetCursorPosHook::CallSetCursorPos ( int X, int Y ) 
{
    if ( m_pfnSetCursorPos == NULL )
    {
        // We should never get here, but if we do, attempt to call 
        // an imported SetCursorPos.
        return SetCursorPos ( X, Y );
    }
    else
    {
        return m_pfnSetCursorPos ( X, Y );
    }
}

BOOL WINAPI CSetCursorPosHook::API_SetCursorPos ( int X, int Y ) 
{
    CSetCursorPosHook * pThis;

    // Get self-pointer.
    pThis = CSetCursorPosHook::GetSingletonPtr ( );

    // Check to see if this function should be called properly.
    if ( (pThis->m_bCanCall) && (pThis->m_pfnSetCursorPos != NULL) )
    {
        return pThis->m_pfnSetCursorPos ( X, Y );
    }
    
    return FALSE;
}

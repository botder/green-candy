/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirectInputHook8.cpp
*  PURPOSE:     Function hooker for DirectInput 8
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "deviare_inproc/Include/NktHookLib.h"

extern CNktHookLib g_hookingLib;

const GUID IID_IDirectInputW_local = { 0x89521361,0xAA8A,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00 };

template<> CDirectInputHook8 * CSingleton< CDirectInputHook8 >::m_pSingleton = NULL;

CDirectInputHook8::CDirectInputHook8 ( )
{
    WriteDebugEvent ( "CDirectInputHook8::CDirectInputHook8" );

    // Set our creation function pointer.
    m_pfnDirectInputCreate  = NULL;
    m_bIsUnicodeInterface   = false;
}

CDirectInputHook8::~CDirectInputHook8 ( )
{
    WriteDebugEvent ( "CDirectInputHook8::~CDirectInputHook8" );
}

HRESULT CDirectInputHook8::API_DirectInput8Create  ( HINSTANCE  hinst,
                                                     DWORD      dwVersion,
                                                     REFIID     riidltf,
                                                     LPVOID*    ppvOut,
                                                     LPUNKNOWN  punkOuter )
{
    CDirectInputHook8 *    pThis;
    HRESULT                hResult;

    // Get our self instance.
    pThis = CDirectInputHook8::GetSingletonPtr ( );

    // Create our interface.
    hResult = pThis->m_pfnDirectInputCreate ( hinst, dwVersion, riidltf, ppvOut, punkOuter );

    // See if it is unicode or ansi.
    if ( riidltf == IID_IDirectInputW_local ) 
    {
        WriteDebugEvent ( "DirectInput8 UNICODE Interface Created." );

        pThis->m_bIsUnicodeInterface = true;
    }
    else
    {
        WriteDebugEvent ( "DirectInput8 ANSI Interface Created." );

        pThis->m_bIsUnicodeInterface = false;
    }

    // Save the real interface temporarily.
    pThis->m_pDevice = static_cast < IUnknown * > ( *ppvOut );

    // Give the caller a proxy interface.
    *ppvOut = new CProxyDirectInput8 ( static_cast < IDirectInput8 * > ( pThis->m_pDevice ) );

    return hResult;
}

bool CDirectInputHook8::ApplyHook ( )
{
    // Hook DirectInput8Create.
    g_hookingLib.Hook( &m_DirectInputCreate_hookid, (LPVOID*)&m_pfnDirectInputCreate, GetProcAddress( GetModuleHandleW( L"DINPUT8.DLL" ), "DirectInput8Create" ), API_DirectInput8Create );

    return true;
}

bool CDirectInputHook8::RemoveHook ( )
{
    // Make sure we should be doing this.
    if ( m_pfnDirectInputCreate != NULL )
    {
        // Unhook Direct3DCreate9.
        g_hookingLib.Unhook( m_DirectInputCreate_hookid );

        // Unset our hook variable.
        m_pfnDirectInputCreate = NULL;
    }
    return true;
}

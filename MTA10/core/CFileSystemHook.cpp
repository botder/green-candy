/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemHook.cpp
*  PURPOSE:     Detouring and hooking of file system functions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "deviare_inproc/Include/NktHookLib.h"

extern CNktHookLib g_hookingLib;

using std::string;

/*
#define _UNICODE
#include <tchar.h>

#include "CFileSystemHook.h"
#include "CDirect3DHook9.h"
#include "CDirectInputHook8.h"
#include "CLogger.h"
#include <shlwapi.h>
#include <tlhelp32.h>
#include <d3d9.h>
*/

template<> CFileSystemHook * CSingleton< CFileSystemHook >::m_pSingleton = NULL;

CFileSystemHook::CFileSystemHook ( )
{
    m_pMemoryHandleManager = new CFileSystemMemoryHandleManager;

    WriteDebugEvent ( "CFileSystemHook::CFileSystemHook" );
}

CFileSystemHook::~CFileSystemHook ( )
{
    delete m_pMemoryHandleManager;

    RemoveHook ( );

    WriteDebugEvent ( "CFileSystemHook::~CFileSystemHook" );
}

CFileSystemMemoryHandleManager* CFileSystemHook::GetFileSystemMemoryHandleManager ( void )
{
    return m_pMemoryHandleManager;
}

void CFileSystemHook::ApplyHook ( )
{
    // Hook createfile.
    g_hookingLib.Hook( &m_CreateFileA_hookid, (LPVOID*)&m_pfnCreateFileA, GetProcAddress( GetModuleHandleW( L"Kernel32.dll" ), "CreateFileA" ), API_CreateFileA );

    // Hook readfile
    g_hookingLib.Hook( &m_ReadFile_hookid, (LPVOID*)&m_pfnReadFile, GetProcAddress( GetModuleHandleW( L"Kernel32.dll" ), "ReadFile" ), API_ReadFile );

    // Hook getfilesize.
    g_hookingLib.Hook( &m_GetFileSize_hookid, (LPVOID*)&m_pfnGetFileSize, GetProcAddress( GetModuleHandleW( L"Kernel32.dll" ), "GetFileSize" ), API_GetFileSize );

    // Hook setfilepointer.
    g_hookingLib.Hook( &m_SetFilePointer_hookid, (LPVOID*)&m_pfnSetFilePointer, GetProcAddress( GetModuleHandleW( L"Kernel32.dll" ), "SetFilePointer" ), API_SetFilePointer );

    // Hook closehandle
    g_hookingLib.Hook( &m_CloseHandle_hookid, (LPVOID*)&m_pfnCloseHandle, GetProcAddress( GetModuleHandleW( L"Kernel32.dll" ), "CloseHandle" ), API_CloseHandle );

    WriteDebugEvent ( "File system hooks applied." );
}

void CFileSystemHook::RemoveHook ( )
{
    // UnHook createfile.
    if ( m_pfnCreateFileA )
    {
        g_hookingLib.Unhook( m_CreateFileA_hookid );

        m_pfnCreateFileA = NULL;
    }

    // UnHook readfile
    if ( m_pfnReadFile )
    {
        g_hookingLib.Unhook( m_ReadFile_hookid );

        m_pfnReadFile = NULL;
    }

    // UnHook getfilesize
    if ( m_pfnGetFileSize )
    {
        g_hookingLib.Unhook( m_GetFileSize_hookid );

        m_pfnGetFileSize = NULL;
    }

    // UnHook setfilepointer
    if ( m_pfnSetFilePointer )
    {
        g_hookingLib.Unhook( m_SetFilePointer_hookid );

        m_pfnSetFilePointer = NULL;
    }

    // UnHook closehandle
    if ( m_pfnCloseHandle )
    {
        g_hookingLib.Unhook( m_CloseHandle_hookid );

        m_CloseHandle_hookid = NULL;
    }



    WriteDebugEvent ( "File system hooks removed." );
}


HANDLE WINAPI CFileSystemHook::API_CreateFileA ( LPCTSTR               lpFileName,
                                                 DWORD                 dwDesiredAccess,
                                                 DWORD                 dwShareMode,
                                                 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                                 DWORD                 dwCreationDisposition,
                                                 DWORD                 dwFlagsAndAttributes,
                                                 HANDLE                hTemplateFile )
{   
    CFileSystemHook *   pThis;
    string              strRedirectedFile;
    bool                bRedirectToFileBuffer;
    void*               pFilebufferOut;
    size_t              size;
    bool                bReadOnly;

    // Get our self-pointer.
    pThis = CFileSystemHook::GetSingletonPtr ( );

    // See if we have a redirect for this file.  If not, just do the normal call.
    if ( pThis->GetRedirectedFile ( lpFileName, strRedirectedFile, bRedirectToFileBuffer, &pFilebufferOut, size, bReadOnly ) )
    {
        if ( !bRedirectToFileBuffer )
        {
            return pThis->m_pfnCreateFileA ( strRedirectedFile.c_str ( ),
                                            dwDesiredAccess,
                                            dwShareMode,
                                            lpSecurityAttributes,
                                            dwCreationDisposition,
                                            dwFlagsAndAttributes,
                                            hTemplateFile );
        }
        else
        {
            // Create a filebuffer for it
            CFileSystemMemoryHandle* pHandle = NULL;
            if ( !pFilebufferOut )
            {
                pHandle = new CFileSystemMemoryHandle ( (HANDLE)13556 );
            }
            else
            {
                pHandle = new CFileSystemMemoryHandle ( (HANDLE)13556, pFilebufferOut, size );
            }

            pThis->m_pMemoryHandleManager->Add ( pHandle );
            SetLastError ( NO_ERROR );
            return (HANDLE)13556;
        }
    }

    // Return the created file.
    return pThis->m_pfnCreateFileA ( lpFileName,
                                     dwDesiredAccess,
                                     dwShareMode,
                                     lpSecurityAttributes,
                                     dwCreationDisposition,
                                     dwFlagsAndAttributes,
                                     hTemplateFile );
}

BOOL WINAPI CFileSystemHook::API_ReadFile           ( HANDLE hFile,
                                                      LPVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped )
{
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ();
    if ( pThis )
    {
        CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
        if ( pHandle )
        {
            return pHandle->ReadFile ( lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead );
        }
        else
        {
            return pThis->m_pfnReadFile ( hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped );
        }
    }

    return FALSE;
}

DWORD WINAPI CFileSystemHook::API_WriteFile         ( HANDLE hFile,
                                                      LPCVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped )
{
    return 0;
}

DWORD WINAPI CFileSystemHook::API_GetFileSize ( HANDLE hFile, LPDWORD lpFileSizeHigh )
{
    CFileSystemHook * pThis;

    // Get our self-pointer.
    pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
    if ( pHandle )
    {
        return pHandle->GetFileSize ( lpFileSizeHigh );
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnGetFileSize ( hFile, lpFileSizeHigh );
    }
}

DWORD WINAPI CFileSystemHook::API_SetFilePointer  ( HANDLE   hFile,
                                                    LONG     lDistanceToMove,
                                                    PLONG    lpDistanceToMoveHigh,
                                                    DWORD    dwMoveMethod )
{
    // Get our self-pointer.
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hFile );
    if ( pHandle )
    {
        return pHandle->SetFilePointer ( lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnSetFilePointer ( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
    }
}

BOOL CFileSystemHook::API_CloseHandle ( HANDLE hObject )
{
    // Get our self-pointer.
    CFileSystemHook* pThis = CFileSystemHook::GetSingletonPtr ( );

    CFileSystemMemoryHandle* pHandle = pThis->m_pMemoryHandleManager->Get ( hObject );
    if ( pHandle )
    {
        pThis->m_pMemoryHandleManager->Remove ( pHandle );
        delete pHandle;
        return TRUE;
    }
    else
    {
        // Return the real call to setfilepointer.
        return pThis->m_pfnCloseHandle ( hObject );
    }
}

void CFileSystemHook::RedirectFile       ( const string& NativeFile, const string& RedirectedFile )
{
    FILEREDIRECT RedirectedFileEntry;

    // Copy in our parameters.
    RedirectedFileEntry.NativeFile      = NativeFile;
    RedirectedFileEntry.RedirectedFile  = RedirectedFile;
    RedirectedFileEntry.bRedirectToBuffer = false;
    RedirectedFileEntry.pBuffer = NULL;
    RedirectedFileEntry.sizeBufferSize = 0;
    RedirectedFileEntry.bBufferReadOnly = false;
    
    // Add this item to the list.
    m_RedirectList.insertAtFront ( RedirectedFileEntry );
}

void CFileSystemHook::RedirectFile  ( const string& RedirectedFileName, void* pFilebufferOut, size_t size, bool bReadOnly )
{
    FILEREDIRECT RedirectedFileEntry;

    // Copy in our parameters.
    RedirectedFileEntry.NativeFile              = RedirectedFileName;
    RedirectedFileEntry.RedirectedFile          = "";
    RedirectedFileEntry.bRedirectToBuffer   = true;
    RedirectedFileEntry.pBuffer             = pFilebufferOut;
    RedirectedFileEntry.sizeBufferSize          = size;
    RedirectedFileEntry.bBufferReadOnly               = bReadOnly;
    
    // Add this item to the list.
    m_RedirectList.insertAtFront ( RedirectedFileEntry );
}

void CFileSystemHook::RemoveRedirect     ( const string& NativeFileToRemove )
{
    CLinkedList< FILEREDIRECT >::CIterator iterator;

    // Find the entry we're looking for.
    for ( iterator = m_RedirectList.getBegin (); 
          iterator != m_RedirectList.getEnd (); 
          iterator++ ) 
    {
        // Check to see if this is the variable.
        if ( iterator->NativeFile == NativeFileToRemove )
        { 
            // Remove this item.
            m_RedirectList.remove ( iterator );

            if ( m_RedirectList.isEmpty() ) break;
            iterator = m_RedirectList.getBegin();
        }
    }
}

bool CFileSystemHook::GetRedirectedFile   ( const string& NativeFileToCheck, string & RedirectedFileOut, bool& bRedirectToFileBuffer, void** pFilebufferOut, size_t& size, bool& bReadOnly )
{
    CLinkedList< FILEREDIRECT >::CIterator iterator;

    // Find the entry we're looking for.
    for ( iterator = m_RedirectList.getBegin (); 
          iterator != m_RedirectList.getEnd (); 
          iterator++ ) 
    {
        // Check to see if this is the variable.
        if ( iterator->NativeFile.find ( NativeFileToCheck ) != string::npos )
        {
            RedirectedFileOut = iterator->RedirectedFile;
            bRedirectToFileBuffer = iterator->bRedirectToBuffer;
            *pFilebufferOut = iterator->pBuffer;
            size = iterator->sizeBufferSize;
            bReadOnly = iterator->bBufferReadOnly;
            return true;
        }
    }

    return false;
}

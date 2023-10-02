/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/Core.cpp
*  PURPOSE:     Core library entry point
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::string;

CCore* g_pCore = NULL;

typedef LPVOID (__stdcall*pfnHeapAlloc_t)( HANDLE heap, DWORD flags, SIZE_T sBytes );
typedef LPVOID (__stdcall*pfnHeapReAlloc_t)( HANDLE heap, DWORD flags, LPVOID mem, SIZE_T size );
typedef SIZE_T (__stdcall*pfnHeapSize_t)( HANDLE heap, DWORD flags, LPVOID mem );
typedef BOOL (__stdcall*pfnHeapValidate_t)( HANDLE heap, DWORD flags, LPVOID mem );
typedef BOOL (__stdcall*pfnHeapFree_t)( HANDLE heap, DWORD flags, LPVOID mem );

static pfnHeapAlloc_t m_pfnHeapAlloc;
static pfnHeapReAlloc_t m_pfnHeapReAlloc;
static pfnHeapSize_t m_pfnHeapSize;
static pfnHeapValidate_t m_pfnHeapValidate;
static pfnHeapFree_t m_pfnHeapFree;

struct _memIntro
{
    unsigned int checksum;
    SIZE_T blockSize;
};

struct _memOutro
{
    unsigned int checksum;
};

static LPVOID WINAPI _HeapAlloc( HANDLE heap, DWORD flags, SIZE_T sBytes )
{
    void *mem = m_pfnHeapAlloc( heap, flags, sBytes + sizeof(_memIntro) + sizeof(_memOutro) );
    _memIntro *intro = (_memIntro*)mem;
    _memOutro *outro = (_memOutro*)( (char*)mem + sBytes + sizeof(_memIntro) );

    intro->checksum = 0xCAFEBABE;
    intro->blockSize = sBytes;
    outro->checksum = 0xBABECAFE;
    return intro + 1;
}

static LPVOID WINAPI _HeapReAlloc( HANDLE heap, DWORD flags, LPVOID _mem, SIZE_T sBytes )
{
    void *mem = m_pfnHeapReAlloc( heap, flags, (char*)_mem - sizeof(_memIntro), sBytes + sizeof(_memIntro) + sizeof(_memOutro) );
    _memIntro *intro = (_memIntro*)mem;
    _memOutro *outro = (_memOutro*)( (char*)mem + sBytes + sizeof(_memIntro) );

    intro->checksum = 0xCAFEBABE;
    intro->blockSize = sBytes;
    outro->checksum = 0xBABECAFE;
    return intro + 1;
}

static SIZE_T WINAPI _HeapSize( HANDLE heap, DWORD flags, LPVOID mem )
{
    return ((_memIntro*)( (char*)mem - sizeof(_memIntro) ))->blockSize;
}

static BOOL WINAPI _HeapValidate( HANDLE heap, DWORD flags, LPVOID mem )
{
    return m_pfnHeapValidate( heap, flags, ( mem == 0 ) ? 0 : ( (char*)mem - sizeof(_memIntro) ) );
}

static BOOL WINAPI _HeapFree( HANDLE heap, DWORD flags, LPVOID mem )
{
    _memIntro *intro = (_memIntro*)( (char*)mem - sizeof(_memIntro) );
    _memOutro *outro = (_memOutro*)( (char*)mem + intro->blockSize );
    
    if ( intro->checksum != 0xCAFEBABE || outro->checksum != 0xBABECAFE )
        __asm int 3

    return m_pfnHeapFree( heap, flags, intro );
}

int WINAPI DllMain( HINSTANCE hModule, DWORD dwReason, PVOID pvNothing )
{
    if ( dwReason == DLL_PROCESS_ATTACH )
    {
#if 0
        // Hook the default allocation runtimes to trace memory invalidations
        m_pfnHeapAlloc = (pfnHeapAlloc_t)DetourFunction( (PBYTE)GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "HeapAlloc" ), (PBYTE)_HeapAlloc );
        m_pfnHeapReAlloc = (pfnHeapReAlloc_t)DetourFunction( (PBYTE)GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "HeapReAlloc" ), (PBYTE)_HeapReAlloc );
        m_pfnHeapSize = (pfnHeapSize_t)DetourFunction( (PBYTE)GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "HeapSize" ), (PBYTE)_HeapSize );
        m_pfnHeapValidate = (pfnHeapValidate_t)DetourFunction( (PBYTE)GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "HeapValidate" ), (PBYTE)_HeapValidate );
        m_pfnHeapFree = (pfnHeapFree_t)DetourFunction( (PBYTE)GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "HeapFree" ), (PBYTE)_HeapFree );
#endif

        g_pCore = new CCore;
    } 
    else if ( dwReason == DLL_PROCESS_DETACH )
    {
        // For now, TerminateProcess if any destruction is attempted (or we'll crash)
        TerminateProcess ( GetCurrentProcess (), 0 );

        if ( g_pCore )
        {
            delete g_pCore;
            g_pCore = NULL;
        }
    }

    return TRUE;
}

//--------------------------------------------------------

extern "C"
{
BOOL WINAPI _DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
}

BOOL WINAPI _dllInit( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
        DbgHeap_Init();
        break;
    }

    BOOL ret = _DllMainCRTStartup( hinstDLL, fdwReason, lpReserved );

    switch( fdwReason )
    {
    case DLL_PROCESS_DETACH:
        DbgHeap_Shutdown();
        break;
    }

    return ret;
}
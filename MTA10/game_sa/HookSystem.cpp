/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/HookSystem.cpp
*  PURPOSE:     Function hook installation system
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

//
////////////////////////////////////////////////////////////////////

inline DWORD GetProcedureOffset( DWORD dwFrom, DWORD dwTo )
{
    return dwTo - (dwFrom + 5);
}

BOOL HookInstall( DWORD dwInstallAddress, DWORD dwHookHandler, int iJmpCodeSize )
{
    BYTE JumpBytes[MAX_JUMPCODE_SIZE];

    MemSetFast ( JumpBytes, 0x90, MAX_JUMPCODE_SIZE );

    if ( CreateJump ( dwInstallAddress, dwHookHandler, JumpBytes ) )
    {
        MemCpy ( (PVOID)dwInstallAddress, JumpBytes, iJmpCodeSize );
        return TRUE;
    }

    return FALSE;
}

BOOL PatchCall( DWORD dwInstallAddress, DWORD dwHookHandler )
{
    *((DWORD*)( dwInstallAddress + 1 )) = GetProcedureOffset( dwInstallAddress, dwHookHandler );
    return TRUE;
}

////////////////////////////////////////////////////////////////////

BYTE * CreateJump ( DWORD dwFrom, DWORD dwTo, BYTE * ByteArray )
{
    ByteArray[0] = 0xE9;
    MemPutFast < DWORD > ( &ByteArray[1], GetProcedureOffset( dwFrom, dwTo ) );
    return ByteArray;
}

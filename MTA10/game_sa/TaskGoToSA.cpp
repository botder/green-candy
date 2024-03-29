/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskGoToSA.cpp
*  PURPOSE:     Go-to game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskComplexWander                                    
// ## Purpose: Generic task that makes peds wander around. Can't be used 
// ##          directly, use a superclass of this instead.
// ##############################################################################

int CTaskComplexWanderSA::GetWanderType()
{
    DEBUG_TRACE("int CTaskComplexWander::GetWanderType()");
    
    return GetInterface()->GetWanderType();
}

CNodeAddress * CTaskComplexWanderSA::GetNextNode()
{
    return &GetInterface()->m_NextNode;
}

CNodeAddress * CTaskComplexWanderSA::GetLastNode()
{
    return &GetInterface()->m_LastNode;
}


// ##############################################################################
// ## Name:    CTaskComplexWanderStandard                                    
// ## Purpose: Standard class used for making normal peds wander around
// ##############################################################################

CTaskComplexWanderStandardSA::CTaskComplexWanderStandardSA ( int iMoveState, unsigned char iDir, bool bWanderSensibly )
{
    DEBUG_TRACE("CTaskComplexWanderStandardSA::CTaskComplexWanderStandardSA(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    DWORD dwFunc = FUNC_CTaskComplexWanderStandard__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThisInterface
        push    bWanderSensibly
        push    iDir
        push    iMoveState
        call    dwFunc
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskBasicSA.cpp
*  PURPOSE:     Basic game tasks
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA ( const int iDuration )
{
    DEBUG_TRACE("CTaskComplexUseMobilePhoneSA::CTaskComplexUseMobilePhoneSA(const int iDuration)");

    CreateTaskInterface();

    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskComplexUseMobilePhone__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    iDuration
        call    dwFunc
    }
}

CTaskSimpleRunAnimSA::CTaskSimpleRunAnimSA( AssocGroupId animGroup, AnimationId animID, float fBlendDelta, int iTaskType, const char *pTaskName, bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunAnimSA::CTaskSimpleRunAnimSA( AssocGroupId animGroup, AnimationId animID, float fBlendDelta, int iTaskType, const char *pTaskName, bool bHoldLastFrame )");

    CreateTaskInterface ();

    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskSimpleRunAnim__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bHoldLastFrame
        push    pTaskName
        push    iTaskType
        push    fBlendDelta
        push    animID
        push    animGroup
        call    dwFunc
    }
}

CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA( const char *pAnimName, const char *pAnimGroupName, int flags, float fBlendDelta, int iTime, bool bDontInterrupt, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunNamedAnimSA::CTaskSimpleRunNamedAnimSA( const char *pAnimName, const char *pAnimGroupName, int flags, float fBlendDelta, int iTime, bool bDontInterrupt, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame )");

    // TODO: Find out the real size
    CreateTaskInterface();

    if ( !IsValid () ) return;
    DWORD dwFunc = FUNC_CTaskSimpleRunNamedAnim__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bHoldLastFrame
        push    bOffsetPed
        push    bRunInSequence
        push    bDontInterrupt
        push    iTime
        push    fBlendDelta
        push    flags
        push    pAnimGroupName
        push    pAnimName
        call    dwFunc
    }
}

CTaskComplexDieSA::CTaskComplexDieSA ( const eWeaponType eMeansOfDeath,
                                       const AssocGroupId animGroup,
                                       const AnimationId anim, 
                                       const float fBlendDelta,
                                       const float fAnimSpeed,
                                       const bool bBeingKilledByStealth,
                                       const bool bFallingToDeath,
                                       const int iFallToDeathDir,
                                       const bool bFallToDeathOverRailing )
{
    DEBUG_TRACE("CTaskComplexDieSA::CTaskComplexDieSA ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )");

    // TODO: Find out the real size
    CreateTaskInterface();

    if ( !IsValid () )
        return;

    DWORD dwFunc = FUNC_CTaskComplexDie__Constructor;
    DWORD dwThisInterface = (DWORD)this->GetInterface ();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bFallToDeathOverRailing
        push    iFallToDeathDir
        push    bFallingToDeath
        push    bBeingKilledByStealth
        push    fAnimSpeed
        push    fBlendDelta
        push    anim
        movzx   eax,animGroup
        push    eax
        push    eMeansOfDeath
        call    dwFunc
    }
}

CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA ( bool bKiller, CPed * pPed, const AssocGroupId animGroup )
{
    DEBUG_TRACE("CTaskSimpleStealthKillSA::CTaskSimpleStealthKillSA ( bool bKiller, class CPed * pPed, const AssocGroupId animGroup )");

    // TODO: Find out the real size
    CreateTaskInterface();

    if ( !IsValid () )
        return;

    CPedSA *ped = dynamic_cast <CPedSA*> ( pPed );

    DWORD dwFunc = FUNC_CTaskSimpleStealthKill__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;
    DWORD dwPedInterface = (DWORD)ped->GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        movzx   eax,animGroup
        push    eax
        push    dwPedInterface
        push    bKiller
        call    dwFunc
    }
}

CTaskSimpleDeadSA::CTaskSimpleDeadSA ( unsigned int uiDeathTimeMS, bool bUnk2 )
{
    DEBUG_TRACE("CTaskSimpleDeadSA::CTaskSimpleDeadSA ( int iUnk1, bool bUnk2 )");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    DWORD dwFunc = FUNC_CTaskSimpleDead__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;
    
    _asm
    {
        mov     ecx, dwThisInterface
        push    bUnk2
        push    uiDeathTimeMS
        call    dwFunc
    }
}

CTaskComplexSunbatheSA::CTaskComplexSunbatheSA ( CObject* pTowel, bool bStartStanding )
{
    DEBUG_TRACE("CTaskComplexSunbatheSA::CTaskComplexSunbatheSA ( CObject* pTowel, const bool bStartStanding )");

    // TODO: Find out the real size
    CreateTaskInterface();

    if ( !IsValid () )
        return;

    CObjectSA *obj = dynamic_cast <CObjectSA*> ( pTowel );

    DWORD dwFunc = FUNC_CTaskComplexSunbathe__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;
    DWORD dwObjectInterface = (DWORD)obj->GetInterface();

    _asm
    {
        mov     ecx, dwThisInterface
        push    bStartStanding
        push    dwObjectInterface;
        call    dwFunc
    }
}

void CTaskComplexSunbatheSA::SetEndTime ( unsigned int time )
{
    GetInterface()->m_BathingTimer.m_timeStart = time;
}

////////////////////
// Player on foot //
////////////////////
CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA()
{
    DEBUG_TRACE("CTaskSimplePlayerOnFootSA::CTaskSimplePlayerOnFootSA");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    DWORD dwFunc = (DWORD)FUNC_CTASKSimplePlayerOnFoot__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
    }
}

////////////////////
// Complex facial //
////////////////////
CTaskComplexFacialSA::CTaskComplexFacialSA()
{
    DEBUG_TRACE("CTaskComplexFacialSA::CTaskComplexFacialSA");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    DWORD dwFunc = (DWORD)FUNC_CTASKComplexFacial__Constructor;
    DWORD dwThisInterface = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThisInterface
        call    dwFunc
    }
}

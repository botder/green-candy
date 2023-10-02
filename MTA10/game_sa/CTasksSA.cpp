/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTasksSA.cpp
*  PURPOSE:     Task creation
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CTasksSA::CTasksSA ( CTaskManagementSystemSA* pTaskManagementSystem )
{
    DEBUG_TRACE("CTasksSA::CTasksSA ( CTaskManagementSystemSA* pTaskManagementSystem )");
    m_pTaskManagementSystem = pTaskManagementSystem;
}

CTaskSimplePlayerOnFoot* CTasksSA::CreateTaskSimplePlayerOnFoot()
{
    DEBUG_TRACE ("CTaskSimplePlayerOnFoot* CTasksSA::CreateTaskSimplePlayerOnFoot()");

    return new CTaskSimplePlayerOnFootSA;
}

CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial ( void )
{
    DEBUG_TRACE ("CTaskComplexFacial* CTasksSA::CreateTaskComplexFacial()");

    return new CTaskComplexFacialSA;
}

CTaskSimpleCarSetPedInAsDriver* CTasksSA::CreateTaskSimpleCarSetPedInAsDriver( CVehicle *pVehicle )
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsDriver* CTasksSA::CreateTaskSimpleCarSetPedInAsDriver( CVehicle *pVehicle )");

    return new CTaskSimpleCarSetPedInAsDriverSA( pVehicle, (CTaskUtilityLineUpPedWithCar*)NULL );
}

CTaskSimpleCarSetPedInAsPassenger* CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger( CVehicle * pVehicle, int iTargetDoor )
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsPassenger* CTasksSA::CreateTaskSimpleCarSetPedInAsPassenger( CVehicle * pVehicle, int iTargetDoor )");

    return new CTaskSimpleCarSetPedInAsPassengerSA( pVehicle, iTargetDoor, (CTaskUtilityLineUpPedWithCar*)NULL );
}

CTaskSimpleCarSetPedOut* CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedOut* CTasksSA::CreateTaskSimpleCarSetPedOut(CVehicle * pVehicle, int iTargetDoor, bool bSwitchOffEngine)");

    return new CTaskSimpleCarSetPedOutSA( pVehicle, iTargetDoor, bSwitchOffEngine );
}

CTaskComplexWanderStandard* CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const char iDir, const bool bWanderSensibly)
{
    DEBUG_TRACE("CTaskComplexWanderStandard* CTasksSA::CreateTaskComplexWanderStandard(const int iMoveState, const unsigned char iDir, const bool bWanderSensibly)");

    return new CTaskComplexWanderStandardSA( iMoveState, iDir, bWanderSensibly );
}

CTaskComplexEnterCarAsDriver* CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterCarAsDriver* CTasksSA::CreateTaskComplexEnterCarAsDriver(CVehicle * pVehicle)");

    return new CTaskComplexEnterCarAsDriverSA( pVehicle );
}

CTaskComplexEnterCarAsPassenger* CTasksSA::CreateTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff )
{
    DEBUG_TRACE("CTaskComplexEnterCarAsPassenger* CTasksSA::CTaskComplexEnterCarAsPassenger(CVehicle * pVehicle, const int iTargetSeat, const bool bCarryOnAfterFallingOff)");

    return new CTaskComplexEnterCarAsPassengerSA( pVehicle, iTargetSeat, bCarryOnAfterFallingOff );
}

CTaskComplexEnterBoatAsDriver* CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)
{
    DEBUG_TRACE("CTaskComplexEnterBoatAsDriver* CTasksSA::CreateTaskComplexEnterBoatAsDriver(CVehicle * pVehicle)");

    return new CTaskComplexEnterBoatAsDriverSA( pVehicle );
}

CTaskComplexLeaveCar* CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )
{
    DEBUG_TRACE("CTaskComplexLeaveCar* CTasksSA::CreateTaskComplexLeaveCar ( CVehicle* pTargetVehicle, const int iTargetDoor, const int iDelayTime, const bool bSensibleLeaveCar, const bool bForceGetOut )");

    return new CTaskComplexLeaveCarSA( pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut );
}

CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )
{
    DEBUG_TRACE("CTaskComplexUseMobilePhone* CTasksSA::CreateTaskComplexUseMobilePhone ( const int iDuration )");

    return new CTaskComplexUseMobilePhoneSA( iDuration );
}

CTaskSimpleDuck* CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )
{
    DEBUG_TRACE("CTaskSimpleDuck* CTasksSA::CreateTaskSimpleDuck ( eDuckControlTypes nDuckControl, unsigned short nLengthOfDuck, unsigned short nUseShotsWhizzingEvents )");

    return new CTaskSimpleDuckSA( nDuckControl, nLengthOfDuck, nUseShotsWhizzingEvents );
}

CTaskSimpleChoking* CTasksSA::CreateTaskSimpleChoking ( CPed* pAttacker, bool bIsTearGas )
{
    DEBUG_TRACE("CTaskSimpleChoking* CTasksSA::CreateTaskSimpleChoking ( CPed* pAttacker, bool bIsTearGas )");

    return new CTaskSimpleChokingSA( pAttacker, bIsTearGas );
}

CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )
{
    DEBUG_TRACE("CTaskSimpleClimb* CTasksSA::CreateTaskSimpleClimb ( CEntity *pClimbEnt, const CVector &vecTarget, float fHeading, unsigned char nSurfaceType, char nHeight, const bool bForceClimb )");

    return new CTaskSimpleClimbSA( pClimbEnt, vecTarget, fHeading, nSurfaceType, nHeight, bForceClimb );
}

CTaskSimpleJetPack* CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )
{
    DEBUG_TRACE("CTaskSimpleJetPack* CTasksSA::CreateTaskSimpleJetpack ( const CVector *pVecTargetPos, float fCruiseHeight, int nHoverTime )");

    return new CTaskSimpleJetPackSA( pVecTargetPos, fCruiseHeight, nHoverTime );
}

CTaskSimpleRunAnim* CTasksSA::CreateTaskSimpleRunAnim ( AssocGroupId animGroup, AnimationId animID, float fBlendDelta, int iTaskType, const char* pTaskName, bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunAnim* CTasksSA::CreateTaskSimpleRunAnim ( AssocGroupId animGroup, AnimationId animID, float fBlendDelta, int iTaskType, const char* pTaskName, bool bHoldLastFrame )");

    return new CTaskSimpleRunAnimSA( animGroup, animID, fBlendDelta, iTaskType, pTaskName, bHoldLastFrame );
}

CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )
{
    DEBUG_TRACE("CTaskSimpleRunNamedAnim* CTasksSA::CreateTaskSimpleRunNamedAnim ( const char* pAnimName, const char* pAnimGroupName, const int flags, const float fBlendDelta, const int iTime, const bool bDontInterrupt, const bool bRunInSequence, const bool bOffsetPed, const bool bHoldLastFrame )");

    return new CTaskSimpleRunNamedAnimSA( pAnimName, pAnimGroupName, flags, fBlendDelta, iTime, bDontInterrupt, bRunInSequence, bOffsetPed, bHoldLastFrame );
}

CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )
{
    DEBUG_TRACE("CTaskComplexDie* CTasksSA::CreateTaskComplexDie ( const eWeaponType eMeansOfDeath, const AssocGroupId animGroup, const AnimationId anim, const float fBlendDelta, const float fAnimSpeed, const bool bBeingKilledByStealth, const bool bFallingToDeath, const int iFallToDeathDir, const bool bFallToDeathOverRailing )");

    return new CTaskComplexDieSA( eMeansOfDeath, animGroup, anim, fBlendDelta, fAnimSpeed, bBeingKilledByStealth, bFallingToDeath, iFallToDeathDir, bFallToDeathOverRailing );
}

CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, AnimationId animGroup )
{
    DEBUG_TRACE("CTaskSimpleStealthKill* CTasksSA::CreateTaskSimpleStealthKill ( bool bKiller, class CPed * pPed, AnimationId anim )");

    return new CTaskSimpleStealthKillSA( bKiller, pPed, (AssocGroupId)animGroup );
}

CTaskSimpleDead* CTasksSA::CreateTaskSimpleDead ( unsigned int uiDeathTimeMS, bool bUnk )
{
    DEBUG_TRACE("CTaskSimpleDead* CTasksSA::CreateTaskSimpleDead ( unsigned int uiDeathTimeMS, bool bUnk )");

    return new CTaskSimpleDeadSA( uiDeathTimeMS, bUnk );
}

CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )
{
    DEBUG_TRACE("CTaskComplexSunbathe* CTasksSA::CreateTaskComplexSunbathe ( class CObject* pTowel, const bool bStartStanding )");

    return new CTaskComplexSunbatheSA( pTowel, bStartStanding );
}

CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )
{
    DEBUG_TRACE("CTaskSimpleIKChain* CTasksSA::CreateTaskSimpleIKChain ( char* idString, int effectorBoneTag, CVector effectorVec, int pivotBoneTag, CEntity* pEntity, int offsetBoneTag, CVector offsetPos, float speed, int time, int blendTime )");

    return new CTaskSimpleIKChainSA( idString, effectorBoneTag, effectorVec, pivotBoneTag, pEntity, offsetBoneTag, offsetPos, speed, time, blendTime );
}

CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )
{
    DEBUG_TRACE("CTaskSimpleIKLookAt* CTasksSA::CreateTaskSimpleIKLookAt ( char* idString, CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int m_priority )");

    return new CTaskSimpleIKLookAtSA( idString, pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, m_priority );
}

CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )
{
    DEBUG_TRACE("CTaskSimpleTriggerLookAt* CTasksSA::CreateTaskSimpleTriggerLookAt ( CEntity* pEntity, int time, int offsetBoneTag, CVector offsetPos, unsigned char useTorso, float speed, int blendTime, int priority )");

    return new CTaskSimpleTriggerLookAtSA( pEntity, time, offsetBoneTag, offsetPos, useTorso, speed, blendTime, priority );
}

CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )
{
    DEBUG_TRACE("CTaskSimpleGangDriveBy* CTasksSA::CreateTaskSimpleGangDriveBy ( CEntity *pTargetEntity, const CVector *pVecTarget, float fAbortRange, char FrequencyPercentage, char nDrivebyStyle, bool bSeatRHS )");

    return new CTaskSimpleGangDriveBySA( pTargetEntity, pVecTarget, fAbortRange, FrequencyPercentage, nDrivebyStyle, bSeatRHS );
}

CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )
{
    DEBUG_TRACE("CTaskSimpleUseGun* CTasksSA::CreateTaskSimpleUseGun ( CEntity * pTargetEntity, CVector vecTarget, char nCommand, short nBurstLength, unsigned char bAimImmediate )");

    return new CTaskSimpleUseGunSA( pTargetEntity, vecTarget, nCommand, nBurstLength, bAimImmediate );
}

CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )
{
    DEBUG_TRACE ("CTaskSimpleFight* CTasksSA::CreateTaskSimpleFight ( CEntity *pTargetEntity, int nCommand, unsigned int nIdlePeriod )");

    return new CTaskSimpleFightSA( pTargetEntity, nCommand, nIdlePeriod );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagementSystemSA.cpp
*  PURPOSE:     Task management system
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CMTATaskPool *mtaTaskPool;
CTaskSA *mtaTasks[MAX_TASKS];

void __cdecl HOOK_CTask_Operator_Delete( CTaskSAInterface *task )
{
    unsigned int id = (*ppTaskPool)->GetIndex( task );

    if ( CTaskSA *task = mtaTasks[id] )
        task->DestroyJustThis();

    // Free it from the pool
    (*ppTaskPool)->Free( id );
}

CTaskManagementSystemSA::CTaskManagementSystemSA()
{
    // Install our hook used to delete our tasks when GTA does
    HookInstall ( FUNC_CTask_Operator_Delete, (DWORD)HOOK_CTask_Operator_Delete, 6 );

    // Initiate our task pool
    mtaTaskPool = new CMTATaskPool;

    // Zero it all out
    memset( mtaTasks, 0, sizeof(mtaTasks) );
}

CTaskManagementSystemSA::~CTaskManagementSystemSA()
{
    delete mtaTaskPool;
}

CTaskSA* CTaskManagementSystemSA::GetTask( CTaskSAInterface *task )
{
    // Return NULL if we got passed NULL
    if ( task == 0 )
        return NULL;

    unsigned int id = (*ppTaskPool)->GetIndex( task );
    CTaskSA *rslt;

    if ( rslt = mtaTasks[id] )
        return rslt;

    // Create a new task
    return CreateAppropriateTask( task, task->GetTaskType() );
}

CTaskSA* CTaskManagementSystemSA::CreateAppropriateTask( CTaskSAInterface *task, int iTaskType )
{
    CTaskSA *pTaskSA;

    switch( iTaskType )
    {
    // Attack
    case TASK_SIMPLE_GANG_DRIVEBY:
        pTaskSA = new CTaskSimpleGangDriveBySA;
        break;
    case TASK_SIMPLE_USE_GUN:
        pTaskSA = new CTaskSimpleUseGunSA;
        break;
    case TASK_SIMPLE_FIGHT:
        pTaskSA = new CTaskSimpleFightSA;
        break;
    
    // Basic
    case TASK_COMPLEX_USE_MOBILE_PHONE:
        pTaskSA = new CTaskComplexUseMobilePhoneSA;
        break;
    case TASK_SIMPLE_ANIM:
        pTaskSA = new CTaskSimpleRunAnimSA;
        break;
    case TASK_SIMPLE_NAMED_ANIM:
        pTaskSA = new CTaskSimpleRunNamedAnimSA;
        break;
    case TASK_COMPLEX_DIE:
        pTaskSA = new CTaskComplexDieSA;
        break;
    case TASK_SIMPLE_STEALTH_KILL:
        pTaskSA = new CTaskSimpleStealthKillSA;
        break;
    case TASK_COMPLEX_SUNBATHE:
        pTaskSA = new CTaskComplexSunbatheSA;
        break;

    // Car accessories
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER:
        pTaskSA = new CTaskSimpleCarSetPedInAsPassengerSA;
        break;
    case TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER:
        pTaskSA = new CTaskSimpleCarSetPedInAsDriverSA;
        break;
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        pTaskSA = new CTaskSimpleCarSetPedOutSA;
        break;

    // Car
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        pTaskSA = new CTaskComplexEnterCarAsDriverSA;
        break;
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        pTaskSA = new CTaskComplexEnterCarAsPassengerSA;
        break;
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        pTaskSA = new CTaskComplexEnterBoatAsDriverSA;
        break;
    case TASK_COMPLEX_LEAVE_CAR:
        pTaskSA = new CTaskComplexLeaveCarSA;
        break;

    // GoTo
    case TASK_COMPLEX_WANDER:
        pTaskSA = new CTaskComplexWanderStandardSA;
        break;

    // IK
    case TASK_SIMPLE_IK_CHAIN:
        pTaskSA = new CTaskSimpleIKChainSA;
        break;
    case TASK_SIMPLE_IK_LOOK_AT:
        pTaskSA = new CTaskSimpleIKLookAtSA;
        break;
    case TASK_SIMPLE_IK_MANAGER:
        pTaskSA = new CTaskSimpleIKManagerSA;
        break;

    // JumpFall
    case TASK_SIMPLE_CLIMB:
        pTaskSA = new CTaskSimpleClimbSA;
        break;
    case TASK_SIMPLE_JETPACK:
        pTaskSA = new CTaskSimpleJetPackSA;  
        break;

    // Physical response
    case TASK_SIMPLE_CHOKING:
        pTaskSA = new CTaskSimpleChokingSA;
        break;

    // Secondary
    case TASK_SIMPLE_DUCK:
        pTaskSA = new CTaskSimpleDuckSA;
        break;    

    // Just create the baseclass
    default:
        pTaskSA = new CTaskSA;
        break;
    }

    if ( pTaskSA )
    {
        // Set the internal interface
        pTaskSA->SetInterface( task );
    }
    return pTaskSA;
}
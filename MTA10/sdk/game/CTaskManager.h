/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTaskManager.h
*  PURPOSE:     Task manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASK_MANAGER
#define __CGAME_TASK_MANAGER

class CPed;
class CVehicle;
class CTask;

#include <CVector.h>

#ifndef GAME_SA_EXPORTS
    #include "TaskBasic.h"
    #include "TaskCarAccessories.h"
    #include "TaskCar.h"
    #include "TaskGoTo.h"
    #include "TaskSecondary.h"
    #include "TaskJumpFall.h"
    #include "TaskPhysicalResponse.h"
    #include "TaskIK.h"
    #include "TaskAttack.h"
#else
    class CTaskSimplePlayerOnFoot;
    class CTaskComplexFacial;
    class CTaskSimpleCarSetPedInAsDriver;
    class CTaskSimpleCarSetPedInAsPassenger;
    class CTaskSimpleCarSetPedOut;
    class CTaskComplexWanderStandard;
    class CTaskComplexEnterCarAsDriver;
    class CTaskComplexEnterCarAsPassenger;
    class CTaskComplexEnterBoatAsDriver;
    class CTaskComplexLeaveCar;
    class CTaskComplexUseMobilePhone;
    class CTaskSimpleDuck;
    class CTaskSimpleChoking;
    class CTaskSimpleClimb;
    class CTaskSimpleJetPack;
    class CTaskSimpleAnim;
    class CTaskSimpleRunAnim;
    class CTaskSimpleRunNamedAnim;
    class CTaskComplexDie;
    class CTaskSimpleStealthKill;
    class CTaskSimpleDead;
    class CTaskComplexSunbathe;

    // IK
    class CTaskSimpleIKChain;
    class CTaskSimpleIKLookAt;
    class CTaskSimpleIKManager;
    class CTaskSimpleTriggerLookAt;

    // Attack
    class CTaskSimpleGangDriveBy;
    class CTaskSimpleUseGun;
    class CTaskSimpleFight;

    enum eDuckControlTypes;
#endif

enum eTaskPriority
{
    TASK_PRIORITY_PHYSICAL_RESPONSE,
    TASK_PRIORITY_EVENT_RESPONSE_TEMP,
    TASK_PRIORITY_EVENT_RESPONSE_NONTEMP,
    TASK_PRIORITY_PRIMARY,
    TASK_PRIORITY_DEFAULT,
    TASK_PRIORITY_MAX
};

enum eTaskSecondary
{
    TASK_SECONDARY_ATTACK,                  // want duck to be after attack
    TASK_SECONDARY_DUCK,                    // because attack controls ducking movement
    TASK_SECONDARY_SAY,
    TASK_SECONDARY_FACIAL_COMPLEX,
    TASK_SECONDARY_PARTIAL_ANIM,
    TASK_SECONDARY_IK,
    TASK_SECONDARY_MAX
};

enum eTaskAbort
{
    ABORT_PRIORITY_LEISURE,
    ABORT_PRIORITY_URGENT,
    ABORT_PRIORITY_IMMEDIATE
};

class CTaskManager
{
public:
    virtual void                RemoveTask( eTaskPriority priority ) = 0;
    virtual CTask*              GetTask( eTaskPriority priority ) const = 0;
    virtual CTask*              GetActiveTask() const = 0;
    virtual CTask*              GetSimplestActiveTask() const = 0;
    virtual CTask*              GetSimplestTask( eTaskPriority priority ) const = 0;

    virtual CTask*              FindActiveTaskByType( int iTaskType ) const = 0;
    virtual CTask*              FindTaskByType( eTaskPriority priority, int iTaskType ) const = 0;

    virtual void                RemoveTaskSecondary( eTaskPriority priority ) = 0;
    virtual CTask*              GetTaskSecondary( int iType ) const = 0;
    virtual bool                HasTaskSecondary( const CTask* pTaskSecondary ) const = 0;

    virtual void                ClearTaskEventResponse() = 0;
    virtual void                Flush( eTaskPriority priority ) = 0;
};

#endif

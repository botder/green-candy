/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagerSA.h
*  PURPOSE:     Header file for task manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASK_MANAGER
#define __CGAMESA_TASK_MANAGER

#include <game/CTaskManager.h>
#include "Common.h"

class CVehicle;
class CTaskSAInterface;

//#define TASK_PRIORITY_MAX     5
//#define TASK_SECONDARY_MAX        6

// functions
#define FUNC_SetTask                    0x681AF0
#define FUNC_GetActiveTask              0x681720
#define FUNC_GetSimplestActiveTask      0x6819D0
//#define FUNC_GetSimplestTask          0x681970 // static
#define FUNC_GetSimplestTask            0x681A00
#define FUNC_FindActiveTaskByType       0x681740
#define FUNC_FindTaskByType             0x6817D0
#define FUNC_SetTaskSecondary           0x681B60
#define FUNC_ClearTaskEventResponse     0x681BD0
#define FUNC_HasTaskSecondary           0x681820

#include "CTaskManagementSystemSA.h"

// task headers
#include "TaskCarAccessoriesSA.h"
#include "TaskGoToSA.h"
#include "TaskCarSA.h"
#include "TaskJumpFallSA.h"
#include "TaskSecondarySA.h"
#include "TaskPhysicalResponseSA.h"
#include "TaskIKSA.h"

class CTaskManagerSAInterface
{
public:
    CTaskSAInterface*   m_tasks[TASK_PRIORITY_MAX];  
    CTaskSAInterface*   m_tasksSecondary[TASK_SECONDARY_MAX];

    CPedSAInterface*    m_pPed;    
};

class CTaskManagerSA : public CTaskManager
{
public:
                                CTaskManagerSA( CTaskManagerSAInterface *taskManager, CPed *ped );

    CTaskManagerSAInterface*    GetInterface()                                      { return m_interface; }

    void                        RemoveTask( eTaskPriority priority );
    void                        SetTask( CTaskSA* pTaskPrimary, eTaskPriority priority, bool bForceNewTask = false );
    CTaskSA*                    GetTask( eTaskPriority priority ) const;
    CTaskSA*                    GetActiveTask() const;
    CTaskSA*                    GetSimplestActiveTask() const;
    CTaskSA*                    GetSimplestTask( eTaskPriority priority ) const;

    CTaskSA*                    FindActiveTaskByType( int iTaskType ) const;
    CTaskSA*                    FindTaskByType( eTaskPriority priority, int iTaskType ) const;

    void                        RemoveTaskSecondary( eTaskPriority priority );
    void                        SetTaskSecondary( CTaskSA* pTaskSecondary, int iType );
    CTaskSA*                    GetTaskSecondary( int iType ) const; // TODO
    bool                        HasTaskSecondary( const CTask* pTaskSecondary ) const; // TODO

    void                        ClearTaskEventResponse();
    void                        Flush( eTaskPriority priority );

private:
    CTaskManagerSAInterface*        m_interface;
    CPed*                           m_ped;
    CTaskManagementSystemSA*        m_pTaskManagementSystem;
};

#endif

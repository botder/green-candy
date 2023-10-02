/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagementSystemSA.h
*  PURPOSE:     Header file for task management system class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CTaskManagementSystemSA;

#ifndef __CGAMESA_TASKSYSTEM
#define __CGAMESA_TASKSYSTEM

#include "TaskSA.h"
#include "TaskCarAccessoriesSA.h"
#include "TaskCarSA.h"
#include <game/CTaskManagementSystem.h>

#define MAX_TASKS   9001

#define FUNC_CTask_Operator_Delete          0x61A5B0

class CTaskManagementSystemSA : public CTaskManagementSystem
{
public:
                                CTaskManagementSystemSA();
                                ~CTaskManagementSystemSA();

    CTaskSA*                    GetTask( CTaskSAInterface *task );
    CTaskSA*                    CreateAppropriateTask( CTaskSAInterface *task, int iTaskType );
};

extern CTaskSA *mtaTasks[MAX_TASKS];

#endif

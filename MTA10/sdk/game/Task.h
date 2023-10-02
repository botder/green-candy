/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/Task.h
*  PURPOSE:     Task interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_TASK
#define __CGAME_TASK

#include "TaskTypes.h"

// these will end up in an enum once we have it
//#define TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER   811
//#define TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER  812
//#define TASK_SIMPLE_SET_PED_OUT                   816
#define TASK_SIMPLE_DUCK                415

#include "CPed.h"

class CEvent;

#define NO_TASK_TYPE            9999

class CTask
{
public:
    virtual                     ~CTask() {};

    virtual void                SetParent( CTask* pParent ) = 0;
    virtual CTask*              GetParent() = 0;

    virtual CTask*              GetSubTask() = 0;
    virtual bool                IsSimpleTask() const = 0;
    virtual int                 GetTaskType() const = 0;
    virtual const char*         GetTaskName() const = 0;
    virtual bool                IsValid() const = 0;

    virtual bool                MakeAbortable( CPed *ped, int iPriority ) = 0;

    virtual void                SetAsPedTask( CPed *ped, int iTaskPriority, bool bForceNewTask = false ) = 0;
    virtual void                SetAsSecondaryPedTask( CPed * pPed, int iType ) = 0;
};

class CTaskSimple : public virtual CTask
{
public:
    virtual                     ~CTaskSimple() {}

    virtual bool                ProcessPed( CPed *ped ) = 0;
    virtual bool                SetPedPosition( CPed *ped ) = 0;
};

class CTaskComplex : public virtual CTask
{
public:
    virtual                     ~CTaskComplex() {};

    virtual void                SetSubTask( CTask *subTask ) = 0;
    virtual CTask*              CreateNextSubTask( CPed *ped ) = 0;
    virtual CTask*              CreateFirstSubTask( CPed *ped ) = 0;
    virtual CTask*              ControlSubTask( CPed *ped ) = 0;
};

#endif

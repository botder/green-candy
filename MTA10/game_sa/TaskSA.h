/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskSA.h
*  PURPOSE:     Base game task
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASK
#define __CGAMESA_TASK

#include <game/Task.h>
#include "TaskNamesSA.h"

class CPedSAInterface;

class CTaskTimer
{
public:
    unsigned int                    m_timeStart; // ?
    unsigned int                    m_timeEnd; // ?
    bool                            m_set;
    bool                            m_stopped;
};

class CTaskSAInterface
{
public:
    virtual                                     ~CTaskSAInterface   ( void ) {}

    virtual CTaskSAInterface* __thiscall        Clone               ( void );
    virtual CTaskSAInterface* __thiscall        GetSubTask          ( void );
    virtual bool __thiscall                     IsSimpleTask        ( void ) const;
    virtual int __thiscall                      GetTaskType         ( void ) const;
    virtual void __thiscall                     StopTimer           ( CEventSAInterface *evt );
    virtual bool __thiscall                     MakeAbortable       ( CPedSAInterface *ped, int priority, CEventSAInterface *evt );

    void*   operator new( size_t );
    void    operator delete( void *ptr );

    CTaskSAInterface*           m_pParent;
};

class CTaskSimpleSAInterface : public CTaskSAInterface
{
public:
    virtual bool __thiscall                     ProcessPed( CPedSAInterface *ped ) = 0;
    virtual bool __thiscall                     SetPedPosition( CPedSAInterface *ped ) = 0;
};

class CTaskComplexSAInterface : public CTaskSAInterface
{
public:
    virtual void __thiscall                     SetSubTask( CTaskSAInterface *task ) = 0;
    virtual CTaskSAInterface* __thiscall        CreateNextSubTask( CPedSAInterface *ped ) = 0;
    virtual CTaskSAInterface* __thiscall        CreateFirstSubTask( CPedSAInterface *ped ) = 0;
    virtual CTaskSAInterface* __thiscall        ControlSubTask( CPedSAInterface *ped ) = 0;

    CTask*                      m_pSubTask;
};

class CTaskSA : public virtual CTask
{
    friend class CTaskManagementSystemSA;
protected:
    CTaskSAInterface*           m_interface;
    CTaskSA*                    m_parent;

public:
                        CTaskSA();
                        ~CTaskSA();

    void* operator new ( size_t );
    void operator delete ( void *ptr );

    unsigned int        GetPoolIndex() const                                { return m_poolIndex; }

    CTask*              Clone();
    void                SetParent( CTask *parent );
    CTask*              GetParent()                                         { return m_parent; }
    CTask*              GetSubTask();

    bool                IsSimpleTask() const                                { return m_interface->IsSimpleTask(); }
    int                 GetTaskType() const                                 { return m_interface->GetTaskType(); }
    const char*         GetTaskName() const                                 { return TaskNames[ m_interface->GetTaskType() ].szName; }
    bool                IsValid() const                                     { return m_interface != NULL; }

    void                StopTimer( CEventSAInterface *evt )                 { m_interface->StopTimer( evt ); }
    bool                MakeAbortable( CPed *ped, int iPriority );

    CTaskSAInterface*   GetInterface()                                      { return m_interface; }
    const CTaskSAInterface* GetInterface() const                            { return m_interface; }

    void                CreateTaskInterface();

    void                SetAsPedTask( CPed *ped, int iTaskPriority, bool bForceNewTask = false );
    void                SetAsSecondaryPedTask( CPed *ped, int iType );
    void                DestroyJustThis();

private:
    void                SetInterface( CTaskSAInterface *pInterface );

    unsigned int        m_poolIndex;
};

class CTaskSimpleSA : public CTaskSA, public virtual CTaskSimple
{
public:
    CTaskSimpleSA ( ) {};

    bool                        ProcessPed ( CPed* pPed );
    bool                        SetPedPosition ( CPed *pPed );

    CTaskSimpleSAInterface*     GetInterface() { return (CTaskSimpleSAInterface*)m_interface; }
};

class CTaskComplexSA : public CTaskSA, public virtual CTaskComplex
{
public:
    CTaskComplexSA() {};

    void                        SetSubTask ( CTask* pSubTask );
    CTask*                      CreateNextSubTask ( CPed* pPed );
    CTask*                      CreateFirstSubTask ( CPed* pPed );
    CTask*                      ControlSubTask ( CPed* pPed );

    CTaskComplexSAInterface*    GetInterface() { return (CTaskComplexSAInterface*)m_interface; }
};

#endif

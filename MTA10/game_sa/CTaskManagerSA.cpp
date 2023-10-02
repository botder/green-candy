/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CTaskManagerSA.cpp
*  PURPOSE:     Task manager
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

CTaskManagerSA::CTaskManagerSA( CTaskManagerSAInterface *taskManager, CPed *ped )
{
    DEBUG_TRACE("CTaskManagerSA::CTaskManagerSA(CTaskManagerSAInterface * taskManagerInterface, CPed * ped)");

    m_ped = ped;
    m_interface = taskManager;
    m_pTaskManagementSystem = pGame->GetTaskManagementSystem();
}

void CTaskManagerSA::RemoveTask( eTaskPriority priority )
{
    if ( priority == TASK_PRIORITY_DEFAULT ) // TASK_PRIORITY_DEFAULT removed = crash
        return;

    SetTask( NULL, priority );
}

void CTaskManagerSA::SetTask( CTaskSA* pTaskPrimary, eTaskPriority priority, bool bForceNewTask )
{
    DEBUG_TRACE("void CTaskManagerSA::SetTask( CTaskSA* pTaskPrimary, eTaskPriority priority, bool bForceNewTask )");
    
    DWORD dwFunc = FUNC_SetTask;
    DWORD dwInterface = (DWORD)m_interface;
    CTaskSAInterface *task = pTaskPrimary ? pTaskPrimary->GetInterface() : NULL;

    _asm
    {
        xor     eax, eax
        movzx   eax, bForceNewTask
        push    eax
        push    priority
        push    task
        mov     ecx, dwInterface
        call    dwFunc
    }    
}

CTaskSA* CTaskManagerSA::GetTask( eTaskPriority priority ) const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::GetTask( eTaskPriority priority ) const");

    return m_pTaskManagementSystem->GetTask ( m_interface->m_tasks[ priority ] );
}

CTaskSA* CTaskManagerSA::GetActiveTask() const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::GetActiveTask() const");

    DWORD dwFunc = FUNC_GetActiveTask;
    DWORD dwThis = (DWORD)m_interface;
    CTaskSAInterface *task;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     task, eax
    }

    return task ? m_pTaskManagementSystem->GetTask ( task ) : NULL;
}

CTaskSA* CTaskManagerSA::GetSimplestActiveTask() const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::GetSimplestActiveTask() const");

    DWORD dwFunc = FUNC_GetSimplestActiveTask;
    DWORD dwThis = (DWORD)m_interface;
    CTaskSAInterface *task;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     task, eax
    }

    return task ? m_pTaskManagementSystem->GetTask ( task ) : NULL;
}

CTaskSA* CTaskManagerSA::GetSimplestTask( eTaskPriority priority ) const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::GetSimplestTask( eTaskPriority priority ) const");

    DWORD dwFunc = FUNC_GetSimplestTask;
    DWORD dwThis = (DWORD)m_interface;
    CTaskSAInterface *task;

    _asm
    {
        mov     ecx, dwThis
        push    priority
        call    dwFunc
        mov     task, eax
    }

    return task ? m_pTaskManagementSystem->GetTask ( task ) : NULL;
}

CTaskSA* CTaskManagerSA::FindActiveTaskByType( int iTaskType ) const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::FindActiveTaskByType( int iTaskType ) const");

    DWORD dwFunc = FUNC_FindActiveTaskByType;
    DWORD dwThis = (DWORD)m_interface;
    CTaskSAInterface *task;

    _asm
    {
        mov     ecx, dwThis
        push    iTaskType
        call    dwFunc
        mov     task, eax
    }

    return task ? m_pTaskManagementSystem->GetTask ( task ) : NULL;
}

CTaskSA* CTaskManagerSA::FindTaskByType( eTaskPriority priority, int iTaskType ) const
{
    DEBUG_TRACE("CTaskSA* CTaskManagerSA::FindTaskByType( eTaskPriority priority, int iTaskType ) const");

    DWORD dwFunc = FUNC_FindTaskByType;
    DWORD dwThis = (DWORD)m_interface;
    CTaskSAInterface *task;

    _asm
    {
        mov     ecx, dwThis
        push    iTaskType
        push    priority
        call    dwFunc
        mov     task, eax
    }

    return task ? m_pTaskManagementSystem->GetTask ( task ) : NULL;
}

void CTaskManagerSA::RemoveTaskSecondary( eTaskPriority priority )
{
    SetTaskSecondary( NULL, priority );
}

void CTaskManagerSA::SetTaskSecondary( CTaskSA* pTask, int iType )
{
    DEBUG_TRACE("void CTaskManagerSA::SetTaskSecondary(CTask* pTaskSecondary, const int iType)");

    DWORD dwFunc = FUNC_SetTaskSecondary;
    DWORD dwInterface = (DWORD)m_interface;
    CTaskSAInterface *task = pTask ? pTask->GetInterface() : NULL;

    _asm
    {
        push    iType
        push    task
        mov     ecx, dwInterface
        call    dwFunc
    }
}

/**
 * \todo Convert to our tasks not gta's (same above too)
 */
CTaskSA* CTaskManagerSA::GetTaskSecondary( int iType ) const
{
    DEBUG_TRACE("CTask* CTaskManagerSA::GetTaskSecondary( int iType ) const");

    return iType < TASK_SECONDARY_MAX ? m_pTaskManagementSystem->GetTask ( m_interface->m_tasksSecondary[ iType ] ) : NULL;
}

bool CTaskManagerSA::HasTaskSecondary( const CTask *pTaskSecondary ) const
{
    DEBUG_TRACE("bool CTaskManagerSA::HasTaskSecondary( const CTask *pTaskSecondary ) const");

    const CTaskSAInterface *task = dynamic_cast <const CTaskSA*> ( pTaskSecondary )->GetInterface();

    for ( unsigned int n = 0; n < TASK_SECONDARY_MAX; n++ )
    {
        if ( m_interface->m_tasksSecondary[n] == task )
            return true;
    }
    return false;
}

void CTaskManagerSA::ClearTaskEventResponse()
{
    DEBUG_TRACE("void CTaskManagerSA::ClearTaskEventResponse()");

    DWORD dwFunc = FUNC_ClearTaskEventResponse;

    _asm
    {
        call    dwFunc
    }
}

void CTaskManagerSA::Flush( eTaskPriority priority )
{
    DEBUG_TRACE("void CTaskManagerSA::Flush( eTaskPriority priority )");

    for ( unsigned int i = 0; i < TASK_PRIORITY_MAX; i++ )
    {
        CTaskSA *task = GetTask ( (eTaskPriority)i );

        if ( !task )
            continue;

        task->MakeAbortable( m_ped, priority );
    }

    for ( unsigned int i = 0; i < TASK_SECONDARY_MAX; i++ )
    {
        CTaskSA *task = GetTaskSecondary ( i );

        if ( !task )
            continue;

        task->MakeAbortable( m_ped, priority );
    }
}

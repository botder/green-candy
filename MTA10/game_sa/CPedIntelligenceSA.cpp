/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedIntelligenceSA.cpp
*  PURPOSE:     Ped entity AI logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void* CPedIntelligenceSAInterface::operator new( size_t )
{
    return (*ppPedIntelligencePool)->Allocate();
}

void CPedIntelligenceSAInterface::operator delete( void *ptr )
{
    (*ppPedIntelligencePool)->Free( (CPedIntelligenceSAInterface*)ptr );
}

bool CPedIntelligenceSAInterface::TestForStealthKill( CPedSAInterface *ped, bool bUnk )
{
    bool bReturn;
    DWORD dwThis = (DWORD)this;
    DWORD dwPed = (DWORD)ped;
    DWORD dwFunc = FUNC_CPedIntelligence_TestForStealthKill;

    _asm
    {
        mov     ecx, dwThis
        push    bUnk
        push    dwPed
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

CPedIntelligenceSA::CPedIntelligenceSA( CPedIntelligenceSAInterface *intelligence, CPedSA *ped )
{
    m_interface = intelligence;
    m_ped = ped;
    m_taskManager = new CTaskManagerSA( &intelligence->m_taskManager, ped );
    m_vehicleScanner = new CVehicleScannerSA( intelligence->m_vehicleScanner );
}

CPedIntelligenceSA::~CPedIntelligenceSA()
{
    delete m_taskManager;
    delete m_vehicleScanner;
}

CVehicleScanner * CPedIntelligenceSA::GetVehicleScanner()
{
    return m_vehicleScanner;
}

bool CPedIntelligenceSA::IsRespondingToEvent()
{
    DWORD dwFunc = FUNC_IsRespondingToEvent;

        
    return false;
}

int CPedIntelligenceSA::GetCurrentEventType()
{
    DWORD dwFunc = FUNC_GetCurrentEventType;
    DWORD dwRet = 0;
    DWORD dwThis = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     dwRet, eax
    }
    return dwRet;
}

CEvent* CPedIntelligenceSA::GetCurrentEvent()
{
    return NULL;
}

bool CPedIntelligenceSA::TestForStealthKill( CPed *ped, bool unk )
{
    return m_interface->TestForStealthKill( dynamic_cast <CPedSA*> ( ped )->GetInterface(), unk );
}
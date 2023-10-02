/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskCarAccessoriesSA.cpp
*  PURPOSE:     Car accessories game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsDriver                                    
// ## Purpose: Puts the specified ped directly into a vehicle
// ##############################################################################

CTaskSimpleCarSetPedInAsDriverSA::CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsDriverSA::CTaskSimpleCarSetPedInAsDriverSA(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( !pTargetVehicleSA )
        throw "invalid vehicle";

    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsDriver__Constructor;
    DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
    DWORD dwThisInterface = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThisInterface
        push    pUtility
        push    dwVehiclePtr
        call    dwFunc
    }
}


void CTaskSimpleCarSetPedInAsDriverSA::SetIsWarpingPedIntoCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetIsWarpingPedIntoCar()");
    
    GetInterface()->m_warpingIn = true;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetDoorFlagsToClear( unsigned char flags ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    
    GetInterface()->m_doorFlagsClear = flags;
}

void CTaskSimpleCarSetPedInAsDriverSA::SetNumGettingInToClear( unsigned char num ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsDriverSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    
    GetInterface()->m_numGettingInClear = num;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsPassenger                                    
// ## Purpose: Puts the specified ped directly into a vehicle as a passenger
// ##############################################################################

CTaskSimpleCarSetPedInAsPassengerSA::CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedInAsPassengerSA::CTaskSimpleCarSetPedInAsPassengerSA(CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)");

    CreateTaskInterface();

    if ( !IsValid() )
        return;

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( !pTargetVehicleSA )
        throw "invalid vehicle";

    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedInAsPassenger__Constructor;
    DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
    DWORD dwThisInterface = (DWORD)m_interface;

    __asm
    {
        mov     ecx,dwThisInterface
        push    pUtility
        push    iTargetDoor
        push    dwVehiclePtr
        call    dwFunc
    }
}


void CTaskSimpleCarSetPedInAsPassengerSA::SetIsWarpingPedIntoCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetIsWarpingPedIntoCar()");
    
    GetInterface()->m_warpingIn = true;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetDoorFlagsToClear( unsigned char flags ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    
    GetInterface()->m_doorFlagsClear = flags;
}

void CTaskSimpleCarSetPedInAsPassengerSA::SetNumGettingInToClear( unsigned char num ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedInAsPassengerSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    
    GetInterface()->m_numGettingInClear = num;
}

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedOut                                    
// ## Purpose: Instantly removes the ped from the vehicle specified
// ##############################################################################

CTaskSimpleCarSetPedOutSA::CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine)
{
    DEBUG_TRACE("CTaskSimpleCarSetPedOutSA::CTaskSimpleCarSetPedOutSA(CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine)");

    CreateTaskInterface();

    if ( !IsValid () )
        return;

    CVehicleSA* pTargetVehicleSA = dynamic_cast < CVehicleSA* > ( pTargetVehicle );

    if ( !pTargetVehicleSA )
        throw "invalid vehicle";

    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__Constructor;
    DWORD dwVehiclePtr = (DWORD)pTargetVehicleSA->GetInterface();
    DWORD dwThisInterface = (DWORD)m_interface;

    _asm
    {
        mov     ecx, dwThisInterface
        xor     eax, eax
        movzx   eax, bSwitchOffEngine
        push    eax
        push    iTargetDoor
        push    dwVehiclePtr
        call    dwFunc
    }
}

void CTaskSimpleCarSetPedOutSA::SetIsWarpingPedOutOfCar()
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetIsWarpingPedOutOfCar()");
    
    GetInterface()->m_warpingOut = true;
}

void CTaskSimpleCarSetPedOutSA::SetKnockedOffBike() 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetKnockedOffBike() ");

    GetInterface()->m_knockedOff = true;
    GetInterface()->m_switchOffEngine = false;
}

void CTaskSimpleCarSetPedOutSA::SetDoorFlagsToClear( unsigned char flags ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetDoorFlagsToClear(const unsigned char nDoorFlagsToClear) ");
    
    GetInterface()->m_doorFlagsClear = flags;
}

void CTaskSimpleCarSetPedOutSA::SetNumGettingInToClear( unsigned char num ) 
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::SetNumGettingInToClear(const unsigned char nNumGettingInToClear) ");
    
    GetInterface()->m_numGettingInClear = num;
}

void CTaskSimpleCarSetPedOutSA::PositionPedOutOfCollision( CPed *ped, CVehicle *vehicle, unsigned char door )
{
    DEBUG_TRACE("void CTaskSimpleCarSetPedOutSA::PositionPedOutOfCollision(CPed * ped, CVehicle * vehicle, int nDoor)");

    CVehicleSA *veh = dynamic_cast <CVehicleSA*> ( vehicle );

    if ( !veh )
        return;

    DWORD dwFunc = FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision;
    DWORD dwVehiclePtr = (DWORD)veh->GetInterface();
    DWORD dwPedPtr = (DWORD)((CEntitySA *)ped)->GetInterface();

    _asm
    {
        push    door
        push    dwVehiclePtr
        push    dwPedPtr
        call    dwFunc
    }
}

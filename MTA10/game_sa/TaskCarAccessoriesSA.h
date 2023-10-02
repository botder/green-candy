/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/TaskCarAccessoriesSA.h
*  PURPOSE:     Car accessories game tasks
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_TASKCARACCESSORIES
#define __CGAMESA_TASKCARACCESSORIES

#include <game/TaskCarAccessories.h>

#include "TaskSA.h"

class CVehicle;
class CVehicleSA;
class CVehicleSAInterface;

// temporary
class CAnimBlendAssociation;
typedef DWORD CTaskUtilityLineUpPedWithCar;

#define FUNC_CTaskSimpleCarSetPedInAsDriver__Constructor            0x6470E0
#define FUNC_CTaskSimpleCarSetPedInAsPassenger__Constructor         0x646FE0
#define FUNC_CTaskSimpleCarSetPedOut__Constructor                   0x6478B0

#define FUNC_CTaskSimpleCarSetPedOut__PositionPedOutOfCollision     0x6479B0

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsDriver                                    
// ## Purpose: Puts the specified ped directly into a vehicle
// ## Notes:   Can cause crash if multiplayer dll isn't used and CPlayerPeds are
// ##############################################################################

class CTaskSimpleCarSetPedInAsDriverSAInterface : public CTaskSimpleSAInterface
{
public:
    bool                                m_isFinished;
    CAnimBlendAssociation*              m_anim;
    CVehicleSAInterface*                m_targetVehicle;
    CTaskUtilityLineUpPedWithCar*       m_utility;
    bool                                m_warpingIn;
    unsigned char                       m_doorFlagsClear;
    unsigned char                       m_numGettingInClear;
};

class CTaskSimpleCarSetPedInAsDriverSA : public CTaskSimpleSA, public virtual CTaskSimpleCarSetPedInAsDriver
{
public:
    CTaskSimpleCarSetPedInAsDriverSA () {};
    CTaskSimpleCarSetPedInAsDriverSA ( CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility );

    void SetIsWarpingPedIntoCar();
    void SetDoorFlagsToClear ( unsigned char nDoorFlagsToClear );
    void SetNumGettingInToClear ( unsigned char nNumGettingInToClear );

    CTaskSimpleCarSetPedInAsDriverSAInterface*  GetInterface() { return (CTaskSimpleCarSetPedInAsDriverSAInterface*)m_interface; }
};

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedInAsPassenger                                    
// ## Purpose: Puts the specified ped directly into a vehicle as a passenger
// ##############################################################################

class CTaskSimpleCarSetPedInAsPassengerSAInterface : public CTaskSimpleSAInterface
{
public:
    bool                                m_isFinished;
    CAnimBlendAssociation*              m_anim;
    CVehicleSAInterface*                m_targetVehicle;
    unsigned int                        m_targetDoor;
    CTaskUtilityLineUpPedWithCar*       m_utility; 
    bool                                m_warpingIn;
    unsigned char                       m_doorFlagsClear;
    unsigned char                       m_numGettingInClear;
};

class CTaskSimpleCarSetPedInAsPassengerSA : public CTaskSimpleSA, public virtual CTaskSimpleCarSetPedInAsPassenger
{
public:
    CTaskSimpleCarSetPedInAsPassengerSA () {};
    CTaskSimpleCarSetPedInAsPassengerSA ( CVehicle* pTargetVehicle, int iTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility );

    void SetIsWarpingPedIntoCar();
    void SetDoorFlagsToClear( unsigned char nDoorFlagsToClear );
    void SetNumGettingInToClear( unsigned char nNumGettingInToClear );

    CTaskSimpleCarSetPedInAsPassengerSAInterface*   GetInterface() { return (CTaskSimpleCarSetPedInAsPassengerSAInterface*)m_interface; }
};

// ##############################################################################
// ## Name:    CTaskSimpleCarSetPedOut                                    
// ## Purpose: Instantly removes the ped from the vehicle specified
// ##############################################################################

class CTaskSimpleCarSetPedOutSAInterface : public CTaskSimpleSAInterface
{
public:
    CVehicleSAInterface*                m_targetVehicle; // 8
    unsigned int                        m_targetDoor;
    bool                                m_switchOffEngine; // 16
    bool                                m_warpingOut;
    bool                                m_fallingOut;    // jumping or falling off car or bike
    bool                                m_knockedOff;
    unsigned char                       m_doorFlagsClear;
    unsigned char                       m_numGettingInClear;
};

class CTaskSimpleCarSetPedOutSA : public CTaskSimpleSA, public virtual CTaskSimpleCarSetPedOut
{
public:
    CTaskSimpleCarSetPedOutSA () {};
    CTaskSimpleCarSetPedOutSA ( CVehicle* pTargetVehicle, int iTargetDoor, bool bSwitchOffEngine = false );

    void SetIsWarpingPedOutOfCar ();
    void SetKnockedOffBike ();
    void SetDoorFlagsToClear ( unsigned char nDoorFlagsToClear );
    void SetNumGettingInToClear ( unsigned char nNumGettingInToClear );
    void PositionPedOutOfCollision ( CPed *ped, CVehicle *vehicle, unsigned char door );

    CTaskSimpleCarSetPedOutSAInterface* GetInterface() { return (CTaskSimpleCarSetPedOutSAInterface*)m_interface; }
};

#define MAX_CAR_UTIL_TASK_SIZE ( max(sizeof(CTaskSimpleCarSetPedInAsDriverSA),max(sizeof(CTaskSimpleCarSetPedInAsPassengerSA),sizeof(CTaskSimpleCarSetPedOutSA))) )

#endif

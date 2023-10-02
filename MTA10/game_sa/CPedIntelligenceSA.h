/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedIntelligenceSA.h
*  PURPOSE:     Header file for ped entity AI class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PEDINTELLIGENCE
#define __CGAMESA_PEDINTELLIGENCE

#include <game/CPedIntelligence.h>
#include "CEventSA.h"
class CTaskManagerSA;
class CVehicleScannerSA;
class CPedSA;
class CPedSAInterface;

#include "CVehicleScannerSA.h"

#define FUNC_IsRespondingToEvent                    0x600DB0
#define FUNC_GetCurrentEvent                        0x4ABE70
#define FUNC_GetCurrentEventType                    0x4ABE60
#define FUNC_CPedIntelligence_TestForStealthKill    0x601E00

class CPed;

class CFightManagerSAInterface : public CTaskSAInterface
{
public:
    BYTE            m_pad1[8];
    BYTE            m_unknownState;
    BYTE            m_pad2[3];
    float           m_strafeState;
    float           m_forwardBackwardState;
};

class CPedIntelligenceSAInterface 
{
public:
    void*   operator new( size_t );
    void    operator delete( void *ptr );

    bool                            TestForStealthKill( CPedSAInterface *ped, bool bUnk );

// CEventHandlerHistory @ + 56
    CPedSAInterface*                m_ped;
    CTaskManagerSAInterface         m_taskManager;          // 4
    BYTE                            m_pad2[160];            // 52
    CVehicleScannerSAInterface*     m_vehicleScanner;       // 212

    BYTE                            m_pad3[444];            // 216
};

class CPedIntelligenceSA : public CPedIntelligence
{
public:
                                    CPedIntelligenceSA( CPedIntelligenceSAInterface *intelligence, CPedSA *ped );
                                    ~CPedIntelligenceSA();

    CPedIntelligenceSAInterface*    GetInterface()                          { return m_interface; }

    bool                            IsRespondingToEvent();
    int                             GetCurrentEventType();
    CEvent*                         GetCurrentEvent();

    CTaskManagerSA*                 GetTaskManager()                        { return m_taskManager; }
    const CTaskManagerSA*           GetTaskManager() const                  { return m_taskManager; }
    CVehicleScanner*                GetVehicleScanner();

    bool                            TestForStealthKill( CPed *ped, bool unk );

private:
    CPedIntelligenceSAInterface*    m_interface;
    CPedSA*                         m_ped;
    CTaskManagerSA*                 m_taskManager;
    CVehicleScannerSA*              m_vehicleScanner;
};

#endif

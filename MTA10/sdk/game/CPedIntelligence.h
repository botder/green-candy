/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPedIntelligence.h
*  PURPOSE:     Ped artificial intelligence interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PEDINTELLIGENCE
#define __CGAME_PEDINTELLIGENCE

#include "CTaskManager.h"
#include "CVehicleScanner.h"
#include "CEvent.h"

class CPed;

class CPedIntelligence
{
public:
    virtual CTaskManager *      GetTaskManager() = 0; 
    virtual bool                IsRespondingToEvent() = 0;
    virtual int                 GetCurrentEventType( ) = 0;
    virtual CEvent *            GetCurrentEvent() = 0;
    virtual CVehicleScanner *   GetVehicleScanner() = 0;
    virtual bool                TestForStealthKill( CPed * pPed, bool bUnk ) = 0;
};

#endif

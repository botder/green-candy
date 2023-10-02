/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CDeathmatchVehicle.h
*  PURPOSE:     Header for deathmatch vehicle class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CDEATHMATCHVEHICLE_H
#define __CDEATHMATCHVEHICLE_H

#include "CClientVehicle.h"

class CDeathmatchVehicle : public CClientVehicle
{
public:
                                    CDeathmatchVehicle( CClientManager *pManager, lua_State *L, bool system, class CUnoccupiedVehicleSync* pUnoccupiedVehicleSync, ElementID ID, unsigned short usVehicleModel );
                                    ~CDeathmatchVehicle();

    inline bool                     IsSyncing()                                     { return m_bIsSyncing; }
    void                            SetIsSyncing( bool bIsSyncing );

    bool                            SyncDamageModel();
    void                            ResetDamageModelSync();

private:
    class CUnoccupiedVehicleSync*   m_pUnoccupiedVehicleSync;
    bool                            m_bIsSyncing;

    unsigned char                   m_ucLastDoorStates[MAX_DOORS];
    unsigned char                   m_ucLastWheelStates[MAX_WHEELS];
    unsigned char                   m_ucLastPanelStates[MAX_PANELS];
    unsigned char                   m_ucLastLightStates[MAX_LIGHTS];
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CDamageManagerSA.h
*  PURPOSE:     Header file for vehicle damage manager class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_DAMAGEMANAGER
#define __CGAMESA_DAMAGEMANAGER

#include <game/CDamageManager.h>
#include "Common.h"

#define FUNC_GetEngineStatus        0x6c22c0 
#define FUNC_SetEngineStatus        0x6c22a0
#define FUNC_GetDoorStatus          0x6c2230
#define FUNC_SetDoorStatus          0x6c21c0
#define FUNC_GetTireStatus          0x6c21b0
#define FUNC_SetTireStatus          0x6c21a0
#define FUNC_GetPanelStatus         0x6c2180
#define FUNC_SetPanelStatus         0x6c2150
#define FUNC_SetLightStatus         0x6c2100
#define FUNC_GetLightStatus         0x6c2130
#define FUNC_SetAeroplaneCompStatus 0x6C22D0
#define FUNC_GetAeroplaneCompStatus 0x6C2300

//006c25d0      public: void __thiscall CDamageManager::FuckCarCompletely(bool)
#define FUNC_FuckCarCompletely      0x6c25d0

#define MAX_DOORS                   6   // also in CAutomobile
#define MAX_WHEELS                  4

class CDamageManagerSAInterface // 24 bytes
{
public:
                        CDamageManagerSAInterface();
                        ~CDamageManagerSAInterface();

    float               m_wheelDamage;          // 0
    unsigned char       m_engineStatus;         // 4, old - wont be used
    unsigned char       m_wheels[MAX_WHEELS];   // 5
    unsigned char       m_doors[MAX_DOORS];     // 9

    BYTE                m_pad;                  // 15

    unsigned int        m_lights;               // 16, 2 bits per light
    unsigned int        m_panels;               // 20, 4 bits per panel
};

class CDamageManagerSA : public CDamageManager
{
private:
    CDamageManagerSAInterface*          m_interface;
    class CAutomobileSAInterface*       m_vehicle;

public: 
    CDamageManagerSA( class CAutomobileSAInterface *vehicle, CDamageManagerSAInterface *dmg )
    {
        m_interface = dmg;
        m_vehicle = vehicle;
    }

    unsigned char       GetEngineStatus() const;
    void                SetEngineStatus( unsigned char status );

    unsigned char       GetDoorStatus( eDoors bDoor ) const;
    void                SetDoorStatus( eDoors bDoor, unsigned char status );

    unsigned char       GetWheelStatus( eWheels bWheel ) const;
    void                SetWheelStatus( eWheels bWheel, unsigned char status );

    unsigned char       GetPanelStatus( unsigned char panel ) const;
    unsigned long       GetPanelStatus() const;
    void                SetPanelStatus( unsigned char panel, unsigned char status );
    void                SetPanelStatus( unsigned long ulStatus );

    unsigned char       GetLightStatus( unsigned char light ) const;
    unsigned char       GetLightStatus() const;
    void                SetLightStatus( unsigned char light, unsigned char status );
    void                SetLightStatus( unsigned char ucStatus );

    unsigned char       GetAeroplaneCompStatus( unsigned char id ) const;
    void                SetAeroplaneCompStatus( unsigned char id, unsigned char status );

    void                FuckCarCompletely( bool keepWheels );
};

#endif

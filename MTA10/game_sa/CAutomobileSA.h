/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAutomobileSA.h
*  PURPOSE:     Header file for automobile vehicle entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_AUTOMOBILE
#define __CGAMESA_AUTOMOBILE

#include <game/CAutomobile.h>

#include "CDoorSA.h"
#include "CDamageManagerSA.h"

#define FUNC_CAutomobile_BurstTyre                  0x6A32B0
#define FUNC_CAutomobile_BreakTowLink               0x6A4400
#define FUNC_CAutomobile_BlowUpCar                  0x6B3780
#define FUNC_CAutomobile_BlowUpCarsInPath           0x6AF110
#define FUNC_CAutomobile_CloseAllDoors              0x6A4520
#define FUNC_CAutomobile_CloseBoot                  0x6AFA20
#define FUNC_CAutomobile_FindWheelWidth             0x6A6090
#define FUNC_CAutomobile_Fix                        0x6A3440
#define FUNC_CAutomobile_FixDoor                    0x6A35A0
#define FUNC_CAutomobile_FixPanel                   0x6A3670
#define FUNC_CAutomobile_GetAllWheelsOffGround      0x6A2F70
#define FUNC_CAutomobile_GetCarPitch                0x6A6050
#define FUNC_CAutomobile_GetCarRoll                 0x6A6010
#define FUNC_CAutomobile_GetComponentWorldPosition  0x6A2210
#define FUNC_CAutomobile_GetHeightAboveRoad         0x6A62B0
#define FUNC_CAutomobile_GetNumContactWheels        0x6A62A0
#define FUNC_CAutomobile_GetRearHeightAboveRoad     0x6A0610
#define FUNC_CAutomobile_IsComponentPresent         0x6A2250
#define FUNC_CAutomobile_IsDoorClosed               0x6A2310
#define FUNC_CAutomobile_IsDoorFullyOpen            0x6A22D0
#define FUNC_CAutomobile_IsDoorMissing              0x6A2330
#define FUNC_CAutomobile_IsDoorReady                0x6A2290
#define FUNC_CAutomobile_IsInAir                    0x6A6140
#define FUNC_CAutomobile_IsOpenTopCar               0x6A2350
#define FUNC_CAutomobile_PlaceOnRoadProperly        0x6AF420
#define FUNC_CAutomobile_PlayCarHorn                0x6A3770
#define FUNC_CAutomobile_PopBoot                    0x6AF910
#define FUNC_CAutomobile_PopBootUsingPhysics        0x6A44D0
#define FUNC_CAutomobile_PopDoor                    0x6ADEF0
#define FUNC_CAutomobile_PopPanel                   0x6ADF80
#define FUNC_CAutomobile_ResetSuspension            0x6A2AE0
#define FUNC_CAutomobile_SetRandomDamage            0x6A2530
#define FUNC_CAutomobile_SetTaxiLight               0x6A3740
#define FUNC_CAutomobile_SetTotalDamage             0x6A27F0
#define FUNC_CAutomobile_SpawnFlyingComponent       0x6A8580

#define FUNC_CAutomobile__SpawnFlyingComponent      0x6a8580
#define FUNC_CAutomobile__UpdateMovingCollision     0x6a1460

class CVehicleUnknown
{
public:
    short                       m_unk;                                  // 0

    BYTE                        m_pad[30];                              // 2
};

#define AUTOMOBILE_TAXILIGHTS       0x0001


struct CUpgradeAssocStoreSA
{
    unsigned short FindModelAssociation( unsigned short model )
    {
        for ( unsigned char n = 0; n < m_count; n++ )
        {
            if ( primary[n] == model )
                return secondary[n];
            if ( secondary[n] == model )
                return primary[n];
        }

        return 0xFFFF;
    }

    unsigned short primary[30];
    unsigned short secondary[30];

    unsigned int m_count;
};

extern CUpgradeAssocStoreSA *const g_upgStore;

class CAutomobileSAInterface : public CVehicleSAInterface   // size: 2440
{
public:
                                CAutomobileSAInterface( bool unk, unsigned short model, unsigned char createdBy );
                                ~CAutomobileSAInterface();

    void                        SetModelIndex( unsigned short id );
    void                        AddUpgrade( unsigned short model );
    bool                        UpdateComponentStatus( unsigned short model, unsigned char collFlags, unsigned short *complex );
    void                        UpdateNitrous( unsigned char rounds );

    CDamageManagerSAInterface   m_damage;                               // 1440

    CDoorSAInterface            m_doors[MAX_DOORS];                     // 1464

    RwFrame*                    m_components[NUM_VEHICLE_COMPONENTS];   // 1608

    CVehicleUnknown             m_unk2[3];                              // 1708

    unsigned int                m_unk3[6];                              // 1804

    BYTE                        m_pad6[296];                            // 1828
    float                       m_rotorSpeed;                           // 2124

    BYTE                        m_pad20[24];                            // 2128
    unsigned char               m_autoFlags;                            // 2152
    BYTE                        m_pad22;                                // 2153
    unsigned short              m_unk5;                                 // 2154

    unsigned short              m_adjustableProperty;                   // 2156

    BYTE                        m_pad7[54];                             // 2158
    float                       m_nitrousFuel;                          // 2212

    BYTE                        m_pad8[60];                             // 2216
    float                       m_burningTime;                          // 2276

    BYTE                        m_pad21[100];                           // 2280

    float                       m_turretHorizontal;                     // 2380
    float                       m_turretVertical;                       // 2384
    BYTE                        m_pad11[52];                            // 2388
};

class CAutomobileSA : public virtual CAutomobile, public CVehicleSA
{
public:
                                CAutomobileSA( CAutomobileSAInterface *automobile );
                                ~CAutomobileSA();

    inline CAutomobileSAInterface*  GetInterface()                                      { return (CAutomobileSAInterface*)m_pInterface; }
    inline const CAutomobileSAInterface*    GetInterface() const                        { return (const CAutomobileSAInterface*)m_pInterface; }

    CDoorSA*                    GetDoor( unsigned char ucDoor );
    void                        SetSwingingDoorsAllowed( bool bAllowed )                { m_swingingDoorsAllowed = bAllowed; }
    bool                        AreSwingingDoorsAllowed() const                         { return m_swingingDoorsAllowed; }

    void                        ExtinguishCarFire();
    void                        PlaceOnRoadProperly();

    CDamageManagerSA*           GetDamageManager()                                      { return m_damageManager; }

    inline float                GetBurningTime() const                                  { return GetInterface()->m_burningTime; }
    inline void                 SetBurningTime( float time )                            { GetInterface()->m_burningTime = time; }

    inline float                GetNitrousFuel() const                                  { return GetInterface()->m_nitrousFuel; }
    inline void                 SetNitrousFuel( float val )                             { GetInterface()->m_nitrousFuel = val; }

    inline SColor               GetHeadLightColor() const                               { return m_HeadLightColor; }
    inline void                 SetHeadLightColor( const SColor color )                 { m_HeadLightColor = color; }

    CObject*                    SpawnFlyingComponent( int i_1, unsigned int ui_2 );
    void                        SetWheelVisibility( eWheels wheel, bool bVisible );
    bool                        GetWheelVisibility( eWheels wheel ) const;

    unsigned short              GetAdjustablePropertyValue() const                      { return GetInterface()->m_adjustableProperty; }
    void                        SetAdjustablePropertyValue( unsigned short value )      { GetInterface()->m_adjustableProperty = value; }

    void                        SetTaxiLightOn( bool on )                               { BOOL_FLAG( GetInterface()->m_autoFlags, AUTOMOBILE_TAXILIGHTS, on ); }
    bool                        IsTaxiLightOn() const                                   { return IS_FLAG( GetInterface()->m_autoFlags, AUTOMOBILE_TAXILIGHTS ); }

    void                        GetTurretRotation( float& horizontal, float& vertical ) const;
    void                        SetTurretRotation( float horizontal, float vertical );

    void*                       GetPrivateSuspensionLines()                             { return m_suspensionLines; }

    void                        RecalculateHandling();
    void                        RecalculateSuspensionLines();
    bool                        UpdateMovingCollision( float angle );

private:
    CDamageManagerSA*           m_damageManager;
    SColor                      m_HeadLightColor;
    bool                        m_swingingDoorsAllowed;
    void*                       m_suspensionLines;
    CDoorSA*                    m_doors[MAX_DOORS];
};

#endif

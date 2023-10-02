/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWeaponSA.h
*  PURPOSE:     Header file for weapon class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_WEAPON
#define __CGAMESA_WEAPON

#include <game/CWeapon.h>
#include <game/CPed.h>

#define FUNC_Shutdown                                   0x73A380
#define FUNC_CWeapon_CheckForShootingVehicleOccupant    0x73f480

class CWeaponSAInterface
{
public:
    eWeaponType             m_eWeaponType;
    eWeaponState            m_eState;
    unsigned int            m_nAmmoInClip;
    unsigned int            m_nAmmoTotal;
    unsigned long           m_nTimer;
    unsigned int            m_Unknown;
    unsigned int            m_Unknown_2;
};

class CWeaponSA : public CWeapon
{
private:
    CWeaponSAInterface*     m_interface;
    CPedSA*                 m_owner;
    eWeaponSlot             m_weaponSlot;

public:
    CWeaponSA ( CWeaponSAInterface * weaponInterface, CPedSA * ped, eWeaponSlot weaponSlot );

    eWeaponType     GetType ();
    void            SetType ( eWeaponType type );
    eWeaponState    GetState ();
    void            SetState ( eWeaponState state );
    unsigned int    GetAmmoInClip ();
    void            SetAmmoInClip ( unsigned int ammo );
    unsigned int    GetAmmoTotal ();
    void            SetAmmoTotal ( unsigned int ammo );
    
    CPed*           GetPed ();
    CPedSA*         GetPedInternal ();
    eWeaponSlot     GetSlot ();

    void            SetAsCurrentWeapon ();
    CWeaponInfo*    GetInfo ();

    void            Remove ();
};

#endif

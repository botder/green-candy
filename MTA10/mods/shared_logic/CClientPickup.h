/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPickup.h
*  PURPOSE:     Pickup entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientPickup;

#ifndef __CCLIENTPICKUP_H
#define __CCLIENTPICKUP_H

#include "CClientPickupManager.h"
#include "CClientEntity.h"
#include "CClientColShape.h"
#include "CClientColCallback.h"

#define LUACLASS_PICKUP     63

class CClientPickup : public CClientStreamElement, private CClientColCallback
{
    friend class CClientColShape;

public:
    enum
    {
        HEALTH,
        ARMOR,
        WEAPON,
        CUSTOM,
        INVALID = 0xFF,
    };

    enum
    {
        WEAPON_BRASSKNUCKLE = 1, 
        WEAPON_GOLFCLUB,
        WEAPON_NIGHTSTICK,
        WEAPON_KNIFE,
        WEAPON_BASEBALLBAT,
        WEAPON_SHOVEL,
        WEAPON_POOL_CUE,
        WEAPON_KATANA,
        WEAPON_CHAINSAW,

        WEAPON_DILDO1,
        WEAPON_DILDO2,
        WEAPON_VIBE1,
        WEAPON_VIBE2,
        WEAPON_FLOWERS,
        WEAPON_CANE,

        WEAPON_GRENADE,
        WEAPON_TEARGAS,
        WEAPON_MOLOTOV,
        WEAPON_FREEFALL_BOMB = 21,

        WEAPON_PISTOL,
        WEAPON_PISTOL_SILENCED,
        WEAPON_DESERT_EAGLE,
        WEAPON_SHOTGUN,
        WEAPON_SAWNOFF_SHOTGUN,
        WEAPON_SPAS12_SHOTGUN,
        WEAPON_MICRO_UZI,
        WEAPON_MP5,
        WEAPON_AK47,
        WEAPON_M4,          
        WEAPON_TEC9,
        
        WEAPON_COUNTRYRIFLE,
        WEAPON_SNIPERRIFLE, 
        WEAPON_ROCKETLAUNCHER,
        WEAPON_ROCKETLAUNCHER_HS,
        WEAPON_FLAMETHROWER,
        WEAPON_MINIGUN,
        WEAPON_REMOTE_SATCHEL_CHARGE,
        WEAPON_DETONATOR,
        WEAPON_SPRAYCAN,
        WEAPON_EXTINGUISHER,
        WEAPON_CAMERA,
        WEAPON_NIGHTVISION,
        WEAPON_INFRARED,
        WEAPON_PARACHUTE,

        WEAPON_ARMOUR = 48,

        WEAPON_INVALID = 0xFF,
    };

                                CClientPickup( class CClientManager* pManager, ElementID ID, lua_State *L, bool system, unsigned short usModel, CVector vecPosition = CVector( 0, 0, 0 ) );
                                ~CClientPickup();

    void                        Unlink();

    inline eClientEntityType    GetType() const                                             { return CCLIENTPICKUP; };

    inline unsigned short       GetModel()                                                  { return m_usModel; };

    inline CPickup *            GetGamePickup()                                             { return m_pPickup; };
    inline CObject *            GetGameObject()                                             { return m_pObject; }
    CEntity*                    GetGameEntity()                                             { return m_pObject; }
    const CEntity*              GetGameEntity() const                                       { return m_pObject; }

    inline const CVector&       GetPosition()                                               { return m_vecPosition; };
    inline void                 GetPosition( CVector& vecPosition ) const                   { vecPosition = m_vecPosition; };
    void                        SetPosition( const CVector& vecPosition );
    void                        SetModel( unsigned short usModel );

    inline bool                 IsVisible()                                                 { return m_bVisible; };
    void                        SetVisible( bool bVisible );

    inline CClientColShape *    GetColShape()                                               { return m_pCollision; }

    void                        Callback_OnCollision( CClientColShape& Shape, CClientEntity& Entity );
    void                        Callback_OnLeave( CClientColShape& Shape, CClientEntity& Entity );

protected:
    void                        StreamIn( bool bInstantly );
    void                        StreamOut();

private:
    void                        Create();
    void                        Destroy();
    void                        ReCreate();

    CClientPickupManager*       m_pPickupManager;

    unsigned short              m_usModel;
    CPickup*                    m_pPickup;
    CObject*                    m_pObject;
    CVector                     m_vecPosition;
    bool                        m_bVisible;

    CClientColShape*            m_pCollision;

public:
    unsigned char               m_ucType;
    unsigned char               m_ucWeaponType;
    float                       m_fAmount;
    unsigned short              m_usAmmo;
};

#endif

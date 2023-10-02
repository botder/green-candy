/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CHud.h
*  PURPOSE:     HUD interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_HUD
#define __CGAME_HUD

#include <CVector.h>

class CHud
{
public:
    virtual void                SetHelpMessage      ( char *szMessage ) = 0;
    virtual void                SetBigMessage       ( char *szBigMessage ) = 0;
    virtual void                Disable             ( bool bDisabled ) = 0;

    virtual bool                CalcScreenCoors     ( const CVector& worldPos, CVector& screenOut, float& x, float& y ) const = 0;

    virtual void                DisableAmmo         ( bool bDisabled ) = 0;
    virtual void                DisableWeaponIcon   ( bool bDisabled ) = 0;
    virtual void                DisableHealth       ( bool bDisabled ) = 0;
    virtual void                DisableBreath       ( bool bDisabled ) = 0;
    virtual void                DisableArmour       ( bool bDisabled ) = 0;
    virtual void                DisableVitalStats   ( bool bDisabled ) = 0;
    virtual void                DisableMoney        ( bool bDisabled ) = 0;
    virtual void                DisableVehicleName  ( bool bDisabled ) = 0;
    virtual void                DisableHelpText     ( bool bDisabled ) = 0;
    virtual void                DisableAreaName     ( bool bDisabled ) = 0;
    virtual void                DisableRadar        ( bool bDisabled ) = 0;
    virtual void                DisableClock        ( bool bDisabled ) = 0;
    virtual void                DisableRadioName    ( bool bDisabled ) = 0;
    virtual void                DisableWantedLevel  ( bool bDisabled ) = 0;
    virtual void                DisableCrosshair    ( bool bDisabled ) = 0;
    virtual void                DisableAll          ( bool bDisabled ) = 0;
};

#endif

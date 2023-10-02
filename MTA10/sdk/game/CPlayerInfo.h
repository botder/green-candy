/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPlayerInfo.h
*  PURPOSE:     Player entity information interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PLAYERINFO
#define __CGAME_PLAYERINFO

#include "Common.h"
#include "CWanted.h"

class CVehicle;

class CPlayerInfo
{
public:
    virtual long            GetPlayerMoney          ( void ) = 0;
    virtual void            SetPlayerMoney          ( long lMoney ) = 0;

    virtual void            GivePlayerParachute     ( void ) = 0;
    virtual void            StreamParachuteWeapon   ( bool allow ) = 0;

    virtual unsigned short  GetLastTimeEaten        ( void ) = 0;
    virtual void            SetLastTimeEaten        ( unsigned short time ) = 0;

    virtual CWanted*        GetWanted               ( void ) = 0;
    virtual float           GetFPSMoveHeading       ( void ) = 0;

    virtual bool            GetCrossHair            ( float& tarX, float& tarY ) = 0;

    virtual bool            GetDoesNotGetTired      ( void ) = 0;
    virtual void            SetDoesNotGetTired      ( bool award ) = 0;

    virtual CVehicle*       GiveRemoteVehicle       ( unsigned short model, float x, float y, float z ) = 0;
    virtual void            StopRemoteControl       ( void ) = 0;
    virtual CVehicle*       GetRemoteVehicle        ( void ) = 0;

    virtual unsigned int    GetLastTimeBigGunFired  ( void ) = 0;
    virtual void            SetLastTimeBigGunFired  ( unsigned int time ) = 0;

    virtual unsigned int    GetCarTwoWheelCounter   ( void ) const = 0;
    virtual float           GetCarTwoWheelDist      ( void ) const = 0;
    virtual unsigned int    GetCarLess3WheelCounter ( void ) const = 0;
    virtual unsigned int    GetBikeRearWheelCounter ( void ) const = 0;
    virtual float           GetBikeRearWheelDist    ( void ) const = 0;
    virtual unsigned int    GetBikeFrontWheelCounter( void ) const = 0;
    virtual float           GetBikeFrontWheelDist   ( void ) const = 0;
};

#endif

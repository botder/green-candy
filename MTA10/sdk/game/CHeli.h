/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CHeli.h
*  PURPOSE:     Heli vehicle entity interface
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CHeli_H_
#define _CHeli_H_

class CHeli : public virtual CAutomobile
{
public:
    virtual                     ~CHeli()    {}

    virtual void                SetWinchType( eWinchType winchType ) = 0;
    virtual void                PickupEntityWithWinch( CEntity *entity ) = 0;
    virtual void                ReleasePickedUpEntityWithWinch() = 0;
    virtual void                SetRopeHeightForHeli( float height ) = 0;
    virtual CPhysical*          QueryPickedUpEntityWithWinch() = 0;

    virtual bool                IsHeliSearchLightVisible() const = 0;
    virtual void                SetHeliSearchLightVisible( bool vis ) = 0;

    virtual float               GetHeliRotorSpeed() const = 0;
    virtual void                SetHeliRotorSpeed( float speed ) = 0;
};

#endif //_CHeli_H_
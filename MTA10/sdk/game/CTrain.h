/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CTrain.h
*  PURPOSE:     Game pool interface
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _TRAIN_INTERFACE_
#define _TRAIN_INTERFACE_

class CTrain : public virtual CVehicle
{
public:
    virtual                     ~CTrain()    {}

    virtual CTrain*             GetNextTrainCarriage() const = 0;
    virtual void                SetNextTrainCarriage( CTrain *next ) = 0;
    virtual CTrain*             GetPreviousTrainCarriage() const = 0;
    virtual void                SetPreviousTrainCarriage( CTrain *prev ) = 0;

    virtual bool                IsDerailed() const = 0;
    virtual void                SetDerailed( bool bDerailed ) = 0;
    virtual bool                IsDerailable() const = 0;

    virtual void                SetDerailable( bool bDerailable ) = 0;
    virtual float               GetTrainSpeed() const = 0;
    virtual void                SetTrainSpeed( float fSpeed ) = 0;
    virtual bool                GetTrainDirection() const = 0;
    virtual void                SetTrainDirection( bool bDirection ) = 0;

    virtual unsigned char       GetRailTrack() const = 0;
    virtual void                SetRailTrack( unsigned char track ) = 0;
};

#endif //_TRAIN_INTERFACE_
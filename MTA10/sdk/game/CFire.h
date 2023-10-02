/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CFire.h
*  PURPOSE:     Fire interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_FIRE
#define __CGAME_FIRE

class CFire
{
public:
    virtual void                SetPosition( const CVector& pos ) = 0;
    virtual const CVector&      GetPosition() const = 0;
    virtual CEntity*            GetCreator() const = 0;
    virtual CEntity*            GetEntityOnFire() const = 0;

    virtual void                Extinguish() = 0;
    virtual void                Ignite() = 0;
    virtual void                SetTimeToBurnOut( unsigned int time ) = 0;
    virtual unsigned int        GetTimeToBurnOut() const = 0;

    virtual bool                IsIgnited() const = 0;
    virtual bool                IsFree() const = 0;
    virtual bool                IsBeingExtinguished() const = 0;

    virtual void                SetTarget( CEntity *entity ) = 0;
    virtual void                SetSilent( bool bSilent ) = 0;

    virtual float               GetStrength() const = 0;
    virtual void                SetStrength( float fStrength ) = 0;

    virtual void                SetNumGenerationsAllowed( char generations ) = 0;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CObject.h
*  PURPOSE:     Object entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_OBJECT
#define __CGAME_OBJECT

#include "CPhysical.h"

class CVehicle;
class CPed;
class CObjectSAInterface;

class CObject : public virtual CPhysical
{
public:
    virtual                         ~CObject        ( void ) {};

    virtual unsigned int            GetPoolIndex    ( void ) const = 0;

    virtual void                    Explode         ( void ) = 0;
    virtual void                    Break           ( void ) = 0;
    virtual void                    SetScale        ( const CVector *scale ) = 0;
    virtual void                    SetScaleValue   ( float fScale ) = 0;
    virtual const CVector*          GetScale        ( void ) const = 0;
    virtual float                   GetScaleValue   ( void ) const = 0;
    virtual void                    SetHealth       ( float fHealth ) = 0;
    virtual float                   GetHealth       ( void ) const = 0;
    virtual void                    SetModelIndex   ( unsigned short ulModel ) = 0;

    virtual CRpAtomic*              CloneAtomic     ( void ) const = 0;

    virtual void                    SetAlpha        ( unsigned char ucAlpha ) = 0;
    virtual unsigned char           GetAlpha        ( void ) const = 0;

    virtual bool                    IsAGangTag      ( void ) const = 0;
};

#endif

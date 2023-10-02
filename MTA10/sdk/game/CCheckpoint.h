/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CCheckpoint.h
*  PURPOSE:     Checkpoint entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CHECKPOINT
#define __CGAME_CHECKPOINT

/**
 * \todo fix SetColor/GetColor, the format is actually BGRA (strange)
 */

enum
{
    CHECKPOINT_TUBE,        // a long tube with arrow as direction
    CHECKPOINT_ENDTUBE,     // finish flag long tube checkpoint
    CHECKPOINT_EMPTYTUBE,   // just a long tube checkpoint
    CHECKPOINT_TORUS,
    CHECKPOINT_TORUS_NOFADE,
    CHECKPOINT_TORUSROT,
    CHECKPOINT_TORUSTHROUGH,
    CHECKPOINT_TORUS_UPDOWN,
    CHECKPOINT_TORUS_DOWN,
    CHECKPOINT_NUM
};

class CCheckpoint
{
public:
    virtual void                SetPosition( const CVector& pos ) = 0;
    virtual const CVector&      GetPosition() const = 0;

    virtual void                SetPointDirection( const CVector& dir ) = 0;
    virtual const CVector&      GetPointDirection() const = 0;

    virtual unsigned int        GetType() const = 0;
    virtual bool                IsActive() const = 0;
    virtual unsigned int        GetIdentifier() const = 0;

    virtual SColor              GetColor() const = 0;
    virtual void                SetColor( SColor color ) = 0;

    virtual void                SetPulsePeriod( unsigned short period ) = 0;
    virtual void                SetRotateRate( short rate ) = 0;

    virtual float               GetSize() const = 0;
    virtual void                SetSize( float size ) = 0;

    virtual void                SetCameraRange( float range ) = 0;
    virtual float               GetPulseFraction() const = 0;

    virtual void                Remove() = 0;
};

#endif

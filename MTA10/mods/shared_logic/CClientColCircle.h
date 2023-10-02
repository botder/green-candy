/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColCircle.h
*  PURPOSE:     Circle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLCIRCLE_H
#define __CCLIENTCOLCIRCLE_H

class CClientColCircle : public CClientColShape
{
public:
                            CClientColCircle( CClientManager *manager, ElementID ID, lua_State *L, bool system, const CVector& pos, float fRadius );

    virtual CSphere         GetWorldBoundingSphere();

    eColShapeType           GetShapeType()                                  { return COLSHAPE_CIRCLE; }

    bool                    DoHitDetection( const CVector& pos, float radius );

    float                   GetRadius() const                               { return m_fRadius; }
    void                    SetRadius( float fRadius )                      { m_fRadius = fRadius; SizeChanged(); }

protected:
    float                   m_fRadius;
};

#endif

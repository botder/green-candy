/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColSphere.h
*  PURPOSE:     Sphere-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLSPHERE_H
#define __CCLIENTCOLSPHERE_H

class CClientColSphere : public CClientColShape
{
public:
                            CClientColSphere( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition, float fRadius );

    virtual CSphere         GetWorldBoundingSphere();

    eColShapeType           GetShapeType()                          { return COLSHAPE_SPHERE; }

    bool                    DoHitDetection( const CVector& vecNowPosition, float fRadius );

    float                   GetRadius()                             { return m_fRadius; }
    void                    SetRadius( float fRadius )              { m_fRadius = fRadius; SizeChanged (); }

protected:
    float                   m_fRadius;
};

#endif

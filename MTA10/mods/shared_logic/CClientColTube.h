/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColTube.h
*  PURPOSE:     Tube-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLTUBE_H
#define __CCLIENTCOLTUBE_H

class CClientColTube : public CClientColShape
{
public:
                            CClientColTube( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition, float fRadius, float fHeight );

    virtual CSphere         GetWorldBoundingSphere();

    eColShapeType           GetShapeType()                              { return COLSHAPE_TUBE ; }

    bool                    DoHitDetection( const CVector& vecNowPosition, float fRadius );

    inline float            GetRadius()                                 { return m_fRadius; }
    inline void             SetRadius( float fRadius )                  { m_fRadius = fRadius; SizeChanged (); }
    inline float            GetHeight()                                 { return m_fHeight; }
    inline void             SetHeight( float fHeight )                  { m_fHeight = fHeight; SizeChanged (); }

protected:
    float                   m_fRadius;
    float                   m_fHeight;
};

#endif

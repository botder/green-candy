/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColRectangle.h
*  PURPOSE:     Rectangle-shaped collision entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLRECTANGLE_H
#define __CCLIENTCOLRECTANGLE_H

class CClientColRectangle : public CClientColShape
{
public:
                            CClientColRectangle( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition, const CVector2D& vecSize );

    virtual CSphere         GetWorldBoundingSphere();

    eColShapeType           GetShapeType()                                  { return COLSHAPE_RECTANGLE; }

    bool                    DoHitDetection( const CVector& vecNowPosition, float fRadius );

    inline const CVector2D& GetSize()                                       { return m_vecSize; }
    inline void             SetSize( const CVector2D& vecSize )             { m_vecSize = vecSize; SizeChanged (); }

protected:
    CVector2D               m_vecSize;
};

#endif
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColPolygon.h
*  PURPOSE:     Polygon-shaped collision entity class
*  DEVELOPERS:  Stanislav Bobrov <lil_Toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLPOLYGON_H
#define __CCLIENTCOLPOLYGON_H

class CClientColPolygon : public CClientColShape
{
public:
                                CClientColPolygon( CClientManager* pManager, ElementID ID, lua_State *L, bool system, const CVector& vecPosition );

    virtual CSphere             GetWorldBoundingSphere();

    eColShapeType               GetShapeType()                                      { return COLSHAPE_POLYGON; }

    bool                        DoHitDetection( const CVector& vecNowPosition, float fRadius );

    void                        SetPosition( const CVector& vecPosition );

    void                        AddPoint( CVector2D vecPoint );

    typedef std::vector <CVector2D> pointList_t;

    unsigned int                CountPoints() const                                 { return m_Points.size(); }
    pointList_t::const_iterator IterBegin()                                         { return m_Points.begin(); }
    pointList_t::const_iterator IterEnd()                                           { return m_Points.end(); }

protected:
    pointList_t                 m_Points;

    bool                        ReadSpecialData();
    bool                        IsInBounds( CVector vecPoint );

    float                       m_fRadius;
};

#endif

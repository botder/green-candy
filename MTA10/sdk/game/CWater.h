/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CWater.h
*  PURPOSE:     Water interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWATER_H
#define __CWATER_H

enum EWaterPolyType
{
    WATER_POLY_NONE,
    WATER_POLY_QUAD,
    WATER_POLY_TRIANGLE,
    WATER_POLY_LIST
};

#define _WATER_VISIBLE 1
#define _WATER_SHALLOW 2

class CWaterVertex abstract
{
public:
    virtual unsigned short      GetID() const = 0;
    virtual void                GetPosition( CVector& vec ) const = 0;
    virtual bool                SetPosition( const CVector& vec, void *pChangeSource = NULL ) = 0;
};

class CWaterPoly abstract
{
public:
    virtual EWaterPolyType      GetType() const = 0;
    virtual unsigned short      GetID() const = 0;
    virtual size_t              GetNumVertices() const = 0;
    virtual CWaterVertex*       GetVertex( unsigned int idx ) = 0;
    virtual bool                ContainsPoint( float x, float y ) const = 0;
};

#endif

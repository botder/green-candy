/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarArea.h
*  PURPOSE:     Radar area entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTRADARAREA_H
#define __CCLIENTRADARAREA_H

#include "CClientEntity.h"
#include <gui/CGUI.h>

#define LUACLASS_RADARAREA  49

class CClientRadarAreaManager;

class CClientRadarArea : public CClientEntity
{
    friend class CClientRadarAreaManager;
public:
                                CClientRadarArea( class CClientManager *pManager, ElementID ID, lua_State *L, bool system );
                                ~CClientRadarArea();

    void                        Unlink();

    inline eClientEntityType    GetType() const                                         { return CCLIENTRADARAREA; }

    inline const CVector2D&     GetPosition() const                                     { return m_vecPosition; }
    inline void                 GetPosition( CVector2D& vecBottomLeft ) const           { vecBottomLeft = m_vecPosition; }
    inline void                 GetPosition( CVector& vecBottomLeft ) const             { vecBottomLeft = CVector ( m_vecPosition.fX, m_vecPosition.fY, 0.0f ); }
    inline void                 SetPosition( const CVector2D& vecBottomLeft )           { m_vecPosition = vecBottomLeft; }
    inline void                 SetPosition( const CVector& vecBottomLeft )             { m_vecPosition = CVector2D ( vecBottomLeft.fX, vecBottomLeft.fY ); }

    inline const CVector2D&     GetSize() const                                         { return m_vecSize; }
    inline void                 GetSize( CVector2D& vecSize )                           { vecSize = m_vecSize; }
    inline void                 SetSize( const CVector2D& vecSize )                     { m_vecSize = vecSize; }

    inline SColor               GetColor() const                                        { return m_Color; }
    void                        SetColor( const SColor color )                          { m_Color = color; }

    inline bool                 IsFlashing() const                                      { return m_bFlashing; }
    inline void                 SetFlashing( bool bFlashing )                           { m_bFlashing = bFlashing; }

    void                        SetDimension( unsigned short usDimension );
    void                        RelateDimension( unsigned short usDimension );

protected:
    void                        DoPulse();
    void                        DoPulse( bool bRender );

    CClientRadarAreaManager*    m_pRadarAreaManager;

    CVector2D                   m_vecPosition;
    CVector2D                   m_vecSize;
    SColor                      m_Color;

    bool                        m_bStreamedIn;
    bool                        m_bFlashing;
    unsigned long               m_ulFlashCycleStart;

};

#endif

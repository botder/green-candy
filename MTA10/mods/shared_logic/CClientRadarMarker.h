/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarker.h
*  PURPOSE:     Radar marker entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CClientRadarMarker;

#ifndef __CCLIENTRADARMARKER_H
#define __CCLIENTRADARMARKER_H

#include "CClientCommon.h"
#include "CClientEntity.h"
#include "CClientRadarMarkerManager.h"
#include <gui/CGUI.h>

#include <game/CMarker.h>

#define LUACLASS_RADARMARKER    50

#define MAP_MARKER_WIDTH 19
#define MAP_MARKER_HEIGHT 20
#define RADAR_MARKER_LIMIT 63


class CClientRadarMarker : public CClientEntity
{
    friend class CClientRadarMarkerManager;
public:
    enum EMapMarkerState
    {
        MAP_MARKER_SQUARE,
        MAP_MARKER_TRIANGLE_UP,
        MAP_MARKER_TRIANGLE_DOWN,
        MAP_MARKER_OTHER,
    };

public:
                                        CClientRadarMarker( class CClientManager *pManager, ElementID ID, lua_State *L, bool system, short usOrdering = 0, unsigned short usVisibleDistance = 16383 );
                                        ~CClientRadarMarker();

    void                                Unlink();

    inline eClientEntityType            GetType() const                                             { return CCLIENTRADARMARKER; };

    void                                DoPulse();

    inline bool                         IsVisible() const                                           { return m_bIsVisible; }

    void                                SetPosition( const CVector& vecPosition );
    void                                GetPosition( CVector& vecPosition ) const;

    inline unsigned short               GetScale() const                                            { return m_usScale; }
    void                                SetScale( unsigned short usScale );

    inline SColor                       GetColor() const                                            { return m_Color; }
    void                                SetColor( const SColor color );

    inline unsigned long                GetSprite() const                                           { return m_ulSprite; }
    void                                SetSprite( unsigned long ulSprite );

//  inline bool                         IsVisible() const                                           { return m_pMarker != NULL; }
    void                                SetVisible( bool bVisible );

    void                                SetDimension( unsigned short usDimension );
    void                                RelateDimension( unsigned short usDimension );

    inline short                        GetOrdering() const                                         { return m_sOrdering; }
    void                                SetOrdering( short sOrdering );

    inline unsigned short               GetVisibleDistance() const                                  { return m_usVisibleDistance; }
    inline void                         SetVisibleDistance( unsigned short usVisibleDistance )      { m_usVisibleDistance = usVisibleDistance; }

    bool                                IsInVisibleDistance() const;

private:
    void                                GetSquareTexture( DWORD dwBitMap[] );
    void                                GetUpTriangleTexture( DWORD dwBitMap[] );
    void                                GetDownTriangleTexture( DWORD dwBitMap[] );

private:
    bool                                Create();
    void                                InternalCreate();
    void                                Destroy();    

    void                                CreateMarker();
    void                                DestroyMarker();

    class CClientRadarMarkerManager*    m_pRadarMarkerManager;
    CMarker*                            m_pMarker;

    CVector                             m_vecPosition;
    unsigned short                      m_usScale;
    SColor                              m_Color;
    unsigned long                       m_ulSprite;

    bool                                m_bIsVisible;
    short                               m_sOrdering;
    unsigned short                      m_usVisibleDistance;
};

#endif

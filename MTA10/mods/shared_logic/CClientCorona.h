/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCorona.h
*  PURPOSE:     Corona marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#ifndef __CCLIENTCORONA_H
#define __CCLIENTCORONA_H

#include "CClientMarker.h"
class CClientMarker;

class CClientCorona : public CClientMarkerCommon
{
public:
                                    CClientCorona( CClientMarker * pThis );
                                    ~CClientCorona();

    inline unsigned int             GetMarkerType() const                           { return CClientMarkerCommon::CLASS_CORONA; }

    bool                            IsHit( const CVector& vecPosition ) const;

    inline void                     GetPosition( CVector& vecPosition ) const       { vecPosition = m_Matrix.vPos; }
    inline void                     SetPosition( const CVector& vecPosition )       { m_Matrix.vPos = vecPosition; }

    inline void                     GetMatrix( RwMatrix& mat ) const                { mat = m_Matrix; }
    inline void                     SetMatrix( const RwMatrix& mat )                { m_Matrix = mat; }

    inline bool                     IsVisible() const                               { return m_bVisible; };
    inline void                     SetVisible( bool bVisible )                     { m_bVisible = bVisible; };

    inline SColor                   GetColor() const                                { return m_Color; }
    inline void                     SetColor( const SColor color )                  { m_Color = color; }

    inline float                    GetSize() const                                 { return m_fSize; };
    inline void                     SetSize( float fSize )                          { m_fSize = fSize; };

protected:
    bool                            IsStreamedIn() const                            { return m_bStreamedIn; };
    void                            StreamIn();
    void                            StreamOut();

    void                            DoPulse();

private:
    CClientMarker*                  m_pThis;
    bool                            m_bStreamedIn;
    unsigned long                   m_ulIdentifier;
    RwMatrix                        m_Matrix;
    bool                            m_bVisible;
    float                           m_fSize;
    SColor                          m_Color;
    CCoronas*                       m_pCoronas;
};

#endif

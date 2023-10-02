/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClient3DMarker.h
*  PURPOSE:     3D marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENT3DMARKER_H
#define __CCLIENT3DMARKER_H

#include "CClientMarker.h"
class CClientMarker;

class CClient3DMarker : public CClientMarkerCommon
{
public:
    enum
    {
        TYPE_CYLINDER,
        TYPE_ARROW,
        TYPE_INVALID,
    };

                                    CClient3DMarker( CClientMarker *pThis );
                                    ~CClient3DMarker();

    inline unsigned int             GetMarkerType() const                               { return CClientMarkerCommon::CLASS_3DMARKER; }

    unsigned long                   Get3DMarkerType() const;
    void                            Set3DMarkerType( unsigned long type );

    bool                            IsHit( const CVector& pos ) const;

    inline void                     GetPosition( CVector& pos ) const                   { pos = m_Matrix.vPos; }
    inline void                     SetPosition( const CVector& pos )                   { m_Matrix.vPos = pos; }

    inline void                     GetMatrix( RwMatrix& mat ) const                    { mat = m_Matrix; }
    inline void                     SetMatrix( const RwMatrix& mat )                    { m_Matrix = mat; }

    inline bool                     IsVisible() const                                   { return m_visible; }
    inline void                     SetVisible( bool bVisible )                         { m_visible = bVisible; }

    inline SColor                   GetColor() const                                    { return m_color; }
    inline void                     SetColor( const SColor color )                      { m_color = color; }

    inline float                    GetSize() const                                     { return m_size; }
    inline void                     SetSize( float fSize )                              { m_size = fSize; }

    inline float                    GetPulseFraction() const                            { return m_pMarker->GetPulseFraction(); }
    void                            SetPulseFraction( float fFraction )                 { m_pMarker->SetPulseFraction( fFraction ); }

protected:
    void                            StreamIn();
    void                            StreamOut();

    void                            DoPulse();

private:
    CClientMarker*                  m_pThis;
    RwMatrix                        m_Matrix;

    bool                            m_visible;
    e3DMarkerType                   m_type;
    float                           m_size;
    SColor                          m_color;
    C3DMarker*                      m_pMarker;
    unsigned int                    m_id;
    bool                            m_streamedIn;
};

#endif

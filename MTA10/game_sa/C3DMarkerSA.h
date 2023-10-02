/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/C3DMarkerSA.h
*  PURPOSE:     Header file for 3D Marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_3DMARKER
#define __CGAMESA_3DMARKER

#include <game/C3DMarker.h>
#include "Common.h"

#define FUNC_DeleteMarkerObject         0x722390

class C3DMarkerSAInterface  //size: 160
{
public:
    void            DeleteRwObject( void );

    RwMatrix        m_mat;                          // 0, local space to world space transform
    unsigned int    dwPad,dwPad2;                   // 64/68, not sure why we need these, it appears to be this way though (eAi)
    RpAtomic*       m_pRwObject;                    // 72
    RpMaterial*     m_pMaterial;                    // 76
    
    unsigned short  m_nType;                        // 80
    bool            m_bIsUsed;                      // 82, has this marker been allocated this frame?
    bool            m_bUnkFlag;                     // 83
    unsigned int    m_nIdentifier;                  // 84

    RwColor         rwColour;                       // 88 
    unsigned short  m_nPulsePeriod;                 // 92
    short           m_nRotateRate;                  // 94, deg per frame (in either direction)
    unsigned int    m_nStartTime;                   // 96
    float           m_fPulseFraction;               // 100
    float           m_fStdSize;                     // 104
    float           m_fSize;                        // 108
    float           m_fBrightness;                  // 112
    float           m_fCameraRange;                 // 116

    CVector         m_normal;                       // 120, Normal of the object point at
    // the following variables remember the last time we read the heigh of the
    // map. Using this we don't have to do this every frame and we can still have moving markers.
    unsigned short  m_LastMapReadX, m_LastMapReadY; // 132 / 134
    float           m_LastMapReadResultZ;           // 136
    float           m_roofHeight;                   // 140
    CVector         m_lastPosition;                 // 144
    unsigned int    m_OnScreenTestTime;             // 156, time last screen check was done
};

class C3DMarkerSA : public C3DMarker
{
private:
    C3DMarkerSAInterface*           m_interface;

public:
    C3DMarkerSA( C3DMarkerSAInterface *marker )             { m_interface = marker; }

    C3DMarkerSAInterface*   GetInterface()                  { return m_interface; }
    const C3DMarkerSAInterface* GetInterface() const        { return m_interface; }

    void            GetMatrix( RwMatrix& matrix ) const;
    void            SetMatrix( const RwMatrix& matrix );

    void            SetPosition( const CVector& pos );
    const CVector&  GetPosition() const;

    unsigned int    GetType() const; // need enum?
    void            SetType( unsigned int type ); // doesn't work propperly (not virtualed)
    bool            IsActive() const;
    unsigned int    GetIdentifier() const;

    SColor          GetColor() const;
    void            SetColor( const SColor color ); // actually BGRA

    void            SetRotateRate( short rate );

    void            SetPulsePeriod( unsigned short period);
    void            SetPulseFraction( float fraction ); // doesn't work propperly (not virtualed)
    float           GetPulseFraction() const;

    float           GetSize() const;
    void            SetSize( float size );

    float           GetBrightness() const;
    void            SetBrightness( float brightness );

    void            SetCameraRange( float range );

    void            Disable();
    void            DeleteMarkerObject();
    void            Reset();
    void            SetActive()                             { m_interface->m_bIsUsed = true; }
};

#endif

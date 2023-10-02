/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientStreamElement.h
*  PURPOSE:     Streamed entity class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CClientStreamElement_H
#define __CClientStreamElement_H

#include "CClientEntity.h"
class CClientStreamer;
class CClientStreamSector;
class CClientStreamSectorRow;

#define LUACLASS_STREAMELEMENT  55

class CClientStreamElement : public CClientEntity
{
    friend class CClientStreamer;
public:
                                CClientStreamElement( CClientStreamer *pStreamer, ElementID ID, lua_State *L, bool system );
                                ~CClientStreamElement();

    void                        UpdateStreamPosition( const CVector& vecPosition );
    CVector                     GetStreamPosition() const                                           { return m_vecStreamPosition; }
    CClientStreamSectorRow*     GetStreamRow()                                                      { return m_pStreamRow; }
    CClientStreamSector*        GetStreamSector()                                                   { return m_pStreamSector; }
    bool                        IsStreamedIn()                                                      { return m_bStreamedIn; }

    void                        InternalStreamIn( bool bInstantly );
    void                        InternalStreamOut();
    virtual void                StreamIn( bool bInstantly ) = 0;
    virtual void                StreamOut() = 0;

    virtual void                NotifyCreate();
    void                        NotifyUnableToCreate();

    void                        AddStreamReference( bool bScript = false );
    void                        RemoveStreamReference( bool bScript = false );

    unsigned short              GetStreamReferences( bool bScript = false );
    unsigned long               GetTotalStreamReferences()                                          { return m_usStreamReferences + m_usStreamReferencesScript; }

    void                        StreamOutForABit();

    void                        SetDimension( unsigned short usDimension );
    float                       GetExpDistance()                                                    { return m_fExpDistance; }
    virtual CSphere             GetWorldBoundingSphere();
    float                       GetDistanceToBoundingBoxSquared( const CVector& vecPosition );

    bool                        IsStreamingCompatibleClass()                                        { return true; }

private:
    void                        SetStreamRow( CClientStreamSectorRow *pRow )                        { m_pStreamRow = pRow; }
    void                        SetStreamSector( CClientStreamSector *pSector )                     { m_pStreamSector = pSector; }
    void                        SetExpDistance( float fDistance )                                   { m_fExpDistance = fDistance; }
    
    CClientStreamer*            m_pStreamer;
    CClientStreamSectorRow*     m_pStreamRow;
    CClientStreamSector*        m_pStreamSector;
    CVector                     m_vecStreamPosition;    
    float                       m_fExpDistance;
    bool                        m_bAttemptingToStreamIn;
    bool                        m_bStreamedIn;
    unsigned short              m_usStreamReferences, m_usStreamReferencesScript;

public:
    float                       m_fCachedRadius;
    int                         m_iCachedRadiusCounter;
    CVector                     m_vecCachedBoundingBox[2];
    int                         m_iCachedBoundingBoxCounter;
};

#endif

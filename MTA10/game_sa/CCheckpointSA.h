/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCheckpointSA.h
*  PURPOSE:     Header file for checkpoint entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_CHECKPOINT
#define __CGAMESA_CHECKPOINT

#include <game/CCheckpoint.h>
#include "Common.h"

class CCheckpointSAInterface
{
public:
    unsigned short          m_type;
    bool                    m_isUsed;
    bool                    m_rotFlag;
    unsigned int            m_id;
    RwColor                 m_rwColor;
    unsigned short          m_pulsePeriod;
    short                   m_rotateRate;  // deg per frame (in either direction)
    CVector                 m_pos;
    CVector                 m_pointDir;
    float                   m_pulseFraction;
    float                   m_size;
    float                   m_cameraRange;
    float                   m_multiSize;    
};

class CCheckpointSA : public CCheckpoint
{
public:
                                CCheckpointSA( CCheckpointSAInterface *intf )       { m_interface = intf; }

    CCheckpointSAInterface*     GetInterface()                                      { return m_interface; }
    const CCheckpointSAInterface*   GetInterface() const                            { return m_interface; }

    void                        SetPosition( const CVector& pos )                   { GetInterface()->m_pos = pos; }
    const CVector&              GetPosition() const                                 { return GetInterface()->m_pos; }

    void                        SetPointDirection( const CVector& dir )             { GetInterface()->m_pointDir = dir; }
    const CVector&              GetPointDirection() const                           { return GetInterface()->m_pointDir; }

    void                        SetType( unsigned short type )                      { GetInterface()->m_type = type; }
    unsigned int                GetType() const                                     { return GetInterface()->m_type; }
    bool                        IsActive() const                                    { return GetInterface()->m_isUsed; }
    void                        Activate()                                          { GetInterface()->m_isUsed = true; }

    void                        SetIdentifier( unsigned int id )                    { GetInterface()->m_id = id; }
    unsigned int                GetIdentifier() const                               { return GetInterface()->m_id; }

    SColor                      GetColor() const;
    void                        SetColor( SColor color );

    void                        SetPulsePeriod( unsigned short period )             { GetInterface()->m_pulsePeriod = period; }
    void                        SetRotateRate( short rate );

    float                       GetSize() const                                     { return GetInterface()->m_size; }
    void                        SetSize( float size )                               { GetInterface()->m_size = size; }

    void                        SetCameraRange( float range )                       { GetInterface()->m_cameraRange = range; }
    void                        SetPulseFraction( float fraction )                  { GetInterface()->m_pulseFraction = fraction; }
    float                       GetPulseFraction() const                            { return GetInterface()->m_pulseFraction; }

    void                        Remove();

private:
    CCheckpointSAInterface*     m_interface;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCheckpoint.h
*  PURPOSE:     Checkpoint marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCHECKPOINT_H
#define __CCLIENTCHECKPOINT_H

#include "CClientMarkerCommon.h"

class CClientMarker;

class CClientCheckpoint : public CClientMarkerCommon
{
    friend class CClientMarkerManager;
public:
    enum
    {
        TYPE_NORMAL,
        TYPE_RING,
        TYPE_INVALID,
    };

    enum
    {
        ICON_NONE,
        ICON_ARROW,
        ICON_FINISH,
    };

                                    CClientCheckpoint( CClientMarker *pThis );
                                    ~CClientCheckpoint();

    inline unsigned int             GetMarkerType() const                                   { return CLASS_CHECKPOINT; }

    unsigned long                   GetCheckpointType() const;
    void                            SetCheckpointType( unsigned long type );

    bool                            IsHit( const CVector& pos ) const;
    
    inline void                     GetPosition( CVector& pos ) const                       { pos = m_Matrix.vPos; }
    void                            SetPosition( const CVector& pos );
    void                            SetDirection( const CVector& dir );
    void                            SetNextPosition( const CVector& pos );

    void                            GetMatrix( RwMatrix& mat ) const;
    void                            SetMatrix( const RwMatrix& mat );

    inline bool                     IsVisible() const                                       { return m_visible; }
    void                            SetVisible( bool bVisible );

    inline unsigned int             GetIcon() const                                         { return m_icon; }
    void                            SetIcon( unsigned int uiIcon );
    
    inline SColor                   GetColor() const                                        { return m_color; }
    void                            SetColor( const SColor color );

    inline float                    GetSize() const                                         { return m_size; }
    void                            SetSize( float fSize );

    inline bool                     HasTarget() const                                       { return m_hasTarget; }
    inline void                     SetHasTarget( bool has )                                { m_hasTarget = has; }
    inline void                     GetTarget( CVector& target ) const                      { target = m_target; }
    inline void                     SetTarget( const CVector& target )                      { m_target = target; }

    static unsigned char            StringToIcon( const char *str );
    static bool                     IconToString( unsigned char ucIcon, char *str );
    void                            ReCreateWithSameIdentifier();

protected:
    bool                            IsStreamedIn() const                                    { return m_streamedIn; }
    void                            StreamIn();
    void                            StreamOut();

private:
    void                            Create( unsigned int id = 0 );
    void                            Destroy();
    void                            ReCreate();

    CClientMarker*                  m_pThis;
    bool                            m_streamedIn;
    RwMatrix                        m_Matrix;
    CVector                         m_dir;
    CVector                         m_targetPosition;
    bool                            m_visible;
    unsigned int                    m_icon;
    unsigned int                    m_type;
    float                           m_size;
    SColor                          m_color;
    CCheckpoint*                    m_checkpoint;

    unsigned int                    m_id;    
    bool                            m_hasTarget;
    CVector                         m_target;
};

#endif

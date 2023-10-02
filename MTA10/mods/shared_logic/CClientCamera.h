/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCamera.h
*  PURPOSE:     Camera entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTCAMERA_H
#define __CCLIENTCAMERA_H

#include "CClientCommon.h"
#include "CClientEntity.h"

#define LUACLASS_CAMERA     42

class CClientEntity;
class CClientManager;
class CClientPlayer;
class CClientVehicle;
class CClientPlayerManager;

enum eClientCameraAttachMode
{
    CLIENTCAMERA_3RDPERSON,
};

class CClientCamera : public CClientEntity
{
    friend class CClientManager;

public:
    inline void                 Unlink()                                        {}

    void                        DoPulse();

    inline eClientEntityType    GetType() const                                 { return CCLIENTCAMERA; }

    bool                        SetMatrix( const RwMatrix& mat );
    bool                        GetMatrix( RwMatrix& mat ) const;
    void                        GetPosition( CVector& pos ) const;
    void                        GetRotation( CVector& rot ) const;
    void                        SetPosition( const CVector& pos );
    void                        SetRotation( const CVector& rot );
    void                        GetTarget( CVector& target ) const;
    void                        SetTarget( const CVector& pos );
    float                       GetRoll() const                                 { return m_fRoll; }
    void                        SetRoll( float roll )                           { m_fRoll = roll; }
    float                       GetFOV() const                                  { return m_fFOV; }
    void                        SetFOV( float fov )                             { m_fFOV = fov; }

    void                        FadeIn( float time );
    void                        FadeOut( float time, unsigned char red, unsigned char green, unsigned char blue );

    inline CClientPlayer*       GetFocusedPlayer()                              { return m_pFocusedPlayer; };
    void                        SetFocus( CClientEntity *entity, eCamMode mode, bool smoothTransition = false );
    void                        SetFocus( CClientPlayer *player, eCamMode mode, bool smoothTransition = false );
    void                        SetFocus( CVector& vecTarget, bool bSmoothTransition );
    void                        SetFocusToLocalPlayer();

    void                        SetCameraViewMode( eVehicleCamMode mode );
    eVehicleCamMode             GetCameraViewMode();
    void                        SetCameraClip( bool objects, bool vehicles );

    void                        SetVehicleInterpolationSource( CClientVehicle *veh );

    inline bool                 IsInFixedMode() const                           { return m_bFixed; }

    void                        ToggleCameraFixedMode( bool enabled );

    CClientEntity*              GetTargetEntity();

    void                        UnreferencePlayer( CClientPlayer *player );

    bool                        IsSphereVisible( const RwSphere& sphere ) const;

private:
                                CClientCamera( CClientManager *manager );
                                ~CClientCamera();

    static bool                 ProcessFixedCamera( CCam *cam );

    void                        SetFocusToLocalPlayerImpl();

    void                        UnreferenceEntity( CClientEntity *entity );    
    void                        InvalidateEntity( CClientEntity *entity );
    void                        RestoreEntity( CClientEntity *entity );

    void                        AssignFixedMode();

    CClientPlayerManager*       m_pPlayerManager;

    CClientPlayer*              m_pFocusedPlayer;
    CClientEntity*              m_pFocusedEntity;
    CEntity*                    m_pFocusedGameEntity;
    bool                        m_bInvalidated;

    RwMatrix                    m_matrix;

    bool                        m_bFixed;
    float                       m_fRoll;
    float                       m_fFOV;

    CCamera*                    m_pCamera;
};

#endif

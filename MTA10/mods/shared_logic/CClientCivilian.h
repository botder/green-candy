/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCivilian.h
*  PURPOSE:     Civilian ped entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

class CClientCivilian;

#ifndef __CCLIENTCIVILIAN_H
#define __CCLIENTCIVILIAN_H

#include <game/CCivilianPed.h>
#include <game/TaskGoTo.h>
#include <game/TaskCarAccessories.h>

#include "CClientCommon.h"
#include "CClientEntity.h"

#define LUACLASS_CIVILIAN   43

class CClientCivilian : public CClientEntity
{
    friend class CClientCivilianManager;

    void                            InstanceLua( lua_State *L );

public:
                                    CClientCivilian( class CClientManager* pManager, int iPedModel, ElementID ID, lua_State *L );
                                    CClientCivilian( class CClientManager* pManager, ElementID ID, lua_State *L, CCivilianPed *pCivilianPed );

                                    ~CClientCivilian();
    
    void                            Unlink();
    
    inline eClientEntityType        GetType() const                                         { return CCLIENTCIVILIAN; };

    inline CCivilianPed*            GetGameCivilian()                                       { return m_pCivilianPed; }

    inline void                     GetPosition( CVector& vecPosition ) const               { m_pCivilianPed->GetPosition( vecPosition ); };
    inline void                     SetPosition( const CVector& vecPosition )               { m_pCivilianPed->SetPosition( vecPosition ); };

    float                           GetCurrentRotation() const                              { return m_pCivilianPed->GetCurrentRotation(); }
    void                            SetCurrentRotation( float rot )                         { m_pCivilianPed->SetCurrentRotation( rot ); }

    inline void                     ModelRequestCallback( unsigned short usModelID )        {};

    float                           GetDistanceFromCentreOfMassToBaseOfModel();

    bool                            GetMatrix( RwMatrix& Matrix ) const;
    bool                            SetMatrix( const RwMatrix& Matrix );

    void                            GetMoveSpeed( CVector& vecMoveSpeed ) const;
    void                            SetMoveSpeed( const CVector& vecMoveSpeed );
    void                            GetTurnSpeed( CVector& vecTurnSpeed ) const;
    void                            SetTurnSpeed( const CVector& vecTurnSpeed );

    bool                            IsVisible();
    void                            SetVisible( bool bVisible );

    float                           GetHealth() const;
    void                            SetHealth( float fHealth );

private:

    void                            Create();
    void                            Destroy();
    void                            ReCreate();

    class CClientCivilianManager*   m_pCivilianManager;

    int                             m_iModel;

    CCivilianPed*                   m_pCivilianPed;
};

#endif

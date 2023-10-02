/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CHeliSA.cpp
*  PURPOSE:     Heli entity class
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CHeliSA::CHeliSA( CHeliSAInterface *heli ) : CAutomobileSA( heli )
{
}

CHeliSA::~CHeliSA()
{
}

void CHeliSA::SetWinchType( eWinchType winchType )
{
    if ( winchType < 4 && winchType != 2 )
        GetInterface()->m_winchType = winchType;
}

void CHeliSA::PickupEntityWithWinch( CEntity* pEntity )
{
    CEntitySA *pEntitySA = dynamic_cast <CEntitySA*> ( pEntity );

    if ( !pEntitySA )
        return;

    DWORD dwFunc = FUNC_CVehicle_PickUpEntityWithWinch;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();

    _asm
    {
        push    dwEntityInterface
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CHeliSA::ReleasePickedUpEntityWithWinch()
{
    DWORD dwFunc = FUNC_CVehicle_ReleasePickedUpEntityWithWinch;
    DWORD dwThis = (DWORD)GetInterface();

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CHeliSA::SetRopeHeightForHeli( float height )
{
    DWORD dwFunc = FUNC_CVehicle_SetRopeHeightForHeli;
    DWORD dwThis = (DWORD) GetInterface();

    _asm
    {
        push    height
        mov     ecx, dwThis
        call    dwFunc
    }
}

CPhysical* CHeliSA::QueryPickedUpEntityWithWinch()
{
    DWORD dwFunc = FUNC_CVehicle_QueryPickedUpEntityWithWinch;
    DWORD dwThis = (DWORD)GetInterface();

    CPhysicalSAInterface *phys;
    _asm
    {
        mov     ecx, dwThis       
        call    dwFunc
        mov     phys, eax
    }

    if ( !phys )
        return NULL;

    switch( phys->nType )
    {
    case ENTITY_TYPE_PED:
        return (CPhysicalSA*)pGame->GetPools()->GetPed( phys );
    case ENTITY_TYPE_VEHICLE:
        return (CPhysicalSA*)pGame->GetPools()->GetVehicle ( phys );
    case ENTITY_TYPE_OBJECT:
        return (CPhysicalSA*)pGame->GetPools()->GetObject( phys );
    }

    return NULL;
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerPedSA.cpp
*  PURPOSE:     Player ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               aru <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define STEALTH_KILL_RANGE 2.5f

CPlayerPedSAInterface::CPlayerPedSAInterface( unsigned int playerIndex, unsigned int unk )
{
    // todo: reven this, it is very complex
    __asm
    {
        mov ecx,this
        mov eax,0x0060D5B0
        push unk
        push playerIndex
        call eax
    }
}

bool CPlayerPedSAInterface::ShouldBeStealthAiming( void )
{
    // Do we have a knife?
    if ( m_currentWeapon != WEAPONTYPE_KNIFE )
        return false;

    CKeyBindsInterface* pKeyBinds = core->GetKeyBinds();

    if ( !pKeyBinds->GetControlState( CONTROL_AIM_WEAPON ) )
        return false;

    // We need to be either crouched, walking or standing
    if ( m_runState != 1 && m_runState != 4 || !pKeyBinds->GetControlState( CONTROL_WALK ) )
        return false;

    // Do we have a target ped?
    if ( !m_target || m_target->nType != ENTITY_TYPE_PED )
        return false;

    // Are we close enough to the target?
    CVector pos, pos2;
    GetPosition( pos );
    m_target->GetPosition( pos2 );

    pos -= pos2;

    if ( pos.Length() > STEALTH_KILL_RANGE )
        return false;

    // Grab our current anim
    if ( !pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation( (RpClump*)GetRwObject() ) )
        return false;

    // GTA:SA checks for stealth killing
    return m_intelligence->TestForStealthKill( (CPedSAInterface*)m_target, false );
}

void CPlayerPedSAInterface::OnFrame()
{
}

/**
 * Constructor for CPlayerPedSA
 */

static CPedClothesDesc* pLocalClothes = 0;
static CWantedSAInterface* pLocalWanted = 0;

CPlayerPedSA::CPlayerPedSA( CPlayerPedSAInterface *ped, unsigned short modelId, bool isLocal ) : CPedSA( ped )
{
    DEBUG_TRACE("CPlayerPedSA::CPlayerPedSA( CPlayerPedSAInterface *ped, unsigned short modelId, bool isLocal )");
    
    SetType( PLAYER_PED );

    m_bIsLocal = isLocal;

    if ( !isLocal )
    {
        SetModelIndex( modelId );

        // Copy the local player data so we're defaulted to something good
        m_pData = new CPlayerPedDataSAInterface( *pGame->GetPlayerInfo()->GetPlayerPed()->GetInterface()->m_playerData );

        // Serialize the data
        m_pData->Serialize();

        // Replace the player ped data in our ped interface with the one we just created
        GetInterface()->m_playerData = m_pData;

        // Set clothes pointer or we'll crash later (TODO: Wrap up with some cloth classes and make it unique per player)
        m_pData->m_pClothes = pLocalClothes;
        m_pData->m_Wanted = pLocalWanted;

        // We do not want this guy to be streamed
        BOOL_FLAG( GetInterface()->m_entityFlags, ENTITY_DISABLESTREAMING, true );
        BOOL_FLAG( GetInterface()->m_entityFlags, ENTITY_NOSTREAM, true );
    }
    else
    {
        m_pData = ped->m_playerData;

        pLocalClothes = m_pData->m_pClothes;
        pLocalWanted = m_pData->m_Wanted;

        m_doNotRemoveFromGame = true;
    }

    // Set default stuff
    m_pData->m_bRenderWeapon = true;
    m_pData->m_fSprintEnergy = 1000.0f;

    SetCanBeShotInVehicle( true );
    SetTestForShotInVehicle( true );
    // Stop remote players targeting eachother, this also stops the local player targeting them (needs to be fixed)
    GetInterface()->pedFlags.bNeverEverTargetThisPed = true;
    GetInterface()->pedFlags.bIsLanding = false;
    GetInterface()->m_rotationSpeed = 7.5;

    if ( !isLocal )
    {
        // Add it to the world now
        pGame->GetWorld()->Add( ped );
    }
}

CPlayerPedSA::~CPlayerPedSA()
{
    DEBUG_TRACE("CPlayerPedSA::~CPlayerPedSA()");

    // Delete the player data
    if ( !m_bIsLocal )
        delete m_pData;
}

CPadSAInterface& CPlayerPedSA::GetJoypad()
{
    if ( m_bIsLocal )
        return *GetInterface()->GetJoypad();

    return CPedSA::GetJoypad();
}

void CPlayerPedSA::OnFrame()
{
    // Update our stealth status
    SetStealthAiming( GetInterface()->ShouldBeStealthAiming() );

    GetInterface()->OnFrame();
}

CWanted* CPlayerPedSA::GetWanted()
{
    return m_pWanted;
}

void CPlayerPedSA::SetSprintEnergy( float energy )
{
    m_pData->m_fSprintEnergy = energy;
}

float CPlayerPedSA::GetSprintEnergy() const
{
    return m_pData->m_fSprintEnergy;
}

void CPlayerPedSA::SetInitialState()
{
    DWORD dwFunction = FUNC_SetInitialState;
    DWORD dwThis = (DWORD) m_pInterface;
    _asm
    {
        push    1
        mov     ecx, dwThis
        call    dwFunction
    }

    // Avoid direction locks for respawning after a jump
    GetInterface()->pedFlags.bIsLanding = false;
    // Avoid direction locks for respawning after a sprint stop
    GetInterface()->m_rotationSpeed = 7.5;
    // This seems to also be causing some movement / direction locks
    SetStayInSamePlace( false );
}

void CPlayerPedSA::SetMoveAnim( eMoveAnim iAnimGroup )
{
    // Set the the new move animation group
    GetInterface()->m_moveAnimGroup = iAnimGroup;

    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CPlayerPed_ReApplyMoveAnims;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

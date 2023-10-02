/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPedSA.cpp
*  PURPOSE:     Ped entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define VALID_POSITION_LIMIT 100000

extern CGameSA* pGame;
extern CBaseModelInfoSAInterface **ppModelInfo;

enum eAnimGroups
{    
    ANIM_GROUP_GOGGLES=32,
    ANIM_GROUP_STEALTH_KN=87,
};

enum eAnimIDs
{
    ANIM_ID_WALK_CIVI = 0,
    ANIM_ID_RUN_CIVI,
    ANIM_ID_SPRINT_PANIC,
    ANIM_ID_IDLE_STANCE,
    ANIM_ID_WEAPON_CROUCH = 55,
    ANIM_ID_GOGGLES_ON = 224,
    ANIM_ID_STEALTH_AIM = 347
};

bool __thiscall CPedSAInterface::IsPlayer( void )
{
    CPedSA *ped = Pools::GetPed( this );

    if ( ped )
    {
        return !ped->IsManagedByGame();
    }

    return false;
}

CPadSAInterface* CPedSAInterface::GetJoypad()
{
    if ( !IsPlayer() )
        return NULL;

    return pGame->GetPadManager()->GetJoypad( 0 );
}

void CPedSAInterface::OnFrame()
{
    // Update player information
    CPedSA *ped = mtaPeds[ (*ppPedPool)->GetIndex( this ) ];

    // Cannot fix if the ped is managed by GTA:SA internally
    if ( !ped )
        return;

    // Update our alpha
    pGame->GetVisibilityPlugins()->SetClumpAlpha( (RpClump*)GetRwObject(), m_areaCode != pGame->GetWorld()->GetCurrentArea() ? 0 : ped->GetAlpha() );

    // Is the player stealth aiming?
    if ( ped->IsStealthAiming() )
    {
        // Grab our current anim
        CAnimBlendAssociation *assoc = pGame->GetAnimManager()->RpAnimBlendClumpGetFirstAssociation( (RpClump*)GetRwObject() );

        if ( assoc )
        {
            // Check we're not doing any important animations
            switch( assoc->GetAnimID() )
            {
            case ANIM_ID_WALK_CIVI:
            case ANIM_ID_RUN_CIVI:
            case ANIM_ID_IDLE_STANCE:
            case ANIM_ID_WEAPON_CROUCH:
            case ANIM_ID_STEALTH_AIM:
                CAnimBlock *block = pGame->GetAnimManager()->GetAnimationBlock( "KNIFE" );

                // The animations have to be loaded first!
                if ( !block )
                    break;

                if ( block->IsLoaded() )
                {
                    // Force the animation
                    pGame->GetAnimManager()->BlendAnimation( (RpClump*)GetRwObject(), ANIM_GROUP_STEALTH_KN, ANIM_ID_STEALTH_AIM, 8.0f );
                }
            }
        }
    }
}

void Ped_Init( void )
{
    // Hook ped management stuff.
    HookInstall( 0x005DF8F0, h_memFunc( &CPedSAInterface::IsPlayer ), 5 );
}

void Ped_Shutdown( void )
{
}

CPedSA::CPedSA( CPedSAInterface *ped )
{
    DEBUG_TRACE("CPedSA::CPedSA( CPedSAInterface *ped )");

    m_poolIndex = (*ppPedPool)->GetIndex( ped );
    mtaPeds[m_poolIndex] = this;

    m_alpha = 0xFF;

    m_pInterface = ped;
    m_intelligence = new CPedIntelligenceSA( GetInterface()->m_intelligence, this );
    m_sound = new CPedSoundSA( &GetInterface()->m_sound );

    for ( unsigned int i = 0; i < WEAPONSLOT_MAX; i++ )
        m_weapons[i] = new CWeaponSA( &GetInterface()->m_weapons[i], this, (eWeaponSlot)i );

    //this->m_pPedIK = new Cm_pPedIKSA(&(pedInterface->m_pPedIK));
}

CPedSA::~CPedSA()
{
    delete m_intelligence;
    delete m_sound;

    for ( unsigned int i = 0; i < WEAPONSLOT_MAX; i++ )
        delete m_weapons[i];

    mtaPeds[m_poolIndex] = NULL;
}

void* CPedSA::operator new ( size_t )
{
    return mtaPedPool->Allocate();
}

void CPedSA::operator delete ( void *ptr )
{
    return mtaPedPool->Free( (CPedSA*)ptr );
}

/**
 * \todo Reimplement weapons and PedIK for SA
 */

void CPedSA::SetModelIndex( unsigned short id )
{
    DEBUG_TRACE("void CPedSA::SetModelIndex( unsigned short id )");

    if ( id > MAX_MODELS-1 )
        return;

    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( !info || info->GetModelType() != MODEL_PED )
        return;

    CPedModelInfoSAInterface *pedInfo = (CPedModelInfoSAInterface*)info;
    
    m_pInterface->SetModelIndex( id );

    // Also set the voice gender
    switch( ((CPedModelInfoSAInterface*)info)->pedType )
    {
    case PEDTYPE_CIVFEMALE:
    case PEDTYPE_PROSTITUTE:
        GetInterface()->m_sound.m_bIsFemale = true;
        break;
    default:
        GetInterface()->m_sound.m_bIsFemale = false;
        break;
    }
}

CPadSAInterface& CPedSA::GetJoypad()
{
    return m_pad;
}

bool CPedSA::IsInWater() const
{
    DEBUG_TRACE("bool CPedSA::IsInWater() const");

    CTask *task = m_intelligence->GetTaskManager()->GetTask( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );

    return ( task && ( task->GetTaskType() == TASK_COMPLEX_IN_WATER ) );
}

void CPedSA::AttachPedToBike( CEntity *entity, const CVector& offset, unsigned short sUnk, float fUnk, float fUnk2, eWeaponType weaponType )
{
    DEBUG_TRACE("void CPedSA::AttachPedToBike( CEntity *entity, const CVector& offset, unsigned short sUnk, float fUnk, float fUnk2, eWeaponType weaponType )");

    CEntitySA* pEntitySA = dynamic_cast <CEntitySA*> ( entity );

    if ( !pEntitySA )
        return;

    DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();
    DWORD dwFunc = FUNC_AttachPedToBike;
    float fX = offset.fX;
    float fY = offset.fY;
    float fZ = offset.fZ;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    weaponType
        push    fUnk2
        push    fUnk
        push    sUnk
        push    fZ
        push    fY
        push    fX
        mov     ecx, dwThis
        push    dwEntityInterface
        call    dwFunc
    }   
}

void CPedSA::DetachPedFromEntity()
{
    DWORD dwFunc = FUNC_DetachPedFromEntity;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }
}

bool CPedSA::InternalAttachTo( CEntitySAInterface *entity, const CVector& pos, const CVector& rot )
{
    AttachPedToEntity( entity, pos, 0, 0.0f, WEAPONTYPE_UNARMED, FALSE );
    return true;
}

void CPedSA::AttachPedToEntity( CEntitySAInterface *entity, const CVector& offset, unsigned short sDirection, float rotLimit, eWeaponType weaponType, bool camChange )
{
    // sDirection and fRotationLimit only apply to first-person shooting (bChangeCamera)
    DEBUG_TRACE("void CPedSA::AttachPedToEntity( CEntitySAInterface *entity, const CVector& offset, unsigned short sDirection, float rotLimit, eWeaponType weaponType, bool camChange )");
    DWORD dwFunc = FUNC_AttachPedToEntity;
    DWORD dwThis = (DWORD)GetInterface();
    float fX = offset.fX;
    float fY = offset.fY;
    float fZ = offset.fZ;
    ePedType type = GetInterface()->bPedType;

    // Hack the CPed type to non-player so the camera doesn't get changed
    if ( !camChange )
        GetInterface()->bPedType = PEDTYPE_PLAYER_NETWORK;

    _asm
    {
        push    weaponType
        push    rotLimit
        movzx   ecx,sDirection
        push    ecx
        push    fZ
        push    fY
        push    fX
        push    entity
        mov     ecx,dwThis
        call    dwFunc
    }
    
    // Hack the CPed type(?) to whatever it was set to
    if ( !camChange )
        GetInterface()->bPedType = type;
}

bool CPedSA::CanSeeEntity( CEntity *entity, float distance ) const
{
    DEBUG_TRACE("bool CPedSA::CanSeeEntity( CEntity *entity, float distance ) const");

    DWORD dwFunc = FUNC_CanSeeEntity;
    bool bReturn = false;

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );

    DWORD dwEntityInterface = (DWORD)pEntitySA->GetInterface();
    _asm
    {
        push    distance
        push    dwEntityInterface
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

CVehicle* CPedSA::GetVehicle() const
{
    DEBUG_TRACE("CVehicle* CPedSA::GetVehicle() const");

    if ( !GetInterface()->pedFlags.bInVehicle )
        return NULL;

    CVehicleSAInterface *vehicle = (CVehicleSAInterface*)GetInterface()->m_objective;

    if ( !vehicle )
        return NULL;

    return pGame->GetPools()->GetVehicle( vehicle );
}

void CPedSA::Respawn( const CVector& pos, bool camCut )
{
    DEBUG_TRACE("void CPedSA::Respawn( const CVector& pos, bool camCut )");

    if ( !camCut )
    {
         // DISABLE call to CCamera__RestoreWithJumpCut when respawning
        MemSet( (void*)0x4422EA, 0x90, 20 );
    }

    float fX = pos.fX;
    float fY = pos.fY;
    float fZ = pos.fZ;
    float fUnk = 1.0f; 
    DWORD dwFunc = FUNC_RestorePlayerStuffDuringResurrection;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    fUnk
        push    fZ
        push    fY
        push    fX
        push    dwThis
        call    dwFunc
        add     esp, 20
    }
#if 0   // Removed to see if it reduces crashes
    dwFunc = 0x441440; // CGameLogic::SortOutStreamingAndMemory
    fUnk = 10.0f;
    _asm
    {
        push    fUnk
        push    position
        call    dwFunc
        add     esp, 8
    }
#endif
    dwFunc = FUNC_RemoveGogglesModel;
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    if ( !camCut )
    {
        // B9 28 F0 B6 00 E8 4C 9A 0C 00 B9 28 F0 B6 00 E8 B2 97 0C 00
        unsigned char szCode[] = {0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0x4C, 0x9A, 0x0C, 0x00, 0xB9, 0x28, 0xF0, 0xB6, 0x00, 0xE8, 0xB2, 0x97, 0x0C, 0x00} ;
        // RE-ENABLE call to CCamera__RestoreWithJumpCut when respawning
        MemCpy ( (void*)0x4422EA, szCode, 20 );
    }
}

bool CPedSA::AddProjectile( eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target )
{
    return pGame->GetProjectileInfo()->AddProjectile( this, eWeapon, vecOrigin, fForce, targetPos, target );
}

void CPedSA::SetStealthAiming( bool enable )
{
    if ( enable == m_stealthAiming )
        return;

    // Stop aiming?
    if ( !enable )
    {
        // Do we have the aiming animation?
        CAnimBlendAssociationSA *assoc = pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation( (RpClump*)GetInterface()->GetRwObject(), ANIM_ID_STEALTH_AIM );

        if ( assoc )
        {
            // Stop our animation
            assoc->SetBlendAmount( -2.0f );
        }
    }

    m_stealthAiming = enable;
}

void CPedSA::SetAnimationProgress( const char *name, float progress )
{
    CAnimBlendAssociationSA *assoc = (CAnimBlendAssociationSA*)pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation( (RpClump*)GetInterface()->GetRwObject(), name );
    
    if ( !assoc )
        return;

    assoc->SetCurrentProgress( progress );
}

void CPedSA::RemoveWeaponModel( unsigned short iModel )
{
    DWORD dwFunc = FUNC_RemoveWeaponModel;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        movzx   eax,iModel
        push    eax
        call    dwFunc
    }
}

void CPedSA::ClearWeapon( eWeaponType weaponType )
{
    DWORD dwFunc = FUNC_ClearWeapon;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    weaponType
        call    dwFunc
    }
}

CWeaponSA* CPedSA::GiveWeapon( eWeaponType weaponType, unsigned int uiAmmo )
{
    if ( weaponType != WEAPONTYPE_UNARMED )
    {
        CWeaponInfo* pInfo = pGame->GetWeaponInfo( weaponType );

        if ( pInfo )
        {
            int iModel = pInfo->GetModel();

            if ( iModel )
            {
                CModelInfo * pWeaponModel = pGame->GetModelInfo ( iModel );
                if ( pWeaponModel )
                {
                    pWeaponModel->Request ( true, true );
                }
            }
            // If the weapon is satchels, load the detonator too
            if ( weaponType == WEAPONTYPE_REMOTE_SATCHEL_CHARGE )
            {
                // Load the weapon and give it properly so getPedWeapon shows the weapon is there.
                GiveWeapon( WEAPONTYPE_DETONATOR, 1 );
            }
        }
    }

    DWORD dwReturn = 0;
    DWORD dwFunc = FUNC_GiveWeapon;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        push    1
        push    uiAmmo
        push    weaponType
        call    dwFunc
        mov     dwReturn, eax
    }

    // ryden: Hack to increase the sniper range
    CWeaponSA *pWeapon = GetWeapon( (eWeaponSlot)dwReturn );

    if ( weaponType == WEAPONTYPE_SNIPERRIFLE )
    {
        pWeapon->GetInfo()->SetWeaponRange ( 300.0f );
        pWeapon->GetInfo()->SetTargetRange ( 250.0f );
    }

    return pWeapon;
}

CWeaponSA* CPedSA::GetWeapon( eWeaponType weaponType )
{
    if ( weaponType > WEAPONTYPE_LAST_WEAPONTYPE-1 )
        return NULL;

    CWeaponSA *pWeapon = GetWeapon( pGame->GetWeaponInfo( weaponType )->GetSlot() );

    if ( pWeapon->GetType() != weaponType )
        return NULL;

    return pWeapon;
}

CWeaponSA* CPedSA::GetWeapon( eWeaponSlot weaponSlot )
{
    if ( weaponSlot > WEAPONSLOT_MAX-1 )
        return NULL;

    return m_weapons[weaponSlot];
}

void CPedSA::ClearWeapons()
{
    // Remove all the weapons
    for ( unsigned int i = 0; i < WEAPONSLOT_MAX; i++ )
    {
        if ( m_weapons[i] ) 
        {
            m_weapons[i]->SetAmmoInClip( 0 );
            m_weapons[i]->SetAmmoTotal( 0 );
            m_weapons[i]->Remove();            
        }
    }
}

void CPedSA::SetCurrentWeaponSlot( eWeaponSlot weaponSlot )
{
    if ( weaponSlot > WEAPONSLOT_MAX-1 )
        return;

    eWeaponSlot currentSlot = GetCurrentWeaponSlot ();

    if ( weaponSlot == GetCurrentWeaponSlot() )
        return;

    CWeapon * pWeapon = GetWeapon( currentSlot );

    if ( pWeapon )
        RemoveWeaponModel( pWeapon->GetInfo ()->GetModel() );

    CPedSAInterface *thisPed = (CPedSAInterface*)GetInterface();
 
    // set the new weapon slot
    thisPed->m_currentWeapon = weaponSlot;

    // is the player the local player?
    CPlayerPedSA *localPlayer = pGame->GetPlayerInfo()->GetPlayerPed();
    DWORD dwThis = (DWORD)GetInterface();

    if ( localPlayer == dynamic_cast <CPlayerPedSA*> ( this ) )
    {
        pGame->GetPlayerInfo()->GetInterface()->PlayerPedData.m_nChosenWeapon = weaponSlot;

        DWORD dwFunc = FUNC_MakeChangesForNewWeapon_Slot;
        _asm
        {
            mov     ecx, dwThis
            push    weaponSlot
            call    dwFunc
        }

        return;
    }

    DWORD dwFunc = FUNC_SetCurrentWeapon;
    _asm
    {
        mov     ecx, dwThis
        push    weaponSlot
        call    dwFunc
    }
}

void CPedSA::GetBonePosition( eBone bone, CVector& pos ) const
{
    DWORD dwFunc = FUNC_GetBonePosition;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    1
        push    bone
        push    pos
        mov     ecx, dwThis
        call    dwFunc
    }
}

void CPedSA::GetTransformedBonePosition( eBone bone, CVector& pos ) const
{
    DWORD dwFunc = FUNC_GetTransformedBonePosition;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    1
        push    bone
        push    pos
        mov     ecx, dwThis
        call    dwFunc
    }

    // Clamp to a sane range as this function can occasionally return massive values,
    // which causes ProcessLineOfSight to effectively freeze
    pos.fX = Clamp <float> ( -VALID_POSITION_LIMIT, pos.fX, VALID_POSITION_LIMIT );
    pos.fY = Clamp <float> ( -VALID_POSITION_LIMIT, pos.fY, VALID_POSITION_LIMIT );
    pos.fZ = Clamp <float> ( -VALID_POSITION_LIMIT, pos.fZ, VALID_POSITION_LIMIT );
}

CTaskSA* CPedSA::GetPrimaryTaskInternal() const
{
    return GetPedIntelligence()->GetTaskManager()->GetTask( TASK_PRIORITY_PRIMARY );
}

CTask* CPedSA::GetPrimaryTask() const
{
    return GetPrimaryTaskInternal();
}

bool CPedSA::IsEnteringVehicle() const
{
    CTaskSA *task = GetPrimaryTaskInternal();

    if ( !task )
        return false;

    switch( task->GetTaskType() )
    {
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return true;
    }

    return false;
}

bool CPedSA::IsGettingIntoVehicle() const
{
    if ( !IsEnteringVehicle() )
        return false;

    switch( GetPrimaryTaskInternal()->GetSubTask()->GetTaskType() )
    {
    case TASK_SIMPLE_CAR_GET_IN:
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE:
    case TASK_SIMPLE_CAR_SHUFFLE:
    case TASK_COMPLEX_ENTER_BOAT_AS_DRIVER:
        return true;
    }

    return false;
}

bool CPedSA::IsBeingJacked() const
{
    CTaskSA *task = GetPrimaryTaskInternal();

    if ( !task )
        return false;

    switch( task->GetTaskType() )
    {
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP:
    case TASK_SIMPLE_BIKE_JACKED:
        return true;
    }

    return false;
}

bool CPedSA::IsLeavingVehicle() const
{
    CTaskSA *task = GetPrimaryTaskInternal();

    if ( !task )
        return false;

    switch( task->GetTaskType() )
    {
    case TASK_COMPLEX_LEAVE_CAR: // We only use this task
    case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
    case TASK_COMPLEX_LEAVE_CAR_AND_FLEE:
    case TASK_COMPLEX_LEAVE_CAR_AND_WANDER:
    case TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE:
        return true;
    }

    return false;
}

bool CPedSA::IsGettingOutOfVehicle() const
{
    // That is technically getting out of vehicle
    if ( IsBeingJacked() )
        return true;

    if ( !IsLeavingVehicle() )
        return false;

    switch( GetPrimaryTaskInternal()->GetSubTask()->GetTaskType() )
    {
    case TASK_SIMPLE_CAR_GET_OUT:
    case TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE:
        return true;
    }

    return false;
}

bool CPedSA::IsPlayingAnimation( const char *name ) const
{
    DWORD dwReturn = 0;
    DWORD dwFunc = FUNC_RpAnimBlendClumpGetAssociation;
    DWORD dwThis = (DWORD)m_pInterface->GetRwObject();

    _asm
    {
        push    name
        push    dwThis
        call    dwFunc
        add     esp, 8
        mov     dwReturn, eax
    }
    return dwReturn != 0;
}

bool CPedSA::IsDying() const
{
    CTaskSA *task = GetPrimaryTaskInternal();

    if ( !task )
        return false;

    switch( task->GetTaskType() )
    {
    case TASK_SIMPLE_DIE:
    case TASK_COMPLEX_DIE:
    case TASK_SIMPLE_DROWN:
    case TASK_SIMPLE_DIE_IN_CAR:
    case TASK_COMPLEX_DIE_IN_CAR:
    case TASK_SIMPLE_DROWN_IN_CAR:
        return true;
    }

    return false;
}

bool CPedSA::IsDead() const
{
    CTaskSA *task = GetPrimaryTaskInternal();

    return task && task->GetTaskType() == TASK_SIMPLE_DEAD;
}

void CPedSA::QuitEnteringCar( CVehicle *vehicle, int iSeat, bool bUnknown )
{
    CVehicleSA* pVehicleSA = dynamic_cast <CVehicleSA*> ( vehicle );

    if ( !pVehicleSA )
        return;

    DWORD dwFunc = FUNC_QuitEnteringCar;
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwVehicle = (DWORD)pVehicleSA->GetInterface();
    _asm
    {
        push    bUnknown
        push    iSeat
        push    dwVehicle
        push    dwThis
        call    dwFunc
        add     esp, 16
    }
}

void CPedSA::SetGogglesState( bool wear )
{
    DWORD dwFunc = wear ? FUNC_PutOnGoggles : FUNC_TakeOffGoggles;

    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
    }

    if ( !wear )
        return;

    // Are our goggle anims loaded?
    CAnimBlock *block = pGame->GetAnimManager()->GetAnimationBlock( "GOGGLES" );

    if ( block->IsLoaded() )
        pGame->GetAnimManager()->BlendAnimation( (RpClump*)GetInterface()->GetRwObject(), ANIM_GROUP_GOGGLES, ANIM_ID_GOGGLES_ON, 4.0f );
}

void CPedSA::SetClothesTextureAndModel( const char *tex, const char *model, short txd )
{
    // Seems to be a very easy function which hashes names and inserts them into the clothes class
    // We should reven this TODO
    DWORD dwFunc = FUNC_CPedClothesDesc__SetTextureAndModel;
    DWORD dwThis = (DWORD)GetInterface()->m_playerData->m_pClothes;
    DWORD texPtr = (DWORD)tex;
    DWORD modelPtr = (DWORD)model;
    DWORD txdStack = txd;
    _asm
    {
        mov     ecx, dwThis
        push    txdStack
        push    modelPtr
        push    texPtr
        call    dwFunc
    }
}

void CPedSA::RebuildPlayer()
{
    DWORD dwFunc = FUNC_CClothes__RebuildPlayer;
    DWORD dwThis = (DWORD)GetInterface();
    _asm
    {
        push    0
        push    dwThis
        call    dwFunc
        add     esp, 8
    }
}

void CPedSA::SetFightingStyle( eFightingStyle style, unsigned char extra )
{
    if ( style == GetInterface()->m_fightStyle )
        return;

    GetInterface()->m_fightStyle = style;

    if ( extra > 6 )
        return;

    switch( extra )
    {
    case 2:
        GetInterface()->m_fightStyleExtra |= 0x01;
        break;
    case 3:
        GetInterface()->m_fightStyleExtra |= 0x02;
        break;
    case 4:
        GetInterface()->m_fightStyleExtra |= 0x04;
        break;
    case 5:
        GetInterface()->m_fightStyleExtra |= 0x08;
        break;
    case 6:
        GetInterface()->m_fightStyleExtra |= 0x10;
        break;
    }
}

CEntity* CPedSA::GetContactEntity() const
{
    CEntitySAInterface* pInterface = GetInterface()->m_contactEntity;

    if ( !pInterface )
        return NULL;

    switch( pInterface->nType )
    {
    case ENTITY_TYPE_VEHICLE:
        return pGame->GetPools()->GetVehicle( pInterface );
    case ENTITY_TYPE_OBJECT:
        return pGame->GetPools()->GetObject( pInterface );
    }

    return NULL;
}

CEntity* CPedSA::GetTargetedEntity() const
{
    CEntitySAInterface *pInterface = GetInterface()->m_target;

    switch( pInterface->nType )
    {
    case ENTITY_TYPE_PED:
        return pGame->GetPools()->GetPed( pInterface );
    case ENTITY_TYPE_VEHICLE:
        return pGame->GetPools()->GetVehicle( pInterface );
    case ENTITY_TYPE_OBJECT:
        return pGame->GetPools()->GetObject( pInterface );
    }

    return NULL;
}

void CPedSA::SetTargetedEntity( CEntity *entity )
{
    CEntitySAInterface* pInterface = NULL;
    if ( entity )
    {
        CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( entity );

        if ( pEntitySA )
            pInterface = pEntitySA->GetInterface();
    }

    GetInterface()->m_target = pInterface;
}

void CPedSA::RemoveBodyPart( int i, char c )
{
    DWORD dwThis = (DWORD)GetInterface();
    DWORD dwFunc = FUNC_CPed_RemoveBodyPart;
    _asm
    {
        mov     ecx, dwThis
        push    c
        push    i
        call    dwFunc
    }
}

void CPedSA::SetFootBlood( unsigned int density )
{
    // Adjust the footBlood flag
    GetInterface()->pedFlags.bDoBloodyFootprints = density != 0;

    // Set the amount of foot blood
    GetInterface()->m_footBloodDensity = density;
}

unsigned int CPedSA::GetFootBlood() const
{
    if ( !GetInterface()->pedFlags.bDoBloodyFootprints )
        return 0;

    return GetInterface()->m_footBloodDensity;
}

void CPedSA::SetOnFire( bool bOnFire )
{
    CPedSAInterface* pInterface = GetInterface();
    CFireSA *pFire;

    if ( !bOnFire )
    {
        // Make sure that we have some attached fire
        if ( pInterface->m_fire != NULL )
        {
            CFire *pFire = pGame->GetFireManager()->GetFire( pInterface->m_fire );

            if ( pFire )
                pFire->Extinguish();
        }
        return;
    }

    // If we are already on fire, don't apply a new fire
    if ( pInterface->m_fire )
        return;

    pFire = pGame->GetFireManager()->StartFire( this, NULL, (float)DEFAULT_FIRE_PARTICLE_SIZE );

    if ( !pFire )
        return;

    // Start the fire
    pFire->SetTarget( this );
    pFire->Ignite();
    pFire->SetStrength( 1.0f );

    // Attach the fire only to the player, do not let it
    // create child fires when moving.
    pFire->SetNumGenerationsAllowed( 0 );
    pInterface->m_fire = pFire->GetInterface();
}

void CPedSA::GetVoice( short *psVoiceType, short *psVoiceID ) const
{
    if ( psVoiceType )
        *psVoiceType = m_sound->GetVoiceTypeID ();

    if ( psVoiceID )
        *psVoiceID = m_sound->GetVoiceID ();
}

void CPedSA::GetVoice( const char **pszVoiceType, const char **pszVoice ) const
{
    short sVoiceType, sVoiceID;
    GetVoice( &sVoiceType, &sVoiceID );

    if ( pszVoiceType )
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID ( sVoiceType );

    if ( pszVoice )
        *pszVoice = CPedSoundSA::GetVoiceNameFromID ( sVoiceType, sVoiceID );
}

void CPedSA::SetVoice( short sVoiceType, short sVoiceID )
{
    m_sound->SetVoiceTypeID ( sVoiceType );
    m_sound->SetVoiceID ( sVoiceID );
}

void CPedSA::SetVoice( const char *szVoiceType, const char *szVoice )
{
    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName ( szVoiceType );
    short sVoiceID = CPedSoundSA::GetVoiceIDFromName ( sVoiceType, szVoice );

    if ( sVoiceType == -1 || sVoiceID == -1 )
        return;

    SetVoice ( sVoiceType, sVoiceID );
}

void* CPedSAInterface::operator new( size_t )
{
    return (*ppPedPool)->Allocate();
}

void CPedSAInterface::operator delete( void *ptr )
{
    (*ppPedPool)->Free( (CPedSAInterface*)ptr );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPlayerInfoSA.cpp
*  PURPOSE:     Player ped type information
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CBaseModelInfoSAInterface **ppModelInfo;

CPlayerPedDataSAInterface::CPlayerPedDataSAInterface( void )
{
    Serialize();
}

void CPlayerPedDataSAInterface::Serialize( void )
{
    // Remote players have their data serialized, because they do not own references or allocations
    m_meleeAnimRef = 0;
    m_meleeAnimExtraRef = 0;
}

/*=========================================================
    GetPlayerPed

    Arguments:
        id - index of the player to get the ped of
    Purpose:
        Returns the ped associated with the player id.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E210
=========================================================*/
CPlayerPedSAInterface* __cdecl GetPlayerPed( int id )
{
    return PlayerInfo::GetInfo( id ).pPed;
}

/*=========================================================
    GetPlayerVehicle

    Arguments:
        id - index of the player
        excludeRemote - return NULL if controlling RC vehicle
    Purpose:
        Returns the player vehicle.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E0D0
=========================================================*/
CVehicleSAInterface* __cdecl GetPlayerVehicle( int id, bool excludeRemote )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    CPlayerPedSAInterface *ped = info.pPed;

    if ( !ped || !ped->pedFlags.bInVehicle )
        return NULL;

    if ( excludeRemote && info.pRemoteVehicle )
        return NULL;

    return (CVehicleSAInterface*)ped->m_objective;
}

/*=========================================================
    FindPlayerCoords

    Arguments:
        pos - vector pointer to write position into
        id - player index
    Purpose:
        Writes the player given by id into pos. If we are inside
        of the streaming update routine, then we write the center
        of world instead. Returns the output vector.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E010
=========================================================*/
const CVector& __cdecl FindPlayerCoords( CVector& pos, int id )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    CPlayerPedSAInterface *ped = info.pPed;

    // If there is no player ped loaded yet, we
    // default to 0,0,0
    if ( !ped )
    {
        pos = CVector( 0, 0, 0 );
        return pos;
    }

    const CVector *entityPos = NULL;

    // If the player is inside of a vehicle,
    // use its position instead.
    if ( ped->pedFlags.bInVehicle )
    {
        CEntitySAInterface *veh = ped->m_objective;

        if ( veh )
            entityPos = &veh->Placeable.GetPosition();
    }

    // If there was no valid position set (i.e. vehicle),
    // we default to ped position.
    if ( !entityPos )
        entityPos = &ped->Placeable.GetPosition();

    pos = *entityPos;
    return pos;
}

/*=========================================================
    FindPlayerCenterOfWorld

    Arguments:
        id - player index to get the center of
    Purpose:
        Returns the vector which describes the exact center
        of world. If we have set a static center of world,
        use it instead.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E250
=========================================================*/
inline const CEntitySAInterface* GetPlayerEntityContext( int id )
{
    CPlayerInfoSAInterface& info = PlayerInfo::GetInfo( id );

    // We check the remote vehicle first.
    if ( CVehicleSAInterface *remoteVehicle = info.pRemoteVehicle )
        return remoteVehicle;

    // Let us try the player's vehicle next.
    if ( CVehicleSAInterface *veh = GetPlayerVehicle( id, false ) )
        return veh;

    // Default to the player.
    return info.pPed;
}

const CVector& __cdecl FindPlayerCenterOfWorld( int id )
{
    // MTA fix: check for static center of world and return
    // it if set.
    if ( World::IsCenterOfWorldSet() )
        return World::GetCenterOfWorld();

    // If the GTA:SA static camera is active, we report back
    // the camera position.
    if ( *(bool*)0x009690C1 )
        return Camera::GetInterface().Placeable.GetPosition();

    // Get the position of the entity context.
    return GetPlayerEntityContext( id )->Placeable.GetPosition();
}

/*=========================================================
    FindPlayerHeading

    Arguments:
        id - player index to get the heading of
    Purpose:
        Returns the floating point heading value active for
        the player. If the center of world is set, return
        a false heading instead.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E450
=========================================================*/
float __cdecl FindPlayerHeading( int id )
{
    // MTA fix: return a false heading if center of world
    // is set.
    if ( World::IsCenterOfWorldSet() )
        return World::GetFalseHeading();

    return GetPlayerEntityContext( id )->Placeable.GetHeading();
}

/*=========================================================
    SetupPlayerPed

    Arguments:
        playerIndex - slot to save the player ped into
    Purpose:
        Creates a new player ped and sets it at the given slot.
        The slot is an entry in the global playerInfo structure.
        Natively, GTA:SA supports only slot 0.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0060D790
=========================================================*/
void __cdecl SetupPlayerPed( unsigned int playerIndex )
{
    CPlayerPedSAInterface *player = (CPlayerPedSAInterface*)( (*ppPedPool)->Allocate() );

    // We should be able to create players natively someday.
    // Until then, here is an ugly ASM chunk.
    // It calls the player constructor.
    __asm
    {
        mov ecx,player
        push 0
        push playerIndex
        mov eax,0x0060D5B0
        call eax
    }

    CPlayerInfoSAInterface& playerInfo = PlayerInfo::GetInfo( playerIndex );
    playerInfo.pPed = player;

    if ( playerIndex == 1 )
        player->bPedType = PEDTYPE_PLAYER2;

    player->SetOrientation( 0, 0, 0 );

    World::AddEntity( player );

    player->m_unkPlayerVal = 100;

    playerInfo.PlayerState = PS_PLAYING;
}

/*=========================================================
    HOOK_CRunningScript_Process

    Purpose:
        Used by the game to execute .SCM scripts. We use it
        to manage the player entity.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0056E450
=========================================================*/
static bool bHasProcessedScript = false;

void __cdecl HOOK_CRunningScript_Process( void )
{
    if ( !bHasProcessedScript )
    {
        CCameraSAInterface& camera = Camera::GetInterface();
        camera.SetFadeColor( 0, 0, 0 );
        camera.Fade( 0.0f, FADE_OUT );

        Streaming::RequestSpecialModel( 0, "player", 26 );
        Streaming::LoadAllRequestedModels( true );

        SetupPlayerPed( 0 );

        PlayerInfo::GetInfo( 0 ).pPed->Placeable.SetPosition( 0, 0, 0 );

        // We have set up our player instance.
        bHasProcessedScript = true;
    }
}

void PlayerInfo_Init( void )
{
    // Hook fixes.
    HookInstall( 0x0056E010, (DWORD)FindPlayerCoords, 5 );
    HookInstall( 0x0056E250, (DWORD)FindPlayerCenterOfWorld, 5 );
    HookInstall( 0x0056E450, (DWORD)FindPlayerHeading, 5 );
    HookInstall( 0x00469F00, (DWORD)HOOK_CRunningScript_Process, 5 );
}

void PlayerInfo_Shutdown( void )
{

}

CPlayerPedSA* CPlayerInfoSA::GetPlayerPed( void )
{
    DEBUG_TRACE("CPlayerPedSA* CPlayerInfoSA::GetPlayerPed( void )");

    return (CPlayerPedSA*)mtaPeds[0];
}

CWantedSA* CPlayerInfoSA::GetWanted( void )
{
    DEBUG_TRACE("CWantedSA* CPlayerInfoSA::GetWanted( void )");

    if ( !m_wanted )
        m_wanted = new CWantedSA( m_interface->PlayerPedData.m_Wanted ); 

    return m_wanted;
}

bool CPlayerInfoSA::GetCrossHair( float& tarX, float& tarY )
{
    DEBUG_TRACE("bool CPlayerInfoSA::GetCrossHair( float &tarX, float &tarY )");

    tarX = m_interface->CrossHair.m_tarX;
    tarY = m_interface->CrossHair.m_tarY;
    return m_interface->CrossHair.m_active;
}

long CPlayerInfoSA::GetPlayerMoney( void )
{
    DEBUG_TRACE("long CPlayerInfoSA::GetPlayerMoney( void )");
    return *(long*)0xB7CE50;
}

void CPlayerInfoSA::SetPlayerMoney( long value )
{
    DEBUG_TRACE("void CPlayerInfoSA::SetPlayerMoney( long value )");

    *(long*)0xB7CE50 = value;
}

void CPlayerInfoSA::GivePlayerParachute( void )
{
    DEBUG_TRACE("VOID CPlayerInfoSA::GivePlayerParachute( void )");

    DWORD dwFunction = FUNC_GivePlayerParachute;
    _asm
    {
        call dwFunction
    }
}

void CPlayerInfoSA::StreamParachuteWeapon( bool allow )
{
    DEBUG_TRACE("void CPlayerInfoSA::StreamParachuteWeapon( bool allow )");

    DWORD dwFunction = FUNC_StreamParachuteWeapon;
    _asm {
        push allow
        call dwFunction
    }
}

void CPlayerInfoSA::MakePlayerSafe( bool safe )
{
    DEBUG_TRACE("void CPlayerInfoSA::MakePlayerSafe( bool safe )");

    DWORD dwFunction = FUNC_MakePlayerSafe;
    _asm
    {
        push    safe
        call    dwFunction
    }
}

void CPlayerInfoSA::CancelPlayerEnteringCars( CVehicleSA* veh )
{
    DEBUG_TRACE("void CPlayerInfoSA::CancelPlayerEnteringCars( CVehicle *veh )");

    DWORD dwFunction = FUNC_CancelPlayerEnteringCars;
    _asm
    {
        push    veh
        call    dwFunction
    }
}

/**
 * Make the player arrested - displays "Busted" text are they are respawned at the nearest police station
 * @see CRestartSA::OverrideNextRestart
 * @see CRestartSA::AddPoliceRestartPoint
 */
void CPlayerInfoSA::ArrestPlayer( void )
{
    DEBUG_TRACE("void CPlayerInfoSA::ArrestPlayer( void )");

    DWORD dwFunction = FUNC_ArrestPlayer;
    _asm
    {
        call    dwFunction
    }
}

/**
 * Kill the player - displays "Wasted" text are they are respawned at the nearest hospital station
 * @see CRestartSA::OverrideNextRestart
 * @see CRestartSA::AddHospitalRestartPoint
 */
void CPlayerInfoSA::KillPlayer( void )
{
    DEBUG_TRACE("void CPlayerInfoSA::KillPlayer( void )");

    DWORD dwFunction = FUNC_KillPlayer;
    _asm
    {
        call    dwFunction
    }
}

/**
 * Creates a remote controlled vehicle of a specific type at a specified point
 * @param vehicletype This is the type of vehicle to create
 * @return CVehicle * for the created remote controlled vehicle
 */
CVehicle* CPlayerInfoSA::GiveRemoteVehicle( unsigned short model, float x, float y, float z )
{
    DEBUG_TRACE("CVehicleSA* CPlayerInfoSA::GivePlayerRemoteVehicle( unsigned short mode, float x, float y, float z )");

    CModelInfoSA *info = pGame->GetModelInfo( model );

    if ( !info || !info->IsVehicle() )
        return NULL;

    DWORD dwFunction = FUNC_GivePlayerRemoteControlledCar;
    _asm
    {
        push    model
        push    0   // rotation
        push    x
        push    y
        push    z
        call    dwFunction
        add     esp, 0x14
    }

    return GetRemoteVehicle();
}

void CPlayerInfoSA::StopRemoteControl( void )
{
    DEBUG_TRACE("void CPlayerInfoSA::StopRemoteControl( void )");

    DWORD dwFunction = FUNC_TakeRemoteControlledCarFromPlayer;
    _asm
    {
        push    ecx
        push    1
        call    dwFunction
        pop     ecx
    }
}

CVehicle* CPlayerInfoSA::GetRemoteVehicle( void )
{
    DEBUG_TRACE("CVehicleSA* CPlayerInfoSA::GetRemoteVehicle( void )");

    return pGame->GetPools()->GetVehicle( (CVehicleSAInterface*)VAR_PlayerRCCar );
}

float CPlayerInfoSA::GetFPSMoveHeading( void )
{
    return 0;//m_interface->m_pedData.m_fFPSMoveHeading;
}

bool CPlayerInfoSA::GetDoesNotGetTired()
{
    return m_interface->DoesNotGetTired;
}

void CPlayerInfoSA::SetDoesNotGetTired( bool award )
{
    m_interface->DoesNotGetTired = award;
}

unsigned short CPlayerInfoSA::GetLastTimeEaten( void )
{
    return m_interface->TimeLastEaten;
}

void CPlayerInfoSA::SetLastTimeEaten( unsigned short timer )
{
    m_interface->TimeLastEaten = timer;
}

unsigned int CPlayerInfoSA::GetLastTimeBigGunFired( void )
{
    return m_interface->LastTimeBigGunFired;
}

void CPlayerInfoSA::SetLastTimeBigGunFired( unsigned int time )
{
    m_interface->LastTimeBigGunFired = time;
}

void* CPlayerPedDataSAInterface::operator new( size_t )
{
    return mtaPlayerDataPool->Allocate();
}

void CPlayerPedDataSAInterface::operator delete( void *ptr )
{
    mtaPlayerDataPool->Free( (CPlayerPedDataSAInterface*)ptr );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientVehicleManager.cpp
*  PURPOSE:     Vehicle entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

// List over all vehicles with their passenger max counts
const unsigned char g_ucMaxPassengers [] = 
                                     { 3, 1, 1, 1, 3, 3, 0, 1, 1, 3, 1, 1, 1, 3, 1, 1,              // 400->415
                                       3, 1, 3, 1, 3, 3, 1, 1, 1, 0, 3, 3, 3, 1, 0, 8,              // 416->431
                                       0, 1, 1, 255, 1, 8, 3, 1, 3, 0, 1, 1, 1, 3, 0, 1,            // 432->447
                                       0, 1, 255, 1, 0, 0, 0, 1, 1, 1, 3, 3, 1, 1, 1,               // 448->462
                                       1, 1, 1, 3, 3, 1, 1, 3, 1, 0, 0, 1, 1, 0, 1, 1,              // 463->478
                                       3, 1, 0, 3, 1, 0, 0, 0, 3, 1, 1, 3, 1, 3, 0, 1,              // 479->494
                                       1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 0, 0,              // 495->510
                                       1, 0, 0, 1, 1, 3, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,              // 511->526
                                       1, 1, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 3, 1,                 // 527->541
                                       1, 1, 1, 1, 3, 3, 1, 1, 3, 3, 1, 0, 1, 1, 1, 1,              // 542->557
                                       1, 1, 3, 3, 1, 1, 0, 1, 3, 3, 0, 255, 1, 0, 0,               // 558->572
                                       1, 0, 1, 1, 1, 1, 3, 3, 1, 3, 0, 255, 3, 1, 1, 1,            // 573->588
                                       1, 255, 255, 1, 1, 1, 0, 3, 3, 3, 1, 1, 1, 1, 1,             // 589->604
                                       3, 1, 255, 255, 255, 3, 255, 255 };                          // 605->611

// List over all vehicles with their special attributes
#define VEHICLE_HAS_TURRENT             0x001UL //1
#define VEHICLE_HAS_SIRENS              0x002UL //2
#define VEHICLE_HAS_LANDING_GEARS       0x004UL //4
#define VEHICLE_HAS_ADJUSTABLE_PROPERTY 0x008UL //8
#define VEHICLE_HAS_SMOKE_TRAIL         0x010UL //16
#define VEHICLE_HAS_TAXI_LIGHTS         0x020UL //32
#define VEHICLE_HAS_SEARCH_LIGHT        0x040UL //64

const unsigned long g_ulVehicleAttributes [] = {
  0, 0, 0, 0, 0, 0, 8, 3, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 32, 0, 0, 2, 0,    // 400-424
  0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,    // 425-449
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 450-474
  0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 64, 0, 0,    // 475-499
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 4, 12, 0, 0, 2, 8,  // 500-524
  8, 0, 0, 2, 0, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0,     // 525-549
  0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 550-574
  0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 2, 2, 2, 2,    // 575-599
  0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

CClientVehicleManager::CClientVehicleManager( CClientManager* pManager )
{
    assert( NUMELMS( g_ucMaxPassengers ) == 212 );
    assert( NUMELMS( g_ulVehicleAttributes ) == 212 );

    // Initialize members
    m_pManager = pManager;
}

CClientVehicleManager::~CClientVehicleManager()
{
}

void CClientVehicleManager::DeleteAll()
{
    luaRefs refs;

    // Delete all the vehicles
    for ( vehicles_t::const_iterator iter = m_List.begin(); iter != m_List.end(); iter++ )
    {
        CClientVehicle *veh = *iter;
        veh->Reference( refs );
        veh->Delete();
    }

    // Clear the list
    m_List.clear();
}

void CClientVehicleManager::DoPulse()
{
    // Loop through our streamed-in vehicles
    vehicles_t cloneList = m_StreamedIn;

    for ( vehicles_t::iterator iter = cloneList.begin(); iter != cloneList.end(); ++iter )
        (*iter)->IncrementMethodStack();

    for ( vehicles_t::iterator iter = cloneList.begin(); iter != cloneList.end(); ++iter )
    {
        CClientVehicle *veh = *iter;

        // We should have a game vehicle here
        assert( veh->GetGameVehicle() );

        veh->StreamedInPulse();
    }

    for ( vehicles_t::iterator iter = cloneList.begin(); iter != cloneList.end(); ++iter )
        (*iter)->DecrementMethodStack();
}

CClientVehicle* CClientVehicleManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTVEHICLE )
    {
        return static_cast < CClientVehicle* > ( pEntity );
    }

    return NULL;
}

CClientVehicle* CClientVehicleManager::Get ( CVehicle* pVehicle, bool bValidatePointer )
{
    if ( !pVehicle )
        return NULL;

    if ( bValidatePointer )
    {
        vehicles_t::const_iterator iter = m_StreamedIn.begin();

        for ( ; iter != m_StreamedIn.end (); iter++ )
        {
            if ( (*iter)->GetGameVehicle() == pVehicle )
                return *iter;
        }
    }
    else
    {
        return reinterpret_cast < CClientVehicle* > ( pVehicle->GetStoredPointer () );
    }
    return NULL;
}

CClientVehicle* CClientVehicleManager::GetSafe ( CEntity * pEntity )
{
    if ( !pEntity )
        return NULL;

    vehicles_t::const_iterator iter = m_StreamedIn.begin();

    for ( ; iter != m_StreamedIn.end(); iter++ )
    {
        if ( dynamic_cast <CEntity*> ( (*iter)->GetGameVehicle () ) == pEntity )
            return *iter;
    }

    return NULL;
}

CClientVehicle* CClientVehicleManager::GetClosest( CVector& vecPosition, float fRadius )
{
    float fClosestDistance = 0.0f;
    CVector vecVehiclePosition;
    CClientVehicle* pClosest = NULL;
    vehicles_t::const_iterator iter = m_List.begin();

    for ( ; iter != m_List.end(); iter++ )
    {
        (*iter)->GetPosition( vecVehiclePosition );
        float fDistance = DistanceBetweenPoints3D( vecPosition, vecVehiclePosition );

        if ( fDistance <= fRadius )
        {
            if ( pClosest == NULL || fDistance < fClosestDistance )
            {
                pClosest = *iter;
                fClosestDistance = fDistance;
            }
        }
    }
    return pClosest;
}

bool CClientVehicleManager::IsValidModel( unsigned short model )
{
    CModelInfo *info = g_pGame->GetModelInfo( model );

    if ( !info )
        return false;

    return info->IsVehicle();
}

eClientVehicleType CClientVehicleManager::GetVehicleType ( unsigned short ulModel )
{
    // Grab the model info for the current vehicle
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( ulModel );
    if ( pModelInfo )
    {
        // Return the appropriate type
        switch( pModelInfo->GetVehicleType() )
        {
        case VEHICLE_CAR:                   return CLIENTVEHICLE_CAR;
        case VEHICLE_PLANE:                 return CLIENTVEHICLE_PLANE;
        case VEHICLE_HELI:                  return CLIENTVEHICLE_HELI;
        case VEHICLE_AUTOMOBILETRAILER:     return CLIENTVEHICLE_TRAILER;
        case VEHICLE_QUADBIKE:              return CLIENTVEHICLE_QUADBIKE;
        case VEHICLE_MONSTERTRUCK:          return CLIENTVEHICLE_MONSTERTRUCK;
        case VEHICLE_BOAT:                  return CLIENTVEHICLE_BOAT;
        case VEHICLE_BICYCLE:               return CLIENTVEHICLE_BMX;
        case VEHICLE_BIKE:                  return CLIENTVEHICLE_BIKE;
        case VEHICLE_TRAIN:                 return CLIENTVEHICLE_TRAIN;
        }
    }

    // Invalid vehicle id or some other error
    return CLIENTVEHICLE_NONE;
}

unsigned char CClientVehicleManager::GetMaxPassengerCount ( unsigned short ulModel )
{
    // Valid model?
    if ( IsValidModel( ulModel ) )
    {
        return g_ucMaxPassengers [ulModel - 400];
    }

    // Invalid index
    return 0xFF;
}

unsigned char CClientVehicleManager::ConvertIndexToGameSeat ( unsigned short ulModel, unsigned char ucIndex )
{
    eClientVehicleType vehicleType = GetVehicleType ( ulModel );
                
    // Grab the max passenger count for the given ID
    unsigned char ucMaxPassengerCount = GetMaxPassengerCount ( ulModel );
    switch( ucMaxPassengerCount )
    {
    // Not passenger seats in this vehicle?
    case 0:
    case 255:
    {
        if ( ucIndex == 0 )
        {
            return DOOR_FRONT_LEFT;
        }
        
        return 0xFF;
    }

    // Only one seat?
    case 1:
    {
        bool bIsBike = ( vehicleType == CLIENTVEHICLE_BIKE ||
                         vehicleType == CLIENTVEHICLE_QUADBIKE );
        if ( ucIndex == 0 )
        {
            return DOOR_FRONT_LEFT;
        }
        else if ( ucIndex == 1 )
        {
            // We use one of the rear seats for bike passengers
            if ( bIsBike )
            {
                return DOOR_REAR_RIGHT;
            }
            
            return DOOR_FRONT_RIGHT;
        }
        else if ( bIsBike )
        {
            switch ( ucIndex )
            {                    
                case 2: return DOOR_REAR_LEFT;
                case 3: return DOOR_REAR_RIGHT;
            }
        }
    
        return 0xFF;
    }

    // Three seats?
    case 3:
    {
        switch ( ucIndex )
        {
            case 0: return DOOR_FRONT_LEFT;                
            case 1: return DOOR_FRONT_RIGHT;
            case 2: return DOOR_REAR_LEFT;
            case 3: return DOOR_REAR_RIGHT;
        }

        return 0xFF;
    }

    // Bus, train (570)?
    case 8:
    {
        if ( ucIndex == 0 )
        {
            return DOOR_FRONT_LEFT;
        }
        
        if ( ucIndex <= 8 )
        {
            return DOOR_FRONT_RIGHT;
        }

        return 0xFF;
    }
    }

    return 0xFF;
}

bool CClientVehicleManager::HasTurret ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_TURRENT ) );
}

bool CClientVehicleManager::HasSirens ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_SIRENS ) );
}

bool CClientVehicleManager::HasTaxiLight ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_TAXI_LIGHTS ) );
}

bool CClientVehicleManager::HasSearchLight ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_SEARCH_LIGHT ) );
}

bool CClientVehicleManager::HasLandingGears ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_LANDING_GEARS ) );
}

bool CClientVehicleManager::HasAdjustableProperty ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_ADJUSTABLE_PROPERTY ) );
}

bool CClientVehicleManager::HasSmokeTrail ( unsigned short ulModel )
{
    return ( IsValidModel ( ulModel ) &&
             ( g_ulVehicleAttributes[ ulModel - 400 ] & VEHICLE_HAS_SMOKE_TRAIL ) );
}

bool CClientVehicleManager::HasDamageModel ( unsigned short ulModel )
{
    return HasDamageModel ( GetVehicleType ( ulModel ) );
}

bool CClientVehicleManager::HasDamageModel( eClientVehicleType Type )
{
    switch( Type )
    {
    case CLIENTVEHICLE_TRAILER:
    case CLIENTVEHICLE_MONSTERTRUCK:
    case CLIENTVEHICLE_QUADBIKE:
    case CLIENTVEHICLE_HELI:
    case CLIENTVEHICLE_PLANE:
    case CLIENTVEHICLE_CAR:
        return true;
    default:
        return false;
    }
}

bool CClientVehicleManager::HasDoors( unsigned short ulModel )
{
    bool bHasDoors = false;

    if ( HasDamageModel ( ulModel ) )
    {
        switch ( ulModel )
        {
        case VT_BFINJECT:
        case VT_RCBANDIT:
        case VT_CADDY:
        case VT_RCRAIDER:
        case VT_BAGGAGE:
        case VT_DOZER:
        case VT_FORKLIFT:
        case VT_TRACTOR:
        case VT_RCTIGER:
        case VT_BANDITO:
        case VT_KART:
        case VT_MOWER:
        case VT_RCCAM:
        case VT_RCGOBLIN:
            break;
        default:
            bHasDoors = true;
        }
    }

    return bHasDoors;
}

void CClientVehicleManager::RemoveFromList( CClientVehicle *pVehicle )
{
    ListRemove( m_List, pVehicle );
}

bool CClientVehicleManager::IsVehicleLimitReached()
{
    // GTA allows max 110 vehicles. We restrict ourselves to 64 for now
    // due to FPS issues and crashes around 100 vehicles.
    // The_GTA: There should be no crash issues with more than 110 vehicles anymore.
    return g_pGame->GetPools()->GetNumberOfUsedSpaces( VEHICLE_POOL ) >= g_pGame->GetPools()->GetPoolCapacity( VEHICLE_POOL );
}

void CClientVehicleManager::OnCreation( CClientVehicle * pVehicle )
{
    m_StreamedIn.push_back( pVehicle );
}

void CClientVehicleManager::OnDestruction( CClientVehicle * pVehicle )
{
    ListRemove( m_StreamedIn, pVehicle );
}

void CClientVehicleManager::RestreamVehicles( unsigned short usModel )
{
    luaRefs refs;

    // Store the affected vehicles
    vehicles_t::const_iterator iter = IterBegin();

    for ( ; iter != IterEnd(); iter++ )
    {
        CClientVehicle *pVehicle = *iter;

        pVehicle->Reference( refs );

        // Streamed in and same vehicle ID?
        if ( pVehicle->IsStreamedIn() && pVehicle->GetModel() == usModel )
        {
            // Stream it out for a while until streamer decides to stream it
            // back in eventually
            pVehicle->StreamOutForABit();
        }
    }
}

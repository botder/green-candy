/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPoolsSA.cpp
*  PURPOSE:     Game entity pools
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define FUNC_InitGamePools  0x00550F10

extern CBaseModelInfoSAInterface **ppModelInfo;

// DEPRECATED globals. Use Pools namespace methods instead!
CPtrNodeSinglePool      **ppPtrNodeSinglePool = (CPtrNodeSinglePool**)CLASS_CPtrNodeSingleLinkPool;
CPtrNodeDoublePool      **ppPtrNodeDoublePool = (CPtrNodeDoublePool**)CLASS_CPtrNodeDoubleLinkPool;
CEntryInfoPool          **ppEntryInfoPool = (CEntryInfoPool**)CLASS_CEntryInfoNodePool;

CVehiclePool            **ppVehiclePool = (CVehiclePool**)CLASS_CVehiclePool;
CPedPool                **ppPedPool = (CPedPool**)CLASS_CPedPool;
CBuildingPool           **ppBuildingPool = (CBuildingPool**)CLASS_CBuildingPool;
CObjectPool             **ppObjectPool = (CObjectPool**)CLASS_CObjectPool;

CDummyPool              **ppDummyPool = (CDummyPool**)CLASS_CDummyPool;

CColModelPool           **ppColModelPool = (CColModelPool**)CLASS_CColModelPool;

CTaskPool               **ppTaskPool = (CTaskPool**)CLASS_CTaskPool;
CEventPool              **ppEventPool = (CEventPool**)CLASS_CEventPool;

CPointRoutePool         **ppPointRoutePool = (CPointRoutePool**)CLASS_CPointRoutePool;
CPatrolRoutePool        **ppPatrolRoutePool = (CPatrolRoutePool**)CLASS_CPatrolRoutePool;
CNodeRoutePool          **ppNodeRoutePool = (CNodeRoutePool**)CLASS_CNodeRoutePool;

CTaskAllocatorPool      **ppTaskAllocatorPool = (CTaskAllocatorPool**)CLASS_CTaskAllocatorPool;
CPedIntelligencePool    **ppPedIntelligencePool = (CPedIntelligencePool**)CLASS_CPedIntelligencePool;
CPedAttractorPool       **ppPedAttractorPool = (CPedAttractorPool**)CLASS_CPedAttractorPool;

// MTA Pools
CMTAVehiclePool *mtaVehiclePool;
CMTAPedPool *mtaPedPool;
CMTAObjectPool *mtaObjectPool;
CMTAPlayerDataPool *mtaPlayerDataPool;
CMTAObjectDataPool *mtaObjectDataPool;

CPoolsSA::CPoolsSA( void )
{
    DEBUG_TRACE("CPoolsSA::CPoolsSA( void )");

    // Do not let GTA:SA init pools!
    *(unsigned char*)FUNC_InitGamePools = 0xC3;

    Pools::GetPtrNodeSinglePool() = new CPtrNodeSinglePool();
    Pools::GetPtrNodeDoublePool() = new CPtrNodeDoublePool();
    Pools::GetEntryInfoPool() = new CEntryInfoPool();

    Pools::GetVehiclePool() = new CVehiclePool();
    Pools::GetPedPool() = new CPedPool();
    Pools::GetBuildingPool() = new CBuildingPool();
    Pools::GetObjectPool() = new CObjectPool();

    Pools::GetDummyPool() = new CDummyPool();
    Pools::GetColModelPool() = new CColModelPool();

    Pools::GetTaskPool() = new CTaskPool();
    Pools::GetEventPool() = new CEventPool();

    Pools::GetPointRoutePool() = new CPointRoutePool();
    Pools::GetPatrolRoutePool() = new CPatrolRoutePool();
    Pools::GetNodeRoutePool() = new CNodeRoutePool();

    Pools::GetTaskAllocatorPool() = new CTaskAllocatorPool();
    Pools::GetPedIntelligencePool() = new CPedIntelligencePool();
    Pools::GetPedAttractorPool() = new CPedAttractorPool();

    // Init our pools
    mtaVehiclePool = new CMTAVehiclePool;
    mtaPlayerDataPool = new CMTAPlayerDataPool;
    mtaPedPool = new CMTAPedPool;
    mtaObjectDataPool = new CMTAObjectDataPool;
    mtaObjectPool = new CMTAObjectPool;

    // Initialize the entity cache
    memset( mtaVehicles, 0, sizeof(mtaVehicles) );
    memset( mtaPeds, 0, sizeof(mtaPeds) );
    memset( mtaObjects, 0, sizeof(mtaObjects) );
}

CPoolsSA::~CPoolsSA( void )
{
    // Clear all our entities and delete the pools
    mtaVehiclePool->Clear();
    mtaPedPool->Clear();
    mtaObjectPool->Clear();

    //todo: clean destruction of interfaces through managers.
    // i.e. entities have to be removed from world first.

    delete mtaObjectPool;
    delete mtaObjectDataPool;
    delete mtaPedPool;
    delete mtaPlayerDataPool;
    delete mtaVehiclePool;

    delete Pools::GetPtrNodeSinglePool();
    delete Pools::GetPtrNodeDoublePool();
    delete Pools::GetEntryInfoPool();

    // Seems to be crashy
#if TODO
    delete Pools::GetVehiclePool();
    delete Pools::GetPedPool();
    delete Pools::GetBuildingPool();
    delete Pools::GetObjectPool();

    delete Pools::GetDummyPool();
#endif
    delete Pools::GetColModelPool();

    delete Pools::GetTaskPool();
    delete Pools::GetEventPool();

    delete Pools::GetPointRoutePool();
    delete Pools::GetPatrolRoutePool();
    delete Pools::GetNodeRoutePool();

    delete Pools::GetTaskAllocatorPool();
    delete Pools::GetPedIntelligencePool();
    delete Pools::GetPedAttractorPool();
}

void Pools::DeleteAllBuildings( void )
{
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                    VEHICLES POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
CVehicleSA *mtaVehicles[MAX_VEHICLES];

// TODO: analyze this and put it into C++ code; will improve understanding and performance!
static inline CVehicleSAInterface* CreateVehicle( modelId_t modelId )
{
    // for SA, we can just call the following function and it should work:
    // SCM_CreateCarForScript(int,class CVector,unsigned char)
    //                              ModelID, Position, IsMissionVehicle
    CVehicleSAInterface *veh;

    DWORD dwFunc = FUNC_CCarCtrlCreateCarForScript;
    _asm
    {
        push    0           // its a mission vehicle
        push    0
        push    0
        push    0           // spawn at 0,0,0
        mov     eax,modelId
        push    eax
        call    dwFunc
        add     esp, 0x14
        mov     veh, eax
    }

    return veh;
}

CBicycleSA* Pools::AddBicycle( modelId_t modelID )
{
    DEBUG_TRACE("CBicycleSA* Pools::AddBicycle( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_BICYCLE )
        return NULL;

    return new CBicycleSA( (CBicycleSAInterface*)CreateVehicle( modelID ) );
}

CBikeSA* Pools::AddBike( modelId_t modelID )
{
    DEBUG_TRACE("CBikeSA* Pools::AddBike( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE )
        return NULL;

    switch( info->GetVehicleType() )
    {
    case VEHICLE_BIKE:
        return new CBikeSA( (CBikeSAInterface*)CreateVehicle( modelID ) );
    case VEHICLE_BICYCLE:
        return new CBicycleSA( (CBicycleSAInterface*)CreateVehicle( modelID ) );
    }

    return NULL;
}

CHeliSA* Pools::AddHeli( modelId_t modelID )
{
    DEBUG_TRACE("CHeliSA* Pools::AddHeli( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_HELI )
        return NULL;

    return new CHeliSA( (CHeliSAInterface*)CreateVehicle( modelID ) );
}

CPlaneSA* Pools::AddPlane( modelId_t modelID )
{
    DEBUG_TRACE("CPlaneSA* Pools::AddPlane( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_PLANE )
        return NULL;

    return new CPlaneSA( (CPlaneSAInterface*)CreateVehicle( modelID ) );
}

CTrainSA* Pools::AddTrain( modelId_t modelID, const CVector& pos, bool direction )
{
    DEBUG_TRACE("CTrainSA* Pools::AddTrain( modelId_t modelID, const CVector& pos, bool direction )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_TRAIN )
        return NULL;

    // clean the existing array
    memset( (unsigned int*)VAR_TrainModelArray, 0, 32 * sizeof(unsigned int) );

    // We only want one model
    *(unsigned short*)VAR_TrainModelArray = modelID; 

    CTrainSAInterface *trainBeginning;
    CTrainSAInterface *trainEnd;
    float fX = pos.fX;
    float fY = pos.fY;
    float fZ = pos.fZ;

    DWORD dwFunc = FUNC_CTrain_CreateMissionTrain;
    _asm
    {
        push    0 // place as close to point as possible (rather than at node)? (maybe) (actually seems to have an effect on the speed, so changed from 1 to 0)
        push    0 // start finding closest from here 
        push    -1 // node to start at (-1 for closest node)
        lea     ecx, trainEnd
        push    ecx // end of train
        lea     ecx, trainBeginning 
        push    ecx // begining of train
        push    0 // train type (always use 0 as thats where we're writing to)
        push    direction
        push    fZ
        push    fY
        push    fX
        call    dwFunc
        add     esp, 0x28
    }

    CTrainSA *trainHead = new CTrainSA( trainBeginning );
#if 0
    CVehicleSA *carriage = trainHead;
    
    while ( carriage )
    {
        CVehicleSAInterface* vehCarriage = carriage->GetNextCarriageInTrain ();

        if ( !vehCarriage )
            break;

        carriage = new CVehicleSA ( vehCarriage );
    }
#endif //0

    // Stops the train from moving at ludacrist speeds right after creation
    // due to some glitch in the node finding in CreateMissionTrain
    trainHead->SetMoveSpeed( CVector( 0, 0, 0 ) );

    return trainHead;
}

CAutomobileTrailerSA* Pools::AddTrailer( modelId_t modelID )
{
    DEBUG_TRACE("CAutomobileTrailerSA* Pools::AddTrailer( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_AUTOMOBILETRAILER )
        return NULL;

    return new CAutomobileTrailerSA( (CAutomobileTrailerSAInterface*)CreateVehicle( modelID ) );
}

CQuadBikeSA* Pools::AddQuadBike( modelId_t modelID )
{
    DEBUG_TRACE("CQuadBikeSA* Pools::AddQuadBike( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_QUADBIKE )
        return NULL;

    return new CQuadBikeSA( (CQuadBikeSAInterface*)CreateVehicle( modelID ) );
}

CMonsterTruckSA* Pools::AddMonsterTruck( modelId_t modelID )
{
    DEBUG_TRACE("CMonsterTruckSA* Pools::AddMonsterTruck( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add when the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_MONSTERTRUCK )
        return NULL;

    return new CMonsterTruckSA( (CMonsterTruckSAInterface*)CreateVehicle( modelID ) );
}

CAutomobileSA* Pools::AddAutomobile( modelId_t modelID )
{
    DEBUG_TRACE("CAutomobileSA* Pools::AddAutomobile( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add if the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE )
        return NULL;

    switch( info->GetVehicleType() )
    {
    case VEHICLE_PLANE:
        return AddPlane( modelID );
    case VEHICLE_HELI:
        return AddHeli( modelID );
    case VEHICLE_CAR:
        return new CAutomobileSA( (CAutomobileSAInterface*)CreateVehicle( modelID ) );
    case VEHICLE_AUTOMOBILETRAILER:
        return AddTrailer( modelID );
    case VEHICLE_QUADBIKE:
        return AddQuadBike( modelID );
    case VEHICLE_MONSTERTRUCK:
        return AddMonsterTruck( modelID );
    }

    return NULL;
}

CBoatSA* Pools::AddBoat( modelId_t modelID )
{
    DEBUG_TRACE("CBoatSA* Pools::AddBoat( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    // We do not wanna add if the pool is full
    if ( Pools::GetVehiclePool()->Full() )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE || info->GetVehicleType() != VEHICLE_BOAT )
        return NULL;

    return new CBoatSA( (CBoatSAInterface*)CreateVehicle( modelID ) );
}

CVehicleSA* Pools::AddVehicle( modelId_t modelID )
{
    DEBUG_TRACE("CVehicleSA* Pools::AddVehicle( modelId_t modelID )");

    if ( modelID > MAX_MODELS-1 )
        return NULL;

    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)ppModelInfo[modelID];

    if ( !info || info->GetModelType() != MODEL_VEHICLE )
        return NULL;

    switch( info->GetVehicleType() )
    {
    case VEHICLE_TRAIN:
        return AddTrain( modelID, CVector( 0, 0, 0 ), true );
    case VEHICLE_BIKE:
    case VEHICLE_BICYCLE:
        return AddBike( modelID );
    case VEHICLE_PLANE:
    case VEHICLE_HELI:
    case VEHICLE_CAR:
    case VEHICLE_AUTOMOBILETRAILER:
    case VEHICLE_QUADBIKE:
    case VEHICLE_MONSTERTRUCK:
        return AddAutomobile( modelID );
    case VEHICLE_BOAT:
        return AddBoat( modelID );
    }

    return NULL;
}

// Safe retrieval of our instance
CVehicleSA* Pools::GetVehicle( void *entity )
{
    unsigned int id = Pools::GetVehiclePool()->GetIndex( (CVehicleSAInterface*)entity );
    
    if ( id > MAX_VEHICLES-1 )
        return NULL;

    return mtaVehicles[id];
}

CVehicleSA* Pools::GetVehicleFromRef( unsigned int index )
{
    DEBUG_TRACE("CVehicleSA* Pools::GetVehicleFromRef( unsigned int index )");

    if ( index > MAX_VEHICLES-1 )
        return NULL;

    return mtaVehicles[index];
}

void Pools::DeleteAllVehicles( void )
{
    DEBUG_TRACE("void Pools::DeleteAllVehicles( void )");

    Pools::GetVehiclePool()->Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                     OBJECTS POOL                                     //
//////////////////////////////////////////////////////////////////////////////////////////
CObjectSA *mtaObjects[MAX_OBJECTS];

static inline CObjectSAInterface* CreateObject( modelId_t model )
{
    DWORD CObjectCreate = FUNC_CObject_Create;  
    CObjectSAInterface *obj;
    _asm
    {
        push    1
        mov     eax,model
        push    eax
        call    CObjectCreate
        add     esp, 8
        mov     obj, eax
    }

    World::AddEntity( obj );
    return obj;
}

CObjectSA* Pools::AddObject( modelId_t modelId )
{
    DEBUG_TRACE("CObjectSA* Pools::AddObject( modelId_t modelId )");

    if ( Pools::GetObjectPool()->Full() )
        return NULL;

    CBaseModelInfoSAInterface *info = ppModelInfo[modelId];

    return new CObjectSA( CreateObject( modelId ) );
}

CObjectSA* Pools::GetObject( void *entity )
{
    unsigned int id = Pools::GetObjectPool()->GetIndex( (CObjectSAInterface*)entity );

    if ( id > MAX_OBJECTS-1 )
        return NULL;

    return mtaObjects[ id ];
}

CObjectSA* Pools::GetObjectFromRef( unsigned int index )
{
    DEBUG_TRACE("CObjectSA* Pools::GetObjectFromRef( unsigned int index )");

    if ( index > MAX_OBJECTS-1 )
        return NULL;

    return mtaObjects[index];
}

void Pools::DeleteAllObjects( void )
{
    DEBUG_TRACE("void Pools::DeleteAllObjects( void )");

    Pools::GetObjectPool()->Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                       PEDS POOL                                      //
//////////////////////////////////////////////////////////////////////////////////////////
CPedSA *mtaPeds[MAX_PEDS];

inline static CPlayerPedSAInterface* CreatePlayerPed( void )
{
    // based on CPlayerPed::SetupPlayerPed (R*)
    DWORD CPlayerPedConstructor = FUNC_CPlayerPedConstructor;
    CPlayerPedSAInterface *player = (CPlayerPedSAInterface*)Pools::GetPedPool()->Allocate();

    _asm
    {
        mov     ecx, player
        push    0 // set to 0 and they'll behave like AI peds
        push    1
        call    CPlayerPedConstructor
    }

    return player;
}

CPedSA* Pools::AddPed( modelId_t modelId )
{
    DEBUG_TRACE("CPedSA* Pools::AddPed( modelId_t modelId )");

    if ( Pools::GetPedPool()->Full() )
        return NULL;

    if ( modelId > MAX_MODELS-1 )
        return NULL;

    CBaseModelInfoSAInterface *info = ppModelInfo[modelId];

    if ( !info || info->GetModelType() != MODEL_PED )
        return NULL;

    return new CPlayerPedSA( CreatePlayerPed(), modelId, false );
}

inline static CCivilianPedSAInterface* CreateCivilianPed( void )
{
    // based on CCivilianPed::SetupPlayerPed (R*)
    DWORD CCivilianPedConstructor = FUNC_CCivilianPedConstructor;
    CCivilianPedSAInterface *ped = (CCivilianPedSAInterface*)Pools::GetPedPool()->Allocate();

    _asm
    {
        mov     ecx,ped
        push    0 // set to 0 and they'll behave like AI peds
        push    1 // ped type
        call    CCivilianPedConstructor
    }

    //pGame->GetWorld()->Add( ped );
    return ped;
}

CCivilianPedSA* Pools::AddCivilianPed( modelId_t modelID )
{
    DEBUG_TRACE("CCivilianPedSA* Pools::AddCivilianPed( modelId_t modelID )");

    if ( Pools::GetPedPool()->Full() )
        return NULL;

    return new CCivilianPedSA( CreateCivilianPed(), modelID );
}

CCivilianPedSA* Pools::AddCivilianPed( void *entity )
{
    DEBUG_TRACE("CCivilianPedSA* Pools::AddCivilianPed( void *entity )");

    return new CCivilianPedSA( (CCivilianPedSAInterface*)entity, ((CCivilianPedSAInterface*)entity)->GetModelIndex() );
}

CPedSA* Pools::GetPed( void *entity )
{
    unsigned int id = Pools::GetPedPool()->GetIndex( (CPedSAInterface*)entity );

    if ( id >= MAX_PEDS )
        return NULL;

    return mtaPeds[ id ];
}

CPedSA* Pools::GetPedFromRef( unsigned int index )
{
    DEBUG_TRACE("CPedSA* Pools::GetPedFromRef( unsigned int index )");

    if ( index > MAX_PEDS )
        return NULL;

    return mtaPeds[index];
}

void Pools::DeleteAllPeds( void )
{
    DEBUG_TRACE("void Pools::DeleteAllPeds( void )");

    (*ppPedPool)->Clear();
}

// This function is insecure, because it accesses the entity prior to validation
// Has to stay this way for optimization purposes; usage only in 100% secure scenarios!
CEntitySA* Pools::GetEntity( void *entity )
{
    if ( !entity )
        return NULL;

    switch( ((CEntitySAInterface*)entity)->nType )
    {
    case ENTITY_TYPE_PED:       return GetPed( entity );
    case ENTITY_TYPE_VEHICLE:   return GetVehicle( entity );
    case ENTITY_TYPE_OBJECT:    return GetObject( entity );
    }

    return NULL;
}

CBuildingSA* Pools::AddBuilding( modelId_t modelID )
{
    DEBUG_TRACE("CBuildingSA* CPoolsSA::AddBuilding( modelId_t modelID )");

    if ( Pools::GetBuildingPool()->Full() )
        return NULL;
    
    return new CBuildingSA( modelID );
}

char szOutput[1024];

void CPoolsSA::DumpPoolsStatus( void ) const
{
    char*  poolNames[] = {"Buildings", "Peds", "Objects", "Dummies", "Vehicles", "ColModels", 
        "Tasks", "Events", "TaskAllocators", "PedIntelligences", "PedAttractors", 
        "EntryInfoNodes", "NodeRoutes", "PatrolRoutes", "PointRoutes", 
        "PointerNodeDoubleLinks", "PointerNodeSingleLinks" };

    int iPosition = 0;
    char percent = '%';

    iPosition += snprintf ( szOutput, 1024, "-----------------\n" );

    for ( int i = 0; i < MAX_POOLS; i++ )
    {
        int usedSpaces = GetNumberOfUsedSpaces ( (ePools)i );
        iPosition += snprintf( szOutput + iPosition, 1024 - iPosition, 
            "%s: %d (%d) (%.2f%c)\n", 
            poolNames[i], 
            usedSpaces, 
            GetPoolCapacity( (ePools)i ), 
            ((float)usedSpaces / (float)GetPoolCapacity( (ePools)i ) * 100), 
            percent );
    }

#ifdef MTA_DEBUG
    OutputDebugString ( szOutput );
#endif
}

unsigned int CPoolsSA::GetPoolDefaultCapacity( ePools pool ) const
{
    switch( pool )
    {
    case BUILDING_POOL:             return 13000;
    case PED_POOL:                  return 140;
    case OBJECT_POOL:               return 350;          // Modded to 700   @ CGameSA.cpp
    case DUMMY_POOL:                return 2500;
    case VEHICLE_POOL:              return 110;
    case COL_MODEL_POOL:            return 10150;
    case TASK_POOL:                 return 500;          // Modded to 5000   @ CGameSA.cpp
    case EVENT_POOL:                return 200;          // Modded to 9001   @ CMultiplayerSA.cpp
    case TASK_ALLOCATOR_POOL:       return 16;
    case PED_INTELLIGENCE_POOL:     return 140;
    case PED_ATTRACTOR_POOL:        return 64;
    case ENTRY_INFO_NODE_POOL:      return 500;          // Modded to 4096   @ CMultiplayerSA.cpp
    case NODE_ROUTE_POOL:           return 64;
    case PATROL_ROUTE_POOL:         return 32;
    case POINT_ROUTE_POOL:          return 64;
    case POINTER_DOUBLE_LINK_POOL:  return 3200;         // Modded to 4000   @ CMultiplayerSA.cpp
    case POINTER_SINGLE_LINK_POOL:  return 70000;
    case ENV_MAP_MATERIAL_POOL:     return 4096;
    case ENV_MAP_ATOMIC_POOL:       return 1024;
    case SPEC_MAP_MATERIAL_POOL:    return 4096;
    case COL_FILE_POOL:             return 256;
    case IPL_FILE_POOL:             return 256;
    }
    return 0;
}

unsigned int CPoolsSA::GetNumberOfUsedSpaces( ePools pool ) const
{
    switch( pool )
    {
    case BUILDING_POOL:             return Pools::GetBuildingPool()->GetCount();
    case PED_POOL:                  return Pools::GetPedPool()->GetCount();
    case OBJECT_POOL:               return Pools::GetObjectPool()->GetCount();
    case DUMMY_POOL:                return Pools::GetDummyPool()->GetCount();
    case VEHICLE_POOL:              return Pools::GetVehiclePool()->GetCount();
    case COL_MODEL_POOL:            return Pools::GetColModelPool()->GetCount();
    case TASK_POOL:                 return Pools::GetTaskPool()->GetCount();
    case EVENT_POOL:                return Pools::GetEventPool()->GetCount();
    case TASK_ALLOCATOR_POOL:       return Pools::GetTaskAllocatorPool()->GetCount();
    case PED_INTELLIGENCE_POOL:     return Pools::GetPedIntelligencePool()->GetCount();
    case PED_ATTRACTOR_POOL:        return Pools::GetPedAttractorPool()->GetCount();
    case ENTRY_INFO_NODE_POOL:      return Pools::GetEntryInfoPool()->GetCount();
    case NODE_ROUTE_POOL:           return Pools::GetNodeRoutePool()->GetCount();
    case PATROL_ROUTE_POOL:         return Pools::GetPatrolRoutePool()->GetCount();
    case POINT_ROUTE_POOL:          return Pools::GetPointRoutePool()->GetCount();
    case POINTER_DOUBLE_LINK_POOL:  return Pools::GetPtrNodeDoublePool()->GetCount();
    case POINTER_SINGLE_LINK_POOL:  return Pools::GetPtrNodeSinglePool()->GetCount();
    case ENV_MAP_MATERIAL_POOL:     return RenderWare::GetEnvMapMaterialPool()->GetCount();
    case ENV_MAP_ATOMIC_POOL:       return RenderWare::GetEnvMapAtomicPool()->GetCount();
    case SPEC_MAP_MATERIAL_POOL:    return RenderWare::GetSpecMapMaterialPool()->GetCount();
    case COL_FILE_POOL:             return Streaming::GetCOLEnvironment().m_pool->GetCount();
    case IPL_FILE_POOL:             return Streaming::GetIPLEnvironment().m_pool->GetCount();
    }

    return 0;
}

unsigned int CPoolsSA::GetPoolCapacity( ePools pool ) const
{
    switch( pool )
    {
    case BUILDING_POOL:                 return Pools::GetBuildingPool()->GetMax();
    case PED_POOL:                      return Pools::GetPedPool()->GetMax();
    case OBJECT_POOL:                   return Pools::GetObjectPool()->GetMax();
    case DUMMY_POOL:                    return Pools::GetDummyPool()->GetMax();
    case VEHICLE_POOL:                  return Pools::GetVehiclePool()->GetMax();
    case COL_MODEL_POOL:                return Pools::GetColModelPool()->GetMax();
    case TASK_POOL:                     return Pools::GetTaskPool()->GetMax();
    case EVENT_POOL:                    return Pools::GetEventPool()->GetMax();
    case TASK_ALLOCATOR_POOL:           return Pools::GetTaskAllocatorPool()->GetMax();
    case PED_INTELLIGENCE_POOL:         return Pools::GetPedIntelligencePool()->GetMax();
    case PED_ATTRACTOR_POOL:            return Pools::GetPedAttractorPool()->GetMax();
    case ENTRY_INFO_NODE_POOL:          return Pools::GetEntryInfoPool()->GetMax();
    case NODE_ROUTE_POOL:               return Pools::GetNodeRoutePool()->GetMax();
    case PATROL_ROUTE_POOL:             return Pools::GetPatrolRoutePool()->GetMax();
    case POINT_ROUTE_POOL:              return Pools::GetPointRoutePool()->GetMax();
    case POINTER_DOUBLE_LINK_POOL:      return Pools::GetPtrNodeDoublePool()->GetMax();
    case POINTER_SINGLE_LINK_POOL:      return Pools::GetPtrNodeSinglePool()->GetMax();
    case ENV_MAP_MATERIAL_POOL:         return RenderWare::GetEnvMapMaterialPool()->GetMax();
    case ENV_MAP_ATOMIC_POOL:           return RenderWare::GetEnvMapAtomicPool()->GetMax();
    case SPEC_MAP_MATERIAL_POOL:        return RenderWare::GetSpecMapMaterialPool()->GetMax();
    case COL_FILE_POOL:                 return Streaming::GetCOLEnvironment().m_pool->GetMax();
    case IPL_FILE_POOL:                 return Streaming::GetIPLEnvironment().m_pool->GetMax();
    }

    return 0;
}

const char* CPoolsSA::GetPoolName( ePools pool ) const
{
    switch( pool )
    {
    case BUILDING_POOL:                 return "Buildings";
    case PED_POOL:                      return "Peds";
    case OBJECT_POOL:                   return "Objects";
    case DUMMY_POOL:                    return "Dummies";
    case VEHICLE_POOL:                  return "Vehicles";
    case COL_MODEL_POOL:                return "Collisions";
    case TASK_POOL:                     return "Tasks";
    case EVENT_POOL:                    return "Events";
    case TASK_ALLOCATOR_POOL:           return "Task Allocators";
    case PED_INTELLIGENCE_POOL:         return "Ped Intelligence";
    case PED_ATTRACTOR_POOL:            return "Ped Attractors";
    case ENTRY_INFO_NODE_POOL:          return "Entry Info";
    case NODE_ROUTE_POOL:               return "Node Routes";
    case PATROL_ROUTE_POOL:             return "Patrol Routes";
    case POINT_ROUTE_POOL:              return "Point Routes";
    case POINTER_DOUBLE_LINK_POOL:      return "PtrNode Double Links";
    case POINTER_SINGLE_LINK_POOL:      return "PtrNode Single Links";
    case ENV_MAP_MATERIAL_POOL:         return "Environment Map Materials";
    case ENV_MAP_ATOMIC_POOL:           return "Environment Map Atomics";
    case SPEC_MAP_MATERIAL_POOL:        return "Specular Map Materials";
    case COL_FILE_POOL:                 return "COL Sectors";
    case IPL_FILE_POOL:                 return "IPL Sectors";
    }

    return "unknown";
}
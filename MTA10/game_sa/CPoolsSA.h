/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPoolsSA.h
*  PURPOSE:     Header file for game entity pools class
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
#ifndef __CGAMESA_POOLS
#define __CGAMESA_POOLS

#include <game/CPools.h>

#include "CModelInfoSA.h"
#include "CClumpModelInfoSA.h"
#include "CVehicleModelInfoSA.h"
#include "CColModelSA.h"
#include "CEntryInfoSA.h"
#include "RenderWare/include.h"
#include "CEventSA.h"
#include "CBuildingSA.h"
#include "CDummySA.h"
#include "CRouteSA.h"
#include "CPedIntelligenceSA.h"
#include "CPedSA.h"
#include "CPlayerPedSA.h"
#include "CPlayerInfoSA.h"
#include "CCivilianPedSA.h"
#include "CVehicleComponentSA.h"
#include "CVehicleSA.h"
#include "CAutomobileSA.h"
#include "CHeliSA.h"
#include "CBoatSA.h"
#include "CBikeSA.h"
#include "CBicycleSA.h"
#include "CAutomobileTrailerSA.h"
#include "CTrainSA.h"
#include "CMonsterTruckSA.h"
#include "CQuadBikeSA.h"
#include "CPlaneSA.h"
#include "CObjectSA.h"
#include "CProjectileSA.h"
#include "CTaskAllocatorSA.h"

template <class type, int max, const size_t size = sizeof(type)>
class CPool
{
public:
    CPool( void )
    {
        m_pool = (type*)new unsigned char[ size * max ];
        m_flags = new unsigned char[ max ];

        m_poolActive = true;

        // Reset all flags
        memset(m_flags, 0x80, max);

        m_max = max;
        m_lastUsed = 0xFFFFFFFF;
    }

    inline type*    GetOffset( unsigned int id )
    {
        return (type*)( (unsigned int)m_pool + size * id );
    }

    // This function expects the entities to free themselves from this;
    // so do not decrease the count here!
    void    Clear( void )
    {
        for ( unsigned int n = 0; n < GetMax(); n++ )
        {
            if ( m_flags[n] & 0x80 )
                continue;

            delete GetOffset( n );
        }
    }

    ~CPool( void )
    {
        //Clear();

        if ( GetMax() > 0 )
        {
            if ( m_poolActive )
            {
                delete [] (unsigned char*)m_pool;
                delete [] m_flags;
            }

            m_pool = NULL;
            m_flags = NULL;
            m_max = 0;
            m_lastUsed = 0;
        }
    }

    inline type*    Allocate( void )
    {
        unsigned int n = ++m_lastUsed;
        unsigned int max = GetMax();

        for ( unsigned int i = 0; i < 2; i++ )
        {
            for ( ; n < max; n++ )
            {
                // If slot is used, we skip
                if ( !( m_flags[n] & 0x80 ) )
                    continue;

                // Mark slot as used
                m_flags[n] &= ~0x80;

                // Next iteration we start from next slot
                m_lastUsed = n;
                return GetOffset( n );
            }

            // Try another iteration starting from the beginning.
            // This ensures that there are no errors in the allocation system.
            n = 0;
            max = m_lastUsed;
            m_lastUsed = 0xFFFFFFFF;
        }

        return NULL;
    }

    inline type*    Get( unsigned int id )
    {
        return ( (id < GetMax()) && !(m_flags[id] & 0x80) ) ? GetOffset( id ) : NULL;
    }

    unsigned int    GetIndex( type *entity ) const
    {
        return ((unsigned int)entity - (unsigned int)m_pool) / size;
    }

    bool    IsValid( type *entity ) const
    {
        return entity >= m_pool && GetIndex( entity ) < GetMax();
    }

    void    Free( unsigned int id )
    {
        if ( id >= m_max )
            return;

        if ( m_flags[id] & 0x80 )
            return;

        m_flags[id] |= 0x80;
        
        if ( id < m_lastUsed )
            m_lastUsed = id - 1;
    }

    void    Free( type *entity )
    {
        // For compatibility with MSVCRT
        if ( entity == NULL )
            return;

        Free( GetIndex( entity ) );
    }

    bool    IsAnySlotFree( void ) const
    {
        for ( unsigned int n = m_lastUsed + 1; n < GetMax(); n++ )
        {
            if ( m_flags[n] & 0x80 )
                return true;
        }

        return false;
    }

    bool    Full( void ) const
    {
        // We do not have to count all of the slots.
        return !IsAnySlotFree();
    }

    unsigned int    GetCount( void ) const
    {
        // Count all occupied slots in this pool
        unsigned int count = 0;

        for ( unsigned int n = 0; n < GetMax(); n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                count++;
        }

        return count;
    }

    unsigned int    GetMax( void ) const
    {
        // We enable compiler optimizations by using the constant template variable.
        // Using the dynamic variable here may enable support for hacks (which dynamically extend pool size).
        // Why should we enable that kind of support?
        return max;
    }

    template <typename callbackType>
    inline void     ForAllActiveEntries( callbackType& cb, unsigned int startIndex = 0 )
    {
        for ( unsigned int n = startIndex; n < GetMax(); n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                cb.OnEntry( GetOffset( n ), n );
        }
    }

    template <typename callbackType>
    inline void     ForAllActiveReverse( callbackType& cb, unsigned int stopIndex = 0 )
    {
        for ( unsigned int n = GetMax() - 1; n >= stopIndex; n++ )
        {
            if ( !( m_flags[n] & 0x80 ) )
                cb.OnEntry( GetOffset( n ), n );
        }
    }

    type*           m_pool;
    unsigned char*  m_flags;
    unsigned int    m_max;
    unsigned int    m_lastUsed;
    bool            m_poolActive;
};

class CPedAttractorSA
{
public:
    BYTE            m_pad[196];
};

#define MAX_DUMMIES     40000
#define MAX_IPL         256

// Rockstar's inheritance trick; keep these chains updated!
#define MAX_VEHICLE_SIZE ( MAX(sizeof(CHeliSAInterface),MAX(sizeof(CTrainSAInterface),MAX(sizeof(CAutomobileSAInterface),MAX(sizeof(CBikeSAInterface),MAX(sizeof(CBicycleSAInterface),MAX(sizeof(CPlaneSAInterface),MAX(sizeof(CBoatSAInterface),MAX(sizeof(CAutomobileTrailerSAInterface),MAX(sizeof(CQuadBikeSAInterface),sizeof(CMonsterTruckSAInterface)))))))))) )

#define MAX_PED_SIZE ( MAX(sizeof(CPedSAInterface),MAX(sizeof(CPlayerPedSAInterface),sizeof(CCivilianPedSAInterface))) )

#define MAX_OBJECT_SIZE ( MAX(sizeof(CObjectSAInterface),sizeof(CProjectileSAInterface)) )

typedef CPool <CVehicleComponentInfoSAInterface, 500> CVehicleComponentInfoPool;
typedef CPool <CColModelSAInterface, 20000> CColModelPool;

typedef CPool <CPtrNodeSingleSA <void>, 100000> CPtrNodeSinglePool; // Allocated for everything that renders on the world (using sector streaming)
typedef CPool <CPtrNodeDoubleSA <void>, 200000> CPtrNodeDoublePool;  // Allocated for physical entities of special types
typedef CPool <CEntryInfoSA, 100000> CEntryInfoPool; // info for every entity in the world (I think!)

typedef CPool <CVehicleSAInterface, MAX_VEHICLES, MAX_VEHICLE_SIZE> CVehiclePool;
typedef CPool <CPedSAInterface, MAX_PEDS, MAX_PED_SIZE> CPedPool;
typedef CPool <CObjectSAInterface, MAX_OBJECTS, MAX_OBJECT_SIZE> CObjectPool;

typedef CPool <CBuildingSAInterface, MAX_BUILDINGS> CBuildingPool;
typedef CPool <CDummySAInterface, MAX_DUMMIES> CDummyPool;

typedef CPool <CTaskSAInterface, MAX_TASKS, 128> CTaskPool;
typedef CPool <CEventSAInterface, 5000> CEventPool;
typedef CPool <CPointRouteSA, 64> CPointRoutePool;
typedef CPool <CPatrolRouteSA, 32> CPatrolRoutePool;
typedef CPool <CNodeRouteSA, 64> CNodeRoutePool;
typedef CPool <CTaskAllocatorSA, 16> CTaskAllocatorPool;

typedef CPool <CPedIntelligenceSAInterface, MAX_PEDS> CPedIntelligencePool;
typedef CPool <CPedAttractorSA, 64> CPedAttractorPool;

typedef CPool <CQuadTreeNodeSAInterface <void>, 400> CQuadTreeNodePool;

// They have to be defined somewhere!
extern CVehicleComponentInfoPool** ppVehicleComponentInfoPool;
extern CColModelPool** ppColModelPool;

extern CPtrNodeSinglePool** ppPtrNodeSinglePool;
extern CPtrNodeDoublePool** ppPtrNodeDoublePool;
extern CEntryInfoPool** ppEntryInfoPool;

extern CVehiclePool** ppVehiclePool;
extern CPedPool** ppPedPool;
extern CObjectPool** ppObjectPool;

extern CBuildingPool** ppBuildingPool;
extern CDummyPool** ppDummyPool;

extern CTaskPool** ppTaskPool;
extern CEventPool** ppEventPool;
extern CPointRoutePool** ppPointRoutePool;
extern CPatrolRoutePool** ppPatrolRoutePool;
extern CNodeRoutePool** ppNodeRoutePool;
extern CTaskAllocatorPool** ppTaskAllocatorPool;

extern CPedIntelligencePool** ppPedIntelligencePool;
extern CPedAttractorPool** ppPedAttractorPool;

extern CQuadTreeNodePool** ppQuadTreeNodePool;

// MTA pools; lets use the trick ourselves, shall we? :P
// Do not forget to extend this chain once new interfaces are spotted!
#define MAX_MTA_VEHICLE_SIZE ( MAX(sizeof(CVehicleSA),MAX(sizeof(CTrainSA),MAX(sizeof(CPlaneSA),MAX(sizeof(CHeliSA),MAX(sizeof(CBikeSA),MAX(sizeof(CBicycleSA),MAX(sizeof(CAutomobileTrailerSA),MAX(sizeof(CBoatSA),MAX(sizeof(CQuadBikeSA),sizeof(CMonsterTruckSA)))))))))) )

#define MAX_MTA_PED_SIZE ( MAX(sizeof(CPedSA),MAX(sizeof(CPlayerPedSA),sizeof(CCivilianPedSA))) )

#define MAX_MTA_OBJECT_SIZE ( MAX(sizeof(CObjectSA),sizeof(CProjectileSA)) )

// this thing doesnt work! compiler error [out of memory/too complex]
//#define MAX_MTA_TASK_SIZE ( MAX(MAX_PHYSICAL_TASK_SIZE, MAX(MAX_CAR_TASK_SIZE, MAX(MAX_CAR_UTIL_TASK_SIZE, MAX(MAX_GOTO_TASK_SIZE, MAX(MAX_ACTION_TASK_SIZE, MAX(sizeof(CTaskSimpleDuckSA), MAX(sizeof(CTaskSimpleSA), MAX(sizeof(CTaskComplexSA), MAX(MAX_BASIC_TASK_SIZE, MAX_ATTACK_TASK_SIZE))))))))) )

typedef CPool <CVehicleSA, MAX_VEHICLES, MAX_MTA_VEHICLE_SIZE> CMTAVehiclePool;
typedef CPool <CPedSA, MAX_PEDS, MAX_MTA_PED_SIZE> CMTAPedPool;
typedef CPool <CObjectSA, MAX_OBJECTS, MAX_MTA_OBJECT_SIZE> CMTAObjectPool;
typedef CPool <CTaskSA, MAX_TASKS, 128> CMTATaskPool; // we align the tasks, please keep this size-value up-to-date!
typedef CPool <CPlayerPedDataSAInterface, MAX_PEDS> CMTAPlayerDataPool;
typedef CPool <dynamicObjectData, MAX_OBJECTS> CMTAObjectDataPool;

extern CMTAVehiclePool *mtaVehiclePool;
extern CMTAPedPool *mtaPedPool;
extern CMTAObjectPool *mtaObjectPool;
extern CMTATaskPool *mtaTaskPool;
extern CMTAPlayerDataPool *mtaPlayerDataPool;
extern CMTAObjectDataPool *mtaObjectDataPool;

// Native memory addresses of pools.
#define CLASS_CPool_VehicleModels           0xB4E680

#define CLASS_CPool_Vehicle                 0xB74494
#define CLASS_CPool_Ped                     0xB74490
#define CLASS_CPool_Object                  0xB7449C

#define CLASS_CBuildingPool                 0xb74498
#define CLASS_CPedPool                      0xb74490
#define CLASS_CObjectPool                   0xb7449c
#define CLASS_CDummyPool                    0xb744a0
#define CLASS_CVehiclePool                  0xb74494
#define CLASS_CColModelPool                 0xb744a4
#define CLASS_CTaskPool                     0xb744a8
#define CLASS_CEventPool                    0xb744ac
#define CLASS_CTaskAllocatorPool            0xb744bc
#define CLASS_CPedIntelligencePool          0xb744c0
#define CLASS_CPedAttractorPool             0xb744c4
#define CLASS_CEntryInfoNodePool            0xb7448c
#define CLASS_CNodeRoutePool                0xb744b8
#define CLASS_CPatrolRoutePool              0xb744b4
#define CLASS_CPointRoutePool               0xb744b0
#define CLASS_CPtrNodeDoubleLinkPool        0xB74488
#define CLASS_CPtrNodeSingleLinkPool        0xB74484
#define CLASS_CColFilePool                  0x965560
#define CLASS_CIPLFilePool                  0x8E3FB0

// Export a namespace, since it avoids the class environment at native level (to speed up look-up).
// I noticed that this file causes issues with Visual Studio IntelliSense, hopefully this helps solve it.
namespace Pools
{
    // Vehicles pool
    CBicycleSA*             AddBicycle                  ( modelId_t modelId );
    CBikeSA*                AddBike                     ( modelId_t modelId );
    CHeliSA*                AddHeli                     ( modelId_t modelId );
    CPlaneSA*               AddPlane                    ( modelId_t modelId );
    CTrainSA*               AddTrain                    ( modelId_t modelId, const CVector& pos, bool direction );
    CAutomobileTrailerSA*   AddTrailer                  ( modelId_t modelId );
    CQuadBikeSA*            AddQuadBike                 ( modelId_t modelId );
    CMonsterTruckSA*        AddMonsterTruck             ( modelId_t modelId );
    CAutomobileSA*          AddAutomobile               ( modelId_t modelId );
    CBoatSA*                AddBoat                     ( modelId_t modelId );
    CVehicleSA*             AddVehicle                  ( modelId_t modelID );
    CVehicleSA*             GetVehicle                  ( void *entity );
    CVehicleSA*             GetVehicleFromRef           ( unsigned int index );
    void                    DeleteAllVehicles           ( void );

    // Objects pool
    CObjectSA*              AddObject                   ( modelId_t modelID );
    CObjectSA*              GetObject                   ( void *entity );
    CObjectSA*              GetObjectFromRef            ( unsigned int index );
    void                    DeleteAllObjects            ( void );

    // Peds pool
    CPedSA*                 AddPed                      ( modelId_t modelID );
    CCivilianPedSA*         AddCivilianPed              ( modelId_t modelID );
    CCivilianPedSA*         AddCivilianPed              ( void *ped );
    CPedSA*                 GetPed                      ( void *entity );
    CPedSA*                 GetPedFromRef               ( unsigned int index );
    void                    DeleteAllPeds               ( void );

    // Common Utilities
    CEntitySA*              GetEntity                   ( void *entity );

    // Others
    CBuildingSA*            AddBuilding                 ( modelId_t modelID );
    void                    DeleteAllBuildings          ( void );

    // Pool inline functions.
    // These should be used instead of the globals.
    inline CPtrNodeSinglePool*&    GetPtrNodeSinglePool         ( void )        { return *(CPtrNodeSinglePool**)CLASS_CPtrNodeSingleLinkPool; }
    inline CPtrNodeDoublePool*&    GetPtrNodeDoublePool         ( void )        { return *(CPtrNodeDoublePool**)CLASS_CPtrNodeDoubleLinkPool; }
    inline CEntryInfoPool*&        GetEntryInfoPool             ( void )        { return *(CEntryInfoPool**)CLASS_CEntryInfoNodePool; }

    inline CVehiclePool*&          GetVehiclePool               ( void )        { return *(CVehiclePool**)CLASS_CVehiclePool; }
    inline CPedPool*&              GetPedPool                   ( void )        { return *(CPedPool**)CLASS_CPedPool; }
    inline CBuildingPool*&         GetBuildingPool              ( void )        { return *(CBuildingPool**)CLASS_CBuildingPool; }
    inline CObjectPool*&           GetObjectPool                ( void )        { return *(CObjectPool**)CLASS_CObjectPool; }

    inline CDummyPool*&            GetDummyPool                 ( void )        { return *(CDummyPool**)CLASS_CDummyPool; }

    inline CColModelPool*&         GetColModelPool              ( void )        { return *(CColModelPool**)CLASS_CColModelPool; }

    inline CTaskPool*&             GetTaskPool                  ( void )        { return *(CTaskPool**)CLASS_CTaskPool; }
    inline CEventPool*&            GetEventPool                 ( void )        { return *(CEventPool**)CLASS_CEventPool; }
 
    inline CPointRoutePool*&       GetPointRoutePool            ( void )        { return *(CPointRoutePool**)CLASS_CPointRoutePool; }
    inline CPatrolRoutePool*&      GetPatrolRoutePool           ( void )        { return *(CPatrolRoutePool**)CLASS_CPatrolRoutePool; }
    inline CNodeRoutePool*&        GetNodeRoutePool             ( void )        { return *(CNodeRoutePool**)CLASS_CNodeRoutePool; }

    inline CTaskAllocatorPool*&    GetTaskAllocatorPool         ( void )        { return *(CTaskAllocatorPool**)CLASS_CTaskAllocatorPool; }
    inline CPedIntelligencePool*&  GetPedIntelligencePool       ( void )        { return *(CPedIntelligencePool**)CLASS_CPedIntelligencePool; }
    inline CPedAttractorPool*&     GetPedAttractorPool          ( void )        { return *(CPedAttractorPool**)CLASS_CPedAttractorPool; }
};

class CPoolsSA : public CPools
{
public:
                            CPoolsSA                    ( void );
                            ~CPoolsSA                   ( void );

public:
    // Vehicles pool
    CBicycleSA*             AddBicycle                  ( modelId_t modelId )                                       { return Pools::AddBicycle( modelId ); }
    CBikeSA*                AddBike                     ( modelId_t modelId )                                       { return Pools::AddBike( modelId ); }
    CHeliSA*                AddHeli                     ( modelId_t modelId )                                       { return Pools::AddHeli( modelId ); }
    CPlaneSA*               AddPlane                    ( modelId_t modelId )                                       { return Pools::AddPlane( modelId ); }
    CTrainSA*               AddTrain                    ( modelId_t modelId, const CVector& pos, bool direction )   { return Pools::AddTrain( modelId, pos, direction ); }
    CAutomobileTrailerSA*   AddTrailer                  ( modelId_t modelId )                                       { return Pools::AddTrailer( modelId ); }
    CQuadBikeSA*            AddQuadBike                 ( modelId_t modelId )                                       { return Pools::AddQuadBike( modelId ); }
    CMonsterTruckSA*        AddMonsterTruck             ( modelId_t modelId )                                       { return Pools::AddMonsterTruck( modelId ); }
    CAutomobileSA*          AddAutomobile               ( modelId_t modelId )                                       { return Pools::AddAutomobile( modelId ); }
    CBoatSA*                AddBoat                     ( modelId_t modelId )                                       { return Pools::AddBoat( modelId ); }
    CVehicleSA*             AddVehicle                  ( modelId_t modelId )                                       { return Pools::AddVehicle( modelId ); }
    CVehicleSA*             GetVehicle                  ( void *entity ) const                                      { return Pools::GetVehicle( entity ); }
    CVehicleSA*             GetVehicleFromRef           ( unsigned int index ) const                                { return Pools::GetVehicleFromRef( index ); }
    void                    DeleteAllVehicles           ( void )                                                    { Pools::DeleteAllVehicles(); }

    // Objects pool
    CObjectSA*              AddObject                   ( modelId_t modelID )                                       { return Pools::AddObject( modelID ); }
    CObjectSA*              GetObject                   ( void *entity ) const                                      { return Pools::GetObject( entity ); }
    CObjectSA*              GetObjectFromRef            ( unsigned int index ) const                                { return Pools::GetObjectFromRef( index ); }
    void                    DeleteAllObjects            ( void )                                                    { Pools::DeleteAllObjects(); }

    // Peds pool
    CPedSA*                 AddPed                      ( modelId_t modelID )                                       { return Pools::AddPed( modelID ); }
    CCivilianPedSA*         AddCivilianPed              ( modelId_t modelID )                                       { return Pools::AddCivilianPed( modelID ); }
    CCivilianPedSA*         AddCivilianPed              ( void *ped )                                               { return Pools::AddCivilianPed( ped ); }
    CPedSA*                 GetPed                      ( void *entity ) const                                      { return Pools::GetPed( entity ); }
    CPedSA*                 GetPedFromRef               ( unsigned int index ) const                                { return Pools::GetPedFromRef( index ); }
    void                    DeleteAllPeds               ( void )                                                    { Pools::DeleteAllPeds(); }

    CEntitySA*              GetEntity                   ( void *entity ) const                                      { return Pools::GetEntity( entity ); }

    // Others
    CBuildingSA*            AddBuilding                 ( modelId_t modelID )                                       { return Pools::AddBuilding( modelID ); }
    void                    DeleteAllBuildings          ( void )                                                    { Pools::DeleteAllBuildings(); }

    unsigned int            GetNumberOfUsedSpaces       ( ePools pools ) const;
    void                    DumpPoolsStatus             ( void ) const;

    unsigned int            GetPoolDefaultCapacity      ( ePools pool ) const;
    unsigned int            GetPoolCapacity             ( ePools pool ) const;
    const char*             GetPoolName                 ( ePools pool ) const;
};

// Global linkage between internal interfaces and MTA
extern CVehicleSA *mtaVehicles[MAX_VEHICLES];
extern CPedSA *mtaPeds[MAX_PEDS];
extern CObjectSA *mtaObjects[MAX_OBJECTS];


#define FUNC_GetVehicle                     0x54fff0
#define FUNC_GetVehicleRef                  0x54ffc0
//#define FUNC_GetVehicleCount              0x429510
#define FUNC_GetPed                         0x54ff90
#define FUNC_GetPedRef                      0x54ff60
//#define FUNC_GetPedCount                  0x4A7440
#define FUNC_GetObject                      0x550050
#define FUNC_GetObjectRef                   0x550020
//#define FUNC_GetObjectCount               0x4A74D0

#define FUNC_CTrain_CreateMissionTrain                      0x6F7550
#define VAR_TrainModelArray                                 0x8D44F8

#endif

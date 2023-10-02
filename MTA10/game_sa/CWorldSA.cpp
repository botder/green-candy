/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CWorldSA.cpp
*  PURPOSE:     Game world/entity logic
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/*=========================================================
    World::AddEntity

    Arguments:
        entity - the entity to add into the world
    Purpose:
        Links the given entity into the world so it is
        processed by the game loop.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00563220
=========================================================*/
void __cdecl World::AddEntity( CEntitySAInterface *entity )
{
    if ( RwObject *rwobj = entity->GetRwObject() )
    {
        entity->Placeable.GetMatrix( rwobj->parent->modelling );
    }

    entity->UpdateRwFrame();
    
    entity->AddToWorld();

    unsigned char entityType = entity->nType;

    if ( entityType != ENTITY_TYPE_BUILDING && entityType != ENTITY_TYPE_DUMMY && !IS_ANY_FLAG( entity->m_entityFlags, ENTITY_WAITFORCOLL | ENTITY_STATIC ) )
    {
        ((CPhysicalSAInterface*)entity)->Link();
    }
}

/*=========================================================
    World::RemoveEntity

    Arguments:
        entity - the entity to remove from the world
    Purpose:
        Unlinks the given entity from the game. It will no
        longer be processed by the game loop.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00563280
=========================================================*/
void __cdecl World::RemoveEntity( CEntitySAInterface *entity )
{
    // Call the common entity removal routine.
    entity->RemoveFromWorld();

    // Unlink the entity from world if it is of physical type.
    switch( entity->nType )
    {
    case ENTITY_TYPE_VEHICLE:
    case ENTITY_TYPE_PED:
    case ENTITY_TYPE_OBJECT:
        ((CPhysicalSAInterface*)entity)->Unlink();
        break;
    }
}

/*=========================================================
    World::SetCenterOfWorld (MTA extension)

    Arguments:
        streamingEntity - entity used for streaming reference
        pos - center of world position, if NULL then
              custom center of world disabled
        heading - player heading
    Purpose:
        Sets a fixed center of world for dynamic streaming
        purposes.
=========================================================*/
static bool isCenterOfWorldSet = false;
static CVector centerOfWorld = CVector( 0, 0, 0 );
static float falseHeading = 0.0f;
static CEntitySAInterface *_streamingEntity = NULL;

void World::SetCenterOfWorld( CEntitySAInterface *streamingEntity, const CVector *pos, float heading )
{
    if ( pos )
    {
        centerOfWorld = *pos;

        _streamingEntity = streamingEntity;

        falseHeading = heading;
        isCenterOfWorldSet = true;
    }
    else 
    {
        _streamingEntity = NULL;
        isCenterOfWorldSet = false;
    }
}

/*=========================================================
    World::GetCenterOfWorld (MTA extension)

    Arguments:
        pos - pointer to write the center into
    Purpose:
        Returns a boolean whether a static center of world
        was set by MTA. If true, then the center of world
        is written into pos.
=========================================================*/
bool World::GetCenterOfWorld( CVector& pos )
{
    if ( !isCenterOfWorldSet )
        return false;

    pos = centerOfWorld;
    return true;
}

bool World::IsCenterOfWorldSet( void )
{
    return isCenterOfWorldSet;
}

const CVector& World::GetCenterOfWorld( void )
{
    return centerOfWorld;
}

/*=========================================================
    World::GetStreamingEntity (MTA extension)

    Purpose:
        Returns the entity which is used for streaming
        reference (can be NULL).
=========================================================*/
CEntitySAInterface* World::GetStreamingEntity( void )
{
    return _streamingEntity;
}

/*=========================================================
    World::GetFalseHeading (MTA extension)

    Purpose:
        Returns a false heading used when center of world
        streaming is active.
=========================================================*/
float World::GetFalseHeading( void )
{
    return falseHeading;
}

CWorldSA::CWorldSA( void )
{
}

CWorldSA::~CWorldSA( void )
{
}

void CWorldSA::Add ( CEntity * pEntity )
{
    DEBUG_TRACE("void CWorldSA::Add ( CEntity * pEntity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    if ( pEntitySA )
    {
        World::AddEntity( pEntitySA->GetInterface() );
    }
}


void CWorldSA::Add ( CEntitySAInterface * entityInterface )
{
    DEBUG_TRACE("void CWorldSA::Add ( CEntitySAInterface * entityInterface )");

    World::AddEntity( entityInterface );
}

void CWorldSA::Remove ( CEntity * pEntity )
{
    DEBUG_TRACE("void CWorldSA::Remove ( CEntity * entity )");

    CEntitySA* pEntitySA = dynamic_cast < CEntitySA* > ( pEntity );

    World::RemoveEntity( pEntitySA->GetInterface() );
}

void CWorldSA::Remove ( CEntitySAInterface * entityInterface )
{
    DEBUG_TRACE("void CWorldSA::Remove ( CEntitySAInterface * entityInterface )");
    
    World::RemoveEntity( entityInterface );
}

void CWorldSA::RemoveReferencesToDeletedObject ( CEntitySAInterface * entity )
{
    DWORD dwFunc = FUNC_RemoveReferencesToDeletedObject;
    DWORD dwEntity = (DWORD)entity;
    _asm
    {
        push    dwEntity
        call    dwFunc
        add     esp, 4
    }
}

bool CWorldSA::TestLineSphere( CVector * vecStart, CVector * vecEnd, CVector * vecSphereCenter, float fSphereRadius, CColPoint ** colCollision )
{
    // THIS FUNCTION IS INCOMPLETE AND SHOULD NOT BE USED
    // Create a CColLine for us
    DWORD dwFunc = FUNC_CColLine_Constructor;
    DWORD dwCColLine[10]; // I don't know how big CColLine is, so we'll just be safe
    _asm
    {
        lea     ecx, dwCColLine
        push    vecEnd
        push    vecStart
        call    dwFunc
    }

    // Now, lets make a CColSphere
    BYTE byteColSphere[18]; // looks like its 18 bytes { vecPos, fSize, byteUnk, byteUnk, byteUnk }
    dwFunc = FUNC_CColSphere_Set;
    _asm
    {
        lea     ecx, byteColSphere
        push    255
        push    0
        push    0
        push    vecSphereCenter
        push    fSphereRadius
        call    dwFunc
    }
}

bool CWorldSA::ProcessLineOfSight( const CVector * vecStart, const CVector * vecEnd, CColPoint ** colCollision, 
                                  CEntity ** CollisionEntity,
                                  const SLineOfSightFlags flags,
                                  SLineOfSightBuildingResult* pBuildingResult )
{
    DEBUG_TRACE("VOID CWorldSA::ProcessLineOfSight(CVector * vecStart, CVector * vecEnd, CColPoint * colCollision, CEntity * CollisionEntity)");
    DWORD dwPadding[100]; // stops the function missbehaving and overwriting the return address
    dwPadding [0] = 0;  // prevent the warning and eventual compiler optimizations from removing it

    CColPointSA * pColPointSA = new CColPointSA();
    CColPointSAInterface * pColPointSAInterface = pColPointSA->GetInterface();  

    //DWORD targetEntity;
    CEntitySAInterface * targetEntity = NULL;
    bool bReturn = false;

    DWORD dwFunc = FUNC_ProcessLineOfSight;
    // bool bCheckBuildings = true,                 bool bCheckVehicles = true,     bool bCheckPeds = true, 
    // bool bCheckObjects = true,                   bool bCheckDummies = true,      bool bSeeThroughStuff = false, 
    // bool bIgnoreSomeObjectsForCamera = false,    bool bShootThroughStuff = false

    _asm
    {
        push    flags.bShootThroughStuff
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        lea     eax, targetEntity
        push    eax
        push    pColPointSAInterface    
        push    vecEnd
        push    vecStart    
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x30
    }

    if ( targetEntity )
    {
        // Building info needed?
        if ( pBuildingResult && targetEntity->nType == ENTITY_TYPE_BUILDING )
        {
            pBuildingResult->bValid = true;
            pBuildingResult->usModelID = targetEntity->GetModelIndex();
            pBuildingResult->vecPosition = targetEntity->Placeable.GetPosition();

            if ( targetEntity->Placeable.matrix )
            {
                CVector& vecRotation = pBuildingResult->vecRotation;
                targetEntity->Placeable.matrix->GetRotationRad( vecRotation.fX, vecRotation.fY, vecRotation.fZ );
                vecRotation = -vecRotation;
            }
        }

        if ( CollisionEntity )
            *CollisionEntity = pGame->GetPools()->GetEntity( targetEntity );
    }

    if ( colCollision )
        *colCollision = pColPointSA;
    else
        pColPointSA->Destroy ();

    return bReturn;
}

void CWorldSA::IgnoreEntity( CEntity * pEntity )
{
    DEBUG_TRACE("VOID CWorldSA::IgnoreEntity(CEntity * entity)");

    if ( !pEntity )
        *(CEntitySAInterface**)VAR_IgnoredEntity = NULL;
    else
        *(CEntitySAInterface**)VAR_IgnoredEntity = dynamic_cast < CEntitySA* > ( pEntity )->GetInterface();
}

// technically this is in CTheZones
BYTE CWorldSA::GetLevelFromPosition( CVector * vecPosition )
{
    DEBUG_TRACE("BYTE CWorldSA::GetLevelFromPosition(CVector * vecPosition)");

    DWORD dwFunc = FUNC_GetLevelFromPosition;
    BYTE bReturn = 0;
    _asm
    {
        push    vecPosition
        call    dwFunc
        mov     bReturn, al
        pop     eax
    }
    return bReturn;
}

float CWorldSA::FindGroundZForPosition( float fX, float fY )
{
    DEBUG_TRACE("FLOAT CWorldSA::FindGroundZForPosition(FLOAT fX, FLOAT fY)");

    DWORD dwFunc = FUNC_FindGroundZFor3DCoord;
    float fReturn = 0;

    _asm
    {
        push    fY
        push    fX
        call    dwFunc
        fstp    fReturn
        add     esp, 8
    }
    return fReturn;
}

float CWorldSA::FindGroundZFor3DPosition( CVector * vecPosition)
{
    DEBUG_TRACE("FLOAT CWorldSA::FindGroundZFor3DPosition(CVector * vecPosition)");
    DWORD dwFunc = FUNC_FindGroundZFor3DCoord;
    float fReturn = 0;
    float fX = vecPosition->fX;
    float fY = vecPosition->fY;
    float fZ = vecPosition->fZ;

    _asm
    {
        push    0
        push    0
        push    fZ
        push    fY
        push    fX
        call    dwFunc
        fstp    fReturn
        add     esp, 0x14
    }
    return fReturn;
}

void CWorldSA::LoadMapAroundPoint(CVector * vecPosition, FLOAT fRadius)
{
    DEBUG_TRACE("VOID CWorldSA::LoadMapAroundPoint(CVector * vecPosition, FLOAT fRadius)");
    DWORD dwFunc = FUNC_CTimer_Stop;
    _asm
    {
        call    dwFunc
    }

    dwFunc = FUNC_CRenderer_RequestObjectsInDirection;
    _asm
    {
        push    32
        push    fRadius
        push    vecPosition
        call    dwFunc
        add     esp, 12
    }

    dwFunc = FUNC_CStreaming_LoadScene;
    _asm
    {
        push    vecPosition
        call    dwFunc
        add     esp, 4
    }

    dwFunc = FUNC_CTimer_Update;
    _asm
    {
        call    dwFunc
    }
}

bool CWorldSA::IsLineOfSightClear ( const CVector * vecStart, const CVector * vecEnd, const SLineOfSightFlags flags )
{
    DWORD dwFunc = FUNC_IsLineOfSightClear;
    bool bReturn = false;

    _asm
    {
        push    flags.bIgnoreSomeObjectsForCamera
        push    flags.bSeeThroughStuff
        push    flags.bCheckDummies
        push    flags.bCheckObjects
        push    flags.bCheckPeds
        push    flags.bCheckVehicles
        push    flags.bCheckBuildings
        push    vecEnd
        push    vecStart    
        call    dwFunc
        mov     bReturn, al
        add     esp, 0x24
    }
    return bReturn;
}

bool CWorldSA::HasCollisionBeenLoaded ( CVector * vecPosition )
{
    DWORD dwFunc = FUNC_HasCollisionBeenLoaded;
    bool bRet = false;

    _asm
    {
        push    0
        push    vecPosition
        call    dwFunc
        mov     bRet, al
        add     esp, 8
    } 
    return bRet;
}

unsigned int CWorldSA::GetCurrentArea ()
{
    return *(unsigned int*)VAR_currArea;
}

void CWorldSA::SetCurrentArea ( unsigned int area )
{
    *(unsigned int*)VAR_currArea = area;

    DWORD dwFunc = FUNC_RemoveBuildingsNotInArea;
    _asm
    {
        push    area
        call    dwFunc
        add     esp, 4
    }
}

void CWorldSA::SetJetpackMaxHeight ( float height )
{
    *(float*)VAR_fJetpackMaxHeight = height;
}

float CWorldSA::GetJetpackMaxHeight ()
{
    return *(float*)VAR_fJetpackMaxHeight;
}

void CWorldSA::SetAircraftMaxHeight ( float height )
{
    *(float*)VAR_fAircraftMaxHeight = height;
}

float CWorldSA::GetAircraftMaxHeight ()
{
    return *(float*)VAR_fAircraftMaxHeight;
}

bool CWorldSA::ProcessVerticalLine( const CVector& pos, float distance, CColPointSAInterface& colPoint, CEntitySAInterface **hitEntity,
                                    bool unk1, bool unk2, bool unk3, bool unk4, bool unk5, bool unk6, bool unk7 )
{
    return ((bool (__cdecl*)( const CVector&, float, CColPointSAInterface&, CEntitySAInterface **, bool, bool, bool, bool, bool, bool, bool ))0x005674E0)
                              ( pos, distance, colPoint, hitEntity, unk1, unk2, unk3, unk4, unk5, unk6, unk7 );
}
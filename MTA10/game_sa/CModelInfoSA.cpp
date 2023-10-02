/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CModelInfoSA.cpp
*  PURPOSE:     Entity model information handler
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CGameSA * pGame;

CBaseModelInfoSAInterface** ppModelInfo = (CBaseModelInfoSAInterface**) ARRAY_ModelInfo;

std::map < unsigned short, int > CModelInfoSA::ms_RestreamTxdIDMap;

void ModelInfo_Init( void )
{
    ModelInfoBase_Init();
    ModelInfoIDE_Init();
}

void ModelInfo_Shutdown( void )
{
    ModelInfoIDE_Shutdown();
    ModelInfoBase_Shutdown();
}

CModelInfoSA::CModelInfoSA( void )
{
    m_pInterface = NULL;
    m_modelID = 0xFFFF;
    m_dwReferences = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomColModel = NULL;
}

CModelInfoSA::CModelInfoSA( unsigned short id )
{
    m_modelID = id;
    m_pInterface = ppModelInfo [ id ];
    m_dwReferences = 0;
    m_pOriginalColModelInterface = NULL;
    m_pCustomColModel = NULL;
}

eVehicleType CModelInfoSA::GetVehicleType() const
{
    return ((CVehicleModelInfoSAInterface*)ppModelInfo[m_modelID])->GetVehicleType();
}

bool CModelInfoSA::IsBoat() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_BOAT;
}

bool CModelInfoSA::IsCar() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_CAR;
}

bool CModelInfoSA::IsTrain() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_TRAIN;
}   

bool CModelInfoSA::IsHeli() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_HELI;
}   

bool CModelInfoSA::IsPlane() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_PLANE;
}   

bool CModelInfoSA::IsBike() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_BIKE;
}

bool CModelInfoSA::IsBmx() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_BICYCLE;
}   

bool CModelInfoSA::IsTrailer() const
{
    if ( !IsVehicle() )
        return false;

    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetVehicleType() == VEHICLE_AUTOMOBILETRAILER;
}   

bool CModelInfoSA::IsVehicle() const
{
    return m_pInterface->GetModelType() == MODEL_VEHICLE;
}   

bool CModelInfoSA::IsUpgrade() const
{
    return m_modelID >= 1000 && m_modelID <= 1193;
}

bool CModelInfoSA::IsPed() const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[m_modelID];

    if ( !info )
        return false;

    return info->GetModelType() == MODEL_PED;
}

bool CModelInfoSA::IsObject() const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[m_modelID];

    if ( !info )
        return false;

    return info->GetRwModelType() == RW_ATOMIC;
}

const char* CModelInfoSA::GetNameIfVehicle() const
{
    DEBUG_TRACE("const char* CModelInfoSA::GetNameIfVehicle() const");

    DWORD dwModelInfo = ARRAY_ModelInfo;
    DWORD dwFunc = FUNC_CText_Get;
    DWORD ModelID = m_modelID;
    DWORD dwReturn = 0;

    _asm
    {       
        push    eax
        push    ebx
        push    ecx

        mov     ebx, ModelID
        lea     ebx, [ebx*4]
        add     ebx, ARRAY_ModelInfo
        mov     eax, [ebx]
        add     eax, 50

        push    eax
        mov     ecx, CLASS_CText
        call    dwFunc

        mov     dwReturn, eax

        pop     ecx
        pop     ebx
        pop     eax
    }
    return (const char*)dwReturn;
}

void CModelInfoSA::Request( bool bAndLoad, bool bWaitForLoad, bool bHighPriority )
{
    DEBUG_TRACE("void CModelInfoSA::Request( bool bAndLoad, bool bWaitForLoad, bool bHighPriority )");

    // don't bother loading it if it already is
    if ( IsLoaded() )
        return;

    DWORD dwFlags;
    if ( bHighPriority )
        dwFlags = 0x16;
    else
        dwFlags = 6;

    pGame->GetStreaming()->RequestModel( m_modelID, dwFlags );

    if ( bAndLoad )
        pGame->GetStreaming()->LoadAllRequestedModels();
}

VOID CModelInfoSA::Remove ( )
{
    DEBUG_TRACE("VOID CModelInfoSA::Remove ( )");

    // Don't remove if GTA refers to it somehow.
    // Or we'll screw up SA's map for example.

    m_pInterface = ppModelInfo [ m_modelID ];

    // Remove our reference
    m_pInterface->Dereference();

    // No references left?
    if ( m_pInterface->GetRefCount() == 0 )
    {  
        // Remove the model
        pGame->GetStreaming()->FreeModel( m_modelID );
    }
}

unsigned char CModelInfoSA::GetLevelFromPosition( const CVector& vecPosition ) const
{
    DEBUG_TRACE("unsigned char CModelInfoSA::GetLevelFromPosition( const CVector& vecPosition ) const");
    DWORD dwFunction = FUNC_GetLevelFromPosition;
    BYTE bReturn = 0;
    _asm
    {
        push    vecPosition
        call    dwFunction
        add     esp, 4
        mov     bReturn, al
    }
    return bReturn;
}

bool CModelInfoSA::IsLoaded( void ) const
{
    DEBUG_TRACE("bool CModelInfoSA::IsLoaded( void ) const");

    if ( IsUpgrade() )
        return pGame->GetStreaming()->HasVehicleUpgradeLoaded( m_modelID );

    return pGame->GetStreaming()->HasModelLoaded(m_modelID);
}

unsigned char CModelInfoSA::GetFlags( void ) const
{
    return Streaming::GetModelLoadInfo( m_modelID ).m_flags;
}

const CBoundingBox* CModelInfoSA::GetBoundingBox( void ) const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[m_modelID];

    if ( !info )
        return NULL;

    if ( !info->pColModel )
        return NULL;

    return &info->pColModel->m_bounds;
}

bool CModelInfoSA::IsValid( void )
{
    return( m_pInterface = ppModelInfo[m_modelID] ) != 0;
}

float CModelInfoSA::GetDistanceFromCentreOfMassToBaseOfModel( void ) const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[m_modelID];

    if ( !info )
        return 0;

    return -info->pColModel->m_bounds.vecBoundMin.fZ;
}

unsigned short CModelInfoSA::GetTextureDictionaryID( void ) const
{
    CBaseModelInfoSAInterface *info = ppModelInfo[m_modelID];

    if ( !info )
        return 0xFFFF;

    return info->usTextureDictionary;
}

void CModelInfoSA::SetTextureDictionaryID( unsigned short usID )
{
    if ( !m_pInterface )
        return;

    m_pInterface->usTextureDictionary = usID;
}

float CModelInfoSA::GetLODDistance() const
{
    if ( !m_pInterface )
        return 0;

    return m_pInterface->GetLODDistance();
}

void CModelInfoSA::SetLODDistance( float fDistance )
{
#if 0
    // fLodDistanceUnscaled values:
    //
    // With the draw distance setting in GTA SP options menu set to maximum:
    //      0 - 170     roughly correlates to a LOD distance of 0 - 300 game units
    //      170 - 480   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      490 - 1e7   sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // With the draw distance setting in GTA SP options menu set to minimum:
    //      0 - 325     roughly correlates to a LOD distance of 0 - 300 game units
    //      340 - 960   sets the LOD distance to 300 game units and has a negative effect on the alpha fade-in
    //      1000 - 1e7  sets the LOD distance to 300 game units and removes the alpha fade-in completely
    //
    // So, to ensure the maximum draw distance with a working alpha fade-in, fLodDistanceUnscaled has to be
    // no more than: 325 - (325-170) * draw_distance_setting
    //

    // Change GTA draw distance value from 0.925 to 1.8 into 0 to 1
    float fDrawDistanceSetting = UnlerpClamped ( 0.925f, CSettingsSA ().GetDrawDistance (), 1.8f );

    // Calc max setting allowed for fLodDistanceUnscaled to preserve alpha fade-in
    float fMaximumValue = Lerp ( 325.f, fDrawDistanceSetting, 170.f );

    // Ensure fDistance is in range
    fDistance = Min ( fDistance, fMaximumValue );
#endif
    if ( !m_pInterface )
        return;

    m_pInterface->SetLODDistance( fDistance );
}

void CModelInfoSA::RestreamIPL()
{
    MapSet( ms_RestreamTxdIDMap, GetTextureDictionaryID(), 0 );
}

// Used to validate an entity pointer
static bool _ValidateEntity( CEntitySAInterface *entity )
{
    return Pools::GetBuildingPool()->Get( Pools::GetBuildingPool()->GetIndex( (CBuildingSAInterface*)entity ) ) != NULL ||
           Pools::GetPedPool()->Get( Pools::GetPedPool()->GetIndex( (CPedSAInterface*)entity ) ) != NULL ||
           Pools::GetVehiclePool()->Get( Pools::GetVehiclePool()->GetIndex( (CVehicleSAInterface*)entity ) ) != NULL ||
           Pools::GetObjectPool()->Get( Pools::GetObjectPool()->GetIndex( (CObjectSAInterface*)entity ) ) != NULL ||
           Pools::GetDummyPool()->Get( Pools::GetDummyPool()->GetIndex( (CDummySAInterface*)entity ) ) != NULL;
}

// Struct used to delete the models of all entities whose texture dictionary has changed.
typedef std::set <unsigned short> modelAssocList_t;

struct _entityModelRefresh
{
    void __forceinline OnSector( Streamer::streamSectorEntry& sector )
    {
        for ( Streamer::streamSectorEntry::ptrNode_t *iter = sector.GetList(); iter != NULL; iter = iter->m_next )
        {
            CEntitySAInterface *pEntity = iter->data;

            if ( !_ValidateEntity( pEntity ) )
                __asm int 3

            if ( MapContains( CModelInfoSA::ms_RestreamTxdIDMap, pEntity->GetModelInfo()->usTextureDictionary ) )
            {
                if ( !IS_FLAG( pEntity->m_entityFlags, ENTITY_DISABLESTREAMING ) && !IS_FLAG( pEntity->m_entityFlags, ENTITY_RENDERING ) )
                {
                    // Delete the renderware object
                    pEntity->DeleteRwObject();

                    // Remember the model.
                    m_modelList.insert( pEntity->GetModelIndex() );
                }
            }
        }
    }

    modelAssocList_t m_modelList;
};

void CModelInfoSA::StaticFlushPendingRestreamIPL( void )
{
    if ( ms_RestreamTxdIDMap.empty() )
        return;
    // This function restreams all instances of the model *that are from the default SA world (ipl)*.
    // In other words, it does not affect elements created by MTA.
    // It's mostly a reimplementation of SA's DeleteAllRwObjects, except that it filters by model ID.

    Streaming::FlushRequestList();

    // Container of all to-be-restreamed model ids.
    _entityModelRefresh modelGather;

    // Scan all streamed in entities and delete their RenderWare objects which need to be updated.
    Streamer::ForAllStreamerSectors( modelGather, true, true, true, true, true, true );

    // We no longer need to update models depending on texture containers.
    ms_RestreamTxdIDMap.clear();

    // Free the models which we deleted all RenderWare instances of.
    for ( modelAssocList_t::iterator it = modelGather.m_modelList.begin(); it != modelGather.m_modelList.end(); it++ )
    {
        Streaming::FreeModel( *it );
    }
}

void CModelInfoSA::AddRef( bool bWaitForLoad, bool bHighPriority )
{
    if ( m_dwReferences == 0 )
    {
        // Notify the client
        if ( ModelManager::modelRequestCallback )
            ModelManager::modelRequestCallback( m_modelID );
    }

    // Are we not loaded?
    if ( !IsLoaded () )
    {
        // Request it. Wait for it to load if we're asked to.
        if ( pGame && pGame->IsASyncLoadingEnabled () )
            Request ( bWaitForLoad, bWaitForLoad, bHighPriority );
        else
            Request ( true, bWaitForLoad, bHighPriority );
    }

    // Increment the references.
    if ( m_dwReferences == 0 )
    {
        m_pInterface = ppModelInfo [ m_modelID ];

        m_pInterface->Reference();
    }

    m_dwReferences++;
}

void CModelInfoSA::RemoveRef( bool bRemoveExtraGTARef )
{
    // Decrement the references
    if ( m_dwReferences > 0 )
        m_dwReferences--;

    // Handle extra ref if requested
    if ( bRemoveExtraGTARef )
        MaybeRemoveExtraGTARef ();

    // Unload it if 0 references left and we're not CJ model.
    // And if we're loaded.
    if ( m_dwReferences == 0 )
    {
        if ( m_modelID != 0 && IsLoaded() )
            Remove();

        // Notify the client
        if ( ModelManager::modelFreeCallback )
            ModelManager::modelFreeCallback( m_modelID );
    }
}

//
// Notes on changing model for the local player only
// -------------------------------------------------
// When changing model for the local player, the model ref count is not decremented correctly. (Due to the ped not being re-created.)
// Eventually, after many changes of local player skin, we run out of entries in COL_MODEL_POOL
//
// So, when changing model for the local player, an extra reference must be removed to allow the old model to be unloaded.
//
// However, manually removing the extra reference creates a memory leak when the model is unloaded.
//
// Therefore, to keep the memory loss to a minimum, the removal of the extra reference is delayed.
//
// The delay scheme implemented to do this consists of two queues:
//      The first queue is filled up with the first 5 models that are used for the local player. Once full the queue is not changed until MTA is restarted.
//      The second queue is FILO, with the last out having it's extra reference removed.
//
// * The_GTA: maybe somebody can improve this? with all the research we have made, bugs are a big no-no.
//

std::vector < CModelInfoSA* >   ms_FirstDelayQueue;     // Don't remove ref for first 5 unique models
std::list < CModelInfoSA* >     ms_SecondDelayQueue;    // Then delay remove for 5 changes

void CModelInfoSA::MaybeRemoveExtraGTARef()
{
    // Safety check
    if ( m_pInterface->GetRefCount() == 0 )
        return;

    //
    // Handle first queue
    //
    if ( ListContains ( ms_FirstDelayQueue, this ) )
    {
        // If already delaying a previous extra ref, we can remove this extra ref now
        m_pInterface->Dereference();
        return;
    }

    if ( ms_FirstDelayQueue.size () < 5 )
    {
        // Fill initial queue
        ms_FirstDelayQueue.push_back ( this );
        return;
    }

    //
    // Handle second queue
    //
    if ( ListContains ( ms_SecondDelayQueue, this ) )
    {
        // If already delaying a previous extra ref, we can remove this extra ref now
        m_pInterface->Dereference();

        // Bring item to front
        ListRemove ( ms_SecondDelayQueue, this );
        ms_SecondDelayQueue.push_back ( this );
    }
    else
    {
        // Top up secondary queue
        ms_SecondDelayQueue.push_back ( this );
        
        // Remove extra ref from the oldest queue item
        if ( ms_SecondDelayQueue.size () > 5 )
        {
            CModelInfoSA* pOldModelInfo = ms_SecondDelayQueue.front ();
            ms_SecondDelayQueue.pop_front ();
            pOldModelInfo->DoRemoveExtraGTARef ();
        }
    }
}

// Remove extra GTA ref and handle actual model unload if then required
void CModelInfoSA::DoRemoveExtraGTARef()
{
    if (m_modelID == 0)
        return;

    m_pInterface->Reference(); // Hack because Remove() decrements this
    Remove ();
}

short CModelInfoSA::GetAvailableVehicleMod( unsigned short usUpgrade ) const
{
    short sreturn = -1;
    if ( usUpgrade >= 1000 && usUpgrade <= 1193 )
    {
        DWORD ModelID = m_modelID;
        _asm
        {
            mov     eax, ModelID
            movsx   edx, usUpgrade
            mov     eax, ARRAY_ModelInfo[eax*4]
            mov     ax, [eax+edx*2+0x2D6]
            mov     sreturn, ax
        }
    }
    return sreturn;
}

bool CModelInfoSA::IsUpgradeAvailable( eVehicleUpgradePosn posn ) const
{
    bool bRet = false;
    DWORD ModelID = m_modelID;
    _asm
    {
        mov     eax, ModelID
        lea     ecx, ARRAY_ModelInfo[eax*4]

        mov     eax, posn
        mov     ecx, [ecx+0x5C]
        shl     eax, 5
        push    esi
        mov     esi, [ecx+eax+0D0h]
        xor     edx, edx
        test    esi, esi
        setnl   dl
        mov     al, dl
        pop     esi

        mov     bRet, al
    }
    return bRet;
}

void CModelInfoSA::SetCustomCarPlateText( const char *szText )
{
    if ( !IsVehicle() )
        return;

    char *szStoredText = ((CVehicleModelInfoSAInterface*)GetInterface())->plateText;

    if ( szText ) 
        strncpy ( szStoredText, szText, 8 );
    else
        szStoredText[0] = 0;
}

unsigned int CModelInfoSA::GetNumRemaps() const
{
    return ((CVehicleModelInfoSAInterface*)m_pInterface)->GetNumberOfValidPaintjobs();
}

void* CModelInfoSA::GetVehicleSuspensionData() const
{
    return ppModelInfo[m_modelID]->pColModel->pColData->pSuspensionLines;
}

void* CModelInfoSA::SetVehicleSuspensionData( void* pSuspensionLines )
{
    CColDataSA* pColData = m_pInterface->pColModel->pColData;
    void* pOrigSuspensionLines = pColData->pSuspensionLines;

    pColData->pSuspensionLines = (CColSuspensionDataSA*)pSuspensionLines;
    return pOrigSuspensionLines;
}

void CModelInfoSA::RequestVehicleUpgrade()
{
    DWORD dwFunc = FUNC_RequestVehicleUpgrade;
    DWORD ModelID = m_modelID;
    _asm
    {
        push    10
        push    ModelID
        call    dwFunc
        add     esp, 8
    }
}

void CModelInfoSA::GetVoice( short* psVoiceType, short* psVoiceID ) const
{
    assert( IsPed() );

    if ( psVoiceType )
        *psVoiceType = GetPedModelInfoInterface ()->sVoiceType;

    if ( psVoiceID )
        *psVoiceID = GetPedModelInfoInterface ()->sFirstVoice;
}

void CModelInfoSA::GetVoice( const char** pszVoiceType, const char** pszVoice ) const
{
    assert( IsPed() );

    short sVoiceType, sVoiceID;
    GetVoice ( &sVoiceType, &sVoiceID );

    if ( pszVoiceType )
        *pszVoiceType = CPedSoundSA::GetVoiceTypeNameFromID ( sVoiceType );

    if ( pszVoice )
        *pszVoice = CPedSoundSA::GetVoiceNameFromID ( sVoiceType, sVoiceID );
}

void CModelInfoSA::SetVoice( short sVoiceType, short sVoiceID )
{
    assert( IsPed() );

    GetPedModelInfoInterface ()->sVoiceType = sVoiceType;
    GetPedModelInfoInterface ()->sFirstVoice = sVoiceID;
    GetPedModelInfoInterface ()->sLastVoice = sVoiceID;
    GetPedModelInfoInterface ()->sNextVoice = sVoiceID;
}

void CModelInfoSA::SetVoice( const char* szVoiceType, const char* szVoice )
{
    assert( IsPed() );

    short sVoiceType = CPedSoundSA::GetVoiceTypeIDFromName ( szVoiceType );
    if ( sVoiceType < 0 )
        return;

    short sVoiceID = CPedSoundSA::GetVoiceIDFromName ( sVoiceType, szVoice );
    if ( sVoiceID < 0 )
        return;

    SetVoice ( sVoiceType, sVoiceID );
}

void CModelInfoSA::MakePedModel( const char *szTexture )
{
    // Create a new CPedModelInfo
    CPedModelInfoSA pedModelInfo;
    ppModelInfo[m_modelID] = pedModelInfo.GetPedModelInfoInterface();

    // Load our texture
    pGame->GetStreaming ()->RequestSpecialModel ( m_modelID, szTexture, 0 );
}

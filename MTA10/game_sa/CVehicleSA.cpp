/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleSA.cpp
*  PURPOSE:     Vehicle base entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CGameSA* pGame;
extern CBaseModelInfoSAInterface **ppModelInfo;

CVehicleControlSAInterface::CVehicleControlSAInterface()
{
    unsigned int n;

    m_unk = -1;
    m_unk2 = -1;
    m_unk3 = -1;

    m_unk4 = -1;
    m_unk5 = -1;
    m_unk6 = -1;

    for ( n=0; n<8; n++ )
        m_unk7[n] = 0xFFFF;

    m_unk9 = true;
    m_unk8 = true;

    m_flags &= ~0x04 & 10;

    m_unk10 = NULL;
    m_unk11 = 1000;

    m_unk12 = 0;
    m_unk39 = 0;
    m_unk13 = 0;
    m_unk14 = 0;
    m_unk15 = 0;

    m_unk17 = 10;

    m_unk16 = 10;

    m_unk18 = 0;
    m_unk19 = NULL;

    m_flags2 &= ~0x04 & 10;

    m_creationTime = m_time = pGame->GetSystemTime();

    m_unk23 = 0;
    m_unk24 = 1;

    m_unk26 = 0xFF;

    m_unk28 = 0;

    m_unk34 = 20;

    m_unk29 = NULL;
    m_unk30 = 0;

    m_unk32 = 0;
    m_unk33 = 0;

    m_unk35 = 10;
    m_unk36 = 10;

    m_unk27 = 0;

    m_unk37 = rand() & 7 + 2;

    m_unk25 = NULL;
    m_unk38 = NULL;
}

CVehicleControlSAInterface::~CVehicleControlSAInterface()
{
}

CVehicleAudioSAInterface::CVehicleAudioSAInterface()
{
    m_unk = NULL;

    m_vtbl = (void*)0x00862CEC;

    m_unk4 = NULL;

    m_unk5 = (void*)0x0085F438;

    m_unk6 = 0;
    m_unk7 = 0;
    m_unk8 = 0;

    m_unk3 = 0;

    m_unk2 = NULL;
}

CVehicleAudioSAInterface::~CVehicleAudioSAInterface()
{
}

CVehicleSAInterface::CVehicleSAInterface( unsigned char createdBy )
{
    unsigned int n;

    new (&m_audio) CVehicleAudioSAInterface();
    new (&m_control) CVehicleControlSAInterface();

    nType = ENTITY_TYPE_VEHICLE;

    m_entityFlags |= ENTITY_PRERENDERED;

    m_unk = 0;
    m_steerAngle = 0;
    m_secondarySteerAngle = 0;

    // We always start off at gear 1
    m_currentGear = 1;

    m_gearChangeCount = 0;
    m_wheelSpinAudio = 0;

    // Set the creation flags
    m_createdBy = createdBy;

    m_forcedRandomSeed = 0;

    m_vehicleFlags = VEHICLE_ENGINESTATUS | VEHICLE_FREEBIES | VEHICLE_PLAYERPOSSESION;

    // Make the vehicle locked by random
    if ( (float)rand() / 0x7FFF <= 0 )
        m_vehicleFlags |= VEHICLE_LOCKED;

    m_genericFlags &= ~( VEHGENERIC_POLICETYPE | VEHGENERIC_NOSTATIC | VEHGENERIC_PARKING | VEHGENERIC_GANGLEANON | VEHGENERIC_GANGROADBLOCK | VEHGENERIC_ISGOODCOVER | VEHGENERIC_MADDRIVER );
    m_genericFlags |= VEHGENERIC_IMPOUNDED | VEHGENERIC_ALARM | VEHGENERIC_ISGOODCOVER | VEHGENERIC_PETROLTANK | VEHGENERIC_NOPARTICLES;

    // Do the population count
    HandlePopulation( true );

    m_bombOnBoard = 0;
    m_overrideLights = 0;
    m_winchType = 0;
    
    m_gunsCycleIndex = 0;
    m_ordnanceCycleIndex = 0;

    m_health = 1000;

    pDriver = NULL;
    m_numPassengers = 0;
    m_numGettingIn = 0;
    m_gettingInFlags = 0;
    m_gettingOutFlags = 0;

    m_maxPassengers = MAX_PASSENGERS;

    for ( n = 0; n < MAX_PASSENGERS; n++ )
        pPassengers[n] = NULL;

    m_ammoInClip &= ~0x10;

    m_usedForCover &= ~0x20;

    m_unk2 = -1;
    m_nSpecialColModel = -1;

    physicalFlags |= 0x10000000;

    m_unk2 = 0;
    m_unk3 = NULL;

    m_numPedsForRoadBlock = 0;

    m_unk5 = NULL;
    m_fire = NULL;

    m_unk6 = 0;
    m_pacMansCollected = 0;

    m_hornActive = 0;
    m_unk8 = 0;
    m_unk9 = 0;

    m_unk11 = NULL;
    m_unk12 = NULL;
    m_unk13 = NULL;
    m_unk14 = NULL;

    m_alarmState = 0;
    m_doorState = 1;

    m_unk15 = 0;
    m_unk16 = 0;
    m_unk17 = 0;

    m_unk18 = 0;

    m_unk20 = NULL;
    m_unk21 = NULL;
    
    m_entityVisibilityCheck = NULL;

    m_unk22 = 0;
    m_unk23 = 0;

    m_unk25 = 0;
    m_unk26 = 0;
    m_unk27 = 0;

    pHandlingData = NULL;
    m_handlingFlags = 0;

    m_control.m_unk14 = 0;
    m_control.m_unk15 = 0;
    m_control.m_creationTime = pGame->GetSystemTime();
    m_control.m_flags &= ~0x04;

    m_paintjobTxd = -1;
    m_queuePaintjob = -1;
    m_paintjobTexture = NULL;

    m_unk31 = 0;
    m_unk32 = 0;
    m_unk33 = 0;
    m_pCustomPlateTexture = NULL;

    for ( n = 0; n < MAX_UPGRADES_ATTACHED; n++ )
        m_upgrades[n] = -1;

    m_usedForCover &= ~0x60;

    m_wheelScale = 1;

    m_windowsOpenFlags = 0;
    m_nitroBoosts = 0;

    m_unk36 = 0;

    m_unk24 = 0;

    m_bodyDirtLevel = (float)( rand() % 15 );
    
    m_timeOfCreation = pGame->GetSystemTime();

    m_unk37 = 205089.12f;
}

CVehicleSAInterface::~CVehicleSAInterface()
{
    HandlePopulation( false );
}

void CVehicleSAInterface::HandlePopulation( bool create )
{
    CVehicleModelInfoSAInterface *info = (CVehicleModelInfoSAInterface*)GetModelInfo();

    if ( create )
    {
        switch( m_createdBy )
        {
        case 1:
            if ( info->GetVehicleType() & VEHICLE_POLICE )
                *((unsigned int*)VAR_PoliceVehicleCount) += 1;

            *((unsigned int*)VAR_VehicleCount) += 1;
            return;
        case 2:
            if ( info->GetVehicleType() & VEHICLE_POLICE )
                *((unsigned int*)VAR_PoliceVehicleCount) += 1;

            *((unsigned int*)0x0096909C) += 1;
            return;
        case 3:
            *((unsigned int*)0x009690A0) += 1;
            return;
        case 4:
            *((unsigned int*)0x009690A4) += 1;
            return;
        }

        return;
    }

    switch( m_createdBy )
    {
    case 1:
        if ( info->GetVehicleType() & VEHICLE_POLICE )
            *((unsigned int*)VAR_PoliceVehicleCount) -= 1;

        *((unsigned int*)VAR_VehicleCount) -= 1;
        return;
    case 2:
        *((unsigned int*)0x0096909C) -= 1;
        return;
    case 3:
        *((unsigned int*)0x009690A0) -= 1;
        return;
    case 4:
        *((unsigned int*)0x009690A4) -= 1;
        return;
    }
}

CVehicleSA::CVehicleSA( CVehicleSAInterface *veh ) : m_alpha( 255 ), m_vecGravity( 0.0f, 0.0f, -1.0f )
{
    DEBUG_TRACE("CVehicleSA::CVehicleSA( CVehicleSAInterface *veh )");

    m_pHandlingData = NULL;
    m_pInterface = veh;

    // Register the interface
    m_poolIndex = (*ppVehiclePool)->GetIndex( veh );
    mtaVehicles[m_poolIndex] = this;

    Init();    // Use common setup
}

void CVehicleSA::Init()
{
    // Destroy available components
    while ( !m_compContainer.empty() )
        delete m_compContainer.begin()->second;

    // We take care of the streaming, so disable GTA:SA control
    BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_DISABLESTREAMING, true );
    BOOL_FLAG( m_pInterface->m_entityFlags, ENTITY_NOSTREAM, true );

    // Reset the car counts to 0 so that this vehicle doesn't affect the population vehicles
    for ( int i = 0; i < 5; i++ )
        *((unsigned int*)VARS_CarCounts + i) = 0;

    BOOL_FLAG( GetInterface()->m_genericFlags, VEHGENERIC_TARGETABLE, true );

    m_alpha = 255;
    m_vecGravity = CVector ( 0.0f, 0.0f, -1.0f );

    m_RGBColors[0] = CVehicleColor::GetRGBFromPaletteIndex( GetInterface()->m_colour1 );
    m_RGBColors[1] = CVehicleColor::GetRGBFromPaletteIndex( GetInterface()->m_colour2 );
    m_RGBColors[2] = CVehicleColor::GetRGBFromPaletteIndex( GetInterface()->m_colour3 );
    m_RGBColors[3] = CVehicleColor::GetRGBFromPaletteIndex( GetInterface()->m_colour4 );
}

CVehicleSA::~CVehicleSA()
{
    DEBUG_TRACE("CVehicleSA::~CVehicleSA()");

    // Remove our registration
    mtaVehicles[m_poolIndex] = NULL;
}

void* CVehicleSA::operator new ( size_t )
{
    return mtaVehiclePool->Allocate();
}

void CVehicleSA::operator delete ( void *ptr )
{
    return mtaVehiclePool->Free( (CVehicleSA*)ptr );
}

void CVehicleSA::SetColor( SColor color1, SColor color2, SColor color3, SColor color4, int )
{
    m_RGBColors[0] = color1;
    m_RGBColors[1] = color2;
    m_RGBColors[2] = color3;
    m_RGBColors[3] = color4;
}

void CVehicleSA::GetColor( SColor& color1, SColor& color2, SColor& color3, SColor& color4, int ) const
{
    color1 = m_RGBColors[0];
    color2 = m_RGBColors[1];
    color3 = m_RGBColors[2];
    color4 = m_RGBColors[3];
}

void CVehicleSA::GetColor( SColor* color1, SColor* color2, SColor* color3, SColor* color4, int ) const
{
    GetColor( *color1, *color2, *color3, *color4, 0 );
}

CModel* CVehicleSA::CloneClump() const
{
    return new CModelSA( RpClumpClone( (RpClump*)GetInterface()->GetRwObject() ), NULL );
}

CVehicleComponent* CVehicleSA::GetComponent( const char *name )
{
    vehComponents_t::iterator iter;

    // Cache the component
    if ( ( iter = m_compContainer.find( name ) ) != m_compContainer.end() )
        return iter->second;

    RpClump *clump = (RpClump*)GetInterface()->GetRwObject();
    RwFrame *frame = clump->parent->FindChildByName( name );

    if ( !frame )
        return NULL;

    return new CVehicleComponentSA( m_compContainer, clump, frame );
}

static int RwFrameListNames( RwFrame *child, std::vector <std::string> *list )
{
    child->ForAllChildren( RwFrameListNames, list );

    list->push_back( child->szName );
    return true;
}

void CVehicleSA::GetComponentNameList( std::vector <std::string>& list )
{
    GetInterface()->GetRwObject()->parent->ForAllChildren( RwFrameListNames, &list );
}

void CVehicleSA::SetHealth( float health )
{
    GetInterface()->m_health = health;

    if ( health >= 250.0f )
        GetInterface()->m_explodeTime = 0;
}

bool CVehicleSA::AddProjectile( eWeaponType eWeapon, const CVector& vecOrigin, float fForce, const CVector& targetPos, CEntity *target )
{
    return pGame->GetProjectileInfo()->AddProjectile( (CEntitySA*)this, eWeapon, vecOrigin, fForce, targetPos, target );
}

void CVehicleSA::AddVehicleUpgrade( unsigned short model )
{   
    DEBUG_TRACE("void CVehicleSA::AddVehicleUpgrade( unsigned short model )");

    if ( model < 1000 || model > 1193 )
        return;

    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_AddVehicleUpgrade;

    _asm
    {
        mov     ecx, dwThis
        movzx   eax,model
        push    eax
        call    dwFunc
    }
}

void CVehicleSA::RemoveVehicleUpgrade( unsigned short model )
{
    DEBUG_TRACE("void CVehicleSA::RemoveVehicleUpgrade( unsigned short model )");

    DWORD dwThis = (DWORD)m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_RemoveVehicleUpgrade;

    _asm
    {
        mov     ecx, dwThis
        movzx   eax,model
        push    eax
        call    dwFunc
    }
}

void CVehicleSA::OpenDoor( unsigned char ucDoor, float fRatio, bool bMakeNoise )
{
    // Grab the car node index for the given door id
    static int s_iCarNodeIndexes [6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };

	GetInterface()->OpenDoor( false, s_iCarNodeIndexes[ucDoor], ucDoor, fRatio, bMakeNoise );
}

bool CVehicleSA::AreDoorsLocked() const
{
    return ( GetInterface()->m_doorState == 2 || GetInterface()->m_doorState == 5 || 
             GetInterface()->m_doorState == 4 || GetInterface()->m_doorState == 7 || 
             GetInterface()->m_doorState == 3 );
}

void CVehicleSA::LockDoors( bool bLocked )
{
    bool bAreDoorsLocked = AreDoorsLocked();
    bool bAreDoorsUndamageable = AreDoorsUndamageable();

    if ( bLocked && !bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetInterface()->m_doorState = 7;
        else
            GetInterface()->m_doorState = 2;
    }
    else if ( !bLocked && bAreDoorsLocked )
    {
        if ( bAreDoorsUndamageable )
            GetInterface()->m_doorState = 1;
        else
            GetInterface()->m_doorState = 0;
    }
}

void CVehicleSA::SetDoorsUndamageable( bool bUndamageable )
{
    bool bAreDoorsLocked = AreDoorsLocked();
    bool bAreDoorsUndamageable = AreDoorsUndamageable();

    if ( bUndamageable && !bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetInterface()->m_doorState = 7;
        else
            GetInterface()->m_doorState = 1;
    }
    else if ( !bUndamageable && bAreDoorsUndamageable )
    {
        if ( bAreDoorsLocked )
            GetInterface()->m_doorState = 2;
        else
            GetInterface()->m_doorState = 0;
    }
}

bool CVehicleSA::CanPedEnterCar() const
{
    DEBUG_TRACE("bool CVehicleSA::CanPedEnterCar() const");

    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_CanPedEnterCar;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

bool CVehicleSA::CanPedLeanOut( CPed *ped ) const
{
    DEBUG_TRACE("bool CVehicleSA::CanPedLeanOut( CPed *ped ) const");

    bool bReturn;
    CPedSA* pPedSA = dynamic_cast < CPedSA* > ( ped );
    DWORD dwThis = (DWORD)m_pInterface;
    CPedSAInterface* pPedInt = pPedSA->GetInterface();
    DWORD dwFunc = FUNC_CVehicle_CanPedLeanOut;

    _asm
    {
        mov     ecx, dwThis
        push    pPedInt
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

bool CVehicleSA::IsOnItsSide() const 
{
    DEBUG_TRACE("bool CVehicleSA::IsOnItsSide() const");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsOnItsSide;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CVehicleSA::IsLawEnforcementVehicle() const
{
    DEBUG_TRACE("bool CVehicleSA::IsLawEnforcementVehicle() const");
    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsLawEnforcementVehicle;
    bool bReturn = false;

    _asm
    {
        mov     ecx, dwThis
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

bool CVehicleSA::IsPassenger( CPed *ped ) const
{
    DEBUG_TRACE("bool CVehicleSA::IsPassenger( CPed *ped ) const");

    CPedSAInterface *pedInt = dynamic_cast <CPedSA*> ( ped )->GetInterface();
    unsigned int n = MAX_PASSENGERS;

    while ( n-- )
    {
        if ( GetInterface()->pPassengers[n] == pedInt )
            return true;
    }

    return false;
}

bool CVehicleSA::IsSphereTouchingVehicle( const CVector& vecOrigin, float fRadius ) const
{
    DEBUG_TRACE("bool CVehicleSA::IsSphereTouchingVehicle( CVector& vecOrigin, float fRadius ) const");

    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_IsSphereTouchingVehicle;
    bool bReturn = false;

    _asm
    {
        push    eax

        mov     ecx, dwThis
        mov     eax, vecOrigin
        push    fRadius
        push    dword ptr [eax]
        push    dword ptr [eax + 4]
        push    dword ptr [eax + 8]
        call    dwFunc
        mov     bReturn, al
    }
    return bReturn;
}

void CVehicleSA::MakeDirty( CColPoint *point )
{
    DEBUG_TRACE("void CVehicleSA::MakeDirty( CColPoint *point )");

    DWORD dwThis = (DWORD) m_pInterface;
    DWORD dwFunc = FUNC_CVehicle_MakeDirty;

    _asm
    {
        mov     ecx, dwThis
        push    point
        call    dwFunc
    }
}

void CVehicleSA::SetEngineOn( bool bEngineOn )
{
    DEBUG_TRACE("void CVehicleSA::SetEngineOn( bool bEngineOn )");

    if ( IsEngineBroken() )
    {
        GetInterface()->SetEngineOn( false );
        return;
    }

    GetInterface()->SetEngineOn( bEngineOn );
}

CPed* CVehicleSA::GetDriver() const
{
    DEBUG_TRACE("CPed* CVehicleSA::GetDriver() const");

    CPedSAInterface *driver = GetInterface()->pDriver;
    
    if ( !driver )
        return NULL;

    return pGame->GetPools()->GetPed( driver );
}

CPed* CVehicleSA::GetPassenger( unsigned char ucSlot ) const
{
    DEBUG_TRACE("CPed* CVehicleSA::GetPassenger( unsigned char ucSlot ) const");

    if ( ucSlot > 7 )
        return NULL;

    CPedSAInterface *ped = GetInterface()->pPassengers[ucSlot];

    if ( !ped )
        return NULL;

    return pGame->GetPools()->GetPed( ped );
}

void CVehicleSA::BlowUp( CEntity *creator, unsigned long unk )
{
    CEntitySA *entity = dynamic_cast <CEntitySA*> ( creator );

    if ( !entity )
        GetInterface()->Blow( NULL, unk );
    else
        GetInterface()->Blow( entity->GetInterface(), unk );
}

CVehicle* CVehicleSA::GetTowedVehicle() const
{
    DEBUG_TRACE("CVehicle* CVehicleSA::GetTowedVehicle() const");

    CVehicleSAInterface *towedVehicle = GetInterface()->m_towedVehicle;

    if ( !towedVehicle )
		return NULL;

    return pGame->GetPools()->GetVehicle( towedVehicle );
}

CVehicle* CVehicleSA::GetTowedByVehicle() const
{
    DEBUG_TRACE("CVehicle* CVehicleSA::GetTowedByVehicle() const");

    CVehicleSAInterface *tower = GetInterface()->m_towedByVehicle;

    if ( !tower )
        return NULL;

    return pGame->GetPools()->GetVehicle( tower );
}

void CVehicleSA::SetRemapTexDictionary( int txd )
{
    if ( txd == GetInterface()->m_paintjobTxd )
        return;

    if ( txd != -1 )
    {
        GetInterface()->m_queuePaintjob = txd;
        return;
    }

    GetInterface()->m_paintjobTexture = NULL;

    TextureManager::GetTxdPool()->Get( GetInterface()->m_paintjobTxd )->Dereference();

    GetInterface()->m_paintjobTxd = -1;
    GetInterface()->m_queuePaintjob = -1;
}

void CVehicleSA::SetRemap( int iRemap )
{
    if ( iRemap == -1 )
    {
        short paintjobId = GetInterface()->m_paintjobTxd;

        if ( paintjobId != -1 )
        {
            TextureManager::GetTxdPool()->Get( paintjobId )->Dereference();

            GetInterface()->m_paintjobTexture = NULL;

            GetInterface()->m_paintjobTxd = -1;
            GetInterface()->m_queuePaintjob = -1;
        }

        return;
    }
    else if ( iRemap >= 5 ) // Crashfix?
        return;

    SetRemapTexDictionary( GetInterface()->GetModelInfo()->paintjobTypes[iRemap] );
}

int CVehicleSA::GetRemapIndex() const
{
    const CVehicleModelInfoSAInterface *info = GetInterface()->GetModelInfo();

    unsigned short num = info->GetNumberOfValidPaintjobs();

    for ( unsigned short n = 0; n < num; n++ )
    {
        if ( (unsigned short)GetInterface()->m_paintjobTxd == info->paintjobTypes[n] )
            return n;
    }

    return -1;
}

void CVehicleSA::SetHandlingData( CHandlingEntry *handling )
{
    // Store the handling and recalculate it
    m_pHandlingData = (CHandlingEntrySA*)handling;

    GetInterface()->pHandlingData = m_pHandlingData->GetInterface();

    RecalculateHandling();
}

void CVehicleSA::RecalculateHandling()
{
    if ( !m_pHandlingData )
        return;

    m_pHandlingData->Recalculate();

    RecalculateSuspensionLines();
}

void GetMatrixForGravity( const CVector& vecGravity, RwMatrix& mat )
{
    // Calculates a basis where the z axis is the inverse of the gravity
    if ( vecGravity.Length() > 0.0001f )
    {
        mat.vUp = -vecGravity;
        mat.vUp.Normalize();

        if ( fabs( mat.vUp.fX ) > 0.0001f || fabs( mat.vUp.fZ ) > 0.0001f )
        {
            CVector y( 0.0f, 1.0f, 0.0f );
            mat.vFront = vecGravity;
            mat.vFront.CrossProduct( &y );
            mat.vFront.CrossProduct( &vecGravity );
            mat.vFront.Normalize();
        }
        else
            mat.vFront = CVector( 0.0f, 0.0f, vecGravity.fY );

        mat.vRight = mat.vFront;
        mat.vRight.CrossProduct( &mat.vUp );
    }
    else
    {
        // No gravity, use default axes
        mat.vRight  = CVector ( 1.0f, 0.0f, 0.0f );
        mat.vFront  = CVector ( 0.0f, 1.0f, 0.0f );
        mat.vUp     = CVector ( 0.0f, 0.0f, 1.0f );
    }
}

void CVehicleSA::SetGravity( const CVector& grav )
{
    if ( pGame->GetPlayerInfo()->GetPlayerPed()->GetVehicle() == this )
    {
        // If this is the local player's vehicle, adjust the camera's position history.
        // This is to keep the automatic camera settling (which happens when driving while not moving the mouse)
        // nice and consistent while the gravity changes.
        CCamSA *pCam = pGame->GetCamera()->GetCam( pGame->GetCamera()->GetActiveCam() );

        RwMatrix matOld, matNew;
        GetMatrixForGravity( m_vecGravity, matOld );
        GetMatrixForGravity( grav, matNew );

        // Make sure we have a matrix
        m_pInterface->AcquaintMatrix();
        
        const CVector& pos = m_pInterface->Placeable.matrix->vPos;

        matOld.Invert();
        pCam->GetTargetHistoryPos()[0] = matOld * (pCam->GetTargetHistoryPos()[0] - pos);
        pCam->GetTargetHistoryPos()[0] = matNew * pCam->GetTargetHistoryPos()[0] + pos;

        pCam->GetTargetHistoryPos()[1] = matOld * (pCam->GetTargetHistoryPos()[1] - pos);
        pCam->GetTargetHistoryPos()[1] = matNew * pCam->GetTargetHistoryPos()[1] + pos;
    }

    m_vecGravity = grav;
}

CColModelSA* CVehicleSA::GetSpecialColModel() const
{
    unsigned char colModel = GetInterface()->m_nSpecialColModel;

    if ( colModel == 0xFF )
        return NULL;

    return new CColModelSA( &((CColModelSAInterface*)VAR_CVehicle_SpecialColModels)[ GetInterface()->m_nSpecialColModel ] );
}

void* CVehicleSAInterface::operator new( size_t )
{
    return (*ppVehiclePool)->Allocate();
}

void CVehicleSAInterface::operator delete( void *ptr )
{
    (*ppVehiclePool)->Free( (CVehicleSAInterface*)ptr );
}
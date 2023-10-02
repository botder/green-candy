/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.cpp
*  PURPOSE:     Base entity
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Kent Simon <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGameSA *pGame;
extern CBaseModelInfoSAInterface **ppModelInfo;

CEntitySAInterface::CEntitySAInterface()
{
    nStatus = 4;
    m_entityFlags = ENTITY_VISIBLE | ENTITY_BACKFACECULL;

    m_nScanCode = 0;

    m_nModelIndex = -1;
    m_pRwObject = NULL;

    m_iplIndex = 0;
    m_areaCode = 0;

    m_randomSeed = rand();

    m_references = NULL;
    m_streamingRef = NULL;
    
    numLodChildren = 0;
    numLodChildrenRendered = 0;
    m_pLod = NULL;
}

CEntitySAInterface::~CEntitySAInterface( void )
{
}

// Implement some virtual stuff, so we can construct entities.
// The compiler complains otherwise.
// We are not going to use these functions anyway.
void __thiscall CEntitySAInterface::AddRect( CBounds2D bounds )                         {}
bool __thiscall CEntitySAInterface::AddToWorld( void )                                  { return false; }
void __thiscall CEntitySAInterface::RemoveFromWorld( void )                             {}
void __thiscall CEntitySAInterface::SetStatic( bool enabled )                           {}
void __thiscall CEntitySAInterface::SetModelIndex( modelId_t id )                       {}
void __thiscall CEntitySAInterface::SetModelIndexNoCreate( modelId_t id )               {}
void __thiscall CEntitySAInterface::CreateRwObject( void )                              {}
void __thiscall CEntitySAInterface::DeleteRwObject( void )                              {}
const CBounds2D& __thiscall CEntitySAInterface::GetBoundingBox( CBounds2D& bounds )     { return bounds; }
void __thiscall CEntitySAInterface::ProcessControl( void )                              {}
void __thiscall CEntitySAInterface::ProcessCollision( void )                            {}
void __thiscall CEntitySAInterface::ProcessShift( void )                                {}
bool __thiscall CEntitySAInterface::TestCollision( void )                               { return false; }
void __thiscall CEntitySAInterface::Teleport( float x, float y, float z, int unk )      {}
void __thiscall CEntitySAInterface::PreFrame( void )                                    {}
bool __thiscall CEntitySAInterface::Frame( void )                                       { return true; }
void __thiscall CEntitySAInterface::PreRender( void )                                   {}
void __thiscall CEntitySAInterface::Render( void )                                      {}
unsigned char __thiscall CEntitySAInterface::SetupLighting( void )                      { return 0; }
void __thiscall CEntitySAInterface::RemoveLighting( unsigned char id )                  {}
void __thiscall CEntitySAInterface::Invalidate( void )                                  {}


void CEntitySAInterface::GetPosition( CVector& pos ) const
{
    pos = Placeable.GetPosition();
}

float CEntitySAInterface::GetBasingDistance( void ) const
{
    return GetColModel()->m_bounds.vecBoundMin.fZ;
}

static bool RpMaterialSetAlpha( RpMaterial *mat, unsigned char alpha )
{
    mat->color.a = alpha;
    return true;
}

static int RpAtomicMaterialSetAlpha( RpAtomic *atom, unsigned char alpha )
{
    atom->geometry->ForAllMateria( RpMaterialSetAlpha, alpha );
    return true;
}

void CEntitySAInterface::SetAlpha( unsigned char alpha )
{
    RwObject *rwobj = GetRwObject();

    if ( !rwobj )
        return;

    if ( rwobj->type == RW_ATOMIC )
    {
        RpAtomicMaterialSetAlpha( (RpAtomic*)rwobj, alpha );
    }
    else if ( rwobj->type == RW_CLUMP )
    {
        ((RpClump*)rwobj)->ForAllAtomics( RpAtomicMaterialSetAlpha, alpha );
    }
}

CColModelSAInterface* CEntitySAInterface::GetColModel( void ) const
{
    CEntitySA *entity = (CEntitySA*)pGame->GetPools()->GetEntity( const_cast <CEntitySAInterface*> ( this ) );

    if ( entity )
    {
        CColModelSA *col = entity->GetColModel();

        if ( col )
            return col->GetInterface();
    }

    if ( nType == ENTITY_TYPE_VEHICLE )
    {
        CVehicleSAInterface *veh = (CVehicleSAInterface*)this;
        unsigned char n = veh->m_nSpecialColModel;

        if ( n != 0xFF )
            return (CColModelSAInterface*)VAR_CVehicle_SpecialColModels + n;
    }

    return GetModelInfo()->pColModel;
}

const CVector& CEntitySAInterface::GetCollisionOffset( CVector& out ) const
{
    GetOffset( out, GetColModel()->m_bounds.vecBoundOffset );
    return out;
}

const CBounds2D& CEntitySAInterface::_GetBoundingBox( CBounds2D& out ) const
{
    CColModelSAInterface *col = GetColModel();
    CVector pos;
    
    GetOffset( pos, col->m_bounds.vecBoundOffset );

    float radius = col->m_bounds.fRadius;

    out.m_minX = pos[0] - radius;
    out.m_maxY = pos[1] + radius;
    out.m_maxX = pos[0] + radius;
    out.m_minY = pos[1] - radius;
    return out;
}

void __thiscall CEntitySAInterface::GetCenterPoint( CVector& out ) const
{
    CColModelSAInterface *col = GetColModel();

    GetOffset( out, col->m_bounds.vecBoundOffset );
}

void __thiscall CEntitySAInterface::SetOrientation( float x, float y, float z )
{
    Placeable.SetRotation( x, y, z );
}

bool CEntitySAInterface::IsOnScreen( void ) const
{
    // Bugfix: no col -> not visible
    if ( GetColModel() )
    {
        CVector pos;

        GetCollisionOffset( pos );

        CCameraSAInterface& camera = Camera::GetInterface();

        // MTA fix: get the real entity radius.
        float entityRadius = GetRadius();

        bool isVisible = camera.IsSphereVisible( pos, entityRadius, camera.m_matInverse );

        if ( isVisible )
        {
            return true;
        }

        // Are mirrors enabled?
        if ( *(unsigned char*)0x00B6F998 )
        {
            // Check if we are visible on the mirror.
            isVisible = camera.IsSphereVisible( pos, entityRadius, camera.m_matMirrorInverse );
            
            if ( isVisible )
            {
                return true;
            }
        }
    }

    return false;
}

bool __thiscall CEntitySAInterface::CheckScreenValidity( void ) const
{
    bool retVal;

    __asm
    {
        mov eax,0x0071FAE0
        call eax
        mov retVal,al
    }

    return retVal;
}

void CEntitySAInterface::UpdateRwMatrix( void )
{
    if ( !GetRwObject() )
        return;

    Placeable.GetMatrix( GetRwObject()->parent->modelling );
}

void CEntitySAInterface::UpdateRwFrame( void )
{
    if ( !GetRwObject() )
        return;

    GetRwObject()->parent->Update();
}

// Binary offsets: (1.0 US and 1.0 EU): 0x00407000
bool __thiscall CEntitySAInterface::IsInStreamingArea( void ) const
{
    return Streaming::IsValidStreamingArea( m_areaCode );
}

// Entity referencing system.
// Should prevent entities that are marked by the system to be destroyed in crucial areas.
// Otherwise the system will crash.
static entityReferenceCallback_t _entityAddRef = NULL;
static entityReferenceCallback_t _entityRemoveRef = NULL;

bool CEntitySAInterface::Reference( void )
{
    if ( _entityAddRef )
    {
        CEntitySA *mtaEntity = Pools::GetEntity( this );

        if ( mtaEntity )
        {
            return _entityAddRef( mtaEntity );
        }
    }

    return false;
}

void CEntitySAInterface::Dereference( void )
{
    if ( _entityRemoveRef )
    {
        CEntitySA *mtaEntity = Pools::GetEntity( this );

        if ( mtaEntity )
            _entityRemoveRef( mtaEntity );
    }
}

void Entity::SetReferenceCallbacks( entityReferenceCallback_t addRef, entityReferenceCallback_t delRef )
{
    _entityAddRef = addRef;
    _entityRemoveRef = delRef;
}

void Entity_Init( void )
{
    HookInstall( 0x00535300, h_memFunc( &CEntitySAInterface::GetColModel ), 5 );
    HookInstall( 0x00534540, h_memFunc( &CEntitySAInterface::IsOnScreen ), 5 );
    HookInstall( 0x00534250, h_memFunc( &CEntitySAInterface::GetCollisionOffset ), 5 );
    HookInstall( 0x005449B0, h_memFunc( &CEntitySAInterface::_GetBoundingBox ), 5 );
    HookInstall( 0x00534290, h_memFunc( &CEntitySAInterface::GetCenterPoint ), 5 );
    HookInstall( 0x00446F90, h_memFunc( &CEntitySAInterface::UpdateRwMatrix ), 5 );
    HookInstall( 0x00532B00, h_memFunc( &CEntitySAInterface::UpdateRwFrame ), 5 );
    HookInstall( 0x00536BE0, h_memFunc( &CEntitySAInterface::GetBasingDistance ), 5 );

    EntityRender_Init();
}

void Entity_Shutdown( void )
{
    EntityRender_Shutdown();
}

CEntitySA::CEntitySA()
{
    // Set these variables to a constant state
    m_pInterface = NULL;
    m_doNotRemoveFromGame = false;
    m_pStoredPointer = NULL;
    m_colModel = NULL;
}

CEntitySA::~CEntitySA()
{
    // Unlink disabled-collisions
    while ( !m_disabledColl.empty() )
        SetCollidableWith( pGame->GetPools()->GetEntity( m_disabledColl.begin()->first ), true );

    if ( m_doNotRemoveFromGame )
        return;

    CWorldSA *world = pGame->GetWorld();
    world->Remove( m_pInterface );
    world->RemoveReferencesToDeletedObject( m_pInterface );

    // Clever little trick, eh
    delete m_pInterface;
}

void CEntitySA::SetPosition( float x, float y, float z )
{
    DEBUG_TRACE("void CEntitySA::SetPosition( float x, float y, float z )");

    GetInterface()->Placeable.SetPosition( x, y, z );
}

void CEntitySA::SetPosition( const CVector& pos )
{
    GetInterface()->Placeable.SetPosition( pos );
}

void CEntitySA::GetPosition( CVector& pos ) const
{
    DEBUG_TRACE("void CEntitySA::GetPosition( CVector& pos ) const");

    pos = GetInterface()->Placeable.GetPosition();
}

void CEntitySA::Teleport( float x, float y, float z )
{
    DEBUG_TRACE("void CEntitySA::Teleport( float x, float y, float z )");

    // Make sure we have a matrix
    m_pInterface->AcquaintMatrix();

    m_pInterface->Teleport( x, y, z, false );
}

void CEntitySA::ProcessControl()
{
    DEBUG_TRACE("void CEntitySA::ProcessControl()");
    
    m_pInterface->ProcessControl();
}

void CEntitySA::SetupLighting()
{
    DEBUG_TRACE("void CEntitySA::SetupLighting()");
    
    m_pInterface->SetupLighting();
}

void CEntitySA::SetOrientation( float x, float y, float z )
{
    DEBUG_TRACE("void CEntitySA::SetOrientation( float x, float y, float z )");

    pGame->GetWorld()->Remove( this );

    CEntitySAInterface *intf = m_pInterface;
    DWORD dwFunc = FUNC_SetOrientation;
    _asm
    {
        // ChrML: I've switched the X and Z at this level because that's how the real rotation
        //        is. GTA has kinda swapped them in this function.

        push    z
        push    y
        push    x
        mov     ecx,intf
        call    dwFunc
    }

    // Update RenderWare specifications
    intf->UpdateRwMatrix();
    intf->UpdateRwFrame();

    pGame->GetWorld()->Add( this );
}

void CEntitySA::FixBoatOrientation()
{
    DEBUG_TRACE("void CEntitySA::FixBoatOrientation()");

    pGame->GetWorld()->Remove ( this );

    // Update RenderWare specifications
    CEntitySAInterface *intf = GetInterface();
    intf->UpdateRwMatrix();
    intf->UpdateRwFrame();

    pGame->GetWorld()->Add ( this );
}

void CEntitySA::GetMatrix( RwMatrix& mat ) const
{
    DEBUG_TRACE("void CEntitySA::GetMatrix( RwMatrix& mat ) const");

    GetInterface()->Placeable.GetMatrix( mat );
}

void CEntitySA::SetMatrix( const RwMatrix& mat )
{
    DEBUG_TRACE("void CEntitySA::SetMatrix( const RwMatrix& mat )");

    // Make sure we keep this matrix (static allocation)
    m_pInterface->AllocateMatrix();
    m_pInterface->Placeable.matrix->assign( mat );

    pGame->GetWorld()->Remove( this );

    // Update RenderWare specifications
    CEntitySAInterface *intf = GetInterface();
    intf->UpdateRwMatrix();
    intf->UpdateRwFrame();

    pGame->GetWorld()->Add( this );
}

unsigned short CEntitySA::GetModelIndex() const
{
    DEBUG_TRACE("unsigned short CEntitySA::GetModelIndex() const");
    return m_pInterface->GetModelIndex();
}

eEntityType CEntitySA::GetEntityType() const
{
    DEBUG_TRACE("eEntityType CEntitySA::GetEntityType() const");
    return (eEntityType)m_pInterface->nType;
}

float CEntitySA::GetBasingDistance() const
{
    DEBUG_TRACE("float CEntitySA::GetBasingDistance() const");
    
    return GetInterface()->GetBasingDistance();
}   

void CEntitySA::SetEntityStatus( eEntityStatus bStatus )
{
    DEBUG_TRACE("void CEntitySA::SetEntityStatus( eEntityStatus bStatus )");
    m_pInterface->nStatus = bStatus;
}

eEntityStatus CEntitySA::GetEntityStatus() const
{
    DEBUG_TRACE("eEntityStatus CEntitySA::GetEntityStatus() const");
    return (eEntityStatus) m_pInterface->nStatus;
}

void CEntitySA::ReplaceTexture( const char *name, CTexture *tex )
{
    // 
}

void CEntitySA::RestoreTexture( const char *name )
{

}

void CEntitySA::SetAlpha( unsigned char alpha )
{
    DEBUG_TRACE("void CEntitySA::SetAlpha( unsigned char alpha )");

    GetInterface()->SetAlpha( alpha );
}

float CEntitySA::GetFadingAlpha( void ) const
{
    return GetInterface()->GetFadingAlpha();
}

bool CEntitySA::IsOnScreen() const
{
    return GetInterface()->IsOnScreen();
}

bool CEntitySA::IsCollidableWith( CEntity *entity ) const
{
    return !MapContains( m_disabledColl, dynamic_cast <CEntitySA*> ( entity )->GetInterface() );
}

void CEntitySA::SetCollidableWith( CEntity *entity, bool enabled )
{
    CEntitySA *intEnt = dynamic_cast <CEntitySA*> ( entity );

    if ( intEnt == this )
        return;

    // quit if no change
    if ( MapContains( m_disabledColl, intEnt->GetInterface() ) != enabled )
        return;

    if ( enabled )
    {
        MapRemove( m_disabledColl, intEnt->GetInterface() );

        if ( m_disabledColl.empty() )
            MapRemove( pGame->m_disabledColl, this );
    }
    else
    {
        MapSet( m_disabledColl, intEnt->GetInterface(), true );
        MapSet( pGame->m_disabledColl, this, true );
    }

    // Set in the other entity as well
    intEnt->SetCollidableWith( this, enabled );
}

unsigned char CEntitySA::GetAreaCode() const
{
    return m_pInterface->m_areaCode;
}

void CEntitySA::SetAreaCode( unsigned char area )
{
    m_pInterface->m_areaCode = area;
}
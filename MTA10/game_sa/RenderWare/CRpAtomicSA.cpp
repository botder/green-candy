/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CRpAtomicSA.cpp
*  PURPOSE:     RenderWare RpAtomic export
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../gamesa_renderware.h"

extern CBaseModelInfoSAInterface **ppModelInfo;

CRpAtomicSA::CRpAtomicSA( RpAtomic *atom ) : CRwObjectSA( atom )
{
    atom->geometry->flags |= RW_GEOMETRY_GLOBALLIGHT;
    atom->scene = *p_gtaScene;
    m_model = NULL;

    _initAtomicNormals( atom );
}

CRpAtomicSA::~CRpAtomicSA( void )
{
    // Restore all models
    RestoreAll();

    // Detach from the clump and destroy
    RemoveFromModel();

    // Detach from the frame, too
    SetFrame( NULL );

    RpAtomicDestroy( GetObject() );
}

CRpAtomic* CRpAtomicSA::Clone( void ) const
{
    return new CRpAtomicSA( RpAtomicClone( GetObject() ) );
}

RpAtomic* CRpAtomicSA::CreateInstance( unsigned short id ) const
{
    RpAtomic *atom = RpAtomicClone( GetObject() );

    atom->SetRenderCallback( NULL );

    RpAtomicSetFrame( atom, RwFrameCreate() );

    RpAtomicSetModelIndex( atom, id );
    return atom;
}

void CRpAtomicSA::Render( void )
{
    if ( !m_frame || !RenderWare::GetInterface()->m_renderCam )
        return;

    RpAtomic *atom = GetObject();

    atom->renderCallback( atom );
}

void CRpAtomicSA::GetWorldSphere( RwSphere& out )
{
    if ( !m_frame )
        return;

    out = GetObject()->GetWorldBoundingSphere();
}

void CRpAtomicSA::AddToModel( CModel *model )
{
    RemoveFromModel();

    m_model = dynamic_cast <CModelSA*> ( model );

    if ( !m_model )
        return;

    m_model->m_atomics.push_back( this );
    GetObject()->AddToClump( m_model->GetObject() );
}

CModel* CRpAtomicSA::GetModel( void )
{
    return m_model;
}

void CRpAtomicSA::RemoveFromModel( void )
{
    if ( !m_model )
        return;

    GetObject()->RemoveFromClump();

    m_model->m_atomics.remove( this );
    m_model = NULL;
}

bool CRpAtomicSA::Replace( unsigned short id )
{
    CAtomicModelInfoSA *info = (CAtomicModelInfoSA*)ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    if ( !info )
        return false;

    // Make sure we are compatible with the model info.
    if ( !info->ValidateResource( id, GetObject() ) )
        return false;

    if ( IsReplaced( id ) )
        return true;

    // Remove previous references to this model to prevent collision
    pGame->GetModelManager()->RestoreModel( id );

    // We want to fence the loading process to get accurate data.
    Streaming::FenceLoading();

    CAtomicModelInfoSA *ainfo = (CAtomicModelInfoSA*)info;

    bool isLoaded = ainfo->GetRwObject() != NULL;
    
    // Remove any active model (either loading or loaded)
    Streaming::FreeModel( id );

    g_replObjectNative[id] = this;
    m_imported.push_back( id );

    // Reinstantiate ourselves
    if ( isLoaded )
        Streaming::RequestModel( id, 0x16 );

    return true;
}

bool CRpAtomicSA::IsReplaced( unsigned short id ) const
{
    return std::find( m_imported.begin(), m_imported.end(), id ) != m_imported.end();
}

bool CRpAtomicSA::Restore( unsigned short id )
{
    imports_t::iterator iter = std::find( m_imported.begin(), m_imported.end(), id );

    if ( iter == m_imported.end() )
        return false;

    CAtomicModelInfoSA *info = (CAtomicModelInfoSA*)ppModelInfo[id];

    if ( !info || info->GetRwModelType() != RW_ATOMIC )
    {
        // This situation should never happen, if it does, terminate our bases!
        m_imported.erase( iter );
        return false;
    }

    // We want to fence the loading process to get accurate data.
    Streaming::FenceLoading();

    CTxdInstanceSA *txd;
    bool isLoaded = info->GetRwObject() != NULL;

    if ( isLoaded )
    {
        // Keep the textures alive
        txd = TextureManager::GetTxdPool()->Get( info->usTextureDictionary );
        txd->Reference();
    }

    if ( Streaming::GetModelLoadInfo( id ).m_eLoading == MODEL_UNAVAILABLE )
    {
        assert( !isLoaded );
    }

    // Force a kill
    Streaming::FreeModel( id );

    g_replObjectNative[id] = NULL;

    // Restore if loaded
    if ( isLoaded )
    {
        // Remove the reference again
        txd->Dereference();

        Streaming::RequestModel( id, 0x10 );
    }

    m_imported.erase( iter );
    return true;
}

void CRpAtomicSA::RestoreAll( void )
{
    while ( !m_imported.empty() )
        Restore( m_imported[0] );
}
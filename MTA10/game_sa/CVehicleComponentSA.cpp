/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CVehicleComponentSA.cpp
*  PURPOSE:     Vehicle component part interface
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

CVehicleComponentSA::CVehicleComponentSA( vehComponents_t& container, RpClump *clump, RwFrame *item ) : m_container( container )
{
    m_clump = clump;
    m_frame = item;

    container[ item->szName ] = this;
}

static int RwObjectDestroyAll( RwObject *obj, int )
{
    if ( obj->type == RW_ATOMIC )
    {
        ((RpAtomic*)obj)->RemoveFromClump();
        RpAtomicDestroy( (RpAtomic*)obj );
    }

    return true;
}

CVehicleComponentSA::~CVehicleComponentSA()
{
    // Clear ourselves from the container
    m_container.erase( GetName() );

    // Clear all objects
    m_frame->ForAllObjects( RwObjectDestroyAll, 0 );
    LIST_CLEAR( m_frame->objects.root );
}

const char* CVehicleComponentSA::GetName() const
{
    return m_frame->szName;
}

void CVehicleComponentSA::SetMatrix( const RwMatrix& mat )
{
    m_frame->SetModelling( mat );
}

const RwMatrix& CVehicleComponentSA::GetMatrix() const
{
    return m_frame->GetModelling();
}

const RwMatrix& CVehicleComponentSA::GetWorldMatrix() const
{
    return m_frame->GetLTM();
}

void CVehicleComponentSA::SetPosition( const CVector& pos )
{
    m_frame->SetPosition( pos );
}

const CVector& CVehicleComponentSA::GetPosition() const
{
    return m_frame->GetPosition();
}

const CVector& CVehicleComponentSA::GetWorldPosition() const
{
    return m_frame->GetLTM().vPos;
}

static int RwObjectSetActive( RwObject *obj, bool visible )
{
    obj->SetVisible( visible );
    return true;
}

void CVehicleComponentSA::SetActive( bool active )
{
    m_frame->ForAllObjects( RwObjectSetActive, active );
}

bool CVehicleComponentSA::IsActive() const
{
    RwObject *obj;

    return ( obj = m_frame->GetFirstObject( RW_ATOMIC ) ) && obj->IsVisible();
}

// Add an atomic to this component
// As long as the atomic is being used, the origin has to stay intact
unsigned int CVehicleComponentSA::AddAtomic( CRpAtomic *atomic )
{
    RpAtomic *clone = RpAtomicClone( dynamic_cast <CRpAtomicSA*> ( atomic )->GetObject() );
    
    clone->AddToFrame( m_frame );
    clone->AddToClump( m_clump );
    return 0;
}

CRpAtomic* CVehicleComponentSA::CloneAtomic( unsigned int idx ) const
{
    RpAtomic *clAtom = (RpAtomic*)m_frame->GetObjectByIndex( RW_ATOMIC, idx );

    if ( !clAtom )
        return NULL;

    RpAtomic *inst = RpAtomicClone( clAtom );

    // Copy important credentials
    RwFrame *frame = RwFrameCreate();
    memcpy( frame->szName, m_frame->szName, 0x10 );

    RpAtomicSetFrame( inst, frame );

    CRpAtomicSA *atom = new CRpAtomicSA( inst );
    atom->SetFrame( new CRwFrameSA( frame ) );    // Assign a frame to it
    return atom;
}

unsigned int CVehicleComponentSA::GetAtomicCount() const
{
    return m_frame->CountObjectsByType( RW_ATOMIC );
}

bool CVehicleComponentSA::RemoveAtomic( unsigned int idx )
{
    RpAtomic *atom = (RpAtomic*)m_frame->GetObjectByIndex( RW_ATOMIC, idx );

    if ( !atom )
        return false;

    atom->RemoveFromClump();
    atom->RemoveFromFrame();

    RpAtomicDestroy( atom );
    return true;
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientCheckpoint.cpp
*  PURPOSE:     Checkpoint marker entity class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CClientCheckpoint::CClientCheckpoint( CClientMarker *pThis )
{
    CClientEntityRefManager::AddEntityRefs( ENTITY_REF_DEBUG( this, "CClientCheckpoint" ), &m_pThis, NULL );

    // Init
    m_pThis = pThis;
    m_checkpoint = NULL;
    m_streamedIn = false;
    m_visible = true;
    m_icon = ICON_NONE;
    m_color = SColorRGBA( 255, 0, 0, 255 );
    m_size = 4.0f;
    m_type = CHECKPOINT_EMPTYTUBE;
    m_dir[0] = 1.0f;
    m_hasTarget = false;
}

CClientCheckpoint::~CClientCheckpoint()
{
    // Eventually destroy the checkpoint
    Destroy();

    CClientEntityRefManager::RemoveEntityRefs( 0, &m_pThis, NULL );
}

unsigned long CClientCheckpoint::GetCheckpointType() const
{
    switch( m_type )
    {
    case CHECKPOINT_TUBE:
    case CHECKPOINT_EMPTYTUBE:
    case CHECKPOINT_ENDTUBE:
        return TYPE_NORMAL;
    case CHECKPOINT_TORUS:
        return TYPE_RING;
    }

    return TYPE_INVALID;
}

void CClientCheckpoint::SetCheckpointType( unsigned long ulType )
{
    // Set the type
    unsigned long ulNewType;

    switch( ulType )
    {
    case CClientCheckpoint::TYPE_NORMAL:
    {
        // Set the correct type depending on the icon we want inside
        if ( m_icon == ICON_ARROW )
            ulNewType = CHECKPOINT_TUBE;
        else if ( m_icon == ICON_FINISH )
            ulNewType = CHECKPOINT_ENDTUBE;
        else
            ulNewType = CHECKPOINT_EMPTYTUBE;

        break;
    }
    case CClientCheckpoint::TYPE_RING:
    {
        ulNewType = CHECKPOINT_TORUS;
        break;
    }
    default:
    {
        // Set the correct type depending on the icon we want inside
        if ( m_icon == ICON_ARROW )
            ulNewType = CHECKPOINT_TUBE;
        else if ( m_icon == ICON_FINISH )
            ulNewType = CHECKPOINT_ENDTUBE;
        else
            ulNewType = CHECKPOINT_EMPTYTUBE;

        break;
    }
    }

    // Different from our current type
    if ( m_type != ulNewType )
    {
        // Set it
        m_type = ulNewType;

        // Recreate it
        ReCreate();
    }
}

bool CClientCheckpoint::IsHit( const CVector& vecPosition ) const
{
    // Grab the type and do a 2D or 3D distance check depending on what type it is
    unsigned long ulType = GetCheckpointType();

    if ( ulType == TYPE_NORMAL )
        return IsPointNearPoint2D( m_Matrix.vPos, vecPosition, m_size + 4 );
    else
        return IsPointNearPoint3D( m_Matrix.vPos, vecPosition, m_size + 4 );
}

void CClientCheckpoint::SetPosition( const CVector& pos )
{ 
    // Different from our current position?
    if ( m_Matrix.vPos == pos )
        return;

    // Recalculate the stored target so that its accurate even if we move it
    if ( m_targetPosition.fX != 0 && m_targetPosition.fY != 0 && m_targetPosition.fZ != 0 )
    {
        m_dir = m_targetPosition - pos;
        m_dir.Normalize();
    }

    // Store the new position, recreate and tell the streamer about the new position
    m_Matrix.vPos = pos;
    ReCreateWithSameIdentifier();
}

void CClientCheckpoint::SetDirection( const CVector& dir )
{
    // Different target position than already?
    if ( m_dir == dir )
        return;

    // Set the direction and reset the target position
    m_targetPosition = CVector( 0.0f, 0.0f, 0.0f );
    m_dir = dir;

    // Recreate
    if ( GetCheckpointType() == TYPE_RING || m_icon == ICON_ARROW )
        ReCreate();
}

void CClientCheckpoint::GetMatrix( RwMatrix& mat ) const
{
    mat = m_Matrix;
}

void CClientCheckpoint::SetMatrix( const RwMatrix& mat )
{
    SetPosition( mat.vPos );
    m_Matrix = mat;
}

void CClientCheckpoint::SetNextPosition( const CVector& pos )
{
    // Different target position than already?
    if ( m_targetPosition == pos )
        return;

    // Set the new target position and direction
    m_targetPosition = pos;
    m_dir = m_targetPosition - m_Matrix.vPos;
    m_dir.Normalize ();

    // Recreate
    if ( GetCheckpointType() == TYPE_RING || m_icon == ICON_ARROW )
        ReCreate();
}

void CClientCheckpoint::SetVisible( bool bVisible )
{
    // If we're streamed in, create/destroy as we're getting visible/invisible
    if ( IsStreamedIn() )
    {
        if ( !bVisible && m_visible )
            Destroy();
        else if ( bVisible && !m_visible )
            Create();
    }

    // Set the new visible state
    m_visible = bVisible;
}

void CClientCheckpoint::SetIcon( unsigned int icon )
{
    // Different from our current icon?
    if ( m_icon == icon )
        return;

    // Set the new icon
    m_icon = icon;

    // Normal checkpoint?
    if ( GetCheckpointType() != TYPE_NORMAL )
        return;

    // Update the type
    unsigned long ulNewType;

    switch( icon )
    {
    case ICON_NONE:
        ulNewType = CHECKPOINT_EMPTYTUBE;
        break;
    case ICON_ARROW:
        ulNewType = CHECKPOINT_TUBE;
        break;
    case ICON_FINISH:
        ulNewType = CHECKPOINT_ENDTUBE;
        break;
    default:
        ulNewType = CHECKPOINT_EMPTYTUBE;
        break;
    }

    // Different from our previous type?
    if ( ulNewType != m_type )
    {
        // Set the new type and recreate the checkpoint
        m_type = ulNewType;
        ReCreate();
    }
}

void CClientCheckpoint::SetColor( const SColor color )
{
    // Different from our current color?
    if ( m_color != color )
    {
        // Set it and recreate
        m_color = color;
        ReCreate();
    }
}

void CClientCheckpoint::SetSize( float size )
{
    // Different from our current size
    if ( m_size != size )
    {
        // Set the new size and recreate
        m_size = size;
        ReCreate();
    }
}

unsigned char CClientCheckpoint::StringToIcon( const char *str )
{
    if ( stricmp( str, "none" ) == 0 )
        return ICON_NONE;
    else if ( stricmp( str, "arrow" ) == 0 )
        return ICON_ARROW;
    else if ( stricmp( str, "finish" ) == 0 )
        return ICON_FINISH;

    return ICON_INVALID;
}

bool CClientCheckpoint::IconToString( unsigned char ucIcon, char *str )
{
    switch( ucIcon )
    {
    case ICON_NONE:
        strcpy( str, "none" );
        return true;
    case ICON_ARROW:
        strcpy( str, "arrow" );
        return true;
    case ICON_FINISH:
        strcpy( str, "finish" );
        return true;
    }

    strcpy( str, "invalid" );
    return false;
}

void CClientCheckpoint::StreamIn()
{
    // We're now streamed in
    m_streamedIn = true;

    // Create us if we're visible
    if ( m_visible )
        Create();
}

void CClientCheckpoint::StreamOut()
{
    // Destroy our checkpoint
    Destroy();

    // No longer streamed in
    m_streamedIn = false;
}

void CClientCheckpoint::Create( unsigned int id )
{
    // If the item already exists, don't create it
    if ( m_checkpoint )
        return;

    // Generate an identifier (TODO: Move identifier stuff to game layer)
    static unsigned int s_ulIdentifier = 128;

    if ( id == 0 )
    {
        s_ulIdentifier++;
        m_id = s_ulIdentifier;
    }
    else
    {
        m_id = id;
    }

    // Create it
    m_checkpoint = g_pGame->GetCheckpoints()->CreateCheckpoint( m_id, m_type, m_Matrix.vPos, m_dir, m_size, 0.2f, m_color );

    if ( m_checkpoint )
    {
        // Set properties
        m_checkpoint->SetRotateRate( 0 );
    }
}

void CClientCheckpoint::Destroy()
{
    // Destroy it
    if ( m_checkpoint )
    {
        m_checkpoint->Remove();
        m_checkpoint = NULL;
    }
}

void CClientCheckpoint::ReCreate()
{
    // Recreate if we're streamed in and visible
    if ( IsStreamedIn() && m_visible )
    {
        Destroy();
        Create();
    }
}

void CClientCheckpoint::ReCreateWithSameIdentifier()
{
    // Recreate if we're streamed in and visible
    if ( IsStreamedIn() && m_visible )
    {
        Destroy();
        Create( m_id );
    }
}

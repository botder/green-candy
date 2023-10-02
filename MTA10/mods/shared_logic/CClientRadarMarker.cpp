/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarMarker.cpp
*  PURPOSE:     Radar marker entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCoreInterface *g_pCore;

static const luaL_Reg blip_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_blip( lua_State *L )
{
    CClientRadarMarker *blip = (CClientRadarMarker*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RADARMARKER, blip );

    j.RegisterInterfaceTrans( L, blip_interface, 0, LUACLASS_RADARMARKER );

    lua_pushlstring( L, "blip", 4 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientRadarMarker::CClientRadarMarker( CClientManager* pManager, ElementID ID, lua_State *L, bool system, short sOrdering, unsigned short usVisibleDistance ) : CClientEntity( ID, system, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_blip, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Init
    m_pManager = pManager;
    m_pRadarMarkerManager = pManager->GetRadarMarkerManager();

    SetTypeName( "blip" );

    m_bIsVisible = true;
    m_pMarker = NULL;
    m_usScale = 2;
    m_ulSprite = 0;
    m_Color = SColorRGBA ( 0, 0, 255, 255 );
    m_sOrdering = sOrdering;
    m_usVisibleDistance = usVisibleDistance;

    // Add us to the radar marker manager list
    m_pRadarMarkerManager->AddToList( this );

    // Try to create it
    Create();
}

CClientRadarMarker::~CClientRadarMarker()
{
    // Try to destroy it
    Destroy();

    // Remove us from the marker manager
    Unlink();
}

void CClientRadarMarker::Unlink()
{
    m_pRadarMarkerManager->RemoveFromList( this );
}

void CClientRadarMarker::DoPulse()
{    
    // Are we attached to something?
    if ( m_pAttachedToEntity )
    {
        CVector vecPosition;
        m_pAttachedToEntity->GetPosition( vecPosition );        
        vecPosition += m_vecAttachedPosition;
        SetPosition( vecPosition );

        // TODO: move to ::SetElementDimension
        if ( m_usDimension != m_pAttachedToEntity->GetDimension() )
        {
            SetDimension( m_pAttachedToEntity->GetDimension() );
        }
    }

    // Are we in a visible distance?
    if ( IsInVisibleDistance() )
    {
        // Are we not created?
        if ( !m_pMarker )
        {
            // Are we visible and in the right dimension?
            if ( m_bIsVisible && m_usDimension == m_pManager->GetRadarMarkerManager()->GetDimension() )
            {
                // Recreate our marker
                CreateMarker();
            }
        }
    }
    else
    {
        // Are we created?
        if ( m_pMarker )
        {
            // Destroy our marker
            DestroyMarker();
        }
    }
}

void CClientRadarMarker::SetPosition( const CVector& vecPosition )
{
    if ( m_pMarker )
        m_pMarker->SetPosition( vecPosition );

    m_vecPosition = vecPosition;
}

void CClientRadarMarker::GetPosition( CVector& vecPosition ) const
{
    if ( m_pMarker )
    {
        vecPosition = m_pMarker->GetPosition();
    }
    else
    {
        vecPosition = m_vecPosition;
    }
}

void CClientRadarMarker::SetScale( unsigned short usScale )
{
    m_usScale = usScale;

    if ( m_pMarker )
    {
        m_pMarker->SetScale( usScale );
    }
}

void CClientRadarMarker::SetColor( const SColor color )
{
    // Store the color
    m_Color = color;

    // Set the color
    if ( m_pMarker )
    {
        // Supposed to be black? Make it almost black as entirely black
        // might make us hit some reserved colors. Which'd be unexpected.
        if ( m_Color.R == 0 && m_Color.G == 0 && m_Color.B == 0 )
        {
            m_pMarker->SetColor( SColorRGBA ( 1, 1, 1, m_Color.A ) );
        }
        else
        {
            m_pMarker->SetColor( m_Color );
        }
    }
}

void CClientRadarMarker::SetSprite( unsigned long ulSprite )
{
    m_ulSprite = ulSprite;

    if ( m_pMarker )
    {
        m_pMarker->SetSprite( (eMarkerSprite)ulSprite );
    }
}

void CClientRadarMarker::SetVisible( bool bVisible )
{
    // Are we in the right dimension?
    if ( m_usDimension == m_pManager->GetRadarMarkerManager()->GetDimension() )
    {
        // Visible now but weren't last time?
        if ( bVisible && !m_bIsVisible )
        {
            Create();
        }

        // Invisible now but weren't last time? Destroy it
        else if ( !bVisible && m_bIsVisible )
        {
            Destroy();
        }
    }

    // Update visibility
    m_bIsVisible = bVisible;
}

bool CClientRadarMarker::Create()
{
    m_pManager->GetRadarMarkerManager()->m_bOrderOnPulse = true;

    return true;
}

void CClientRadarMarker::InternalCreate()
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        CreateMarker ();

        // Create the marker on the full size map
        SetSprite ( m_ulSprite );
    }
}

void CClientRadarMarker::Destroy()
{
    DestroyMarker();
}

void CClientRadarMarker::CreateMarker()
{
    // Not already got a marker?
    if ( !m_pMarker )
    {
        if ( IsInVisibleDistance() )
        {
            // Create the marker
            m_pMarker = g_pGame->GetRadar()->CreateMarker( m_vecPosition );
            if ( m_pMarker )
            {
                SColor color = m_Color;

                // Supposed to be black? Make it almost black as entirely black
                // might make us hit some reserved colors. Which'd be unexpected.
                if ( color.R == 0 && color.G == 0 && color.B == 0 )
                {
                    color = SColorRGBA( 1, 1, 1, color.A );
                }

                // Set the properties
                m_pMarker->SetPosition( m_vecPosition );
                m_pMarker->SetScale( m_usScale );
                m_pMarker->SetColor( color );
                m_pMarker->SetSprite( (eMarkerSprite)m_ulSprite );
            }
        }
    }
}

void CClientRadarMarker::DestroyMarker()
{
    if ( m_pMarker )
    {
        m_pMarker->Remove();
        m_pMarker = NULL;
    }
}

void CClientRadarMarker::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pManager->GetRadarMarkerManager ()->GetDimension () );
}

void CClientRadarMarker::RelateDimension( unsigned short usDimension )
{
    // Are we visible?
    if ( m_bIsVisible )
    {
        if ( usDimension == m_usDimension )
        {
            Create();
        }
        else
        {
            Destroy();
        }
    }
}

void CClientRadarMarker::SetOrdering( short sOrdering )
{
    if ( sOrdering != m_sOrdering )
    {
        m_sOrdering = sOrdering;
        m_pManager->GetRadarMarkerManager()->m_bOrderOnPulse = true;
    }
}

bool CClientRadarMarker::IsInVisibleDistance() const
{
    float fDistance = DistanceBetweenPoints3D ( m_vecPosition, m_pRadarMarkerManager->m_vecCameraPosition );
    return ( fDistance <= m_usVisibleDistance );
}

void CClientRadarMarker::GetSquareTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_Color.A, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_Color.A, m_Color.R, m_Color.G, m_Color.B );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*1*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*2*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*3*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*4*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*5*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*6*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*7*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*8*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*9*/   dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*10*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*11*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*12*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*13*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*14*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*15*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*16*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*17*/  dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB,
/*18*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*19*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}


void CClientRadarMarker::GetUpTriangleTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_Color.A, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_Color.A, m_Color.R, m_Color.G, m_Color.B );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*1*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*2*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*3*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*4*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*5*/   dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*6*/   dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*7*/   dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*8*/   dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA,
/*9*/   dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA,
/*10*/  dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA,
/*11*/  dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA,
/*12*/  dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA,
/*13*/  dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA,
/*14*/  dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA,
/*15*/  dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA,
/*16*/  dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA,
/*17*/  dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA,
/*18*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*19*/  dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}


void CClientRadarMarker::GetDownTriangleTexture ( DWORD dwBitMap[] )
{
    DWORD dwA = COLOR_ARGB ( 0, 0, 0, 0 );
    DWORD dwB = COLOR_ARGB ( m_Color.A, 0, 0, 0 );
    DWORD dwC = COLOR_ARGB ( m_Color.A, m_Color.R, m_Color.G, m_Color.B );

    DWORD dwBitMapTemp[MAP_MARKER_WIDTH*MAP_MARKER_HEIGHT] = 
    {
/*0*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*1*/   dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB, dwB,
/*2*/   dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA,
/*3*/   dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA,
/*4*/   dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA,
/*5*/   dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA,
/*6*/   dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA,
/*7*/   dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA,
/*8*/   dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA,
/*9*/   dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA,
/*10*/  dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA,
/*11*/  dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA,
/*12*/  dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*13*/  dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwC, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA,
/*14*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwC, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*15*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwC, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*16*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwC, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*17*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwB, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*18*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
/*19*/  dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwB, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA, dwA,
    };

    memcpy ( dwBitMap, dwBitMapTemp, sizeof ( dwBitMapTemp ) );
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRadarArea.cpp
*  PURPOSE:     Radar area entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Oliver Brown <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Chris McArthur <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static const luaL_Reg radararea_interface[] =
{
    { NULL, NULL }
};

static int luaconstructor_radararea( lua_State *L )
{
    CClientRadarArea *area = (CClientRadarArea*)lua_touserdata( L, lua_upvalueindex( 1 ) );

    ILuaClass& j = *lua_refclass( L, 1 );
    j.SetTransmit( LUACLASS_RADARAREA, area );

    j.RegisterInterfaceTrans( L, radararea_interface, 0, LUACLASS_RADARAREA );

    lua_pushlstring( L, "radararea", 9 );
    lua_setfield( L, LUA_ENVIRONINDEX, "__type" );
    return 0;
}

CClientRadarArea::CClientRadarArea( class CClientManager* pManager, ElementID ID, lua_State *L, bool system ) : CClientEntity( ID, system, L )
{
    // Lua instancing
    PushStack( L );
    lua_pushlightuserdata( L, this );
    lua_pushcclosure( L, luaconstructor_radararea, 1 );
    luaJ_extend( L, -2, 0 );
    lua_pop( L, 1 );

    // Init
    m_pManager = pManager;
    m_pRadarAreaManager = pManager->GetRadarAreaManager();
    m_Color = SColorRGBA( 255, 255, 255, 255 );
    m_bFlashing = false;
    m_ulFlashCycleStart = 0;
    m_bStreamedIn = true;

    SetTypeName( "radararea" );

    // Make sure we're visible/invisible according to our dimension
    RelateDimension( m_pRadarAreaManager->GetDimension() );

    // Add us to the manager's list
    m_pRadarAreaManager->AddToList( this );
}

CClientRadarArea::~CClientRadarArea ( void )
{
    // Remove us from the manager's list
    Unlink ();
}

void CClientRadarArea::Unlink ( void )
{
    m_pRadarAreaManager->RemoveFromList ( this );
}

void CClientRadarArea::DoPulse ( void )
{
    DoPulse ( true );
}


void CClientRadarArea::DoPulse ( bool bRender )
{
    #define RADAR_FLASH_CYCLETIME 1000

    // Suppose to show?
    if ( m_bStreamedIn )
    {
        // If it's flashing, calculate a new alpha
        SColor color = m_Color;
     
        if ( m_bFlashing )
        {
            // Time to start a new cycle?
            unsigned long ulCurrentTime = CClientTime::GetTime ();
            if ( m_ulFlashCycleStart == 0 )
            {
                m_ulFlashCycleStart = ulCurrentTime;
            }
            // Time to end the cycle and start a new?
            else if ( ulCurrentTime >= m_ulFlashCycleStart + RADAR_FLASH_CYCLETIME )
            {
                m_ulFlashCycleStart = ulCurrentTime;
            }

            // Calculate the alpha based on the last cycle time and the cycle intervals
            float fAlphaFactor;

            // We're in the fade in part of the cycle?
            if ( ulCurrentTime >= m_ulFlashCycleStart + RADAR_FLASH_CYCLETIME / 2 )
            {
                // Calculate the alpha-factor
                fAlphaFactor = static_cast < float > ( ulCurrentTime - m_ulFlashCycleStart - RADAR_FLASH_CYCLETIME / 2 ) / ( RADAR_FLASH_CYCLETIME / 2 );
            }
            else
            {
                // Calculate the alpha-factor
                fAlphaFactor = 1.0f - static_cast < float > ( ulCurrentTime - m_ulFlashCycleStart ) / ( RADAR_FLASH_CYCLETIME / 2 );
            }

            // Multiply the alpha-factor with the alpha we're supposed to have to find what alpha to use and set it
            color.A = static_cast < unsigned char > ( fAlphaFactor * static_cast < float > ( color.A ) );
        }

        // Only render the radar area if we are told to
        if ( bRender )
        {
            // Draw it
            g_pGame->GetRadar ()->DrawAreaOnRadar ( m_vecPosition.fX + m_vecSize.fX, m_vecPosition.fY,
                                                    m_vecPosition.fX, m_vecPosition.fY + m_vecSize.fY,
                                                    color );
        }
    }
}


void CClientRadarArea::SetDimension ( unsigned short usDimension )
{
    m_usDimension = usDimension;
    RelateDimension ( m_pRadarAreaManager->GetDimension () );
}


void CClientRadarArea::RelateDimension ( unsigned short usDimension )
{
    m_bStreamedIn = ( usDimension == m_usDimension );
}

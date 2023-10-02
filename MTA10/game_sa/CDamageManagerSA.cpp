/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CDamageManagerSA.cpp
*  PURPOSE:     Vehicle damage manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CDamageManagerSAInterface::CDamageManagerSAInterface()
{
    m_engineStatus = 0;

    memset( m_wheels, 0, sizeof(m_wheels) );
    memset( m_doors, 0, sizeof(m_doors) );

    m_lights = 0;
    m_panels = 0;
}

CDamageManagerSAInterface::~CDamageManagerSAInterface()
{

}

unsigned char CDamageManagerSA::GetEngineStatus() const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetEngineStatus() const");

    return m_interface->m_engineStatus;
}

void CDamageManagerSA::SetEngineStatus( unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetEngineStatus( unsigned char status )");

    if ( status > 250 )
        status = 250;

    m_interface->m_engineStatus = status;
}

unsigned char CDamageManagerSA::GetDoorStatus( eDoors bDoor ) const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetDoorStatus( eDoors bDoor ) const");

    return m_interface->m_doors[ bDoor ];
}

void CDamageManagerSA::SetDoorStatus( eDoors door, unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetDoorStatus( eDoors door, unsigned char status )");

    // Different from before?
    if ( m_interface->m_doors[door] == status )
        return;

    // Set it
    m_interface->m_doors[door] = status;

    // Are we making it intact?
    if ( status == DT_DOOR_INTACT || status == DT_DOOR_SWINGING_FREE )
    {
        // Grab the car node index for the given door id
        static int s_iCarNodeIndexes [6] = { 0x10, 0x11, 0x0A, 0x08, 0x0B, 0x09 };

        // Call CAutomobile::FixDoor to update the model
        DWORD dwFunc = 0x6A35A0;
        DWORD dwThis = (DWORD)m_vehicle;
        int iCarNodeIndex = s_iCarNodeIndexes[door];
        DWORD dwDoor = (DWORD)door;
        _asm
        {
            mov     ecx, dwThis
            push    dwDoor
            push    iCarNodeIndex
            call    dwFunc
        }
    }
    else
    {
        // Call CAutomobile::SetDoorDamage to update the model
        DWORD dwFunc = 0x6B1600;
        DWORD dwThis = (DWORD)m_vehicle;
        DWORD dwDoor = (DWORD)door;
        bool bUnknown = false;
        _asm
        {
            mov     ecx, dwThis
            push    bUnknown
            push    dwDoor
            call    dwFunc
        }
    }
}

unsigned char CDamageManagerSA::GetWheelStatus( eWheels bWheel ) const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetWheelStatus( eWheels bWheel ) const");

    if ( bWheel > MAX_WHEELS-1 )
        return 0;

    return m_interface->m_wheels[bWheel];
}

void CDamageManagerSA::SetWheelStatus( eWheels bWheel, unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetWheelStatus( eWheels bWheel, unsigned char status )");

    if ( bWheel > MAX_WHEELS-1 )
        return;

    m_interface->m_wheels[bWheel] = status;
}

void CDamageManagerSA::SetPanelStatus( unsigned char panel, unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetPanelStatus( unsigned char panel, unsigned char status )");

    if ( panel > MAX_PANELS-1 || status > 3 )
        return;


    // Different than already?
    if ( GetPanelStatus( panel ) == status )
        return;

    // Call the function to set it
    DWORD dwFunction = FUNC_SetPanelStatus;
    DWORD dwThis = (DWORD)m_interface;
    DWORD dwPanel = panel;
    DWORD dwStatus = status;
    _asm
    {
        mov     ecx, dwThis
        push    dwStatus
        push    dwPanel
        call    dwFunction
    }

    // Intact?
    if ( status == DT_PANEL_INTACT )
    {
        // Grab the car node index for the given panel
        static int s_iCarNodeIndexes [7] = { 0x0F, 0x0E, 0x00 /*?*/, 0x00 /*?*/, 0x12, 0x0C, 0x0D };

        //  Call CAutomobile::FixPanel to update the vehicle
        dwFunction = 0x6A3670;
        dwThis = (DWORD)m_vehicle;
        int iCarNodeIndex = s_iCarNodeIndexes[panel];
        _asm
        {
            mov     ecx, dwThis
            push    dwPanel
            push    iCarNodeIndex
            call    dwFunction
        }
    }
    else
    {
        // Call CAutomobile::SetPanelDamage to update the vehicle
        dwFunction = 0x6B1480;
        dwThis = (DWORD)m_vehicle;
        bool bUnknown = false;
        _asm
        {
            mov     ecx, dwThis
            push    bUnknown
            push    dwPanel
            call    dwFunction
        }
    }
}

void CDamageManagerSA::SetPanelStatus( unsigned long ulStatus )
{
    unsigned int uiIndex;

    for ( uiIndex = 0; uiIndex < MAX_PANELS; uiIndex++ )
    {
        SetPanelStatus ( static_cast < eDoors > ( uiIndex ), static_cast < unsigned char > ( ulStatus ) );
        ulStatus >>= 4;
    }
}

unsigned char CDamageManagerSA::GetPanelStatus( unsigned char panel ) const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetPanelStatus( unsigned char panel ) const");

    if ( panel > MAX_PANELS-1 )
        return 0;

    DWORD dwFunction = FUNC_GetPanelStatus;
    DWORD dwPointer = (DWORD)m_interface;
    unsigned char bReturn;
    DWORD dwPanel = panel;
    _asm
    {
        mov     ecx, dwPointer
        push    dwPanel
        call    dwFunction
        mov     bReturn, al
    }

    return bReturn;
}

unsigned long CDamageManagerSA::GetPanelStatus() const
{
    return m_interface->m_panels;
}

void CDamageManagerSA::SetLightStatus( unsigned char light, unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetLightStatus( unsigned char light, unsigned char status )");
    DWORD dwFunction = FUNC_SetLightStatus;
    DWORD dwPointer = (DWORD)m_interface;
    DWORD dwLight = light;
    DWORD dwStatus = status;
    _asm
    {
        mov     ecx, dwPointer
        push    dwStatus
        push    dwLight
        call    dwFunction
    }
}

void CDamageManagerSA::SetLightStatus( unsigned char ucStatus )
{
    m_interface->m_lights = ucStatus;
}

unsigned char CDamageManagerSA::GetLightStatus( unsigned char light ) const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetLightStatus( unsigned char light ) const");
    DWORD dwFunction = FUNC_GetLightStatus;
    DWORD dwPointer = (DWORD)m_interface;
    unsigned char bReturn;
    DWORD dwLight = light;
    _asm
    {
        mov     ecx, dwPointer
        push    dwLight
        call    dwFunction
        mov     bReturn, al
    }
    return bReturn;
}

unsigned char CDamageManagerSA::GetLightStatus() const
{
    return (unsigned char)m_interface->m_panels;
}

void CDamageManagerSA::SetAeroplaneCompStatus( unsigned char id, unsigned char status )
{
    DEBUG_TRACE("void CDamageManagerSA::SetAeroplaneCompStatus( unsigned char id, unsigned char status )");
    DWORD dwFunction = FUNC_SetAeroplaneCompStatus;
    DWORD dwPointer = (DWORD)m_interface;
    DWORD dwPanel = id;
    _asm
    {
        mov     ecx,dwPointer
        push    status
        push    dwPanel
        call    dwFunction
    }
}

unsigned char CDamageManagerSA::GetAeroplaneCompStatus( unsigned char id ) const
{
    DEBUG_TRACE("unsigned char CDamageManagerSA::GetAeroplaneCompStatus( unsigned char id ) const");
    DWORD dwFunction = FUNC_GetAeroplaneCompStatus;
    DWORD dwPointer = (DWORD)m_interface;
    unsigned char bReturn;
    DWORD dwPannel = id;
    _asm
    {
        mov     ecx,dwPointer
        push    dwPannel
        call    dwFunction
        mov     bReturn, al
    }
    return bReturn;
}

void CDamageManagerSA::FuckCarCompletely( bool keepWheels )
{
    DEBUG_TRACE("void CDamageManagerSA::FuckCarCompletely( bool keepWheels )");
    DWORD dwFunc = FUNC_FuckCarCompletely;
    DWORD dwPointer = (DWORD)m_interface;
    _asm
    {
        mov     ecx,dwPointer
        push    keepWheels
        call    dwFunc
    }   
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPadManagerSA.cpp
*  PURPOSE:     Joypad management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPadManagerSA::CPadManagerSA()
{
    m_keys = core->GetKeyBinds();
}

CPadManagerSA::~CPadManagerSA()
{
}

CPadSAInterface* CPadManagerSA::GetJoypad( unsigned int index )
{
    // There is only one playerData
    if ( index != 0 )
        return NULL;

    return (CPadSAInterface*)0x00B73458;
}

void CPadManagerSA::GetFootControl( const CControlInterface& states, const CPedSA& ped, CControllerState& cs ) const
{
    bool detonator = ped.GetCurrentWeaponSlot() == 12;
    bool aimingWeapon = states.GetControlState( CONTROL_AIM_WEAPON );
    bool enteringVehicle = ped.IsEnteringVehicle();

    cs.ButtonCircle = DIGITAL_BUTTON( states.GetControlState( CONTROL_FIRE ) && !detonator );
    cs.RightShoulder2 = DIGITAL_BUTTON( states.GetControlState( CONTROL_NEXT_WEAPON ) || ( aimingWeapon && states.GetControlState( CONTROL_ZOOM_IN ) ) );
    cs.LeftShoulder2 = DIGITAL_BUTTON( states.GetControlState( CONTROL_PREVIOUS_WEAPON ) || ( aimingWeapon && states.GetControlState( CONTROL_ZOOM_OUT ) ) );
    cs.LeftStickY = DIGITAL_AXIS( states.GetControlState( CONTROL_FORWARDS ), states.GetControlState( CONTROL_BACKWARDS ) );
    cs.LeftStickX = DIGITAL_AXIS( states.GetControlState( CONTROL_LEFT ), states.GetControlState( CONTROL_RIGHT ) );

    cs.ButtonSquare = DIGITAL_BUTTON( !enteringVehicle && states.GetControlState( CONTROL_JUMP ) );
    cs.ButtonCross = DIGITAL_BUTTON( states.GetControlState( CONTROL_SPRINT ) );
    cs.ShockButtonR = DIGITAL_BUTTON( states.GetControlState( CONTROL_LOOK_BEHIND ) );
    cs.ShockButtonL = DIGITAL_BUTTON( states.GetControlState( CONTROL_CROUCH ) );
    cs.LeftShoulder1 = DIGITAL_BUTTON( states.GetControlState( CONTROL_ACTION ) );
    cs.m_bPedWalk = DIGITAL_BUTTON( states.GetControlState( CONTROL_WALK ) );

    cs.RightShoulder1 = DIGITAL_BUTTON( states.GetControlState( CONTROL_AIM_WEAPON ) );
    cs.m_digitalRight = DIGITAL_BUTTON( states.GetControlState( CONTROL_CONVERSATION_YES ) );
    cs.m_digitalLeft = DIGITAL_BUTTON( states.GetControlState( CONTROL_CONVERSATION_NO ) );
    cs.m_digitalUp = DIGITAL_BUTTON( states.GetControlState( CONTROL_GROUP_CONTROL_FORWARDS ) );
    cs.m_digitalDown = DIGITAL_BUTTON( states.GetControlState( CONTROL_GROUP_CONTROL_BACK ) );
}

void CPadManagerSA::GetVehicleControl( const CControlInterface& states, const CPedSA& ped, CControllerState& cs ) const
{
    cs.ButtonCircle = DIGITAL_BUTTON( states.GetControlState( CONTROL_VEHICLE_FIRE ) );
    cs.LeftShoulder1 = DIGITAL_BUTTON( states.GetControlState( CONTROL_VEHICLE_SECONDARY_FIRE ) );
    cs.LeftStickX = DIGITAL_AXIS( states.GetControlState( CONTROL_VEHICLE_LEFT ), states.GetControlState( CONTROL_VEHICLE_RIGHT ) );
    cs.LeftStickY = DIGITAL_AXIS( states.GetControlState( CONTROL_STEER_FORWARD ), states.GetControlState( CONTROL_STEER_BACK ) );

    cs.ButtonCross = DIGITAL_BUTTON( states.GetControlState( CONTROL_ACCELERATE ) );
    cs.ButtonSquare = DIGITAL_BUTTON( states.GetControlState( CONTROL_BRAKE_REVERSE ) );
    cs.m_digitalUp = DIGITAL_BUTTON( states.GetControlState( CONTROL_RADIO_NEXT ) );
    cs.m_digitalDown = DIGITAL_BUTTON( states.GetControlState( CONTROL_RADIO_PREVIOUS ) );
    cs.m_radioTrackSkip = DIGITAL_BUTTON( states.GetControlState( CONTROL_RADIO_USER_TRACK_SKIP ) );
    cs.ShockButtonL = DIGITAL_BUTTON( states.GetControlState( CONTROL_HORN ) );
    cs.ShockButtonR = DIGITAL_BUTTON( states.GetControlState( CONTROL_SUB_MISSION ) );
    cs.RightShoulder1 = DIGITAL_BUTTON( states.GetControlState( CONTROL_HANDBRAKE ) );
    cs.LeftShoulder2 = DIGITAL_BUTTON( states.GetControlState( CONTROL_VEHICLE_LOOK_LEFT ) || states.GetControlState( CONTROL_VEHICLE_LOOK_BEHIND ) );
    cs.RightShoulder2 = DIGITAL_BUTTON( states.GetControlState( CONTROL_VEHICLE_LOOK_RIGHT ) || states.GetControlState( CONTROL_VEHICLE_LOOK_BEHIND ) );

    cs.RightStickX = DIGITAL_AXIS( states.GetControlState( CONTROL_SPECIAL_CONTROL_LEFT ), states.GetControlState( CONTROL_SPECIAL_CONTROL_RIGHT ) );
    cs.RightStickY = DIGITAL_AXIS( states.GetControlState( CONTROL_SPECIAL_CONTROL_DOWN ), states.GetControlState( CONTROL_SPECIAL_CONTROL_UP ) );
}

void CPadManagerSA::MakeControllerStateForPed( CControlInterface& states, const CPedSA& ped, CControllerState& cs )
{
    if ( !ped.IsDead() )
    {
        if ( ped.GetVehicle() != NULL )
        {
            GetVehicleControl( states, ped, cs );
        }
        else
        {
            GetFootControl( states, ped, cs );
        }

        // Global controls
        cs.ButtonTriangle = DIGITAL_BUTTON( states.GetControlState( CONTROL_ENTER_EXIT ) );
        cs.m_select = DIGITAL_BUTTON( states.GetControlState( CONTROL_CHANGE_CAMERA ) );  
    }
}

void CPadManagerSA::UpdateJoypadEx( CControlInterface& states, CPedSA& ped )
{
    CPadSAInterface& pad = ped.GetJoypad();

    // Retrive the current controls
    CControllerState cs;

    MakeControllerStateForPed( states, ped, cs );
    
    pad.SetState( cs );

    // Sirens
    pad.SetHornHistory( cs.ShockButtonL == 255 );
}

void CPadManagerSA::UpdateLocalJoypad( CPedSA& ped )
{
    CPadSAInterface& pad = ped.GetJoypad();

    // Retrive the current controls
    CControllerState cs;

    MakeControllerStateForPed( *m_keys, ped, cs );

    if ( !ped.IsDead() )
    {
        core->GetJoystickManager()->ApplyAxes( cs, ped.GetVehicle() != NULL );
    }

    pad.SetState( cs );

    // Sirens
    pad.SetHornHistory( cs.ShockButtonL == 255 );
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPadManagerSA.h
*  PURPOSE:     Joypad management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CPadManagerSA_
#define _CPadManagerSA_

#define DIGITAL_BUTTON( state ) ( state ? 0xFF : 0 )
#define DIGITAL_AXIS( upState, downState ) ( ( ( (upState) && (downState) ) || !(upState) && !(downState) ) ? 0 : (upState) ? -0x80 : 0x80 )

class CPadManagerSA : public CPadManager
{
public:
                            CPadManagerSA();
                            ~CPadManagerSA();

    CPadSAInterface*        GetJoypad( unsigned int index );

    void                    GetFootControl( const CControlInterface& states, const CPedSA& ped, CControllerState& cs ) const;
    void                    GetVehicleControl( const CControlInterface& states, const CPedSA& ped, CControllerState& cs ) const;
    void                    MakeControllerStateForPed( CControlInterface& states, const CPedSA& ped, CControllerState& cs );
    void                    UpdateLocalJoypad( CPedSA& ped );
    void                    UpdateJoypadEx( CControlInterface& states, CPedSA& ped );

    // Virtual functions
    void                    UpdateJoypad( CControlInterface& states, CPed& ped )                { UpdateJoypadEx( states, dynamic_cast <CPedSA&> ( ped ) ); }

private:
    CKeyBindsInterface*     m_keys;
};

#endif //_CPadManagerSA_
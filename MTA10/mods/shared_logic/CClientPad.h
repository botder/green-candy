/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPad.h
*  PURPOSE:     Ped entity controller pad handler class
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CClientPad_H
#define __CClientPad_H

#define MAX_GTA_ANALOG_CONTROLS 14

class CClientPed;
class CControllerState;

class CClientPad : public virtual CControlInterface
{
public:
                                    CClientPad();

    bool                            GetControlState( eBindableControl control ) const;

    bool                            GetControlState( const char *name, bool& state );
    bool                            SetControlState( const char *name, bool state );

    bool                            GetControlState( eBindableControl cntrl );

    static bool                     GetControlIndex( const char *name, unsigned int& index );
    static const char*              GetControlName( unsigned int index );

    static bool                     GetAnalogControlIndex( const char *name, unsigned int& index );

    static bool                     GetAnalogControlState( const char *name, CControllerState& cs, bool onFoot, float& state );
    static bool                     GetControlState( const char *name, CControllerState& State, bool onFoot );

protected:
    bool                            m_states[MAX_CONTROLS];
};

#endif
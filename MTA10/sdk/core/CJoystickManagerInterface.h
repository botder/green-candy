/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CJoystickManagerInterface.h
*  PURPOSE:     Header file for the joystick manager class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CJoystickManagerInterface_
#define _CJoystickManagerInterface_

class CJoystickManagerInterface
{
public:
    virtual             ~CJoystickManagerInterface  () {}

    virtual void        ApplyAxes                   ( class CControllerState& cs, bool bInVehicle ) = 0;

    // Status
    virtual bool        IsJoypadConnected           () = 0;

    // Settings
    virtual std::string GetControllerName           () = 0;
    virtual int         GetDeadZone                 () = 0;
    virtual int         GetSaturation               () = 0;
    virtual void        SetDeadZone                 ( int iDeadZone ) = 0;
    virtual void        SetSaturation               ( int iSaturation ) = 0;
    virtual int         GetSettingsRevision         () = 0;

    // Binding
    virtual int         GetOutputCount              () = 0;
    virtual std::string GetOutputName               ( int iOutputIndex ) = 0;
    virtual std::string GetOutputInputName          ( int iOutputIndex ) = 0;
    virtual bool        BindNextUsedAxisToOutput    ( int iOutputIndex ) = 0;
    virtual bool        IsAxisBindComplete          () = 0;
    virtual bool        IsCapturingAxis             () = 0;
    virtual void        CancelCaptureAxis           ( bool bClear ) = 0;
};

#endif //_CJoystickManagerInterface_
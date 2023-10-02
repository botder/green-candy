/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CJoystickManager.h
*  PURPOSE:     Header file for the joystick manager class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CJOYSTICKMANAGER_H
#define __CJOYSTICKMANAGER_H

//////////////////////////////////////////////////////////
//
// Mapping
//
enum eJoy
{
    eJoyX,
    eJoyY,
    eJoyZ,
    eJoyRx,
    eJoyRy,
    eJoyRz,
    eJoyS1,
    eJoyMax
};

enum eDir
{
    eDirNeg,
    eDirPos,
    eDirMax
};

enum eStick
{
    eLeftStickX,
    eLeftStickY,
    eRightStickX,
    eRightStickY,
    eAccelerate,
    eBrake,
    eStickMax
};

struct SMappingLine
{
    eJoy    SourceAxisIndex;    // 0 - 7
    eDir    SourceAxisDir;      // 0 - 1
    eStick  OutputAxisIndex;    // 0/1 2/3 4 5
    eDir    OutputAxisDir;      // 0 - 1
    bool    bEnabled;
    int     MaxValue;
};

//////////////////////////////////////////////////////////
//
// DeviceInfo
//
struct SInputDeviceInfo
{
    IDirectInputDevice8A*   pDevice;
    bool                    bDoneEnumAxes;
    int                     iAxisCount;
    int                     iDeadZone;
    int                     iSaturation;
    GUID                    guidProduct;
    std::string             strGuid;
    std::string             strProductName;

    struct
    {
        bool    bEnabled;
        long    lMax;
        long    lMin;
    } axis[7];
};

// Internal state
struct SJoystickState
{
    float   rgfAxis[7];             /* axis positions     -1.f to 1.f       */
    DWORD   rgdwPOV[4];             /* POV directions                       */
    BYTE    rgbButtons[32];         /* 32 buttons                           */
    BYTE    rgbButtonsWas[32];
    BYTE    povButtonsWas[4];
};

class CJoystickManager : public CJoystickManagerInterface
{
public:
    ZERO_ON_NEW
                        CJoystickManager            ();
                        ~CJoystickManager           ();

    // CJoystickManagerInterface methods
    void                OnSetDataFormat             ( IDirectInputDevice8A* pDevice, LPCDIDATAFORMAT a );
    void                RemoveDevice                ( IDirectInputDevice8A* pDevice );
    void                DoPulse                     ();
    void                ApplyAxes                   ( CControllerState& cs, bool bInVehicle );

    // Status
    bool                IsJoypadConnected           ();

    // Settings
    std::string         GetControllerName           ();
    int                 GetDeadZone                 ();
    int                 GetSaturation               ();
    void                SetDeadZone                 ( int iDeadZone );
    void                SetSaturation               ( int iSaturation );
    int                 GetSettingsRevision         ();
    void                SetDefaults                 ();
    bool                SaveToXML                   ();

    // Binding
    int                 GetOutputCount              ();
    std::string         GetOutputName               ( int iOutputIndex );
    std::string         GetOutputInputName          ( int iOutputIndex );
    bool                BindNextUsedAxisToOutput    ( int iOutputIndex );
    bool                IsAxisBindComplete          ();
    bool                IsCapturingAxis             ();
    void                CancelCaptureAxis           ( bool bClearBinding );

    // CJoystickManager methods
    BOOL                DoEnumObjectsCallback       ( const DIDEVICEOBJECTINSTANCE* pdidoi );
private:
    bool                IsJoypadValid               ();
    void                EnumAxes                    ();
    void                ReadCurrentState            ();
    CXMLNode*           GetConfigNode               ( bool bCreateIfRequired );
    bool                LoadFromXML                 ();

    int                     m_SettingsRevision;
    SInputDeviceInfo        m_DevInfo;
    SJoystickState          m_JoystickState;
    SMappingLine            m_currentMapping[10];

    // Used during axis binding
    bool                    m_bCaptureAxis;
    int                     m_iCaptureOutputIndex;
    SJoystickState          m_PreBindJoystickState;
};

// Custom VK_ codes for the joystick buttons - Starts at VK_JOY(1)
#define VK_JOY(x)       ( 0x100+(x) )
// Custom VK_ codes for the joystick pov hat - Starts at VK_POV(1)
#define VK_POV(x)       ( 0x150+(x) )

// Unique numbers for the joystick buttons - Used to identify buttons in the GTA settings
#define GTA_KEY_JOY(x)  ( 500+(x) )

#endif


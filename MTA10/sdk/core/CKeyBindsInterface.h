/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CKeyBindsInterface.h
*  PURPOSE:     Keybind manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CKEYBINDSINTERFACE_H
#define __CKEYBINDSINTERFACE_H

// Beware: this class is poorly written and depends on game while it shouldn't
// The_GTA: Thank you for pointing out the obvious and not helping anything against it
// while it would be a great improvement of code quality, faggot.
#include <game/CControllerConfigManager.h>

class CKeyFunctionBind;
class CControlFunctionBind;
struct SBindableKey;

typedef void (*KeyFunctionBindHandler )     ( CKeyFunctionBind* pBind );
typedef void (*ControlFunctionBindHandler)  ( CControlFunctionBind* pBind );
typedef void (*KeyStrokeHandler)            ( const SBindableKey * pKey, bool bState );
typedef bool (*CharacterKeyHandler)         ( WPARAM wChar );
 
enum eKeyData
{
    DATA_NONE = 0,
    DATA_EXTENDED,
    DATA_NOT_EXTENDED,
    DATA_NUMPAD,
};

struct SBindableKey
{
    char szKey [20];
    unsigned long ulCode;
    int iGTARelative;
    eKeyData data;
    unsigned char ucNumpadRelative;
    bool bIgnoredByGUI;
};

enum eControlType
{
    CONTROL_FOOT,
    CONTROL_VEHICLE,
    CONTROL_BOTH
};

enum eBindableControl
{
    CONTROL_FIRE,
    CONTROL_NEXT_WEAPON,
    CONTROL_PREVIOUS_WEAPON,
    CONTROL_FORWARDS,
    CONTROL_BACKWARDS,
    CONTROL_LEFT,
    CONTROL_RIGHT,
    CONTROL_ZOOM_IN,
    CONTROL_ZOOM_OUT,
    CONTROL_ENTER_EXIT,
    CONTROL_CHANGE_CAMERA,
    CONTROL_JUMP,
    CONTROL_SPRINT,
    CONTROL_LOOK_BEHIND,
    CONTROL_CROUCH,
    CONTROL_ACTION,
    CONTROL_WALK,
    CONTROL_VEHICLE_FIRE,
    CONTROL_VEHICLE_SECONDARY_FIRE,
    CONTROL_VEHICLE_LEFT,
    CONTROL_VEHICLE_RIGHT,
    CONTROL_STEER_FORWARD,
    CONTROL_STEER_BACK,
    CONTROL_ACCELERATE,
    CONTROL_BRAKE_REVERSE,
    CONTROL_RADIO_NEXT,
    CONTROL_RADIO_PREVIOUS,
    CONTROL_RADIO_USER_TRACK_SKIP,
    CONTROL_HORN,
    CONTROL_SUB_MISSION,
    CONTROL_HANDBRAKE,
    CONTROL_VEHICLE_LOOK_LEFT,
    CONTROL_VEHICLE_LOOK_RIGHT,
    CONTROL_VEHICLE_LOOK_BEHIND,
    CONTROL_VEHICLE_MOUSE_LOOK,
    CONTROL_SPECIAL_CONTROL_LEFT,
    CONTROL_SPECIAL_CONTROL_RIGHT,
    CONTROL_SPECIAL_CONTROL_DOWN,
    CONTROL_SPECIAL_CONTROL_UP,
    CONTROL_AIM_WEAPON,
    CONTROL_CONVERSATION_YES,
    CONTROL_CONVERSATION_NO,
    CONTROL_GROUP_CONTROL_FORWARDS,
    CONTROL_GROUP_CONTROL_BACK,
    MAX_CONTROLS
};

struct SBindableGTAControl
{
    char szControl [25];
    eControllerAction action;
    eControlType controlType;
    bool bEnabled;
    char* szDescription;
};

enum eKeyBindType
{
    KEY_BIND_COMMAND, // bind key function args
    KEY_BIND_GTA_CONTROL, // bind key gta_control 
    KEY_BIND_FUNCTION, // script bind key to function 
    KEY_BIND_CONTROL_FUNCTION, // script bind gta_control to function (pressing control, calls function)
    KEY_BIND_UNDEFINED,
};

class CKeyBind
{
public:
    CKeyBind( const SBindableKey *key ) : beingDeleted( false )
    {
        bActive = true;
        boundKey = key;
    }

    inline bool             IsBeingDeleted() const  { return beingDeleted; }

    virtual eKeyBindType    GetType() const = 0;

    const SBindableKey*     boundKey;
    bool                    beingDeleted;
    bool                    bActive;
};

class CKeyBindWithState : public CKeyBind
{
public:
    CKeyBindWithState( const SBindableKey *key ) : CKeyBind( key )
    {
        bState = false;
        bHitState = false;
    }

    bool            bState;
    bool            bHitState;
};


class CCommandBind : public CKeyBindWithState
{
public:
    CCommandBind( const SBindableKey *key, const char *cmd, const char *args, const char *res, bool altKey ) : CKeyBindWithState( key )
    {
        m_cmd = cmd;

        if ( args )
            m_args = args;

        if ( res )
        {
            m_res = new std::string( res );
            m_defKey = new std::string( altKey ? "" : key->szKey );
        }
        else
            m_res = m_defKey = NULL;
    }

    ~CCommandBind()
    {
        delete m_res;
        delete m_defKey;
    }

    eKeyBindType    GetType() const
    {
        return KEY_BIND_COMMAND;
    }

    const char*     GetResource() const
    {
        if ( !m_res )
            return NULL;

        return m_res->c_str();
    }

    std::string     m_cmd;
    std::string     m_args;
    std::string*    m_res;
    std::string*    m_defKey;
};

class CKeyFunctionBind : public CKeyBindWithState
{
public:
    CKeyFunctionBind( const SBindableKey *key, KeyFunctionBindHandler handler, bool ignoreGUI ) : CKeyBindWithState( key )
    {
        Handler = handler;
        bIgnoreGUI = ignoreGUI;
    }

    eKeyBindType            GetType() const         { return KEY_BIND_FUNCTION; }

    KeyFunctionBindHandler  Handler;
    bool                    bIgnoreGUI;
};

class CControlFunctionBind : public CKeyBindWithState
{
public:
    CControlFunctionBind( const SBindableKey *key, SBindableGTAControl *ctrl, ControlFunctionBindHandler handler ) : CKeyBindWithState( key )
    {
        control = ctrl;
        Handler = handler;
    }

    eKeyBindType               GetType() const      { return KEY_BIND_CONTROL_FUNCTION; }

    SBindableGTAControl*       control;
    ControlFunctionBindHandler Handler;
};

struct CGTAControlStatus
{
    CGTAControlStatus()
    {
        m_refCount = 0;
    }

    ~CGTAControlStatus()
    {
    }

    bool    IsActive() const
    {
        return m_refCount != 0;
    }

    void    AddRef()
    {
        m_refCount++;
    }

    void    RemoveRef()
    {
        if ( m_refCount > 0 )
            m_refCount--;
    }

    void    SetState( bool state )
    {
        m_refCount = state ? 1 : 0;
    }

    void    Reset()
    {
        m_refCount = 0;
    }

    unsigned int    m_refCount;
};

class CGTAControlBind : public CKeyBind
{
public:
    CGTAControlBind( const SBindableKey *key, SBindableGTAControl *ctrl, CGTAControlStatus *_status ) : CKeyBind( key )
    {
        control = ctrl;
        status = _status;
        bState = false;
        bEnabled = true;
    }

    eKeyBindType    GetType() const                 { return KEY_BIND_GTA_CONTROL; }

    SBindableGTAControl*    control;
    CGTAControlStatus*      status;
    bool            bState;
    bool            bEnabled;
};

class CControlInterface abstract
{
public:
    virtual bool                    GetControlState( eBindableControl control ) const = 0;
};

class CKeyBindsInterface : public CControlInterface
{
public:
    virtual bool                    ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;

    // Basic funcs
    virtual void                    Add( CKeyBind* pKeyBind ) = 0;
    virtual void                    Remove( CKeyBind* pKeyBind ) = 0;
    virtual void                    Clear() = 0;
    virtual bool                    Call( CKeyBind* pKeyBind ) = 0;

    typedef std::list <CKeyBind*> binds_t;

    virtual binds_t::const_iterator IterBegin() = 0;
    virtual binds_t::const_iterator IterEnd() = 0;

    typedef void (*cmdIterCallback_t)( CCommandBind *bind, void *ud );
    typedef void (*cntrlIterCallback_t)( CGTAControlBind *bind, void *ud );

    // Command-bind funcs
    virtual bool                    AddCommand( const char* szKey, const char* szCommand, const char* szArguments = NULL, bool bState = true, const char* szResource = NULL, bool bAltKey = false ) = 0;
    virtual bool                    AddCommand( const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true ) = 0;
    virtual CCommandBind*           GetCommandBind( const char *key, const char *cmd, bool checkState = false, bool state = true, const char *args = NULL, const char *res = NULL ) = 0;
    virtual bool                    SetCommandActive( const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive, bool checkHitState ) = 0;
    virtual void                    SetAllCommandsActive( const char* szResource, bool bActive, const char* szCommand = NULL, bool bState = true, const char* szArguments = NULL, bool checkHitState = false ) = 0;
    virtual CCommandBind*           GetBindFromCommand( const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true, const char* szKey = NULL, bool bCheckHitState = false, bool bState = NULL ) = 0;
    virtual void                    ForAllBoundCommands( const char *cmd, cmdIterCallback_t cb, void *ud ) = 0;

    // Control-bind funcs
    virtual void                    CallAllGTAControlBinds( eControlType controlType, bool bState ) = 0;
    virtual void                    ForAllBoundControls( SBindableGTAControl *control, cntrlIterCallback_t cb, void *ud ) = 0;

    virtual void                    SwitchToControlScheme( eControlType controlType ) = 0;

    virtual bool                    GetMultiGTAControlState( CGTAControlBind* pBind ) = 0;
    virtual bool                    IsControlEnabled( const char* szControl ) const = 0;
    virtual bool                    IsControlEnabled( eBindableControl index ) const = 0;
    virtual bool                    SetControlEnabled( const char* szControl, bool bEnabled ) = 0;
    virtual void                    SetAllControlsEnabled( bool bGameControls, bool bMTAControls, bool bEnabled ) = 0;
    
    // Function-bind funcs
    virtual bool                    AddFunction( const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false ) = 0;
    virtual void                    AddFunction( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false ) = 0;
    virtual bool                    RemoveFunction( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveFunction( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveAllFunctions( KeyFunctionBindHandler Handler ) = 0;
    virtual bool                    RemoveAllFunctions() = 0;
    virtual bool                    FunctionExists( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    FunctionExists( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;

    // Function-control-bind funcs
    virtual bool                    AddControlFunction( const char* szControl, ControlFunctionBindHandler Handler, bool bState = true ) = 0;
    virtual void                    AddControlFunction( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState = true ) = 0;
    virtual bool                    RemoveControlFunction( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveControlFunction( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    RemoveAllControlFunctions( ControlFunctionBindHandler Handler ) = 0;
    virtual bool                    RemoveAllControlFunctions() = 0;
    virtual bool                    ControlFunctionExists( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;
    virtual bool                    ControlFunctionExists( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true ) = 0;

    // Key/code funcs
    virtual const char*             GetKeyFromCode( unsigned long ulCode ) = 0;
    virtual bool                    GetCodeFromKey( const char* szKey, unsigned long& ucCode ) = 0;
    virtual const SBindableKey*     GetBindableFromKey( const char* szKey ) = 0;
    virtual const SBindableKey*     GetBindableFromGTARelative( int iGTAKey ) = 0;
    virtual bool                    IsKey( const char* szKey ) = 0;
    virtual const char*             GetKeyFromGTARelative( int iGTAKey ) = 0;
    virtual const SBindableKey*     GetBindableFromMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState ) = 0;
    virtual void                    SetKeyStrokeHandler( KeyStrokeHandler Handler ) = 0;
    virtual void                    SetCharacterKeyHandler( CharacterKeyHandler Handler ) = 0;

    // Control/action funcs
    virtual void                    SetRealControlState( eBindableControl control, bool state ) = 0;
    virtual bool                    GetRealControlState( eBindableControl control ) const = 0;
    virtual bool                    GetControlState( eBindableControl control ) const = 0;
    virtual const char*             GetControlFromAction( eControllerAction action ) = 0;
    virtual bool                    GetActionFromControl( const char* szControl, eControllerAction& action ) = 0;
    virtual SBindableGTAControl*    GetBindableFromControl( const char* szControl ) const = 0;
    virtual SBindableGTAControl*    GetBindableFromAction( eControllerAction action ) = 0;
    virtual SBindableGTAControl*    GetBindable( eBindableControl cntrl ) = 0;
    virtual bool                    GetBindableIndex( const std::string& name, eBindableControl& cntrl ) = 0;
    virtual bool                    IsControl( const char* szControl ) = 0;

    virtual void                    SetAllFootControls( bool bState ) = 0;
    virtual void                    SetAllVehicleControls( bool bState ) = 0;

    virtual void                    SetAllBindStates( bool bState, eKeyBindType onlyType = KEY_BIND_UNDEFINED ) = 0;
    virtual unsigned int            Count( eKeyBindType bindType ) = 0;

    virtual void                    DoPreFramePulse() = 0;
    virtual void                    DoPostFramePulse() = 0;
    virtual void                    DoPostGameFramePulse() = 0;

    virtual bool                    LoadFromXML( class CXMLNode* pMainNode ) = 0;
    virtual bool                    SaveToXML( class CXMLNode* pMainNode ) = 0;
    virtual void                    LoadDefaultBinds() = 0;
    virtual void                    LoadDefaultControls() = 0;
    virtual void                    LoadDefaultCommands( bool bForce ) = 0;
    virtual void                    LoadControlsFromGTA() = 0;

    virtual void                    BindCommand( const char* szCmdLine ) = 0;
    virtual void                    UnbindCommand( const char* szCmdLine ) = 0;
    virtual void                    PrintBindsCommand( const char* szCmdLine ) = 0;
};

#endif

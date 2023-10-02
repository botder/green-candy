/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CKeyBinds.h
*  PURPOSE:     Header file for core keybind manager class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CKeyBinds;

#ifndef __CKEYBINDS_H
#define __CKEYBINDS_H

#include <core/CCoreInterface.h>
#include <core/CCommandsInterface.h>

struct SDefaultCommandBind
{
    char szKey[20];
    bool bState;
    char szCommand[20];
    char szArguments[20];
};

class CKeyBinds : public CKeyBindsInterface
{
public:
                            CKeyBinds( class CCore *core );
                            ~CKeyBinds();

    bool                    ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected:
    bool                    ProcessCharacter( WPARAM wChar );
    bool                    ProcessKeyStroke( const SBindableKey *pKey, bool bState );

public:
    // Basic funcs
    void                    Add( CKeyBind *keyBind );
    void                    Remove( CKeyBind *keyBind );
    void                    Clear();
    void                    RemoveDeletedBinds();
    void                    ClearCommandsAndControls();
    bool                    Call( CKeyBind *keyBind );

    typedef std::list <CCommandBind*> cmdBinds_t;
    
    binds_t::const_iterator IterBegin()                                                                         { return m_list.begin(); }
    binds_t::const_iterator IterEnd()                                                                           { return m_list.end(); }


    // Command-bind funcs
    bool                    AddCommand( const char* szKey, const char* szCommand, const char* szArguments = NULL, bool bState = true, const char* szResource = NULL, bool bAltKey = false );
    bool                    AddCommand( const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true );
    bool                    RemoveCommand( const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllCommands( const char* szKey, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllCommands();
    CCommandBind*           GetCommandBind( const char *key, const char *cmd, bool checkState = false, bool state = true, const char *args = NULL, const char *res = NULL );
    bool                    SetCommandActive( const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive, bool checkHitState );
    void                    SetAllCommandsActive( const char* szResource, bool bActive, const char* szCommand = NULL, bool bState = true, const char* szArguments = NULL, bool checkHitState = false );
    CCommandBind*           GetBindFromCommand( const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true, const char* szKey = NULL, bool bCheckHitState = false, bool bState = NULL );
    void                    ForAllBoundCommands( const char *cmd, cmdIterCallback_t cb, void *ud );
    
    // Control-bind funcs
    void                    AddGTAControl( const char* szKey, const char* szControl );
    bool                    AddGTAControl( const char* szKey, eControllerAction action );
    void                    AddGTAControl( const SBindableKey *key, SBindableGTAControl *ctrl );
    void                    AddGTAControl( const SBindableKey* pKey, SBindableGTAControl *pControl, CGTAControlStatus *status );
    bool                    RemoveGTAControl( const char* szKey, const char* szControl );
    bool                    RemoveAllGTAControls( const char* szKey );
    bool                    RemoveAllGTAControls();
    bool                    GTAControlExists( const char* szKey, const char* szControl );
    bool                    GTAControlExists( const SBindableKey* pKey, SBindableGTAControl* pControl );
    void                    CallGTAControlBind( CGTAControlBind* pBind, bool bState );
    void                    CallAllGTAControlBinds( eControlType controlType, bool bState );
    void                    ForAllBoundControls( SBindableGTAControl *control, cntrlIterCallback_t cb, void *ud );

    void                    SwitchToControlScheme( eControlType controlType );
    bool                    IsActiveControlScheme( eControlType controlType ) const;

    // Control-bind extra funcs
    bool                    GetMultiGTAControlState( CGTAControlBind* pBind );
    bool                    IsControlEnabled( const char* szControl ) const;
    bool                    IsControlEnabled( eBindableControl index ) const;
    bool                    SetControlEnabled( const char* szControl, bool bEnabled );
    void                    SetAllControlsEnabled( bool bGameControls, bool bMTAControls, bool bEnabled );
    void                    ResetGTAControlState( SBindableGTAControl * pControl );
    void                    ResetAllGTAControlStates();    

    // Function-bind funcs
    bool                    AddFunction( const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false );
    void                    AddFunction( const SBindableKey *key, KeyFunctionBindHandler handler, bool state, bool ignoreGUI );
    bool                    RemoveFunction( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveFunction( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllFunctions( KeyFunctionBindHandler Handler );
    bool                    RemoveAllFunctions();
    bool                    FunctionExists( const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    FunctionExists( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );

    // Function-control-bind funcs
    bool                    AddControlFunction( const char *control, ControlFunctionBindHandler handler, bool state );
    void                    AddControlFunction( SBindableGTAControl *control, ControlFunctionBindHandler handler, bool state );
    bool                    RemoveControlFunction( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveControlFunction( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    RemoveAllControlFunctions( ControlFunctionBindHandler Handler );
    bool                    RemoveAllControlFunctions();
    bool                    ControlFunctionExists( const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );
    bool                    ControlFunctionExists( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true );

    // Key/code funcs
    const char*             GetKeyFromCode( unsigned long ulCode );
    bool                    GetCodeFromKey( const char* szKey, unsigned long& ucCode );
    const SBindableKey*     GetBindableFromKey( const char* szKey );
    const SBindableKey*     GetBindableFromGTARelative( int iGTAKey );
    bool                    IsKey( const char* szKey );
    const char*             GetKeyFromGTARelative( int iGTAKey );
    const SBindableKey*     GetBindableFromMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState );
    void                    SetKeyStrokeHandler( KeyStrokeHandler Handler )                                     { m_KeyStrokeHandler = Handler; }
    void                    SetCharacterKeyHandler( CharacterKeyHandler Handler )                               { m_CharacterKeyHandler = Handler; }

    // Control/action funcs
    void                    SetRealControlState( eBindableControl control, bool state );
    bool                    GetRealControlState( eBindableControl control ) const;
    bool                    GetControlState( eBindableControl control ) const;

    const char*             GetControlFromAction( eControllerAction action );
    bool                    GetActionFromControl( const char* szControl, eControllerAction& action );
    SBindableGTAControl*    GetBindableFromControl( const char* szControl ) const;
    SBindableGTAControl*    GetBindableFromAction( eControllerAction action );
    SBindableGTAControl*    GetBindable( eBindableControl cntrl );
    bool                    GetBindableIndex( const std::string& name, eBindableControl& cntrl );
    bool                    IsControl( const char* szControl );
    void                    SetAllControls( bool bState );
    void                    SetAllFootControls( bool bState );
    void                    SetAllVehicleControls( bool bState );

    void                    SetAllBindStates( bool bState, eKeyBindType onlyType = KEY_BIND_UNDEFINED );

    unsigned int            Count( eKeyBindType bindType = KEY_BIND_UNDEFINED );

    void                    DoPreFramePulse();
    void                    DoPostFramePulse();
    void                    DoPostGameFramePulse();

    bool                    LoadFromXML( CXMLNode* pMainNode );
    bool                    SaveToXML( CXMLNode* pMainNode );
    void                    LoadDefaultBinds();
    void                    LoadDefaultControls();
    void                    LoadDefaultCommands( bool bForce );
    void                    LoadControlsFromGTA();

    void                    BindCommand( const char* szCmdLine );
    void                    UnbindCommand( const char* szCmdLine );
    void                    PrintBindsCommand( const char* szCmdLine );

    static bool             IsFakeCtrl_L( UINT message, WPARAM wParam, LPARAM lParam );

    void                    AddSection( char* szSection );
    void                    RemoveSection( char* szSection );
    void                    RemoveAllSections();

private:    
    CCore*                      m_pCore;

    binds_t                     m_list;
    char*                       m_szFileName;
    bool                        m_bMouseWheel;
    bool                        m_bFrameMouseWheelUp;
    bool                        m_bFrameMouseWheelDown;
    CCommandBind*               m_pChatBoxBind;
    bool                        m_bProcessingKeyStroke;
    KeyStrokeHandler            m_KeyStrokeHandler;
    CharacterKeyHandler         m_CharacterKeyHandler;
    bool                        m_bWaitingToLoadDefaults;

    eControlType                m_currentControlScheme;
};

#endif

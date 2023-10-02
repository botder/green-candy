/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CKeyBinds.cpp
*  PURPOSE:     Core keybind manager
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

using std::list;

SBindableKey g_bkKeys[] = 
{ 
    { "mouse1",  0x01,          GTA_KEY_LMOUSE,         DATA_NONE,              0 },
    { "mouse2",  0x02,          GTA_KEY_RMOUSE,         DATA_NONE,              0 },
    { "mouse3",  0x04,          GTA_KEY_MMOUSE,         DATA_NONE,              0 },
    { "mouse4",  0x00,          GTA_KEY_MXB1,           DATA_NONE,              0 },
    { "mouse5",  0x00,          GTA_KEY_MXB2,           DATA_NONE,              0 }, 
    { "mouse_wheel_up", 0x00,   GTA_KEY_MSCROLLUP,      DATA_NONE,              0 },
    { "mouse_wheel_down", 0x00, GTA_KEY_MSCROLLDOWN,    DATA_NONE,              0 },
    { "backspace",   0x08,      GTA_KEY_BACK,           DATA_NONE,              0 },
    { "tab",     0x09,          GTA_KEY_TAB,            DATA_NONE,              0 }, 
    { "lshift",  0x10,          GTA_KEY_LSHIFT,         DATA_NOT_EXTENDED,      0 },
    { "rshift",  0x10,          GTA_KEY_RSHIFT,         DATA_EXTENDED,          0 }, // 10
    { "lctrl",   0x11,          GTA_KEY_LCONTROL,       DATA_NOT_EXTENDED,      0 }, 
    { "rctrl",   0x11,          GTA_KEY_RCONTROL,       DATA_EXTENDED,          0 },
    { "lalt",    0x12,          GTA_KEY_LMENU,          DATA_NOT_EXTENDED,      0,      true },
    { "ralt",    0x12,          GTA_KEY_RMENU,          DATA_EXTENDED,          0,      true },
    { "pause",   0x13,          GTA_KEY_PAUSE,          DATA_NONE,              0,      true },
    { "capslock",0x14,          GTA_KEY_CAPSLOCK,       DATA_NONE,              0 },
    { "enter",   0x0D,          GTA_KEY_RETURN,         DATA_NOT_EXTENDED,      0 },
    { "space",   0x20,          GTA_KEY_SPACE,          DATA_NONE,              0 },
    { "pgup",    0x21,          GTA_KEY_PGUP,           DATA_NUMPAD,            74,     true },
    { "pgdn",    0x22,          GTA_KEY_PGDN,           DATA_NUMPAD,            68,     true }, // 20
    { "end",     0x23,          GTA_KEY_END,            DATA_NUMPAD,            66 },
    { "home",    0x24,          GTA_KEY_HOME,           DATA_NUMPAD,            72 },
    { "arrow_l", 0x25,          GTA_KEY_LEFT,           DATA_NUMPAD,            69 },
    { "arrow_u", 0x26,          GTA_KEY_UP,             DATA_NUMPAD,            73 },
    { "arrow_r", 0x27,          GTA_KEY_RIGHT,          DATA_NUMPAD,            71 }, 
    { "arrow_d", 0x28,          GTA_KEY_DOWN,           DATA_NUMPAD,            67 },
    { "insert",  0x2D,          GTA_KEY_INSERT,         DATA_NUMPAD,            65,     true }, 
    { "delete",  0x2E,          GTA_KEY_DELETE,         DATA_NUMPAD,            79 },
    { "0",       0x30,          GTA_KEY_0,              DATA_NONE,              0 }, 
    { "1",       0x31,          GTA_KEY_1,              DATA_NONE,              0 }, // 30
    { "2",       0x32,          GTA_KEY_2,              DATA_NONE,              0 }, 
    { "3",       0x33,          GTA_KEY_3,              DATA_NONE,              0 }, 
    { "4",       0x34,          GTA_KEY_4,              DATA_NONE,              0 },
    { "5",       0x35,          GTA_KEY_5,              DATA_NONE,              0 },
    { "6",       0x36,          GTA_KEY_6,              DATA_NONE,              0 }, 
    { "7",       0x37,          GTA_KEY_7,              DATA_NONE,              0 }, 
    { "8",       0x38,          GTA_KEY_8,              DATA_NONE,              0 }, 
    { "9",       0x39,          GTA_KEY_9,              DATA_NONE,              0 },
    { "a",       0x41,          GTA_KEY_A,              DATA_NONE,              0 },
    { "b",       0x42,          GTA_KEY_B,              DATA_NONE,              0 }, // 40
    { "c",       0x43,          GTA_KEY_C,              DATA_NONE,              0 }, 
    { "d",       0x44,          GTA_KEY_D,              DATA_NONE,              0 }, 
    { "e",       0x45,          GTA_KEY_E,              DATA_NONE,              0 },
    { "f",       0x46,          GTA_KEY_F,              DATA_NONE,              0 }, 
    { "g",       0x47,          GTA_KEY_G,              DATA_NONE,              0 }, 
    { "h",       0x48,          GTA_KEY_H,              DATA_NONE,              0 }, 
    { "i",       0x49,          GTA_KEY_I,              DATA_NONE,              0 }, 
    { "j",       0x4A,          GTA_KEY_J,              DATA_NONE,              0 },
    { "k",       0x4B,          GTA_KEY_K,              DATA_NONE,              0 },
    { "l",       0x4C,          GTA_KEY_L,              DATA_NONE,              0 }, // 50
    { "m",       0x4D,          GTA_KEY_M,              DATA_NONE,              0 }, 
    { "n",       0x4E,          GTA_KEY_N,              DATA_NONE,              0 }, 
    { "o",       0x4F,          GTA_KEY_O,              DATA_NONE,              0 },
    { "p",       0x50,          GTA_KEY_P,              DATA_NONE,              0 }, 
    { "q",       0x51,          GTA_KEY_Q,              DATA_NONE,              0 }, 
    { "r",       0x52,          GTA_KEY_R,              DATA_NONE,              0 }, 
    { "s",       0x53,          GTA_KEY_S,              DATA_NONE,              0 }, 
    { "t",       0x54,          GTA_KEY_T,              DATA_NONE,              0 },
    { "u",       0x55,          GTA_KEY_U,              DATA_NONE,              0 },
    { "v",       0x56,          GTA_KEY_V,              DATA_NONE,              0 }, // 60
    { "w",       0x57,          GTA_KEY_W,              DATA_NONE,              0 }, 
    { "x",       0x58,          GTA_KEY_X,              DATA_NONE,              0 }, 
    { "y",       0x59,          GTA_KEY_Y,              DATA_NONE,              0 },
    { "z",       0x5A,          GTA_KEY_Z,              DATA_NONE,              0 },
    { "num_0",   0x60,          GTA_KEY_NUMPAD0,        DATA_NONE,              0 }, 
    { "num_1",   0x61,          GTA_KEY_NUMPAD1,        DATA_NONE,              0 }, 
    { "num_2",   0x62,          GTA_KEY_NUMPAD2,        DATA_NONE,              0 }, 
    { "num_3",   0x63,          GTA_KEY_NUMPAD3,        DATA_NONE,              0 }, 
    { "num_4",   0x64,          GTA_KEY_NUMPAD4,        DATA_NONE,              0 },
    { "num_5",   0x65,          GTA_KEY_NUMPAD5,        DATA_NONE,              0 }, // 70
    { "num_6",   0x66,          GTA_KEY_NUMPAD6,        DATA_NONE,              0 },
    { "num_7",   0x67,          GTA_KEY_NUMPAD7,        DATA_NONE,              0 },
    { "num_8",   0x68,          GTA_KEY_NUMPAD8,        DATA_NONE,              0 }, 
    { "num_9",   0x69,          GTA_KEY_NUMPAD9,        DATA_NONE,              0 },    
    { "num_mul", 0x6A,          GTA_KEY_MULTIPLY,       DATA_NONE,              0 },
    { "num_add", 0x6B,          GTA_KEY_ADD,            DATA_NONE,              0 },
    { "num_sep", 0x6C,          NO_KEY_DEFINED,         DATA_NONE,              0 },
    { "num_sub", 0x6D,          GTA_KEY_SUBTRACT,       DATA_NONE,              0 },
    { "num_dec", 0x6E,          GTA_KEY_DECIMAL,        DATA_NONE,              0 },
    { "num_div", 0x6F,          GTA_KEY_DIVIDE,         DATA_NONE,              0 }, // 80
    { "F1",      0x70,          GTA_KEY_F1,             DATA_NONE,              0,     true }, 
    { "F2",      0x71,          GTA_KEY_F2,             DATA_NONE,              0,     true }, 
    { "F3",      0x72,          GTA_KEY_F3,             DATA_NONE,              0,     true }, 
    { "F4",      0x73,          GTA_KEY_F4,             DATA_NONE,              0,     true }, 
    { "F5",      0x74,          GTA_KEY_F5,             DATA_NONE,              0,     true },
    { "F6",      0x75,          GTA_KEY_F6,             DATA_NONE,              0,     true }, 
    { "F7",      0x76,          GTA_KEY_F7,             DATA_NONE,              0,     true }, 
    //{ "F8",    0x77,          GTA_KEY_F8,             DATA_NONE },  * Used for console
    { "F9",      0x78,          GTA_KEY_F9,             DATA_NONE,              0,     true },
    { "F10",     0x79,          GTA_KEY_F10,            DATA_NONE,              0,     true },
    { "F11",     0x7A,          GTA_KEY_F11,            DATA_NONE,              0,     true }, // 90
    { "F12",     0x7B,          GTA_KEY_F12,            DATA_NONE,              0,     true },
    { "scroll",  0x91,          GTA_KEY_SCROLL,         DATA_NONE,              0,     true },
    { ";",       0xBA,          GTA_KEY_SEMICOLON,      DATA_NONE,              0 },
    { "=",       0xBB,          GTA_KEY_EQUALS,         DATA_NONE,              0 },
    { ",",       0xBC,          GTA_KEY_COMMA,          DATA_NONE,              0 },
    { "-",       0xBD,          GTA_KEY_MINUS,          DATA_NONE,              0 },
    { ".",       0xBE,          GTA_KEY_PERIOD,         DATA_NONE,              0 },
    { "/",       0xBF,          GTA_KEY_SLASH,          DATA_NONE,              0 },
    { "'",       0xC0,          GTA_KEY_APOSTROPHE,     DATA_NONE,              0 },
    { "[",       0xDB,          GTA_KEY_LBRACKET,       DATA_NONE,              0 }, // 100
    { "\\",      0xDC,          GTA_KEY_BACKSLASH,      DATA_NONE,              0 },
    { "]",       0xDD,          GTA_KEY_RBRACKET,       DATA_NONE,              0 },
    { "#",       0xDE,          GTA_KEY_HASH,           DATA_NONE,              0 },
    { "num_enter", 0x0D,        GTA_KEY_NUMPADENTER,    DATA_EXTENDED,          0 },
    { "clear",   0x0C,          NO_KEY_DEFINED,         DATA_NUMPAD,            70 },

    { "joy1",    VK_JOY(1),          GTA_KEY_JOY(1),            DATA_NONE,              0 },
    { "joy2",    VK_JOY(2),          GTA_KEY_JOY(2),            DATA_NONE,              0 },
    { "joy3",    VK_JOY(3),          GTA_KEY_JOY(3),            DATA_NONE,              0 },
    { "joy4",    VK_JOY(4),          GTA_KEY_JOY(4),            DATA_NONE,              0 },
    { "joy5",    VK_JOY(5),          GTA_KEY_JOY(5),            DATA_NONE,              0 },
    { "joy6",    VK_JOY(6),          GTA_KEY_JOY(6),            DATA_NONE,              0 },
    { "joy7",    VK_JOY(7),          GTA_KEY_JOY(7),            DATA_NONE,              0 },
    { "joy8",    VK_JOY(8),          GTA_KEY_JOY(8),            DATA_NONE,              0 },
    { "joy9",    VK_JOY(9),          GTA_KEY_JOY(9),            DATA_NONE,              0 },
    { "joy10",    VK_JOY(10),          GTA_KEY_JOY(10),            DATA_NONE,              0 },
    { "joy11",    VK_JOY(11),          GTA_KEY_JOY(11),            DATA_NONE,              0 },
    { "joy12",    VK_JOY(12),          GTA_KEY_JOY(12),            DATA_NONE,              0 },
    { "joy13",    VK_JOY(13),          GTA_KEY_JOY(13),            DATA_NONE,              0 },
    { "joy14",    VK_JOY(14),          GTA_KEY_JOY(14),            DATA_NONE,              0 },
    { "joy15",    VK_JOY(15),          GTA_KEY_JOY(15),            DATA_NONE,              0 },
    { "joy16",    VK_JOY(16),          GTA_KEY_JOY(16),            DATA_NONE,              0 },
    { "joy17",    VK_JOY(17),          GTA_KEY_JOY(17),            DATA_NONE,              0 },
    { "joy18",    VK_JOY(18),          GTA_KEY_JOY(18),            DATA_NONE,              0 },
    { "joy19",    VK_JOY(19),          GTA_KEY_JOY(19),            DATA_NONE,              0 },
    { "joy20",    VK_JOY(20),          GTA_KEY_JOY(20),            DATA_NONE,              0 },
    { "joy21",    VK_JOY(21),          GTA_KEY_JOY(21),            DATA_NONE,              0 },
    { "joy22",    VK_JOY(22),          GTA_KEY_JOY(22),            DATA_NONE,              0 },
    { "joy23",    VK_JOY(23),          GTA_KEY_JOY(23),            DATA_NONE,              0 },
    { "joy24",    VK_JOY(24),          GTA_KEY_JOY(24),            DATA_NONE,              0 },
    { "joy25",    VK_JOY(25),          GTA_KEY_JOY(25),            DATA_NONE,              0 },
    { "joy26",    VK_JOY(26),          GTA_KEY_JOY(26),            DATA_NONE,              0 },
    { "joy27",    VK_JOY(27),          GTA_KEY_JOY(27),            DATA_NONE,              0 },
    { "joy28",    VK_JOY(28),          GTA_KEY_JOY(28),            DATA_NONE,              0 },
    { "joy29",    VK_JOY(29),          GTA_KEY_JOY(29),            DATA_NONE,              0 },
    { "joy30",    VK_JOY(30),          GTA_KEY_JOY(30),            DATA_NONE,              0 },
    { "joy31",    VK_JOY(31),          GTA_KEY_JOY(31),            DATA_NONE,              0 },
    { "joy32",    VK_JOY(32),          GTA_KEY_JOY(32),            DATA_NONE,              0 },

    { "pov_up",     VK_POV(1),          NO_KEY_DEFINED,            DATA_NONE,              0 },
    { "pov_right",  VK_POV(2),          NO_KEY_DEFINED,            DATA_NONE,              0 },
    { "pov_down",   VK_POV(3),          NO_KEY_DEFINED,            DATA_NONE,              0 },
    { "pov_left",   VK_POV(4),          NO_KEY_DEFINED,            DATA_NONE,              0 },

    { "",           0,          NO_KEY_DEFINED,         DATA_NONE }
};

// Sync this with eBindableControl!
SBindableGTAControl g_bcControls[] =
{
    { "fire",                   FIRE,                   CONTROL_FOOT,    true, "Fire" },
    { "next_weapon",            NEXT_WEAPON,            CONTROL_FOOT,    true, "Next weapon" },
    { "previous_weapon",        PREVIOUS_WEAPON,        CONTROL_FOOT,    true, "Previous weapon" },
    { "forwards",               FORWARDS,               CONTROL_FOOT,    true, "Forwards" },
    { "backwards",              BACKWARDS,              CONTROL_FOOT,    true, "Backwards"},
    { "left",                   LEFT,                   CONTROL_FOOT,    true, "Left" },
    { "right",                  RIGHT,                  CONTROL_FOOT,    true, "Right" },
    { "zoom_in",                ZOOM_IN,                CONTROL_FOOT,    true, "Zoom in" },
    { "zoom_out",               ZOOM_OUT,               CONTROL_FOOT,    true, "Zoom out" },
    { "enter_exit",             ENTER_EXIT,             CONTROL_BOTH,    true, "Enter/Exit" },
    { "change_camera",          CHANGE_CAMERA,          CONTROL_BOTH,    true, "Change camera" }, // 10
    { "jump",                   JUMP,                   CONTROL_FOOT,    true, "Jump" },
    { "sprint",                 SPRINT,                 CONTROL_FOOT,    true, "Sprint" },
    { "look_behind",            LOOK_BEHIND,            CONTROL_FOOT,    true, "Look behind" },
    { "crouch",                 CROUCH,                 CONTROL_FOOT,    true, "Crouch" },
    { "action",                 ACTION,                 CONTROL_FOOT,    true, "Action" },
    { "walk",                   WALK,                   CONTROL_FOOT,    true, "Walk" },
    { "vehicle_fire",           VEHICLE_FIRE,           CONTROL_VEHICLE, true, "Vehicle fire" },
    { "vehicle_secondary_fire", VEHICLE_SECONDARY_FIRE, CONTROL_VEHICLE, true, "Vehicle secondary fire" },
    { "vehicle_left",           VEHICLE_LEFT,           CONTROL_VEHICLE, true, "Vehicle left" },
    { "vehicle_right",          VEHICLE_RIGHT,          CONTROL_VEHICLE, true, "Vehicle right" }, // 20
    { "steer_forward",          STEER_FORWARDS_DOWN,    CONTROL_VEHICLE, true, "Steer forwards/down" },
    { "steer_back",             STEER_BACK_UP,          CONTROL_VEHICLE, true, "Steer backwards/up" },
    { "accelerate",             ACCELERATE,             CONTROL_VEHICLE, true, "Accelerate" },
    { "brake_reverse",          BRAKE_REVERSE,          CONTROL_VEHICLE, true, "Brake/Reverse" },
    { "radio_next",             RADIO_NEXT,             CONTROL_VEHICLE, true, "Radio next" },
    { "radio_previous",         RADIO_PREVIOUS,         CONTROL_VEHICLE, true, "Radio previous" },
    { "radio_user_track_skip",  RADIO_USER_TRACK_SKIP,  CONTROL_VEHICLE, true, "Radio user track skip" },
    { "horn",                   HORN,                   CONTROL_VEHICLE, true, "Horn" },
    { "sub_mission",            SUB_MISSION,            CONTROL_VEHICLE, true, "Sub-mission" },
    { "handbrake",              HANDBRAKE,              CONTROL_VEHICLE, true, "Handbrake" }, // 30
    { "vehicle_look_left",      VEHICLE_LOOK_LEFT,      CONTROL_VEHICLE, true, "Vehicle look left" },
    { "vehicle_look_right",     VEHICLE_LOOK_RIGHT,     CONTROL_VEHICLE, true, "Vehicle look right" },
    { "vehicle_look_behind",    VEHICLE_LOOK_BEHIND,    CONTROL_VEHICLE, true, "Vehicle look behind" },
    { "vehicle_mouse_look",     VEHICLE_MOUSE_LOOK,     CONTROL_VEHICLE, true, "Vehicle mouse look" },
    { "special_control_left",   SPECIAL_CONTROL_LEFT,   CONTROL_VEHICLE, true, "Special control left" },
    { "special_control_right",  SPECIAL_CONTROL_RIGHT,  CONTROL_VEHICLE, true, "Special control right" },
    { "special_control_down",   SPECIAL_CONTROL_DOWN,   CONTROL_VEHICLE, true, "Special control down" },
    { "special_control_up",     SPECIAL_CONTROL_UP,     CONTROL_VEHICLE, true, "Special control up" },
    { "aim_weapon",             AIM_WEAPON,             CONTROL_FOOT,    true, "Aim weapon" },
    { "conversation_yes",       CONVERSATION_YES,       CONTROL_FOOT,    true, "Conversation yes" }, // 40
    { "conversation_no",        CONVERSATION_NO,        CONTROL_FOOT,    true, "Conversation no" },
    { "group_control_forwards", GROUP_CONTROL_FORWARDS, CONTROL_FOOT,    true, "Group control forwards" },
    { "group_control_back",     GROUP_CONTROL_BACK,     CONTROL_FOOT,    true, "Group control backwards" },

    { "", (eControllerAction)0, (eControlType)0, false, false }
};

SDefaultCommandBind g_dcbDefaultCommands[] = 
{ 
    { "g",       true,  "enter_passenger",  NULL },
    { "F12",     true,  "screenshot",       NULL },
    { "t",       true,  "chatbox",          "chatboxsay" },
    { "y",       true,  "chatbox",          "teamsay 255 0 0" },
    { "F11",     true,  "radar",            "-1" },
    { "num_add", true,  "radar_zoom_in",    NULL },
    { "num_sub", true,  "radar_zoom_out",   NULL },
    { "num_8",   true,  "radar_move_north", NULL },
    { "num_2",   true,  "radar_move_south", NULL },
    { "num_6",   true,  "radar_move_east",  NULL },
    { "num_4",   true,  "radar_move_west",  NULL },
    { "num_0",   true,  "radar_attach",     NULL },
    { "z",       true,  "voiceptt",         "1" },
    { "z",       false, "voiceptt",         "0" },
    { "pgup",    true,  "chatscrollup",     "1" },
    { "pgup",    false, "chatscrollup",     "0" },
    { "pgdn",    true,  "chatscrolldown",   "-1" },
    { "pgdn",    false, "chatscrolldown",   "0" },
    { "pgup",    true,  "debugscrollup",    "1" },
    { "pgup",    false, "debugscrollup",    "0" },
    { "pgdn",    true,  "debugscrolldown",  "-1" },
    { "pgdn",    false, "debugscrolldown",  "0" },

    { "", false, NULL, NULL }
};

// There has to be a more flexible solution than the garbage disposable technique applied in CKeyBinds.
// Making MTA:Lua a core component does not come into question; it is too dynamic. We will see about that.

#define NUM_GTA_CONTROLS ( sizeof(g_bcControls) / sizeof(*g_bcControls) - 1 )

static CGTAControlStatus g_controlStatus[NUM_GTA_CONTROLS];

static CGTAControlStatus* GetGTAControlStatus( SBindableGTAControl *ctrl )
{
    return &g_controlStatus[ctrl - g_bcControls];
}

// HACK: our current shift key states
bool bPreLeftShift = false, bPreRightShift = false;


CKeyBinds::CKeyBinds( CCore* pCore )
{
    m_pCore = pCore;

    m_szFileName = NULL;
    m_bMouseWheel = false;
    m_bFrameMouseWheelUp = false;
    m_bFrameMouseWheelDown = false;
    m_pChatBoxBind = NULL;
    m_bProcessingKeyStroke = false;
    m_KeyStrokeHandler = NULL;
    m_CharacterKeyHandler = NULL;
    m_bWaitingToLoadDefaults = false;

    m_currentControlScheme = CONTROL_FOOT;
}

CKeyBinds::~CKeyBinds()
{
    Clear();
}

bool CKeyBinds::ProcessMessage( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // Don't process Shift keys here, we have a hack for that
    if ( wParam == 0x10 && ( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) )
        return false;

    bool bState;
    const SBindableKey *pKey = GetBindableFromMessage( uMsg, wParam, lParam, bState );

    if ( pKey )
        return ProcessKeyStroke( pKey, bState );

    if ( uMsg == WM_CHAR )
        return ProcessCharacter( wParam );

    return false;
}

bool CKeyBinds::ProcessCharacter( WPARAM wChar )
{
    return m_CharacterKeyHandler && m_CharacterKeyHandler( wChar );
}

bool CKeyBinds::ProcessKeyStroke( const SBindableKey * pKey, bool bState )
{
    m_bProcessingKeyStroke = true;

    // If the console, chat input or menu is up, ignore any messages and unset
    // any already pressed
    static bool bInputGoesToGUI = false;
    static bool bIsCursorForced = false;

    if ( m_pCore->IsMenuVisible() || ( m_pCore->GetConsole()->IsVisible() || m_pCore->IsChatInputEnabled() ||
         m_pCore->GetGUI()->GetGUIInputEnabled() ) && !pKey->bIgnoredByGUI )
    {
        if ( !bInputGoesToGUI )
        {
            SetAllBindStates( false, KEY_BIND_COMMAND );
            SetAllBindStates( false, KEY_BIND_FUNCTION );
            bInputGoesToGUI = true;
        }
    }
    else
        bInputGoesToGUI = false;

    if ( m_pCore->IsCursorForcedVisible() )
    {
        if ( !bIsCursorForced )
        {
            if ( m_pCore->IsCursorControlsToggled() )
                SetAllControls( false );

            bIsCursorForced = true;
        }
    }
    else
        bIsCursorForced = false;

    int iKeyType = pKey->iGTARelative;

    if ( iKeyType == GTA_KEY_MSCROLLUP || iKeyType == GTA_KEY_MSCROLLDOWN )
        m_bMouseWheel = true;

    if ( iKeyType == GTA_KEY_MSCROLLUP )
        m_bFrameMouseWheelUp = true;
    else if ( iKeyType == GTA_KEY_MSCROLLDOWN )
        m_bFrameMouseWheelDown = true;

    // Call the key-stroke handler if we have one
    if ( m_KeyStrokeHandler )
        m_KeyStrokeHandler( pKey, bState );

    // Search through binds
    bool bFound = false;
    CKeyBind* pBind = NULL;
    cmdBinds_t processedList;

    binds_t cloneList = m_list;
    binds_t::const_iterator iter = cloneList.begin();

    for ( ; iter != cloneList.end(); ++iter )
    {
        pBind = *iter;

        if ( pBind->IsBeingDeleted() || !pBind->bActive || !pBind->boundKey )
            continue;

        // Does this bind's key match?
        if ( pBind->boundKey != pKey )
            continue;

        // Call the individual function for the bind
        switch( (*iter)->GetType() )
        {
        case KEY_BIND_GTA_CONTROL:
        {
            if ( !bState || ( !bInputGoesToGUI && ( !m_pCore->IsCursorForcedVisible() || !m_pCore->IsCursorControlsToggled() ) ) )
            {
                CallGTAControlBind( (CGTAControlBind*)*iter, bState );
                bFound = true;
            }
            break;
        }
        case KEY_BIND_COMMAND:
        case KEY_BIND_FUNCTION:
        {
            CKeyBindWithState* pBind = static_cast < CKeyBindWithState* > ( *iter );
            
            // If it wasnt already pressed or released
            if ( pBind->bState == bState )
                break;

            // Set its new state
            pBind->bState = bState;

            bFound = true;

            // Does it match our up/down state?
            if ( bState != pBind->bHitState )
                break;

            switch( pBind->GetType() )
            {
            case KEY_BIND_COMMAND:
            {
                if ( bInputGoesToGUI )
                    break;

                CCommandBind *pCommandBind = (CCommandBind*)pBind;

                // HACK: call chatbox commands on the next frame to stop a translated WM_CHAR key to pop up                                            
                if ( pCommandBind->m_cmd == "chatbox" )
                    m_pChatBoxBind = pCommandBind;
                else
                {
                    cmdBinds_t::iterator iter = processedList.begin();

                    for ( ; iter != processedList.end(); ++iter )
                    {
                        if ( pCommandBind->m_cmd == (*iter)->m_cmd &&
                             (*iter)->bHitState == pCommandBind->bHitState &&
                             pCommandBind->m_args == (*iter)->m_args
                           )
                        {
                            goto alreadyProcessed;
                        }
                    }

                    //don't fire if its already fired
                    Call( pCommandBind );
                    processedList.push_back( pCommandBind );
                }
alreadyProcessed:
                break;
            }
            case KEY_BIND_FUNCTION:
            {
                CKeyFunctionBind *pFunctionBind = (CKeyFunctionBind*)pBind;

                if ( !bInputGoesToGUI || pFunctionBind->bIgnoreGUI )
                    pFunctionBind->Handler( pFunctionBind );

                break;
            }
            }
            break;
        }
        }
    }

    m_bProcessingKeyStroke = false;
    RemoveDeletedBinds();
    return bFound;
}

void CKeyBinds::Add( CKeyBind* pKeyBind )
{
    m_list.push_back( pKeyBind );
}

void CKeyBinds::Remove ( CKeyBind* pKeyBind )
{
    if ( m_bProcessingKeyStroke )
        pKeyBind->beingDeleted = true;
    else
    {
        m_list.remove( pKeyBind );
        delete pKeyBind;
    }
}

void CKeyBinds::Clear()
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
        delete *iter;

    m_list.clear();
}

void CKeyBinds::RemoveDeletedBinds()
{
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( (*iter)->IsBeingDeleted() )
        {
            delete *iter;
            iter = m_list.erase( iter );
        }
        else
            ++iter;
    }
}

void CKeyBinds::ClearCommandsAndControls()
{
    binds_t keyList;
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == KEY_BIND_FUNCTION || (*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION )
            keyList.push_back( *iter );
        else
            delete *iter;
    }

    m_list = keyList;
}

bool CKeyBinds::Call( CKeyBind *pKeyBind )
{
    switch( pKeyBind->GetType() )
    {
    case KEY_BIND_COMMAND:
    {
        CCommandBind *pBind = (CCommandBind*)pKeyBind;
        if ( pBind->bActive )
            m_pCore->GetCommands()->Execute( pBind->m_cmd.c_str(), pBind->m_args.c_str() );
        return true;
    }
    case KEY_BIND_FUNCTION:
    {
        CKeyFunctionBind *pBind = (CKeyFunctionBind*)pKeyBind;
        if ( pBind->Handler )
            pBind->Handler( pBind );
        return true;
    }
    case KEY_BIND_CONTROL_FUNCTION:
    {
        CControlFunctionBind *pBind = (CControlFunctionBind*)pKeyBind;
        if ( pBind->Handler )
            pBind->Handler( pBind );
        return true;
    }
    }
    return false;
}

bool CKeyBinds::AddCommand( const char *key, const char *cmd, const char *args, bool state, const char *res, bool altKey )
{
    const SBindableKey *boundKey = GetBindableFromKey( key );
    
    if ( !boundKey )
        return false;

    CCommandBind *bind = new CCommandBind( boundKey, cmd, args, res, altKey );

    bind->bHitState = state;
    m_list.push_back( bind );
    return true;
}

bool CKeyBinds::AddCommand( const SBindableKey *key, const char *cmd, const char *args, bool state )
{
    CCommandBind *bind = new CCommandBind( key, cmd, args, NULL, false );

    bind->bHitState = state;
    m_list.push_back( bind );
    return true;
}

bool CKeyBinds::RemoveCommand( const char *key, const char *cmd, bool checkState, bool state )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_COMMAND )
        {
            CCommandBind *pBind = (CCommandBind*)*iter;

            if ( stricmp( key, pBind->boundKey->szKey ) == 0 && pBind->m_cmd == cmd &&
                ( !checkState || pBind->bHitState == state ) && !pBind->m_res )
            {
                bFound = true;

                if ( m_bProcessingKeyStroke )
                    pBind->beingDeleted = true;
                else
                {
                    delete pBind;
                    iter = m_list.erase( iter );
                    continue;
                }
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllCommands( const char *key, bool checkState, bool state )
{
    bool bFound = false;
    binds_t cloneList = m_list;
    binds_t::iterator iter = cloneList.begin();

    for ( ; iter != cloneList.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_COMMAND )
            continue;

        CCommandBind *pBind = (CCommandBind*)*iter;

        if ( stricmp( key, pBind->boundKey->szKey ) != 0 || checkState && pBind->bHitState != state )
            continue;

        Remove( *iter );
        bFound = true;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllCommands()
{
    bool bFound = false;
    binds_t cloneList = m_list;
    binds_t::iterator iter = cloneList.begin();

    for ( ; iter != cloneList.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_COMMAND )
            continue;

        Remove( *iter );
        bFound = true;
    }

    return bFound;
}

CCommandBind* CKeyBinds::GetCommandBind( const char *key, const char *cmd, bool checkState, bool state, const char *args, const char *res )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_COMMAND )
            continue;

        CCommandBind *pBind = (CCommandBind*)*iter;

        if ( key && stricmp( pBind->boundKey->szKey, key ) != 0 || pBind->m_cmd != cmd ||
            checkState && pBind->bHitState != state ||
            args && pBind->m_args != args ||
            res && ( !pBind->m_res || *pBind->m_res != res ) )
            continue;

        return pBind;
    }
    
    return NULL;
}

bool CKeyBinds::SetCommandActive( const char *key, const char *cmd, bool state, const char *args, const char *res, bool active, bool checkState )
{
    CCommandBind *bind = GetCommandBind( key, cmd, checkState, state, args, res );

    if ( !bind )
        return false;

    bind->bActive = active;
    return true;
}

// TODO
void CKeyBinds::SetAllCommandsActive ( const char* szResource, bool bActive, const char* szCommand, bool bState, const char* szArguments, bool checkHitState )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType () == KEY_BIND_COMMAND )
        {
            CCommandBind *pBind = static_cast < CCommandBind* > ( *iter );
            if ( !szResource || pBind->m_res && *pBind->m_res == szResource )
            {
                if ( !szCommand || pBind->m_cmd == szCommand )
                {
                    if ( !checkHitState || ( pBind->bHitState == bState ) )
                    {
                        if ( !szArguments || pBind->m_args == szArguments )
                            pBind->bActive = bActive;
                    }
                }
            }
        }
    }  
}

// TODO
CCommandBind* CKeyBinds::GetBindFromCommand( const char* szCommand, const char* szArguments, bool bMatchCase, const char* szKey, bool bCheckHitState, bool bState )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == KEY_BIND_COMMAND )
        {
            CCommandBind* pBind = (CCommandBind*)*iter;
            const char* szBindCommand = pBind->m_cmd.c_str();
            const char* szBindArguments = pBind->m_args.c_str();

            if ( ( bMatchCase && strcmp ( szBindCommand, szCommand ) == 0 ) ||
                 ( !bMatchCase && stricmp ( szBindCommand, szCommand ) == 0 ) )
            {
                if ( szArguments == NULL || ( szBindArguments &&
                             ( bMatchCase && strcmp ( szBindArguments, szArguments ) == 0 ) ||
                             ( !bMatchCase && stricmp ( szBindArguments, szArguments ) == 0 ) ) )
                {
                    if ( ( szKey == NULL ) ||
                         ( stricmp ( pBind->boundKey->szKey, szKey ) == 0 ) )
                    {
                        if ( ( !bCheckHitState ) ||
                             ( bState == pBind->bHitState ) )
                        {
                            return pBind;
                        }
                    }
                }
            }
        }
    }

    return NULL;
}        

void CKeyBinds::ForAllBoundCommands( const char *cmd, cmdIterCallback_t cb, void *ud )
{
    binds_t::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_COMMAND )
            continue;

        CCommandBind *pBind = (CCommandBind*)*iter;

        if ( pBind->m_cmd == cmd )
            cb( pBind, ud );
    }
}

void CKeyBinds::AddGTAControl( const char *szKey, const char *szControl )
{
    const SBindableKey *boundKey = GetBindableFromKey( szKey );
    SBindableGTAControl *boundControl = GetBindableFromControl( szControl );

    AddGTAControl( boundKey, boundControl, GetGTAControlStatus( boundControl ) );
}

bool CKeyBinds::AddGTAControl( const char *szKey, eControllerAction action )
{
    const char *szControl = GetControlFromAction( action );

    if ( szControl )
        return false;

    const SBindableKey *boundKey = GetBindableFromKey ( szKey );
    SBindableGTAControl *boundControl = GetBindableFromControl ( szControl );

    AddGTAControl( boundKey, boundControl, GetGTAControlStatus( boundControl ) );
    return true;
}

void CKeyBinds::AddGTAControl( const SBindableKey *key, SBindableGTAControl *control )
{
    AddGTAControl( key, control, GetGTAControlStatus( control ) );
}

void CKeyBinds::AddGTAControl( const SBindableKey *key, SBindableGTAControl *control, CGTAControlStatus *status )
{
    m_list.push_back( new CGTAControlBind( key, control, status ) );
}

bool CKeyBinds::RemoveGTAControl( const char *key, const char *control )
{
    binds_t::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->IsBeingDeleted() || (*iter)->GetType() != KEY_BIND_GTA_CONTROL )
            continue;

        CGTAControlBind *pBind = (CGTAControlBind*)*iter;

        if ( stricmp( key, pBind->boundKey->szKey ) != 0 || strcmp( control, pBind->control->szControl ) != 0 )
            continue;

        delete pBind;
        m_list.erase( iter );
        return true;
    }

    return false;
}

bool CKeyBinds::RemoveAllGTAControls( const char *key )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( (*iter)->GetType() == KEY_BIND_GTA_CONTROL )
        {
            CGTAControlBind *pBind = (CGTAControlBind*)*iter;
            if ( stricmp( key, pBind->boundKey->szKey ) == 0 )
            {
                delete pBind;
                iter = m_list.erase( iter );
                bFound = true;
                continue;
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllGTAControls()
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( (*iter)->GetType() == KEY_BIND_GTA_CONTROL )
        {
            delete *iter;
            iter = m_list.erase( iter );
            bFound = true;
        }
        else
            ++iter;
    }

    return bFound;
}

bool CKeyBinds::GTAControlExists( const char *key, const char *control )
{
    const SBindableKey *keyInfo = GetBindableFromKey( key );
    SBindableGTAControl *controlInfo = GetBindableFromControl( control );

    if ( !keyInfo || !controlInfo )
        return false;

    return GTAControlExists( keyInfo, controlInfo );
}

bool CKeyBinds::GTAControlExists( const SBindableKey* pKey, SBindableGTAControl* pControl )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == KEY_BIND_GTA_CONTROL )
        {
            CGTAControlBind* pBind = (CGTAControlBind*)*iter;

            if ( pBind->boundKey == pKey && pBind->control == pControl )
                return true;
        }
    }
    
    return false;
}

void CKeyBinds::CallGTAControlBind( CGTAControlBind* pBind, bool bState )
{
    // HACK: temp fix for binds using mouse_wheel for zooming (the game seems to still do this itself)
    if ( m_bMouseWheel && ( pBind->control->action == ZOOM_IN || pBind->control->action == ZOOM_OUT ) )
        return;

    // Don't allow custom track skips. Crashes for some reason. (TODO: why?)
    if ( pBind->control->action == RADIO_USER_TRACK_SKIP )
        return;

    // Check if the state changed
    if ( pBind->bState != bState && pBind->control->bEnabled )
    {
        // Update the bind's private info
        pBind->bState = bState;

        // Reference or dereference depending on state
        if ( bState )
            pBind->status->AddRef();
        else
            pBind->status->RemoveRef();
    }

    // If its keydown, or there isnt another bind for this control down
    if ( bState || !GetMultiGTAControlState( pBind ) )
    {
        // Do we have a function bound to this control?
        binds_t::const_iterator iter = m_list.begin();

        for ( ; iter != m_list.end(); ++iter )
        {
            if ( (*iter)->GetType() != KEY_BIND_CONTROL_FUNCTION )
                continue;

            CControlFunctionBind *pControlBind = (CControlFunctionBind*)*iter;
            
            // If its bound to this control
            if ( pControlBind->control == pBind->control )
            {
                // If its a key up or our player's state matches the control
                eControlType controlType = pControlBind->control->controlType;
                if ( !bState || IsActiveControlScheme( controlType ) )
                {
                    // Is the up / down state matching
                    if ( pControlBind->bHitState == bState )
                    {
                        if ( pControlBind->bState != bState )
                            Call( pControlBind );
                    }
                    // Set its state if it was called or not
                    pControlBind->bState = bState;
                }
            }
        }
    }
}

void CKeyBinds::CallAllGTAControlBinds( eControlType controlType, bool bState )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_GTA_CONTROL )
            continue;

        CGTAControlBind *pBind = (CGTAControlBind*)*iter;

        if ( controlType == CONTROL_BOTH || pBind->control->controlType == controlType )
            CallGTAControlBind ( pBind, bState );
    }
}

bool CKeyBinds::GetMultiGTAControlState( CGTAControlBind *pBind )
{
    eControllerAction action = pBind->control->action;
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_GTA_CONTROL )
            continue;

        CGTAControlBind *pTemp = (CGTAControlBind*)*iter;

        if ( pTemp->control->action == action &&
             pTemp == pBind &&
             pTemp->bState )
            return true;
    }

    return false;
}

bool CKeyBinds::IsControlEnabled( const char *control ) const
{
    SBindableGTAControl *controlInfo = GetBindableFromControl( control );
    COMMANDENTRY *cmd;
    
    return controlInfo && controlInfo->bEnabled ||
        ( cmd = m_pCore->GetCommands()->Get( control, true, true ) ) && cmd->bEnabled;
}

bool CKeyBinds::IsControlEnabled( eBindableControl index ) const
{
    if ( index > MAX_CONTROLS-1 )
        return false;

    return g_bcControls[index].bEnabled;
}

bool CKeyBinds::SetControlEnabled ( const char* szControl, bool bEnabled )
{
    SBindableGTAControl* pControl = GetBindableFromControl ( szControl );
    if ( pControl )
    {
        if ( pControl->bEnabled != bEnabled )
        {
            pControl->bEnabled = bEnabled;

            if ( bEnabled )
                ResetGTAControlState ( pControl );                
            else
                GetGTAControlStatus( pControl )->Reset();

            return true;
        }
    }

    COMMANDENTRY* pCommand = m_pCore->GetCommands ()->Get ( szControl, true, true );
    if ( pCommand )
    {
        pCommand->bEnabled = bEnabled;
        return true;
    }

    return false;
}

void CKeyBinds::SetAllControlsEnabled ( bool bGameControls, bool bMTAControls, bool bEnabled )
{
    if ( bGameControls )
    {
        for ( int i = 0 ; *g_bcControls [ i ].szControl != NULL ; i++ )
        {
            SBindableGTAControl& temp = g_bcControls [ i ];

            temp.bEnabled = bEnabled;

            if ( !bEnabled )
                GetGTAControlStatus( &temp )->Reset();
        }
        if ( bEnabled )
            ResetAllGTAControlStates ();
    }

    if ( bMTAControls )
    {
        list < COMMANDENTRY* > ::iterator iter = m_pCore->GetCommands ()->IterBegin ();
        for ( ; iter != m_pCore->GetCommands ()->IterEnd () ; ++iter )
        {
            if ( (*iter)->bModCommand )
                (*iter)->bEnabled = bEnabled;
        }
    }
}

void CKeyBinds::ResetGTAControlState( SBindableGTAControl *pControl )
{
    CGTAControlStatus *status = GetGTAControlStatus( pControl );

    // Reset it
    status->Reset();

    // Check if we have a bind pressed for this control
    binds_t::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() != KEY_BIND_GTA_CONTROL )
            continue;

        CGTAControlBind* pBind = (CGTAControlBind*)*iter;

        if ( pBind->control == pControl )
        {
            if ( pBind->bState )
            {
                // We are referenced
                status->AddRef();
            }
        }
    }
}

void CKeyBinds::ResetAllGTAControlStates()
{
    // Set all our control states to false
    for ( unsigned int i = 0; i < NUM_GTA_CONTROLS; i++ )
        g_controlStatus[i].Reset();

    // Go through all our control binds, if they're state is true, turn their control on
    binds_t::iterator iter = m_list.begin();
    
    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == KEY_BIND_GTA_CONTROL )
        {
            CGTAControlBind *pBind = (CGTAControlBind*)*iter;

            if ( pBind->bState )
                pBind->status->AddRef();
        }
    }
}

void CKeyBinds::ForAllBoundControls( SBindableGTAControl *control, cntrlIterCallback_t cb, void *ud )
{
    binds_t::iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == KEY_BIND_GTA_CONTROL )
        {
            CGTAControlBind* pBind = (CGTAControlBind*)*iter;

            if ( pBind->control == control )
                cb( pBind, ud );
        }
    }
}

void CKeyBinds::SwitchToControlScheme( eControlType controlType )
{
    m_currentControlScheme = controlType;
}

bool CKeyBinds::IsActiveControlScheme( eControlType controlType ) const
{
    return controlType == CONTROL_BOTH || m_currentControlScheme == controlType;
}

bool CKeyBinds::AddFunction( const char* szKey, KeyFunctionBindHandler Handler, bool bState, bool bIgnoreGUI )
{
    const SBindableKey *boundKey = GetBindableFromKey( szKey );

    if ( !boundKey )
        return false;
    
    AddFunction( boundKey, Handler, bState, bIgnoreGUI );
    return true;
}

void CKeyBinds::AddFunction( const SBindableKey *key, KeyFunctionBindHandler handler, bool state, bool ignoreGUI )
{   
    CKeyFunctionBind *bind = new CKeyFunctionBind( key, handler, ignoreGUI );
    bind->bHitState = state;
    m_list.push_back( bind );
}

bool CKeyBinds::RemoveFunction( const char *key, KeyFunctionBindHandler handler, bool checkState, bool state )
{
    const SBindableKey *keyInfo = GetBindableFromKey( key );

    return keyInfo != NULL && RemoveFunction( keyInfo, handler, checkState, state );
}

bool CKeyBinds::RemoveFunction( const SBindableKey *key, KeyFunctionBindHandler handler, bool checkState, bool state )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_FUNCTION )
        {
            CKeyFunctionBind *pBind = (CKeyFunctionBind*)*iter;
            
            if ( pBind->Handler == handler && key == pBind->boundKey && ( !checkState || pBind->bHitState == state ) )
            {
                if ( m_bProcessingKeyStroke )
                    pBind->beingDeleted = true;
                else
                {
                    delete pBind;
                    iter = m_list.erase( iter );
                    continue;
                }
                bFound = true;
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllFunctions( KeyFunctionBindHandler Handler )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_FUNCTION )
        {
            CKeyFunctionBind *pBind = (CKeyFunctionBind*)*iter;
            if ( pBind->Handler == Handler )
            {
                if ( m_bProcessingKeyStroke )
                    pBind->beingDeleted = true;
                else
                {
                    delete pBind;
                    iter = m_list.erase( iter );
                    continue;
                }
                bFound = true;
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllFunctions()
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_FUNCTION )
        {
            if ( m_bProcessingKeyStroke )
                (*iter)->beingDeleted = true;
            else
            {
                delete *iter;
                iter = m_list.erase( iter );
                continue;
            }
            bFound = true;
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::FunctionExists( const char *key, KeyFunctionBindHandler handler, bool checkState, bool state )
{
    const SBindableKey *keyInfo = GetBindableFromKey( key );

    return keyInfo && FunctionExists( keyInfo, handler, checkState, state );
}

bool CKeyBinds::FunctionExists( const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState, bool bState )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_FUNCTION )
        {
            CKeyFunctionBind *pBind = (CKeyFunctionBind*)*iter;
            if ( !Handler || pBind->Handler == Handler )
            {
                if ( pBind->boundKey == pKey )
                {
                    if ( !bCheckState || pBind->bHitState == bState )
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CKeyBinds::AddControlFunction( const char *control, ControlFunctionBindHandler handler, bool state )
{
    SBindableGTAControl *boundControl = GetBindableFromControl( control );
    
    if ( !boundControl )
        return false;

    AddControlFunction( boundControl, handler, state );
    return true;
}

void CKeyBinds::AddControlFunction( SBindableGTAControl *control, ControlFunctionBindHandler handler, bool state )
{
    CControlFunctionBind *bind = new CControlFunctionBind( NULL, control, handler );
    bind->bHitState = state;
    m_list.push_back( bind );
}

bool CKeyBinds::RemoveControlFunction( const char *control, ControlFunctionBindHandler handler, bool checkState, bool state )
{
    SBindableGTAControl *ctrlInfo = GetBindableFromControl( control );

    return ctrlInfo && RemoveControlFunction( ctrlInfo, handler, checkState, state );
}

bool CKeyBinds::RemoveControlFunction( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();
    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION )
        {
            CControlFunctionBind *pBind = (CControlFunctionBind*)*iter;
            if ( pBind->Handler == Handler )
            {
                if ( pControl == pBind->control )
                {                
                    if ( !bCheckState || pBind->bHitState == bState )
                    {
                        if ( m_bProcessingKeyStroke )
                            pBind->beingDeleted = true;
                        else
                        {
                            delete pBind;
                            iter = m_list.erase( iter );
                            continue;
                        }
                        bFound = true;
                    }
                }
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllControlFunctions( ControlFunctionBindHandler Handler )
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION )
        {
            CControlFunctionBind *pBind = (CControlFunctionBind*)*iter;

            if ( pBind->Handler == Handler )
            {
                if ( m_bProcessingKeyStroke )
                    pBind->beingDeleted = true;
                else
                {
                    delete pBind;
                    iter = m_list.erase( iter );
                    continue;
                }
                bFound = true;
            }
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::RemoveAllControlFunctions()
{
    bool bFound = false;
    binds_t::iterator iter = m_list.begin();

    while ( iter != m_list.end() )
    {
        if ( !(*iter)->IsBeingDeleted() && (*iter)->GetType() == KEY_BIND_CONTROL_FUNCTION )
        {
            if ( m_bProcessingKeyStroke )
                (*iter)->beingDeleted = true;
            else
            {
                delete *iter;
                iter = m_list.erase( iter );
                continue;
            }
            bFound = true;
        }
        ++iter;
    }

    return bFound;
}

bool CKeyBinds::ControlFunctionExists( const char *control, ControlFunctionBindHandler handler, bool checkState, bool state )
{
    SBindableGTAControl *ctrlInfo = GetBindableFromControl( control );

    return ctrlInfo && ControlFunctionExists( ctrlInfo, handler, checkState, state );
}

bool CKeyBinds::ControlFunctionExists( SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState, bool bState )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->IsBeingDeleted() || (*iter)->GetType() != KEY_BIND_CONTROL_FUNCTION )
            continue;

        CControlFunctionBind *pBind = (CControlFunctionBind*)*iter;

        if ( pBind->Handler != Handler || pBind->control != pControl || bCheckState && pBind->bHitState != bState )
            continue;
        
        return true;
    }
    
    return false;
}

const char* CKeyBinds::GetKeyFromCode( unsigned long ulCode )
{
    for ( unsigned int i = 0; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey *temp = &g_bkKeys[i];

        if ( temp->ulCode == ulCode )
            return temp->szKey;
    }

    return NULL;
}

bool CKeyBinds::GetCodeFromKey( const char *szKey, unsigned long& ulCode )
{
    for ( unsigned int i = 0 ; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey* temp = &g_bkKeys[i];

        if ( stricmp( temp->szKey, szKey ) == 0 )
        {
            ulCode = temp->ulCode;
            return true;
        }
    }

    return false;
}

const SBindableKey* CKeyBinds::GetBindableFromKey( const char *szKey )
{
    for ( unsigned int i = 0; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey *temp = &g_bkKeys[i];

        if ( !stricmp( temp->szKey, szKey ) )
            return temp;
    }

    return NULL;
}

SBindableGTAControl* CKeyBinds::GetBindableFromAction( eControllerAction action )
{
    for ( unsigned int i = 0; *g_bcControls[i].szControl != 0; i++ )
    {
        SBindableGTAControl *temp = &g_bcControls[i];

        if ( temp->action == action )
            return temp;
    }

    return NULL;
}

SBindableGTAControl* CKeyBinds::GetBindable( eBindableControl cntrl )
{
    if ( cntrl > MAX_CONTROLS-1 || cntrl < 0 )
        return NULL;

    return &g_bcControls[cntrl];
}

bool CKeyBinds::GetBindableIndex( const std::string& name, eBindableControl& cntrl )
{
    for ( unsigned int n=0; g_bcControls[n].szControl[0] != NULL; n++ )
    {
        if ( name == g_bcControls[n].szControl )
        {
            cntrl = (eBindableControl)n;
            return true;
        }
    }
    return false;
}

bool CKeyBinds::IsKey( const char *szKey )
{
    for ( unsigned int i = 0 ; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey *temp = &g_bkKeys[i];

        if ( !stricmp( temp->szKey, szKey ) )
            return true;
    }

    return false;
}

const char* CKeyBinds::GetKeyFromGTARelative( int iGTAKey )
{
    for ( unsigned int i = 0; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey *temp = &g_bkKeys[i];

        if ( temp->iGTARelative == iGTAKey )
            return temp->szKey;
    }
    
    return NULL;
}

const SBindableKey* CKeyBinds::GetBindableFromMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState )
{
    if ( uMsg != WM_KEYDOWN && uMsg != WM_KEYUP && uMsg != WM_SYSKEYDOWN &&
         uMsg != WM_SYSKEYUP && uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONUP &&
         uMsg != WM_RBUTTONDOWN && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONDOWN &&
         uMsg != WM_MBUTTONUP && uMsg != WM_XBUTTONDOWN && uMsg != WM_XBUTTONUP &&
         uMsg != WM_MOUSEWHEEL )
        return NULL;

    // Prevents a simulated lctrl with Alt Gr keys
    if ( IsFakeCtrl_L ( uMsg, wParam, lParam ) )
        return NULL;

    bool bFirstHit = ( lParam & 0x40000000 ) ? false:true;
    if ( uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP || uMsg == WM_XBUTTONDOWN ||
         uMsg == WM_XBUTTONUP || uMsg == WM_MOUSEWHEEL )
         bFirstHit = true;

    bool bExtended = ( lParam & 0x1000000 ) ? true:false;
    bool bNumLock = ( GetKeyState ( VK_NUMLOCK ) & 0x1 ) ? true:false;

    bState = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_LBUTTONDOWN ||
                       uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN ||
                       uMsg == WM_MOUSEWHEEL || uMsg == WM_XBUTTONDOWN );

    for ( int i = 0 ; *g_bkKeys [ i ].szKey != NULL ; i++ )
    {
        SBindableKey* bindable = &g_bkKeys [ i ];

        unsigned long ulCode = bindable->ulCode;
        char* szKey = bindable->szKey;
        eKeyData keyData = bindable->data;

        bool bMouseWheel = false;
        if ( ulCode == 0 && uMsg == WM_MOUSEWHEEL )
        {
            bool bMouseWheelUp = ( ( short ) HIWORD ( wParam ) > 0 );
            if ( ( bMouseWheelUp && bindable->iGTARelative == GTA_KEY_MSCROLLUP ) ||
                 ( !bMouseWheelUp && bindable->iGTARelative == GTA_KEY_MSCROLLDOWN ) )
            {
                bMouseWheel = true;
            }
        }

        bool bMouseXButton = false;
        if ( ulCode == 0 && ( uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP ) )
        {
            bool bMouseX1 = ( HIWORD ( wParam ) == XBUTTON1 );
            if ( ( bMouseX1 && bindable->iGTARelative == GTA_KEY_MXB1 ) ||
                 ( !bMouseX1 && bindable->iGTARelative == GTA_KEY_MXB2 ) )
            {
                bMouseXButton = true;
            }
        }

        if ( ( ulCode == wParam && ( uMsg == WM_KEYDOWN || uMsg == WM_KEYUP ||
                                     uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) ) ||
             ( ulCode == 0x01 && ( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP ) ) ||
             ( ulCode == 0x02 && ( uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP ) ) ||
             ( ulCode == 0x04 && ( uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP ) ) ||
             ( bMouseWheel ) ||
             ( bMouseXButton ) )             
        {           
            // Does it match the extended value?
            if ( keyData == DATA_NONE || keyData == DATA_NUMPAD || ( ( ( keyData == DATA_EXTENDED ) ? true:false ) == bExtended ) )
            {
                // If its the first hit, or key up
                if ( !bState || bFirstHit )
                {
                    // Is this a shift key
                    if ( bindable->iGTARelative == GTA_KEY_LSHIFT )
                    {
                        // If our current rshift state is opposite to bState
                        if ( bPreRightShift != bState )
                        {
                            // If our actual rshift state is the same as bState
                            if ( ( ( GetAsyncKeyState ( VK_RSHIFT ) & 0x8000 ) ? true:false ) == bState )
                            {
                                // Return the right-shift bindable instead
                                return &g_bkKeys [ 10 ];
                            }
                        }
                    }

                    // Is num-lock off and breaking our key-codes?
                    if ( !bNumLock && !bExtended && bindable->data == DATA_NUMPAD )
                    {
                        // Grab the correct key
                        if ( !bindable->ucNumpadRelative )                  
                            return NULL;

                        //m_pCore->GetConsole ()->Printf ( "returned: %s", g_bkKeys [ bindable->ucNumpadRelative ].szKey );
                        return &g_bkKeys [ bindable->ucNumpadRelative ];
                    }

                    return bindable;
                }
            }
        }
    }
    return NULL;
}

void CKeyBinds::SetRealControlState( eBindableControl control, bool state )
{
    if ( control > MAX_CONTROLS-1 )
        return;

    g_controlStatus[control].SetState( state );
}

bool CKeyBinds::GetRealControlState( eBindableControl control ) const
{
    if ( control > MAX_CONTROLS-1 )
        return false;

    return g_controlStatus[control].IsActive();
}

bool CKeyBinds::GetControlState( eBindableControl control ) const
{
    eControlType cntrlType = g_bcControls[control].controlType;

    if ( IsActiveControlScheme( cntrlType ) )
    {
        if ( control == RADIO_NEXT )
            return m_bFrameMouseWheelUp;
        if ( control == RADIO_PREVIOUS )
            return m_bFrameMouseWheelDown;

        return GetRealControlState( control );
    }

    return false;
}

const char* CKeyBinds::GetControlFromAction( eControllerAction action )
{
    for ( unsigned int i = 0; *g_bcControls[i].szControl != NULL; i++ )
    {
        SBindableGTAControl *temp = &g_bcControls[i];

        if ( temp->action == action )
            return temp->szControl;
    }

    return NULL;
}

bool CKeyBinds::GetActionFromControl( const char *control, eControllerAction& action )
{
    for ( unsigned int i = 0; *g_bcControls[i].szControl != NULL; i++ )
    {
        SBindableGTAControl *temp = &g_bcControls[i];

        if ( stricmp( temp->szControl, control ) == 0 )
        {
            action = temp->action;
            return true;
        }
    }

    return false;
}

SBindableGTAControl* CKeyBinds::GetBindableFromControl( const char *szControl ) const
{
    for ( unsigned int i = 0; *g_bcControls[i].szControl != NULL; i++ )
    {
        SBindableGTAControl *temp = &g_bcControls[i];

        if ( !stricmp( temp->szControl, szControl ) )
            return temp;
    }

    return NULL;
}

const SBindableKey* CKeyBinds::GetBindableFromGTARelative( int iGTAKey )
{
    for ( unsigned int i = 0; *g_bkKeys[i].szKey != NULL; i++ )
    {
        SBindableKey *temp = &g_bkKeys[i];

        if ( temp->iGTARelative == iGTAKey )
            return temp;
    }

    return NULL;
}

bool CKeyBinds::IsControl ( const char* szControl )
{
    for ( int i = 0 ; *g_bcControls [ i ].szControl != NULL ; i++ )
    {
        SBindableGTAControl* temp = &g_bcControls [ i ];
        if ( !strcmp ( temp->szControl, szControl ) )
        {
            return true;
        }
    }

    return false;
}

void CKeyBinds::SetAllControls ( bool bState )
{
    SetAllFootControls ( bState );
    SetAllVehicleControls ( bState );
}

void CKeyBinds::SetAllFootControls( bool bState )
{
    for ( unsigned int i = 0; i < NUM_GTA_CONTROLS; i++ )
    {
        CGTAControlStatus& status = g_controlStatus[i];
        SBindableGTAControl& temp = g_bcControls[i];

        if ( temp.controlType == CONTROL_FOOT )
            status.SetState( bState );
    }
}

void CKeyBinds::SetAllVehicleControls( bool bState )
{
    for ( int i = 0 ; *g_bcControls [ i ].szControl != NULL ; i++ )
    {
        CGTAControlStatus& status = g_controlStatus[i];
        SBindableGTAControl& temp = g_bcControls[i];

        if ( temp.controlType == CONTROL_VEHICLE )
            status.SetState( bState );
    }
}

void CKeyBinds::SetAllBindStates ( bool bState, eKeyBindType onlyType )
{
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        eKeyBindType bindType = (*iter)->GetType();

        if ( onlyType == KEY_BIND_UNDEFINED || bindType == onlyType )
        {
            if ( bindType == KEY_BIND_COMMAND || bindType == KEY_BIND_FUNCTION )
            {
                CKeyBindWithState* pBind = (CKeyBindWithState*)*iter;

                if ( pBind->bState != bState )
                {
                    if ( pBind->bHitState == bState )
                        Call ( pBind );

                    pBind->bState = bState;
                }
            }
            else if ( bindType == KEY_BIND_GTA_CONTROL )
                CallGTAControlBind( (CGTAControlBind*)*iter, bState );
        }
    }
}

unsigned int CKeyBinds::Count( eKeyBindType bindType )
{
    if ( bindType == KEY_BIND_UNDEFINED )
        return m_list.size();

    unsigned int uiCount = 0;
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->GetType() == bindType )
            uiCount++;
    }
    return uiCount;
}

void CKeyBinds::DoPreFramePulse()
{
    // HACK: chatbox binds
    if ( m_pChatBoxBind )
    {
        Call ( m_pChatBoxBind );
        m_pChatBoxBind = NULL;
    }

    // HACK: shift keys
    if ( m_pCore->IsFocused() )
    {
        bool bLeftShift = ( GetKeyState( VK_LSHIFT ) & 0x8000 ) != 0;
        bool bRightShift = ( GetKeyState( VK_RSHIFT ) & 0x8000 ) != 0;

        if ( bLeftShift != bPreLeftShift )
        {
            if ( bLeftShift )
                ProcessKeyStroke( &g_bkKeys[9], true );
            else
                ProcessKeyStroke( &g_bkKeys[9], false );

            bPreLeftShift = bLeftShift;
        }

        if ( bRightShift != bPreRightShift )
        {
            if ( bRightShift )
                ProcessKeyStroke( &g_bkKeys[10], true );
            else
                ProcessKeyStroke( &g_bkKeys[10], false );

            bPreRightShift = bRightShift;
        }
    }
}

void CKeyBinds::DoPostFramePulse()
{
    eSystemState SystemState = CCore::GetSingleton ().GetGame ()->GetSystemState ();

    if ( m_bWaitingToLoadDefaults && ( SystemState == GS_FRONTEND || SystemState == GS_PLAYING_GAME ) ) // Are GTA controls actually initialized?
    {
        LoadDefaultBinds ();
        m_bWaitingToLoadDefaults = false; 
    }

    if ( SystemState != GS_PLAYING_GAME )
        return;

    // Unset the mousewheel states if we have any bound
    if ( m_bMouseWheel )
    {
        binds_t::const_iterator iter = m_list.begin();

        for ( ; iter != m_list.end(); ++iter )
        {
            eKeyBindType bindType = (*iter)->GetType ();
            if ( bindType == KEY_BIND_COMMAND || bindType == KEY_BIND_FUNCTION )
            {
                CKeyBindWithState* pBind = static_cast < CKeyBindWithState* > ( *iter );
                if ( pBind->boundKey->iGTARelative == GTA_KEY_MSCROLLUP ||
                     pBind->boundKey->iGTARelative == GTA_KEY_MSCROLLDOWN )
                {
                    pBind->bState = false;
                }
            }
            else if ( bindType == KEY_BIND_GTA_CONTROL )
            {
                CGTAControlBind* pBind = static_cast < CGTAControlBind* > ( *iter );
                if ( pBind->boundKey->iGTARelative == GTA_KEY_MSCROLLUP ||
                     pBind->boundKey->iGTARelative == GTA_KEY_MSCROLLDOWN )
                {
                    if ( pBind->bState )
                    {
                        CallGTAControlBind ( pBind, false );
                    }
                }
            }
        }
        m_bMouseWheel = false;
    }
}

void CKeyBinds::DoPostGameFramePulse( void )
{
    // Reset mouse wheel status.
    m_bFrameMouseWheelDown = false;
    m_bFrameMouseWheelUp = false;
}

bool CKeyBinds::LoadFromXML ( CXMLNode* pMainNode )
{
    bool bSuccess = false;
    bool bLoadDefaults = false;
    if ( pMainNode )
    {
        CXMLNode* pNode = NULL;
        unsigned int uiCount = pMainNode->GetSubNodeCount ();
        
        if ( uiCount == 0 )
            bLoadDefaults = true;

        for ( unsigned int i = 0; i < uiCount; i++ )
        {
            pNode = pMainNode->GetSubNode ( i );

            if ( pNode == NULL ) continue;

            std::string strValue;
            strValue = pNode->GetTagName ();
            if ( strValue.compare ( "bind" ) == 0 )
            {
                CXMLAttribute* pAttribute = pNode->GetAttributes ().Find ( "key" );

                if ( pAttribute )
                {
                    std::string strKey;
                    strKey = pAttribute->GetValue ();
                    
                    if ( !strKey.empty () )
                    {
                        pAttribute = pNode->GetAttributes ().Find ( "command" );

                        if ( pAttribute )
                        {
                            std::string strCommand;
                            strCommand = pAttribute->GetValue ();

                            if ( !strCommand.empty () )
                            {
                                // HACK
                                std::string strState, strArguments, strResource ;
                                pNode = pMainNode->GetSubNode ( i );

                                pAttribute = pNode->GetAttributes ().Find ( "state" );

                                if ( pAttribute )
                                {
                                    strState = pAttribute->GetValue ();
                                }
                                bool bState = strState.compare ( "up" ) != 0;

                                pAttribute = pNode->GetAttributes ().Find ( "arguments" );
                                
                                if ( pAttribute )
                                {
                                    strArguments = pAttribute->GetValue ();
                                }

                                pAttribute = pNode->GetAttributes ().Find ( "resource" );
                                if ( pAttribute )
                                {
                                    strResource = pAttribute->GetValue ();
                                    AddCommand ( strKey.c_str (), strCommand.c_str (), strArguments.c_str (), bState, strResource.c_str() );
                                    SetCommandActive ( strKey.c_str (), strCommand.c_str(), bState, strArguments.c_str(), strResource.c_str(), false, true );
                                }
                                else if ( !GetCommandBind( strKey.c_str(), strCommand.c_str(), true, bState ) )
                                    AddCommand ( strKey.c_str (), strCommand.c_str (), strArguments.c_str (), bState );
                            }                                        
                        }
                        else
                        {
                            pAttribute = pNode->GetAttributes ().Find ( "control" );

                            if ( pAttribute )
                            {
                                std::string strControl;
                                strControl = pAttribute->GetValue ();

                                if ( !strControl.empty () )
                                {
                                    if ( !GTAControlExists ( strKey.c_str (), strControl.c_str () ) )
                                        AddGTAControl ( strKey.c_str (), strControl.c_str () );
                                }
                            }
                        }
                    }
                }
            }
        }
        bSuccess = true;
    }
    else
        bLoadDefaults = true;

    eSystemState SystemState = CCore::GetSingleton ().GetGame ()->GetSystemState ();
    if ( bLoadDefaults )
    {
        if ( SystemState == 7 || SystemState == 9 ) // Are GTA controls actually initialized?
            LoadDefaultBinds ();
        else
            m_bWaitingToLoadDefaults = true;
    }

    return bSuccess;
}


bool CKeyBinds::SaveToXML( CXMLNode *pMainNode )
{
    CXMLAttribute *pA = NULL;

    // Clear our current bind nodes
    pMainNode->DeleteAllSubNodes();

    // Iterate the key binds adding them to the XML tree
    CXMLNode* pNode;
    CXMLAttributes *pAttributes;
    binds_t::const_iterator iter = m_list.begin();

    for ( ; iter != m_list.end(); ++iter )
    {
        if ( (*iter)->boundKey == NULL )
            continue;

        // Create the new 'bind' node
        pNode = pMainNode->CreateSubNode ( "bind" );

        // If it was created
        if ( pNode )
        {
            pAttributes = &pNode->GetAttributes ();

            eKeyBindType type = (*iter)->GetType ();
            if ( type == KEY_BIND_COMMAND )
            {
                // Create the key attribute
                const char* szKey = (*iter)->boundKey->szKey;
                if ( szKey )
                {
                    pA = pAttributes->Create ( "key" );
                    pA->SetValue ( szKey );
                }

                CCommandBind* pBind = static_cast < CCommandBind* > ( *iter );
                char* szState = ( pBind->bHitState ) ? "down" : "up";


                pA = pAttributes->Create ( "state" );
                pA->SetValue ( szState );

                const char* szCommand = pBind->m_cmd.c_str();
                if ( szCommand )
                {
                    pA = pAttributes->Create ( "command" );                     
                    pA->SetValue ( szCommand );
                }

                const char* szArguments = pBind->m_args.c_str();
                if ( szArguments )
                {
                    pA = pAttributes->Create ( "arguments" );                       
                    pA->SetValue ( szArguments );
                }

                const char* szResource = pBind->GetResource();
                if ( szResource )
                {
                    pA = pAttributes->Create ( "resource" );                        
                    pA->SetValue ( szResource );

                    //If its still the default key dont bother saving it
                    if ( *pBind->m_defKey == szKey )
                        pNode->GetParent()->DeleteSubNode(pNode);
                }
            }
            else if ( type == KEY_BIND_GTA_CONTROL )
            {
                // Create the key attribute
                const char* szKey = (*iter)->boundKey->szKey;
                if ( szKey )
                {
                    pA = pAttributes->Create ( "key" );
                    pA->SetValue ( szKey );
                }

                CGTAControlBind* pBind = static_cast < CGTAControlBind* > ( *iter );
                char* szControl = pBind->control->szControl;
                if ( szControl )
                {
                    pA = pAttributes->Create ( "control" );
                    pA->SetValue ( szControl );
                }
            }
        }
    }
    return true;
}

void CKeyBinds::LoadDefaultBinds()
{
    Clear();
    
    LoadControlsFromGTA();
    LoadDefaultCommands( true );
}

void CKeyBinds::LoadDefaultControls()
{
    AddGTAControl( "lctrl", "fire" );
    AddGTAControl( "mouse1", "fire" );
    AddGTAControl( "e", "next_weapon" );
    AddGTAControl( "mouse_wheel down", "next_weapon" );
    AddGTAControl( "q", "previous_weapon" );
    AddGTAControl( "num_dec", "previous_weapon" );
    AddGTAControl( "mouse_wheel_up", "previous_weapon" );
    AddGTAControl( "g", "group_control_forwards" );
    AddGTAControl( "h", "group_control_back" );
    AddGTAControl( "y", "conversation_yes" );
    AddGTAControl( "n", "conversation_no" );
    AddGTAControl( "arrow_u", "forwards" );
    AddGTAControl( "w", "forwards" );
    AddGTAControl( "arrow_d", "backwards" );
    AddGTAControl( "s", "backwards" );
    AddGTAControl( "arrow_l", "left" );
    AddGTAControl( "a", "left" );
    AddGTAControl( "arrow_r", "right" );
    AddGTAControl( "d", "right" );
    AddGTAControl( "x", "zoom_in" );
    AddGTAControl( "pgup", "zoom_in" );
    AddGTAControl( "mouse_wheel_up", "zoom_in" );
    AddGTAControl( "y", "zoom_out" );
    AddGTAControl( "pgdn", "zoom_out" );
    AddGTAControl( "mouse_wheel_down", "zoom_out" );
    AddGTAControl( "f", "enter_exit" );
    AddGTAControl( "enter", "enter_exit" );
    AddGTAControl( "v", "change_camera" );
    AddGTAControl( "home", "change_camera" );
    AddGTAControl( "shift", "jump" );
    AddGTAControl( "rctrl", "jump" );
    AddGTAControl( "space", "sprint" );
    AddGTAControl( "delete", "aim_weapon" );
    AddGTAControl( "capslock", "aim_weapon" );
    AddGTAControl( "c", "crouch" );
    AddGTAControl( "tab", "action" );
    AddGTAControl( "lalt", "walk" );
    AddGTAControl( "num_1", "look_behind" );
    AddGTAControl( "mouse3", "look_behind" );

    AddGTAControl( "lalt", "vehicle_fire" );
    AddGTAControl( "mouse1", "vehicle_fire" );
    AddGTAControl( "ctrl", "vehicle_secondary_fire" );
    AddGTAControl( "num_0", "vehicle_secondary_fire" );
    AddGTAControl( "w", "accelerate" );
    AddGTAControl( "s", "brake_reverse" );
    AddGTAControl( "a", "vehicle_left" );
    AddGTAControl( "arrow_l", "vehicle_left" );
    AddGTAControl( "d", "vehicle_right" );
    AddGTAControl( "arrow_r", "vehicle_right" );
    AddGTAControl( "arrow_u", "steer_forward" );
    AddGTAControl( "arrow_d", "steer_back" );
    AddGTAControl( "insert", "radio_next" );
    AddGTAControl( "4", "radio_next" );
    AddGTAControl( "mouse_wheel_up", "radio_next" );
    AddGTAControl( "delete", "radio_previous" );
    AddGTAControl( "r", "radio_previous" );
    AddGTAControl( "mouse_wheel_down", "radio_previous" );
    AddGTAControl( "f5", "radio_user_track_skip" );
    AddGTAControl( "capslock", "horn" );
    AddGTAControl( "h", "horn" );
    AddGTAControl( "2", "sub_mission" );
    AddGTAControl( "num_add", "sub_mission" );
    AddGTAControl( "space", "handbrake" );
    AddGTAControl( "rctrl", "handbrake" );
    AddGTAControl( "mouse3", "vehicle_look_behind" );
    AddGTAControl( "mouse2", "vehicle_mouse_look" );
    AddGTAControl( "q", "vehicle_look_left" );
    AddGTAControl( "e", "vehicle_look_right" );
    AddGTAControl( "num_4", "special_control_left" );
    AddGTAControl( "num_6", "special_control_right" );
    AddGTAControl( "num_8", "special_control_up" );
    AddGTAControl( "delete", "special_control_up" );
    AddGTAControl( "num_2", "special_control_down" );
    AddGTAControl( "end", "special_control_down" );
}

void CKeyBinds::LoadDefaultCommands( bool bForce )
{
    for ( int i = 0 ; *g_dcbDefaultCommands [ i ].szKey != NULL ; i++ )
    {
        SDefaultCommandBind* temp = &g_dcbDefaultCommands [ i ];
        if ( bForce || !GetCommandBind( NULL, temp->szCommand, true, temp->bState, temp->szArguments ) )
            AddCommand ( temp->szKey, temp->szCommand, temp->szArguments, temp->bState );
    }
}


void CKeyBinds::LoadControlsFromGTA()
{
    for ( int i = FIRE; i <= GROUP_CONTROL_BACK; i++ )
    {
        CGame* pGame = m_pCore->GetGame ();
        if ( pGame )
        {
            SBindableGTAControl* pControl = GetBindableFromAction ( (eControllerAction)i );
            if ( pControl )
            {
                // Keyboard
                int iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction ( (eControllerAction)i, KEYBOARD );
                if ( iKey != NO_KEY_DEFINED )
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative ( iKey );
                    if ( pKey )
                        AddGTAControl ( pKey, pControl, GetGTAControlStatus( pControl ) );
                }

                // Optional Key
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction ( (eControllerAction)i, OPTIONAL_EXTRA );
                if ( iKey != NO_KEY_DEFINED )
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative ( iKey );
                    if ( pKey )
                        AddGTAControl ( pKey, pControl, GetGTAControlStatus( pControl ) );
                }

                // Mouse
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction ( (eControllerAction)i, MOUSE );
                if ( iKey != NO_KEY_DEFINED )
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative ( iKey );
                    if ( pKey )
                        AddGTAControl ( pKey, pControl, GetGTAControlStatus( pControl ) );
                }

                // Joystick
                iKey = pGame->GetControllerConfigManager()->GetControllerKeyAssociatedWithAction ( (eControllerAction)i, JOYSTICK );
                if ( iKey )
                {
                    const SBindableKey* pKey = GetBindableFromGTARelative ( GTA_KEY_JOY(iKey) );
                    if ( pKey )
                        AddGTAControl ( pKey, pControl, GetGTAControlStatus( pControl ) );
                }
            }
        }
    }
}

void CKeyBinds::BindCommand ( const char* szCmdLine )
{
    CConsoleInterface* pConsole = m_pCore->GetConsole (); 

    char* szError = "* Syntax: bind <defaults/key> [<up/down>] <command> [<arguments>]";
    if ( szCmdLine == NULL )
    {
        pConsole->Print ( szError );
        return;
    }

    // Copy the buffer
    char* szTemp = new char [ strlen ( szCmdLine ) + 16 ];
    strcpy ( szTemp, szCmdLine );

    // Split it up into bind key, command and arguments
    char* szKey = strtok ( szTemp, " " );
    char* szCommand = strtok ( NULL, " " );
       
    if ( szKey )
    {
        if ( strcmp ( szKey, "defaults" ) == 0 )
        {
            LoadDefaultBinds ();
            pConsole->Print ( "* Bound all default commands and controls\n" );
        }
        else if ( szCommand )
        {
            if ( IsControl ( szCommand ) )
            {
                if ( !GTAControlExists( szKey, szCommand ) )
                {
                    AddGTAControl( szKey, szCommand );
                    pConsole->Printf( "* Bound key '%s' to control '%s'", szKey, szCommand );
                }
                else
                    pConsole->Printf ( "* '%s' key already bound to control '%s'", szKey, szCommand );
            }
            else
            {
                bool bState = true;
                if ( strcmp ( szCommand, "up" ) == 0 || strcmp ( szCommand, "down" ) == 0 )
                {
                    bState = ( strcmp ( szCommand, "down" ) == 0 );
                    szCommand = strtok ( NULL, " " );
                }

                if ( szCommand )
                {
                    char* szArguments = strtok ( NULL, "\0" );
                    SString strKeyState ( "%s", bState? "down" : "up" );
                    SString strCommandAndArguments ( "%s%s%s", szCommand, szArguments ? " " : "", szArguments ? szArguments : "" );

                    if ( !GetCommandBind( szKey, szCommand, true, bState, szArguments ) )
                    {
                        if ( AddCommand ( szKey, szCommand, szArguments, bState ) )
                            pConsole->Printf ( "* Bound key '%s' '%s' to command '%s'", szKey, *strKeyState, *strCommandAndArguments );
                        else
                            pConsole->Printf ( "* Failed to bind '%s' '%s' to command '%s'", szKey, *strKeyState, *strCommandAndArguments );
                    }
                    else
                        pConsole->Printf ( "* '%s' '%s' key already bound to command '%s'", szKey, *strKeyState, *strCommandAndArguments );
                }
                else
                    pConsole->Print ( szError );
            }
        }
        else
            pConsole->Print ( szError );
    }
    else
        pConsole->Print ( szError );

    delete [] szTemp;
}


void CKeyBinds::UnbindCommand ( const char* szCmdLine )
{
    CConsoleInterface* pConsole = m_pCore->GetConsole (); 

    char* szError = "* Syntax: unbind <all/key> [<up/down> <command>]";
    if ( szCmdLine == NULL )
    {
        pConsole->Print ( szError );
        return;
    }

    // Copy the buffer
    char* szTemp = new char [ strlen ( szCmdLine ) + 16 ];
    strcpy ( szTemp, szCmdLine );

    // Split it up into bind key and command
    char* szKey = strtok ( szTemp, " " );
    char* szCommand = strtok ( NULL, " " );

    if ( szKey )
    {
        if ( stricmp ( szKey, "all" ) == 0 )
        {
            RemoveAllCommands ();
            RemoveAllGTAControls ();
            pConsole->Print ( "* Removed all bound keys." );
        }
        else if ( szCommand )
        {
            if ( IsControl ( szCommand ) )
            {
                if ( RemoveGTAControl ( szKey, szCommand ) )
                    pConsole->Printf ( "* Unbound key '%s' from control '%s'", szKey, szCommand );
                else
                    pConsole->Printf ( "* Failed to unbind '%s' from control '%s'", szKey, szCommand );
            }
            else
            {
                bool bState = true;
                if ( strcmp ( szCommand, "up" ) == 0 || strcmp ( szCommand, "down" ) == 0 )
                {
                    bState = ( strcmp ( szCommand, "down" ) == 0 );
                    szCommand = strtok ( NULL, " " );
                }

                if ( szCommand )
                {
                    if ( RemoveCommand ( szKey, szCommand, true, bState ) )
                        pConsole->Printf ( "* Unbound key '%s' '%s' from command '%s'", szKey, ( bState ) ? "down" : "up", szCommand );
                    else
                        pConsole->Printf ( "* Failed to unbind '%s' '%s' from command '%s'", szKey, ( bState ) ? "down" : "up", szCommand );
                }
                else if ( RemoveAllCommands ( szKey, true, bState ) )
                    pConsole->Printf ( "* Removed all binds from key '%s' '%s'", szKey, ( bState ) ? "down" : "up" );
                else
                    pConsole->Printf ( "* Failed to remove binds from key '%s' '%s'", szKey, ( bState ) ? "down" : "up" );
            }
        }
        else
        {
            bool b = RemoveAllGTAControls ( szKey );
            if ( RemoveAllCommands ( szKey ) || b )
                pConsole->Printf ( "* Removed all binds from key '%s'", szKey );
            else
                pConsole->Printf ( "* Failed to remove binds from key '%s'", szKey );
        }
    }
    else
        pConsole->Print ( szError );

    delete [] szTemp;
}


void CKeyBinds::PrintBindsCommand( const char *szCmdLine )
{
    CConsoleInterface* pConsole = m_pCore->GetConsole ();

    // Get the key
    char * szTemp = NULL;
    char* szKey = NULL;

    if ( szCmdLine )
    {
        // Copy the buffer
        szTemp = new char [ strlen ( szCmdLine ) + 16 ];
        strcpy ( szTemp, szCmdLine );    
        szKey = strtok ( szTemp, " " );
    }

    if ( szKey )
    {
        const SBindableKey * pKey = GetBindableFromKey ( szKey );
        if ( pKey )
        {
            pConsole->Printf ( "* Current key binds for '%s': *", szKey );

            bool bIsEmpty = true;
            binds_t::const_iterator iter = m_list.begin();

            for ( ; iter != m_list.end(); ++iter )
            {
                CKeyBind *pKeyBind = *iter;
                if ( pKeyBind->boundKey && pKeyBind->boundKey->szKey )
                {
                    if ( pKeyBind->boundKey == pKey )
                    {
                        eKeyBindType type = pKeyBind->GetType ();
                        if ( type == KEY_BIND_COMMAND )
                        {
                            CCommandBind* pBind = static_cast < CCommandBind* > ( pKeyBind );
                            pConsole->Printf ( "Command^%s %s: %s %s", pBind->boundKey->szKey,
                                            ( pBind->bHitState ) ? "down" : "up", pBind->m_cmd.c_str(), pBind->m_args.c_str() );
                        }
                        else if ( type == KEY_BIND_GTA_CONTROL )
                        {
                            CGTAControlBind* pBind = static_cast < CGTAControlBind* > ( pKeyBind );
                            pConsole->Printf ( "Control^%s: %s", pBind->boundKey->szKey,
                                                                    pBind->control->szControl );
                        }
                        bIsEmpty = false;
                    }
                }
            }
            if ( bIsEmpty )
                pConsole->Print ( "empty." );
        }
        else
            pConsole->Printf ( "* Error: '%s' key not found", szKey );
    }
    else
    {
        bool bIsEmpty = true;

        pConsole->Print ( "* Current key binds: *" );

        binds_t::const_iterator iter = m_list.begin();
        for ( ; iter != m_list.end(); ++iter )
        {
            eKeyBindType type = (*iter)->GetType ();
            if ( type == KEY_BIND_COMMAND )
            {
                CCommandBind* pBind = static_cast < CCommandBind* > ( *iter );
                pConsole->Printf ( "Command^%s %s: %s %s", pBind->boundKey->szKey,
                                ( pBind->bHitState ) ? "down" : "up", pBind->m_cmd.c_str(), pBind->m_args.c_str() );
            }
            else if ( type == KEY_BIND_GTA_CONTROL )
            {
                CGTAControlBind* pBind = static_cast < CGTAControlBind* > ( *iter );
                pConsole->Printf ( "Control^%s: %s", pBind->boundKey->szKey, pBind->control->szControl );
            }
            bIsEmpty = false;
        }
        if ( bIsEmpty )
            pConsole->Print ( "empty." );
    }
    if ( szTemp )
        delete [] szTemp;
}


bool CKeyBinds::IsFakeCtrl_L ( UINT message, WPARAM wParam, LPARAM lParam )
{
    MSG msgNext;
    LONG lTime;
    BOOL bReturn;

    /*
    * Fake Ctrl_L presses will be followed by an Alt_R keypress
    * with the same timestamp as the Ctrl_L press.
    */
    if ( ( message == WM_KEYDOWN || message == WM_SYSKEYDOWN ) &&
          wParam == VK_CONTROL &&
          ( HIWORD ( lParam ) & KF_EXTENDED ) == 0 )
    {
        /* Got a Ctrl_L press */

        /* Get time of current message */
        lTime = GetMessageTime ();

        /* Look for fake Ctrl_L preceeding an Alt_R press. */
        bReturn = PeekMessage ( &msgNext, NULL, WM_KEYDOWN, WM_SYSKEYDOWN, PM_NOREMOVE );

        /*
        * Try again if the first call fails.
        * NOTE: This usually happens when TweakUI is enabled.
        */
        if ( !bReturn )
        {
            /* Voodoo to make sure that the Alt_R message has posted */
            Sleep ( 0 );

            /* Look for fake Ctrl_L preceeding an Alt_R press. */
            bReturn = PeekMessage ( &msgNext, NULL, WM_KEYDOWN, WM_SYSKEYDOWN, PM_NOREMOVE );
        }

        if ( msgNext.message != WM_KEYDOWN && msgNext.message != WM_SYSKEYDOWN )
            bReturn = 0;

        /* Is next press an Alt_R with the same timestamp? */
        if ( bReturn && msgNext.wParam == VK_MENU && msgNext.time == lTime && 
            ( HIWORD ( msgNext.lParam ) & KF_EXTENDED ) )
        {
            /* 
            * Next key press is Alt_R with same timestamp as current
            * Ctrl_L message.  Therefore, this Ctrl_L press is a fake
            * event, so discard it.
            */
            return TRUE;
        }
    }

    /* 
    * Fake Ctrl_L releases will be followed by an Alt_R release
    * with the same timestamp as the Ctrl_L release.
    */
    if ( ( message == WM_KEYUP || message == WM_SYSKEYUP ) && wParam == VK_CONTROL &&
         ( HIWORD ( lParam ) & KF_EXTENDED ) == 0 )
    {
        /* Got a Ctrl_L release */

        /* Get time of current message */
        lTime = GetMessageTime ();

        /* Look for fake Ctrl_L release preceeding an Alt_R release. */
        bReturn = PeekMessage ( &msgNext, NULL, WM_KEYUP, WM_SYSKEYUP, PM_NOREMOVE );

        /*
        * Try again if the first call fails.
        * NOTE: This usually happens when TweakUI is enabled.
        */
        if ( !bReturn )
        {
            /* Voodoo to make sure that the Alt_R message has posted */
            Sleep ( 0 );

            /* Look for fake Ctrl_L release preceeding an Alt_R release. */
            bReturn = PeekMessage ( &msgNext, NULL, WM_KEYUP, WM_SYSKEYUP, PM_NOREMOVE );
        }

        if ( msgNext.message != WM_KEYUP && msgNext.message != WM_SYSKEYUP )
            bReturn = 0;
      
        /* Is next press an Alt_R with the same timestamp? */
        if ( bReturn && ( msgNext.message == WM_KEYUP || msgNext.message == WM_SYSKEYUP ) &&
            msgNext.wParam == VK_MENU && msgNext.time == lTime &&
            ( HIWORD ( msgNext.lParam) & KF_EXTENDED ) )
        {
            /*
            * Next key release is Alt_R with same timestamp as current
            * Ctrl_L message. Therefore, this Ctrl_L release is a fake
            * event, so discard it.
            */
            return TRUE;    
        }
    }
  
    /* Not a fake control left press/release */
    return FALSE;
}

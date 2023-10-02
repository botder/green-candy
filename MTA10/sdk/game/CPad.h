/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPad.h
*  PURPOSE:     Controller pad interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_PAD
#define __CGAME_PAD

// NOTE: This is an internal interface!
class CControllerState
{
public:
    CControllerState()
    {
        memset( this, 0, sizeof(CControllerState) );
    }

    inline void ResetLeftAxis()
    {
        LeftStickX = 0;
        LeftStickY = 0;
    }

    inline void SetSquareDown( bool down )
    {
        ButtonSquare = down ? 0x100 : 0;
    }

    inline bool IsSquareDown()
    {
        return ButtonSquare > 0x80;
    }

    inline void SetTriangleDown( bool down )
    {
        ButtonTriangle = down ? 0x100 : 0;
    }

    inline bool IsTriangleDown()
    {
        return ButtonTriangle > 0x80;
    }

    inline void SetCrossDown( bool down )
    {
        ButtonCross = down ? 0x100 : 0;
    }

    inline bool IsCrossDown()
    {
        return ButtonCross > 0x80;
    }

    inline void SetCircleDown( bool down )
    {
        ButtonCircle = down ? 0x100 : 0;
    }

    inline void SetLeftShockDown( bool down )
    {
        ShockButtonL = down ? 0x100 : 0;
    }

    inline void SetRightShockDown( bool down )
    {
        ShockButtonR = down ? 0x100 : 0;
    }

    inline bool IsCircleDown()
    {
        return ButtonCircle > 0x80;
    }

    inline bool IsLeftShoulder1Down()
    {
        return LeftShoulder1 == 0xFF;
    }

    inline bool IsLeftShoulder2Down()
    {
        return LeftShoulder2 == 0xFF;
    }

    inline bool IsRightShoulder1Down()
    {
        return RightShoulder1 == 0xFF;
    }

    inline bool IsRightShoulder2Down()
    {
        return RightShoulder2 == 0xFF;
    }

    short   LeftStickX;             // 0, move/steer left (-128?)/right (+128)
    short   LeftStickY;             // 2, move back(+128)/forwards(-128?)
    short   RightStickX;            // 4, numpad 6(+128)/numpad 4(-128?)
    short   RightStickY;            // 6
    
    short   LeftShoulder1;          // 8
    short   LeftShoulder2;          // 10
    short   RightShoulder1;         // 12, target / hand brake
    short   RightShoulder2;         // 14
    
    short   m_digitalUp;            // 16, radio change up
    short   m_digitalDown;          // 18, radio change down
    short   m_digitalLeft;          // 20
    short   m_digitalRight;         // 22

    short   m_start;                // 24
    short   m_select;               // 26

    short   ButtonSquare;           // 28, jump / reverse
    short   ButtonTriangle;         // 30, get in/out
    short   ButtonCross;            // 32, sprint / accelerate
    short   ButtonCircle;           // 34, fire

    short   ShockButtonL;           // 36
    short   ShockButtonR;           // 38, look behind

    short   m_chatIndicated;        // 40
    short   m_bPedWalk;             // 42
    short   m_vehicleMouseLook;     // 44
    short   m_radioTrackSkip;       // 46
};

class CPad
{
public:
    virtual const CControllerState&     GetState() = 0;
    virtual const CControllerState&     GetPreviousState() = 0;
    virtual void                        SetState( const CControllerState& cs ) = 0;
    virtual void                        InjectCurrent( const CControllerState& cs ) = 0;

    virtual void                        Restore() = 0;
    virtual void                        Store() = 0;
    virtual bool                        IsEnabled() = 0;
    virtual void                        Disable( bool bDisable ) = 0;
    virtual void                        Clear() = 0;
    virtual void                        SetHornHistoryValue( bool value ) = 0;
    virtual long                        GetAverageWeapon() = 0;
    virtual void                        SetLastTimeTouched( long time ) = 0;
};

#endif

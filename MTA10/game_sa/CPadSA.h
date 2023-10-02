/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPadSA.h
*  PURPOSE:     Header file for controller pad input class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_PAD
#define __CGAMESA_PAD

#include <game/CPad.h>

#define MAX_HORN_HISTORY        5

#define FUNC_CPad_UpdatePads    0x00541DD0

class CPadSAInterface
{
public:
    void                SetState( const CControllerState& cs );
    void                InjectCurrent( const CControllerState& cs );
    void                SetHornHistory( bool state );

    CControllerState    m_new;                              // 0
    CControllerState    m_prev;                             // 48
    
    short               m_steering[10];                     // 96
    long                m_drunkDriving;                     // 116

    CControllerState    m_keyState;                         // 120
    CControllerState    m_joyState;                         // 168
    CControllerState    m_mouseState;                       // 216

    unsigned char       m_phase;                            // 264, needed for switching into right state (analogue, pressure sensitive etc)
    unsigned short      m_mode;                             // 265, key configuration
    short               m_shakeDuration;                    // 267
    unsigned short      m_disabled;                         // 269
    unsigned char       m_shakeFrequency;                   // 271
    bool                m_hornHistory[MAX_HORN_HISTORY];    // 272
    unsigned char       m_numHorn;                          // 277
    unsigned char       m_disableFrames;                    // 278
    bool                m_brakeControl;                     // 279
    bool                m_disableCarjack;                   // 280
    bool                m_disableDuck;                      // 281
    bool                m_disableFire;                      // 282
    bool                m_disableFire2;                     // 283
    bool                m_disableWeaponCycle;               // 284
    bool                m_disableJump;                      // 285
    bool                m_disableStats;                     // 286

    long                m_lastControl;                      // 287
    long                m_averageWeapon;                    // 291, since last reset
    long                m_averageEntries;                   // 295

    long                m_noShakeTime;                      // 299
    bool                m_noShake;                          // 303

    BYTE                m_pad[4];                           // 304
};

class CPadSA : public CPad
{
private:
    CPadSAInterface*            m_interface;
    CPadSAInterface             m_stored;

public:
                                CPadSA( CPadSAInterface *pad );
                                ~CPadSA();

    inline CPadSAInterface*     GetInterface()                  { return m_interface; }

    const CControllerState&     GetState();
    const CControllerState&     GetPreviousState();
    void                        SetState( const CControllerState& cs );
    void                        InjectCurrent( const CControllerState& cs );

    void                        Store();
    void                        Restore();

    // Those are hacks
    bool                        IsEnabled();
    void                        Disable( bool bDisable );

    void                        Clear();
    void                        SetHornHistoryValue( bool value );
    long                        GetAverageWeapon();
    void                        SetLastTimeTouched( long time );
};

#endif

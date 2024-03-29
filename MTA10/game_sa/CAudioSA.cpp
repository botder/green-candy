/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAudioSA.cpp
*  PURPOSE:     Audio manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#define HOOKPOS_CAEAmbienceTrackManager_CheckForPause       0x4D6E21
DWORD RETURN_CAEAmbienceTrackManager_CheckForPause =        0x4D6E27;
void HOOK_CAEAmbienceTrackManager_CheckForPause ();

CAudioSA::CAudioSA()
{
    m_bRadioOn = false;
    m_bRadioMuted = false;
    m_ucRadioChannel = 0;
    m_bAmbientSoundsPaused = false;
    m_bAmbientGeneralEnabled = true;
    m_bAmbientGunfireEnabled = true;

    HookInstall ( HOOKPOS_CAEAmbienceTrackManager_CheckForPause, (DWORD)HOOK_CAEAmbienceTrackManager_CheckForPause, 6 );
}

void CAudioSA::StopRadio()
{
    m_bRadioOn = false;

    //DWORD dwFunc = FUNC_StopRadio;
    DWORD dwFunc = 0x4E9823; // Some function CAudio::StopRadio jumps to immediately

    _asm 
    {
        // This doesn't work anymore because we've
        // returned out the function.
        /*
        push    0
        push    0
        mov     ecx, CLASS_CAudioEngine
        call    dwFunc
        */

        // Push our arguments onto the stack and emulate a "call" instruction
        // by pushing the return position on the stack aswell.
        push        0
        push        0
        push        retpoint

        // Do what CAudio::StopRadio does. Mov the AERadioTrackManager class instance into 'ecx' (this)
        mov         ecx, CLASS_AERadioTrackManager  

        // Do what this global func we've removed does in the beginning.
        push        ebx  
        push        ebp  
        push        esi

        // Jump to behind the return code we've replaced.
        jmp         dwFunc

        retpoint:
    }
}

void CAudioSA::StartRadio( unsigned int station )
{
    m_ucRadioChannel = station;
    m_bRadioOn = true;

    // Make sure we have the correct muted state
    m_bRadioMuted = pGame->GetSettings ()->GetRadioVolume () < 1;
    if ( m_bRadioMuted )
        return;

    DWORD dwFunc = 0x4DBEC3;
    DWORD dwFunc2 = 0x4EB3C3;
    _asm 
    {
        // We can't do this anymore as we've returned out StartRadio
        /*
        push    0
        push    station
        mov     ecx, CLASS_CAudioEngine
        call    dwFunc
        */

        // Push our arguments onto the stack
        push        0
        push        station

        // Call something, skip 3 bytes that we have our return instruction on (no arguments)
        mov         ecx,CLASS_AECutsceneTrackManager 
        mov         eax,dword ptr [ecx+8] 
        call        dwFunc

        // Check the return value, eventually skip
        test        al,al 
        jne         skip

        mov         eax,dword ptr [esp+4] 
        mov         ecx,dword ptr [esp] 

        // Push arguments to some other function
        push        0
        push        0
        push        eax  
        push        ecx  

        // Call it (emulate call instruction)
        mov         ecx,8CB6F8h 
        push        done
        push        ebx  
        mov         bl,byte ptr [esp+8] 
        jmp         dwFunc2

        // Pop our arguments back
        done:
        pop         eax
        pop         eax

        skip:
    }
}

// 43 = race one
// 32 = help
// 13 = camera take picture
void CAudioSA::PlayFrontEndSound( unsigned short sound )
{
    if  ( *(DWORD *)VAR_AudioEventVolumes != 0 && sound <= 101 ) // may prevent a crash
    {
        DEBUG_TRACE("void CAudioSA::PlayFrontEndSound( unsigned short sound )");
        DWORD dwFunc = FUNC_ReportFrontendAudioEvent;
        FLOAT fUnknown = 1.0f;
        _asm
        {
            push    fUnknown
            push    0
            movzx   eax,sound
            push    eax
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }

/*  DWORD dwAudioEntity = 0xB6BC90;

    DWORD dwFunc = 0x507290;
    _asm
    {
        push    1
        push    dwSound
        call    dwFunc
    }

        
    dwFunc = 0x5072B0;
    _asm
    {
        push    dwSound
        mov     ecx,dwAudioEntity
        call    dwFunc
    }*/
}

void CAudioSA::SetEffectsMasterVolume( unsigned char volume )
{
    DWORD dwFunc = FUNC_SetEffectsMasterVolume;
    DWORD dwVolume = volume;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    dwVolume
        call    dwFunc
    }
}

void CAudioSA::SetMusicMasterVolume( unsigned char volume )
{
    DWORD dwFunc = FUNC_SetMusicMasterVolume;
    DWORD dwVolume = volume;
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    dwVolume
        call    dwFunc
    }

    //
    // See if radio stream should be stopped/started
    //

    bool bNewRadioMuted = volume < 1;
    bool bRadioMutedChanged = m_bRadioMuted != bNewRadioMuted;
    m_bRadioMuted = bNewRadioMuted;

    // If mute state has changed while the radio is on
    if ( bRadioMutedChanged && m_bRadioOn )
    {
        if ( !m_bRadioMuted )
        {
            // mute -> unmute
            StartRadio ( m_ucRadioChannel );
        }
        else
        {
            // unmute -> mute
            StopRadio ();
            m_bRadioOn = true;  // StopRadio was only called to stop the radio stream. Radio is logically still on
        }
    }
}

void CAudioSA::PlayBeatTrack( short iTrack )
{
    if  ( *(DWORD *)VAR_AudioEventVolumes != 0 ) // may prevent a crash
    {
        DEBUG_TRACE("VOID CAudioSA::PlayBeatTrack ( short iTrack )");
        DWORD dwFunc = FUNC_PreloadBeatTrack;
        DWORD dwTrack = iTrack;
        _asm
        {
            mov     ecx, CLASS_CAudioEngine
            push    dwTrack
            call    dwFunc
        }

        dwFunc = FUNC_PlayPreloadedBeatTrack;
        _asm
        {
            mov     ecx, CLASS_CAudioEngine
            push    1
            call    dwFunc
        }
    }
}

void CAudioSA::PushEntityAudio( CEntitySAInterface *entity )
{
    DWORD dwFunc = 0x00506ED0;

    __asm
    {
        mov ecx,CLASS_CAudioEngine
        push entity
        call dwFunc
    }
}

void CAudioSA::ClearMissionAudio( int slot )
{
    DWORD dwFunc = 0x5072F0; // CAudioEngine::ClearMissionAudio(unsigned char)
    __asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot // sound bank slot?
        call    dwFunc
    }
}

bool CAudioSA::IsMissionAudioSampleFinished( int slot ) const
{
    DWORD dwFunc = 0x5072C0; // CAudioEngine::IsMissionAudioSampleFinished
    bool cret = 0;
    __asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot
        call    dwFunc
        mov     cret, al
    }
    return cret;
}

void CAudioSA::PreloadMissionAudio( unsigned short usAudioEvent, int slot )
{
    DWORD dwFunc = 0x507290; // CAudioEngine__PreloadMissionAudio
    DWORD AudioEvent = usAudioEvent;
    __asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    AudioEvent
        push    slot
        call    dwFunc
    }
}

unsigned char CAudioSA::GetMissionAudioLoadingStatus( int slot ) const
{
    DWORD dwFunc = 0x5072A0; // get load status
    unsigned char cret = 0;
    __asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    slot
        call    dwFunc
        mov     cret, al
    }
    return cret;
}

void CAudioSA::AttachMissionAudioToPhysical( CPhysical *physical, int slot )
{
    CPhysicalSA *pPhysical = dynamic_cast <CPhysicalSA*> ( physical );
    CEntitySAInterface *entity = pPhysical ? pPhysical->GetInterface() : NULL;
    
    DWORD dwFunc = 0x507330; // AttachMissionAudioToPhysical
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    entity
        push    slot
        call    dwFunc
    }
}

void CAudioSA::SetMissionAudioPosition( const CVector& pos, int slot )
{
    DWORD dwFunc = 0x507300; // CAudioEngine__SetMissionAudioPosition
    _asm
    {
        mov     ecx, CLASS_CAudioEngine
        push    pos
        push    slot
        call    dwFunc
    }
}

bool CAudioSA::PlayLoadedMissionAudio( int slot )
{
    if ( GetMissionAudioLoadingStatus(slot) == 1 )
    {
        DWORD dwFunc = 0x5072B0; // CAudioEngine::PlayLoadedMissionAudio(unsigned char)
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            push    slot
            call    dwFunc
        }
        return true;
    }
    return false;
}

void CAudioSA::PauseAllSound( bool bPaused )
{
    if ( bPaused )
    {
        DWORD dwFunc = FUNC_PauseAllSounds;
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }
    else
    {
        DWORD dwFunc = FUNC_ResumeAllSounds;
        _asm    
        {
            mov     ecx, CLASS_CAudioEngine
            call    dwFunc
        }
    }
}

void CAudioSA::PauseAmbientSounds( bool bPaused )
{
    m_bAmbientSoundsPaused = bPaused;
    UpdateAmbientSoundSettings ();
}

void CAudioSA::SetAmbientSoundEnabled( eAmbientSoundType eType, bool bEnabled )
{
    if ( eType == AMBIENT_SOUND_GENERAL )
    {
        m_bAmbientGeneralEnabled = bEnabled;
    }
    else
    if ( eType == AMBIENT_SOUND_GUNFIRE )
    {
        m_bAmbientGunfireEnabled = bEnabled;
    }
    UpdateAmbientSoundSettings ();
}

bool CAudioSA::IsAmbientSoundEnabled( eAmbientSoundType eType ) const
{
    if ( m_bAmbientSoundsPaused )
        return false;

    if ( eType == AMBIENT_SOUND_GENERAL )
        return m_bAmbientGeneralEnabled;
    else if ( eType == AMBIENT_SOUND_GUNFIRE )
        return m_bAmbientGunfireEnabled;

    return false;
}

void CAudioSA::ResetAmbientSounds()
{
    SetAmbientSoundEnabled ( AMBIENT_SOUND_GENERAL, true );
    SetAmbientSoundEnabled ( AMBIENT_SOUND_GUNFIRE, true );
}

void CAudioSA::UpdateAmbientSoundSettings()
{
    // Update gunfire setting
    if ( IsAmbientSoundEnabled ( AMBIENT_SOUND_GUNFIRE ) )
        MemPut < BYTE > ( 0x507814, 0x85 );     // Enable gunfire (default)
    else
        MemPut < BYTE > ( 0x507814, 0x33 );     // No gunfire
}

bool _cdecl IsAmbientSoundGeneralEnabled()
{
    return pGame->GetAudio ()->IsAmbientSoundEnabled( AMBIENT_SOUND_GENERAL );
}

// Hook for manual ambient sound pause
void _declspec(naked) HOOK_CAEAmbienceTrackManager_CheckForPause()
{
    _asm
    {
        // Hooked from 004D6E21  6 bytes
        call IsAmbientSoundGeneralEnabled
        test al, al
        jnz skip
        mov     dword ptr [esp+08h], 0      // Pause
    skip:

        // orig
        mov     edi, [esp+08h]
        xor     ecx, ecx
        jmp     RETURN_CAEAmbienceTrackManager_CheckForPause  // 4D6E27
    }
}

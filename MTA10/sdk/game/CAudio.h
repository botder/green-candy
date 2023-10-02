/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAudio.h
*  PURPOSE:     Game audio interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_AUDIO
#define __CGAME_AUDIO

enum eAmbientSoundType
{
    AMBIENT_SOUND_GENERAL,
    AMBIENT_SOUND_GUNFIRE,
};

class CPhysical;

class CAudio
{
public:
    virtual void                PlayFrontEndSound( unsigned short id ) = 0;
    virtual void                PlayBeatTrack( short iTrack ) = 0;
    virtual void                SetEffectsMasterVolume( unsigned char volume ) = 0; // 64 = max volume
    virtual void                SetMusicMasterVolume( unsigned char volume ) = 0;
    virtual void                ClearMissionAudio( int slot = 1) = 0;
    virtual void                PreloadMissionAudio( unsigned short usAudioEvent, int slot = 1 ) = 0;
    virtual unsigned char       GetMissionAudioLoadingStatus( int slot = 1 ) const = 0;
    virtual bool                IsMissionAudioSampleFinished( int slot = 1 ) const = 0;
    virtual void                AttachMissionAudioToPhysical( CPhysical *physical, int slot = 1 ) = 0;
    virtual void                SetMissionAudioPosition( const CVector& pos, int slot = 1 ) = 0;
    virtual bool                PlayLoadedMissionAudio( int slot = 1 ) = 0;
    virtual void                PauseAllSound( bool bPaused ) = 0;
    virtual void                StopRadio() = 0;
    virtual void                StartRadio( unsigned int station ) = 0;
    virtual void                PauseAmbientSounds( bool bPaused ) = 0;
    virtual void                SetAmbientSoundEnabled( eAmbientSoundType eType, bool bEnabled ) = 0;
    virtual bool                IsAmbientSoundEnabled( eAmbientSoundType eType ) const = 0;
    virtual void                ResetAmbientSounds() = 0;
};

#endif

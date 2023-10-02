/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPacketRecorder.h
*  PURPOSE:     Client packet recorder and "demo" replay class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

class CClientPacketRecorder;

#ifndef __CCLIENTPACKETRECORDER_H
#define __CCLIENTPACKETRECORDER_H

#include "CClientManager.h"
#include <ctime>

class CClientPacketRecorder
{
    friend class CClientManager;

public:
                        CClientPacketRecorder       ( CClientManager* pManager );
                        ~CClientPacketRecorder      ();

    void                SetPacketHandler            ( PPACKETHANDLER pfnPacketHandler );

    void                StartPlayback               ( const char *input, bool frames );
    void                StartRecord                 ( const char *output, bool frames );
    void                Stop                        ();

    bool                IsPlaying                   ();
    bool                IsRecording                 ();
    bool                IsPlayingOrRecording        ();
    bool                IsFrameBased                ();

    void                SetFrameSkip                ( unsigned int frames );

    void                RecordPacket                ( unsigned char id, NetBitStreamInterface& bitStream );
    void                RecordLocalData             ( CClientPlayer* pLocalPlayer );

private:
    void                ReadLocalData               ( CFile *file );

    void                DoPulse                     ();

    CClientManager*     m_pManager;
    filePath            m_filename;
    PPACKETHANDLER      m_pfnPacketHandler;

    long                m_lRelative;
    long                m_lFrames;

    bool                m_bPlaying;
    bool                m_bRecording;
    bool                m_bFrameBased;

    unsigned long       m_ulCurrentOffset;
    long                m_lNextPacketTime;

    unsigned int        m_uiFrameSkip;
};

#endif

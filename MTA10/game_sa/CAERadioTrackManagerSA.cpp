/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CAERadioTrackManagerSA.cpp
*  PURPOSE:     Audio entity radio track manager
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

unsigned char CAERadioTrackManagerSA::GetCurrentRadioStationID()
{
    DEBUG_TRACE("unsigned char CAERadioTrackManagerSA::GetCurrentRadioStationID()");

    DWORD dwFunc = FUNC_GetCurrentRadioStationID;
    unsigned char station;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     station, al
    }

    return station;
}

bool CAERadioTrackManagerSA::IsVehicleRadioActive()
{
    DEBUG_TRACE("bool CAERadioTrackManagerSA::IsVehicleRadioActive()");

    DWORD dwFunc = FUNC_IsVehicleRadioActive;
    bool active;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     active, al
    }

    return active;
}

char* CAERadioTrackManagerSA::GetRadioStationName( unsigned char id )
{
    DEBUG_TRACE("char* CAERadioTrackManagerSA::GetRadioStationName( unsigned char id )");

    DWORD dwFunc = FUNC_GetRadioStationName;
    DWORD dwStationID = id;
    char *name = 0;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    dwStationID
        call    dwFunc
        mov     name, eax
    }

    return name;
}

bool CAERadioTrackManagerSA::IsRadioOn()
{
    DEBUG_TRACE("bool CAERadioTrackManagerSA::IsRadioOn()");

    DWORD dwFunc = FUNC_IsRadioOn;
    bool bReturn;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
        mov     bReturn, al
    }

    return bReturn;
}

void CAERadioTrackManagerSA::SetBassSetting( float bass )
{
    DEBUG_TRACE("void CAERadioTrackManagerSA::SetBassSetting( float bass )");

    DWORD dwFunc = FUNC_SetBassSetting;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    0x3F800000 // 1.0f
        push    bass
        call    dwFunc
    }
}

void CAERadioTrackManagerSA::Reset()
{
    DEBUG_TRACE("void CAERadioTrackManagerSA::Reset()");

    DWORD dwFunc = FUNC_Reset;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        call    dwFunc
    }
}

void CAERadioTrackManagerSA::StartRadio( unsigned char station, unsigned char unk )
{
    DEBUG_TRACE("void CAERadioTrackManagerSA::StartRadio( unsigned char station, unsigned char unk )");

    DWORD dwFunc = FUNC_StartRadio;
    DWORD dwStationID = station;
    DWORD dwUnknown = unk;

    _asm
    {
        mov     ecx, CLASS_CAERadioTrackManager
        push    0
        push    0
        push    dwUnknown
        push    dwStationID
        call    dwFunc
    }
}

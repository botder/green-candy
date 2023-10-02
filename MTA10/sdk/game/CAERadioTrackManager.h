/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CAERadioTrackManager.h
*  PURPOSE:     Radio track audio entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CAERADIOTRACKMANAGER
#define __CGAME_CAERADIOTRACKMANAGER

class CAERadioTrackManager
{
public:
    virtual unsigned char   GetCurrentRadioStationID        () = 0;
    virtual bool            IsVehicleRadioActive            () = 0;
    virtual char*           GetRadioStationName             ( unsigned char id ) = 0;
    virtual bool            IsRadioOn                       () = 0;
    virtual void            SetBassSetting                  ( float bass ) = 0;
    virtual void            Reset                           () = 0;
    virtual void            StartRadio                      ( unsigned char station, unsigned char unk ) = 0;
};

#endif

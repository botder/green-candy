/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CPadManager.h
*  PURPOSE:     Played ped entity interface
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _VIRTUAL_PAD_MANAGER_
#define _VIRTUAL_PAD_MANAGER_

class CPadManager abstract
{
public:
    virtual void                    UpdateJoypad( CControlInterface& states, CPed& ped ) = 0;
};

#endif //_VIRTUAL_PAD_MANAGER_
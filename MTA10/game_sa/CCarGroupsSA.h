/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCarGroupsSA.h
*  PURPOSE:     Header file for car group definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CAR_GROUPS_
#define _CAR_GROUPS_

// Module initialization.
void    CarGroups_Init( void );
void    CarGroups_Shutdown( void );

namespace CarGroups
{
    modelId_t __cdecl   GetRandomModelByGroup( unsigned int id );
    bool __cdecl        DoesGroupHaveModel( unsigned int id, modelId_t model );
};

#endif //_CAR_GROUPS_
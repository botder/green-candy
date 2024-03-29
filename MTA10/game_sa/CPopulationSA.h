/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPopulationSA.h
*  PURPOSE:     Header file for ped entity population manager class
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAMESA_POPULATION
#define __CGAMESA_POPULATION

#include <game/CPopulation.h>

// Module initialization.
void Population_Init( void );
void Population_Shutdown( void );

class CPopulationSA : public CPopulation
{

};

#endif
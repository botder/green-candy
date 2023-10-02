/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCarGroupsSA.cpp
*  PURPOSE:     Header file for car group definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Structure holding car groups.
#define MAX_CAR_GROUP_MODELS        23

struct carGroup_t
{
    unsigned short models[MAX_CAR_GROUP_MODELS];
};

/*=========================================================
    CarGroups_Init

    Purpose:
        Loads the DATA\\CARGRP.DAT file and initializes the vehicles
        which are used by ped groups.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005BD1A0
=========================================================*/
static carGroup_t *const VAR_carGroups = (carGroup_t*)0x00C0ED38;
static unsigned short *const VAR_numCarGroupModels = (unsigned short*)0x00C0EC78;

void __cdecl _CarGroups_Init( void )
{
    // Open up the car groups file.
    CFile *file = gameFileRoot->Open( "\\DATA\\CARGRP.DAT", "r" );

    // Bugfix: If the file does not exist, exit.
    if ( !file )
        return;

    // NOTE: the original code changed into the "\\DATA\\" directory.
    // it is not required (it switched back immediatly afterward).

    // Loop through every line of the file.
    char buf[0x400];

    // Index of current car group; increase every line until max.
    unsigned int carGroupIndex = 0;

    while ( FileSystem::FileGetString( file, buf, sizeof(buf) ) )
    {
        // Filter out commas and line seek resets.
        {
            char *bufSeek = buf;

            // This logic is a little changed.
            for ( ; *bufSeek != '\0'; bufSeek++ )
            {
                char c = *bufSeek;

                if ( c == ',' || c == 13 )
                    *bufSeek = ' ';
            }

            // NULL-terminate for security.
            *bufSeek = 0;
        }

        // Get the vehicle names of this line.
        const char *bufSeek = buf;

        unsigned int carIndex = 0;
        carGroup_t& group = VAR_carGroups[carGroupIndex];

        for ( unsigned int n = 0; n < MAX_CAR_GROUP_MODELS; n++ )
        {
            // Get the length of a token.
            size_t tokenLen = 0;

            for ( ; *bufSeek <= ' '; bufSeek++ )
            {
                if ( *bufSeek == 0 )
                    break;

                tokenLen++;
            }

            // Do we encounter a line terminator?
            if ( *bufSeek == '#' || *bufSeek <= ' ' )
                break;

            // Seek through the characters again with the starting point 'tokenLen'
            unsigned int m;

            for (m = tokenLen; buf[m] > ' '; m++ );

            if ( m != tokenLen )
            {
                char modelName[0x100];
                size_t nameLen = m - tokenLen;

                strncpy( modelName, bufSeek, nameLen );

                modelName[nameLen] = 0;

                modelId_t modelIndex;

                if ( Streaming::GetModelByName( modelName, &modelIndex ) )
                {
                    group.models[carIndex++] = modelIndex;
                }
            }
        }

        // If any car has been added.
        if ( carIndex )
        {
            if ( carIndex < 23 )
            {
                // Set the number of models saved.
                VAR_numCarGroupModels[carGroupIndex] = carIndex;

                // Fill in the remaining slots if it was not added full.
                for ( unsigned int n = carIndex; n < MAX_CAR_GROUP_MODELS; n++ )
                    group.models[n] = 2000;
            }

            carIndex++;
        }
    }

    // Close our config file.
    delete file;
}

/*=========================================================
    CarGroups::GetRandomModelByGroup
    
    Arguments:
        id - car/ped group index
    Purpose:
        Returns a random not loaded vehicle model belonging to
        the given group. Returns 0xFFFFFFFF if it has found
        none.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00421900
=========================================================*/
modelId_t __cdecl CarGroups::GetRandomModelByGroup( unsigned int id )
{
    const carGroup_t& group = VAR_carGroups[id];
    unsigned int numCarModels = VAR_numCarGroupModels[id];

    // Attempt to get random car model sixteen times.
    for ( unsigned int n = 0; n < 16; n++ )
    {
        unsigned int selectIndex = (unsigned int)( (float)((short)rand() / 0x8000) * (float)numCarModels );
        modelId_t modelIndex = group.models[selectIndex];

        if ( Streaming::GetModelLoadInfo( modelIndex ).m_eLoading != MODEL_LOADED )
            return modelIndex;
    }

    return 0xFFFFFFFF;
}

/*=========================================================
    CarGroups::DoesGroupHaveModel
    
    Arguments:
        id - car/ped group index
        model - model index to check
    Purpose:
        Returns whether the given model is present in group
        id.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00406F50
=========================================================*/
bool __cdecl CarGroups::DoesGroupHaveModel( unsigned int id, modelId_t model )
{
    unsigned int numCarModels = VAR_numCarGroupModels[id];
    carGroup_t& group = VAR_carGroups[id];

    for ( unsigned int n = 0; n < numCarModels; n++ )
    {
        if ( group.models[n] == model )
            return true;
    }

    return false;
}

void CarGroups_Init( void )
{
    HookInstall( 0x005BD1A0, (DWORD)_CarGroups_Init, 5 );
    HookInstall( 0x00421900, (DWORD)CarGroups::GetRandomModelByGroup, 5 );
    HookInstall( 0x00406F50, (DWORD)CarGroups::DoesGroupHaveModel, 5 );
}

void CarGroups_Shutdown( void )
{
}
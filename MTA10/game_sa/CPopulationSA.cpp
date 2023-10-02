/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CPopulationSA.cpp
*  PURPOSE:     Ped entity population manager
*  DEVELOPERS:  
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

/*=========================================================
    InitPopulationCycle

    Purpose:
        Loads the DATA\\POPCYCLE.DAT file which contains ped
        and vehicle density settings depending on time and
        area.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005BC090
=========================================================*/
#define NUM_TIMEZONES               12

enum ePedGroup : unsigned int
{
    PEDG_WORKER,
    PEDG_BUSINESS,
    PEDG_CLUBBER,
    PEDG_FARMER,
    PEDG_BEACHFOLK,
    PEDG_PARKFOLK,
    PEDG_CASUAL_RICH,
    PEDG_CASUAL_AVERAGE,
    PEDG_CASUAL_POOR,
    PEDG_PROZZY,
    PEDG_CRIMINAL,
    PEDG_GOLFER,
    PEDG_SERVANT,
    PEDG_AIRCREW,
    PEDG_ENTERTAINER,
    PEDG_OOT_FACT,
    PEDG_DESERTFOLK,
    PEDG_AIRCREW_RUNWAY,
    NUM_PED_GROUPS
};

enum eZoneType : unsigned int
{
    ZONE_BUSINESS,
    ZONE_DESERT,
    ZONE_ENTERTAINMENT,
    ZONE_COUNTRYSIDE,
    ZONE_RESIDENTIAL_RICH,
    ZONE_RESIDENTIAL_AVERAGE,
    ZONE_RESIDENTIAL_POOR,
    ZONE_GANGLAND,
    ZONE_BEACH,
    ZONE_SHOPPING,
    ZONE_PARK,
    ZONE_INDUSTRY,
    ZONE_ENTERTAINMENT_BUSY,
    ZONE_SHOPPING_BUSY,
    ZONE_SHOPPING_POSH,
    ZONE_RESIDENTIAL_RICH_SECLUDED,
    ZONE_AIRPORT,
    ZONE_GOLF_CLUB,
    ZONE_OUT_OF_TOWN_FACTORY,
    ZONE_AIRPORT_RUNWAY,
    NUM_ZONE_TYPES
};

struct pedGroupProbability_t
{
    unsigned char perc[NUM_PED_GROUPS];
};

enum eDayType : unsigned int
{
    DAYTYPE_WORKDAY,
    DAYTYPE_WEEKEND,
    NUM_DAYTYPES
};

template <typename carryType>
struct dayInfo_t
{
    carryType   areas[NUM_ZONE_TYPES];
};

template <typename carryType>
struct timeZone_t
{
    dayInfo_t <carryType>   dayTypes[NUM_DAYTYPES];
};

template <typename carryType>
struct timeData_t
{
    timeZone_t <carryType>  zones[NUM_TIMEZONES];
};

// Binary offset: 0x00C0BC78
// This structure is responsible for the ped diversity in the GTA:SA world.
// We could export this information to the Deathmatch module.
struct population_t
{
    timeData_t <pedGroupProbability_t>  spawnProbability;   // 0
    timeData_t <unsigned char>          otherPerc;          // 8640
    timeData_t <unsigned char>          copPerc;            //
    timeData_t <unsigned char>          gangPerc;           //
    timeData_t <unsigned char>          dealerPerc;         //
    timeData_t <unsigned char>          numCars;            //
    timeData_t <unsigned char>          numPeds;            //
};

inline population_t& GetPopulationInfo( void )
{
    return *(population_t*)0x00C0BC78;
}

void __cdecl InitPopulationCycle( void )
{
    // Note: the original code changed to DATA\\ directory and back.
    // it is not required, as we have the file translators.

    CFile *file = OpenGlobalStream( "DATA\\POPCYCLE.DAT", "rb" );

    // Bugfix: if the file does not exist, terminate.
    if ( !file )
        return;

    // Note: this function is made to run through all entries.
    // If there are missing entries, the engine will crash.
    for ( unsigned int area = 0; area < NUM_ZONE_TYPES; area++ )
    {
        for ( unsigned int day = 0; day < NUM_DAYTYPES; day++ )
        {
            for ( unsigned int i = 0; i < NUM_TIMEZONES; i++ )
            {
                while ( const char *line = FileMgr::GetConfigLine( file ) )
                {
                    // Skip invalid lines.
                    do
                    {
                        char token = line[0];

                        if ( token != '/' && token != '\0' )
                            break;
                    }
                    while ( line = FileMgr::GetConfigLine( file ) );

                    // Read the line with the many entries.
                    int numPeds, numCars, dealerPerc, gangPerc, copsPerc, otherPerc,
                        workerPerc, businessPerc, clubberPerc, farmerPerc, beachFolkPerc,
                        parkfolkPerc, casualRichPerc, casualAveragePerc, casualPoorPerc,
                        prozzyPerc, criminalPerc, golferPerc, servantPerc, aircrewPerc,
                        entertainerPerc, oot_factPerc, desertfolkPerc, aircrewRunwayPerc;

                    sscanf( line, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                        &numPeds, &numCars, &dealerPerc, &gangPerc, &copsPerc, &otherPerc,
                        &workerPerc, &businessPerc, &clubberPerc, &farmerPerc, &beachFolkPerc,
                        &parkfolkPerc, &casualRichPerc, &casualAveragePerc, &casualPoorPerc,
                        &prozzyPerc, &criminalPerc, &golferPerc, &servantPerc, &aircrewPerc,
                        &entertainerPerc, &oot_factPerc, &desertfolkPerc, &aircrewRunwayPerc
                    );

                    // Save it inside of our structure.
                    population_t& population = GetPopulationInfo();
                    population.numPeds.zones[i].dayTypes[day].areas[area] = numPeds;
                    population.numCars.zones[i].dayTypes[day].areas[area] = numCars;
                    population.dealerPerc.zones[i].dayTypes[day].areas[area] = dealerPerc;
                    population.gangPerc.zones[i].dayTypes[day].areas[area] = gangPerc;
                    population.copPerc.zones[i].dayTypes[day].areas[area] = copsPerc;
                    population.otherPerc.zones[i].dayTypes[day].areas[area] = otherPerc;

                    // Now save the probability.
                    pedGroupProbability_t& groupPerc = population.spawnProbability.zones[i].dayTypes[day].areas[area];
                    groupPerc.perc[PEDG_WORKER] = workerPerc;
                    groupPerc.perc[PEDG_BUSINESS] = businessPerc;
                    groupPerc.perc[PEDG_CLUBBER] = clubberPerc;
                    groupPerc.perc[PEDG_FARMER] = farmerPerc;
                    groupPerc.perc[PEDG_BEACHFOLK] = beachFolkPerc;
                    groupPerc.perc[PEDG_PARKFOLK] = parkfolkPerc;
                    groupPerc.perc[PEDG_CASUAL_RICH] = casualRichPerc;
                    groupPerc.perc[PEDG_CASUAL_AVERAGE] = casualAveragePerc;
                    groupPerc.perc[PEDG_CASUAL_POOR] = casualPoorPerc;
                    groupPerc.perc[PEDG_PROZZY] = prozzyPerc;
                    groupPerc.perc[PEDG_CRIMINAL] = criminalPerc;
                    groupPerc.perc[PEDG_GOLFER] = golferPerc;
                    groupPerc.perc[PEDG_SERVANT] = servantPerc;
                    groupPerc.perc[PEDG_AIRCREW] = aircrewPerc;
                    groupPerc.perc[PEDG_ENTERTAINER] = entertainerPerc;
                    groupPerc.perc[PEDG_OOT_FACT] = oot_factPerc;
                    groupPerc.perc[PEDG_DESERTFOLK] = desertfolkPerc;
                    groupPerc.perc[PEDG_AIRCREW_RUNWAY] = aircrewRunwayPerc;

                    // Normalize the percentage.
                    {
                        unsigned int maxPercentage = 0;

                        for ( unsigned int n = 0; n < NUM_PED_GROUPS; n++ )
                            maxPercentage += groupPerc.perc[n];

                        double normalizeRatio = 100.0f / maxPercentage;

                        // Make sure the percentages add up to 100%.
                        for ( unsigned int n = 0; n < NUM_PED_GROUPS; n++ )
                            groupPerc.perc[n] = (unsigned char)( (double)groupPerc.perc[n] * normalizeRatio );
                    }

                    // Normalize the percentage scale even more, by making
                    // sure the unsigned char values add up to 100.
                    {
                        // Add up the percentages in uchar format.
                        int maxPercentage = 0;
                        int highestPercIndex = PEDG_CLUBBER;   // assume clubbers are highest by default.
                        int highestPercValue = 0;

                        for ( unsigned int n = 0; n < NUM_PED_GROUPS; n++ )
                        {
                            int perc = groupPerc.perc[n];

                            maxPercentage += perc;

                            if ( perc >= highestPercIndex )
                            {
                                highestPercValue = perc;
                                highestPercIndex = n;
                            }
                        }

                        // Normalize the percentage of the highest type.
                        groupPerc.perc[highestPercIndex] += 100 - maxPercentage;
                    }
                }
            }
        }
    }

    // Init management values.
    *(unsigned int*)0x00C0BC74 = 0;
    *(unsigned int*)0x00C0BC70 = 0;
    *(unsigned int*)0x00C0BC68 = 0;
    *(unsigned int*)0x00C0BC6C = 0xFFFFFFFF;

    delete file;
}

void Population_Init( void )
{
    HookInstall( 0x005BC090, (DWORD)InitPopulationCycle, 5 );
}

void Population_Shutdown( void )
{
}
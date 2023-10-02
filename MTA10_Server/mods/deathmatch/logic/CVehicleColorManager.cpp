/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CVehicleColorManager.cpp
*  PURPOSE:     Vehicle entity color manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVehicleColor CVehicleColors::GetRandomColor()
{
    // Grab a random index
    unsigned int uiSize = CountColors ();
    if ( uiSize > 0 )
    {
        // Create a random index
        unsigned int uiRandomIndex = GetRandom ( 0, uiSize - 1 );

        // Grab the random color we got off the list
        unsigned int uiIndex = 0;
        list < CVehicleColor > ::iterator iter = m_Colors.begin ();
        for ( ; iter != m_Colors.end (); iter++ )
        {
            if ( uiIndex == uiRandomIndex )
            {
                return *iter;
            }

            ++uiIndex;
        }
    }

    // No items, return default color (black)
    return CVehicleColor ();
}

bool CVehicleColorManager::Load( const char *filename )
{
    // Make sure we're cleared
    Reset ();

    // Load vehiclecolors.conf
    CFile *file = modFileRoot->Open( filename, "r" );

    if ( !file )
        return false;

    // Read each line of it
    std::string line;

    while ( !file->IsEOF() )
    {
        // Grab the current line
        file->GetString( line );

        // Is this a comment?
        if ( line[0] == '#' )
            continue;

        // Split it up in vehicle id, color1, color2, color3 and color4
        const char *szModel = strtok( szBuffer, " " );
        const char *szColor1 = strtok( NULL, " " );
        const char *szColor2 = strtok( NULL, " " );
        const char *szColor3 = strtok( NULL, " " );
        const char *szColor4 = strtok( NULL, " " );

        if ( !szModel )
            continue;

        // Add it to the list
        CVehicleColor color;

        color.SetPaletteColors(
            szColor1 ? atoi(szColor1) : 0,
            szColor2 ? atoi(szColor2) : 0,
            szColor3 ? atoi(szColor3) : 0,
            szColor4 ? atoi(szColor4) : 0 );

        AddColor( atoi(szModel), color );
    }

    // Close it
    delete file;

    return true;
}

bool CVehicleColorManager::Generate( const char *filename )
{
    return modFileRoot->WriteData( filename, szDefaultVehicleColors, strlen(szDefaultVehicleColors) );
}

void CVehicleColorManager::Reset()
{
    // Remove all colors from all vehicles
    for ( int i = 0; i < 212; i++ )
    {
        m_Colors [i].RemoveAllColors ();
    }
}

void CVehicleColorManager::AddColor ( unsigned short usModel, const CVehicleColor& colVehicle )
{
    if ( usModel >= 400 && usModel <= 611 )
    {
        m_Colors [ usModel - 400 ].AddColor ( colVehicle );
    }
}


CVehicleColor CVehicleColorManager::GetRandomColor ( unsigned short usModel )
{
    if ( usModel >= 400 && usModel <= 611 )
    {
        return m_Colors [ usModel - 400 ].GetRandomColor ();
    }
    else
    {
        return CVehicleColor ();
    }
}

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwInternals.h
*  PURPOSE:     RenderWare internal definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_INTERNALS_
#define _RENDERWARE_INTERNALS_

#define MAX_SAMPLERS    8

struct d3d9RasterStage  //size: 24 bytes
{
    AINLINE d3d9RasterStage( void )
    {
        raster = NULL;
        u_addressMode = RWTEXADDRESS_WRAP;
        v_addressMode = RWTEXADDRESS_WRAP;
        filterType = RWFILTER_DISABLE;
        borderColor = 0xFFFFFFFF;
        maxAnisotropy = 0;
    }

    RwRaster *raster;                           // 0
    rwRasterStageAddressMode u_addressMode;     // 4
    rwRasterStageAddressMode v_addressMode;     // 8
    rwRasterStageFilterMode filterType;         // 12
    RwColor borderColor;                        // 16
    int maxAnisotropy;                          // 20
};

extern d3d9RasterStage _currentRasterStages[];      // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A508

inline d3d9RasterStage& GetRasterStageInfo( unsigned int index )
{
    assert( index < MAX_SAMPLERS );

    return _currentRasterStages[ index ];
}

// Template parameters for easier management.
struct deviceStateValue
{
    deviceStateValue( void )
    {
        // We have to initialize this parameter this way.
        // Later in the engine we have to set up these values properly
        // That is looping through all TSS and applying them to our
        // internal table.
        value = 0xFFFFFFFF;
    }

    deviceStateValue( const DWORD& value )
    {
        this->value = value;
    }

    void operator = ( DWORD value )
    {
        this->value = value;
    }

    operator DWORD ( void ) const
    {
        return value;
    }

    operator DWORD& ( void )
    {
        return value;
    }

    DWORD value;
};

#endif //_RENDERWARE_INTERNALS_
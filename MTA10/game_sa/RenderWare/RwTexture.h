/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTexture.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_TEXTURE_
#define _RENDERWARE_TEXTURE_

struct RwRaster //size: 52 bytes
{
    RwRaster*               parent;                             // 0
    void*                   pixels;                             // 4, is not NULL if pixel data has been requested from the GPU
    void*                   palette;                            // 8
    int                     width, height, depth;               // 12, 16 / 0x10, 20
    int                     stride;                             // 24 / 0x18
    short                   u, v;                               // 28
    unsigned char           type;                               // 32, raster type field
    unsigned char           flags;                              // 33, bits 4..8 of raster format
    unsigned char           privateFlags;                       // 34
    unsigned char           format;                             // 35, 0..3 bits is raster format, 4..7 bits is mipmap flags/palette flags
    unsigned char*          origPixels;                         // 36
    int                     origWidth, origHeight, origDepth;   // 40
};
struct RwTexture
{
    RwRaster*                   raster;                         // 0
    RwTexDictionary*            txd;                            // 4
    RwListEntry <RwTexture>     TXDList;                        // 8
    char                        name[RW_TEXTURE_NAME_LENGTH];   // 16
    char                        mask[RW_TEXTURE_NAME_LENGTH];   // 48

    union
    {
        unsigned int            flags;                          // 80

        struct
        {
            unsigned char       filterMode : 8;                 // 80
			unsigned char		u_addressMode : 4;              // 81
            unsigned char       v_addressMode : 4;
        };
    };
    unsigned int                refs;                           // 84
    char                        anisotropy;                     // 88

    // Methods.
    void                        SetName                 ( const char *name );

    void                        AddToDictionary         ( RwTexDictionary *txd );
    void                        RemoveFromDictionary    ( void );

    // Flag derived from research by MTA team.
    void                        SetFiltering            ( bool filter )     { BOOL_FLAG( flags, 0x1102, filter ); }
    bool                        IsFiltering             ( void ) const      { return IS_FLAG( flags, 0x1102 ); }
};

struct RwTextureCoordinates
{
    float u,v;
};

// Texture API.
RwTexture*          RwTextureStreamReadEx           ( RwStream *stream );

#endif //_RENDERWARE_TEXTURE_
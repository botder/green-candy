/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwRenderStates.cpp
*  PURPOSE:     RenderWare renderstate management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RwRenderTools.hxx"
#include "RwInternals.h"

static float                _currentFogDensity = 0.0f;              // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4FC
static rwDeviceValue_t      _currentFogEnable = false;              // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4F4
static rwFogModeState       _currentFogTableIndex = RWFOG_DISABLE;  // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4F8
static rwDeviceValue_t      _currentFogColor = 0x00000000;          // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A500
static rwShadeModeState     _currentShadeMode = RWSHADE_FLAT;       // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A504
static rwDeviceValue_t      _currentZWriteEnable = false;           // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4C0
static rwDeviceValue_t      _currentDepthFunctionEnable = false;    // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4C4
static rwDeviceValue_t      _currentStencilEnable = false;          // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4C8
static rwStencilOpState     _currentStencilFail = RWSTENCIL_KEEP;   // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4CC
static rwStencilOpState     _currentStencilZFail = RWSTENCIL_KEEP;  // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4D0
static rwStencilOpState     _currentStencilPass = RWSTENCIL_KEEP;   // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4D4
static rwCompareOpState     _currentStencilFunc = RWCMP_NEVER;      // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4D8
static rwDeviceValue_t      _currentStencilRef = 0;                 // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4DC
static rwDeviceValue_t      _currentStencilMask = 0x00000000;       // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4E0
static rwDeviceValue_t      _currentStencilWriteMask = 0x00000000;  // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4E4
static rwCullModeState      _currentCullMode = RWCULL_NONE;         // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A4F0
static rwBlendModeState     _currentSrcBlend = RWBLEND_ZERO;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5C8
static rwBlendModeState     _currentDstBlend = RWBLEND_ZERO;        // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5CC
static rwCompareOpState     _currentAlphaRefFunc = RWCMP_NEVER;     // Binary offsets: (1.0 US and 1.0 EU): 0x00C9A5D0

inline float& GetCurrentFogDensity( void )                          { return _currentFogDensity; }
inline rwDeviceValue_t& GetCurrentFogEnable( void )                 { return _currentFogEnable; }
inline rwFogModeState& GetCurrentFogTableIndex( void )              { return _currentFogTableIndex; }
inline rwDeviceValue_t& GetCurrentFogColor( void )                  { return _currentFogColor; }
inline rwShadeModeState& GetCurrentShadeMode( void )                { return _currentShadeMode; }
inline rwDeviceValue_t& GetCurrentZWriteEnable( void )              { return _currentZWriteEnable; }
inline rwDeviceValue_t& GetCurrentDepthFunctionEnable( void )       { return _currentDepthFunctionEnable; }
inline rwDeviceValue_t& GetCurrentStencilEnable( void )             { return _currentStencilEnable; }
inline rwStencilOpState& GetCurrentStencilFail( void )              { return _currentStencilFail; }
inline rwStencilOpState& GetCurrentStencilZFail( void )             { return _currentStencilZFail; }
inline rwStencilOpState& GetCurrentStencilPass( void )              { return _currentStencilPass; }
inline rwCompareOpState& GetCurrentStencilFunc( void )              { return _currentStencilFunc; }
inline rwDeviceValue_t& GetCurrentStencilRef( void )                { return _currentStencilRef; }
inline rwDeviceValue_t& GetCurrentStencilMask( void )               { return _currentStencilMask; }
inline rwDeviceValue_t& GetCurrentStencilWriteMask( void )          { return _currentStencilWriteMask; }
inline rwCullModeState& GetCurrentCullMode( void )                  { return _currentCullMode; }
inline rwBlendModeState& GetCurrentSrcBlend( void )                 { return _currentSrcBlend; }
inline rwBlendModeState& GetCurrentDstBlend( void )                 { return _currentDstBlend; }
inline rwCompareOpState& GetCurrentAlphaRefFunc( void )             { return _currentAlphaRefFunc; }

static const D3DFOGMODE _fogTableModes[] =
{
    D3DFOG_NONE,
    D3DFOG_LINEAR,
    D3DFOG_EXP,
    D3DFOG_EXP2
};

inline int _RwD3D9SetFogColor( rwDeviceValue_t color )
{
    RwD3D9SetRenderState( D3DRS_FOGCOLOR, color );

    GetCurrentFogColor() = color;
    return 1;
}

int RwD3D9SetFogColor( rwDeviceValue_t color )
{
    if ( GetCurrentFogColor() == color )
        return 1;

    return _RwD3D9SetFogColor( color );
}

rwDeviceValue_t RwD3D9GetFogColor( void )
{
    return GetCurrentFogColor();
}

inline int RwD3D9ResetFogColor( void )
{
    RwD3D9SetRenderState( D3DRS_FOGCOLOR, GetCurrentFogColor() );
    return 1;
}

inline void _RwD3D9UpdateFogDensity( float density )
{
    RwD3D9SetRenderState( D3DRS_FOGDENSITY, F2DW( density ) );
}

inline int _RwD3D9SetFogDensity( float density )
{
    _RwD3D9UpdateFogDensity( density );

    GetCurrentFogDensity() = density;
    return 1;
}

int RwD3D9SetFogDensity( float density )
{
    if ( GetCurrentFogDensity() == density )
        return 1;

    return _RwD3D9SetFogDensity( density );
}

float RwD3D9GetFogDensity( void )
{
    return GetCurrentFogDensity();
}

inline int RwD3D9ResetFogDensity( void )
{
    _RwD3D9UpdateFogDensity( GetCurrentFogDensity() );
    return 1;
}

inline void _RwD3D9UpdateFogTableMode( rwFogModeState mode )
{
    D3DFOGMODE fogMode = _fogTableModes[ mode ];

    if ( ( *(unsigned int*)0x00C9BF24 & 0x100100 ) != 0 )
    {
        RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, fogMode );
        RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
    }
    else
    {
        RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
        RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE, fogMode );
    }
}

inline int _RwD3D9SetFogTableMode( rwFogModeState mode )
{
    _RwD3D9UpdateFogTableMode( mode );

    GetCurrentFogTableIndex() = mode;
    return 1;
}

int RwD3D9SetFogTableMode( rwFogModeState mode )
{
    int result = 1;

    if ( GetCurrentFogTableIndex() != mode )
    {
        result = _RwD3D9SetFogTableMode( mode );
    }

    if ( mode == 1 )
    {
        RwCamera *camera = *(RwCamera**)0x00C9BCC0;

        float fogStart = camera->fog;
        float fogEnd = camera->farplane;

        // MTA fix: make sure fog start never overshoots fog end.
        if ( fogStart > fogEnd )
        {
            fogStart = fogEnd;
        }

        RwD3D9SetRenderState( D3DRS_FOGSTART, F2DW( fogStart ) );
        RwD3D9SetRenderState( D3DRS_FOGEND, F2DW( fogEnd ) );
    }

    return result;
}

rwFogModeState RwD3D9GetFogTableMode( void )
{
    return GetCurrentFogTableIndex();
}

inline int RwD3D9ResetFogTableMode( void )
{
    _RwD3D9UpdateFogTableMode( GetCurrentFogTableIndex() );
    return 1;
}

static const D3DSHADEMODE _shadeModeTable[] =
{
    (D3DSHADEMODE)0,
    D3DSHADE_FLAT,
    D3DSHADE_GOURAUD
};

inline int _RwD3D9SetShadeMode( rwShadeModeState mode )
{
    RwD3D9SetRenderState( D3DRS_SHADEMODE, _shadeModeTable[mode] );

    GetCurrentShadeMode() = mode;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FEC70 (1.0 EU): 0x007FECB0
int RwD3D9SetShadeMode( rwShadeModeState mode )
{
    if ( GetCurrentShadeMode() == mode )
        return 1;

    return _RwD3D9SetShadeMode( mode );
}

rwShadeModeState RwD3D9GetShadeMode( void )
{
    return GetCurrentShadeMode();
}

inline int RwD3D9ResetShadeMode( void )
{
    RwD3D9SetRenderState( D3DRS_SHADEMODE, _shadeModeTable[ GetCurrentShadeMode() ] );
    return 1;
}

inline int _RwD3D9SetZWriteEnable( rwDeviceValue_t enable )
{
    RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, enable );

    GetCurrentZWriteEnable() = enable;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FEA70 (1.0 EU): 0x007FEAB0
int RwD3D9SetZWriteEnable( rwDeviceValue_t enable )
{
    if ( GetCurrentZWriteEnable() == enable )
        return 1;

    if ( GetCurrentDepthFunctionEnable() == false )
    {
        RwD3D9SetRenderState( D3DRS_ZENABLE, enable );
    }

    return _RwD3D9SetZWriteEnable( enable );
}

rwDeviceValue_t RwD3D9GetZWriteEnable( void )
{
    return GetCurrentZWriteEnable();
}

inline int RwD3D9ResetZWriteEnable( void )
{
    RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, GetCurrentZWriteEnable() );
    return 1;
}

static inline void _RwD3D9UpdateDepthFunctionEnable( rwDeviceValue_t enable )
{
    RwD3D9SetRenderState( D3DRS_ZFUNC, ( enable ) ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS );
    RwD3D9SetRenderState( D3DRS_ZENABLE, enable );
}

inline int _RwD3D9SetDepthFunctionEnable( rwDeviceValue_t enable )
{
    _RwD3D9UpdateDepthFunctionEnable( enable );

    GetCurrentDepthFunctionEnable() = enable;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FEB70 (1.0 EU): 0x007FEBB0
int RwD3D9SetDepthFunctionEnable( rwDeviceValue_t enable )
{
    if ( GetCurrentDepthFunctionEnable() == enable )
        return 1;

    if ( GetCurrentZWriteEnable() == false )
    {
        RwD3D9SetRenderState( D3DRS_ZENABLE, enable );
    }

    return _RwD3D9SetDepthFunctionEnable( enable );
}

rwDeviceValue_t RwD3D9GetDepthFunctionEnable( void )
{
    return GetCurrentDepthFunctionEnable();
}

inline int RwD3D9ResetDepthFunctionEnable( void )
{
    _RwD3D9UpdateDepthFunctionEnable( GetCurrentDepthFunctionEnable() );
    return 1;
}

inline int _RwD3D9SetStencilEnable( rwDeviceValue_t enable )
{
    RwD3D9SetRenderState( D3DRS_STENCILENABLE, enable );

    GetCurrentStencilEnable() = enable;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE1A0 (1.0 EU): 0x007FE1E0
int RwD3D9SetStencilEnable( rwDeviceValue_t enable )
{
    if ( GetCurrentStencilEnable() == enable )
        return 1;

    return _RwD3D9SetStencilEnable( enable );
}

rwDeviceValue_t RwD3D9GetStencilEnable( void )
{
    return GetCurrentStencilEnable();
}

inline int RwD3D9ResetStencilEnable( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILENABLE, GetCurrentStencilEnable() );
    return 1;
}

static const D3DSTENCILOP _stencilOpTable[] =
{
    (D3DSTENCILOP)0,
    D3DSTENCILOP_KEEP,
    D3DSTENCILOP_ZERO,
    D3DSTENCILOP_REPLACE,
    D3DSTENCILOP_INCRSAT,
    D3DSTENCILOP_DECRSAT,
    D3DSTENCILOP_INVERT,
    D3DSTENCILOP_INCR,
    D3DSTENCILOP_DECR
};

inline int _RwD3D9SetStencilFail( rwStencilOpState opMode )
{
    RwD3D9SetRenderState( D3DRS_STENCILFAIL, _stencilOpTable[opMode] );

    GetCurrentStencilFail() = opMode;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE1F0 (1.0 EU): 0x007FE230
int RwD3D9SetStencilFail( rwStencilOpState opMode )
{
    if ( GetCurrentStencilFail() == opMode )
        return 1;

    return _RwD3D9SetStencilFail( opMode );
}

rwStencilOpState RwD3D9GetStencilFail( void )
{
    return GetCurrentStencilFail();
}

inline int RwD3D9ResetStencilFail( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILFAIL, _stencilOpTable[GetCurrentStencilFail()] );
    return 1;
}

inline int _RwD3D9SetStencilZFail( rwStencilOpState opMode )
{
    RwD3D9SetRenderState( D3DRS_STENCILZFAIL, _stencilOpTable[opMode] );

    GetCurrentStencilZFail() = opMode;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE240 (1.0 EU): 0x007FE280
int RwD3D9SetStencilZFail( rwStencilOpState opMode )
{
    if ( GetCurrentStencilZFail() == opMode )
        return 1;

    return _RwD3D9SetStencilZFail( opMode );
}

rwStencilOpState RwD3D9GetStencilZFail( void )
{
    return GetCurrentStencilZFail();
}

inline int RwD3D9ResetStencilZFail( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILZFAIL, _stencilOpTable[GetCurrentStencilZFail()] );
    return 1;
}

inline int _RwD3D9SetStencilPass( rwStencilOpState opMode )
{
    RwD3D9SetRenderState( D3DRS_STENCILPASS, _stencilOpTable[opMode] );

    GetCurrentStencilPass() = opMode;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE290 (1.0 EU): 0x007FE2D0
int RwD3D9SetStencilPass( rwStencilOpState opMode )
{
    if ( GetCurrentStencilPass() == opMode )
        return 1;

    return _RwD3D9SetStencilPass( opMode );
}

rwStencilOpState RwD3D9GetStencilPass( void )
{
    return GetCurrentStencilPass();
}

inline int RwD3D9ResetStencilPass( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILPASS, _stencilOpTable[GetCurrentStencilPass()] );
    return 1;
}

static const D3DCMPFUNC _cmpOpTable[] =
{
    (D3DCMPFUNC)0,
    D3DCMP_NEVER,
    D3DCMP_LESS,
    D3DCMP_EQUAL,
    D3DCMP_LESSEQUAL,
    D3DCMP_GREATER,
    D3DCMP_NOTEQUAL,
    D3DCMP_GREATEREQUAL,
    D3DCMP_ALWAYS
};

inline int _RwD3D9SetStencilFunc( rwCompareOpState cmpOp )
{
    RwD3D9SetRenderState( D3DRS_STENCILFUNC, _cmpOpTable[cmpOp] );

    GetCurrentStencilFunc() = cmpOp;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE2E0 (1.0 EU): 0x007FE320
int RwD3D9SetStencilFunc( rwCompareOpState cmpOp )
{
    if ( GetCurrentStencilFunc() == cmpOp )
        return 1;

    return _RwD3D9SetStencilFunc( cmpOp );
}

rwCompareOpState RwD3D9GetStencilFunc( void )
{
    return GetCurrentStencilFunc();
}

inline int RwD3D9ResetStencilFunc( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILFUNC, _cmpOpTable[GetCurrentStencilFunc()] );
    return 1;
}

inline int _RwD3D9SetStencilRef( rwDeviceValue_t ref )
{
    RwD3D9SetRenderState( D3DRS_STENCILREF, ref );

    GetCurrentStencilRef() = ref;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE330 (1.0 EU): 0x007FE370
int RwD3D9SetStencilRef( rwDeviceValue_t ref )
{
    if ( GetCurrentStencilRef() == ref )
        return 1;

    return _RwD3D9SetStencilRef( ref );
}

rwDeviceValue_t RwD3D9GetStencilRef( void )
{
    return GetCurrentStencilRef();
}

inline int RwD3D9ResetStencilRef( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILREF, GetCurrentStencilRef() );
    return 1;
}

inline int _RwD3D9SetStencilMask( rwDeviceValue_t mask )
{
    RwD3D9SetRenderState( D3DRS_STENCILMASK, mask );

    GetCurrentStencilMask() = mask;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE380 (1.0 EU): 0x007FE3C0
int RwD3D9SetStencilMask( rwDeviceValue_t mask )
{
    if ( GetCurrentStencilMask() == mask )
        return 1;

    return _RwD3D9SetStencilMask( mask );
}

rwDeviceValue_t RwD3D9GetStencilMask( void )
{
    return GetCurrentStencilMask();
}

inline int RwD3D9ResetStencilMask( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILMASK, GetCurrentStencilMask() );
    return 1;
}

inline int _RwD3D9SetStencilWriteMask( rwDeviceValue_t mask )
{
    RwD3D9SetRenderState( D3DRS_STENCILWRITEMASK, mask );

    GetCurrentStencilWriteMask() = mask;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FE3D0 (1.0 EU): 0x007FE410
int RwD3D9SetStencilWriteMask( rwDeviceValue_t mask )
{
    if ( GetCurrentStencilWriteMask() == mask )
        return 1;

    return _RwD3D9SetStencilWriteMask( mask );
}

rwDeviceValue_t RwD3D9GetStencilWriteMask( void )
{
    return GetCurrentStencilWriteMask();
}

inline int RwD3D9ResetStencilWriteMask( void )
{
    RwD3D9SetRenderState( D3DRS_STENCILWRITEMASK, GetCurrentStencilWriteMask() );
    return 1;
}

struct _samplerFilterMode
{
    D3DTEXTUREFILTERTYPE filterType;
    D3DTEXTUREFILTERTYPE mipFilterType;
};

static const _samplerFilterMode _samplerFilterModes[] =
{
    { D3DTEXF_NONE, D3DTEXF_NONE },
    { D3DTEXF_POINT, D3DTEXF_NONE },
    { D3DTEXF_LINEAR, D3DTEXF_NONE },
    { D3DTEXF_POINT, D3DTEXF_POINT },
    { D3DTEXF_LINEAR, D3DTEXF_POINT },
    { D3DTEXF_POINT, D3DTEXF_LINEAR },
    { D3DTEXF_LINEAR, D3DTEXF_LINEAR },
    { D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR }
};

static inline void _RwD3D9RasterStageUpdateFilterMode( unsigned int stageIdx, const _samplerFilterMode& mode )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MINFILTER, mode.filterType );
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MAGFILTER, mode.filterType );

    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MIPFILTER, mode.mipFilterType );
}

inline int _RwD3D9RasterStageSetFilterMode( unsigned int stageIdx, rwRasterStageFilterMode filterMode )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    const _samplerFilterMode& mode = _samplerFilterModes[filterMode];

    _RwD3D9RasterStageUpdateFilterMode( stageIdx, mode );

    rasterStage.filterType = filterMode;
    return 1;
}

int RwD3D9RasterStageSetFilterMode( unsigned int stageIdx, rwRasterStageFilterMode filterMode )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    if ( rasterStage.filterType == filterMode )
        return 1;

    return _RwD3D9RasterStageSetFilterMode( stageIdx, filterMode );
}

rwRasterStageFilterMode RwD3D9RasterStageGetFilterMode( unsigned int stageIdx )
{
    return GetRasterStageInfo( stageIdx ).filterType;
}

inline int RwD3D9RasterStageResetFilterMode( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    const _samplerFilterMode& mode = _samplerFilterModes[ rasterStage.filterType ];

    _RwD3D9RasterStageUpdateFilterMode( stageIdx, mode );
    return 1;
}

static const D3DTEXTUREADDRESS _textureAddressModes[] =
{
    (D3DTEXTUREADDRESS)0,
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER
};

static inline void _RwD3D9RasterStageUpdateAddressModeU( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_ADDRESSU, _textureAddressModes[ modeIdx ] );
}

inline int _RwD3D9RasterStageSetAddressModeU( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeU( stageIdx, modeIdx );

    rasterStage.u_addressMode = modeIdx;
    return 1;
}

int RwD3D9RasterStageSetAddressModeU( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    if ( rasterStage.u_addressMode == modeIdx )
        return 1;

    return _RwD3D9RasterStageSetAddressModeU( stageIdx, modeIdx );
}

rwRasterStageAddressMode RwD3D9RasterStageGetAddressModeU( unsigned int stageIdx )
{
    return GetRasterStageInfo( stageIdx ).u_addressMode;
}

int RwD3D9RasterStageResetAddressModeU( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeU( stageIdx, rasterStage.u_addressMode );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateAddressModeV( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_ADDRESSV, _textureAddressModes[modeIdx] );
}

inline int _RwD3D9RasterStageSetAddressModeV( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeV( stageIdx, modeIdx );

    rasterStage.v_addressMode = modeIdx;
    return 1;
}

int RwD3D9RasterStageSetAddressModeV( unsigned int stageIdx, rwRasterStageAddressMode modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    if ( rasterStage.v_addressMode == modeIdx )
        return 1;

    return _RwD3D9RasterStageSetAddressModeV( stageIdx, modeIdx );
}

rwRasterStageAddressMode RwD3D9RasterStageGetAddressModeV( unsigned int stageIdx )
{
    return GetRasterStageInfo( stageIdx ).v_addressMode;
}

inline int RwD3D9RasterStageResetAddressModeV( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeV( stageIdx, rasterStage.v_addressMode );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateBorderColor( unsigned int stageIdx, unsigned int color )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_BORDERCOLOR, color );
}

inline int _RwD3D9RasterStageSetBorderColor( unsigned int stageIdx, unsigned int color )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateBorderColor( stageIdx, color );

    rasterStage.borderColor = color;
    return 1;
}

int RwD3D9RasterStageSetBorderColor( unsigned int stageIdx, unsigned int color )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    if ( rasterStage.borderColor == color )
        return 1;

    return _RwD3D9RasterStageSetBorderColor( stageIdx, color );
}

unsigned int RwD3D9RasterStageGetBorderColor( unsigned int stageIdx )
{
    return GetRasterStageInfo( stageIdx ).borderColor;
}

inline int RwD3D9RasterStageResetBorderColor( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateBorderColor( stageIdx, rasterStage.borderColor );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateMaxAnisotropy( unsigned int stageIdx, int maxAnisotropy )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MAXANISOTROPY, (DWORD)maxAnisotropy );
}

inline int _RwD3D9RasterStageSetMaxAnisotropy( unsigned int stageIdx, int maxAnisotropy )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateMaxAnisotropy( stageIdx, maxAnisotropy );

    rasterStage.maxAnisotropy = maxAnisotropy;
    return 1;
}

int RwD3D9RasterStageSetMaxAnisotropy( unsigned int stageIdx, int maxAnisotropy )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    if ( rasterStage.maxAnisotropy == maxAnisotropy )
        return 1;

    return _RwD3D9RasterStageSetMaxAnisotropy( stageIdx, maxAnisotropy );
}

int RwD3D9RasterStageGetMaxAnisotropy( unsigned int stageIdx )
{
    return GetRasterStageInfo( stageIdx ).maxAnisotropy;
}

inline int RwD3D9RasterStageResetMaxAnisotropy( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateMaxAnisotropy( stageIdx, rasterStage.maxAnisotropy );
    return 1;
}

static const D3DBLEND blendModes[] =
{
    (D3DBLEND)0,
    D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCCOLOR,
    D3DBLEND_INVSRCCOLOR,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA,
    D3DBLEND_DESTCOLOR,
    D3DBLEND_INVDESTCOLOR,
    D3DBLEND_SRCALPHASAT
};

inline int RwD3D9ResetSrcBlend( void )
{
    RwD3D9SetRenderState( D3DRS_SRCBLEND, blendModes[GetCurrentSrcBlend()] );
    return 1;
}

inline int _RwD3D9SetSrcBlend( rwBlendModeState blendMode )
{
    RwD3D9SetRenderState( D3DRS_SRCBLEND, blendModes[blendMode] );

    GetCurrentSrcBlend() = blendMode;
    return 1;
}

int RwD3D9SetSrcBlend( rwBlendModeState blendMode )
{
    if ( GetCurrentSrcBlend() != blendMode )
        _RwD3D9SetSrcBlend( blendMode );

    return 1;
}

rwBlendModeState RwD3D9GetSrcBlend( void )
{
    return GetCurrentSrcBlend();
}

inline int RwD3D9ResetDstBlend( void )
{
    RwD3D9SetRenderState( D3DRS_DESTBLEND, blendModes[GetCurrentDstBlend()] );
    return 1;
}

inline int _RwD3D9SetDstBlend( rwBlendModeState blendMode )
{
    RwD3D9SetRenderState( D3DRS_DESTBLEND, blendModes[blendMode] );

    GetCurrentDstBlend() = blendMode;
    return 1;
}

int RwD3D9SetDstBlend( rwBlendModeState blendMode )
{
    if ( GetCurrentDstBlend() != blendMode )
        _RwD3D9SetDstBlend( blendMode );

    return 1;
}

rwBlendModeState RwD3D9GetDstBlend( void )
{
    return GetCurrentDstBlend();
}

inline int RwD3D9ResetAlphaFunc( void )
{
    RwD3D9SetRenderState( D3DRS_ALPHAFUNC, _cmpOpTable[ GetCurrentAlphaRefFunc() ] );
    return 1;
}

inline int _RwD3D9SetAlphaFunc( rwCompareOpState cmpOp )
{
    RwD3D9SetRenderState( D3DRS_ALPHAFUNC, _cmpOpTable[cmpOp] );

    GetCurrentAlphaRefFunc() = cmpOp;
    return 1;
}

int RwD3D9SetAlphaFunc( rwCompareOpState cmpOp )
{
    if ( GetCurrentAlphaRefFunc() == cmpOp )
        return 1;

    RwD3D9SetVirtualAlphaTestState( cmpOp != 8 );

    return _RwD3D9SetAlphaFunc( cmpOp );
}

rwCompareOpState RwD3D9GetAlphaFunc( void )
{
    return GetCurrentAlphaRefFunc();
}

static const D3DCULL cullModes[] =
{
    (D3DCULL)0,
    D3DCULL_NONE,
    D3DCULL_CW,
    D3DCULL_CCW
};

inline int _RwD3D9SetCullMode( rwCullModeState cullMode )
{
    RwD3D9SetRenderState( D3DRS_CULLMODE, cullModes[cullMode] );

    GetCurrentCullMode() = cullMode;
    return 1;
}

// Binary offsets: (1.0 US): 0x007FED00 (1.0 EU): 0x007FED40
int RwD3D9SetCullMode( rwCullModeState cullMode )
{
    if ( GetCurrentCullMode() == cullMode )
        return 1;

    return _RwD3D9SetCullMode( cullMode );
}

rwCullModeState RwD3D9GetCullMode( void )
{
    return GetCurrentCullMode();
}

inline int RwD3D9ResetCullMode( void )
{
    RwD3D9SetRenderState( D3DRS_CULLMODE, cullModes[ GetCurrentCullMode() ] );
    return 1;
}

inline int _RwD3D9SetFogEnable( rwDeviceValue_t enable )
{
    RwD3D9SetRenderState( D3DRS_FOGENABLE, enable );

    GetCurrentFogEnable() = enable;
    return 1;
}

int RwD3D9SetFogEnable( rwDeviceValue_t enable )
{
    if ( GetCurrentFogEnable() == enable )
        return 1;

    if ( enable == TRUE && !IS_ANY_FLAG( *(unsigned int*)0x00C9BF20, 0x180 ) )
        return 1;

    return _RwD3D9SetFogEnable( enable );
}

rwDeviceValue_t RwD3D9GetFogEnable( void )
{
    return GetCurrentFogEnable();
}

inline int RwD3D9ResetFogEnable( void )
{
    RwD3D9SetRenderState( D3DRS_FOGENABLE, GetCurrentFogEnable() );
    return 1;
}

/*=========================================================
    RwD3D9_RwSetRenderState

    Arguments:
        deviceType - type of the device state to change
        value - parameter to pass to the device
    Purpose:
        Changes device states of the RenderWare rendering
        context. This is the function that shall be used by
        game code.
    Binary offsets:
        (1.0 US): 0x007FE420
        (1.0 EU): 0x007FE460
=========================================================*/
template <typename enumType>
inline bool RwD3D9DeviceValueToEnumRanged( rwDeviceValue_t value, enumType begin, enumType end, enumType& outValue )
{
#ifndef RENDERWARE_TRUST_IMPLEMENTATION
    bool success = ( value >= begin && value <= end );
#else
    bool success = true;
#endif //RENDERWARE_TRUST_IMPLEMENTATION

    if ( success )
    {
        outValue = (enumType)value;
    }

    return success;
}

struct FogModeDispatcher
{
    typedef rwFogModeState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwFogModeState& outFogMode )         { return RwD3D9DeviceValueToEnumRanged( value, RWFOG_DISABLE, RWFOG_EXP2, outFogMode ); }
    static inline int Execute( rwFogModeState outFogMode )                                          { return RwD3D9SetFogTableMode( outFogMode ); }
};
struct BlendModeDispatcher
{
    typedef rwBlendModeState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwBlendModeState& outBlendMode )     { return RwD3D9DeviceValueToEnumRanged( value, RWBLEND_ZERO, RWBLEND_SRCALPHASAT, outBlendMode ); }
};
struct SrcBlendModeDispatcher : BlendModeDispatcher
{
    static inline int Execute( rwBlendModeState outBlendMode )                                      { return RwD3D9SetSrcBlend( outBlendMode ); }
};
struct DstBlendModeDispatcher : BlendModeDispatcher
{
    static inline int Execute( rwBlendModeState outBlendMode )                                      { return RwD3D9SetDstBlend( outBlendMode ); }
};
struct ShadeModeDispatcher
{
    typedef rwShadeModeState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwShadeModeState& outShadeMode )     { return RwD3D9DeviceValueToEnumRanged( value, RWSHADE_FLAT, RWSHADE_GOURAUD, outShadeMode ); }
    static inline int Execute( rwShadeModeState outShadeMode )                                      { return RwD3D9SetShadeMode( outShadeMode ); }
};
struct CullModeDispatcher
{
    typedef rwCullModeState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwCullModeState& outCullMode )       { return RwD3D9DeviceValueToEnumRanged( value, RWCULL_NONE, RWCULL_COUNTERCLOCKWISE, outCullMode ); }
    static inline int Execute( rwCullModeState outCullMode )                                        { return RwD3D9SetCullMode( outCullMode ); }
};
struct StencilOpDispatcher
{
    typedef rwStencilOpState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwStencilOpState& outStencilOp )     { return RwD3D9DeviceValueToEnumRanged( value, RWSTENCIL_KEEP, RWSTENCIL_DECR, outStencilOp ); }
};
struct StencilFailDispatcher : StencilOpDispatcher
{
    static inline int Execute( rwStencilOpState outStencilOp )                                      { return RwD3D9SetStencilFail( outStencilOp ); }
};
struct StencilZFailDispatcher : StencilOpDispatcher
{
    static inline int Execute( rwStencilOpState outStencilOp )                                      { return RwD3D9SetStencilZFail( outStencilOp ); }
};
struct StencilPassDispatcher : StencilOpDispatcher
{
    static inline int Execute( rwStencilOpState outStencilOp )                                      { return RwD3D9SetStencilPass( outStencilOp ); }
};
struct CompareOpDispatcher
{
    typedef rwCompareOpState enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwCompareOpState& outCompareOp )     { return RwD3D9DeviceValueToEnumRanged( value, RWCMP_NEVER, RWCMP_ALWAYS, outCompareOp ); }
};
struct StencilFuncDispatcher : CompareOpDispatcher
{
    static inline int Execute( rwCompareOpState outCompareOp )                                      { return RwD3D9SetStencilFunc( outCompareOp ); }
};
struct AlphaFuncDispatcher : CompareOpDispatcher
{
    static inline int Execute( rwCompareOpState outCompareOp )                                      { return RwD3D9SetAlphaFunc( outCompareOp ); }
};

struct TexAddressModeDispatcher
{
    typedef rwRasterStageAddressMode enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwRasterStageAddressMode& outMode )  { return RwD3D9DeviceValueToEnumRanged( value, RWTEXADDRESS_WRAP, RWTEXADDRESS_BORDER, outMode ); } 
};
struct CombinedTexAddressModeDispatcher : TexAddressModeDispatcher
{
    static inline int Execute( rwRasterStageAddressMode outMode )
    {
        RwD3D9RasterStageSetAddressModeU( 0, outMode );
        RwD3D9RasterStageSetAddressModeV( 0, outMode );
        return 1;
    }
};
struct TexAddressModeUDispatcher : TexAddressModeDispatcher
{
    static inline int Execute( rwRasterStageAddressMode outMode )
    {
        RwD3D9RasterStageSetAddressModeU( 0, outMode );
        return 1;
    }
};
struct TexAddressModeVDispatcher : TexAddressModeDispatcher
{
    static inline int Execute( rwRasterStageAddressMode outMode )
    {
        RwD3D9RasterStageSetAddressModeV( 0, outMode );
        return 1;
    }
};
struct TextureFilterModeDispatcher
{
    typedef rwRasterStageFilterMode enumType;

    static inline bool FromDeviceValue( rwDeviceValue_t value, rwRasterStageFilterMode& outMode )   { return RwD3D9DeviceValueToEnumRanged( value, RWFILTER_DISABLE, RWFILTER_ANISOTROPY, outMode ); }
    static inline int Execute( rwRasterStageFilterMode outMode )
    {
        // Make sure max anisotropy does not overshoot 1.
        {
            int maxAliasing = RwD3D9RasterStageGetMaxAnisotropy( 0 );

            if ( maxAliasing > 1 )
            {
                _RwD3D9RasterStageSetMaxAnisotropy( 0, 1 );   
            }
        }

        // todo
        return RwD3D9RasterStageSetFilterMode( 0, outMode );
    }
};

template <typename dispatcherType>
AINLINE int RwD3D9RenderStateSet( rwDeviceValue_t value )
{
    dispatcherType::enumType theEnumValue;

    int iResult = false;

    if ( dispatcherType::FromDeviceValue( value, theEnumValue ) )
    {
        iResult = dispatcherType::Execute( theEnumValue );
    }
    else
    {
#ifdef _DEBUG
        assert( 0 );
#endif
    }

    return iResult;
}

inline int RwD3D9RenderStateSetFogEnable( rwDeviceValue_t value )               { return RwD3D9SetFogEnable( value ); }
inline int RwD3D9RenderStateSetFogColor( rwDeviceValue_t value )                { return RwD3D9SetFogColor( value ); }
inline int RwD3D9RenderStateSetFogMode( rwDeviceValue_t value )                 { return RwD3D9RenderStateSet <FogModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetFogDensity( rwDeviceValue_t value )              { return RwD3D9SetFogDensity( *(float*)value ); }
inline int RwD3D9RenderStateSetCombinedTexAddressMode( rwDeviceValue_t value )  { return RwD3D9RenderStateSet <CombinedTexAddressModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetTexAddressModeU( rwDeviceValue_t value )         { return RwD3D9RenderStateSet <TexAddressModeUDispatcher> ( value ); }
inline int RwD3D9RenderStateSetTexAddressModeV( rwDeviceValue_t value )         { return RwD3D9RenderStateSet <TexAddressModeVDispatcher> ( value ); }

// Binary offsets: (1.0 US): 0x007FDB80 (1.0 EU): 0x007FDBC0
int __cdecl RwD3D9RenderStateSetTextureFilter( rwDeviceValue_t mode )
{
    return RwD3D9RenderStateSet <TextureFilterModeDispatcher> ( mode );
}

inline int RwD3D9RenderStateSetCurrentRaster( rwDeviceValue_t value )
{
    // maybe check that value is a valid pointer to an active raster?
    return RwD3D9SetRasterForStage( (RwRaster*)value, 0 );
}
inline int RwD3D9RenderStateSetZWriteEnable( rwDeviceValue_t value )            { return RwD3D9SetZWriteEnable( value ); }
inline int RwD3D9RenderStateSetZTestEnable( rwDeviceValue_t value )             { return RwD3D9SetDepthFunctionEnable( value ); }
inline int RwD3D9RenderStateSetSrcBlend( rwDeviceValue_t value )                { return RwD3D9RenderStateSet <SrcBlendModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetDstBlend( rwDeviceValue_t value )                { return RwD3D9RenderStateSet <DstBlendModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetShadeMode( rwDeviceValue_t value )               { return RwD3D9RenderStateSet <ShadeModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetTextureBorderColor( rwDeviceValue_t value )      { return RwD3D9RasterStageSetBorderColor( 0, value ); }
inline int RwD3D9RenderStateSetCullMode( rwDeviceValue_t value )                { return RwD3D9RenderStateSet <CullModeDispatcher> ( value ); }
inline int RwD3D9RenderStateSetStencilEnable( rwDeviceValue_t value )           { return RwD3D9SetStencilEnable( value ); }
inline int RwD3D9RenderStateSetStencilFail( rwDeviceValue_t value )             { return RwD3D9RenderStateSet <StencilFailDispatcher> ( value ); }
inline int RwD3D9RenderStateSetStencilZFail( rwDeviceValue_t value )            { return RwD3D9RenderStateSet <StencilZFailDispatcher> ( value ); }
inline int RwD3D9RenderStateSetStencilPass( rwDeviceValue_t value )             { return RwD3D9RenderStateSet <StencilPassDispatcher> ( value ); }
inline int RwD3D9RenderStateSetStencilFunc( rwDeviceValue_t value )             { return RwD3D9RenderStateSet <StencilFuncDispatcher> ( value ); }
inline int RwD3D9RenderStateSetStencilRef( rwDeviceValue_t value )              { return RwD3D9SetStencilRef( value ); }
inline int RwD3D9RenderStateSetStencilMask( rwDeviceValue_t value )             { return RwD3D9SetStencilMask( value ); }
inline int RwD3D9RenderStateSetStencilWriteMask( rwDeviceValue_t value )        { return RwD3D9SetStencilWriteMask( value ); }
inline int RwD3D9RenderStateSetAlphaFunc( rwDeviceValue_t value )               { return RwD3D9RenderStateSet <AlphaFuncDispatcher> ( value ); }
inline int RwD3D9RenderStateSetAlphaRef( rwDeviceValue_t value )
{
    RwD3D9SetRenderState( D3DRS_ALPHAREF, value );
    return 1;
}

int __cdecl RwD3D9_RwSetRenderState( eRwDeviceCmd deviceType, rwDeviceValue_t value )
{
    switch( deviceType )
    {
    case RWSTATE_FOGENABLE:                 return RwD3D9RenderStateSetFogEnable( value );
    case RWSTATE_FOGCOLOR:                  return RwD3D9RenderStateSetFogColor( value );
    case RWSTATE_FOGMODE:                   return RwD3D9RenderStateSetFogMode( value );
    case RWSTATE_FOGDENSITY:                return RwD3D9RenderStateSetFogDensity( value );
    case RWSTATE_COMBINEDTEXADDRESSMODE:    return RwD3D9RenderStateSetCombinedTexAddressMode( value );
    case RWSTATE_UTEXADDRESSMODE:           return RwD3D9RenderStateSetTexAddressModeU( value );
    case RWSTATE_VTEXADDRESSMODE:           return RwD3D9RenderStateSetTexAddressModeV( value );
    case RWSTATE_TEXFILTER:                 return RwD3D9RenderStateSetTextureFilter( value );
    case RWSTATE_CURRENTRASTER:             return RwD3D9RenderStateSetCurrentRaster( value );
    case RWSTATE_ZWRITEENABLE:              return RwD3D9RenderStateSetZWriteEnable( value );
    case RWSTATE_ZTESTENABLE:               return RwD3D9RenderStateSetZTestEnable( value );
    case RWSTATE_SRCBLEND:                  return RwD3D9RenderStateSetSrcBlend( value );
    case RWSTATE_DSTBLEND:                  return RwD3D9RenderStateSetDstBlend( value );
    case RWSTATE_SHADEMODE:                 return RwD3D9RenderStateSetShadeMode( value );
    case RWSTATE_ALPHABLENDENABLE:          return RwD3D9RenderStateSetVertexAlphaEnabled( value );
    case RWSTATE_TEXTUREBORDERCOLOR:        return RwD3D9RenderStateSetTextureBorderColor( value );
    case RWSTATE_UNKNOWN1:                  return ( value == TRUE );
    case RWSTATE_CULLMODE:                  return RwD3D9RenderStateSetCullMode( value );
    case RWSTATE_STENCILENABLE:             return RwD3D9RenderStateSetStencilEnable( value );
    case RWSTATE_STENCILFAIL:               return RwD3D9RenderStateSetStencilFail( value );
    case RWSTATE_STENCILZFAIL:              return RwD3D9RenderStateSetStencilZFail( value );
    case RWSTATE_STENCILPASS:               return RwD3D9RenderStateSetStencilPass( value );
    case RWSTATE_STENCILFUNC:               return RwD3D9RenderStateSetStencilFunc( value );
    case RWSTATE_STENCILREF:                return RwD3D9RenderStateSetStencilRef( value );
    case RWSTATE_STENCILMASK:               return RwD3D9RenderStateSetStencilMask( value );
    case RWSTATE_STENCILWRITEMASK:          return RwD3D9RenderStateSetStencilWriteMask( value );
    case RWSTATE_ALPHAFUNC:                 return RwD3D9RenderStateSetAlphaFunc( value );
    case RWSTATE_ALPHAREF:                  return RwD3D9RenderStateSetAlphaRef( value );
    }

    return 1;
}

/*=========================================================
    RwD3D9_RwGetRenderState

    Arguments:
        deviceType - type of the device state to get
        valuePtr - pointer to write current state to
    Purpose:
        Retrieves the current render state value that is
        exposed by RenderWare in general. The returned
        RenderWare renderstate does not have to match the
        actual device state.
    Binary offsets:
        (1.0 US): 0x007FD810
        (1.0 EU): 0x007FD850
=========================================================*/
int __cdecl RwD3D9_RwGetRenderState( eRwDeviceCmd deviceType, rwDeviceValue_t *valuePtr )
{
    switch( deviceType )
    {
    case RWSTATE_FOGENABLE:                     *valuePtr = RwD3D9GetFogEnable(); return 1;
    case RWSTATE_FOGMODE:                       *valuePtr = RwD3D9GetFogTableMode(); return 1;
    case RWSTATE_FOGCOLOR:                      *valuePtr = RwD3D9GetFogColor(); return 1;
    case RWSTATE_FOGDENSITY:                    *(float*)valuePtr = RwD3D9GetFogDensity(); return 1;
    case RWSTATE_COMBINEDTEXADDRESSMODE:
        {
            bool success = ( RwD3D9RasterStageGetAddressModeU( 0 ) == RwD3D9RasterStageGetAddressModeV( 0 ) );

            *valuePtr = ( success ) ? ( TRUE ) : ( FALSE );
            
            return success;
        }
        break;
    case RWSTATE_UTEXADDRESSMODE:               *valuePtr = RwD3D9RasterStageGetAddressModeU( 0 ); return 1;
    case RWSTATE_VTEXADDRESSMODE:               *valuePtr = RwD3D9RasterStageGetAddressModeV( 0 ); return 1;
    case RWSTATE_TEXFILTER:                     *valuePtr = RwD3D9RasterStageGetFilterMode( 0 ); return 1;
    case RWSTATE_CURRENTRASTER:                 *(RwRaster**)valuePtr = RwD3D9GetRasterForStage( 0 ); return 1;
    case RWSTATE_ZWRITEENABLE:                  *valuePtr = RwD3D9GetZWriteEnable(); return 1;
    case RWSTATE_ZTESTENABLE:                   *valuePtr = RwD3D9GetDepthFunctionEnable(); return 1;
    case RWSTATE_SRCBLEND:                      *valuePtr = RwD3D9GetSrcBlend(); return 1;
    case RWSTATE_DSTBLEND:                      *valuePtr = RwD3D9GetDstBlend(); return 1;
    case RWSTATE_SHADEMODE:                     *valuePtr = RwD3D9GetShadeMode(); return 1;
    case RWSTATE_ALPHABLENDENABLE:              *valuePtr = RwD3D9RenderStateIsVertexAlphaEnabled(); return 1;
    case RWSTATE_TEXTUREBORDERCOLOR:            *valuePtr = RwD3D9RasterStageGetBorderColor( 0 ); return 1;
    case RWSTATE_CULLMODE:                      *valuePtr = RwD3D9GetCullMode(); return 1;
    case RWSTATE_STENCILENABLE:                 *valuePtr = RwD3D9GetStencilEnable(); return 1;
    case RWSTATE_STENCILFAIL:                   *valuePtr = RwD3D9GetStencilFail(); return 1;
    case RWSTATE_STENCILZFAIL:                  *valuePtr = RwD3D9GetStencilZFail(); return 1;
    case RWSTATE_STENCILPASS:                   *valuePtr = RwD3D9GetStencilPass(); return 1;
    case RWSTATE_STENCILFUNC:                   *valuePtr = RwD3D9GetStencilFunc(); return 1;
    case RWSTATE_STENCILREF:                    *valuePtr = RwD3D9GetStencilRef(); return 1;
    case RWSTATE_STENCILMASK:                   *valuePtr = RwD3D9GetStencilMask(); return 1;
    case RWSTATE_STENCILWRITEMASK:              *valuePtr = RwD3D9GetStencilWriteMask(); return 1;
    case RWSTATE_ALPHAFUNC:                     *valuePtr = RwD3D9GetAlphaFunc(); return 1;
    case RWSTATE_ALPHAREF:                      RwD3D9GetRenderState( D3DRS_ALPHAREF, *(DWORD*)valuePtr ); return 1;
    default:
        return 0;
    }

    return 1;
}

/*=========================================================
    RwD3D9InitializeDeviceStates

    Purpose:
        Sets up the RenderWare Direct3D 9 device state
        environment. After this, the RenderWare API is ready
        for usage.
    Binary offsets:
        (1.0 US): 0x007FCAC0
        (1.0 EU): 0x007FCB00
=========================================================*/
inline void RwD3D9SetupRenderingTransformation( void )
{
    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    RwD3D9SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
    RwD3D9SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
    RwD3D9SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
    
    RwD3D9SetRenderState( D3DRS_DITHERENABLE, IS_ANY_FLAG( *(unsigned int*)0x00C9BF24, 0x01 ) );
    RwD3D9SetRenderState( D3DRS_SPECULARENABLE, false );
    RwD3D9SetRenderState( D3DRS_LOCALVIEWER, false );
    RwD3D9SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
    RwD3D9SetRenderState( D3DRS_NORMALIZENORMALS, false );
}

void __cdecl RwD3D9InitializeDeviceStates( void )
{
    // Set current device states.
    RwD3D9InitializeCurrentStates();

    GetCurrentFogDensity() = 1.0f;

    // Set states.
    _RwD3D9SetFogEnable( false );
    _RwD3D9SetFogTableMode( RWFOG_LINEAR );
    _RwD3D9SetFogColor( 0x00000000 );
    _RwD3D9SetShadeMode( RWSHADE_GOURAUD );

    _RwD3D9SetDepthFunctionEnable( true );
    _RwD3D9SetZWriteEnable( true );
    RwD3D9SetRenderState( D3DRS_ZENABLE, true );

    _RwD3D9SetStencilEnable( false );
    _RwD3D9SetStencilFail( RWSTENCIL_KEEP );
    _RwD3D9SetStencilZFail( RWSTENCIL_KEEP );
    _RwD3D9SetStencilPass( RWSTENCIL_KEEP );
    _RwD3D9SetStencilFunc( RWCMP_ALWAYS );
    _RwD3D9SetStencilRef( 0 );
    _RwD3D9SetStencilMask( 0xFFFFFFFF );
    _RwD3D9SetStencilWriteMask( 0xFFFFFFFF );

    // Initialize the texture stage states and sampler states.
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    for ( unsigned int stage = 0; stage < MAX_SAMPLERS; stage++ )
    {
        d3d9RasterStage& rasterStage = GetRasterStageInfo( stage );

        // Make sure there is no texture attached.
        renderDevice->SetTexture( stage, NULL );

        rasterStage.raster = NULL;

        unsigned int samplerStartupFlags = *(unsigned int*)0x00C9BF40;

        bool makeSamplerActive = IS_ANY_FLAG( samplerStartupFlags, 0x200 );

        _RwD3D9RasterStageSetFilterMode( stage, ( makeSamplerActive ) ? RWFILTER_LINEAR : RWFILTER_DISABLE );
        _RwD3D9RasterStageSetAddressModeU( stage, RWTEXADDRESS_WRAP );
        _RwD3D9RasterStageSetAddressModeV( stage, RWTEXADDRESS_WRAP );
        _RwD3D9RasterStageSetBorderColor( stage, 0xFF000000 );
        _RwD3D9RasterStageSetMaxAnisotropy( stage, 1 );

        if ( stage != 0 )
        {
            RwD3D9SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    _RwD3D9SetSrcBlend( RWBLEND_SRCALPHA );
    _RwD3D9SetDstBlend( RWBLEND_INVSRCALPHA );

    _RwD3D9SetAlphaFunc( RWCMP_GREATER );

    RwD3D9SetRenderState( D3DRS_ALPHAREF, 0 );
    _RwD3D9SetAlphaEnable( false, true );

    RwTextureD3D9_InitializeDeviceStates();

    RwD3D9SetRenderState( D3DRS_LIGHTING, false );
    
    _RwD3D9SetCullMode( RWCULL_CLOCKWISE );

    RwD3D9SetupRenderingTransformation();

    RwD3D9ApplyDeviceStates();
}

/*=========================================================
    RwD3D9ResetDeviceStates

    Purpose:
        Resets the device states as if the device was reset
        to the beginning.
    Binary offsets:
        (1.0 US): 0x007FD100
        (1.0 EU): 0x007FD140
=========================================================*/
void __cdecl RwD3D9ResetDeviceStates( void )
{
    DWORD previousFogStart;
    DWORD previousFogEnd;
    DWORD previousAlphaRef;

    RwD3D9GetRenderState( D3DRS_FOGSTART, previousFogStart );
    RwD3D9GetRenderState( D3DRS_FOGEND, previousFogEnd );
    RwD3D9GetRenderState( D3DRS_ALPHAREF, previousAlphaRef );

    // Get the device states to the beginning.
    RwD3D9InitializeCurrentStates();

    RwD3D9ResetFogDensity();
    RwD3D9ResetFogEnable();
    RwD3D9ResetFogTableMode();

    RwD3D9SetRenderState( D3DRS_FOGSTART, previousFogStart );
    RwD3D9SetRenderState( D3DRS_FOGEND, previousFogEnd );
    RwD3D9ResetFogColor();
    
    RwD3D9ResetShadeMode();

    RwD3D9ResetZWriteEnable();
    RwD3D9ResetDepthFunctionEnable();

    RwD3D9ResetStencilEnable();
    RwD3D9ResetStencilFail();
    RwD3D9ResetStencilZFail();
    RwD3D9ResetStencilPass();
    RwD3D9ResetStencilFunc();
    RwD3D9ResetStencilRef();
    RwD3D9ResetStencilMask();
    RwD3D9ResetStencilWriteMask();

    // Initialize the texture stage states and sampler states.
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    for ( unsigned int stage = 0; stage < MAX_SAMPLERS; stage++ )
    {
        d3d9RasterStage& rasterStage = GetRasterStageInfo( stage );

        // Make sure there is no texture attached.
        renderDevice->SetTexture( stage, NULL );

        rasterStage.raster = NULL;

        RwD3D9RasterStageResetFilterMode( stage );
        RwD3D9RasterStageResetAddressModeU( stage );
        RwD3D9RasterStageResetAddressModeV( stage );
        RwD3D9RasterStageResetBorderColor( stage );
        RwD3D9RasterStageResetMaxAnisotropy( stage );

        if ( stage != 0 )
        {
            RwD3D9SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    // MTA extension: make sure we reset the light states.
    RpD3D9LightingOnDeviceReset();

    RwD3D9ResetSrcBlend();
    RwD3D9ResetDstBlend();

    RwD3D9ResetAlphaFunc();

    RwD3D9ResetAlphaEnable();
    RwD3D9SetRenderState( D3DRS_ALPHAREF, previousAlphaRef );

    RwTextureD3D9_ResetDeviceStates();

    RwD3D9SetRenderState( D3DRS_LIGHTING, false );
    
    RwD3D9ResetCullMode();

    RwD3D9SetupRenderingTransformation();

    RwD3D9ApplyDeviceStates();
}

// Module initialization.
void RwRenderStatesD3D9_Init( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007FE460, (DWORD)RwD3D9_RwSetRenderState, 5 );
        HookInstall( 0x007FD850, (DWORD)RwD3D9_RwGetRenderState, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FE420, (DWORD)RwD3D9_RwSetRenderState, 5 );
        HookInstall( 0x007FD810, (DWORD)RwD3D9_RwGetRenderState, 5 );
        break;
    }
}

void RwRenderStatesD3D9_Shutdown( void )
{
    return;
}
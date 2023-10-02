/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwRenderStates.h
*  PURPOSE:     RenderWare renderstate management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDERSTATES_
#define _RENDERWARE_RENDERSTATES_

inline DWORD F2DW( float val )  { return *(DWORD*)&val; }

// RenderState functions.
int RwD3D9SetFogColor                       ( rwDeviceValue_t color );
int RwD3D9ResetFogColor                     ( void );
int RwD3D9SetFogTableMode                   ( rwFogModeState mode );
int RwD3D9ResetFogTableMode                 ( void );
int RwD3D9SetShadeMode                      ( rwShadeModeState mode );
int RwD3D9ResetShadeMode                    ( void );
int RwD3D9SetZWriteEnable                   ( rwDeviceValue_t enable );
int RwD3D9ResetZWriteEnable                 ( void );
int RwD3D9SetDepthFunctionEnable            ( rwDeviceValue_t enable );
int RwD3D9ResetDepthFunctionEnable          ( void );
int RwD3D9SetStencilEnable                  ( rwDeviceValue_t enable );
int RwD3D9ResetStencilEnable                ( void );
int RwD3D9SetStencilFail                    ( rwStencilOpState opMode );
int RwD3D9ResetStencilFail                  ( void );
int RwD3D9SetStencilZFail                   ( rwStencilOpState opMode );
int RwD3D9ResetStencilZFail                 ( void );
int RwD3D9SetStencilPass                    ( rwStencilOpState opMode );
int RwD3D9ResetStencilPass                  ( void );
int RwD3D9SetStencilFunc                    ( rwCompareOpState cmpOp );
int RwD3D9ResetStencilFunc                  ( void );
int RwD3D9SetStencilRef                     ( rwDeviceValue_t ref );
int RwD3D9ResetStencilRef                   ( void );
int RwD3D9SetStencilMask                    ( rwDeviceValue_t mask );
int RwD3D9ResetStencilMask                  ( void );
int RwD3D9SetStencilWriteMask               ( rwDeviceValue_t mask );
int RwD3D9ResetStencilWriteMask             ( void );

// Renderstate get functions.
rwDeviceValue_t RwD3D9GetFogColor               ( void );
rwFogModeState RwD3D9GetFogTableMode            ( void );
rwShadeModeState RwD3D9GetShadeMode             ( void );
rwDeviceValue_t RwD3D9GetZWriteEnable           ( void );
rwDeviceValue_t RwD3D9GetDepthFunctionEnable    ( void );
rwDeviceValue_t RwD3D9GetStencilEnable          ( void );
rwStencilOpState RwD3D9GetStencilFail           ( void );
rwStencilOpState RwD3D9GetStencilZFail          ( void );
rwStencilOpState RwD3D9GetStencilPass           ( void );
rwCompareOpState RwD3D9GetStencilFunc           ( void );
rwDeviceValue_t RwD3D9GetStencilRef             ( void );
rwDeviceValue_t RwD3D9GetStencilMask            ( void );
rwDeviceValue_t RwD3D9GetStencilWriteMask       ( void );

// Raster stage functions.
int RwD3D9RasterStageSetFilterMode          ( unsigned int stageIdx, rwRasterStageFilterMode filterMode );
int RwD3D9RasterStageResetFilterMode        ( unsigned int stageIdx );
int RwD3D9RasterStageSetAddressModeU        ( unsigned int stageIdx, rwRasterStageAddressMode modeIdx );
int RwD3D9RasterStageResetAddressModeU      ( unsigned int stageIdx );
int RwD3D9RasterStageSetAddressModeV        ( unsigned int stageIdx, rwRasterStageAddressMode modeIdx );
int RwD3D9RasterStageResetAddressModeV      ( unsigned int stageIdx );
int RwD3D9RasterStageSetBorderColor         ( unsigned int stageIdx, unsigned int color );
int RwD3D9RasterStageResetBorderColor       ( unsigned int stageIdx );
int RwD3D9RasterStageSetMaxAnisotropy       ( unsigned int stageIdx, int maxAnisotropy );
int RwD3D9RasterStageResetMaxAnisotropy     ( unsigned int stageIdx );

// Raster stage get functions.
rwRasterStageFilterMode RwD3D9RasterStageGetFilterMode      ( unsigned int stageIdx );
rwRasterStageAddressMode RwD3D9RasterStageGetAddressModeU   ( unsigned int stageIdx );
rwRasterStageAddressMode RwD3D9RasterStageGetAddressModeV   ( unsigned int stageIdx );
unsigned int RwD3D9RasterStageGetBorderColor                ( unsigned int stageIdx );
int RwD3D9RasterStageGetMaxAnisotropy                       ( unsigned int stageIdx );

// More RenderState functions.
int RwD3D9SetSrcBlend                       ( rwBlendModeState blendMode );
int RwD3D9ResetSrcBlend                     ( void );
int RwD3D9SetDstBlend                       ( rwBlendModeState blendMode );
int RwD3D9ResetDstBlend                     ( void );
int RwD3D9SetAlphaFunc                      ( rwCompareOpState cmpOp );
int RwD3D9ResetAlphaFunc                    ( void );
int RwD3D9SetCullMode                       ( rwCullModeState cullMode );
int RwD3D9ResetCullMode                     ( void );
int RwD3D9SetFogEnable                      ( rwDeviceValue_t enable );
int RwD3D9ResetFogEnable                    ( void );

// More RenderState get functions.
rwBlendModeState RwD3D9GetSrcBlend              ( void );
rwBlendModeState RwD3D9GetDstBlend              ( void );
rwCompareOpState RwD3D9GetAlphaFunc             ( void );
rwCullModeState RwD3D9GetCullMode               ( void );
rwDeviceValue_t RwD3D9GetFogEnable              ( void );

// Initializators.
void __cdecl RwD3D9InitializeDeviceStates   ( void );
void __cdecl RwD3D9ResetDeviceStates        ( void );

// Module initialization.
void RwRenderStatesD3D9_Init( void );
void RwRenderStatesD3D9_Shutdown( void );

#endif //_RENDERWARE_RENDERSTATES_
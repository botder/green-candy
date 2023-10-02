/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTextureD3D9.h
*  PURPOSE:     RenderWare texture implementation for D3D9
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_TEXTURE_PLUGIN_D3D9_
#define _RENDERWARE_TEXTURE_PLUGIN_D3D9_

// Exported renderstate functions.
int _RwD3D9SetAlphaEnable                           ( rwDeviceValue_t blendEnable, rwDeviceValue_t testEnable );
int RwD3D9SetAlphaEnable                            ( rwDeviceValue_t blendEnable, rwDeviceValue_t testEnable );
int RwD3D9ResetAlphaEnable                          ( void );
int RwD3D9SetVirtualAlphaTestState                  ( bool enable );

// Texture render state event functions.
void RwTextureD3D9_InitializeDeviceStates           ( void );
void RwTextureD3D9_ResetDeviceStates                ( void );

// Main Direct3D 9 RenderWare texture runtime functions.
int __cdecl RwD3D9SetRasterForStage                 ( RwRaster *raster, unsigned int stageIdx );
RwRaster* __cdecl RwD3D9GetRasterForStage           ( unsigned int stageIdx );
int __cdecl RwD3D9SetTexture                        ( RwTexture *texture, unsigned int stageIndex );

int __cdecl RwD3D9RenderStateSetVertexAlphaEnabled              ( rwDeviceValue_t enabled );
rwDeviceValue_t __cdecl RwD3D9RenderStateIsVertexAlphaEnabled   ( void );

// TODO: think about improving my RenderWare definitions:
// * plugins should not plant OOP functions into the classes.
int __cdecl RwTextureHasAlpha                       ( RwTexture *tex );
IDirect3DBaseTexture9* RwTextureGetVideoData        ( RwTexture *tex );

// Module initialization.
void RwTextureD3D9_Init( void );
void RwTextureD3D9_Shutdown( void );

#endif //_RENDERWARE_TEXTURE_PLUGIN_D3D9_
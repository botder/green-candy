/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwUtilsD3D9.h
*  PURPOSE:     RenderWare utilities for Direct3D 9 interfacing
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_DIRECT3D9_UTILITIES_
#define _RENDERWARE_DIRECT3D9_UTILITIES_

// Utility functions.
bool __cdecl RwD3D9MatrixEqual( const D3DMATRIX& left, const D3DMATRIX& right );
int __cdecl RwD3D9SetTransform( D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix );
void __cdecl RwD3D9GetTransform( D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix );
bool __cdecl RwD3D9IsTransformActive( D3DTRANSFORMSTATETYPE state );

// Module initialization.
void RenderWareUtilsD3D9_Init( void );
void RenderWareUtilsD3D9_Shutdown( void );

#endif //_RENDERWARE_DIRECT3D9_UTILITIES_
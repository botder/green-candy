/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomicD3D9.h
*  PURPOSE:     RenderWare Atomic Direct3D 9 plugin definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_ATOMIC_DIRECT3D9_
#define _RENDERWARE_ATOMIC_DIRECT3D9_

#define RW_PLUGIN_BUCKETSORT        0x52729854

// RenderWare Atomic Direct3D Environment Reflection Map Plugin.
CEnvMapAtomicSA* __cdecl RpAtomicGetEnvironmentReflectionMap( RpAtomic *atomic );

// RenderWare Direct3D Atomic Bucket Sorting Extension.
void RpAtomicD3D9_RegisterPlugins( void );

// Module initialization.
void RpAtomicD3D9_Init( void );
void RpAtomicD3D9_Shutdown( void );

#endif //_RENDERWARE_ATOMIC_DIRECT3D9_
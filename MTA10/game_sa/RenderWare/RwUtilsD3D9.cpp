/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwUtilsD3D9.cpp
*  PURPOSE:     RenderWare utilities for Direct3D 9 interfacing
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RwRenderTools.hxx"

#include "../CRenderWareSA.state.transf.hxx"

// Location of the transformation state manager.
RwTransformationStateManager g_transformationStateManager;

bool __cdecl RwD3D9MatrixEqual( const D3DMATRIX& left, const D3DMATRIX& right )
{
    for ( unsigned int i = 0; i < 4; i++ )
    {
        for ( unsigned int j = 0; j < 4; j++ )
        {
            if ( left.m[i][j] != right.m[i][j] )
                return false;
        }
    }

    return true;
}

static const D3DMATRIX& GetIdentityMatrix( void )
{
    static const float identityMatrix[] =
    {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    return *(const D3DMATRIX*)identityMatrix;
}

static float realMatrix[] =
{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 1
};

static D3DMATRIX& GetRealMatrixCached( void )
{
    return *(D3DMATRIX*)realMatrix;
}

// Binary offsets: (1.0 US): 0x007FA520
int __cdecl RwD3D9SetTransformWorld( const RwMatrix *matrix )
{
    const D3DMATRIX *setToMatrix = NULL;

    if ( matrix )
    {
        // We must contruct a conforming matrix before passing it to Direct3D 9
        D3DMATRIX& realMatrix = GetRealMatrixCached();

        realMatrix.m[0][0] = matrix->right.x;
        realMatrix.m[0][1] = matrix->right.y;
        realMatrix.m[0][2] = matrix->right.z;

        realMatrix.m[1][0] = matrix->at.x;
        realMatrix.m[1][1] = matrix->at.y;
        realMatrix.m[1][2] = matrix->at.z;

        realMatrix.m[2][0] = matrix->up.x;
        realMatrix.m[2][1] = matrix->up.y;
        realMatrix.m[2][2] = matrix->up.z;

        realMatrix.m[3][0] = matrix->pos.x;
        realMatrix.m[3][1] = matrix->pos.y;
        realMatrix.m[3][2] = matrix->pos.z;

        setToMatrix = &realMatrix;
    }
    else
        setToMatrix = &GetIdentityMatrix();

    RwD3D9SetTransform( D3DTS_WORLDMATRIX( 0 ), setToMatrix );
    return true;
}

// Binary offsets: (1.0 US): 0x007FA390
int __cdecl RwD3D9SetTransform( D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix )
{
    const D3DMATRIX *setToMatrix = ( matrix ) ? matrix : &GetIdentityMatrix();

    {
        RwTransformationStateManager::stateAddress address;
        address.transType = state;

        g_transformationStateManager.SetDeviceStateChecked( address, *setToMatrix );
    }

    // Is this an optimization? What is this?
    if ( state == 0x100 )
        *(int*)0x00C97C68 = ( matrix == NULL );

    return true;
}

// Binary offsets: (1.0 US): 0x007FA4F0
void __cdecl RwD3D9GetTransform( D3DTRANSFORMSTATETYPE state, D3DMATRIX *matrix )
{
    if ( matrix )
    {
        RwTransformationStateManager::stateAddress address;
        address.transType = state;

        *matrix = g_transformationStateManager.GetDeviceState( address );
    }
}

// MTA extension
bool __cdecl RwD3D9IsTransformActive( D3DTRANSFORMSTATETYPE state )
{
    return true;
}

void RenderWareUtilsD3D9_Init( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007FA560, (DWORD)RwD3D9SetTransformWorld, 5 );
        HookInstall( 0x007FA3D0, (DWORD)RwD3D9SetTransform, 5 );
        HookInstall( 0x007FA530, (DWORD)RwD3D9GetTransform, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FA520, (DWORD)RwD3D9SetTransformWorld, 5 );
        HookInstall( 0x007FA390, (DWORD)RwD3D9SetTransform, 5 );
        HookInstall( 0x007FA4F0, (DWORD)RwD3D9GetTransform, 5 );
        break;
    }
}

void RenderWareUtilsD3D9_Shutdown( void )
{
}
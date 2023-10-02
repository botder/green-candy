/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.transf.hxx
*  PURPOSE:     RenderWare Direct3D 9 transformation state manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_TRANSFORMATION_STATE_MANAGER_
#define _RENDERWARE_TRANSFORMATION_STATE_MANAGER_

// Include generic logic.
#include "CRenderWareSA.stateman.hxx"

struct TransformationStateManager
{
    typedef D3DMATRIX valueType;

    struct stateAddress
    {
        D3DTRANSFORMSTATETYPE transType;

        AINLINE stateAddress( void )
        {
            transType = (D3DTRANSFORMSTATETYPE)0;
        }

        AINLINE void Increment( void )
        {
            if ( !IsEnd() )
            {
                ((DWORD&)transType)++;
            }
        }

        AINLINE bool IsEnd( void ) const
        {
            return (DWORD)transType == 260;
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return (unsigned int)transType;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->transType == right.transType );
        }
    };

    valueType invalidState;

    AINLINE TransformationStateManager( void )
    {
        // Set up invalid matrix.
        // It should be the identity matrix.
        invalidState.m[0][0] = 1.0f;
        invalidState.m[0][1] = 0.0f;
        invalidState.m[0][2] = 0.0f;
        invalidState.m[0][3] = 0.0f;

        invalidState.m[1][0] = 0.0f;
        invalidState.m[1][1] = 1.0f;
        invalidState.m[1][2] = 0.0f;
        invalidState.m[1][3] = 0.0f;

        invalidState.m[2][0] = 0.0f;
        invalidState.m[2][1] = 0.0f;
        invalidState.m[2][2] = 1.0f;
        invalidState.m[2][3] = 0.0f;
        
        invalidState.m[3][0] = 0.0f;
        invalidState.m[3][1] = 0.0f;
        invalidState.m[3][2] = 0.0f;
        invalidState.m[3][3] = 1.0f;
    }

    AINLINE ~TransformationStateManager( void )
    {
        return;
    }

    static D3DMATRIX** GetTransformationCache( void )
    {
        // todo: maybe reverse free list management.
        return (D3DMATRIX**)0x00C97C70;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        GetRenderDevice_Native()->GetTransform( address.transType, &value );
    }

    AINLINE bool SetDeviceState( const stateAddress& address, const valueType& value )
    {
        return GetRenderDevice_Native()->SetTransform( address.transType, &value ) >= 0;
    }
    
    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        // Reset to the identity matrix.
        GetRenderDevice_Native()->SetTransform( address.transType, &invalidState );
    }

    AINLINE bool CompareDeviceStates( const valueType& left, const valueType& right ) const
    {
        return RwD3D9MatrixEqual( left, right );
    }

    template <typename referenceDeviceType>
    AINLINE bool IsDeviceStateRequired( const stateAddress& address, referenceDeviceType& refDevice )
    {
        if ( address.transType == D3DTS_WORLD )
            return false;

        return true;
    }

    AINLINE const valueType& GetInvalidDeviceState( void )
    {
        return invalidState;
    }
};

typedef RwStateManager <TransformationStateManager> RwTransformationStateManager;

extern RwTransformationStateManager g_transformationStateManager;

#endif //_RENDERWARE_TRANSFORMATION_STATE_MANAGER_
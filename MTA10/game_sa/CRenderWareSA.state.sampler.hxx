/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.sampler.hxx
*  PURPOSE:     RenderWare Direct3D 9 sampler state manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_SAMPLER_STATE_MANAGER_
#define _RENDERWARE_SAMPLER_STATE_MANAGER_

// Include the generic logic.
#include "CRenderWareSA.stateman.hxx"

struct SamplerStateManager
{
    typedef deviceStateValue valueType;

    struct stateAddress
    {
        DWORD samplerId;
        D3DSAMPLERSTATETYPE stateType;

        AINLINE stateAddress( void )
        {
            samplerId = 0;
            stateType = D3DSAMP_ADDRESSU;
        }

        AINLINE void Increment( void )
        {
            ((DWORD&)stateType)++;

            if ( !IsEnd() && stateType == 14 )
            {
                samplerId++;

                stateType = D3DSAMP_ADDRESSU;
            }
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( samplerId == 7 && stateType == 14 );
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return ( (unsigned int)stateType - 1 ) + samplerId * 13;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->samplerId == right.samplerId && this->stateType == right.stateType );
        }
    };

    AINLINE SamplerStateManager( void ) : invalidState( -1 )
    {
        return;
    }

    AINLINE ~SamplerStateManager( void )
    {
        return;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        GetRenderDevice_Native()->GetSamplerState( address.samplerId, address.stateType, &value.value );
    }

    AINLINE void SetDeviceState( const stateAddress& address, const valueType& value )
    {
        GetRenderDevice_Native()->SetSamplerState( address.samplerId, address.stateType, value.value );
    }

    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        // Nothing to do for sampler states.
        return;
    }

    AINLINE bool CompareDeviceStates( const valueType& left, const valueType& right ) const
    {
        return ( left == right );
    }

    template <typename referenceDeviceType>
    AINLINE bool IsDeviceStateRequired( const stateAddress& address, referenceDeviceType& refDevice )
    {
        return true;
    }

    const valueType invalidState;

    AINLINE const valueType& GetInvalidDeviceState( void )
    {
        return invalidState;
    }
};

typedef RwStateManager <SamplerStateManager> RwSamplerStateManager;

extern RwSamplerStateManager g_samplerStateManager;

#endif //_RENDERWARE_SAMPLER_STATE_MANAGER_
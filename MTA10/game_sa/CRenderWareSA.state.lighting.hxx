/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.lighting.hxx
*  PURPOSE:     RenderWare Direct3D 9 lighting state manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_LIGHTING_STATE_MANAGER_
#define _RENDERWARE_LIGHTING_STATE_MANAGER_

// Include generic logic.
#include "CRenderWareSA.stateman.hxx"

struct LightingStateManager
{
    struct valueType
    {
        int enable;
        D3DLIGHT9 lightStruct;
    };

    struct stateAddress
    {
        int lightIndex;

        AINLINE stateAddress( void )
        {
            lightIndex = 0;
        }

        AINLINE void Increment( void )
        {
            if ( !IsEnd() )
                lightIndex++;
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( lightIndex == 8 );
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return (unsigned int)lightIndex;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->lightIndex == right.lightIndex );
        }
    };

    valueType invalidState;

    AINLINE LightingStateManager( void )
    {
        // Set up invalid lighting state.
        invalidState.enable = false;
        invalidState.lightStruct.Type = (D3DLIGHTTYPE)0;
    }

    AINLINE ~LightingStateManager( void )
    {
        return;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        value.enable = RpD3D9GetLightEnable( address.lightIndex );
        
        RpD3D9GetLight( address.lightIndex, value.lightStruct );
    }

    AINLINE void SetDeviceState( const stateAddress& address, const valueType& value )
    {
        RpD3D9SetLight( address.lightIndex, value.lightStruct );
        RpD3D9EnableLight( address.lightIndex, value.enable );
    }

    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        RpD3D9EnableLight( address.lightIndex, false );
    }

    AINLINE bool CompareDeviceStates( const valueType& left, const valueType& right ) const
    {
        if ( left.enable != right.enable )
            return false;

        if ( left.enable && !RpD3D9LightsEqual( left.lightStruct, right.lightStruct ) )
            return false;

        return true;
    }

    template <typename referenceDeviceType>
    AINLINE bool IsDeviceStateRequired( const stateAddress& address, referenceDeviceType& refDevice )
    {
        return ( RpD3D9GetLightEnable( address.lightIndex ) == 1 );
    }

    AINLINE const valueType& GetInvalidDeviceState( void )
    {
        return invalidState;
    }
};

typedef RwStateManager <LightingStateManager> RwLightingStateManager;

extern RwLightingStateManager g_lightingStateManager;

#endif //_RENDERWARE_LIGHTING_STATE_MANAGER_
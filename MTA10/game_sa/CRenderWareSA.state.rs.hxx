/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.rs.hxx
*  PURPOSE:     RenderWare Direct3D 9 render state manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDER_STATE_MANAGER_
#define _RENDERWARE_RENDER_STATE_MANAGER_

// Include generic code.
#include "CRenderWareSA.stateman.hxx"

struct RenderStateManager
{
    typedef deviceStateValue valueType;

    struct stateAddress
    {
        D3DRENDERSTATETYPE type;

        AINLINE stateAddress( void )
        {
            type = D3DRS_ZENABLE;
        }

        AINLINE void Increment( void )
        {
            if ( !IsEnd() )
            {
                ((DWORD&)type)++;
            }
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( type == 210 );
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return (unsigned int)type;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->type == right.type );
        }
    };

    AINLINE RenderStateManager( void ) : invalidState( -1 )
    {
        return;
    }

    AINLINE ~RenderStateManager( void )
    {
        return;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        GetRenderDevice_Native()->GetRenderState( address.type, &value.value );
    }

    AINLINE void SetDeviceState( const stateAddress& address, const valueType& value )
    {
        GetRenderDevice_Native()->SetRenderState( address.type, value.value );
    }

    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        // Nothing to do for render states.
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

typedef RwStateManager <RenderStateManager> RwRenderStateManager;

extern RwRenderStateManager g_renderStateManager;

#endif //_RENDERWARE_RENDER_STATE_MANAGER_
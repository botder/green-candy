/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.stream.hxx
*  PURPOSE:     RenderWare Direct3D 9 vertex stream state manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_VERTEX_STREAM_MANAGER_
#define _RENDERWARE_VERTEX_STREAM_MANAGER_

#include "CRenderWareSA.stateman.hxx"

struct VertexStreamStateManager
{
    typedef RwD3D9StreamInfo valueType;

    struct stateAddress
    {
        unsigned int streamNumber;

        AINLINE stateAddress( void )
        {
            streamNumber = 0;
        }

        AINLINE void Increment( void )
        {
            streamNumber++;
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( streamNumber == 8 );
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return streamNumber;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->streamNumber == right.streamNumber );
        }
    };

    valueType invalidState;

    AINLINE VertexStreamStateManager( void )
    {
        // Set up invalid stream info.
        invalidState.m_vertexBuf = (IDirect3DVertexBuffer9*)0xFFFFFFFF;
        invalidState.m_offset = 0;
        invalidState.m_stride = 0;
    }

    AINLINE ~VertexStreamStateManager( void )
    {
        return;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        GetRenderDevice_Native()->GetStreamSource( address.streamNumber, &value.m_vertexBuf, &value.m_offset, &value.m_stride );
    }

    AINLINE void SetDeviceState( const stateAddress& address, const valueType& value )
    {
        GetRenderDevice_Native()->SetStreamSource( address.streamNumber, value.m_vertexBuf, value.m_offset, value.m_stride );
    }

    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        GetRenderDevice_Native()->SetStreamSource( address.streamNumber, NULL, 0, 0 );
    }

    AINLINE bool CompareDeviceStates( const valueType& left, const valueType& right ) const
    {
        return
            ( left.m_vertexBuf == right.m_vertexBuf &&
              left.m_offset == right.m_offset &&
              left.m_stride == right.m_stride );
    }

    template <typename referenceDeviceType>
    AINLINE bool IsDeviceStateRequired( const stateAddress& address, referenceDeviceType& refDevice )
    {
        return true;
    }

    AINLINE const valueType& GetInvalidDeviceState( void )
    {
        return invalidState;
    }
};

typedef RwStateManager <VertexStreamStateManager> RwVertexStreamStateManager;

extern RwVertexStreamStateManager g_vertexStreamStateManager;

#endif //_RENDERWARE_VERTEX_STREAM_MANAGER_
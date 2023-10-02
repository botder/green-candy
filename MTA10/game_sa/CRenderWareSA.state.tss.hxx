/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.state.tss.hxx
*  PURPOSE:     RenderWare Direct3D 9 TSS manager
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_D3D_TSS_MANAGER_
#define _RENDERWARE_D3D_TSS_MANAGER_

// Include generic code.
#include "CRenderWareSA.stateman.hxx"

struct TSSStateManager
{
    typedef deviceStateValue valueType;

    struct stateAddress
    {
        DWORD stageIndex;
        D3DTEXTURESTAGESTATETYPE stageType;

        AINLINE stateAddress( void )
        {
            stageIndex = 0;
            stageType = D3DTSS_COLOROP;
        }

        AINLINE void Increment( void )
        {
            assume( stageIndex < 8 );
            assume( stageType < 33 );
            assume( stageType > 0 );

            ((unsigned int&)stageType)++;

            if ( !IsEnd() && stageType == 33 )
            {
                stageIndex++;

                stageType = D3DTSS_COLOROP;
            }
        }

        AINLINE bool IsEnd( void ) const
        {
            return ( stageIndex == 7 && stageType == 33 );
        }

        AINLINE unsigned int GetArrayIndex( void ) const
        {
            return ( (unsigned int)stageType - 1 ) + stageIndex * 32;
        }

        AINLINE bool operator == ( const stateAddress& right ) const
        {
            return ( this->stageIndex == right.stageIndex && this->stageType == right.stageType );
        }
    };

    AINLINE TSSStateManager( void ) : invalidState( -1 )
    {
        return;
    }

    AINLINE ~TSSStateManager( void )
    {
        return;
    }

    AINLINE void GetDeviceState( const stateAddress& address, valueType& value )
    {
        GetRenderDevice_Native()->GetTextureStageState( address.stageIndex, address.stageType, &value.value );
    }

    AINLINE void SetDeviceState( const stateAddress& address, const valueType& value )
    {
        GetRenderDevice_Native()->SetTextureStageState( address.stageIndex, address.stageType, value );
    }

    AINLINE void ResetDeviceState( const stateAddress& address )
    {
        // Nothing to do for TSS.
        return;
    }

    AINLINE bool CompareDeviceStates( const valueType& left, const valueType& right ) const
    {
        return ( left == right );
    }

    AINLINE bool IsGeneralPurposeModifier( D3DTEXTURESTAGESTATETYPE type )
    {
        return
           ( type == D3DTSS_BUMPENVMAT00 || type == D3DTSS_BUMPENVMAT01 ||
             type == D3DTSS_BUMPENVMAT10 || type == D3DTSS_BUMPENVMAT11 ||
             type == D3DTSS_TEXCOORDINDEX || type == D3DTSS_BUMPENVLSCALE ||
             type == D3DTSS_BUMPENVLOFFSET || type == D3DTSS_TEXTURETRANSFORMFLAGS ||
             type == D3DTSS_CONSTANT );
    }

    template <typename referenceDeviceType>
    AINLINE bool IsDeviceStateRequired( const stateAddress& address, referenceDeviceType& refDevice )
    {
        bool wasHandled = false;
        bool requiredByColor = false;

        if ( address.stageType == D3DTSS_COLORARG1 || address.stageType == D3DTSS_COLORARG2 ||
             address.stageType == D3DTSS_COLORARG0 || IsGeneralPurposeModifier( address.stageType ) )
        {
            deviceStateValue colorOp;

            stateAddress coloropAddress;
            coloropAddress.stageIndex = address.stageIndex;
            coloropAddress.stageType = D3DTSS_COLOROP;

            refDevice.GetDeviceState( coloropAddress, colorOp );

            requiredByColor = ( colorOp != D3DTOP_DISABLE );

            wasHandled = true;
        }

        bool requiredByAlpha = false;

        if ( address.stageType == D3DTSS_ALPHAARG1 || address.stageType == D3DTSS_ALPHAARG2 ||
             address.stageType == D3DTSS_ALPHAARG0 || IsGeneralPurposeModifier( address.stageType ) )
        {
            deviceStateValue alphaOp;

            stateAddress alphaopAddress;
            alphaopAddress.stageIndex = address.stageIndex;
            alphaopAddress.stageType = D3DTSS_ALPHAOP;

            refDevice.GetDeviceState( alphaopAddress, alphaOp );

            requiredByAlpha = ( alphaOp != D3DTOP_DISABLE );

            wasHandled = true;
        }

        return ( !wasHandled ) || ( requiredByColor || requiredByAlpha );
    }

    const valueType invalidState;

    AINLINE const valueType& GetInvalidDeviceState( void )
    {
        return invalidState;
    }
};

typedef RwStateManager <TSSStateManager> RwTextureStageStateManager;

extern RwTextureStageStateManager g_textureStageStateManager;

#endif //_RENDERWARE_D3D_TSS_MANAGER_
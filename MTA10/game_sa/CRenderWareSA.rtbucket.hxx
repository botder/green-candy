/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtbucket.hxx
*  PURPOSE:     Internal logic for Bucket rendering
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_BUCKET_RENDER_INTERNAL_
#define _RENDERWARE_BUCKET_RENDER_INTERNAL_

// We require knowledge about device states.
#include "CRenderWareSA.state.rs.hxx"
#include "CRenderWareSA.state.tss.hxx"
#include "CRenderWareSA.state.lighting.hxx"
#include "CRenderWareSA.state.sampler.hxx"
#include "CRenderWareSA.state.transf.hxx"
#include "CRenderWareSA.state.stream.hxx"

// Internal definitions.
namespace RenderBucket
{
    struct renderSystemState
    {
        typedef RwVertexStreamStateManager::capturedState vsState;
        typedef RwRenderStateManager::capturedState rsState;
        typedef RwTextureStageStateManager::capturedState tssState;
        typedef RwSamplerStateManager::capturedState samplState;
        typedef RwLightingStateManager::capturedState lighState;
        typedef RwTransformationStateManager::capturedState transfState;

        // Variables here describe an invariant render state.
        vsState*                        vertexStreamState;
        rsState*                        renderState;
        tssState*                       textureStageState;
        samplState*                     samplerState;
        lighState*                      lightingState;
        transfState*                    transformationState;

        renderSystemState( void )
        {
            vertexStreamState = NULL;
            renderState = NULL;
            textureStageState = NULL;
            samplerState = NULL;
            lightingState = NULL;
            transformationState = NULL;

            _lastConflict = CONFLICT_NONE;
        }

        ~renderSystemState( void )
        {
            Terminate();
        }

        void Terminate( void )
        {
            if ( vertexStreamState )
            {
                g_vertexStreamStateManager.FreeState( vertexStreamState );

                vertexStreamState = NULL;
            }

            if ( renderState )
            {
                g_renderStateManager.FreeState( renderState );

                renderState = NULL;
            }

            if ( textureStageState )
            {
                g_textureStageStateManager.FreeState( textureStageState );

                textureStageState = NULL;
            }

            if ( samplerState )
            {
                g_samplerStateManager.FreeState( samplerState );

                samplerState = NULL;
            }

            if ( lightingState )
            {
                g_lightingStateManager.FreeState( lightingState );

                lightingState = NULL;
            }

            if ( transformationState )
            {
                g_transformationStateManager.FreeState( transformationState );

                transformationState = NULL;
            }
        }

        void Capture( void )
        {
            // Capture all current states.
            if ( vertexStreamState )
            {
                vertexStreamState->Capture();
            }
            else
            {
                vertexStreamState = g_vertexStreamStateManager.CaptureState();
            }

            if ( renderState )
            {
                renderState->Capture();
            }
            else
            {
                renderState = g_renderStateManager.CaptureState();
            }

            if ( textureStageState )
            {
                textureStageState->Capture();
            }
            else
            {
                textureStageState = g_textureStageStateManager.CaptureState();
            }

            if ( lightingState )
            {
                lightingState->Capture();
            }
            else
            {
                lightingState = g_lightingStateManager.CaptureState();
            }

            if ( transformationState )
            {
                transformationState->Capture();
            }
            else
            {
                transformationState = g_transformationStateManager.CaptureState();
            }

            if ( samplerState )
            {
                samplerState->Capture();
            }
            else
            {
                samplerState = g_samplerStateManager.CaptureState();
            }
        }

        enum eRenderStateConflict
        {
            CONFLICT_NONE,
            CONFLICT_VERTEXSTREAM,
            CONFLICT_RENDERSTATE,
            CONFLICT_TEXTURESTAGESTATE,
            CONFLICT_LIGHTINGSTATE,
            CONFLICT_TRANSFORMATIONSTATE,
            CONFLICT_SAMPLERSTATE
        };

        eRenderStateConflict _lastConflict;

        bool IsCurrent( void )
        {
            if ( !vertexStreamState->IsCurrent() )
            {
                _lastConflict = CONFLICT_VERTEXSTREAM;
                return false;
            }

            if ( !renderState->IsCurrent() )
            {
                _lastConflict = CONFLICT_RENDERSTATE;
                return false;
            }

            if ( !textureStageState->IsCurrent() )
            {
                _lastConflict = CONFLICT_TEXTURESTAGESTATE;
                return false;
            }

            if ( !lightingState->IsCurrent() )
            {
                _lastConflict = CONFLICT_LIGHTINGSTATE;
                return false;
            }

            if ( !transformationState->IsCurrent() )
            {
                _lastConflict = CONFLICT_TRANSFORMATIONSTATE;
                return false;
            }

            if ( !samplerState->IsCurrent() )
            {
                _lastConflict = CONFLICT_SAMPLERSTATE;
                return false;
            }

            _lastConflict = CONFLICT_NONE;
            return true;
        }

        template <typename stateType>
        AINLINE bool CompareStates( const stateType *left, const stateType *right ) const
        {
            return 
                ( left != NULL ) == ( right != NULL ) &&
                ( !left || left &&
                  left->CompareWith( right ) );
        }

        bool CompareWith( const renderSystemState& compareWith ) const
        {
            // Check the vertex streams.
            if ( !CompareStates( this->vertexStreamState, compareWith.vertexStreamState ) )     return false;
            if ( !CompareStates( this->renderState, compareWith.renderState ) )                 return false;
            if ( !CompareStates( this->textureStageState, compareWith.textureStageState ) )     return false;
            if ( !CompareStates( this->lightingState, compareWith.lightingState ) )             return false;
            if ( !CompareStates( this->transformationState, compareWith.transformationState ) ) return false;
            if ( !CompareStates( this->samplerState, compareWith.samplerState ) )               return false;

            // We are same because there are no conflicts!
            return true;
        }

        eRenderStateConflict GetLastConflict( void )
        {
            return _lastConflict;
        }

        void AcquireContext( void )
        {
            vertexStreamState->SetDeviceTo();
            renderState->SetDeviceTo();
            textureStageState->SetDeviceTo();
            lightingState->SetDeviceTo();
            transformationState->SetDeviceTo();
            samplerState->SetDeviceTo();
        }
    };

    struct renderDataCachedMesh
    {
        // Variables here describe states that are not as expensive.
        D3DMATERIAL9                    materialState;
        D3DMATRIX                       worldMatrix;

        void Capture( void )
        {
            RwD3D9GetMaterial( materialState );

            RwD3D9GetTransform( D3DTS_WORLD, &worldMatrix );
        }

        void Apply( void )
        {
            RwD3D9SetMaterial( materialState );

            RwD3D9SetTransform( D3DTS_WORLD, &worldMatrix );
        }

        // Members deciding the rendering call.
        RwD3D9RenderCallbackData        renderCall;
    };

    // Buckets are snapshots of the runtime that are linked to multiple
    // rendering calls. Effectively, they are used to batch multiple
    // render calls together.
    struct RwRenderBucket
    {
        renderSystemState renderState;

        struct bucketRenderEntry
        {
            renderDataCachedMesh renderData;
            unsigned int globListIndex;
        };

        typedef iterativeGrowableArray <bucketRenderEntry, 12, 0, unsigned int> renderItems_t;

        renderItems_t renderItems;

        // Management methods.
        volatile LONG refCount;     // reference count to prevent destruction at critical areas.
        RwListEntry <RwRenderBucket> activeListNode;

        unsigned int usageCount;    // how many atomics are using this bucket.

        AINLINE RwRenderBucket( void )
        {
            refCount = 0;
            usageCount = 0;

            _isOnList = false;
        }

        void Reference( void );
        void Dereference( void );

        bool _isOnList;

        void SetOnList( bool bSwitch )
        {
            _isOnList = bSwitch;
        }

        bool IsOnList( void )
        {
            return _isOnList;
        }
    };
   
    typedef iterativeGrowableArray <RwRenderBucket*, 16, 0, unsigned int> renderBuckets_t;

    void GetActiveRenderBuckets( renderBuckets_t& list );
};

#endif //_RENDERWARE_BUCKET_RENDER_INTERNAL_
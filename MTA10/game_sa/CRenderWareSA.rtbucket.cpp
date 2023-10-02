/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtbucket.cpp
*  PURPOSE:     Bucket rendering optimizations using rendering metadata
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

//todo: will work on this system when I have lots of free time.
// It will improve the FPS a huge amount.

#include "StdInc.h"
#include "RenderWare/RwRenderTools.hxx"

// Include internal definitions.
#include "CRenderWareSA.rtbucket.hxx"

// Keep statistics about our efficiency.
struct runtimeStatistics
{
    AINLINE runtimeStatistics( void )
    {
        Reset();
    }

    AINLINE void Reset( void )
    {
        maxEntriesPerBucket = 0;
        totalRenderedEntries = 0;
        totalNumberOfBuckets = 0;
        bucketCacheMissCount = 0;
        bucketAllocationCount = 0;
        bucketAdaptionCount = 0;

        cacheNumVertexStreamConflicts = 0;
        cacheNumRenderStateConflicts = 0;
        cacheNumTextureStageStateConflicts = 0;
        cacheNumLightingStateConflicts = 0;
        cacheNumTransformationStateConflicts = 0;
        cacheNumSamplerStateConflicts = 0;

        uniqueBucketUsageCount = 0;
        bucketTerminationCount = 0;
    }

    unsigned int maxEntriesPerBucket;
    unsigned int totalRenderedEntries;
    unsigned int totalNumberOfBuckets;
    unsigned int bucketCacheMissCount;
    unsigned int bucketAllocationCount;
    unsigned int bucketAdaptionCount;

    // Conflict counts.
    unsigned int cacheNumVertexStreamConflicts;
    unsigned int cacheNumRenderStateConflicts;
    unsigned int cacheNumTextureStageStateConflicts;
    unsigned int cacheNumLightingStateConflicts;
    unsigned int cacheNumTransformationStateConflicts;
    unsigned int cacheNumSamplerStateConflicts;

    unsigned int uniqueBucketUsageCount;
    unsigned int bucketTerminationCount;
};

// Per-pass statistics.
static runtimeStatistics _currentPassStats;
static runtimeStatistics _lastPassStats;

// Persistent statistics.
static unsigned int totalAllocatedBuckets;

namespace RenderBucket
{
    typedef RwRenderBucket Pipelineable;

    // RenderBucket variables.
    static bool isInPhase = false;

    // Pointer to the currently rendering atomic object.
    static RpAtomic *currentlyRenderingAtomic = NULL;

    struct pipelineArrayManager
    {
        AINLINE void InitField( Pipelineable*& data )
        {
            return;
        }
    };
    typedef growableArray <Pipelineable*, 4, 0, pipelineArrayManager, unsigned int> pipelineArray_t;

    // Array of all data on the virtual pipeline.
    static pipelineArray_t pipelineData;

    static Pipelineable *currentPipeData = NULL;

    // Allocator so we do not polute the memory environment.
    typedef CachedConstructedClassAllocator <RwRenderBucket> bucketAlloc_t;

    static bucketAlloc_t bucketAlloc( 512 );

    // Lock used for bucket allocation and termination.
    static CRITICAL_SECTION bucketAllocLock;

    // Utility functions.
    RwRenderBucket* AllocateRenderBucket( void )
    {
        EnterCriticalSection( &bucketAllocLock );

        RwRenderBucket *renderBucket = bucketAlloc.Allocate();

        LeaveCriticalSection( &bucketAllocLock );

        // Initialize the members.
        renderBucket->renderState.Capture();
        renderBucket->renderItems.Clear();

        return renderBucket;
    }

    void FreeRenderBucket( RwRenderBucket *bucket )
    {
        if ( currentPipeData == bucket )
        {
            currentPipeData = NULL;
        }

        bucket->renderState.Terminate();

        EnterCriticalSection( &bucketAllocLock );

        bucketAlloc.Free( bucket );

        LeaveCriticalSection( &bucketAllocLock );

        _currentPassStats.bucketTerminationCount++;
    }

    // Array of all active render buckets.
    static RwList <RwRenderBucket> activeRenderBuckets;

    // Lock used when iterating through the list
    static CRITICAL_SECTION activeRenderBuckets_lock;

    void GetActiveRenderBuckets( renderBuckets_t& list )
    {
        EnterCriticalSection( &activeRenderBuckets_lock );

        // Add them to the list.
        LIST_FOREACH_BEGIN( RwRenderBucket, activeRenderBuckets.root, activeListNode )
            item->Reference();

            list.AddItem( item );
        LIST_FOREACH_END

        LeaveCriticalSection( &activeRenderBuckets_lock );
    }

    // Reference counting algorithms of render buckets.
    void RwRenderBucket::Reference( void )
    {
        LONG prevRefValue = InterlockedExchangeAdd( &this->refCount, (LONG)1 );

        if ( prevRefValue == 0 )
        {
            EnterCriticalSection( &activeRenderBuckets_lock );

            // Insert our render bucket into the active list.
            LIST_INSERT( activeRenderBuckets.root, activeListNode );

            LeaveCriticalSection( &activeRenderBuckets_lock );
        }
    }

    void RwRenderBucket::Dereference( void )
    {
        LONG prevRefCount = InterlockedExchangeAdd( &this->refCount, (LONG)-1 );

        if ( prevRefCount == 1 )
        {
            EnterCriticalSection( &activeRenderBuckets_lock );

            // Remove our bucket from the active list.
            LIST_REMOVE( activeListNode );

            LeaveCriticalSection( &activeRenderBuckets_lock );

            // Deallocate the bucket.
            FreeRenderBucket( this );
        }
    }

    RwRenderBucket* FindRenderBucket( void )
    {
        RwRenderBucket *currentBucket = NULL;

        // Check the most notable render bucket first.
        // This one is very likely to succeed.
        if ( currentBucket == NULL )
        {
            if ( RpAtomic *renderingAtomic = currentlyRenderingAtomic )
            {
                RwRenderBucket *lastBestBucket = RpAtomicGetContextualRenderBucket( renderingAtomic );

                if ( lastBestBucket )
                {
                    if ( lastBestBucket->renderState.IsCurrent() )
                    {
                        currentBucket = lastBestBucket;
                    }
                    else
                    {
                        _currentPassStats.bucketCacheMissCount++;

                        // Check what conflict we encountered and log it.
                        {
                            renderSystemState::eRenderStateConflict conflictType = lastBestBucket->renderState.GetLastConflict();

                            if ( conflictType == renderSystemState::CONFLICT_VERTEXSTREAM )
                            {
                                _currentPassStats.cacheNumVertexStreamConflicts++;
                            }
                            else if ( conflictType == renderSystemState::CONFLICT_RENDERSTATE )
                            {
                                if ( _currentPassStats.cacheNumRenderStateConflicts > 2000 )
                                    __asm nop

                                _currentPassStats.cacheNumRenderStateConflicts++;
                            }
                            else if ( conflictType == renderSystemState::CONFLICT_TEXTURESTAGESTATE )
                            {
                                _currentPassStats.cacheNumTextureStageStateConflicts++;
                            }
                            else if ( conflictType == renderSystemState::CONFLICT_TRANSFORMATIONSTATE )
                            {
                                _currentPassStats.cacheNumTransformationStateConflicts++;
                            }
                            else if ( conflictType == renderSystemState::CONFLICT_SAMPLERSTATE )
                            {
                                _currentPassStats.cacheNumSamplerStateConflicts++;
                            }
                        }

                        // Try to adapt the bucket if nothing has scheduled a render with it yet.
                        // This is breaking bucket unique-ness in favour of performance.
                        if ( lastBestBucket->renderItems.GetCount() == 0 )
                        {
                            lastBestBucket->renderState.Capture();

                            currentBucket = lastBestBucket;

                            _currentPassStats.bucketAdaptionCount++;
                        }
                    }
                }
            }
        }

        if ( currentBucket == NULL )
        {
            RwRenderBucket *likelyBucket = currentPipeData;

            if ( likelyBucket && likelyBucket->renderState.IsCurrent() )
            {
                currentBucket = likelyBucket;
            }
        }

        if ( currentBucket == NULL )
        {
            unsigned int n = pipelineData.GetCount();

            while ( n != 0 )
            {
                RwRenderBucket *rtBucket = pipelineData.GetFast( --n );

                if ( rtBucket && rtBucket != currentPipeData )
                {
                    if ( rtBucket->renderState.IsCurrent() )
                    {
                        currentBucket = rtBucket;
                        break;
                    }
                }
            }
        }

#if 0
        // Slowest method.
        if ( currentBucket == NULL )
        {
            LIST_FOREACH_BEGIN( RwRenderBucket, activeRenderBuckets.root, activeListNode )
                if ( item->renderState.IsCurrent() )
                {
                    currentBucket = item;
                    break;
                }
            LIST_FOREACH_END
        }
#endif

        if ( currentBucket == NULL )
        {
            currentBucket = AllocateRenderBucket();

            if ( currentBucket != NULL )
            {
                _currentPassStats.bucketAllocationCount++;
            }
        }

        if ( currentBucket != NULL )
        {
            if ( !currentBucket->IsOnList() )
            {
                pipelineData.AddItem( currentBucket );

                currentBucket->SetOnList( true );

                // Reference our render bucket so we start using it.
                currentBucket->Reference();

                _currentPassStats.uniqueBucketUsageCount++;
            }
        }

        return currentBucket;
    }

    static void LockStateMachines( void )
    {
        // Lock all state machines.
        g_renderStateManager.LockLocalState();
        g_textureStageStateManager.LockLocalState();
        g_transformationStateManager.LockLocalState();
        g_samplerStateManager.LockLocalState();
        g_lightingStateManager.LockLocalState();
    }

    static void UnlockStateMachines( void )
    {
        // Unlock all state machines.
        g_renderStateManager.UnlockLocalState();
        g_textureStageStateManager.UnlockLocalState();
        g_transformationStateManager.UnlockLocalState();
        g_samplerStateManager.UnlockLocalState();
        g_lightingStateManager.UnlockLocalState();
    }

#if 0
    // Task runtime that handles sorting.
    struct renderQueueSortingItem
    {
        RwD3D9RenderCallbackData lastRenderCall;
        renderSystemState sysState;
    };
    typedef SynchronizedHyperQueue <renderQueueSortingItem> renderSortingQueue_t;

    static renderSortingQueue_t renderSortingQueue( 256 );

    static renderSortingQueue_t processedList( 512 );

    struct SimpleRenderTaskQueueApplicator
    {
        renderQueueSortingItem& theItem;

        AINLINE SimpleRenderTaskQueueApplicator( renderQueueSortingItem& theItem ) : theItem( theItem )
        {
            return;
        }

        AINLINE void PutItem( renderQueueSortingItem& theField )
        {
            theField.sysState = theItem.sysState;
        }
    };

    static void __stdcall RenderBucketSortingTask( NativeExecutive::CExecTask *theTask, void *memPtr )
    {
        // Do nothing for initialization.
        NativeExecutive::CFiber *theFiber = theTask->runtimeFiber;

        theFiber->yield();

        // Handle sorting requests, forever.
        while ( true )
        {
            renderQueueSortingItem theItem;

            renderSortingQueue.GetSheduledItem( theItem );

            // todo.

            // Yield once finished.
            theFiber->yield();
        }
    }

    static NativeExecutive::CExecTask *sortingTask = NULL;
#endif

    struct renderProcessItem
    {
        RwRenderBucket *rwBucket;
        unsigned int bucketItemIndex;
    };

    struct renderProcessItemArrayManager
    {
        AINLINE void InitField( renderProcessItem& theItem )
        {
            return;
        }
    };
    typedef growableArray <renderProcessItem, 12, 0, renderProcessItemArrayManager, unsigned int> renderProcessItemArray_t;

    static renderProcessItemArray_t renderProcessItemArray;
}

void RenderBucket::Initialize( void )
{
    // Set up runtime data.
    isInPhase = false;
    currentPipeData = NULL;

    LIST_CLEAR( activeRenderBuckets.root );

    InitializeCriticalSection( &activeRenderBuckets_lock );
    InitializeCriticalSection( &bucketAllocLock );

#if 0
    // Create the sorting task for multi-threaded execution.
    sortingTask = pGame->GetExecutiveManager()->CreateTask( RenderBucketSortingTask, NULL );

    assert( sortingTask != NULL );
#endif
}

void RenderBucket::Shutdown( void )
{
#if 0
    if ( sortingTask )
    {
        // Close our task.
        pGame->GetExecutiveManager()->CloseTask( sortingTask );

        sortingTask = NULL;
    }
#endif

    DeleteCriticalSection( &bucketAllocLock );
    DeleteCriticalSection( &activeRenderBuckets_lock );

    assert( isInPhase == false );
}

void RenderBucket::BeginPass( void )
{
    assert( isInPhase == false );

    // Notify all state managers.
    g_vertexStreamStateManager.BeginBucketPass();
    g_renderStateManager.BeginBucketPass();
    g_textureStageStateManager.BeginBucketPass();
    g_samplerStateManager.BeginBucketPass();
    g_lightingStateManager.BeginBucketPass();
    g_transformationStateManager.BeginBucketPass();

    // Reset statistics.
    _currentPassStats.Reset();

    // For all active render buckets, initialize them.
    LIST_FOREACH_BEGIN( RwRenderBucket, activeRenderBuckets.root, activeListNode )
        item->renderItems.Clear();
    LIST_FOREACH_END

    // Enter the phase.
    isInPhase = true;
}

namespace RenderBucket
{
    const static D3DVERTEXELEMENT9 bucketRaster_instanceData[] = 
    {
        D3DDECL_END()
    };

    struct BucketedRasterizer
    {
        AINLINE BucketedRasterizer( void )
        {
            currentBucket = NULL;
        }

        AINLINE void setContext( RwRenderBucket *theBucket )
        {
            if ( currentBucket != theBucket )
            {
                theBucket->renderState.AcquireContext();

                currentBucket = theBucket;
            }
        }

        

        RwRenderBucket *currentBucket;
    };
}

static const bool allowBucketRendering = false;

AINLINE void ResetToBucketState( void )
{
    // Reset all state managers to bucket states.
    g_vertexStreamStateManager.SetBucketStates();
    g_renderStateManager.SetBucketStates();
    g_textureStageStateManager.SetBucketStates();
    g_samplerStateManager.SetBucketStates();
    g_lightingStateManager.SetBucketStates();
    g_transformationStateManager.SetBucketStates();
}

void RenderBucket::RenderPass( void )
{
    assert( isInPhase == true );

    if ( !allowBucketRendering )
        return;

    // Make sure we are initially at the bucket states.
    ResetToBucketState();

    // Render all items.
    BucketedRasterizer rasterizer;

    for ( unsigned int n = 0; n < renderProcessItemArray.GetCount(); n++ )
    {
        const renderProcessItem& renderItem = renderProcessItemArray.Get( n );

        // Unpack bucket and corresponding item.
        RwRenderBucket *theBucket = renderItem.rwBucket;
        RwRenderBucket::bucketRenderEntry& meshData = theBucket->renderItems.Get( renderItem.bucketItemIndex );

        rasterizer.setContext( theBucket );

    }

    renderProcessItemArray.Clear();
}

void RenderBucket::EndPass( void )
{
    assert( isInPhase == true );

    // Process the pass.
    RenderPass();

    // Free all rendering data.
    for ( unsigned int n = 0; n < pipelineData.GetCount(); n++ )
    {
        RwRenderBucket *bucket = pipelineData.GetFast( n );

        bucket->Dereference();
        bucket->SetOnList( false );
    }

    // Clear runtime variables.
    pipelineData.Clear();

    currentPipeData = NULL;

    // Notify the managers.
    g_vertexStreamStateManager.EndBucketPass();
    g_renderStateManager.EndBucketPass();
    g_textureStageStateManager.EndBucketPass();
    g_samplerStateManager.EndBucketPass();
    g_lightingStateManager.EndBucketPass();
    g_transformationStateManager.EndBucketPass();

    // Put finished statistics into the global storage.
    _lastPassStats = _currentPassStats;

    // Leave the phase.
    isInPhase = false;
}

void RenderBucket::SetContextAtomic( RpAtomic *renderObject )
{
    // Only allow context atomics during bucket phase.
    if ( !isInPhase )
        return;

    if ( renderObject == currentlyRenderingAtomic )
        return;

    currentlyRenderingAtomic = renderObject;
}

renderBucketStats RenderBucket::GetRuntimeStatistics( void )
{
    renderBucketStats statsOut;
    statsOut.totalNumberOfRenderCalls = _lastPassStats.totalRenderedEntries;
    statsOut.maxRenderCallsPerBucket = _lastPassStats.maxEntriesPerBucket;

    unsigned int totalBucketCount = 0;

    LIST_FOREACH_BEGIN( RwRenderBucket, activeRenderBuckets.root, activeListNode )
        totalBucketCount++;
    LIST_FOREACH_END

    statsOut.totalNumberOfActiveBuckets = totalBucketCount;
    statsOut.bucketCacheMissCount = _lastPassStats.bucketCacheMissCount;
    statsOut.bucketAllocationCount = _lastPassStats.bucketAllocationCount;
    statsOut.bucketAdaptionCount = _lastPassStats.bucketAdaptionCount;

    statsOut.cacheNumVertexStreamConflicts = _lastPassStats.cacheNumVertexStreamConflicts;
    statsOut.cacheNumRenderStateConflicts = _lastPassStats.cacheNumRenderStateConflicts;
    statsOut.cacheNumTextureStageStateConflicts = _lastPassStats.cacheNumTextureStageStateConflicts;
    statsOut.cacheNumLightingStateConflicts = _lastPassStats.cacheNumLightingStateConflicts;
    statsOut.cacheNumTransformationStateConflicts = _lastPassStats.cacheNumTransformationStateConflicts;
    statsOut.cacheNumSamplerStateConflicts = _lastPassStats.cacheNumSamplerStateConflicts;

    statsOut.uniqueBucketUsageCount = _lastPassStats.uniqueBucketUsageCount;
    statsOut.bucketTerminationCount = _lastPassStats.bucketTerminationCount;

    return statsOut;
}

// Since this is the lowest rendering layer, it is very important to keep this code as optimized as possible.
// Rules of thumb:
// * avoid memory allocations as much as possible, since they scale dramatically
// * use growableArray for data that needs scaling
// * use CachedConstructedClassAllocator to create complex classes whose data does not have to be constructed on allocation
// * make the code not care about memory-saving
// * do not allow any memory leak, since its a definite crash
// * make sure performance is 100 FPS at rendering 50 vehicles

#if 0
struct RenderItemQueueInitializator
{
    const RwD3D9RenderCallbackData& callbackData;

    AINLINE RenderItemQueueInitializator( const RwD3D9RenderCallbackData& theData ) : callbackData( theData )
    {
        return;
    }

    AINLINE void PutItem( RenderBucket::renderQueueSortingItem& theField )
    {
        theField.sysState.Capture();
        theField.lastRenderCall = callbackData;
    }
};
#endif

bool RenderBucket::OnCachedRenderCall( const RwD3D9RenderCallbackData& callbackData )
{
#if 1
    return false;
#else
    // If we are not in the virtual pipeline phase, ignore.
    if ( !isInPhase )
        return false;

    RwRenderBucket *currentBucket = FindRenderBucket();

    currentPipeData = currentBucket;

    RwRenderBucket::bucketRenderEntry entry;

    renderDataCachedMesh& cachedData = entry.renderData;
    cachedData.Capture();
    cachedData.renderCall = callbackData;

    if ( allowBucketRendering )
    {
        // Store the global index for performance optimizations.
        entry.globListIndex = renderProcessItemArray.GetCount();

        // Add the request to the global render item list.
        renderProcessItem& thisItem = renderProcessItemArray.ObtainItem();
        thisItem.rwBucket = currentBucket;
        thisItem.bucketItemIndex = ( currentBucket->renderItems.GetCount() - 1 );
    }

    currentBucket->renderItems.AddItem( entry );

    // If we have a rendering atomic, inform it about the render bucket.
    if ( RpAtomic *renderingAtomic = currentlyRenderingAtomic )
    {
        RpAtomicSetContextualRenderBucket( renderingAtomic, currentBucket );
    }

    // Update statistics.
    {
        if ( _currentPassStats.maxEntriesPerBucket < currentBucket->renderItems.GetCount() )
        {
            _currentPassStats.maxEntriesPerBucket = currentBucket->renderItems.GetCount();
        }

        _currentPassStats.totalRenderedEntries++;
    }

    // TODO: actually handle rendering requests.
    return true;
#endif
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtbucket.h
*  PURPOSE:     Bucket rendering optimizations using rendering metadata
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_BUCKET_OPTIMIZATIONS_
#define _RENDERWARE_BUCKET_OPTIMIZATIONS_

/*
    A render bucket system is an optimization routine based on metadata collection
    that assumes the collected data is invariant.
*/
namespace RenderBucket
{
    // Forward declarations.
    struct RwRenderBucket;

    void                    Initialize              ( void );
    void                    Shutdown                ( void );

    void                    Begin                   ( void );
    void                    End                     ( void );

    void                    BeginPass               ( void );
    void                    RenderPass              ( void );
    void                    EndPass                 ( void );

    void                    SetContextAtomic        ( RpAtomic *renderObject );

    renderBucketStats       GetRuntimeStatistics    ( void );

    bool                    OnCachedRenderCall      ( const RwD3D9RenderCallbackData& callbackData );
};

#endif //_RENDERWARE_BUCKET_OPTIMIZATIONS_
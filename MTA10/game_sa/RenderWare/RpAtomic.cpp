/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomic.cpp
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include <StdInc.h>
#include "../gamesa_renderware.h"

/*=========================================================
    RpAtomic::GetWorldBoundingSphere

    Purpose:
        Calculates and returns a bounding sphere in world space
        which entirely contains the geometry.
    Binary offsets:
        (1.0 US): 0x00749330
        (1.0 EU): 0x00749380
=========================================================*/
const RwSphere& RpAtomic::GetWorldBoundingSphere( void )
{
    return RpAtomicGetWorldBoundingSphere( this );
}

/*=========================================================
    RpAtomic::IsNight (GTA:SA extension)

    Purpose:
        Returns whether this atomic is rendered using the night
        vertex colors extension.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005D7F40
=========================================================*/
bool RpAtomic::IsNight( void )
{
    unsigned int thePipeline = RpAtomicGetRenderPipeline( this );

    if ( thePipeline == RW_ATOMIC_RENDER_NIGHT )
        return true;

    if ( thePipeline == RW_ATOMIC_RENDER_REFLECTIVE )
        return false;

    return geometry->nightColor && geometry->colors != NULL;
}

/*=========================================================
    RpAtomic::AddToClump

    Arguments:
        clump - model to register this atomic at
    Purpose:
        Adds this atomic to a clump. The atomic is unlinked
        from any previous clump.
    Binary offsets:
        (1.0 US): 0x0074A490
        (1.0 EU): 0x0074A4E0
    Note:
        At the binary offset location actually is
        RpClumpAddAtomic.
=========================================================*/
void RpAtomic::AddToClump( RpClump *clump )
{
    RemoveFromClump();

    this->clump = clump;

    LIST_INSERT( clump->atomics.root, atomics );
}

RpClump* __cdecl RpClumpAddAtomic( RpClump *clump, RpAtomic *atomic )       { atomic->AddToClump( clump ); return clump; }
/*=========================================================
    RpAtomic::RemoveFromClump

    Purpose:
        Removes this atomic from any clump it may be registered at.
    Binary offsets:
        (1.0 US): 0x0074A4C0
        (1.0 EU): 0x0074A510
    Note:
        At the binary offset location actually is
        RpClumpRemoveAtomic.
=========================================================*/
void RpAtomic::RemoveFromClump( void )
{
    if ( !clump )
        return;

    LIST_REMOVE( atomics );

    clump = NULL;
}

RpClump* __cdecl RpClumpRemoveAtomic( RpClump *clump, RpAtomic *atomic )    { atomic->RemoveFromClump(); return clump; }
/*=========================================================
    RpAtomic::SetRenderCallback

    Arguments:
        callback - an anonymous function which is called at rendering
    Purpose:
        Sets a new rendering callback to this atomic. If NULL is
        given, then the rendering callback of this atomic is set
        to the default one.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007328A0
=========================================================*/
void RpAtomic::SetRenderCallback( RpAtomicCallback callback )
{
    if ( !callback )
    {
        renderCallback = (RpAtomicCallback)RpAtomicRender;
    }
    else
    {
        renderCallback = callback;
    }
}

/*=========================================================
    RpAtomic::FetchMateria

    Arguments:
        mats - container for material storage
    Purpose:
        Adds all materia of this atomic to a specified container.
        It fails if a special visibility flag is set.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004C8B60
=========================================================*/
void RpAtomic::FetchMateria( RpMaterials& mats )
{
    if ( RpAtomicGetComponentFlags( this ) & 0x2000 )
        return;

    for ( unsigned int n = 0; n < geometry->linkedMateria->count; n++ )
    {
        mats.Add( geometry->linkedMateria->Get(n)->material );
    }
}

/*=========================================================
    RenderWare Atomic Animation Hierarchy Store Plugin.
=========================================================*/
struct _atomicAnimHierarchyStorePlugin
{
    RpAnimHierarchy *animHier;
};

inline int GetAnimHierarchyStorePluginOffset( void )
{
    return *(int*)0x00C978A4;
}

inline _atomicAnimHierarchyStorePlugin* GetAnimHierarchyStoreAtomicInfo( RpAtomic *atomic )
{
    int pluginOffset = GetAnimHierarchyStorePluginOffset();

    if ( pluginOffset == -1 )
        return NULL;

    return ( atomic ) ? ( RW_PLUGINSTRUCT <_atomicAnimHierarchyStorePlugin> ( atomic, pluginOffset ) ) : ( NULL );
}

/*=========================================================
    RpAtomicSetAnimHierarchy (plugin function)

    Arguments:
        atomic - renderware atomic object
        anim - animation hierarchy object
    Purpose:
        Updates the anim hierarchy plugin field of the given
        atomic object,
    Binary offsets:
        (1.0 US): 0x007C7520
        (1.0 EU): 0x007C7560
=========================================================*/
void __cdecl RpAtomicSetAnimHierarchy( RpAtomic *atomic, RpAnimHierarchy *anim )
{
    _atomicAnimHierarchyStorePlugin *info = GetAnimHierarchyStoreAtomicInfo( atomic );

    if ( info )
    {
        info->animHier = anim;
    }
}

/*=========================================================
    RpAtomicGetAnimHierarchy (plugin function)

    Arguments:
        mats - container for material storage
    Purpose:
        Returns the animation hierarchy structure of the
        given atomic object.
    Binary offsets:
        (1.0 US): 0x007C7540
        (1.0 EU): 0x007C7580
=========================================================*/
RpAnimHierarchy* __cdecl RpAtomicGetAnimHierarchy( RpAtomic *atomic )
{
    RpAnimHierarchy *animHier = NULL;

    if ( _atomicAnimHierarchyStorePlugin *info = GetAnimHierarchyStoreAtomicInfo( atomic ) )
    {
        animHier = info->animHier;
    }

    return animHier;
}

/*=========================================================
    RenderWare Atomic Model Info Plugin.
=========================================================*/
struct _atomicModelInfoPlugin
{
    unsigned short modelId;
    unsigned short componentFlags;
};

inline int GetAtomicModelInfoPluginOffset( void )
{
    return *(int*)0x008D608C;
}

inline _atomicModelInfoPlugin* GetAtomicModelInfoPlugin( RpAtomic *atomic )
{
    int pluginOffset = GetAtomicModelInfoPluginOffset();

    if ( pluginOffset == -1 )
        return NULL;

    return ( atomic ) ? ( RW_PLUGINSTRUCT <_atomicModelInfoPlugin> ( atomic, pluginOffset ) ) : ( NULL );
}


/*=========================================================
    RpAtomicSetModelIndex (plugin function)

    Arguments:
        atomic - renderware atomic object
        modelIndex - index of the model info to assign to the atomic
    Purpose:
        Links an atomic to a GTA:SA model info structure.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732230
=========================================================*/
void __cdecl RpAtomicSetModelIndex( RpAtomic *atomic, unsigned short modelIndex )
{
    _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic );

    if ( info )
    {
        info->modelId = modelIndex;
    }
}

/*=========================================================
    RpAtomicGetModelIndex (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Returns the model index that is assigned to the given
        RenderWare atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732250
=========================================================*/
unsigned short __cdecl RpAtomicGetModelIndex( RpAtomic *atomic )
{
    unsigned short modelIndex = 0xFFFF;

    _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic );

    if ( info )
    {
        modelIndex = info->modelId;
    }

    return modelIndex;
}

/*=========================================================
    RpAtomicGetModelInfo (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Returns the model info that is associated with the
        given RenderWare atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732260
=========================================================*/
CBaseModelInfoSAInterface* __cdecl RpAtomicGetModelInfo( RpAtomic *atomic )
{
    CBaseModelInfoSAInterface *modelInfo = NULL;

    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        unsigned short modelIndex = info->modelId;

        if ( modelIndex != 0xFFFF )
        {
            modelInfo = ppModelInfo[ modelIndex ];
        }
    }

    return modelInfo;
}

/*=========================================================
    RpAtomicAssignComponentFlags (plugin function)

    Arguments:
        atomic - renderware atomic object
        flags - component flags to assign
    Purpose:
        Sets component flags to the given atomic. Component
        flags are used for identification purposes by the
        atomic rendering system (for vehicles).
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732290
=========================================================*/
void __cdecl RpAtomicAssignComponentFlags( RpAtomic *atomic, unsigned int flags )
{
    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        info->componentFlags = (unsigned short)flags;
    }
}

/*=========================================================
    RpAtomicAddComponentFlags (plugin function)

    Arguments:
        atomic - renderware atomic object
        flags - component flags to add
    Purpose:
        ORs the component flags of the atomic with the given
        flags.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007322B0
=========================================================*/
void __cdecl RpAtomicAddComponentFlags( RpAtomic *atomic, unsigned int flags )
{
    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        info->componentFlags |= flags;
    }
}

/*=========================================================
    RpAtomicRemoveComponentFlags (plugin function)

    Arguments:
        atomic - renderware atomic object
        flags - component flags to remove
    Purpose:
        Removes the given flags from the component flags of
        the atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732310
=========================================================*/
void __cdecl RpAtomicRemoveComponentFlags( RpAtomic *atomic, unsigned int flags )
{
    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        info->componentFlags &= ~flags;
    }
}

/*=========================================================
    RpAtomicGetComponentFlags (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Returns the component flags of the given atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732370
=========================================================*/
unsigned int __cdecl RpAtomicGetComponentFlags( RpAtomic *atomic )
{
    unsigned int flags = 0x00000000;

    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        flags = (unsigned int)info->componentFlags;
    }

    return flags;
}

/*=========================================================
    RpAtomicSetComponentFlagsUShort (plugin function)

    Arguments:
        atomic - renderware atomic object
        flags - component flags to set to the atomic.
    Purpose:
        Assigns component flags to the atomic as unsigned
        short.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00732380
=========================================================*/
void __cdecl RpAtomicSetComponentFlagsUShort( RpAtomic *atomic, unsigned short flags )
{
    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        info->componentFlags = flags;
    }
}

/*=========================================================
    RpAtomicGetComponentFlagsUShort (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Returns the atomic component flags in unsigned short
        format.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x007323A0
=========================================================*/
unsigned short __cdecl RpAtomicGetComponentFlagsUShort( RpAtomic *atomic )
{
    unsigned short flags = 0x0000;

    if ( _atomicModelInfoPlugin *info = GetAtomicModelInfoPlugin( atomic ) )
    {
        flags = info->componentFlags;
    }

    return flags;
}

/*=========================================================
    RenderWare Atomic Rendering Pipeline Store Plugin.
=========================================================*/
struct _atomicPipelineStorePlugin
{
    unsigned int pipeline;
};

inline int GetAtomicRenderPipelineStorePluginOffset( void )
{
    return *(int*)0x008D6080;
}

inline _atomicPipelineStorePlugin* GetAtomicRenderPipelineStorePlugin( RpAtomic *atomic )
{
    int pluginOffset = GetAtomicRenderPipelineStorePluginOffset();

    if ( pluginOffset == -1 )
        return NULL;

    return ( atomic ) ? ( RW_PLUGINSTRUCT <_atomicPipelineStorePlugin> ( atomic, pluginOffset ) ) : ( NULL );
}

/*=========================================================
    RpAtomicGetRenderPipeline (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Returns the render pipeline identifier that is
        associated with the given atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0072FC40
=========================================================*/
unsigned int __cdecl RpAtomicGetRenderPipeline( RpAtomic *atomic )
{
    unsigned int pipelineId = 0;

    if ( _atomicPipelineStorePlugin *info = GetAtomicRenderPipelineStorePlugin( atomic ) )
    {
        pipelineId = info->pipeline;
    }

    return pipelineId;
}

/*=========================================================
    RpAtomicSetRenderPipeline (plugin function)

    Arguments:
        atomic - renderware atomic object
    Purpose:
        Sets the render pipeline identifier to a given atomic.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0072FC50
=========================================================*/
void __cdecl RpAtomicSetRenderPipeline( RpAtomic *atomic, unsigned int pipeline )
{
    if ( _atomicPipelineStorePlugin *info = GetAtomicRenderPipelineStorePlugin( atomic ) )
    {
        info->pipeline = pipeline;
    }
}
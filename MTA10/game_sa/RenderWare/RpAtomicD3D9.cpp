/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RpAtomicD3D9.cpp
*  PURPOSE:     RenderWare Atomic Direct3D 9 plugin definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

#include "../gamesa_renderware.h"

#include <shared_mutex>

/*=========================================================
    RenderWare Atomic Direct3D Environment Reflection Map Plugin
=========================================================*/

void* CEnvMapAtomicSA::operator new ( size_t )
{
    return RenderWare::GetEnvMapAtomicPool()->Allocate();
}

void CEnvMapAtomicSA::operator delete ( void *ptr )
{
    RenderWare::GetEnvMapAtomicPool()->Free( (CEnvMapAtomicSA*)ptr );
}

/*=========================================================
    RpAtomicGetEnvironmentReflectionMap

    Purpose:
        Returns the environment reflection map associated
        with the given atomic. It is allocated if it does
        not already exist.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005D96F0
=========================================================*/
struct _atomicReflectionMapStorePlugin
{
    CEnvMapAtomicSA *envMap;
};

inline int GetAtomicReflectionMapStorePluginOffset( void )
{
    return *(int*)0x008D12C8;
}

inline _atomicReflectionMapStorePlugin* GetAtomicReflectionMapStorePlugin( RpAtomic *atomic )
{
    int pluginOffset = GetAtomicReflectionMapStorePluginOffset();

    if ( pluginOffset == -1 )
        return NULL;

    return ( atomic ) ? ( RW_PLUGINSTRUCT <_atomicReflectionMapStorePlugin> ( atomic, pluginOffset ) ) : ( NULL );
}

CEnvMapAtomicSA* __cdecl RpAtomicGetEnvironmentReflectionMap( RpAtomic *atomic )
{
    CEnvMapAtomicSA *envMap = NULL;

    if ( _atomicReflectionMapStorePlugin *info = GetAtomicReflectionMapStorePlugin( atomic ) )
    {
        envMap = info->envMap;

        if ( !envMap )
        {
            envMap = new CEnvMapAtomicSA( 0, 0, 0 );

            info->envMap = envMap;
        }
    }

    return envMap;
}

/*=========================================================
    RenderWare Atomic Direct3D Bucket Sorting Extension
=========================================================*/
// Include internal bucket sorting definitions.
#include "../CRenderWareSA.rtbucket.hxx"

void RpAtomicD3D9_RegisterPlugins( void )
{
    return;
}

/*=========================================================
    RpAtomicContextualRenderSetPassIndex (MTA extension/plugin)

    Arguments:
        passIndex - the pass index to set for the current
            rendering context
    Purpose:
        Sets the current pass index for the current
        rendering context.
=========================================================*/
static unsigned int contextualPassIndex = 0;
static unsigned int contextualStageIndex = 0;

void __cdecl RpAtomicContextualRenderSetPassIndex( unsigned int passIndex )
{
    contextualPassIndex = passIndex;
}

/*=========================================================
    RpAtomicContextualRenderSetStageIndex (MTA extension/plugin)

    Arguments:
        stageIndex - the stage index to set for the current
            rendering context
    Purpose:
        Sets the current stage index for the current
        rendering context.
=========================================================*/
void __cdecl RpAtomicContextualRenderSetStageIndex( unsigned int stageIndex )
{
    contextualStageIndex = stageIndex;
}

/*=========================================================
    RpAtomicSetContextualRenderBucket (MTA extension/plugin)

    Arguments:
        theAtomic - the atomic to put the render bucket link in
        theBucket - bucket to put into the atomic
    Purpose:
        Stores a render bucket link into the specified atomic.
        This is used to accelerate bucket sorting in the
        bucket rasterizer.
=========================================================*/
bool __cdecl RpAtomicSetContextualRenderBucket( RpAtomic *theAtomic, RenderBucket::RwRenderBucket *theBucket )
{
    return NULL;
}

/*=========================================================
    RpAtomicGetContextualRenderBucket (MTA extension/plugin)

    Arguments:
        theAtomic - the atomic to get a render bucket link from
    Purpose:
        Attempts to return a render bucket link from the atomic
        in the current rendering context. If returned, it is
        assumed to contain a render bucket that is most-likely
        same to the current context.
=========================================================*/
RenderBucket::RwRenderBucket* __cdecl RpAtomicGetContextualRenderBucket( RpAtomic *theAtomic )
{
    return NULL;
}

// Module initialization.
void RpAtomicD3D9_Init( void )
{
    return;
}

void RpAtomicD3D9_Shutdown( void )
{
    return;
}
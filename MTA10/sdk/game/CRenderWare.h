/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CRenderWare.h
*  PURPOSE:     RenderWare engine interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRENDERWARE
#define __CRENDERWARE

#include "RenderWare_shared.h"

class CD3DDUMMY;
class CSHADERDUMMY;
class CColModel;

typedef void (*PFN_WATCH_CALLBACK) ( CSHADERDUMMY* pContext, CD3DDUMMY* pD3DDataNew, CD3DDUMMY* pD3DDataOld );

enum eWorldRenderMode
{
    WORLD_RENDER_ORIGINAL,
    WORLD_RENDER_MESHLOCAL_ALPHAFIX,
    WORLD_RENDER_SCENE_ALPHAFIX
};

enum eShaderLightingMode
{
    SHADER_LIGHTING_SINGULAR,
    SHADER_LIGHTING_MULTI
};

struct renderBucketStats
{
    unsigned int totalNumberOfRenderCalls;
    unsigned int totalNumberOfActiveBuckets;
    unsigned int maxRenderCallsPerBucket;
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

struct RwTexture;

class CRenderWare
{
public:
    virtual class CRpLight*         CreateLight                         ( RpLightType type ) = 0;
    virtual class CRwFrame*         CreateFrame                         ( void ) = 0;
    virtual class CRwCamera*        CreateCamera                        ( int width, int height ) = 0;
    virtual class CModel*           CreateClump                         ( void ) = 0;

    virtual bool                    IsRendering                         ( void ) const = 0;

    virtual void                    EnableEnvMapRendering               ( bool enabled ) = 0;
    virtual bool                    IsEnvMapRenderingEnabled            ( void ) const = 0;

    // Lighting utilities.
    virtual void                    SetGlobalLightingAlwaysEnabled      ( bool enabled ) = 0;
    virtual bool                    IsGlobalLightingAlwaysEnabled       ( void ) const = 0;

    virtual void                    SetLocalLightingAlwaysEnabled       ( bool enabled ) = 0;
    virtual bool                    IsLocalLightingAlwaysEnabled        ( void ) const = 0;

    // Shader lighting management.
    virtual void                    SetShaderLightingMode               ( eShaderLightingMode mode ) = 0;
    virtual eShaderLightingMode     GetShaderLightingMode               ( void ) const = 0;

    // Rendering modes.
    virtual void                    SetWorldRenderMode                  ( eWorldRenderMode mode ) = 0;
    virtual eWorldRenderMode        GetWorldRenderMode                  ( void ) const = 0;

    virtual void                    GetRenderBucketStatistics           ( renderBucketStats& statsOut ) = 0;

    virtual CColModel*              ReadCOL                             ( CFile *file ) = 0;

    // Shader management logic.
    virtual unsigned short          GetTXDIDForModelID                  ( unsigned short model ) = 0;
    virtual void                    InitWorldTextureWatch               ( PFN_WATCH_CALLBACK pfnWatchCallback ) = 0;
    virtual bool                    AddWorldTextureWatch                ( CSHADERDUMMY* pShaderData, const char* szMatch, float fShaderPriority ) = 0;
    virtual void                    RemoveWorldTextureWatch             ( CSHADERDUMMY* pShaderData, const char* szMatch ) = 0;
    virtual void                    RemoveWorldTextureWatchByContext    ( CSHADERDUMMY* pShaderData ) = 0;
    virtual void                    PulseWorldTextureWatch              ( void ) = 0;
    virtual void                    GetModelTextureNames                ( std::vector < SString >& outNameList, unsigned short model ) = 0;
    virtual void                    GetTxdTextures                      ( std::vector < RwTexture* >& outTextureList, unsigned short txd ) = 0;
    virtual const SString&          GetTextureName                      ( CD3DDUMMY* pD3DData ) = 0;
};

#endif

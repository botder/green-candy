/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.h
*  PURPOSE:     Header file for RenderWare game engine class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRENDERWARESA
#define __CRENDERWARESA

#define WIN32_LEAN_AND_MEAN

#include <game/CRenderWare.h>

#include "CModelInfoSA.h"
#include "CColModelSA.h"

//
// STexInfo and SShadInfo are used for mapping GTA world textures to shaders
//
struct STexInfo;
struct SShadInfo
{
    SShadInfo ( const SString& strMatch, CSHADERDUMMY* pShaderData, float fOrderPriority )
        : strMatch ( strMatch.ToLower () )
        , pShaderData ( pShaderData )
        , fOrderPriority ( fOrderPriority )
    {
    }
    const SString           strMatch;           // Always lower case
    CSHADERDUMMY* const     pShaderData;
    const float             fOrderPriority;
    std::set < STexInfo* >  associatedTexInfoMap;
};

struct STexInfo
{
    STexInfo ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData )
        : usTxdId ( usTxdId )
        , strTextureName ( strTextureName.ToLower () )
        , pD3DData ( pD3DData )
        , pAssociatedShadInfo ( NULL )
    {
    }
    const ushort            usTxdId;
    const SString           strTextureName;     // Always lower case
    CD3DDUMMY* const        pD3DData;
    SShadInfo*              pAssociatedShadInfo;
};

class CRenderWareSA : public CRenderWare
{
public:
                        CRenderWareSA                   ( enum eGameVersion version );
                        ~CRenderWareSA                  ( void );

    RwMatrix*           AllocateMatrix                  ( void );
    CRpLight*           CreateLight                     ( RpLightType type );
    CRwFrame*           CreateFrame                     ( void );
    CRwCamera*          CreateCamera                    ( int width, int height );
    CModel*             CreateClump                     ( void );

    bool                IsRendering                     ( void ) const;
    
    void                EnableEnvMapRendering           ( bool enabled );
    bool                IsEnvMapRenderingEnabled        ( void ) const;

    // Lighting utilities.
    void                SetGlobalLightingAlwaysEnabled  ( bool enabled );
    bool                IsGlobalLightingAlwaysEnabled   ( void ) const;

    void                SetLocalLightingAlwaysEnabled   ( bool enabled );
    bool                IsLocalLightingAlwaysEnabled    ( void ) const;

    // Shader lighting management.
    void                SetShaderLightingMode           ( eShaderLightingMode mode );
    eShaderLightingMode GetShaderLightingMode           ( void ) const;

    // Rendering modes.
    void                SetWorldRenderMode              ( eWorldRenderMode mode );
    eWorldRenderMode    GetWorldRenderMode              ( void ) const;

    void                GetRenderBucketStatistics       ( renderBucketStats& statsOut );

    // uiModelID == 0 means no collisions will be loaded
    RpClump*            ReadDFF                         ( CFile *file, unsigned short usModelID, CColModelSA*& colOut );
    CColModel*          ReadCOL                         ( CFile *file );

    // Positions the front seat by reading out the vector from the 'ped_frontseat' atomic in the clump (RpClump*)
    // and changing the vector in the CModelInfo class identified by the model id (usModelID)
    bool                PositionFrontSeat               ( RpClump *clump, unsigned short usModelID );

    // Shader management logic.
    ushort              GetTXDIDForModelID              ( ushort usModelID );
    void                InitWorldTextureWatch           ( PFN_WATCH_CALLBACK pfnWatchCallback );
    bool                AddWorldTextureWatch            ( CSHADERDUMMY* pShaderData, const char* szMatch, float fOrderPriority );
    void                RemoveWorldTextureWatch         ( CSHADERDUMMY* pShaderData, const char* szMatch );
    void                RemoveWorldTextureWatchByContext ( CSHADERDUMMY* pShaderData );
    void                PulseWorldTextureWatch          ( void );
    void                GetModelTextureNames            ( std::vector < SString >& outNameList, ushort usModelID );
    void                GetTxdTextures                  ( std::vector < RwTexture* >& outTextureList, ushort usTxdId );
    const SString&      GetTextureName                  ( CD3DDUMMY* pD3DData );

protected:
    // Protected Shader management logic.
    void                InitShaderSystem                ( void );
    void                ShutdownShaderSystem            ( void );

    void                AddActiveTexture                ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData );
    void                RemoveTxdActiveTextures         ( ushort usTxdId );
    void                FindNewAssociationForTexInfo    ( STexInfo* pTexInfo );
    STexInfo*           CreateTexInfo                   ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData );
    void                OnDestroyTexInfo                ( STexInfo* pTexInfo );
    SShadInfo*          CreateShadInfo                  ( CSHADERDUMMY* pShaderData, const SString& strTextureName, float fOrderPriority );
    void                OnDestroyShadInfo               ( SShadInfo* pShadInfo );
    void                MakeAssociation                 ( SShadInfo* pShadInfo, STexInfo* pTexInfo );
    void                BreakAssociation                ( SShadInfo* pShadInfo, STexInfo* pTexInfo );

    // Shader management variables.
    // Watched world textures
    std::list < SShadInfo >                 m_ShadInfoList;
    std::list < STexInfo >                  m_TexInfoList;

    std::map < SString, STexInfo* >         m_UniqueTexInfoMap;
    std::map < CD3DDUMMY*, STexInfo* >      m_D3DDataTexInfoMap;
    std::map < SString, SShadInfo* >        m_UniqueShadInfoMap;
    std::multimap < float, SShadInfo* >     m_orderMap;

    PFN_WATCH_CALLBACK                      m_pfnWatchCallback;
};

extern IDirect3DDevice9 *const *g_renderDevice;

// Include sub modules
#include "CRenderWareSA.rwapi.h"
#include "CRenderWareSA.pipeline.h"
#include "CRenderWareSA.mem.h"
#include "CRenderWareSA.render.h"
#include "CRenderWareSA.rtcallback.h"
#include "CRenderWareSA.lighting.h"
#include "CRenderWareSA.shader.h"
#include "CRenderWareSA.rtbucket.h"

#endif

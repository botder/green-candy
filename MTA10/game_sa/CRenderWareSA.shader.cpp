/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.shader.cpp
*  PURPOSE:     RenderWare shader implementation for MTA
*               SubInterface of RenderWare
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

//
// STxdAction stores the current frame txd create and destroy events
//
struct STxdAction
{
    bool bAdd;
    ushort usTxdId;
};

std::vector < STxdAction > ms_txdActionList;

//
// Hooks for creating txd create and destroy events
//

void CRenderWareSA::InitShaderSystem( void )
{
    // Our stuff
    m_pfnWatchCallback = NULL;
}

void CRenderWareSA::ShutdownShaderSystem( void )
{
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTXDIDForModelID
//
// Get a TXD ID associated with the model ID
//
////////////////////////////////////////////////////////////////
ushort CRenderWareSA::GetTXDIDForModelID ( unsigned short usModelID )
{
    if ( usModelID >= DATA_TEXTURE_BLOCK && usModelID < DATA_TEXTURE_BLOCK + MAX_TXD )
    {
        // Get global TXD ID instead
        return usModelID - DATA_TEXTURE_BLOCK;
    }

    // Ensure valid
    if ( usModelID >= DATA_TEXTURE_BLOCK || !ppModelInfo[usModelID] )
        return 0;

    return ppModelInfo[usModelID]->usTextureDictionary;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::InitWorldTextureWatch
//
// Setup texture watch callback
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::InitWorldTextureWatch( PFN_WATCH_CALLBACK pfnWatchCallback )
{
    m_pfnWatchCallback = pfnWatchCallback;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AddWorldTextureWatch
//
// Begin watching for changes to the d3d resource associated with this texture name 
//
// Returns false if shader/match already exists
//
////////////////////////////////////////////////////////////////
bool CRenderWareSA::AddWorldTextureWatch( CSHADERDUMMY* pShaderData, const char* szMatch, float fOrderPriority )
{
    SShadInfo* pNewShadInfo = CreateShadInfo( pShaderData, szMatch, fOrderPriority );
    if ( !pNewShadInfo )
        return false;

    // Step through texinfo list looking for all matches
    for ( std::list < STexInfo > ::iterator iter = m_TexInfoList.begin () ; iter != m_TexInfoList.end () ; ++iter )
    {
        STexInfo* pTexInfo = &*iter;
        if ( WildcardMatch ( pNewShadInfo->strMatch, pTexInfo->strTextureName ) )
        {
            // Check previous association
            if ( SShadInfo* pPrevShadInfo = pTexInfo->pAssociatedShadInfo )
            {
				// Check priority
                if ( pPrevShadInfo->fOrderPriority > pNewShadInfo->fOrderPriority )
                    continue;

                // Remove previous association
                BreakAssociation ( pPrevShadInfo, pTexInfo );
            }

            // Add new association
            MakeAssociation ( pNewShadInfo, pTexInfo );
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::MakeAssociation
//
// Make the texture use the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::MakeAssociation ( SShadInfo* pShadInfo, STexInfo* pTexInfo )
{
    assert ( !pTexInfo->pAssociatedShadInfo );
    assert ( !MapContains ( pShadInfo->associatedTexInfoMap, pTexInfo ) );
    pTexInfo->pAssociatedShadInfo = pShadInfo;
    pShadInfo->associatedTexInfoMap.insert ( pTexInfo );

    if ( m_pfnWatchCallback )
        m_pfnWatchCallback ( pShadInfo->pShaderData, pTexInfo->pD3DData, NULL );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::BreakAssociation
//
// Stop the texture using the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::BreakAssociation ( SShadInfo* pShadInfo, STexInfo* pTexInfo )
{
    assert ( pTexInfo->pAssociatedShadInfo == pShadInfo );
    assert ( MapContains ( pShadInfo->associatedTexInfoMap, pTexInfo ) );
    pTexInfo->pAssociatedShadInfo = NULL;
    MapRemove ( pShadInfo->associatedTexInfoMap, pTexInfo );
    if ( m_pfnWatchCallback )
        m_pfnWatchCallback ( pShadInfo->pShaderData, NULL, pTexInfo->pD3DData );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveWorldTextureWatch
//
// End watching for changes to the d3d resource associated with this texture name
//
// When shadinfo removed:
//      unassociate with matches, let each texture find a new latest match
//
// If szMatch is NULL, remove all usage of the shader
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveWorldTextureWatch ( CSHADERDUMMY* pShaderData, const char* szMatch )
{
    SString strMatch = SStringX ( szMatch ).ToLower ();
    // Find shadInfo
    for ( std::list < SShadInfo > ::iterator iter = m_ShadInfoList.begin () ; iter != m_ShadInfoList.end () ; )
    {
        SShadInfo* pShadInfo = &*iter;
        if ( pShadInfo->pShaderData == pShaderData && ( !szMatch || pShadInfo->strMatch == strMatch ) )
        {
            std::vector < STexInfo* > reassociateList;

            // Unassociate with all matches
            while ( !pShadInfo->associatedTexInfoMap.empty () )
            {
                STexInfo* pTexInfo = *pShadInfo->associatedTexInfoMap.begin ();
                BreakAssociation ( pShadInfo, pTexInfo );

                // reassociateList for earlier shaders
                reassociateList.push_back ( pTexInfo );
            }

            OnDestroyShadInfo ( pShadInfo );
            iter = m_ShadInfoList.erase ( iter );

            // Allow unassociated texinfos to find new associations
            for ( uint i = 0 ; i < reassociateList.size () ; i++ )
            {
                STexInfo* pTexInfo = reassociateList [ i ];
                FindNewAssociationForTexInfo ( pTexInfo );
            }

			// If a match string was supplied, there should be only one in the list, so we can stop here
            if ( szMatch )
                break;
        }
        else
            ++iter;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveWorldTextureWatchByContext
//
// End watching for changes to the d3d resource associated with this context 
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveWorldTextureWatchByContext ( CSHADERDUMMY* pShaderData )
{
    RemoveWorldTextureWatch ( pShaderData, NULL );
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::PulseWorldTextureWatch
//
// Update watched textures status
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::PulseWorldTextureWatch()
{
    // Go through ms_txdActionList
    for ( std::vector < STxdAction >::const_iterator iter = ms_txdActionList.begin () ; iter != ms_txdActionList.end () ; ++iter )
    {
        const STxdAction& action = *iter;
        if ( action.bAdd )
        {
            //
            // New txd has been loaded
            //

            // Get list of texture names and data to add

            // Note: If txd has been unloaded since, textureList will be empty
            std::vector < RwTexture* > textureList;
            GetTxdTextures ( textureList, action.usTxdId );

            for ( std::vector < RwTexture* > ::iterator iter = textureList.begin () ; iter != textureList.end () ; ++iter )
            {
                RwTexture* texture = *iter;
                const char* szTextureName = texture->name;
                CD3DDUMMY* pD3DData = (CD3DDUMMY*)RwTextureGetVideoData( texture );
                AddActiveTexture ( action.usTxdId, szTextureName, pD3DData );
            }
        }
        else
        {
            //
            // Txd has been unloaded
            //

            RemoveTxdActiveTextures ( action.usTxdId );
        }
    }

    ms_txdActionList.clear ();
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::AddActiveTexture
//
// Create a texinfo for the texture and find a best match shader for it
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::AddActiveTexture( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData )
{
    STexInfo* pTexInfo = CreateTexInfo ( usTxdId, strTextureName, pD3DData );
    FindNewAssociationForTexInfo ( pTexInfo );
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::CreateTexInfo
//
////////////////////////////////////////////////////////////////
STexInfo* CRenderWareSA::CreateTexInfo ( ushort usTxdId, const SString& strTextureName, CD3DDUMMY* pD3DData )
{
    // Create texinfo
    m_TexInfoList.push_back ( STexInfo ( usTxdId, strTextureName, pD3DData ) );
    STexInfo* pTexInfo = &m_TexInfoList.back ();

    // Add to lookup maps
    SString strUniqueKey ( "%d_%s", pTexInfo->usTxdId, *pTexInfo->strTextureName );
    //assert ( !MapContains ( m_UniqueTexInfoMap, strUniqueKey ) );
    MapSet ( m_UniqueTexInfoMap, strUniqueKey, pTexInfo );

	// This assert fails when using engine txd replace functions - TODO find out why
    //assert ( !MapContains ( m_D3DDataTexInfoMap, pTexInfo->pD3DData ) );
    MapSet ( m_D3DDataTexInfoMap, pTexInfo->pD3DData, pTexInfo );
    return pTexInfo;
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDestroyTexInfo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDestroyTexInfo ( STexInfo* pTexInfo )
{
    // Remove from lookup maps
    SString strUniqueKey ( "%d_%s", pTexInfo->usTxdId, *pTexInfo->strTextureName );
    assert ( MapContains ( m_UniqueTexInfoMap, strUniqueKey ) );
    MapRemove ( m_UniqueTexInfoMap, strUniqueKey );

	// This assert fails when using engine txd replace functions - TODO find out why
    //assert ( MapContains ( m_D3DDataTexInfoMap, pTexInfo->pD3DData ) );
    MapRemove ( m_D3DDataTexInfoMap, pTexInfo->pD3DData );
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::CreateShadInfo
//
////////////////////////////////////////////////////////////////
SShadInfo* CRenderWareSA::CreateShadInfo (  CSHADERDUMMY* pShaderData, const SString& strMatch, float fOrderPriority )
{
    // Create shadinfo
    m_ShadInfoList.push_back ( SShadInfo ( strMatch, pShaderData, fOrderPriority ) );
    SShadInfo* pShadInfo = &m_ShadInfoList.back ();

    // Check for uniqueness
    SString strUniqueKey ( "%08x_%s", pShadInfo->pShaderData, *pShadInfo->strMatch );
    if ( MapContains ( m_UniqueShadInfoMap, strUniqueKey ) )
    {
        // Remove again if not unique
        m_ShadInfoList.pop_back ();
        return NULL;
    }

    // Set unique map
    MapSet ( m_UniqueShadInfoMap, strUniqueKey, pShadInfo );

    // Add to order map
    MapInsert ( m_orderMap, pShadInfo->fOrderPriority, pShadInfo );

    return pShadInfo;
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::OnDestroyShadInfo
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::OnDestroyShadInfo ( SShadInfo* pShadInfo )
{
    // Remove from lookup maps
    SString strUniqueKey ( "%08x_%s", pShadInfo->pShaderData, *pShadInfo->strMatch );
    assert ( MapContains ( m_UniqueShadInfoMap, strUniqueKey ) );
    MapRemove ( m_UniqueShadInfoMap, strUniqueKey );

    //  Remove from order map
    MapRemovePair ( m_orderMap, pShadInfo->fOrderPriority, pShadInfo );
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::FindNewAssociationForTexInfo
//
// Find best match for this texinfo
// Called when a texture is loaded or a shader is removed from a texture
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::FindNewAssociationForTexInfo ( STexInfo* pTexInfo )
{
    // Stepping backwards will bias high priority, and if the priorty is the same, bias the latest additions
    for ( std::multimap < float, SShadInfo* > ::reverse_iterator iter = m_orderMap.rbegin () ; iter != m_orderMap.rend () ; ++iter )
    {
        SShadInfo* pShadInfo = iter->second;
        if ( WildcardMatch ( pShadInfo->strMatch, pTexInfo->strTextureName ) )
        {
            // Found one
            MakeAssociation ( pShadInfo, pTexInfo );
            break;
        }
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::RemoveTxdActiveTextures
//
// Called when a TXD is being unloaded.
// Delete texinfos which came from that TXD
//
////////////////////////////////////////////////////////////////
void CRenderWareSA::RemoveTxdActiveTextures ( ushort usTxdId )
{
    // Find all TexInfo's for this txd
    for ( std::list < STexInfo > ::iterator iter = m_TexInfoList.begin () ; iter != m_TexInfoList.end () ; )
    {
        STexInfo* pTexInfo = &*iter;
        if ( pTexInfo->usTxdId == usTxdId )
        {
            // If texinfo has a shadinfo, unassociate
            if ( SShadInfo* pShadInfo = pTexInfo->pAssociatedShadInfo )
            {
                BreakAssociation ( pShadInfo, pTexInfo );
            }

            OnDestroyTexInfo ( pTexInfo );
            iter = m_TexInfoList.erase ( iter );
        }
        else
            ++iter;
    }
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetModelTextureNames
//
// Get list of texture names associated with the model
//
////////////////////////////////////////////////////////////////
static int StaticGetTxdTextureNames( RwTexture *tex, std::vector <SString> *names )
{
    names->push_back( tex->name );
    return true;
}

void CRenderWareSA::GetModelTextureNames ( std::vector < SString >& outNameList, unsigned short usModelID )
{
    TextureManager::GetTxdPool()->Get( GetTXDIDForModelID( usModelID ) )->m_txd->ForAllTextures( StaticGetTxdTextureNames, &outNameList );
}


////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTxdTextures
//
////////////////////////////////////////////////////////////////
static int StaticGetTextureCB( RwTexture* texture, std::vector <RwTexture*>* list )
{
    list->push_back( texture );
    return true;
}

void CRenderWareSA::GetTxdTextures ( std::vector < RwTexture* >& outTextureList, ushort usTxdId )
{
    if ( usTxdId == 0 )
        return;

    CTxdInstanceSA *txd = TextureManager::GetTxdPool()->Get( usTxdId );

    if ( !txd )
        return;

    if ( !txd->m_txd )
        return;

    txd->m_txd->ForAllTextures( StaticGetTextureCB, &outTextureList );
}

////////////////////////////////////////////////////////////////
//
// CRenderWareSA::GetTextureName
//
////////////////////////////////////////////////////////////////
const SString& CRenderWareSA::GetTextureName ( CD3DDUMMY* pD3DData )
{
    STexInfo** ppTexInfo = MapFind ( m_D3DDataTexInfoMap, pD3DData );
    if ( ppTexInfo )
        return (*ppTexInfo)->strTextureName;
    static SString strDummy;
    return strDummy;
}

////////////////////////////////////////////////////////////////
//
// Hooks
//
////////////////////////////////////////////////////////////////

void _cdecl OnStreamingAddedTxd( unsigned short id )
{
    STxdAction action;
    action.bAdd = true;
    action.usTxdId = id ;
    ms_txdActionList.push_back ( action );
}

void _cdecl OnStreamingRemoveTxd( unsigned short id )
{
    STxdAction action;
    action.bAdd = false;
    action.usTxdId = id - 20000;
    ms_txdActionList.push_back ( action );
}

/////////////////////////////////////////

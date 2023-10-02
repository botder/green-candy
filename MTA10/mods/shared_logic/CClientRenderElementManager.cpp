/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElementManager.cpp
*  PURPOSE:
*  DEVELOPERS:  idiot
*
*****************************************************************************/

#include "StdInc.h"


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::CClientRenderElementManager ( CClientManager* pClientManager )
{
    m_pClientManager = pClientManager;
    m_pRenderItemManager = g_pCore->GetGraphics ()->GetRenderItemManager ();
    m_uiStatsDxFontCount = 0;
    m_uiStatsGuiFontCount = 0;
    m_uiStatsTextureCount = 0;
    m_uiStatsShaderCount = 0;
    m_uiStatsRenderTargetCount = 0;
    m_uiStatsScreenSourceCount = 0;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::~CClientRenderElementManager
//
//
////////////////////////////////////////////////////////////////
CClientRenderElementManager::~CClientRenderElementManager()
{
    // Remove any existing
    while ( m_ItemElementMap.size () )
        Remove ( m_ItemElementMap.begin ()->second );
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateDxFont
//
//
//
////////////////////////////////////////////////////////////////
CClientDxFont* CClientRenderElementManager::CreateDxFont( const SString& strFullFilePath, uint uiSize, bool bBold, LuaClass& root )
{
    // Create the item
    CDxFontItem* pDxFontItem = m_pRenderItemManager->CreateDxFont ( strFullFilePath, uiSize, bBold );

    // Check create worked
    if ( !pDxFontItem )
        return NULL;

    // Create the element
    CClientDxFont *pDxFontElement = new CClientDxFont( m_pClientManager, INVALID_ELEMENT_ID, root.GetVM(), pDxFontItem );
    pDxFontElement->SetRoot( &root );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pDxFontItem, pDxFontElement );

    // Update stats
    m_uiStatsDxFontCount++;

    return pDxFontElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateGuiFont
//
//
//
////////////////////////////////////////////////////////////////
CClientGuiFont* CClientRenderElementManager::CreateGuiFont( const SString& strFullFilePath, const SString& strUniqueName, uint uiSize, LuaClass& root )
{
    // Create the item
    CGuiFontItem* pGuiFontItem = m_pRenderItemManager->CreateGuiFont( strFullFilePath, strUniqueName, uiSize );

    // Check create worked
    if ( !pGuiFontItem )
        return NULL;

    // Create the element
    CClientGuiFont* pGuiFontElement = new CClientGuiFont( m_pClientManager, INVALID_ELEMENT_ID, pGuiFontItem, root.GetVM() );
    pGuiFontElement->SetRoot( &root );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pGuiFontItem, pGuiFontElement );

    // Update stats
    m_uiStatsGuiFontCount++;

    return pGuiFontElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateTexture
//
//
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::CreateTexture ( const SString& strFullFilePath, LuaClass& root )
{
    // Create the item
    CTextureItem* pTextureItem = m_pRenderItemManager->CreateTexture( strFullFilePath );

    // Check create worked
    if ( !pTextureItem )
        return NULL;

    // Create the element
    CClientTexture *pTextureElement = new CClientTexture( m_pClientManager, INVALID_ELEMENT_ID, root.GetVM(), pTextureItem );
    pTextureElement->SetRoot( &root );

    // Add to this manager's list
    MapSet( m_ItemElementMap, pTextureItem, pTextureElement );

    // Update stats
    m_uiStatsTextureCount++;

    return pTextureElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateShader
//
//
//
////////////////////////////////////////////////////////////////
#include "SharedUtil.FrameworkABI.h"

CClientShader* CClientRenderElementManager::CreateShader ( const SString& strFullFilePath, const SString& strRootPath, SString& strOutStatus, bool bDebug, LuaClass& root )
{
    // Create the item
    //TODO: maxdist prio
    abiString getStatusStr;

    CShaderItem* pShaderItem = m_pRenderItemManager->CreateShader ( strFullFilePath, strRootPath, getStatusStr, 0, 0, bDebug );

    strOutStatus = std::string( getStatusStr );

    // Check create worked
    if ( !pShaderItem )
        return NULL;

    // Create the element
    CClientShader* pShaderElement = new CClientShader( m_pClientManager, INVALID_ELEMENT_ID, root.GetVM(), pShaderItem );
    pShaderElement->SetRoot( &root );

    // Add to this manager's list
    MapSet( m_ItemElementMap, pShaderItem, pShaderElement );

    // Update stats
    m_uiStatsShaderCount++;

    return pShaderElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateRenderTarget
//
//
//
////////////////////////////////////////////////////////////////
CClientRenderTarget* CClientRenderElementManager::CreateRenderTarget ( uint uiSizeX, uint uiSizeY, bool bWithAlphaChannel, LuaClass& root )
{
    // Create the item
    CRenderTargetItem* pRenderTargetItem = m_pRenderItemManager->CreateRenderTarget ( uiSizeX, uiSizeY, bWithAlphaChannel );

    // Check create worked
    if ( !pRenderTargetItem )
        return NULL;

    // Create the element
    CClientRenderTarget* pRenderTargetElement = new CClientRenderTarget ( m_pClientManager, INVALID_ELEMENT_ID, root.GetVM(), pRenderTargetItem );
    pRenderTargetElement->SetRoot( &root );

    // Add to this manager's list
    MapSet ( m_ItemElementMap, pRenderTargetItem, pRenderTargetElement );

    // Update stats
    m_uiStatsRenderTargetCount++;

    return pRenderTargetElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::CreateScreenSource
//
//
//
////////////////////////////////////////////////////////////////
CClientScreenSource* CClientRenderElementManager::CreateScreenSource( uint uiSizeX, uint uiSizeY, LuaClass& root )
{
    // Create the item
    CScreenSourceItem *pScreenSourceItem = m_pRenderItemManager->CreateScreenSource( uiSizeX, uiSizeY );

    // Check create worked
    if ( !pScreenSourceItem )
        return NULL;

    // Create the element
    CClientScreenSource *pScreenSourceElement = new CClientScreenSource( m_pClientManager, INVALID_ELEMENT_ID, root.GetVM(), pScreenSourceItem );
    pScreenSourceElement->SetRoot( &root );

    // Add to this manager's list
    MapSet( m_ItemElementMap, pScreenSourceItem, pScreenSourceElement );

    // Update stats
    m_uiStatsScreenSourceCount++;

    return pScreenSourceElement;
}

////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::FindAutoTexture
//
// Find texture by unique name. Create if not found.
//
////////////////////////////////////////////////////////////////
CClientTexture* CClientRenderElementManager::FindAutoTexture ( const SString& strFullFilePath, const SString& strUniqueName, LuaClass& root )
{
    // Check if we've already done this file
    CClientTexture** ppTextureElement = MapFind ( m_AutoTextureMap, strUniqueName );
    if ( !ppTextureElement )
    {
        // Try to create
        CClientTexture* pNewTextureElement = CreateTexture ( strFullFilePath, root );
        if ( !pNewTextureElement )
            return NULL;

        // Add to automap if created
        MapSet ( m_AutoTextureMap, strUniqueName, pNewTextureElement );
        ppTextureElement = MapFind ( m_AutoTextureMap, strUniqueName );
    }

    return *ppTextureElement;
}


////////////////////////////////////////////////////////////////
//
// CClientRenderElementManager::Remove
//
// Called when an element is being deleted.
// Remove from lists and release render item
//
////////////////////////////////////////////////////////////////
void CClientRenderElementManager::Remove ( CClientRenderElement* pElement )
{
    // Validate
    assert ( pElement == *MapFind( m_ItemElementMap, pElement->GetRenderItem () ) );

    // Remove from this managers list
    MapRemove ( m_ItemElementMap, pElement->GetRenderItem () );

    // Remove from auto texture map
    if ( pElement->IsTransmit( LUACLASS_CORETEXTURE ) )
    {
        for ( std::map < SString, CClientTexture* >::iterator iter = m_AutoTextureMap.begin () ; iter != m_AutoTextureMap.end () ; ++iter )
        {
            if ( iter->second == pElement )
            {
                m_AutoTextureMap.erase ( iter );
                break;
            }
        }
    }

    // Update stats
    if ( pElement->IsTransmit( LUACLASS_DXFONT ) )
        m_uiStatsDxFontCount--;
    else
    if ( pElement->IsTransmit( LUACLASS_GUIFONT ) )
        m_uiStatsGuiFontCount--;
    else
    if ( pElement->IsTransmit( LUACLASS_SHADER ) )
        m_uiStatsShaderCount--;
    else
    if ( pElement->IsTransmit( LUACLASS_CORERENDERTARGET ) )
        m_uiStatsRenderTargetCount--;
    else
    if ( pElement->IsTransmit( LUACLASS_CORESCREENSOURCE ) )
        m_uiStatsScreenSourceCount--;
    else
    if ( pElement->IsTransmit( LUACLASS_CORETEXTURE ) )
        m_uiStatsTextureCount--;

    // Release render item
    pElement->GetRenderItem()->Release();
}

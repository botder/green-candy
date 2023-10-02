/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientTexture.h
*  PURPOSE:     Core texture interface for deathmatch
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDER_TEXTURE_
#define _RENDER_TEXTURE_

#define LUACLASS_CORETEXTURE        67
#define LUACLASS_CORERENDERTARGET   88
#define LUACLASS_CORESCREENSOURCE   89

class CClientTexture : public CClientMaterial
{
public:
                            CClientTexture( CClientManager *pManager, ElementID ID, lua_State *L, CTextureItem *pTextureItem );

    eClientEntityType       GetType() const                                     { return CCLIENTTEXTURE; }

    // CClientTexture methods
    CTextureItem*           GetTextureItem()                                    { return (CTextureItem*)m_pRenderItem; }

};

class CClientRenderTarget : public CClientTexture
{
public:
                            CClientRenderTarget( CClientManager *pManager, ElementID ID, lua_State *L, CRenderTargetItem *pRenderTargetItem );

    // CClientRenderTarget methods
    CRenderTargetItem*      GetRenderTargetItem()                               { return (CRenderTargetItem*)m_pRenderItem; }
};

class CClientScreenSource : public CClientTexture
{
public:
                            CClientScreenSource( CClientManager *pManager, ElementID ID, lua_State *L, CScreenSourceItem *pScreenSourceItem );

    // CClientScreenSource methods
    CScreenSourceItem*      GetScreenSourceItem()                               { return (CScreenSourceItem*)m_pRenderItem; }
};

#endif //_RENDER_TEXTURE_
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientRenderElement.h
*  PURPOSE:     Client module's version of something renderable
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDER_ELEMENT_
#define _RENDER_ELEMENT_

#define LUACLASS_RENDERELEMENT  52

class CClientRenderElement : public CClientEntity
{
public:
                            CClientRenderElement( CClientManager* pManager, ElementID ID, lua_State *L );
                            ~CClientRenderElement();

    void                    Unlink();
    void                    GetPosition( CVector& vecPosition ) const           { vecPosition = CVector(); }
    void                    SetPosition( const CVector& vecPosition )           {}

    // CClientRenderElement methods
    CRenderItem*            GetRenderItem()                                     { return m_pRenderItem; }

protected:
    bool                            bDoneUnlink;
    CRenderItem*                    m_pRenderItem;
    CClientRenderElementManager*    m_pRenderElementManager;
};

#endif //_RENDER_ELEMENT_
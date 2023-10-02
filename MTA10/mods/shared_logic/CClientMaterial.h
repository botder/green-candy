/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientMaterial.h
*  PURPOSE:     Deathmatch render material management
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_MATERIAL_
#define _CLIENT_MATERIAL_

#define LUACLASS_MATERIAL   89

class CClientMaterial : public CClientRenderElement
{
public:
                            CClientMaterial( CClientManager* pManager, ElementID ID, lua_State *L );

    CMaterialItem*          GetMaterialItem()                                   { return (CMaterialItem*)m_pRenderItem; }
};

#endif //_CLIENT_MATERIAL_
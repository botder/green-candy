/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientShader.h
*  PURPOSE:     Core shader rendering management for deathmatch
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_SHADER_
#define _CLIENT_SHADER_

#define LUACLASS_SHADER     69

class CClientShader : public CClientMaterial
{
public:
                            CClientShader( CClientManager *pManager, ElementID ID, lua_State *L, CShaderItem *pShaderItem );

    eClientEntityType       GetType() const                                     { return CCLIENTSHADER; }

    // CClientShader methods
    CShaderItem*            GetShaderItem()                                     { return (CShaderItem*)m_pRenderItem; }

};

#endif //_CLIENT_SHADER_
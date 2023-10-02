/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientLight.h
*  PURPOSE:     RenderWare light virtualization
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_RWLIGHT_
#define _CLIENT_RWLIGHT_

#define LUACLASS_LIGHT  101

class CClientLight : public CClientRwObject
{
public:
                                    CClientLight( lua_State *L, CClientDFF *model, CRpLight& light );
                                    ~CClientLight();

    void                            SetModel( CClientDFF *model );

    CClientDFF* m_clump;
    CRpLight&   m_light;
};

#endif //_CLIENT_RWLIGHT_
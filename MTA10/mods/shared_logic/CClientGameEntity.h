/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGameEntity.h
*  PURPOSE:     Game rendered entity instance
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CLIENT_GAME_ENTITY_
#define _CLIENT_GAME_ENTITY_

#include "CClientStreamElement.h"

class CClientColModel;
class CClientGameTexture;

#define LUACLASS_GAMEENTITY 104

class CClientGameEntity abstract : public CClientStreamElement
{
public:
                                CClientGameEntity( CClientStreamer *pStreamer, ElementID ID, lua_State *L, bool system );
                                ~CClientGameEntity();

    void                        SetColModel( CClientColModel *col );
    CClientColModel*            GetColModel()                           { return m_colModel; }

    void                        ReplaceTexture( const char *name, CClientGameTexture *tex );
    bool                        IsReplacingTexture( const char *name );
    void                        RestoreTexture( const char *name );

    void                        NotifyCreate();

protected:
    CClientColModel*            m_colModel;

    typedef std::map <std::string, CClientGameTexture*> textureMap_t;

    textureMap_t                m_replacedTextures;
};

#endif //_CLIENT_GAME_ENTITY_
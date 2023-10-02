/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModel.h
*  PURPOSE:     Model collision (.col file) entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCLIENTCOLMODEL_H
#define __CCLIENTCOLMODEL_H

#define LUACLASS_COLMODEL   85

class CClientColModel : public LuaElement
{
public:
                                    CClientColModel( lua_State *L );
                                    ~CClientColModel();

    bool                            LoadCol( CFile *file );
    inline bool                     IsLoaded() const                { return m_pColModel != NULL; };

    bool                            Replace( unsigned short usModel );
    bool                            HasReplaced( unsigned short usModel ) const;
    bool                            Restore( unsigned short usModel );
    void                            RestoreAll();

    CColModel*                      m_pColModel;

private:
    std::list <unsigned short>      m_Replaced;
};

#endif

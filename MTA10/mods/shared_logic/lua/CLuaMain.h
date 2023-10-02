/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaMain.h
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAMAIN_H
#define __CLUAMAIN_H

#include "CLuaTimerManager.h"
#include "CLuaFunctionDefs.h"

//class CLuaManager;
class CResource;

class CLuaMain : public LuaMain
{
public:
                                    CLuaMain( CLuaManager& manager, CFileTranslator& fileRoot );
                                    ~CLuaMain();

    void                            InitVM( int structure, int meta );

    inline CResource*               GetResource()       { return (CResource*)m_resource; }
    inline const CResource*         GetResource() const { return (const CResource*)m_resource; }

    bool                            ParseRelative( const char *in, filePath& out ) const;

    CXMLFile*                       CreateXML( const char *path );
    void                            SaveXML( CXMLNode * pRootNode );
    unsigned long                   GetElementCount();

protected:
    CFileTranslator&    m_fileRoot;
};

#endif

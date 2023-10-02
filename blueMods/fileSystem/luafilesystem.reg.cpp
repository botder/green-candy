/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafilesystem.reg.cpp
*  PURPOSE:     Pointer registry for CFileTranslator
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static eir::Set <CFileTranslator*, FileSysCommonAllocator> _reg_ftrans;

void fsRegisterTranslator( CFileTranslator *trans )
{
    _reg_ftrans.Insert( trans );
}

bool fsIsTranslator( void *objptr )
{
    return ( _reg_ftrans.Find( (CFileTranslator*)objptr ) != nullptr );
}

void fsCleanupTranslator( CFileTranslator *trans )
{
    delete trans;

    _reg_ftrans.Remove( trans );
}

void fsShutdownTranslators( void )
{
    for ( CFileTranslator *trans : _reg_ftrans )
    {
        delete trans;
    }

    _reg_ftrans.Clear();
}

CFileTranslator* fsLuaGetTranslator( lua_State *L, int idx )
{
    void *obj = luaclass_getpointer( L, idx );

    if ( fsIsTranslator( obj ) == false )
    {
        lua_pushlstring( L, "not a file-translator", 10 );
        lua_error( L );
    }

    return (CFileTranslator*)obj;
}
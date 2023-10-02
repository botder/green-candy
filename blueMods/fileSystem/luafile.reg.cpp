/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.reg.cpp
*  PURPOSE:     Pointer registry for CFile
*
*  For documentation visit http://wiki.mtasa.com/wiki/MTA:Eir/FileSystem/
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

static eir::Set <CFile*, FileSysCommonAllocator> _reg_files;

void fsRegisterFile( CFile *obj )
{
    _reg_files.Insert( obj );
}

bool fsIsFile( void *objptr )
{
    return ( _reg_files.Find( (CFile*)objptr ) != nullptr );
}

void fsCleanupFile( CFile *obj )
{
    delete obj;

    _reg_files.Remove( obj );
}

void fsShutdownFiles( void )
{
    for ( CFile *file : _reg_files )
    {
        delete file;
    }

    _reg_files.Clear();
}

CFile* fsLuaGetFile( lua_State *L, int idx )
{
    void *obj = luaclass_getpointer( L, idx );

    if ( fsIsFile( obj ) == false )
    {
        lua_pushlstring( L, "not a file", 10 );
        lua_error( L );
    }

    return (CFile*)obj;
}
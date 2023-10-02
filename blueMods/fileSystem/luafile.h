/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafile.h
*  PURPOSE:     File environment header
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILELIB_
#define _FILELIB_

#define LUACLASS_FILE   136

void luafile_makemeta( lua_State *lua, int ridx );

void fsRegisterFile( CFile *obj );
bool fsIsFile( void *objptr );
void fsCleanupFile( CFile *obj );
void fsShutdownFiles( void );
CFile* fsLuaGetFile( lua_State *L, int idx );

#endif //_FILELIB_

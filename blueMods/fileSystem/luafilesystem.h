/*****************************************************************************
*
*  PROJECT:     Eir FileSystem for MTA:BLUE
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafilesystem.h
*  PURPOSE:     Lua filesystem access
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILESYSTEMLIB_
#define _FILESYSTEMLIB_

#define LUACLASS_FILETRANSLATOR     137

int luafsys_createArchiveTranslator( lua_State *L );
int luafsys_createZIPArchive( lua_State *L );
void luafilesystem_open( lua_State *L );

void fsRegisterTranslator( CFileTranslator *trans );
bool fsIsTranslator( void *objptr );
void fsCleanupTranslator( CFileTranslator *trans );
void fsShutdownTranslators( void );
CFileTranslator* fsLuaGetTranslator( lua_State *L, int idx );

#endif //_FILESYSTEMLIB_

/*****************************************************************************
*
*  PROJECT:     Lua Interpreter
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        luafilesystem.h
*  PURPOSE:     Lua filesystem access
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _FILESYSTEMLIB_
#define _FILESYSTEMLIB_

#define LUACLASS_FILETRANSLATOR     137

int luafsys_createArchiveTranslator( lua_State *L );
int luafsys_createZIPArchive( lua_State *L );
void luafsys_pushroot( lua_State *L, CFileTranslator *root );
void luafilesystem_open( lua_State *L );

#endif //_FILESYSTEMLIB_

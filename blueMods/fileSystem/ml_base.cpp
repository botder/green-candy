/*********************************************************
*
*  Multi Theft Auto: San Andreas - Deathmatch
*
*  ml_base, External lua add-on module
*
*  Copyright © 2003-2008 MTA.  All Rights Reserved.
*
*  Grand Theft Auto is © 2002-2003 Rockstar North
*
*  THE FOLLOWING SOURCES ARE PART OF THE MULTI THEFT
*  AUTO SOFTWARE DEVELOPMENT KIT AND ARE RELEASED AS
*  OPEN SOURCE FILES. THESE FILES MAY BE USED AS LONG
*  AS THE DEVELOPER AGREES TO THE LICENSE THAT IS
*  PROVIDED WITH THIS PACKAGE.
*
*********************************************************/

#include "StdInc.h"

#include <cstring>

#ifndef __linux__
#include <NativeExecutive/CExecutiveManager.h>
#endif //__linux__

ILuaModuleManager10 *pModuleManager = nullptr;
CFileSystemInterface *pubFileSystem = nullptr;
#ifndef __linux__
NativeExecutive::CExecutiveManager *natExecMan = nullptr;
#endif //__linux__

bool _global_doBufferAllRaw;

// Initialisation function (module entrypoint)
MTAEXPORT bool InitModule ( ILuaModuleManager10 *pManager, char *szModuleName, char *szAuthor, float *fVersion )
{
    pModuleManager = pManager;
#ifndef __linux__
    natExecMan = NativeExecutive::CExecutiveManager::Create();
#endif //__linux__
    try
    {
        fs_construction_params fsparams;
#ifndef __linux__
        fsparams.nativeExecMan = natExecMan;
#endif //__linux__
        // TODO: anything left to configure for the Eir FileSystem module here?

        pubFileSystem = CFileSystem::Create( fsparams );

        // Allow per-resource switching of the global file buffering.
        _global_doBufferAllRaw = pubFileSystem->GetDoBufferAllRaw();

        pubFileSystem->SetDoBufferAllRaw( false );
    }
    catch( ... )
    {
#ifndef __linux__
        NativeExecutive::CExecutiveManager::Delete( natExecMan );
#endif //__linux__
        return false;
    }

    // Set the module info
    strncpy ( szModuleName, MODULE_NAME, MAX_INFO_LENGTH );
    strncpy ( szAuthor, MODULE_AUTHOR, MAX_INFO_LENGTH );
    (*fVersion) = (float)MODULE_VERSION;

    return true;
}

static int lua_createFileInterface( lua_State *L )
{
    luafilesystem_open( L );
    return 1;
}


MTAEXPORT void RegisterFunctions ( lua_State * luaVM )
{
    pModuleManager->RegisterFunction( luaVM, "createFilesystemInterface", lua_createFileInterface );
}


MTAEXPORT bool DoPulse ( void )
{
    return true;
}

MTAEXPORT bool ShutdownModule ( void )
{
    // Clean-up any remaining handles.
    fsShutdownTranslators();
    fsShutdownFiles();

    if ( pubFileSystem != nullptr )
    {
        CFileSystem::Destroy( (CFileSystem*)pubFileSystem );
    }
#ifndef __linux__
    if ( natExecMan != nullptr )
    {
        NativeExecutive::CExecutiveManager::Delete( natExecMan );
    }
#endif //__linux__
    return true;
}

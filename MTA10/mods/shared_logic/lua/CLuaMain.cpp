/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua main
*  DEVELOPERS:  Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Derek Abdine <>
*               Jax <>
*               Ed Lyons <eai@opencoding.net>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CLuaMain
#include "profiler/SharedUtil.Profiler.h"

using std::list;

extern CClientGame* g_pClientGame;

static CLuaManager* m_pLuaManager;

#define HOOK_INSTRUCTION_COUNT 1000000
#define HOOK_MAXIMUM_TIME 5000


// This script is loaded into all VM's created.
const char szPreloadedScript [] = ""\

    // Code for allowing this syntax:   exports.resourceName:exportedFunctionName (...)
    //                                  exports["resourceName"]:exportedFunctionName (...)
    //                                  exports[resourcePointer]:exportedFunctionName (...)
    // Aswell as the old:               call ( getResourceFromName ( "resourceName" ), "exportedFunctionName", ... )
    "local rescallMT = {}\n" \
    "function rescallMT:__index(k)\n" \
    "        if type(k) ~= 'string' then k = tostring(k) end\n" \
    "        self[k] = function(resExportTable, ...)\n" \
    "                if type(self.res) == 'resource' and getResourceRootElement(self.res) then\n" \
    "                        return call(self.res, k, ...)\n" \
    "                else\n" \
    "                        return nil\n" \
    "                end\n" \
    "        end\n" \
    "        return self[k]\n" \
    "end\n" \
    "local exportsMT = {}\n" \
    "function exportsMT:__index(k)\n" \
    "        if type(k) == 'resource' and getResourceRootElement(k) then\n" \
    "                return setmetatable({ res = k }, rescallMT)\n" \
    "        elseif type(k) ~= 'string' then\n" \
    "                k = tostring(k)\n" \
    "        end\n" \
    "        local res = getResourceFromName(k)\n" \
    "        if res then\n" \
    "                return setmetatable({ res = res }, rescallMT)\n" \
    "        else\n" \
    "                outputDebugString('exports: Call to non-running client resource (' .. k .. ')', 1)\n" \
    "                return setmetatable({}, rescallMT)\n" \
    "        end\n" \
    "end\n" \
    "exports = setmetatable({}, exportsMT)\n";


CLuaMain::CLuaMain( CLuaManager& manager, CFileTranslator& fileRoot ) : LuaMain( manager ), m_fileRoot( fileRoot )
{
    CClientPerfStatLuaMemory::GetSingleton()->OnLuaMainCreate( this );
    CClientPerfStatLuaTiming::GetSingleton()->OnLuaMainCreate( this );
}

CLuaMain::~CLuaMain()
{
    CClientPerfStatLuaMemory::GetSingleton()->OnLuaMainDestroy( this );
    CClientPerfStatLuaTiming::GetSingleton()->OnLuaMainDestroy( this );
}

void CLuaMain::InitVM( int structure, int meta )
{
    LuaMain::InitVM( structure, meta );

    // Update global variables
    g_pClientGame->GetRootEntity()->PushStack( m_lua );
    lua_setglobal( m_lua, "root" );

    g_pClientGame->GetLocalPlayer()->PushStack( m_lua );
    lua_setglobal( m_lua, "localPlayer" );

    g_pClientGame->GetManager()->GetCamera()->PushStack( m_lua );
    lua_setglobal( m_lua, "camera" );

    // Load pre-loaded lua code
    LoadScript( szPreloadedScript );
}

CXMLFile* CLuaMain::CreateXML( const char *path )
{
    CXMLFile *file = g_pCore->GetXML()->CreateXML( path, true );

    if ( !file )
        return NULL;

    m_XMLFiles.push_back( file );
    return file;
}

void CLuaMain::SaveXML( CXMLNode *root )
{
    xmlFiles_t::iterator iterFile;

    for ( iterFile = m_XMLFiles.begin(); iterFile != m_XMLFiles.end(); iterFile++ )
    {
        CXMLFile *file = *iterFile;

        if ( file->GetRootNode() == root )
        {
            // Create the directory if it does not exist
            resFileRoot->CreateDir( file->GetFilename() );

            file->Write();
            break;
        }
    }

    CResource::configList_t::iterator iter = GetResource()->ConfigIterBegin();

    for ( ; iter != GetResource()->ConfigIterEnd(); iter++ )
    {
        CResourceConfigItem* cfg = *iter;

        if ( cfg->GetRoot() != root )
            continue;

        CXMLFile *file = cfg->GetFile();

        if ( !file )
            continue;

        // Directory has to exist
        resFileRoot->CreateDir( file->GetFilename() );

        file->Write();
        break;
    }
}

bool CLuaMain::ParseRelative( const char *in, filePath& out ) const
{
    return resFileRoot->GetRelativePath( in, true, out );
}

unsigned long CLuaMain::GetElementCount()
{
    if ( !m_resource || !GetResource()->GetElementGroup() ) 
        return 0;

    return GetResource()->GetElementGroup ()->GetCount ();
}
/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaMain.cpp
*  PURPOSE:     Lua virtual machine container class
*  DEVELOPERS:  Oliver Brown <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <>
*               Cecill Etheredge <>
*               Jax <>
*               Kevin Whiteside <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

#ifndef WIN32
#include <clocale>
#endif

static CLuaManager* m_pLuaManager;
extern CGame* g_pGame;

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
    "                if type(self.res) == 'userdata' and getResourceRootElement(self.res) then\n" \
    "                        return call(self.res, k, ...)\n" \
    "                else\n" \
    "                        return nil\n" \
    "                end\n" \
    "        end\n" \
    "        return self[k]\n" \
    "end\n" \
    "local exportsMT = {}\n" \
    "function exportsMT:__index(k)\n" \
    "        if type(k) == 'userdata' and getResourceRootElement(k) then\n" \
    "                return setmetatable({ res = k }, rescallMT)\n" \
    "        elseif type(k) ~= 'string' then\n" \
    "                k = tostring(k)\n" \
    "        end\n" \
    "        local res = getResourceFromName(k)\n" \
    "        if res and getResourceRootElement(res) then\n" \
    "                return setmetatable({ res = res }, rescallMT)\n" \
    "        else\n" \
    "                outputDebugString('exports: Call to non-running server resource (' .. k .. ')', 1)\n" \
    "                return setmetatable({}, rescallMT)\n" \
    "        end\n" \
    "end\n" \
    "exports = setmetatable({}, exportsMT)\n";

CLuaMain::CLuaMain( CLuaManager& luaManager,
                    CObjectManager *objectManager,
                    CPlayerManager *playerManager,
                    CVehicleManager *vehicleManager,
                    CBlipManager *blipManager,
                    CRadarAreaManager *radarAreaManager,
                    CMapManager *mapManager,
                    CResource *resource ) : LuaMain( luaManager )
{
    // Initialise everything to be setup in the Start function
    m_resource = resource;
    m_resourceFile = NULL;

    m_objectManager = objectManager;
    m_playerManager = playerManager;
    m_radarAreaManager = radarAreaManager;
    m_vehicleManager = vehicleManager;
    m_blipManager = blipManager;
    m_mapManager = mapManager;

    CPerfStatLuaMemory::GetSingleton()->OnLuaMainCreate( this );
    CPerfStatLuaTiming::GetSingleton()->OnLuaMainCreate( this );
}

CLuaMain::~CLuaMain()
{
    // remove all current remote calls originating from this VM
    g_pGame->GetRemoteCalls()->Remove( this );

    // Delete all keybinds
    std::list <CPlayer*> ::const_iterator iter = m_playerManager->IterBegin();
    for ( ; iter != m_playerManager->IterEnd(); iter++ )
    {
        if ( (*iter)->IsJoined() )
            (*iter)->GetKeyBinds()->RemoveAllKeys( this );
    }

    // Clean up scripted elements
    std::list <CXMLFile*>::iterator iterXML = m_XMLFiles.begin();
    for ( ; iterXML != m_XMLFiles.end(); iterXML++ )
        delete *iterXML;

    std::list <CTextDisplay*>::iterator iterDisplays = m_Displays.begin();
    for ( ; iterDisplays != m_displays.end(); iterDisplays++ )
        delete *iterDisplays;

    std::list <CTextItem*>::iterator iterItems = m_TextItems.begin();
    for ( ; iterItems != m_textItems.end(); iterItems++ )
        delete *iterItems;

    CPerfStatLuaMemory::GetSingleton()->OnLuaMainDestroy( this );
    CPerfStatLuaTiming::GetSingleton()->OnLuaMainDestroy( this );
}

void CLuaMain::InitVM( int structure, int meta )
{
    // Registering C functions
    CLuaCFunctions::RegisterFunctionsWithVM( m_lua );

    // Oli: Don't forget to add new ones to CLuaManager::LoadCFunctions. Thanks!

    // create global vars
    lua_pushelement( m_lua, g_pGame->GetMapManager()->GetRootElement() );
    lua_setglobal( m_lua, "root" );

    lua_pushresource( m_lua, m_pResource );
    lua_setglobal( m_lua, "resource" );

    lua_pushelement( m_lua, m_pResource->GetResourceRootElement () );
    lua_setglobal( m_lua, "resourceRoot" );

    // Load pre-loaded lua code
    LoadScript( szPreloadedScript );
}

// Special function(s) that are only visible to HTMLD scripts
void CLuaMain::RegisterHTMLDFunctions()
{
    CLuaHTTPDefs::LoadFunctions( m_lua );
}

CXMLFile* CLuaMain::CreateXML( const char *filename )
{
    CXMLFile *file = g_pServerInterface->GetXML()->CreateXML( szFilename, true );

    if ( !file )
        return NULL;

    m_XMLFiles.push_back( file );
    return file;
}

void CLuaMain::SaveXML( CXMLNode *root )
{
    for ( xmlFiles_t::iterator iter = m_XMLFiles.begin(); iter != m_XMLFiles.end(); iter++ )
    {
        CXMLFile *file = *iter;

        if ( file->GetRootNode() == root )
        {
            // Create the directory if it does not exist
            resFileRoot->CreateDir( file->GetFilename() );

            file->Write();
            break;
        }
    }

    CResource::fileList_t::iterator iter = GetResource()->IterBegin();

    for ( ; iter != GetResource()->IterEnd(); iter++ )
    {
        CResourceFile *file = *iter;

        if ( file->GetType() != CResourceFile::RESOURCE_FILE_TYPE_CONFIG )
            continue;

        CResourceConfigItem *cnfItem = (CResourceConfigItem*)file;

        if ( cnfItem->GetRoot() != root )
            continue;

        CXMLFile *xmlFile = cnfItem->GetFile():

        if ( xmlFile )
        {
            // Directory has to exist
            resFileRoot->CreateDir( file->GetPath().c_str() );

            xmlFile->Write();
        }
        break;
    }
}

CTextDisplay* CLuaMain::CreateDisplay()
{
    CTextDisplay *display = new CTextDisplay;

    m_displays.push_back( display );
    return display;
}

void CLuaMain::DestroyDisplay( CTextDisplay *display )
{
    m_displays.remove( display );
    delete display;
}

CTextItem* CLuaMain::CreateTextItem( const char *text, float x, float y, eTextPriority priority, const SColor color, float scale, unsigned char format, unsigned char shadowAlpha )
{
    CTextItem *item = new CTextItem( text, CVector2D( x, y ), priority, color, scale, format, shadowAlpha );
    m_TextItems.push_back( textItem );
    return textItem;
}

void CLuaMain::DestroyTextItem( CTextItem *item )
{
    m_textItems.remove( item );
    delete item;
}

bool CLuaMain::TextDisplayExists( CTextDisplay *display )
{
    std::list <CTextDisplay*>::const_iterator iter = m_displays.begin();

    for ( ; iter != m_displays.end(); iter++ )
    {
        if ( *iter == display )
            return true;
    }

    return false;
}

bool CLuaMain::TextItemExists( CTextItem *item )
{
    std::list <CTextItem*>::const_iterator iter = m_textItems.begin();

    for ( ; iter != m_textItems.end(); iter++ )
    {
        if ( *iter == item )
            return true;
    }

    return false;
}
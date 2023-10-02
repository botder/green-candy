/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLocalServer.cpp
*  PURPOSE:     Local server setup GUI
*  DEVELOPERS:  Stanislav Bobrov <lil_toady@hotmail.com>
*               The_GTA <quiret@gmx.de>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using namespace LocalServer;

extern CCoreInterface* g_pCore;
extern CClientGame* g_pClientGame;

// SResInfo - Item in list of potential resources - Used in GetResourceNameList()
struct SResInfo
{
    filePath absPath;
    filePath name;
    bool isDir;
    bool pathIssue;
};


CLocalServer::CLocalServer ( const char* szConfig )
{
    m_strConfig = szConfig;
    m_pConfig = NULL;

    m_pGUI = g_pCore->GetGUI();

    m_pWindow = reinterpret_cast < CGUIWindow* > ( m_pGUI->CreateWnd ( NULL, "HOST GAME" ) );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = m_pGUI->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 360.0f / 2, resolution.fY / 2 - 440.0f / 2 + yoff  ), false );
    m_pWindow->SetSize ( CVector2D ( 360.0f, 440.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetVisible ( true );

    m_pTabs = reinterpret_cast < CGUITabPanel* > ( m_pGUI->CreateTabPanel ( m_pWindow ) );
    m_pTabs->SetPosition ( CVector2D ( 0.0f, 0.06f ), true );
    m_pTabs->SetSize ( CVector2D ( 1.0f, 0.85f ), true );
    m_pTabGeneral = m_pTabs->CreateTab ( "General" );
    //m_pTabs->CreateTab ( "Gamemode" );
    m_pTabResources = m_pTabs->CreateTab ( "Resources" );

    m_pLabelName = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Server name:" ) );
    m_pLabelName->SetPosition ( CVector2D ( 0.03f, 0.07f ), true );
    m_pLabelName->AutoSize ( "Server name:" );

    m_pEditName = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral, "Default MTA Server" ) );
    m_pEditName->SetPosition ( CVector2D ( 0.4f, 0.06f ), true );
    m_pEditName->SetSize ( CVector2D ( 0.57f, 0.06f ), true );
    m_pEditName->SetMaxLength ( 64 );

    m_pLabelPass = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Password:" ) );
    m_pLabelPass->SetPosition ( CVector2D ( 0.03f, 0.14f ), true );
    m_pLabelPass->AutoSize ( "Password:" );

    m_pEditPass = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral ) );
    m_pEditPass->SetPosition ( CVector2D ( 0.4f, 0.13f ), true );
    m_pEditPass->SetSize ( CVector2D ( 0.57f, 0.06f ), true );
    m_pEditPass->SetMaxLength ( 8 );

    m_pLabelPlayers = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Max players:" ) );
    m_pLabelPlayers->SetPosition ( CVector2D ( 0.03f, 0.21f ), true );
    m_pLabelPlayers->AutoSize ( "Max players:" );

    m_pEditPlayers = reinterpret_cast < CGUIEdit* > ( m_pGUI->CreateEdit ( m_pTabGeneral ) );
    m_pEditPlayers->SetPosition ( CVector2D ( 0.4f, 0.20f ), true );
    m_pEditPlayers->SetSize ( CVector2D ( 0.17f, 0.06f ), true );
    m_pEditPlayers->SetMaxLength ( 3 );

    m_pLabelBroadcast = reinterpret_cast < CGUILabel* > ( m_pGUI->CreateLabel ( m_pTabGeneral, "Broadcast:" ) );
    m_pLabelBroadcast->SetPosition ( CVector2D ( 0.03f, 0.35f ), true );
    m_pLabelBroadcast->AutoSize ( "Broadcast:" );

    m_pBroadcastLan = reinterpret_cast < CGUICheckBox* > ( m_pGUI->CreateCheckBox ( m_pTabGeneral, "LAN", true ) );
    m_pBroadcastLan->SetPosition ( CVector2D ( 0.4f, 0.33f ), true );
    m_pBroadcastLan->SetSize ( CVector2D ( 0.45f, 0.08f ), true );

    m_pBroadcastInternet = reinterpret_cast < CGUICheckBox* > ( m_pGUI->CreateCheckBox ( m_pTabGeneral, "Internet", true ) );
    m_pBroadcastInternet->SetPosition ( CVector2D ( 0.4f, 0.38f ), true );
    m_pBroadcastInternet->SetSize ( CVector2D ( 0.45f, 0.08f ), true );

    m_pResourcesCur = reinterpret_cast < CGUIGridList* > ( m_pGUI->CreateGridList ( m_pTabResources, false ) );
    m_pResourcesCur->SetPosition ( CVector2D ( 0.03f, 0.06f ), true );
    m_pResourcesCur->SetSize ( CVector2D ( 0.45f, 0.5f ), true );
    m_pResourcesCur->SetSorting ( false );
    m_pResourcesCur->SetSelectionMode ( SelectionModes::CellSingle );
    m_hResourcesCur = m_pResourcesCur->AddColumn ( "Selected", 0.80f );

    m_pResourceDel = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pTabResources, ">" ) );
    m_pResourceDel->SetPosition ( CVector2D ( 0.03f, 0.65f ), true );
    m_pResourceDel->SetSize ( CVector2D ( 0.45f, 0.05f ), true );
    m_pResourceDel->SetZOrderingEnabled ( false );

    m_pResourceAdd = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pTabResources, "<" ) );
    m_pResourceAdd->SetPosition ( CVector2D ( 0.03f, 0.58f ), true );
    m_pResourceAdd->SetSize ( CVector2D ( 0.45f, 0.05f ), true );
    m_pResourceAdd->SetZOrderingEnabled ( false );

    m_pResourcesAll = reinterpret_cast < CGUIGridList* > ( m_pGUI->CreateGridList ( m_pTabResources, false ) );
    m_pResourcesAll->SetPosition ( CVector2D ( 0.52f, 0.06f ), true );
    m_pResourcesAll->SetSize ( CVector2D ( 0.45f, 0.9f ), true );
    m_pResourcesAll->SetSorting ( false );
    m_pResourcesAll->SetSelectionMode ( SelectionModes::CellSingle );
    m_hResourcesAll = m_pResourcesAll->AddColumn ( "All", 0.80f );

    m_pButtonStart = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pWindow, "Start" ) );
    m_pButtonStart->SetPosition ( CVector2D ( 0.33f, 0.93f ), true );
    m_pButtonStart->SetSize ( CVector2D ( 0.3f, 0.05f ), true );
    m_pButtonStart->SetZOrderingEnabled ( false );

    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( m_pGUI->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 0.65f, 0.93f ), true );
    m_pButtonCancel->SetSize ( CVector2D ( 0.3f, 0.05f ), true );
    m_pButtonCancel->SetZOrderingEnabled ( false );

    m_pResourceAdd->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnAddButtonClick, this ) );
    m_pResourceDel->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnDelButtonClick, this ) );
    m_pButtonStart->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnStartButtonClick, this ) );
    m_pButtonCancel->SetClickHandler ( GUI_CALLBACK ( &CLocalServer::OnCancelButtonClick, this ) );

    Load();
}


CLocalServer::~CLocalServer ( void )
{
    if ( m_pConfig )
        delete m_pConfig;
    delete m_pWindow;
}


bool CLocalServer::OnAddButtonClick ( CGUIElement *pElement )
{
    CGUIListItem *pItem = m_pResourcesAll->GetSelectedItem ();
    if ( pItem )
    {
        m_pResourcesCur->SetItemText ( m_pResourcesCur->AddRow (), m_hResourcesCur, &pItem->GetText().c_str()[3] );
        m_pResourcesAll->RemoveRow ( m_pResourcesAll->GetItemRowIndex ( pItem ) );
    }
    return true;
}


bool CLocalServer::OnDelButtonClick ( CGUIElement *pElement )
{
    CGUIListItem *pItem = m_pResourcesCur->GetSelectedItem ();
    if ( pItem )
    {
        m_pResourcesAll->SetItemText ( m_pResourcesAll->AddRow (), m_hResourcesAll, &pItem->GetText().c_str()[3] );
        m_pResourcesCur->RemoveRow ( m_pResourcesCur->GetItemRowIndex ( pItem ) );
    }
    return true;
}


bool CLocalServer::OnStartButtonClick ( CGUIElement *pElement )
{
    Save();
    // Connect

    g_pClientGame->StartLocalGame ( m_strConfig.c_str(), m_pEditPass->GetText().c_str() );

    return true;
}


bool CLocalServer::OnCancelButtonClick ( CGUIElement *pElement )
{
    g_pCore->GetModManager ()->RequestUnload ();
    return true;
}

bool CLocalServer::Load()
{
    filePath path;
    g_pCore->GetServer()->GetFileRoot().GetFullPathFromRoot( "server/mods/deathmatch/", false, path );

    path += m_strConfig;

    m_pConfig = g_pCore->GetXML()->CreateXML( path.c_str() );
    if ( m_pConfig && m_pConfig->Parse() )
    {
        CXMLNode* pRoot = m_pConfig->GetRootNode();
        CXMLNode* pServerName = pRoot->FindSubNode ( "servername", 0 );
        if ( pServerName ) m_pEditName->SetText ( pServerName->GetTagContent() );
        CXMLNode* pServerPass = pRoot->FindSubNode ( "password", 0 );
        if ( pServerPass ) m_pEditPass->SetText ( pServerPass->GetTagContent() );
        CXMLNode* pServerPlayers = pRoot->FindSubNode ( "maxplayers", 0 );
        if ( pServerPlayers ) m_pEditPlayers->SetText ( pServerPlayers->GetTagContent() );

        // Read the startup resources
        list < CXMLNode* > ::const_iterator iter = pRoot->ChildrenBegin ();
        for ( ; iter != pRoot->ChildrenEnd (); ++iter )
        {
            CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( *iter );
            if ( pNode->GetTagName ().compare ( "resource" ) == 0 )
            {
                CXMLAttribute* src = pNode->GetAttributes().Find ( "src" );
                if ( src && src->GetValue()[1] )
                {
                    m_pResourcesCur->SetItemText ( m_pResourcesCur->AddRow (), m_hResourcesCur, src->GetValue().c_str() );
                }
            }
        }
    }

    // Get list of resource names
    resNameList_t resourceNameList;
    GetResourceNameList( resourceNameList, path );

    // Put resource names into the GUI
    for ( resNameList_t::iterator iter = resourceNameList.begin(); iter != resourceNameList.end(); ++iter )
        HandleResource( (*iter).c_str() );

    return true;
}


//
// Scan resource directories
//
typedef std::map <filePath, SResInfo> resInfo_t;

struct _resscan
{
    const CFileTranslator *resRoot;
    resInfo_t map;
    size_t rootLen;
};

static inline bool resname_legal( const filePath& name )
{
    size_t n;

    for ( n = 0; n < name.size(); n++ )
    {
        char charcode = name.at( n );

        switch( charcode )
        {
        case ' ':
        case '.':
            return false;
        }
    }

    return true;
}

static void _resscan_file( const filePath& path, void *ud )
{
    if ( path.at( 0 ) == '.' )
        return;

    struct _resscan& scan = *(struct _resscan*)ud;
    filePath name = path.substr( scan.rootLen, path.size() - 4 );

    if ( !resname_legal( name ) )
        return;

    filePath absPath = filePath( path );
    absPath.resize( path.size() - name.size() - 4 );

    if ( SResInfo *dup = MapFind( scan.map, name ) )
    {
        if ( dup->absPath != absPath )
            dup->pathIssue = true;

        return;
    }

    SResInfo info;
    info.name = name;
    info.absPath = absPath;
    info.isDir = false;
    info.pathIssue = false;
    MapSet( scan.map, name, info );
}

static void _resscan_dir( const filePath& path, void *ud )
{
    struct _resscan& scan = *(struct _resscan*)ud;
    const char c = path.at( 0 );

    if ( c == '.' )
        return;

    if ( c == '#' || c == '[' && path.at( path.size() - 1 ) == ']' )
    {
        // Optimization for our scan
        size_t rl = scan.rootLen;
        scan.rootLen = path.size();

        scan.resRoot->ScanDirectory( path.c_str(), "*.zip", false, _resscan_dir, _resscan_file, ud );

        scan.rootLen = rl;
        return;
    }

    filePath name = filePath( path ).substr( scan.rootLen, path.size() - 1 );

    if ( !resname_legal( name ) )
        return;

    filePath metaPath = path;
    metaPath += "meta.xml";

    if ( !scan.resRoot->Exists( metaPath.c_str() ) )
        return;

    if ( SResInfo *dup = SharedUtil::MapFind( scan.map, name ) )
    {
        if ( dup->absPath == path )
            dup->isDir = true;
        else
            dup->pathIssue = true;

        return;
    }

    SResInfo info;
    info.name = name;
    info.absPath = path;
    info.isDir = true;
    info.pathIssue = false;
    SharedUtil::MapSet( scan.map, name, info );
}

void CLocalServer::GetResourceNameList( resNameList_t& outResourceNameList, const filePath& strModPath )
{
    // Make list of potential active resources
    _resscan scan;
    scan.resRoot = &g_pCore->GetServer()->GetFileRoot();
    scan.rootLen = strModPath.size();

    scan.resRoot->ScanDirectory( strModPath, "*.zip", false, _resscan_dir, _resscan_file, &scan );

    // Print important errors
    for ( resInfo_t::const_iterator iter = scan.map.begin(); iter != scan.map.end(); ++iter )
    {
        const SResInfo& info = iter->second;

        if ( info.pathIssue )
        {
            CLogger::ErrorPrintf( "Not processing resource '%s' as it has duplicates on different paths\n", info.name.c_str() );
            continue;
        }

        outResourceNameList.push_back( info.name );
    }
}

bool CLocalServer::Save()
{
    if ( m_pConfig && m_pConfig->GetRootNode() )
    {
        StoreConfigValue ( "servername", ( std::string( m_pEditName->GetText() ).length() > 0 ) ? m_pEditName->GetText().c_str() : "MTA Local Server" );
        StoreConfigValue ( "maxplayers", ( atoi ( m_pEditPlayers->GetText().c_str() ) ) ? m_pEditPlayers->GetText().c_str() : "32" );
        StoreConfigValue ( "donotbroadcastlan", ( m_pBroadcastLan->IsActive () ) ? "0" : "1" );
        StoreConfigValue ( "ase", ( m_pBroadcastInternet->IsActive () ) ? "1" : "0" );
        StoreConfigValue ( "password", m_pEditPass->GetText().c_str() );

        // Remove old resources from the config
        CXMLNode* pRoot = m_pConfig->GetRootNode();
        list < CXMLNode* > ::const_iterator iter = pRoot->ChildrenBegin ();
        for ( ; iter != pRoot->ChildrenEnd (); ++iter )
        {
            CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( *iter );
            if ( pNode->GetTagName().compare ( "resource" ) == 0 )
            {
                pRoot->DeleteSubNode ( pNode );
                iter = pRoot->ChildrenBegin ();
            }
        }

        // Add new resources to the config
        for ( int i = 0; i < m_pResourcesCur->GetRowCount(); i++ )
        {
            CXMLNode* pResourceNode = pRoot->CreateSubNode ( "resource" );
            pResourceNode->GetAttributes().Create ( "src" )->SetValue ( m_pResourcesCur->GetItemText ( i, 1 ) );
            pResourceNode->GetAttributes().Create ( "startup" )->SetValue ( "1" );
            pResourceNode->GetAttributes().Create ( "protected" )->SetValue ( "0" );
        }
        m_pConfig->Write ();
    }
    return true;
}


void CLocalServer::StoreConfigValue ( const char* szNode, const char* szValue )
{
    CXMLNode* pRoot = m_pConfig->GetRootNode();
    CXMLNode* pNode = pRoot->FindSubNode ( szNode, 0 );
    if ( pNode )
    {
        pNode->SetTagContent ( szValue );
    }
    else
    {
        pNode = pRoot->CreateSubNode ( szNode );
        pNode->SetTagContent ( szValue );
    }
}

void CLocalServer::HandleResource( const char *szResource )
{
    for ( int i = 0; i < m_pResourcesCur->GetRowCount(); i++ )
    {
        if ( strcmp ( szResource, m_pResourcesCur->GetItemText ( i, 1 ) ) == 0 )
            return;
    }
    m_pResourcesAll->SetItemText ( m_pResourcesAll->AddRow (), m_hResourcesAll, szResource );
}

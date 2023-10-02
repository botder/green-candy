/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/ClientCommands.cpp
*  PURPOSE:     Client commands handler class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::vector;

using namespace CommandlineTools;

#ifdef MTA_DEBUG
#include <Tlhelp32.h>
#include <Psapi.h>
#include <shlwapi.h>
#include <Utils.h>
#endif

// Hide the "conversion from 'unsigned long' to 'DWORD*' of greater size" warning
#pragma warning(disable:4312)

extern CClientGame* g_pClientGame;

static inline void luaexecute( const char *name, const char *cmdArgs )
{
    if ( g_pClientGame )
    {
        if ( Command *cmd = g_pClientGame->GetRegisteredCommands()->Get( name ) )
        {
            std::vector <std::string> args;

            if ( cmdArgs )
                strsplit( cmdArgs, args );

            cmd->Execute( args );
        }
    }
}

bool COMMAND_Executed ( const char* szCommand, const char* szArguments, bool bHandleRemotely, bool bHandled )
{
    // Has the core already handled this command?
    if ( !bHandled )
    {
        //char szBuffer [256];
        const char* szCommandBufferPointer = szCommand;

        if ( !bHandleRemotely )
        {
            // Is the command "say" and the arguments start with '/' ? (command comes from the chatbox)
            if ( stricmp ( szCommand, "chatboxsay" ) == 0 )
                szCommandBufferPointer = "say";
        }

        // Toss them together so we can send it to the server
        SString strClumpedCommand;
        if ( szArguments && szArguments [ 0 ] )
            strClumpedCommand.Format ( "%s %s", szCommandBufferPointer, szArguments );
        else
            strClumpedCommand = szCommandBufferPointer;

        // Convert to Unicode, and clamp it to a maximum command length
        std::wstring strClumpedCommandUTF = MbUTF8ToUTF16(strClumpedCommand.c_str());
        strClumpedCommandUTF = strClumpedCommandUTF.substr(0,MAX_COMMAND_LENGTH);
        strClumpedCommand = UTF16ToMbUTF8(strClumpedCommandUTF);

        luaexecute( szCommandBufferPointer, szArguments );

        // Call the event on the local player's onClientConsole first
        if ( g_pClientGame )
        {
            if ( g_pClientGame->GetLocalPlayer() )
            {
                lua_State *L = g_pClientGame->GetLuaManager()->GetVirtualMachine();
                lua_pushlstring( L, strClumpedCommand.c_str(), strClumpedCommand.size() );
                g_pClientGame->GetLocalPlayer()->CallEvent( "onClientConsole", L, 1 );
            }
        }

        // Write the chatlength and the content
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
        if ( !pBitStream ) 
            return false;

        // Write it to the bitstream
        pBitStream->Write ( strClumpedCommand.c_str(), static_cast < int > ( strlen ( strClumpedCommand.c_str() ) ) );

        // Send the packet to the server and free it
        g_pNet->SendPacket ( PACKET_ID_COMMAND, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_RELIABLE, PACKET_ORDERING_CHAT );
        g_pNet->DeallocateNetBitStream ( pBitStream );

        return true;
    }
    else
    {
        // Call our comand-handlers for core-executed commands too
        luaexecute( szCommand, szArguments );
    }
    return false;
}


void COMMAND_ChatBox ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    // Split it up into command and rgb
    char* szCommand = strtok ( const_cast < char* > ( szCmdLine ), " " );
    char* szRed = strtok ( NULL, " " );
    char* szGreen = strtok ( NULL, " " );
    char* szBlue = strtok ( NULL, " " );
    unsigned char ucRed = 0, ucGreen = 0, ucBlue = 0;

    if ( !szCommand )
        return;

    if ( szRed && szGreen && szBlue )
    {     
        ucRed = static_cast < unsigned char > ( atoi ( szRed ) );
        ucGreen = static_cast < unsigned char > ( atoi ( szGreen ) );
        ucBlue = static_cast < unsigned char > ( atoi ( szBlue ) );
    }

    // Open the chatbox input with command and color
    g_pCore->EnableChatInput ( szCommand, COLOR_ARGB ( 255, ucRed, ucGreen, ucBlue ) );
}


void COMMAND_Help ( const char *szCmdLine )
{
    // This isnt used
}


void COMMAND_Disconnect ( const char *szCmdLine )
{
    g_pCore->GetModManager ()->RequestUnload ();
}

void COMMAND_ShowNametags ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pCore->IsChatVisible ();
    g_pClientGame->GetNametags ()->SetVisible ( bShow );
}

void COMMAND_ShowChat ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pCore->IsChatVisible ();
    g_pCore->SetChatVisible ( bShow );
}

void COMMAND_ShowNetstat ( const char* szCmdLine )
{
    if ( szCmdLine && szCmdLine [ 0 ] )
    {
        g_pClientGame->ShowNetstat ( atoi ( szCmdLine ) == 1 );
    }
}

void COMMAND_Eaeg ( const char* szCmdLine )
{
    g_pClientGame->ShowEaeg ( true );
}

void COMMAND_EnterPassenger ( const char* szCmdLine )
{
    // HACK: we don't want them to enter a vehicle if they're in cursor mode
    if ( g_pClientGame && !g_pCore->IsCursorForcedVisible() )
    {
        // Disable passenger entry while reloading so it doesen't abort the animation
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();
        if ( pPlayer )
        {
            CWeapon* pWeapon = pPlayer->GetWeapon ();
            if ( pWeapon )
            {
                if ( pWeapon->GetState() != WEAPONSTATE_RELOADING )
                {
                    g_pClientGame->ProcessVehicleInOutKey ( true );
                }
            }
        }
    }
}


void COMMAND_RadioNext ( const char* szCmdLine )
{
    if ( g_pClientGame )
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();
        if ( pPlayer )
        {
            pPlayer->NextRadioChannel ();
        }
    }
}


void COMMAND_RadioPrevious ( const char* szCmdLine )
{
    if ( g_pClientGame )
    {
        CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->GetLocalPlayer ();
        if ( pPlayer )
        {
            pPlayer->PreviousRadioChannel ();
        }
    }
}

/*
void COMMAND_Screenshot ( const char* szCmdLine )
{
    g_pClientGame->GetScreenshot ()->SetScreenshotKey ( true );
}
*/

void COMMAND_RadarMap ( const char* szCmdLine )
{
    int iCmd = ( szCmdLine && szCmdLine [ 0 ] ) ? atoi ( szCmdLine ) : -1;
    bool bShow = ( iCmd == 1 ) ? true : ( iCmd == 0 ) ? false : !g_pClientGame->GetRadarMap ()->GetRadarEnabled ();
    g_pClientGame->GetRadarMap ()->SetRadarEnabled ( bShow );
}

void COMMAND_RadarZoomIn ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->ZoomIn ();
    }
}

void COMMAND_RadarZoomOut ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->ZoomOut ();
    }
}

void COMMAND_RadarMoveNorth ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        // Toggle on/off
        if ( pRadarMap->IsMovingNorth () )
            pRadarMap->SetMovingNorth ( false );
        else if ( pRadarMap->IsMovingSouth () )
            pRadarMap->SetMovingSouth ( false );
        else
        {
            pRadarMap->SetMovingNorth ( true );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveSouth ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        // Toggle on/off
        if ( pRadarMap->IsMovingSouth () )
            pRadarMap->SetMovingSouth ( false );
        else if ( pRadarMap->IsMovingNorth () )
            pRadarMap->SetMovingNorth ( false );
        else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( true );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveEast ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        // Toggle on/off
        if ( pRadarMap->IsMovingEast () )
            pRadarMap->SetMovingEast ( false );
        else if ( pRadarMap->IsMovingWest () )
            pRadarMap->SetMovingWest ( false );
        else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( true );
            pRadarMap->SetMovingWest ( false );
        }
    }
}

void COMMAND_RadarMoveWest ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        // Toggle on/off
        if ( pRadarMap->IsMovingWest () )
            pRadarMap->SetMovingWest ( false );
        else if ( pRadarMap->IsMovingEast () )
            pRadarMap->SetMovingEast ( false );
        else
        {
            pRadarMap->SetMovingNorth ( false );
            pRadarMap->SetMovingSouth ( false );
            pRadarMap->SetMovingEast ( false );
            pRadarMap->SetMovingWest ( true );
        }
    }
}

void COMMAND_RadarAttach ( const char* szCmdLine )
{
    CRadarMap* pRadarMap = g_pClientGame->GetRadarMap ();

    if ( pRadarMap->IsRadarShowing () )
    {
        pRadarMap->SetAttachedToLocalPlayer ( !g_pClientGame->GetRadarMap ()->IsAttachedToLocalPlayer () );
    }
}

void COMMAND_RadarTransparency ( const char* szCmdLine )
{
}

void COMMAND_MessageTarget ( const char* szCmdLine )
{
    if ( !(szCmdLine || szCmdLine[0]) )
        return;

    CClientPlayer* pTarget = g_pClientGame->GetTargetedPlayer ();
    if ( pTarget )
    {
        const char * szNick = pTarget->GetNick ();
        if ( !szNick )
            return;
        SString strParameters ( "%s %s", pTarget->GetNick (), szCmdLine );
        g_pCore->GetCommands ()->Execute ( "msg", strParameters );
    }
    else
        g_pCore->ChatEchoColor ( "Error: no player target found", 255, 168, 0 );
}

void COMMAND_VehicleNextWeapon ( const char* szCmdLine )
{
    g_pClientGame->ChangeVehicleWeapon ( true );
}

void COMMAND_VehiclePreviousWeapon ( const char* szCmdLine )
{
    g_pClientGame->ChangeVehicleWeapon ( false );
}

void COMMAND_ResetShaders( const char *params )
{
    g_pGame->ResetShaders();
}

void COMMAND_TextScale ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine [ 0 ]) )
        return;

    float fScale = ( float ) atof ( szCmdLine );
    if ( fScale >= 0.8f && fScale <= 3.0f )
    {
        CClientTextDisplay::SetGlobalScale ( fScale );
        g_pCore->GetCVars ()->Set ( "text_scale", fScale );
    }
}


void DumpPlayer ( CClientPlayer* pPlayer, CFile *file )
{
    return;
}


void COMMAND_DumpPlayers( const char *szCmdLine )
{
    return;
}


void COMMAND_ShowSyncData( const char* szCmdLine )
{
    // Grab the player
    CClientPlayer* pPlayer = g_pClientGame->GetPlayerManager ()->Get ( szCmdLine );

    if ( pPlayer )
        g_pClientGame->GetSyncDebug ()->Attach ( *pPlayer );
    else
        g_pClientGame->GetSyncDebug ()->Detach ();
}

void COMMAND_VoicePushToTalk ( const char* szCmdLine)
{
    if ( g_pClientGame->GetVoiceRecorder()->IsEnabled() )
        g_pClientGame->GetVoiceRecorder()-> UpdatePTTState ( atoi(szCmdLine) );
    else
        g_pCore->GetConsole ()->Print ( "voiceptt: This server does not have voice enabled" );
}

#if defined (MTA_DEBUG) || defined (MTA_BETA)

void COMMAND_ShowSyncing ( const char *szCmdLine )
{
    g_pClientGame->ShowSyncingInfo ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_FakeLag ( const char *szCmdLine )
{
    char szBuffer [256];
    char* szExtraPing = NULL;
    char* szExtraPingVary = NULL;

    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    strncpy ( szBuffer, szCmdLine, 256 );
    szBuffer[255] = 0;
    szExtraPing = strtok ( szBuffer, " " );
    szExtraPingVary = strtok ( NULL, " " );
    
    if ( !(szExtraPing && szExtraPingVary) )
    {
        g_pCore->GetConsole ()->Echo ( "Bad syntax" );
        return;
    }

    g_pNet->SetFakeLag ( 0, atoi ( szExtraPing ), atoi ( szExtraPingVary ) );
    g_pCore->GetConsole ()->Printf ( "Fake lag set to %s extra ping with %s extra ping variance", szExtraPing, szExtraPingVary );
}


#endif

#ifdef MTA_DEBUG

void COMMAND_Foo ( const char* szCmdLine )
{
    g_pClientGame->m_Foo.Test ( szCmdLine );
}

#endif

#if defined(MTA_DEBUG) || defined(MTA_DEBUG_COMMANDS)
void COMMAND_ShowWepdata ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->ShowWepdata ( szCmdLine );
}

void COMMAND_ShowTasks ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->ShowTasks ( szCmdLine );
}

void COMMAND_ShowPlayer ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->ShowPlayer ( szCmdLine );
}

void COMMAND_SetMimic ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetMimic ( atoi ( szCmdLine ) );
}

void COMMAND_SetMimicLag ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetMimicLag ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_Paintballs ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->SetDoPaintballs ( atoi ( szCmdLine ) == 1 );
}

void COMMAND_Breakpoint ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    _asm int 3
    // Make our main pointer easily accessable
    // Added by slush:  You're a lazy ass if you use this.
    g_pClientGame;
}


void COMMAND_GiveWeapon ( const char *szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;

    int nWeaponID = atoi(szCmdLine);
    /*
     * Check validity of the command line weapon id.
     */

    if ( !CClientPickupManager::IsValidWeaponID ( nWeaponID ) )
        return;

    CClientPed * pPed = g_pClientGame->GetManager()->GetPlayerManager()->GetLocalPlayer();
    if ( pPed )
    {
        CWeapon * pPlayerWeapon = pPed->GiveWeapon ( (eWeaponType)nWeaponID, 9999 );
        if ( pPlayerWeapon )
        {
            pPlayerWeapon->SetAsCurrentWeapon();
        }
    }
}

void COMMAND_ShowRPCs ( const char* szCmdLine )
{
    if ( !(szCmdLine && szCmdLine[0]) )
        return;
    g_pClientGame->GetRPCFunctions ()->m_bShowRPCs = ( atoi ( szCmdLine ) == 1 );
}


void COMMAND_ShowInterpolation ( const char* )
{
    g_pClientGame->ShowInterpolation ( ! g_pClientGame->IsShowingInterpolation () );
}

void COMMAND_Watch ( const char *szCmdLine )
{
    // Note: This code might be a little unsafe if the detouring done by the DLL happens to be done
    //       exactly on a call to WriteProcessMemory even though the chance is small.
    // adds a hook to a process and watches for WPMs to this one
    DWORD dwProcessIDs[250];
    DWORD pBytesReturned = 0;
    unsigned int uiListSize = 50;
    if ( EnumProcesses ( dwProcessIDs, 250 * sizeof(DWORD), &pBytesReturned ) )
    {
        for ( unsigned int i = 0; i < pBytesReturned / sizeof ( DWORD ); i++ )
        {
            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessIDs[i]);
            if ( hProcess )
            {
                HMODULE pModule;
                DWORD cbNeeded;
                if ( EnumProcessModules ( hProcess, &pModule, sizeof ( HMODULE ), &cbNeeded ) )
                {
                    char szModuleName[500];
                    if ( GetModuleFileNameEx( hProcess, pModule, szModuleName, 500 ) )
                    {
                        if ( stricmp ( szModuleName + strlen(szModuleName) - strlen(szCmdLine), szCmdLine ) == 0 )
                        {
                            g_pCore->GetConsole()->Printf ( "Attaching to %s with process id %d...", szModuleName, hProcess );
                            RemoteLoadLibrary ( hProcess, "C:/Program Files/Rockstar Games/GTA San Andreas/mta/wpmhookdll.dll" );
                            CloseHandle ( hProcess );
                            return;
                        } 
                    }
                }

                // Close the process
                CloseHandle ( hProcess );
            }
        }
    }
}

void COMMAND_Modules( const char *szCmdLine )
{
#if 0
    // Get the base address of the requested module
    // Take a snapshot of all modules in the specified process. 
    HANDLE hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId() );

    if ( hModuleSnap == INVALID_HANDLE_VALUE )
        return;

    // Set the size of the structure before using it. 
    MODULEENTRY32 ModuleEntry; 
    ModuleEntry.dwSize = sizeof(MODULEENTRY32); 

    // Retrieve information about the first module, 
    // and exit if unsuccessful 
    if ( Module32First( hModuleSnap, &ModuleEntry ) ) 
    {
        // Create a file
        CFile *file = modFileRoot->Open( "modules.txt", "w" );

        if ( !file )
            return;

        do
        {
            // Print it
            file->Printf( "** MODULE **\n"
                             "Name: %s\n"
                             "Base: 0x%P\n"
                             "Size: 0x%P\n"
                             "\n",
                             ModuleEntry.szModule,
                             ModuleEntry.modBaseAddr,
                             ModuleEntry.modBaseSize );
        } while ( Module32Next( hModuleSnap, &ModuleEntry ) );

        // Close it
        delete file;
    }

    // Close the snapshot object
    CloseHandle( hModuleSnap ); 
#endif
}

#include <CClientCorona.h>
CClientPickup* pPickupTest = NULL;
CClientCorona* pCoronaTest = NULL;
CVehicle * debugTrain = NULL;
CClientPlayer* pRonkert = NULL;
CObject * obj = NULL;

void COMMAND_Debug ( const char *szCmdLine )
{
    __debugbreak();

    return;

    CPools* pPools = g_pGame->GetPools ();
    int iEntryInfoNodeEntries = pPools->GetNumberOfUsedSpaces( ENTRY_INFO_NODE_POOL );
    int iPointerNodeSingleLinkEntries = pPools->GetNumberOfUsedSpaces( POINTER_SINGLE_LINK_POOL );
    int iPointerNodeDoubleLinkEntries = pPools->GetNumberOfUsedSpaces( POINTER_DOUBLE_LINK_POOL );

    g_pCore->GetConsole ()->Printf ( "entry info: %i", iEntryInfoNodeEntries );
    g_pCore->GetConsole ()->Printf ( "single node: %i", iPointerNodeSingleLinkEntries );
    g_pCore->GetConsole ()->Printf ( "dbl node: %i", iPointerNodeDoubleLinkEntries );
}

#include "CVehicleNames.h"

CVehicle* aaa = NULL;
CVehicle* bbb = NULL;

RwMatrix *save = NULL;
float fTest = 0;

#include <crtdbg.h>
void COMMAND_Debug2 ( const char *szCmdLine )
{
    g_pGame->GetAudio ()->StopRadio ();
}

CClientPed* pTest = NULL;
CClientVehicle *v, *vnew;

void COMMAND_Debug3 ( const char *szCmdLine )
{
    _asm {
        pushad
        mov     ecx, 0x8CB6F8
        mov     eax, 0x4E7F80
        call    eax
        popad
    }
    g_pGame->GetAudio()->StopRadio();
    g_pGame->GetAudio()->StartRadio(1);
    return;
}

CVector origin22;
CObject *o;

void COMMAND_Debug4 ( const char* szCmdLine )
{
    g_pCore->GetConsole ()->Printf ( "debug4" );
    g_pClientGame->StartPlayback ();
    return;
}
#endif

#pragma message("Compiling precompiled header.\n")

// Pragmas
#pragma warning (disable:4995)
#pragma warning (disable:4244)

//#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <sys/stat.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <direct.h>
#include <stdio.h>
#include <dbghelp.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <shlwapi.h>
#include <winsock.h>
#include <conio.h>

#include <dbgheap.h>

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdlib>
#include <iomanip>

// Core general
#include <core/Common.h>

#include <game/CRenderWare.h>

// SDK includes
#include <xml/CXMLNode.h>
#include <xml/CXMLFile.h>
#include <xml/CXMLAttribute.h>
#include <xml/CXMLAttributes.h>
#include <Common.h>

#include <net/CNet.h>
#include <multiplayer/CMultiplayer.h>

// Core-level includes
#define MTA_CLIENT
#include "SharedUtil.h"
#include <utils/CBuffer.h>
#include <sdk/MemoryUtils.h>
#include "CrashHandler.h"
#include "CCore.h"
#include "CDebugView.h"
#include "CChat.h"
#include "CDirect3DHookManager.h"
#include "CDirectInputHookManager.h"
#include "CDirectInputEvents8.h"
#include "CDirect3DEvents9.h"
#include "CFileFormat.h"
#include "CProxyDirectInput8.h"
#include "CProxyDirect3D9.h"
#include "CPEHookUtils.h"
#include "CLanguageLocale.h"
#include "CCommandFuncs.h"
#include "CExceptionInformation_Impl.h"
#include "tracking/CHTTPClient.h"
#include "UtilityMacros.h"
#include "CVideoModeManager.h"
#include "CServerCache.h"

extern CFileSystem *fileSystem;
extern CCore *g_pCore;
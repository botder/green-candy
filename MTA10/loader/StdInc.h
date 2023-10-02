#define MTA_LOADER

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#undef GetCurrentDirectory

#include "resource.h"
#include <shlwapi.h>
#include <stdio.h>
#include "shellapi.h"

#include <shlobj.h>
#include <Psapi.h>
#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <DbgHelp.h>

#define MTA_CLIENT
#include "SharedUtil.h"
#include <core/interface.h>
#include <CFileSystem.h>
#include "Main.h"
#include "Install.h"
#include "Utils.h"
#include "..\version.h"
#include "CInstallManager.h"

extern CFileSystem *fileSystem;
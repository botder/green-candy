/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/core/Common.h
*  PURPOSE:     Internal shared routines
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _CORE_SHARED_
#define _CORE_SHARED_

#ifdef _WIN32
#include <windows.h>
#include <DbgHelp.h>
#endif

#include <sys/stat.h>

#include <SharedUtil.h>
#include <sdk/MemoryUtils.h>
#include  <sdk/OSUtils.h>
#include "interface.h"
#include "CFileSystem.h"
#include "CDynamicLibrary.h"

#endif //_CORE_SHARED_
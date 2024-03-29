/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CPEHookUtils.h
*  PURPOSE:     Header file for PE hooking class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPEHOOKUTILS_H
#define __CPEHOOKUTILS_H

typedef struct _section_info_t {
    DWORD Address;
    DWORD  Size;
} SECTIONINFO, *PSECTIONINFO;

class CPEHookUtils
{
    public:

    DWORD               GetIATAddress   ( const std::string& ModuleName,
                                          const std::string& FunctionName, 
                                          HANDLE hImgBase );
    DWORD               WriteIATAddress ( const std::string& ModuleName,
                                          const std::string& FunctionName,
                                          HANDLE hImgBase,
                                          PVOID  pvNewProc );
    PVOID               HookVTableFunc  ( PVOID  pvInterface,
                                          int    nOffset,
                                          PVOID  pvHookFunc,
                                          int    nVtableSlot );
    void                GetSectionHeader( const std::string& SectName,
                                          const HANDLE hImgBase,
                                          PSECTIONINFO pSectionInfo );

    private:

    PIMAGE_NT_HEADERS   xImageNTHeader  ( const HANDLE hImgBase );
};

#endif

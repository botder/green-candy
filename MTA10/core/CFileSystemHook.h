/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CFileSystemHook.h
*  PURPOSE:     Header file for file system hooking class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CFILESYSTEMHOOK_H
#define __CFILESYSTEMHOOK_H

#include "CSingleton.h"
#include "CLinkedList.h"
#include "CFileSystemMemoryHandleManager.h"

typedef HANDLE  (__stdcall*pCreateFileA)        (LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
typedef BOOL    (__stdcall*pReadFile)           (HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
typedef DWORD   (__stdcall*pGetFileSize)        (HANDLE,LPDWORD);
typedef DWORD   (__stdcall*pSetFilePointer)     (HANDLE,LONG,PLONG,DWORD);
typedef BOOL    (__stdcall*pCloseHandle)        (HANDLE);

typedef struct tagFILEREDIRECT
{
    std::string                     NativeFile;
    std::string                     RedirectedFile;
    bool                            bRedirectToBuffer;
    void*                           pBuffer;
    size_t                          sizeBufferSize;
    bool                            bBufferReadOnly;

} FILEREDIRECT;

class CFileSystemHook : public CSingleton < CFileSystemHook >
{
    public:

                            CFileSystemHook         ( );
                           ~CFileSystemHook         ( );

CFileSystemMemoryHandleManager*     GetFileSystemMemoryHandleManager ( void );

            void            RemoveHook              ( );
            void            ApplyHook               ( );

            void            RedirectFile            ( const std::string& RedirectedFileName, const std::string& DestinationFile );
            void            RedirectFile            ( const std::string& RedirectedFileName, void* pFilebufferOut, size_t size, bool bReadOnly );
            void            RemoveRedirect          ( const std::string& RedirectedFileName );

    static  HANDLE  WINAPI   API_CreateFileA        ( LPCTSTR               lpFileName,
                                                      DWORD                 dwDesiredAccess,
                                                      DWORD                 dwShareMode,
                                                      LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                                      DWORD                 dwCreationDisposition,
                                                      DWORD                 dwFlagsAndAttributes,
                                                      HANDLE                hTemplateFile );

    static  BOOL    WINAPI   API_ReadFile            ( HANDLE hFile,
                                                      LPVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped );

    static  DWORD   WINAPI   API_WriteFile           ( HANDLE hFile,
                                                      LPCVOID lpBuffer,
                                                      DWORD nNumberOfBytesToRead,
                                                      LPDWORD lpNumberOfBytesRead,
                                                      LPOVERLAPPED lpOverlapped );

    static  DWORD   WINAPI   API_GetFileSize         ( HANDLE hFile,
                                                       LPDWORD lpFileSizeHigh );

    static  DWORD   WINAPI   API_SetFilePointer      ( HANDLE                hFile,
                                                      LONG                  lDistanceToMove,
                                                      PLONG                 lpDistanceToMoveHigh,
                                                      DWORD                 dwMoveMethod );

    static BOOL     WINAPI   API_CloseHandle         ( HANDLE hObject );

    private:

            bool            GetRedirectedFile       ( const std::string&                   NativeFileToCheck,
                                                      std::string &                 RedirectedFileOut,
                                                      bool&                         bRedirectToFileBuffer,
                                                      void**                        pFilebufferOut,
                                                      size_t&                       size,
                                                      bool&                         bReadOnly );

    CFileSystemMemoryHandleManager* m_pMemoryHandleManager;

    pCreateFileA                    m_pfnCreateFileA;
    SIZE_T                          m_CreateFileA_hookid;
    pReadFile                       m_pfnReadFile;
    SIZE_T                          m_ReadFile_hookid;
    pGetFileSize                    m_pfnGetFileSize;
    SIZE_T                          m_GetFileSize_hookid;
    pSetFilePointer                 m_pfnSetFilePointer;
    SIZE_T                          m_SetFilePointer_hookid;
    pCloseHandle                    m_pfnCloseHandle;
    SIZE_T                          m_CloseHandle_hookid;
  
    CLinkedList < FILEREDIRECT >    m_RedirectList;

};

#endif

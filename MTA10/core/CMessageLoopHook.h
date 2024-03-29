/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMessageLoopHook.h
*  PURPOSE:     Header file for message loop hook class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMESSAGELOOPHOOK_H
#define __CMESSAGELOOPHOOK_H

#include "CSingleton.h"

#define URI_CONNECT 1

class CMessageLoopHook : public CSingleton < CMessageLoopHook >
{
    public:
    
                                CMessageLoopHook        ( );
                               ~CMessageLoopHook        ( );

    void                        ApplyHook               ( HWND hFocusWindow );
    void                        RemoveHook              ( );
    HWND                        GetHookedWindowHandle   ( ) const;

    static  LRESULT CALLBACK    ProcessMessage      ( HWND hwnd, 
                                                      UINT uMsg, 
                                                      WPARAM wParam, 
                                                      LPARAM lParam );

    private:

    WNDPROC     m_HookedWindowProc;
    HWND        m_HookedWindowHandle;

    static WPARAM      m_LastVirtualKeyCode;
    static UCHAR       m_LastScanCode;
    static BYTE*       m_LastKeyboardState;
  
};

#endif
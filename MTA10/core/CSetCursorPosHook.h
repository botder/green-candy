/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSetCursorPosHook.cpp
*  PURPOSE:     Header file for cursor position hook class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSETCURSORPOSHOOK_H
#define __CSETCURSORPOSHOOK_H

#include "CSingleton.h"
#include <core/CSetCursorPosHookInterface.h>

typedef BOOL ( WINAPI * pSetCursorPos ) ( int X, int Y );

class CSetCursorPosHook : public CSetCursorPosHookInterface, public CSingleton < CSetCursorPosHook > 
{
    public:
                        CSetCursorPosHook       ( void );
                       ~CSetCursorPosHook       ( void );

    void                ApplyHook               ( void );
    void                RemoveHook              ( void );

    void                DisableSetCursorPos     ( void );
    void                EnableSetCursorPos      ( void );
    bool                IsSetCursorPosEnabled   ( void );

    BOOL                CallSetCursorPos        ( int X, int Y );

    static BOOL WINAPI  API_SetCursorPos        ( int X, int Y );

    private:

    bool            m_bCanCall;
    pSetCursorPos   m_pfnSetCursorPos;
    SIZE_T          m_SetCursorPos_hookid;
};

#endif

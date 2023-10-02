/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleHistory.h
*  PURPOSE:     Header file for the console history class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONSOLEHISTORY_H
#define __CCONSOLEHISTORY_H

#include <list>

class CConsoleHistory
{
public:
                    CConsoleHistory             ( unsigned int uiHistoryLength );
                    ~CConsoleHistory            ( void );

    bool            LoadFromFile                ( const char* szFilename, bool bAutosave );

    void            Add                         ( const char* szLine );
    void            Clear                       ( void );

    const char*     Get                         ( unsigned int uiIndex );

private:
    void                    DeleteLastEntry             ( void );

    typedef std::list <std::string> history_t;
    history_t               m_History;
    unsigned int            m_uiHistoryLength;

    std::string             m_strFilename;
    CFile*                  m_file;
};

#endif

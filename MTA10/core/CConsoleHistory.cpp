/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CConsoleHistory.cpp
*  PURPOSE:     Maintaining the history of the in-game console
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

CConsoleHistory::CConsoleHistory ( unsigned int uiHistoryLength )
{
    m_uiHistoryLength = uiHistoryLength;
    m_file = NULL;
}

CConsoleHistory::~CConsoleHistory ( void )
{
    // Unload an eventual file
    if ( m_file )
        delete m_file;

    // Clear our history
    Clear ();
}

bool CConsoleHistory::LoadFromFile ( const char* szFilename, bool bAutosave )
{
    // Close the previous file if any
    if ( m_file )
        delete m_file;

    // Open the file
    m_file = mtaFileRoot->Open( szFilename, "r" );

    if ( !m_file )
        return false;

    m_file->Seek( 0, SEEK_SET );

    // Read all the lines
    std::string line;

    while ( !m_file->IsEOF() )
    {
        FileSystem::FileGetString( m_file, line );

        if ( line.empty() )
            continue;

        // Put it in our list
        m_History.push_back( line );

        line.clear();
    }

    // Close the file if we're not supposed to autosave to it
    if ( !bAutosave )
    {
        delete m_file;

        m_file = NULL;
    }
    else
        m_strFilename = szFilename ? szFilename : "";

    // Success
    return true;
}

void CConsoleHistory::Add ( const char* szLine )
{
    if ( m_History.size() != 0 && m_History.front() == szLine )
        return;

    std::string line( szLine );

    m_History.push_front( line );

    // Is the list too big? Remove the last item
    if ( m_History.size () > m_uiHistoryLength )
        DeleteLastEntry();

    // Write to the file history if any
    if ( !m_file )
        return;

    // Add a newline character so every line is an actual line
    line += '\n';

    delete m_file;

    m_file = mtaFileRoot->Open( m_strFilename.c_str(), "a" );

    // Append our line
    m_file->Write( line.c_str(), 1, line.size() );

    // Flush it into the file
    m_file->Flush();
}

void CConsoleHistory::Clear ( void )
{
    // Clear the list
    m_History.clear ();
}

const char* CConsoleHistory::Get ( unsigned int index )
{
    // Grab the item with the chosen index
    unsigned int n = 0;
    history_t::iterator iter;

    for ( iter = m_History.begin (); iter != m_History.end (); ++iter )
    {
        if ( n++ != index )
            continue;

        return (*iter).c_str();
    }

    // Couldn't find it, return NULL
    return NULL;
}

void CConsoleHistory::DeleteLastEntry ( void )
{
    // We got entries?
    if ( m_History.empty () )
        return;

    m_History.pop_back();
}

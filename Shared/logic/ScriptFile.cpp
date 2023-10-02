/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ScriptFile.cpp
*  PURPOSE:     Script file class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               The_GTA <quiret@gmx.de>
*
*****************************************************************************/

#include <StdInc.h>

#if 0

#include "ScriptFile.h"

ScriptFile::ScriptFile( CFile *file )
{
    // Init
    m_file = file;
}

ScriptFile::~ScriptFile()
{
    // Close the file
    delete m_file;
}

bool ScriptFile::IsEOF()
{
    // Reached end of file?
    return m_file->IsEOF();
}

size_t ScriptFile::GetPointer()
{
    return m_file->Tell();
}

size_t ScriptFile::GetSize()
{
    return m_file->GetSize();
}

size_t ScriptFile::SetPointer( size_t pos )
{
    // Move the pointer
    m_file->Seek( (long)pos, SEEK_SET );

    // Return the new position
    return m_file->Tell();
}

void ScriptFile::SetSize( size_t size )
{
    m_file->SetSize( size );
}

void ScriptFile::Flush()
{
    m_file->Flush();
}

size_t ScriptFile::Read( size_t size, char *data )
{
    // Try to read data into the given block. Return number of bytes we read.
    return m_file->Read( data, 1, size );
}

size_t ScriptFile::Write( size_t size, const char *data )
{
    // Write the data into the given block. Return number of bytes we wrote.
    return m_file->Write( data, 1, size );
}

#endif //0
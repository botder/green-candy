/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CScriptFile.cpp
*  PURPOSE:     Script file entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

CScriptFile::CScriptFile( CResource *resource, CFile *file ) : ScriptFile( file ), CElement( NULL )
{
    m_iType = SCRIPTFILE;
    SetTypeName( "file" );

    m_resource = resource;
}

ScriptFile::~ScriptFile()
{
}
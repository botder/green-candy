/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.File.hpp
*  PURPOSE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <CFileSystemInterface.h>

void SharedUtil::ExtractFilename ( const SString& in, SString* strPath, SString* strFilename )
{
    if ( !in.Split ( PATH_SEPERATOR, strPath, strFilename, -1 ) )
        if ( strFilename )
            *strFilename = in;
}

bool SharedUtil::ExtractExtention( const SString& in, SString* strMain, SString* strExt )
{
    return in.Split ( ".", strMain, strExt, -1 );
}

SString SharedUtil::ExtractFilename( const SString& in )
{
    SString strFilename;
    ExtractFilename ( in, NULL, &strFilename );
    return strFilename;
}

SString SharedUtil::ExtractPath( const SString& strPathFilename )
{
    SString strPath;
    ExtractFilename ( strPathFilename, &strPath, NULL );
    return strPath;
}

SString SharedUtil::ExtractExtention( const SString& strPathFilename )
{
    SString strExt;
    ExtractExtention ( strPathFilename, NULL, &strExt );
    return strExt;
}

SString SharedUtil::ExtractBeforeExtention( const SString& strPathFilename )
{
    SString strMain;
    ExtractExtention ( strPathFilename, &strMain, NULL );
    return strMain;
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

filePath SharedUtil::MakeUniquePath( const filePath& path )
{
    unsigned int n = 0;
    filePath pre, post;
    filePath strPath;

    FileSystem::GetFileNameItem( path, false, &strPath );

    SString ext;

    bool hasExtention = SharedUtil::ExtractExtention( SString( path.convert_ansi() ), NULL, &ext );

    if ( hasExtention )
    {
        pre = strPath;
        post = ".";
        post += ext;
    }
    else
        pre = path;

    filePath strTest = path;

    while ( true )
    {
        std::wstring unicodePath = strTest.convert_unicode();

        bool exists = ( GetFileAttributesW( unicodePath.c_str() ) != INVALID_FILE_ATTRIBUTES );

        if ( !exists )
            break;

        std::string num_str = std::to_string( n++ );
        
        strTest = pre + filePath( L"_" ) + filePath( num_str ) + post;
    }

    return strTest;
}
#endif
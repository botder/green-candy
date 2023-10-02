/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceHTMLItem.cpp
*  PURPOSE:     Resource server-side HTML item class
*  DEVELOPERS:  Ed Lyons <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

// This class represents a single HTML resource item.
// This parses it and converts into into a script

#include "StdInc.h"

extern CServerInterface* g_pServerInterface;
extern CGame* g_pGame;

CResourceHTMLItem::CResourceHTMLItem( CResource *resource, const char *name, const filePath& path, CXMLAttributes *attr, bool bIsDefault, bool bIsRaw, bool bRestricted ) : 
    CResourceFile( resource, szShortName, szResourceFileName, attr )
{
    m_bIsRaw = bIsRaw;
    m_type = RESOURCE_FILE_TYPE_HTML;
    m_bDefault = bIsDefault;
    m_pVM = NULL;
    m_bIsBeingRequested = false;
    m_bRestricted = bRestricted;
}

CResourceHTMLItem::~CResourceHTMLItem()
{
    Stop();
}

ResponseCode CResourceHTMLItem::Request( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, CAccount * account )
{
    if ( !m_pVM )
        Start ();

    if ( m_bIsBeingRequested )
    {
        ipoHttpResponse->SetBody ( "Busy!", strlen("Busy!") );
        return HTTPRESPONSECODE_500_INTERNALSERVERERROR;
    }

    m_bIsBeingRequested = true;

    m_responseCode = HTTPRESPONSECODE_200_OK;

    if ( !m_bIsRaw )
    {
        ipoHttpResponse->oResponseHeaders [ "content-type" ] = m_strMime;

        CLuaArguments formData;
        for ( FormValueMap::iterator iter = ipoHttpRequest->oFormValueMap.begin(); iter != ipoHttpRequest->oFormValueMap.end(); iter++ )
        {
            formData.PushString ( (*iter).first.c_str() );
            formData.PushString ( ((FormValue)(*iter).second).sBody.c_str() );
        }

        CLuaArguments cookies;
        for ( CookieMap::iterator iter = ipoHttpRequest->oCookieMap.begin(); iter != ipoHttpRequest->oCookieMap.end(); iter++ )
        {
            cookies.PushString ( (*iter).first.c_str() );
            cookies.PushString ( (*iter).second.c_str() );
        }

        CLuaArguments headers;
        for ( StringMap::iterator iter = ipoHttpRequest->oRequestHeaders.begin(); iter != ipoHttpRequest->oRequestHeaders.end(); iter++ )
        {
            headers.PushString ( (*iter).first.c_str() );
            headers.PushString ( (*iter).second.c_str() );
        }

        m_currentResponse = ipoHttpResponse;
        CLuaArguments querystring ( formData );
        CLuaArguments args;
        args.PushTable ( &headers ); // requestHeaders
        args.PushTable ( &formData ); // form
        args.PushTable ( &cookies ); // cookies
        args.PushString ( ipoHttpRequest->GetAddress().c_str() ); // hostname
        args.PushString ( ipoHttpRequest->sOriginalUri.c_str() ); // url
        args.PushTable ( &querystring ); // querystring
        args.PushAccount ( account );

       // g_pGame->Lock(); // get the mutex (blocking)
        args.CallGlobal ( m_pVM, "renderPage" );
       // g_pGame->Unlock(); // release the mutex

        ipoHttpResponse->SetBody ( m_strPageBuffer.c_str (), m_strPageBuffer.size () );
        m_strPageBuffer.clear ();
    }
    else
    {
        std::vector <char> content;

        if ( modFileRoot->ReadToBuffer( m_path.c_str(), content ) )
        {
            ipoHttpResponse->oResponseHeaders["content-type"] = m_strMime.c_str();
            ipoHttpResponse->SetBody( &content[0], content.size() );
        }
        else
            ipoHttpResponse->SetBody( "Can't read file!", 16 );
    }
    m_bIsBeingRequested = false;
    return m_responseCode;
}

void CResourceHTMLItem::ClearPageBuffer()
{
    m_strPageBuffer.clear ();
}

void CResourceHTMLItem::SetResponseHeader( const char* szHeaderName, const char* szHeaderValue )
{
    m_currentResponse->oResponseHeaders[szHeaderName] = szHeaderValue;
}

void CResourceHTMLItem::SetResponseCode( int responseCode )
{
    m_responseCode = (ResponseCode)responseCode;
}

void CResourceHTMLItem::SetResponseCookie( const char* szCookieName, const char* szCookieValue )
{
    CookieParameters params;
    Datum data;
    data = szCookieValue;
    params [ szCookieName ] = data;
    m_currentResponse->SetCookie ( params );
}

bool CResourceHTMLItem::AppendToPageBuffer( const char * szText, size_t length )
{
    if ( szText )
        m_strPageBuffer.append( szText, length );
    return true;
}

bool CResourceHTMLItem::Start()
{
    CFile *file;

    if ( m_bIsRaw )
    {
        // its a raw page
        file = modFileRoot->Open( m_path.c_str(), "rb" );

        if ( !file )
            return false;

        GetMimeType( m_strResourceFileName.c_str() );

        // don't actually read it here, it could be way too large
        delete file;

        return true;
    }

    // go through and search for <* or *>
    file = modFileRoot->Open( m_path.c_str(), "r" );

    if ( !file )
        return false;

    bool bInCode = false;
    bool bJustStartedCodeBlock = false;
    bool bIsShorthandCodeBlock = false;
    std::string strScript( "function renderPage( requestHeaders, form, cookies, hostname, url, querystring, user )\n" );
    strScript += "\nhttpWrite( \""; // bit hacky, possibly can be terminated straight away
    unsigned char c;
    unsigned int i = 0;

    while ( !file->IsEOF() )
    {
        c = file->ReadByte();

        if ( file->IsEOF() )
            break;

        if ( bInCode == false ) // we're in a plain HTML section
        {
            if ( c == '<' )
            {
                c = file->ReadByte();

                if ( c == '*' ) // we've found <*
                {
                    bInCode = true;
                    bJustStartedCodeBlock = true;
                    strScript += "\" )\n"; // add ") to the end to terminate our last non-code section
                }
                else
                {   // we found < but not a *, so just output both characters we read
                    strScript += '<';
                    strScript += c;
                }
            }
            else
            {
                if ( c == '\r' )
                    strScript += "\\r";
                else if ( c == '\n' )
                    strScript += "\\n";
                else if ( c == '\\' )
                    strScript += "\\\\";
                else if ( c == '\"' )
                    strScript += "\\\"";
                else
                    strScript += c;
            }
        }
        else
        {
            // we're in a code block
            if ( c == '*' )
            {
                c = file->ReadByte();

                if ( c == '>' ) // we've found *>
                {
                    bInCode = false;

                    if ( bIsShorthandCodeBlock )
                    {
                        bIsShorthandCodeBlock = false;
                        strScript += ')'; // terminate the 'httpWrite' function
                    }

                    strScript += "\nhttpWrite ( \""; // add httpWrite ( " to start a new non-code section
                }
                else
                {
                    // we found * but not a >, so just output both characters we read
                    strScript += '*';
                    strScript += c;
                }
            }
            else if ( c == '=' && bJustStartedCodeBlock )
            {
                strScript += "httpWrite ( ";
                bIsShorthandCodeBlock = true;
            }
            else
            {
                if ( c != '\t' && c != ' ' ) // we allow whitespace before the shorthand '=' sign
                    bJustStartedCodeBlock = false;

                strScript += c;
            }
        }
        i++;
    }

    if ( !bInCode )
        strScript += "\" )\n";

    strScript += "\nend";

#if 0
    FILE *debug = fopen( "debug.lua", "w" );
    fwrite( m_szBuffer, 1, strlen(m_szBuffer), debug );
    fclose( debug );
#endif

    m_pVM = g_pGame->GetLuaManager()->CreateVirtualMachine( m_resource );
    m_pVM->LoadScript( strScript.c_str() );
    m_pVM->SetResourceFile( this );
    m_pVM->RegisterHTMLDFunctions();

    delete file;

    GetMimeType ( m_strResourceFileName.c_str () );
    return true;
}

void CResourceHTMLItem::GetMimeType( const char * szFilename )
{
    const char* pExtn = strrchr( szFilename, '.' );

    if ( !pExtn )
    {
        m_strMime = "text/html";
        return;
    }

    pExtn++;

    if ( stricmp( pExtn, "css" ) == 0 )
    {
        m_strMime = "text/css";
        return;
    }
    else if ( stricmp( pExtn, "png" ) == 0 )
    {
        m_strMime = "image/png";
        return;
    }
    else if ( stricmp( pExtn, "gif" ) == 0 )
    {
        m_strMime = "image/gif";
        return;
    }
    else if ( stricmp( pExtn, "jpg" ) == 0 || stricmp( pExtn, "jpeg" ) == 0 )
    {
        m_strMime = "image/jpg";
        return;
    }
    else if ( stricmp( pExtn, "js" ) == 0 )
    {
        m_strMime = "text/javascript";
        return;
    }

    m_strMime = "text/html";
}

bool CResourceHTMLItem::Stop()
{
    if ( m_pVM )
    {
        // Delete the events on this VM
        g_pGame->GetMapManager()->GetRootElement()->DeleteEvents( m_pVM, true );

        g_pGame->GetLuaManager()->RemoveVirtualMachine( m_pVM );
    }

    m_pVM = NULL;
    return true;
}

void CResourceHTMLItem::OutputHTMLEntry( std::string& buf )
{
    if ( m_bDefault )
        buf += "<b>";

    buf += "<li>";

    buf += "<a href='/resources/";
    buf += m_resource->GetName();
    buf += '/';
    buf += m_name;
    buf += "' style='color:black;'>";
    
    CResourceFile::OutputHTMLEntry( buf );

    buf += "</a>";

    if ( m_bDefault )
        buf += "</b>";
}
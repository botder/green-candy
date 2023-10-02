/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceHTMLItem.h
*  PURPOSE:     Resource server-side HTML item class
*  DEVELOPERS:  Ed Lyons <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCEHTMLITEM_H
#define CRESOURCEHTMLITEM_H

#include "CResourceFile.h"
#include "ehs/ehs.h"

class CResourceHTMLItem : public CResourceFile
{

public:

                                        CResourceHTMLItem               ( class CResource *resource, const char *namr, const filePath& path, CXMLAttributes *attr, bool bIsDefault, bool bIsRaw, bool bRestricted );
                                        ~CResourceHTMLItem              ();

    bool                                Start                           ();
    bool                                Stop                            ();
    ResponseCode                        Request                         ( HttpRequest * ipoHttpRequest, HttpResponse * ipoHttpResponse, class CAccount * account );
    bool                                AppendToPageBuffer              ( const char * szText, size_t length=0 );

    void                                SetResponseHeader               ( const char* szHeaderName, const char* szHeaderValue );
    void                                SetResponseCode                 ( int responseCode );
    void                                SetResponseCookie               ( const char* szCookieName, const char* szCookieValue );
    void                                ClearPageBuffer                 ();

    inline bool                         IsDefaultPage                   () { return m_bDefault; }
    inline void                         SetDefaultPage                  ( bool bDefault ) { m_bDefault = bDefault; }
    inline bool                         IsRestricted                    () { return m_bRestricted; };

    void                                OutputHTMLEntry                 ( std::string& buf );

private:
    void                                GetMimeType                     ( const char * szFilename );

    bool                                m_bIsBeingRequested; // crude mutex
    bool                                m_bIsRaw;
    CLuaMain*                           m_pVM;
    std::string                         m_strPageBuffer;     // contains what we're sending
    bool                                m_bDefault;          // is this the default page for this resource?
    std::string                         m_strMime;
    bool                                m_bRestricted;

    ResponseCode                        m_responseCode;
    HttpResponse*                       m_currentResponse;
};

#endif


/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/xml/CXMLAttributes.h
*  PURPOSE:     XML attributes container interface
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CXMLATTRIBUTES_H
#define __CXMLATTRIBUTES_H

#include <list>
#include "SharedUtil.h"

class CXMLAttribute;

class CXMLAttributes
{
public:
    virtual unsigned int                Count           () = 0;
    virtual CXMLAttribute*              Find            ( const char* szName ) = 0;
    virtual CXMLAttribute*              Get             ( unsigned int uiIndex ) = 0;

    virtual CXMLAttribute*              Create          ( const char* szName ) = 0;
    virtual CXMLAttribute*              Create          ( const CXMLAttribute& Copy ) = 0;
    virtual void                        DeleteAll       () = 0;

    virtual std::list < CXMLAttribute* >::iterator  ListBegin() = 0;
    virtual std::list < CXMLAttribute* >::iterator  ListEnd() = 0;

    template <class type>
    void    Set( const char *key, type value )
    {
        CXMLAttribute *att = Find( key );

        if ( att )
            att->SetValue( value );
        else
            Create( key )->SetValue( value );
    }

    template <class type>
    type    Get( const char *key )
    {
        CXMLAttribute *att = Find( key );

        if ( !att )
            return (type)0;

        return att->GetValue();
    }

    template <class type>
    type    GetDefault( const char *key, type def )
    {
        CXMLAttribute *att = Find( key );

        if ( !att )
            return def;

        return att->GetValue();
    }
};

#endif

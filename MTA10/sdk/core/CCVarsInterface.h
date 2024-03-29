/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CCVarsInterface.h
*  PURPOSE:     Core client variables interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCVARSINTERFACE_H
#define __CCVARSINTERFACE_H

#include <string>

class CCVarsInterface
{
public:
    // Existance check
    virtual bool    Exists                  ( const std::string& strVariable ) = 0;

    // Get queries
    virtual bool    Get                     ( const std::string& strVariable, bool &val ) = 0;
    virtual const char* Get                 ( const std::string& strVariable ) = 0;
    virtual bool    Get                     ( const std::string& strVariable, int &val ) = 0;
    virtual bool    Get                     ( const std::string& strVariable, unsigned int &val ) = 0;
    virtual bool    Get                     ( const std::string& strVariable, float &val ) = 0;

    // Set queries
    virtual void    Set                     ( const std::string& strVariable, bool val ) = 0;
    virtual void    Set                     ( const std::string& strVariable, const std::string& val ) = 0;
    virtual void    Set                     ( const std::string& strVariable, int val ) = 0;
    virtual void    Set                     ( const std::string& strVariable, unsigned int val ) = 0;
    virtual void    Set                     ( const std::string& strVariable, float val ) = 0;
};

#endif

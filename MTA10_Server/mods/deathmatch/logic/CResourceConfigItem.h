/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CResourceConfigItem.h
*  PURPOSE:     Resource server-side (XML) configuration file item class
*  DEVELOPERS:  Ed Lyons <>
*               Kevin Whiteside <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef CRESOURCECONFIGITEM_H
#define CRESOURCECONFIGITEM_H

#include "CResourceFile.h"

class CResourceConfigItem : public CResourceFile
{
    
public:

                                        CResourceConfigItem             ( class CResource *resource, const filePath& path, CXMLAttributes *attr );
                                        ~CResourceConfigItem            ();

    bool                                Start                           ();
    bool                                Stop                            ();
    inline class CXMLFile *             GetFile                         () { return m_pXMLFile; }
    inline class CXMLNode *             GetRoot                         () { return m_pXMLRootNode; }

private:
    class CXMLFile*                     m_pXMLFile;
    CXMLNode*                           m_pXMLRootNode;

    bool                                m_bInvalid;
};

#endif


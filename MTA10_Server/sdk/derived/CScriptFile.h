/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CScriptFile.h
*  PURPOSE:     Script file entity
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _SCRIPTFILE_
#define _SCRIPTFILE_

class CScriptFile : public ScriptFile <CElement>
{
public:
                            CScriptFile( CResource *resource, CFile *file );
                            ~CScriptFile();

    // Functions required by CElement
    void                    Unlink                  () {};
    bool                    ReadSpecialData         ()    { return true; };
    
private:
    CResource*              m_resource;
};

#endif //_SCRIPTFILE_
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

// Make sure you can only include either derived server or client
// That being, call the macros the same ;)
#ifndef _SCRIPTFILE_
#define _SCRIPTFILE_

class CScriptFile : public ScriptFile <CClientEntity>
{
    DECLARE_CLASS( CScriptFile, CClientEntity )
public:
                            CScriptFile( CFile *file );
                            ~CScriptFile();

    // Functions required for linking
    void                    GetPosition             ( CVector& vecPosition ) const {};
    void                    SetPosition             ( const CVector& vecPosition ) {};

    // Functions required by CClientEntity
    eClientEntityType       GetType                 () const  { return SCRIPTFILE; };
    void                    Unlink                  () {};
    bool                    ReadSpecialData         ()    { return true; };
};

#endif //_SCRIPTFILE_
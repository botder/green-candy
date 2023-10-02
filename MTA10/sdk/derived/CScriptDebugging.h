/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        derived/CScriptDebugging.h
*  PURPOSE:     Script debugging
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _SCRIPT_DEBUGGING_
#define _SCRIPT_DEBUGGING_

class CLuaManager;

class CScriptDebugging : public ScriptDebugging
{
public:
                                CScriptDebugging( CLuaManager& manager );

    bool                        SetLogfile( const char *path, unsigned int level );

protected:
    void                        NotifySystem( unsigned int level, const filePath& filename, int line, std::string& msg, unsigned char r, unsigned char g, unsigned char b );
    void                        PathRelative( const char *in, filePath& out );

private:
    bool                        m_triggerCall;
};

#endif //_SCRIPT_DEBUGGING_
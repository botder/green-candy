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

class CScriptDebugging : public ScriptDebugging
{
public:
                                CScriptDebugging();
                                ~CScriptDebugging();

    bool                        AddPlayer                       ( class CPlayer& player, unsigned int level );
    bool                        RemovePlayer                    ( class CPlayer& player );
    void                        ClearPlayers                    ();

    void                        LogError                        ( const char *path, int line, SString msg );

    bool                        SetLogfile                      ( const char *path, unsigned int level );
    void                        SetHTMLLogLevel                 ( unsigned int level );

protected:
    void                        NotifySystem                    ( unsigned int level, lua_State *lua, const filePath& filename, int line, std::string& msg, unsigned char r, unsigned char g, unsigned char b );
    void                        PathRelative                    ( const char *in, filePath& out );

    void                        Broadcast                       ( const CPacket& packet, unsigned int level );

private:
    bool                        m_triggerCall;
    unsigned int                m_htmlLogLevel;

    std::list <class CPlayer*>  m_players;
};

#endif //_SCRIPT_DEBUGGING_
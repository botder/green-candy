/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/ScriptDebugging.h
*  PURPOSE:     Script debugging base
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_SCRIPT_DEBUGGING_
#define _BASE_SCRIPT_DEBUGGING_

class ScriptDebugging
{
public:
                                    ScriptDebugging( class LuaManager& manager );
                                    ~ScriptDebugging();

    void                            LogCustom( unsigned char red, unsigned char green, unsigned char blue, const char *fmt, ... );
    void                            LogCustom( const char *msg, unsigned char red = 255, unsigned char green = 255, unsigned char blue = 255 );
    void                            LogInformation( const char *fmt, ... );
    void                            LogWarning( const char *fmt, ... );
    void                            LogError( const char *fmt, ... );
    void                            LogBadPointer( const char *func, const char *argType, unsigned int argID );
    void                            LogBadType( const char *func );
    void                            LogBadLevel( const char *func, unsigned int level );

    // Remember to add a log file set function!
    void                            CloseLogFile();

protected:
    // Inheritance requirements
    virtual void                    NotifySystem( unsigned int level, const filePath& filename, int line, std::string& msg, unsigned char r, unsigned char g, unsigned char b ) = 0;
    virtual void                    PathRelative( const char *in, filePath& out ) = 0;

    void                            LogString( const char *pre, const char *msg, unsigned int minLevel, unsigned char red = 255, unsigned char green = 255, unsigned char blue = 255 );
    void                            PrintLog( const char *msg );

    CFile*                          m_file;
    unsigned int                    m_fileLevel;
    LuaManager&                     m_system;
};

// We require a global definition of the script debugging
// Define it in your project somewhere
extern ScriptDebugging *scriptDebug;

#endif //_BASE_SCRIPT_DEBUGGING_
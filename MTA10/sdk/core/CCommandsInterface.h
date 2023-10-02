/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/core/CCommandsInterface.h
*  PURPOSE:     Dynamic command manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCOMMANDSINTERFACE_H
#define __CCOMMANDSINTERFACE_H

typedef void (*PFNCOMMANDHANDLER)               ( const char* );
typedef bool (*pfnExecuteCommandHandler)        ( const char*, const char*, bool, bool );

typedef void (*PFNCOMMAND) ( const char * );

#define MAX_COMMAND_NAME_LENGTH             128
#define MAX_COMMAND_DESCRIPTION_LENGTH      128

typedef struct tagCOMMANDENTRY
{
    char            szCommandName[MAX_COMMAND_NAME_LENGTH];
    char            szDescription[MAX_COMMAND_DESCRIPTION_LENGTH];
    PFNCOMMAND      pfnCmdFunc;
    bool            bModCommand;
    bool            bEnabled;
} COMMANDENTRY;

class CCommandsInterface
{
public:
    virtual void            Add( const char* szCommand, const char* szDescription, PFNCOMMANDHANDLER pfnHandler, bool bModCommand = false ) = 0;

    virtual unsigned int    Count() = 0;
    virtual bool            Exists( const char* szCommand ) = 0;

    virtual bool            Execute( const char* szCommandLine ) = 0;
    virtual bool            Execute( const char* szCommand, const char* szParameters, bool bHandleRemotely = false ) = 0;

    virtual void            Delete( const char* szCommand ) = 0;
    virtual void            DeleteAll() = 0;

    virtual void            SetExecuteHandler( pfnExecuteCommandHandler pfnHandler ) = 0;

    virtual COMMANDENTRY*   Get( const char* szCommand, bool bCheckIfMod = false, bool bModCommand = false ) = 0;

    typedef std::list <COMMANDENTRY*> commands_t;

    virtual commands_t::iterator    IterBegin() = 0;
    virtual commands_t::iterator    IterEnd() = 0;

};

#endif

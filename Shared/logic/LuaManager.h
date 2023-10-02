/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/lua/LuaManager.h
*  PURPOSE:     Management of Lua hyperstructures
*       Every lua hyperstructure is a table in a global lua environment
*       upkept by the LuaManager entity. LuaManager is supposed to secure
*       the connections between hyperstructures and provide the basic
*       framework for their runtime.
*  DEVELOPERS:  Ed Lyons <>
*               Cecill Etheredge <>
*               Christian Myhre Lundheim <>
*               Oliver Brown <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BASE_LUA_MANAGER_
#define _BASE_LUA_MANAGER_

#include "LuaCFunctions.h"

#include <chrono>

enum eLuaTypeEx
{
    LUA_TGLOBALPROXY = LUA_TTHREAD + 1,
    LUA_TELEMENT
};

class ResourceManager;

class LuaManager abstract
{
	friend class LuaClass;
    friend class LuaMain;
    friend class RegisteredCommands;
public:
                                    LuaManager( Events& events, ScriptDebugging& debug );
                                    ~LuaManager();

    void                            Shutdown();

    inline ScriptDebugging&         GetDebug()                  { return m_debug; }
    inline int                      GetGlobalEnvironment()      { return LUA_GLOBALSINDEX; }

    // Security inheritances
    virtual int                     AccessGlobal();
    virtual int                     AccessGlobalTable();

    // Garbage collector extension
    virtual void                    GarbageCollect( lua_State *L );

    // Special MTA:Lua events
    virtual bool                    OnLuaClassDeallocationFail( lua_State *L, ILuaClass *j );

    lua_State*                      GetVirtualMachine() const   { return m_lua; }

    // Current execution information
    const LuaMain*                  GetStatus( int *line = NULL, std::string *src = NULL, std::string *proto_name = NULL );
    unsigned long                   GetRuntime();

    // Provide your own creation function
    void                            Init( LuaMain *lua );
    LuaMain*                        Get( lua_State *lua );
    virtual bool                    Remove( LuaMain *lua );

    // Global referencing functions - use with caution
    void                            PushReference( lua_State *L, const LuaFunctionRef& ref );

    void                            PushThread( lua_State *thread )     { m_threadStack.push_back( thread ); }
    lua_State*                      GetThread() const                   { return m_threadStack.empty() ? m_lua : m_threadStack.back(); }
    lua_State*                      PopThread()
    {
        if ( m_threadStack.empty() )
            return NULL;

        lua_State *thread = m_threadStack.back();

        m_threadStack.pop_back();
        return thread;
    }

    inline std::list <LuaMain*>::const_iterator IterBegin()     { return m_structures.begin(); }
    inline std::list <LuaMain*>::const_iterator IterEnd()       { return m_structures.end(); }

    void                            DoPulse();
    void                            InstructionCountHook( lua_State *L, lua_Debug *ar );

    static ResourceManager*         m_resMan;

private:
    void                            InitSecurity();

protected:
    void                            CallStack( int args );
    void                            CallStackVoid( int args );
    bool                            PCallStack( int args );
    bool                            PCallStackVoid( int args );

    class env_status
    {
        inline void Update( int *line, std::string *src, std::string *proto_name ) const
        {
            lua_Debug info;

            // Gather main information
            if ( lua_getstack( &m_lua, 1, &info ) && lua_getinfo( &m_lua, "nlS", &info ) )
            {
                if ( line )
                    *line = info.currentline;

                if ( src )
                    *src = info.short_src;

                if ( proto_name )
                {
                    if ( !info.name )
                        proto_name->clear();
                    else
                        *proto_name = info.name;
                }
            }
            else if ( line )
                *line = -1;
        }

    public:
        env_status( lua_State& lua, const LuaMain& context ) : m_env( context ), m_lua( lua )
        {
            m_frozen = false;
            m_functionEnter = std::chrono::system_clock::now();
        }

        const LuaMain&    Get( int *line, std::string *src, std::string *proto_name ) const
        {
            if ( !m_frozen )
                Update( line, src, proto_name );
            else
            {
                if ( line )
                    *line = m_line;

                if ( src )
                    *src = m_src;

                if ( proto_name )
                    *proto_name = m_proto_name;
            }

            return m_env;
        }

    private:
        inline std::chrono::milliseconds CalcRuntime( void ) const
        {
            return std::chrono::duration_cast <std::chrono::milliseconds> ( ( std::chrono::system_clock::now() - m_functionEnter ) );
        }

    public:
        unsigned long   GetRuntime()
        {
            if ( m_frozen )
                return (unsigned long)m_runtime.count();

            return (unsigned long)( CalcRuntime().count() );
        }

        void    Finalize()
        {
            if ( m_frozen )
                return;

            Update( &m_line, &m_src, &m_proto_name );

            m_frozen = true;
            m_runtime = CalcRuntime();
        }

        void    Resume()
        {
            m_frozen = false;
            m_functionEnter = ( std::chrono::system_clock::now() - m_runtime );   // Optimization!
        }

    private:
        int m_line;
        std::string m_src;
        std::string m_proto_name;
        const LuaMain& m_env;
        std::chrono::milliseconds m_runtime;
        std::chrono::system_clock::time_point m_functionEnter;

        bool m_frozen;
        lua_State& m_lua;
    };

public:
    // Inline stack utility
    class context
    {
        friend class LuaManager;

        context( LuaMain& context, LuaManager& system ) : m_context( context ), m_system( system )
        {
            m_lua = *m_context;

            system.PushStatus( context );
            system.PushThread( *context );
        }

        LuaMain& m_context;
        LuaManager& m_system;
        lua_State* m_lua;

    public:
        ~context()
        {
            m_system.PopStatus();
            m_system.PopThread();
        }

        inline void CallStack( int args )
        {
            m_system.CallStack( args );
        }

        inline void CallStackVoid( int args )
        {
            m_system.CallStack( args );
        }

        inline bool PCallStack( int args )
        {
            return m_system.PCallStack( args );
        }

        inline bool PCallStackVoid( int args )
        {
            return m_system.PCallStackVoid( args );
        }

        inline lua_State* GetLuaState( void )
        {
            return this->m_lua;
        }
    };

    friend class context;

    AINLINE context                 AcquireContext( LuaMain& env )  { return context( env, *this ); }
protected:
    void                            PushStatus( const LuaMain& main );
    const LuaMain*                  PopStatus( int *line = NULL, std::string *src = NULL, std::string *proto = NULL );

    std::list <env_status>          m_envStack;

    lua_config*                     m_luaConfig;

    lua_State*                      m_lua;

    std::list <lua_State*>          m_threadStack;

    class Events&                   m_events;
    ScriptDebugging&                m_debug;
    std::list <LuaMain*>            m_structures;

	RwList <LuaClass>				m_gcList;

    void*                           m_privateAllocator;
};

// quick macros
#define lua_getmanager( L ) (lua_rawgeti( L, LUA_STORAGEINDEX, 1 ))
#define lua_getcontext( L ) (lua_rawgeti( L, LUA_STORAGEINDEX, 2 ))

// global inline definition
extern lua_State *g_L;
extern LuaManager *g_luaManager;

#endif //_BASE_LUA_MANAGER_

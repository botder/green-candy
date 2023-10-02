/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArgument.cpp
*  PURPOSE:     Lua argument handler class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

using namespace std;


CLuaArgument::CLuaArgument( CElement *element )
{
    m_table = NULL;
    Read( element );
}

void CLuaArgument::Clone() const
{
    return new CLuaArgument( *this );
}

void CLuaArgument::ReadElement( CElement *element )
{
    m_string = "";

    DeleteTableData();

    if ( !element )
    {
        m_type = LUA_TNIL;
        return;
    }

    m_type = LUA_TLIGHTUSERDATA;
    m_lightUD = (void*)element->GetID().Value();
}

CElement* CLuaArgument::GetElement() const
{
    return CElementIDs::GetElement( TO_ELEMENTID( m_lightUD ) );
}

bool CLuaArgument::ReadTypeFromBitStream( NetBitStreamInterface& stream, int type )
{
    switch( type )
    {
    // Element type
    case LUA_TLIGHTUSERDATA:
        {
            ElementID id;

            if ( !bitStream.Read( id ) )
                return false;

            Read( CElementIDs::GetElement( id ) );
        }
        return true;
    }
    return LuaArgument::ReadTypeFromBitStream( stream, type );
}

bool CLuaArgument::WriteToBitStream( NetBitStreamInterface& stream ) const
{
    SLuaTypeSync type;

    switch( m_type )
    {
    // Element argument
    case LUA_TLIGHTUSERDATA:
        CElement *element = (CElement*)m_lightUD;

        if ( !element || element->GetID() == INVALID_ELEMENT_ID )
        {
            // Write a nil though so other side won't get out of sync
            type.data.ucType = LUA_TNIL;
            stream.Write( &type );
            return false;
        }

        // Write its ID
        type.data.ucType = LUA_TLIGHTUSERDATA;
        stream.Write( &type );
        stream.Write( element->GetID() );
        return true;
    }

    return LuaArgument::WriteToBitStream( stream );
}

// TODO: do the path logic?
void CLuaArgument::LogUnableToPacketize( const char *msg ) const
{
    if ( m_path.empty() )
    {
        g_pGame->GetScriptDebugging()->LogWarning( "Unknown: %s\n", msg );
        return;
    }

    g_pGame->GetScriptDebugging()->LogWarning( "%s:%d: %s\n", m_path.c_str(), m_line, msg );
}
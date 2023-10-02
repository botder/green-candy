/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
*  PURPOSE:     Lua argument handler class
*  DEVELOPERS:  Ed Lyons <>
*               Christian Myhre Lundheim <>
*               Jax <>
*               The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAARGUMENT_H
#define __CLUAARGUMENT_H

#include "../common/CBitStream.h"
#include "json.h"

class CElement;
class CLuaArguments;

class CLuaArgument : public LuaArgument
{
public:
                            CLuaArgument( bool b ) : LuaArgument( b )   {}
                            CLuaArgument( double d ) : LuaArgument( d ) {}
                            CLuaArgument( const std::string& str ) : LuaArgument( str ) {}
                            CLuaArgument( void *ud ) : LuaArgument( ud )    {}
                            CLuaArgument( const LuaArgument& arg ) : LuaArgument( arg ) {}
                            CLuaArgument( NetBitStreamInterface& bitStream ) : LuaArgument( bitStream ) {}
                            CLuaArgument( lua_State *lua, int idx ) : LuaArgument( lua, idx )   {}
                            CLuaArgument( CElement* pElement );
                            ~CLuaArgument();

    LuaArgument*            Clone() const;

    void                    ReadElement( CElement* pElement );
    CElement*               GetElement() const;

    bool                    WriteToBitStream( NetBitStreamInterface& bitStream ) const;
    json_object*            WriteToJSONObject( bool serialize = false );
    bool                    ReadFromJSONObject( json_object* object );

    void                    LogUnableToPacketize( const char *msg ) const;

protected:
    bool                    ReadTypeFromBitStream( NetBitStreamInterface& stream, int type );
};

#endif

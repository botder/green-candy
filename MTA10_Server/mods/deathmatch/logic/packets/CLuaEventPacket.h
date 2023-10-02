/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLuaEventPacket.h
*  PURPOSE:     Lua event packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CLUAEVENTPACKET_H
#define __PACKETS_CLUAEVENTPACKET_H

#include "CPacket.h"
#include "../lua/CLuaArguments.h"

class CLuaEventPacket : public CPacket
{
public:
                                    CLuaEventPacket();
                                    CLuaEventPacket( const char* szName, ElementID ID, CLuaArguments& Arguments );

    inline ePacketID                GetPacketID() const                                 { return PACKET_ID_LUA_EVENT; }
    inline unsigned long            GetFlags() const                                    { return PACKET_RELIABLE | PACKET_SEQUENCED; }

    bool                            Read( NetBitStreamInterface& BitStream );
    bool                            Write( NetBitStreamInterface& BitStream ) const;

    inline const char*              GetName() const                                     { return m_szName; }
    inline ElementID                GetElementID() const                                { return m_ElementID; }
    inline CLuaArguments&           GetArguments()                                      { return m_Arguments; }

private:
    char                            m_szName[MAX_EVENT_NAME_LENGTH];
    ElementID                       m_ElementID;
    CLuaArguments                   m_Arguments;
};

#endif

/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/Common.h
*  PURPOSE:     Header for common definitions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

// Min and max number of characters in player serial
#define MIN_SERIAL_LENGTH 1
#define MAX_SERIAL_LENGTH 32

// Network disconnection reason (message) size
#define NET_DISCONNECT_REASON_SIZE  256

// Element IDs
#define RESERVED_ELEMENT_ID 0xFFFFFFFE
#define INVALID_ELEMENT_ID 0xFFFFFFFF

// Element name characteristics
#define MAX_TYPENAME_LENGTH 32
#define MAX_ELEMENT_NAME_LENGTH 64

// Allow 2^17 server elements and 2^17 client elements
#define MAX_SERVER_ELEMENTS 131072
#define MAX_CLIENT_ELEMENTS 131072

#if (MAX_SERVER_ELEMENTS&(MAX_SERVER_ELEMENTS-1)) != 0
    #error MAX_SERVER_ELEMENTS "Macro must be power of 2"
#endif

// ElementID structure
struct ElementID
{
public:
    ElementID ( const unsigned int& value = INVALID_ELEMENT_ID ) : m_value(value) {}
    ElementID& operator= ( const unsigned int& value ) { m_value = value; return *this; }
    bool operator== ( const ElementID& ID ) const { return m_value == ID.m_value; }
    bool operator!= ( const ElementID& ID ) const { return m_value != ID.m_value; }
    bool operator> ( const ElementID& ID ) const { return m_value > ID.m_value; }
    bool operator>= ( const ElementID& ID ) const { return m_value >= ID.m_value; }
    bool operator< ( const ElementID& ID ) const { return m_value < ID.m_value; }
    bool operator<= ( const ElementID& ID ) const { return m_value <= ID.m_value; }
    ElementID& operator+= ( const ElementID& ID ) { m_value += ID.m_value; return *this; }
    ElementID& operator-= ( const ElementID& ID ) { m_value += ID.m_value; return *this; }
    ElementID operator+ ( const ElementID& ID ) const { return m_value + ID.m_value; }
    ElementID operator- ( const ElementID& ID ) const { return m_value - ID.m_value; }
    ElementID operator++ ( int ) { ElementID ret ( m_value ); ++m_value; return ret; }
    ElementID& operator++ () { ++m_value; return *this; }
    unsigned int& Value () { return m_value; }
    const unsigned int& Value () const { return m_value; }
private:
    unsigned int m_value;
};

// Event name characteristics
#define MAX_EVENT_NAME_LENGTH 512

// LAN packet broadcasting port
#define SERVER_LIST_BROADCAST_PORT          34219

// Server game to query port offset (gameport + offset = queryport)
#define SERVER_LIST_QUERY_PORT_OFFSET       123
#define SERVER_LIST_QUERY_PORT_OFFSET_LAN   234

// LAN packet broadcasting string
#define SERVER_LIST_CLIENT_BROADCAST_STR    "MTA-CLIENT"
#define SERVER_LIST_SERVER_BROADCAST_STR    "MTA-SERVER"

// Defines the min/max size for the player nick for use in the core module
#define MIN_PLAYER_NICK_LENGTH          1
#define MAX_PLAYER_NICK_LENGTH          22

// Windows Specific stuff
#ifdef _WIN32
#define _DECLSPEC_EX extern "C" _declspec(dllexport) 
#else
#define _DECLSPEC_EX extern "C" 
#endif

#include <float.h>

// High precision math wrap.
// Use it if you are encountering floating point precision issues.
// This wrap is used in timing critical code.
struct HighPrecisionMathWrap
{
    unsigned int _oldFPUVal;

    inline HighPrecisionMathWrap( void )
    {
        _oldFPUVal = _controlfp( 0, 0 );

        _controlfp( _PC_64, _MCW_PC );
    }

    inline ~HighPrecisionMathWrap( void )
    {
        _controlfp( _oldFPUVal, _MCW_PC );
    }
};
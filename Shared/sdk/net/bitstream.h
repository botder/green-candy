/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/net/bitstream.h
*  PURPOSE:     Network bitstream interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once

#include "Common.h"
#include "../Common.h"
#include <string>
#ifndef WIN32
    #include <alloca.h>
#endif

struct ISyncStructure;

class NetBitStreamInterface : public CRefCountable
{
    NetBitStreamInterface      ( const NetBitStreamInterface& );
    const NetBitStreamInterface& operator= ( const NetBitStreamInterface& );
protected:
                        NetBitStreamInterface       ( void ) {}
    virtual             ~NetBitStreamInterface      ( void ) {}
public:
    virtual int         GetReadOffsetAsBits         ( void ) = 0;

    virtual void        Reset                       ( void ) = 0;
    virtual void        ResetReadPointer            ( void ) = 0;

    // Don't use this, it screws up randomly in certain conditions causing packet misalign
    virtual void        Write                       ( const unsigned char& input ) = 0;
    virtual void        Write                       ( const char& input ) = 0;
    virtual void        Write                       ( const unsigned short& input ) = 0;
    virtual void        Write                       ( const short& input ) = 0;
    virtual void        Write                       ( const unsigned int& input ) = 0;
    virtual void        Write                       ( const int& input ) = 0;
    virtual void        Write                       ( const unsigned long& input ) = 0;
    virtual void        Write                       ( const long& input ) = 0;
    virtual void        Write                       ( const float& input ) = 0;
    virtual void        Write                       ( const double& input ) = 0;
    virtual void        Write                       ( const char* input, int numberOfBytes ) = 0;
    virtual void        Write                       ( const ISyncStructure* syncStruct ) = 0;

public:    // Use char functions only when they will be 0 most times
    virtual void        WriteCompressed             ( const unsigned char& input ) = 0;
    virtual void        WriteCompressed             ( const char& input ) = 0;
public:
    virtual void        WriteCompressed             ( const unsigned short& input ) = 0;
    virtual void        WriteCompressed             ( const short& input ) = 0;
    virtual void        WriteCompressed             ( const unsigned int& input ) = 0;
    virtual void        WriteCompressed             ( const int& input ) = 0;
    virtual void        WriteCompressed             ( const unsigned long& input ) = 0;
    virtual void        WriteCompressed             ( const long& input ) = 0;
private:    // Float functions not used because they only cover -1 to +1 and are lossy
    virtual void        WriteCompressed             ( const float& input ) = 0;
    virtual void        WriteCompressed             ( const double& input ) = 0;
public:
    virtual void        WriteBits                   ( const char* input, unsigned int numbits ) = 0;
    virtual void        WriteBit                    ( bool input ) = 0;

    // Write a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12 bytes.  Will further compress y or z axis aligned vectors. Accurate to 1/32767.5.
    virtual void        WriteNormVector             ( float x, float y, float z ) = 0;

    // Write a vector, using 10 bytes instead of 12. Loses accuracy to about 3/10ths and only saves 2 bytes, so only use if accuracy is not important.
    virtual void        WriteVector                 ( float x, float y, float z ) = 0;

    // Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
    virtual void        WriteNormQuat               ( float w, float x, float y, float z) = 0;

    // Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each for 6 bytes instead of 36
    virtual void        WriteOrthMatrix             (
                                                      float m00, float m01, float m02,
                                                      float m10, float m11, float m12,
                                                      float m20, float m21, float m22
                                                    ) = 0;

    virtual bool        Read                        ( unsigned char& output ) = 0;
    virtual bool        Read                        ( char& output ) = 0;
    virtual bool        Read                        ( unsigned short& output ) = 0;
    virtual bool        Read                        ( short& output ) = 0;
    virtual bool        Read                        ( unsigned int& output ) = 0;
    virtual bool        Read                        ( int& output ) = 0;
    virtual bool        Read                        ( unsigned long& output ) = 0;
    virtual bool        Read                        ( long& output ) = 0;
    virtual bool        Read                        ( float& output ) = 0;
    virtual bool        Read                        ( double& output ) = 0;
    virtual bool        Read                        ( char* output, int numberOfBytes ) = 0;
    virtual bool        Read                        ( ISyncStructure* syncStruct ) = 0;

public:    // Use char functions only when they will be 0 most times
    virtual bool        ReadCompressed              ( unsigned char& output ) = 0;
    virtual bool        ReadCompressed              ( char& output ) = 0;
public:
    virtual bool        ReadCompressed              ( unsigned short& output ) = 0;
    virtual bool        ReadCompressed              ( short& output ) = 0;
    virtual bool        ReadCompressed              ( unsigned int& output ) = 0;
    virtual bool        ReadCompressed              ( int& output ) = 0;
    virtual bool        ReadCompressed              ( unsigned long& output ) = 0;
    virtual bool        ReadCompressed              ( long& output ) = 0;
private:    // Float functions not used because they only cover -1 to +1 and are lossy
    virtual bool        ReadCompressed              ( float& output ) = 0;
    virtual bool        ReadCompressed              ( double& output ) = 0;
public:
    virtual bool        ReadBits                    ( char* output, unsigned int numbits ) = 0;
    virtual bool        ReadBit                     ( ) = 0;

    virtual bool        ReadNormVector              ( float &x, float &y, float &z ) = 0;
    virtual bool        ReadVector                  ( float &x, float &y, float &z ) = 0;
    virtual bool        ReadNormQuat                ( float &w, float &x, float &y, float &z) = 0;
    virtual bool        ReadOrthMatrix              (
                                                      float &m00, float &m01, float &m02,
                                                      float &m10, float &m11, float &m12,
                                                      float &m20, float &m21, float &m22
                                                    ) = 0;
    // GetNumberOfBitsUsed appears to round up to the next byte boundary, when reading
    virtual int         GetNumberOfBitsUsed         ( void ) const = 0;
    virtual int         GetNumberOfBytesUsed        ( void ) const = 0;
    // GetNumberOfUnreadBits appears to round up to the next byte boundary, when reading
    virtual int         GetNumberOfUnreadBits       ( void ) const = 0;

    // Helper template methods that are not actually part
    // of the interface but get inline compiled.

    template < typename T >
    inline void         WriteBits                   ( T * input, unsigned int numbits )
    {
        WriteBits ( reinterpret_cast < const char * > ( input ), numbits );
    }

    template < typename T >
    inline bool         ReadBits                    ( T * output, unsigned int numbits )
    {
        return ReadBits ( reinterpret_cast < char * > ( output ), numbits );
    }

    // Read a single bit
    bool                ReadBit                     ( bool& output )
    {
        unsigned char ucTemp = 0;
        if ( ReadBits ( &ucTemp, 1 ) )
        {
            output = ucTemp & 1;
            return true;
        }
        return false;
    }


    // Write characters from a std::string
    void WriteStringCharacters ( const std::string& value, unsigned short usLength )
    {
        // Send the data
        if ( usLength )
            Write ( &value.at ( 0 ), usLength );
    }

    // Read characters into a std::string
    bool ReadStringCharacters ( std::string& result, unsigned short usLength )
    {
        result = "";
        if ( usLength )
        {
            // Read the data
            char* buffer = static_cast < char* > ( alloca ( usLength ) );
            if ( !Read ( buffer, usLength ) )
                return false;

            result = std::string ( buffer, usLength );
        }
        return true;
    }
	
    void WriteStringCompressed( const std::string& str )
    {
        size_t strlen = str.size();

        assert( strlen < std::numeric_limits <unsigned short>::max() );

        unsigned short len = (unsigned short)strlen;

        WriteCompressed( len );

        WriteStringCharacters( str, len );
    }

    bool ReadStringCompressed( std::string& buf )
    {
        unsigned short len;

        if ( !ReadCompressed( len ) )
            return false;

        return ReadStringCharacters( buf, len );
    }


    // Write a string (incl. ushort size header)
    void WriteString ( const std::string& value )
    {
        size_t strlen = value.length();

        assert( strlen < std::numeric_limits <unsigned short>::max() );

        // Write the length
        unsigned short usLength = (unsigned short)strlen;
        Write ( usLength );

        // Write the characters
        return WriteStringCharacters ( value, usLength );
    }

    // Read a string (incl. ushort size header)
    bool ReadString ( std::string& result )
    {
        result = "";

        // Read the length
        unsigned short usLength = 0;
        if ( !Read ( usLength ) )
            return false;

        // Read the characters
        return ReadStringCharacters ( result, usLength );
    }

    #ifdef MTA_CLIENT
        #define MAX_ELEMENTS    MAX_CLIENT_ELEMENTS
    #else
        #define MAX_ELEMENTS    MAX_SERVER_ELEMENTS
    #endif

    // Write an element ID
    void Write(const ElementID& ID)
    {
        static const unsigned int bitcount = NumberOfSignificantBits < ( MAX_ELEMENTS - 1 ) >::COUNT;
        const unsigned int& IDref = ID.Value ();
        WriteBits ( reinterpret_cast < const unsigned char* > ( &IDref ), bitcount );
    }

    // Read an element ID
    bool Read(ElementID& ID)
    {
        static const unsigned int bitcount = NumberOfSignificantBits < ( MAX_ELEMENTS - 1 ) > ::COUNT;
        unsigned int& IDref = ID.Value ();
        IDref = 0;
        bool bResult = ReadBits ( reinterpret_cast < unsigned char* > ( &IDref ), bitcount );

        // If max value, change to INVALID_ELEMENT_ID
        static const unsigned int maxValue = ( 1 << bitcount ) - 1;
        if ( IDref == maxValue )
            IDref = INVALID_ELEMENT_ID;

        return bResult;
    }

    virtual unsigned short Version                  ( void ) const = 0;
};

// Interface for all sync structures
struct ISyncStructure
{
    virtual         ~ISyncStructure () {}
    virtual bool    Read            ( NetBitStreamInterface& bitStream ) = 0;
    virtual void    Write           ( NetBitStreamInterface& bitStream ) const = 0;
};

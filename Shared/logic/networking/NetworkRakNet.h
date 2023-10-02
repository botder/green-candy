/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/networking/NetworkStruct.h
*  PURPOSE:     RakNet network traversal extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _NETWORKING_RAKNET_
#define _NETWORKING_RAKNET_

namespace Networking
{
    class RakDataStream
    {
    public:
        RakDataStream( NetBitStreamInterface& stream ) : m_stream( stream )
        { }

        template <class type>
        inline NETWORK_NOEXCEPT void Write( const type& val )
        {
            m_stream.Write( val );
        }

        template <>
        inline NETWORK_NOEXCEPT void Write <CVector> ( const CVector& val )
        {
            m_stream.Write( val.fX );
            m_stream.Write( val.fY );
            m_stream.Write( val.fZ );
        }

        template <class type>
        inline NETWORK_NOEXCEPT type Read()
        {
            type val;
            m_stream.Read( val );
            return val;
        }

        template <>
        inline NETWORK_NOEXCEPT CVector Read <CVector> ()
        {
            float x, y, z;

            m_stream.Read( x );
            m_stream.Read( y );
            m_stream.Read( z );
            return CVector( x, y, z );
        }

    protected:
        NetBitStreamInterface&  m_stream;
    };

    class RakBitStream : public RakDataStream
    {
    public:
        RakBitStream( const NetworkDataType *type, NetBitStreamInterface& stream ) : RakDataStream( stream )
        {
        }

        inline NETWORK_NOEXCEPT void WriteBits( const unsigned int val, const size_t count )
        {
            m_stream.WriteBits( &val, count );
        }
    };
};

#endif //_NETWORKING_RAKNET_
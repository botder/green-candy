/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/logic/networking/NetworkStruct.h
*  PURPOSE:     Network traversal structure management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _NETWORK_STRUCT_
#define _NETWORK_STRUCT_

// 12.6.2013: now using template unrolled-loops for better optimization.

#ifdef _MSC_VER
#define NET_FAST            __forceinline __declspec(noalias)
#define NET_RESTRICT        __declspec(restrict)
#else
#define NET_FAST
#define NET_RESTRICT
#endif

// http://groups.google.ie/group/comp.lang.c/msg/706324f25e4a60b0?hl=en&
#define LOG2_8BIT(v)  (8 - 90/(((v)/4+14)|1) - 2/((v)/2+1))
#define LOG2_16BIT(v) (8*((v)>255) + LOG2_8BIT((v) >>8*((v)>255)))
#define LOG2_32BIT(v) \
    (16*((v)>65535L) + LOG2_16BIT((v)*1L >>16*((v)>65535L)))
#define LOG2_64BIT(v)\
    (32*((v)/2L>>31 > 0) \
     + LOG2_32BIT((v)*1L >>16*((v)/2L>>31 > 0) \
                         >>16*((v)/2L>>31 > 0)))

#define INT2BITS(x)     ( (unsigned char)LOG2_32BIT(x) + 1 )
#define BITSIZE(x)      ( INT2BITS(sizeof(x)) )
#define ETSIZE(x)       ( sizeof(x) / sizeof(*x) )

#include <CVector.h>

template <size_t count, class loopFunc>
struct ForceLoopUnrollCount
{
    template <size_t n>
    NET_FAST void Performator( loopFunc&& func )
    {
        func.Exec( count - n );

        if constexpr ( n > 0 )
        {
            Performator <n - 1> ( std::move( func ) );
        }
    }

    NET_FAST ForceLoopUnrollCount( loopFunc&& func )
    {
        Performator <count> ( std::move( func ) );
    }
};

namespace Networking
{
    // exceptions
    class bad_streamer_seek
    { };

    enum eNetworkDataType
    {
        NETWORK_NONE,
        NETWORK_BYTE,
        NETWORK_WORD,
        NETWORK_DWORD,
        NETWORK_FLOAT,
        NETWORK_DOUBLE,
        NETWORK_VECTOR3D,
        NETWORK_BOOL
    };

#define NETWORK_NULL_DATA \
    template <class type> NET_FAST void     NetworkRead( const unsigned int n, type& val ) const \
    { \
        val = type(); \
    } \
    template <class type> NET_FAST void     NetworkWrite( const unsigned int n, const type& val )          { return; }

    struct NetworkDataType
    {
        const eNetworkDataType  type;
        const char* const       name;
    };

    template <size_t defStructSize>
    class DefaultDataStreamer
    {
        NET_FAST void ForwardID() noexcept
        {
            if ( INT2BITS(defStructSize) <= 8 )
                Forward <char> ();
            else if ( INT2BITS(defStructSize) <= 16 )
                Forward <short> ();
            else
                Forward <int> ();
        }

    public:
        template <class type>
        NET_FAST void Forward( const unsigned int count = 1 ) noexcept
        {
            (type*&)m_seek += count;
        }

        NET_FAST DefaultDataStreamer( size_t size ) noexcept
        {
            m_seek = m_instance = new char [ size ];
        }

        struct DataStreamerLoop
        {
            NET_FAST DataStreamerLoop( const NetworkDataType *const def, DefaultDataStreamer& streamer ) : m_def( def ), m_streamer( streamer )
            { }

            NET_FAST void Exec( const unsigned int n )
            {
                const NetworkDataType& def = m_def[n];

                if ( def.type == NETWORK_BOOL )            m_streamer.Forward <bool> ();
                else if ( def.type == NETWORK_WORD )       m_streamer.Forward <char> ();
                else if ( def.type == NETWORK_DWORD )      m_streamer.Forward <short> ();
                else if ( def.type == NETWORK_FLOAT )      m_streamer.Forward <int> ();
                else if ( def.type == NETWORK_DOUBLE )     m_streamer.Forward <double> ();
                else if ( def.type == NETWORK_VECTOR3D )   m_streamer.Forward <CVector> ();

                m_streamer.ForwardID();
            }

            const NetworkDataType *const m_def;
            DefaultDataStreamer& m_streamer;
        };

        NET_FAST DefaultDataStreamer( const NetworkDataType *const def ) noexcept
        {
            m_seek = 0;

            // First count for changes
            ForwardID();

            ForceLoopUnrollCount <defStructSize, DataStreamerLoop> ( DataStreamerLoop( def, *this ) );

            m_seek = m_instance = new char [ (size_t)(const char*)m_seek ];
        }

        NET_FAST ~DefaultDataStreamer()
        {
            delete (char*)m_instance;
        }

        NET_FAST void Reset() noexcept
        {
            m_seek = m_instance;
        }

        template <class type>
        NET_FAST void Write( const type& val ) noexcept
        {
            *(type*)m_seek = val;
            Forward <type> ();
        }

        template <class type>
        NET_FAST const type& Get() noexcept
        {
            return *(type*)m_seek;
        }

        template <class type>
        NET_FAST const type& Read() noexcept
        {
            const type& val = Get <type> ();
            Forward <type> ();
            return val;
        }

    protected:
        void*   m_instance;
        void*   m_seek;
    };

    class NullSyncStruct
    {
    public:
        template <class type>
        NET_FAST const type& Acknowledge( const unsigned int, const type&, const type& next ) noexcept
        {
            return next;
        }
    };

    template <class Owner, const size_t defStructSize, class DataStreamer = DefaultDataStreamer <defStructSize>, class Syncer = NullSyncStruct>
    class NetworkStruct
    {
    public:
        NET_FAST NetworkStruct( const NetworkDataType *const def ) : m_streamer( def )
        { }

        NET_FAST NetworkStruct( DataStreamer streamer ) : m_streamer( streamer )
        { }

        template <typename SyncerEx>
        struct StreamerWriteLoop
        {
            NET_FAST StreamerWriteLoop( const NetworkDataType *const def, DataStreamer& ds, const Owner& instance, SyncerEx& sync )
                : m_def( def ), m_instance( instance ), m_streamer( ds ), m_syncer( sync )
            { }

            template <class type>
            NET_FAST void Writer( const unsigned int n )
            {
                type data;
                this->m_instance.NetworkRead( n, data );
                this->m_streamer.Write( this->m_syncer.template Acknowledge <type> ( n, this->m_streamer.template Get <type> (), data ) );
            }

            NET_FAST void Exec( const unsigned int n )
            {
                const NetworkDataType *const seek = &m_def[n];

                switch( seek->type )
                {
                case NETWORK_BOOL:      this->Writer <bool> ( n ); break;
                case NETWORK_BYTE:      this->Writer <char> ( n ); break;
                case NETWORK_WORD:      this->Writer <short> ( n ); break;
                case NETWORK_DWORD:     this->Writer <int> ( n ); break;
                case NETWORK_FLOAT:     this->Writer <float> ( n ); break;
                case NETWORK_DOUBLE:    this->Writer <double> ( n ); break;
                case NETWORK_VECTOR3D:  this->Writer <CVector> ( n ); break;
                default:
                    break;
                }
            }

            const NetworkDataType *const m_def;
            const Owner& m_instance;
            DataStreamer& m_streamer;
            SyncerEx& m_syncer;
        };

        template <class SyncerEx>
        NET_FAST void Write( const NetworkDataType *def, const Owner& instance, SyncerEx& syncer ) noexcept
        {
            m_streamer.Reset();

            typedef StreamerWriteLoop <SyncerEx> LoopDef;

            ForceLoopUnrollCount <defStructSize, LoopDef> ( LoopDef( def, m_streamer, instance, syncer ) );
        }

        struct StreamerReadLoop
        {
            NET_FAST StreamerReadLoop( const NetworkDataType *const def, DataStreamer& ds, Owner& instance ) : m_def( def ), m_instance( instance ), m_streamer( ds )
            { }

            NET_FAST void Exec( const unsigned int n )
            {
                const NetworkDataType *const seek = &m_def[n];

                switch( seek->type )
                {
                case NETWORK_BOOL:      m_instance.NetworkWrite( n, (bool)m_streamer.template Read <bool> () ); break;
                case NETWORK_BYTE:      m_instance.NetworkWrite( n, (char)m_streamer.template Read <char> () ); break;
                case NETWORK_WORD:      m_instance.NetworkWrite( n, (short)m_streamer.template Read <short> () ); break;
                case NETWORK_DWORD:     m_instance.NetworkWrite( n, (int)m_streamer.template Read <int> () ); break;
                case NETWORK_FLOAT:     m_instance.NetworkWrite( n, (float)m_streamer.template Read <float> () ); break;
                case NETWORK_DOUBLE:    m_instance.NetworkWrite( n, (double)m_streamer.template Read <double> () ); break;
                case NETWORK_VECTOR3D:  m_instance.NetworkWrite( n, (CVector)m_streamer.template Read <CVector> () ); break;
                }
            }

            const NetworkDataType *const m_def;
            Owner& m_instance;
            DataStreamer& m_streamer;
        };

        NET_FAST void Read( const NetworkDataType *def, Owner& instance ) noexcept
        {
            m_streamer.Reset();

            ForceLoopUnrollCount <defStructSize, StreamerReadLoop> ( StreamerReadLoop( def, m_streamer, instance ) );
        }

    protected:
        DataStreamer    m_streamer;
    };

    template <size_t defStructSize>
    class DefaultBitStreamer : public DefaultDataStreamer <defStructSize>
    {
    public:
        NET_FAST DefaultBitStreamer( const NetworkDataType *const def ) : DefaultDataStreamer <defStructSize> ( def )
        {

        }

        NET_FAST void Wake() noexcept {}
        NET_FAST void Finalize() noexcept {}

        NET_FAST void WriteBits( const unsigned int val, const size_t bits ) noexcept
        {
            if ( bits <= 8 )
            {
                this->template Write <char> ( (char)val );
            }
            else if ( bits <= 16 )
            {
                this->template Write <short> ( (short)val );
            }
            else
                this->template Write <int> ( (int)val );
        }

        NET_FAST unsigned int ReadBits( const size_t bits ) noexcept
        {
            if ( bits <= 8 )
            {
                return this->template Read <char> ();
            }
            else if ( bits <= 16 )
            {
                return this->template Read <short> ();
            }
            else
            {
                return this->template Read <int> ();
            }
        }

        NET_FAST bool IsWritten() noexcept
        {
            return ( this->m_seek != this->m_instance );
        }
    };

    template <const size_t defEntryCount, class BitStreamer = DefaultBitStreamer <defEntryCount>>
    class BitwiseSyncStruct
    {
    private:
        NET_FAST BitwiseSyncStruct( const BitwiseSyncStruct& other )
        { }

    public:
        typedef BitStreamer streamType;

        NET_FAST BitwiseSyncStruct( streamType& stream ) : m_stream( stream )
        {
        }

        NET_FAST ~BitwiseSyncStruct()
        {
            if ( m_instructs.empty() )
                return;

            m_stream.Wake();
            m_stream.WriteBits( (unsigned int)m_instructs.size(), INT2BITS(defEntryCount) );

            for ( auto iter = m_instructs.begin(); iter != m_instructs.end(); ++iter )
            {
                (*iter)->Write( m_stream );
                delete *iter;
            }

            m_stream.Finalize();
        }

        class virtual_instruct
        {
        public:
            NET_FAST virtual_instruct( const unsigned int id ) : m_id( id )  {}
            virtual ~virtual_instruct() {}

            virtual void Write( streamType& out ) const noexcept = 0;

        protected:
            const unsigned int m_id;
        };

        template <class type>
        class instruct : public virtual_instruct
        {
        public:
            instruct( const unsigned int id, const type& t ) noexcept : virtual_instruct( id ), m_val( t )
            { }

            void Write( streamType& out ) const noexcept
            {
                out.WriteBits( this->m_id, INT2BITS( defEntryCount ) );
                out.template Write <type> ( this->m_val );
            }

        private:
            type    m_val;
        };

        template <class type>
        NET_FAST const type& Acknowledge( const unsigned int id, const type& previous, const type& next ) noexcept
        {
            if ( previous != next )
            {
                m_instructs.push_back( new instruct <type> ( id, next ) );
            }

            return next;
        }

    private:
        typedef std::vector <virtual_instruct*> instList_t;

        instList_t      m_instructs;
        streamType&     m_stream;
    };

    template <class Owner, size_t defStructSize, class BitStreamer = DefaultBitStreamer <defStructSize>, class DataStreamer = DefaultDataStreamer <defStructSize>, class Syncer = BitwiseSyncStruct <defStructSize, BitStreamer>>
    class NetworkSyncStruct : public NetworkStruct <Owner, defStructSize, DataStreamer, Syncer>
    {
        typedef NetworkStruct <Owner, defStructSize, DataStreamer, Syncer> NetSyncBase;

    public:
        typedef BitStreamer streamType;
        typedef Syncer syncType;

        NET_FAST NetworkSyncStruct( const NetworkDataType *const def ) : NetSyncBase( def )
        {
        }

        NET_FAST void Set( const NetworkDataType *def, const Owner& instance ) noexcept
        {
            NullSyncStruct syncer;

            NetSyncBase::Write( def, instance, syncer );
        }

        NET_FAST void Write( const NetworkDataType *def, const Owner& instance, streamType& stream ) noexcept
        {
            syncType syncer( stream );

            NetSyncBase::Write( def, instance, syncer );
        }

        NET_FAST void Establish( const NetworkDataType *def, Owner& instance, streamType& stream ) noexcept
        {
            size_t chgcnt = stream.ReadBits( INT2BITS(defStructSize) );

            while ( chgcnt )
            {
                unsigned int id = stream.ReadBits( INT2BITS(defStructSize) );
                const NetworkDataType& type = def[id];

                switch( type.type )
                {
                case NETWORK_BOOL:      instance.NetworkWrite( id, (bool)stream.template Read <bool> () ); break;
                case NETWORK_BYTE:      instance.NetworkWrite( id, (char)stream.template Read <char> () ); break;
                case NETWORK_WORD:      instance.NetworkWrite( id, (short)stream.template Read <short> () ); break;
                case NETWORK_DWORD:     instance.NetworkWrite( id, (int)stream.template Read <int> () ); break;
                case NETWORK_FLOAT:     instance.NetworkWrite( id, (float)stream.template Read <float> () ); break;
                case NETWORK_DOUBLE:    instance.NetworkWrite( id, (double)stream.template Read <double> () ); break;
                case NETWORK_VECTOR3D:  instance.NetworkWrite( id, (CVector)stream.template Read <CVector> () ); break;
                default:
                    break;
                }

                chgcnt--;
            }
        }
    };
}

#endif //_NETWORK_STRUCT_

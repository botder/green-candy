/*****************************************************************************
*
*  PROJECT:     Eir SDK
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        eirrepo/sdk/BitManage.h
*  PURPOSE:     Low-level bit manipulation acceleration helpers
*
*  Find the Eir SDK at: https://osdn.net/projects/eirrepo/
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _BIT_MANAGE_HEADER_
#define _BIT_MANAGE_HEADER_

#include <stdint.h>
#include <stdlib.h> // for size_t

#include "MemoryRaw.h"
#include "Endian.h"
#include "MetaHelpers.h"    // for no_volatile
#include "PlatformStrategy.h"
#include "Variant.h"

// Code in this file will be used in compilers that do not yet support C++20 thus we must not make
// use of advanced language design features such as C++20 concepts.

// Due to no C++20 support for A LOT of the compilers targetting old MCUs I cannot really unfold the full potential in below code graph templates.

namespace eir
{

struct invalid_memory_request_exception {};

#ifdef __cpp_if_constexpr
#define _BITMANAGE_IF_CONSTEXPR constexpr
#else
#define _BITMANAGE_IF_CONSTEXPR
#endif

template <typename numberType>
struct identity_number_specificator
{
    typedef numberType type;
};

template <typename numberType>
struct volatile_number_specificator
{
    typedef volatile numberType type;
};

template <typename T>
struct choose_default_number_specificator
{
    template <typename Y>
    using spec = identity_number_specificator <Y>;
};
template <typename T>
struct choose_default_number_specificator <volatile T>
{
    template <typename Y>
    using spec = volatile_number_specificator <Y>;
};

typedef CustomTypeSelector <
    eTypeIdentification::BY_SIZE,
#if PLATFORM_WORDSIZE >= 8
    uint64_t,
#endif
#if PLATFORM_WORDSIZE >= 4
    uint32_t,
#endif
#if PLATFORM_WORDSIZE >= 2
    uint16_t,
#endif
    uint8_t
> PlatformNumberTypeSelector;

// Actually just a std::variant from C++20 but has to be implemented like this because old compilers are in the support-set!
struct PlatformNumberStorage
{
    AINLINE PlatformNumberStorage( void ) = default;
    AINLINE PlatformNumberStorage( const PlatformNumberStorage& right ) noexcept : seltype( right.seltype )
    {
        this->seltype.Switch(
            [&] <typename T> () LAINLINE
        {
            this->data.template Get <T> () = right.data.template Get <T> ();
        });
    }

    template <typename callbackType>
    AINLINE void PerformOn( callbackType&& cb )
    {
        this->seltype.Switch( 
            [&] <typename T> () LAINLINE
        {
            cb( this->data.template Get <T> () );
        });
    }
    
    template <typename callbackType>
    AINLINE void Select( callbackType&& cb )
    {
        bool executed = decltype(this->seltype)::ForEach(
            [&] <typename T> () LAINLINE
        {
            if (cb(this->data.template Get <T> ()))
            {
                this->seltype.template Select <T> ();
                return true;
            }
            return false;
        });
        if (executed == false)
        {
            this->seltype.Unselect();
        }
    }
    
    AINLINE uint8_t GetSize( void ) const noexcept
    {
        return (uint8_t)decltype(this->seltype)::GetSizeForIndex( this->seltype.GetIndex() );
    }

    AINLINE bool IsValid( void ) const noexcept { return this->seltype.HasSelection(); }

    template <typename T>
    AINLINE bool IsType( void ) const noexcept
    {
        return this->seltype.template IsSelected <T> ();
    }

    AINLINE void Invalidate( void ) noexcept
    {
        this->seltype.Unselect();
    }

    template <typename T>
    AINLINE void Store( const T& v ) noexcept
    {
        if constexpr ( decltype(this->seltype)::template Contains <T> () )
        {
            this->data.template Get <T> () = v;
            this->seltype.template Select <T> ();
        }
        else
        {
            this->seltype.Unselect();
        }
    }

    AINLINE void* GetStoragePointer( void ) noexcept
    {
        return this->data.GetStoragePointer();
    }

private:
    Union <PlatformNumberTypeSelector> data;
    PlatformNumberTypeSelector seltype;
};

template <typename T>
struct is_bitfield_view : public false_type {};
template <typename T>
struct plain_type_bitfield : public plain_type <T> {};

// Replaces bits inside of a single unsigned integer.
template <typename srcNumType, typename dstNumType>
AINLINE constexpr void replace_bits( dstNumType& dst, const srcNumType& src, platformLocalBitcountType bitoff, platformLocalBitcountType bitcnt ) noexcept;

// Extracts bits from a single unsigned integer.
template <typename numberType>
AINLINE constexpr typename plain_type_bitfield <numberType>::type extract_bits( const numberType& val, platformLocalBitcountType bitoff, platformLocalBitcountType bitcnt ) noexcept;

// numberType has to be unsigned integer.
template <typename numberType>
struct BitfieldNumberSelectionView
{
    AINLINE BitfieldNumberSelectionView( numberType *num, platformLocalBitcountType bitoff = 0u, platformLocalBitcountType bitcnt = sizeof(numberType)*8u ) noexcept
        : num( num ), bitoff( bitoff ), bitcnt( bitcnt )
    {
        return;
    }
    AINLINE BitfieldNumberSelectionView( const BitfieldNumberSelectionView& view, platformLocalBitcountType bitoff, platformLocalBitcountType bitcnt = sizeof(numberType)*8u ) noexcept
        : BitfieldNumberSelectionView( view.num, bitoff, bitcnt )
    {
        return;
    }
    AINLINE BitfieldNumberSelectionView( const BitfieldNumberSelectionView& ) = default;

    AINLINE void Write( typename plain_type <numberType>::type val ) noexcept
    {
        replace_bits( *num, val, this->bitoff, this->bitcnt );
    }

    AINLINE typename plain_type <numberType>::type Read( void ) const noexcept
    {
        return extract_bits( *num, this->bitoff, this->bitcnt );
    }

    AINLINE platformLocalBitcountType GetBitOffset( void ) const noexcept
    {
        return bitoff;
    }
    AINLINE platformLocalBitcountType GetBitCount( void ) const noexcept
    {
        return bitcnt;
    }

    // Helpers for nicety.
    AINLINE BitfieldNumberSelectionView& operator = ( typename plain_type <numberType>::type val ) noexcept
    {
        this->Write( val );
        return *this;
    }
    AINLINE operator typename plain_type <numberType>::type ( void ) const noexcept
    {
        return this->Read();
    }

    AINLINE platformLocalBitcountType GetRegionStart( void ) const noexcept
    {
        return this->bitoff;
    }
    AINLINE platformLocalBitcountType GetRegionEnd( void ) const noexcept
    {
        return this->bitoff + this->bitcnt;
    }

private:
    numberType *num;
    platformLocalBitcountType bitoff;
    platformLocalBitcountType bitcnt;
};

template <typename... args>
struct is_bitfield_view <BitfieldNumberSelectionView <args...>> : public true_type {};
template <typename... args>
struct is_bitfield_view <BitfieldNumberSelectionView <args...>&> : public true_type {};
template <typename... args>
struct is_bitfield_view <BitfieldNumberSelectionView <args...>&&> : public true_type {};

template <typename insideNumberType, typename numberType>
struct StaticBitfieldNumberSelectionView
{
    static constexpr platformLocalBitcountType bitcnt = sizeof(insideNumberType)*8u;

    AINLINE StaticBitfieldNumberSelectionView( numberType *num, platformLocalBitcountType bitoff = 0u ) noexcept
        : num( num ), bitoff( bitoff )
    {
        return;
    }
    AINLINE StaticBitfieldNumberSelectionView( const StaticBitfieldNumberSelectionView& ) = default;

    AINLINE void Write( typename plain_type <insideNumberType>::type val ) noexcept
    {
        replace_bits( *num, val, this->bitoff, bitcnt );
    }

    AINLINE typename plain_type <insideNumberType>::type Read( void ) const noexcept
    {
        return (typename plain_type <insideNumberType>::type)extract_bits( *num, this->bitoff, bitcnt );
    }

    AINLINE constexpr platformLocalBitcountType GetBitCount( void ) const noexcept
    {
        return bitcnt;
    }

    // Helpers for nicety.
    AINLINE StaticBitfieldNumberSelectionView& operator = ( typename plain_type <insideNumberType>::type val ) noexcept
    {
        this->Write( val );
        return *this;
    }
    AINLINE operator typename plain_type <insideNumberType>::type ( void ) const noexcept
    {
        return this->Read();
    }

    AINLINE platformLocalBitcountType GetRegionStart( void ) const noexcept
    {
        return this->bitoff;
    }
    AINLINE platformLocalBitcountType GetRegionEnd( void ) const noexcept
    {
        return this->bitoff + bitcnt;
    }

    template <typename T>
    AINLINE BitfieldNumberSelectionView <numberType>& SharedBitRegion( platformLocalBitcountType right_start, platformLocalBitcountType right_end ) noexcept
    {
        platformLocalBitcountType left_start = this->bitoff;
        platformLocalBitcountType left_end = left_start + bitcnt;

        return BitfieldNumberSelectionView( this->num, left_start < right_start ? left_start : right_start, left_end < right_end ? left_end : right_end );
    }

private:
    numberType *num;
    platformLocalBitcountType bitoff;
};

template <typename... args>
struct is_bitfield_view <StaticBitfieldNumberSelectionView <args...>> : public true_type {};
template <typename... args>
struct is_bitfield_view <StaticBitfieldNumberSelectionView <args...>&> : public true_type {};
template <typename... args>
struct is_bitfield_view <StaticBitfieldNumberSelectionView <args...>&&> : public true_type {};

// Helper to abstract away bitfield type checking.
// With C++20 we could write this much easier using conceptual restrictions.
template <typename IT, typename T>
struct plain_type_bitfield <StaticBitfieldNumberSelectionView <IT, T>> : public plain_type <IT> {};
template <typename IT, typename T>
struct plain_type_bitfield <StaticBitfieldNumberSelectionView <IT, T>&> : public plain_type <IT> {};
template <typename IT, typename T>
struct plain_type_bitfield <const StaticBitfieldNumberSelectionView <IT, T>&> : public plain_type <IT> {};
template <typename IT, typename T>
struct plain_type_bitfield <StaticBitfieldNumberSelectionView <IT, T>&&> : public plain_type <IT> {};
template <typename T>
struct plain_type_bitfield <BitfieldNumberSelectionView <T>> : public plain_type <T> {};
template <typename T>
struct plain_type_bitfield <BitfieldNumberSelectionView <T>&> : public plain_type <T> {};
template <typename T>
struct plain_type_bitfield <const BitfieldNumberSelectionView <T>&> : public plain_type <T> {};
template <typename T>
struct plain_type_bitfield <BitfieldNumberSelectionView <T>&&> : public plain_type <T> {};

template <typename numberType>
AINLINE constexpr platformLocalBitcountType get_bitcount( const numberType& v ) noexcept
{
    if constexpr ( is_bitfield_view <numberType>::value )
    {
        return v.GetBitCount();
    }
    else
    {
        return sizeof(numberType)*8u;
    }
}

template <typename srcNumType, typename dstNumType>
AINLINE constexpr void replace_bits( dstNumType& dst, const srcNumType& src, platformLocalBitcountType bitoff, platformLocalBitcountType bitcnt ) noexcept
{
    // Using C++20 concepts we could move that into a requires-clause.
    static_assert( is_unsigned_integral <typename plain_type_bitfield <srcNumType>::type>::value, "srcNumType has to be unsigned integral" );
    static_assert( is_unsigned_integral <typename plain_type_bitfield <dstNumType>::type>::value, "dstNumType has to be unsigned integral" );

    platformLocalBitcountType src_bitcount = get_bitcount( src );
    platformLocalBitcountType dst_bitcount = get_bitcount( dst );

    if ( bitcnt == dst_bitcount )
    {
        dst = src;
    }
    else
    {
        typedef typename plain_type_bitfield <srcNumType>::type srcFastNumType;
        typedef typename plain_type_bitfield <dstNumType>::type dstFastNumType;

        // In case of bitfield, do not modify the original value, as the bitfield is a view.
        srcFastNumType src_val = src;

        dstFastNumType dst_bitmask;
        if ( bitcnt < src_bitcount )
        {
            srcFastNumType bitmask = ((srcFastNumType)1u<<bitcnt)-1;
            src_val &= bitmask;
            dst_bitmask = bitmask;
        }
        else
        {
            dst_bitmask = ((dstFastNumType)1u<<bitcnt)-1;
        }
        dstFastNumType tmp;
        dstFastNumType dst_val;
#ifdef _PLATFORM_HAS_FAST_BITROTATE
        if constexpr ( __platform_number_supports_brotate <dstFastNumType>::value )
        {
            dst_val = (dstFastNumType)src_val;
            tmp = __platform_brotate_r( dst, bitoff );
        }
        else
        {
#endif
            dst_bitmask <<= bitoff;
            dst_val = ((dstFastNumType)src_val << bitoff);
            tmp = dst;
#ifdef _PLATFORM_HAS_FAST_BITROTATE
        }
#endif
        tmp &= ~dst_bitmask;
        tmp |= dst_val;
#ifdef _PLATFORM_HAS_FAST_BITROTATE
        if constexpr ( __platform_number_supports_brotate <dstFastNumType>::value )
        {
            dst = __platform_brotate_l( tmp, bitoff );
        }
        else
        {
#endif
            dst = tmp;
#ifdef _PLATFORM_HAS_FAST_BITROTATE
        }
#endif
    }
}

template <typename numberType>
AINLINE constexpr typename plain_type_bitfield <numberType>::type extract_bits( const numberType& val, platformLocalBitcountType bitoff, platformLocalBitcountType bitcnt ) noexcept
{
    // Using C++20 concepts we could move that into a requires-clause.
    static_assert( is_unsigned_integral <typename plain_type_bitfield <numberType>::type>::value, "srcNumType has to be unsigned integral" );

    if constexpr ( is_bitfield_view <numberType>::value )
    {
        return extract_bits( val.Read(), bitoff, bitcnt );
    }
    else
    {
#ifdef _PLATFORM_HAS_FAST_BITEXTRACT
        if constexpr ( __platform_number_supports_bitextract <numberType>::value )
        {
            return __platform_bitextract( val, bitoff, bitcnt );
        }
        else
        {
#endif
            if ( bitcnt == sizeof(numberType)*8u )
            {
                return val;
            }
            else
            {
                typedef typename no_volatile <numberType>::type fastNumberType;

                fastNumberType bitmask = ((fastNumberType)1u<<bitcnt)-1;
                fastNumberType tmp = val;
                tmp >>= bitoff;
                tmp &= bitmask;
                return tmp;
            }
#ifdef _PLATFORM_HAS_FAST_BITEXTRACT
        }
#endif
    }
}

// Similar to PlatformNumberStorage but internally only implements the biggest unsigned integer for storage.
// This type should be much simpler for code-gen.
// WARNING: this class is broken if endianness is concerned! Don't use it due to
// performance reasons.
struct PlatformNumberBitfieldStorage
{
    AINLINE PlatformNumberBitfieldStorage( void ) noexcept = default;
    AINLINE PlatformNumberBitfieldStorage( const PlatformNumberBitfieldStorage& ) = default;

    template <typename callbackType>
    AINLINE void PerformOn( callbackType&& cb )
    {
        if ( this->seltype.HasSelection() )
        {
            BitfieldNumberSelectionView bfacc( &this->data, 0u, this->GetSize()*8u );
            cb( bfacc );
        }
    }
    template <typename callbackType>
    AINLINE void PerformOnByType( callbackType&& cb )
    {
        this->seltype.Switch( 
            [&] <typename T> () LAINLINE
        {
            StaticBitfieldNumberSelectionView <T, decltype(this->data)> bfacc( &this->data );
            cb( bfacc );
        });
    }
    
#if 0
    template <typename callbackType>
    AINLINE void SelectByLoop( callbackType&& cb )
    {
        // Much simpler for code-gen than an unpack operator over n types.
        // The free compilers are not perfect, you know?
        constexpr PlatformNumberTypeSelector::typeid_t cnt_types = PlatformNumberTypeSelector::GetCount();

        for ( PlatformNumberTypeSelector::typeid_t n = 0; n < cnt_types; n++ )
        {
            BitfieldNumberSelectionView bfacc( &this->data, 0, (platformLocalBitcountType)( PlatformNumberTypeSelector::GetSizeForIndex(n)*8u ) );
            if (cb(bfacc))
            {
                this->seltype.SelectByIndex( n );
                return;
            }
        }
        this->seltype.Unselect();
    }
#endif
    template <typename callbackType>
    AINLINE void Select( callbackType&& cb )
    {
        bool executed = decltype(this->seltype)::ForEach(
            [&] <typename T> () LAINLINE
        {
            StaticBitfieldNumberSelectionView <T, decltype(this->data)> bfacc( &this->data, 0 );
            if (cb(bfacc))
            {
                this->seltype.template Select <T> ();
                return true;
            }
            return false;
        });
        if (executed == false)
        {
            this->seltype.Unselect();
        }
    }

    template <typename numberType>
    AINLINE numberType& GetTypeRef( uint8_t byteoff )
    {
        return *(numberType*)( (char*)&this->data + byteoff );
    }

    AINLINE uint8_t GetSize( void ) const noexcept
    {
        return (uint8_t)decltype(this->seltype)::GetSizeForIndex( this->seltype.GetIndex() );
    }

    AINLINE bool IsValid( void ) const noexcept { return this->seltype.HasSelection(); }

    template <typename T>
    AINLINE bool IsType( void ) const noexcept
    {
        return this->seltype.template IsSelected <T> ();
    }

    AINLINE void Invalidate( void ) noexcept
    {
        this->seltype.Unselect();
    }

    template <typename T>
    AINLINE void Store( const T& v ) noexcept
    {
        if constexpr ( decltype(this->seltype)::template Contains <T> () )
        {
            this->data = v;
            this->seltype.template Select <T> ();
        }
        else
        {
            this->seltype.Unselect();
        }
    }

private:
    typename biggest_type <PlatformNumberTypeSelector>::type data;
    PlatformNumberTypeSelector seltype;
};

struct PlatformNumberSolitaryStorage
{
    AINLINE PlatformNumberSolitaryStorage( void ) noexcept = default;
    AINLINE PlatformNumberSolitaryStorage( const PlatformNumberSolitaryStorage& ) = default;

    template <typename callbackType>
    AINLINE void PerformOn( callbackType&& cb, uint8_t bitoff = 0u )
    {
        if ( this->seltype.HasSelection() )
        {
            BitfieldNumberSelectionView bfacc( &this->data, bitoff, this->GetSize()*8u - bitoff );
            cb( bfacc );
        }
    }
    template <typename callbackType>
    AINLINE void PerformOnByType( callbackType&& cb )
    {
        this->seltype.Switch( 
            [&] <typename T> () LAINLINE
        {
            StaticBitfieldNumberSelectionView <T, decltype(this->data)> bfacc( &this->data );
            cb( bfacc );
        });
    }

    template <typename numberType, typename callbackType>
    AINLINE void ViewDivisionByType( callbackType&& cb, uint8_t off = 0u )
    {
        uint8_t sz = this->GetSize();

        if ( sz < off ) return;

        uint8_t cnt = ( ( sz - off ) / sizeof(numberType) );
        numberType *items = (numberType*)( (char*)&this->data + off );

        cb( items, cnt );
    }
    
#if 0
    template <typename callbackType>
    AINLINE void SelectByLoop( callbackType&& cb )
    {
        // Much simpler for code-gen than an unpack operator over n types.
        // The free compilers are not perfect, you know?
        constexpr PlatformNumberTypeSelector::typeid_t cnt_types = PlatformNumberTypeSelector::GetCount();

        for ( PlatformNumberTypeSelector::typeid_t n = 0; n < cnt_types; n++ )
        {
            decltype(this->data) data = 0u;
            BitfieldNumberSelectionView bfacc( &data, 0, (platformLocalBitcountType)( PlatformNumberTypeSelector::GetSizeForIndex(n)*8u ) );
            if (cb(bfacc))
            {
                this->data = data;
                this->seltype.SelectByIndex( n );
                return;
            }
        }
        this->seltype.Unselect();
    }
#endif
    template <typename callbackType>
    AINLINE void Select( callbackType&& cb )
    {
        bool executed = decltype(this->seltype)::ForEach(
            [&] <typename T> () LAINLINE
        {
            decltype(this->data) data = 0u;
            StaticBitfieldNumberSelectionView <T, decltype(this->data)> bfacc( &data, 0 );
            if (cb(bfacc))
            {
                this->data = data;
                this->seltype.template Select <T> ();
                return true;
            }
            return false;
        });
        if (executed == false)
        {
            this->seltype.Unselect();
        }
    }

    AINLINE uint8_t GetSize( void ) const noexcept
    {
        return (uint8_t)decltype(this->seltype)::GetSizeForIndex( this->seltype.GetIndex() );
    }

    AINLINE bool IsValid( void ) const noexcept { return this->seltype.HasSelection(); }

    template <typename T>
    AINLINE bool IsType( void ) const noexcept
    {
        return this->seltype.template IsSelected <T> ();
    }

    AINLINE void Invalidate( void ) noexcept
    {
        this->seltype.Unselect();
    }

    template <typename T>
    AINLINE void Store( const T& v ) noexcept
    {
        if constexpr ( decltype(this->seltype)::template Contains <T> () )
        {
            this->data = v;
            this->seltype.template Select <T> ();
        }
        else
        {
            this->seltype.Unselect();
        }
    }

private:
    typename biggest_type <PlatformNumberTypeSelector>::type data;
    PlatformNumberTypeSelector seltype;
};

template <template <typename numberType> typename numberSpecific = identity_number_specificator>
struct PlatformBitCache
{
    AINLINE PlatformBitCache( void ) = default;
    AINLINE PlatformBitCache( const PlatformBitCache& ) = default;

    AINLINE PlatformBitCache& operator = ( const PlatformBitCache& ) = default;

    AINLINE void Flush( void ) noexcept
    {
        this->data.PerformOnByType(
            [this]( auto& data ) LAINLINE
            {
                *(typename numberSpecific <typename plain_type_bitfield <decltype(data)>::type>::type*)this->data_pointer = data;
            }
        );

        this->data.Invalidate();
    }
    AINLINE void Invalidate( void ) noexcept
    {
        this->data.Invalidate();
    }
    AINLINE bool IsInitialized( void ) const noexcept { return this->data.IsValid(); }

    template <typename numberType>
    AINLINE void PutIntoCache( numberType& num ) noexcept
    {
        this->data.Store( num );
        this->data_pointer = &num;
    }

    AINLINE void CacheMemoryLocation( typename numberSpecific <void>::type *loc, size_t bytes_left, size_t alignment ) noexcept
    {
        this->data.Select(
            [&]( auto& data ) LAINLINE
            {
                typedef typename plain_type_bitfield <decltype(data)>::type numType;

                if (bytes_left >= sizeof(numType) && (alignment % alignof(numType) == 0 || (uintptr_t)loc % alignof(numType) == 0))
                {
                    data = *(typename numberSpecific <numType>::type*)loc;
                    this->data_pointer = loc;
                    return true;
                }
                return false;
            }
        );
    }

    template <typename numberType>
    AINLINE bool CanSatisfy( numberType& mem_val ) noexcept
    {
        uintptr_t numloc = (uintptr_t)&mem_val;
        uintptr_t cachenumloc = (uintptr_t)this->data_pointer;

        auto cache_size = this->data.GetSize();

        // Structures must always be aligned but I can check for that anyway.
        return /*(numloc % alignof(numberType) == 0) &&*/ (numloc >= cachenumloc && numloc + sizeof(numberType) <= cachenumloc + cache_size);
    }

    template <typename numberType, typename callbackType>
    AINLINE void SelectRef( typename numberSpecific <void>::type *orig_ptr, callbackType&& cb ) noexcept
    {
        const uintptr_t cachenumloc = (uintptr_t)this->data_pointer;

        auto& num = *(numberType*)( (char*)&this->data + ( (uintptr_t)orig_ptr - cachenumloc ) );

        cb( num );
    }

    template <typename numberType, typename callbackType>
    AINLINE void ViewDivision( numberType& mem_val, callbackType&& cb )
    {
        uintptr_t numloc = (uintptr_t)&mem_val;
        uintptr_t cachenumloc = (uintptr_t)this->data_pointer;

        uint8_t off = (uint8_t)(numloc - cachenumloc);

        this->data.ViewDivisionByType <typename no_volatile <numberType>::type> ( (callbackType&&)cb, off );
    }

    template <typename numberType, typename callbackType>
    AINLINE void SelectCachedNumber( numberType& mem_val, size_t bytes_left, callbackType&& cb )
    {
        // All number types have a power-of-two size.
        if (this->data.IsValid())
        {
            if ( this->template CanSatisfy <numberType> ( mem_val ) )
            {
                goto returnResult;
            }

            // We cannot use the previous cache anymore, so flush it.
            Flush();
        }

        CacheMemoryLocation( (typename numberSpecific <numberType>::type*)&mem_val, bytes_left, alignof(numberType) );

        if (this->data.IsValid() && this->data.GetSize() >= sizeof(numberType))
        {
            goto returnResult;
        }

        // Error (unlikely to even be compiled due to alignment-guarantees of types).
        throw invalid_memory_request_exception();

    returnResult:
        this->SelectRef <numberType> ( &mem_val, (callbackType&&)cb );
    }

    template <typename numberType>
    AINLINE typename no_volatile <numberType>::type ReadCacheInto( numberType& mem_val, size_t bytes_left = sizeof(numberType) )
    {
        numberType result = 0u;

        this->SelectCachedNumber( mem_val, bytes_left,
            [&]( auto& val ) LAINLINE
            {
                // Trim-off any bits that are too much, fill in any bits with zeroes that are missing.
                result = (numberType)val;
            }
        );

        return result;
    }

    AINLINE uint8_t GetCacheSize( void ) const noexcept
    {
        return this->data.GetSize();
    }
    AINLINE typename numberSpecific <void>::type* GetOriginalDataPointer( void ) const noexcept
    {
        return this->data_pointer;
    }

private:
    PlatformNumberSolitaryStorage data;
    typename numberSpecific <void>::type *data_pointer;
};

template <platformLocalBitcountType _hostBitCount>
struct BitNumberIterator
{
    static constexpr platformLocalBitcountType hostBitCount = _hostBitCount;
     
    AINLINE constexpr BitNumberIterator( size_t numOffset = 0, platformLocalBitcountType bitOffset = 0 ) noexcept : numOffset( numOffset ), bitOffset( bitOffset )
    {
        return;
    }
    AINLINE BitNumberIterator( const BitNumberIterator& ) = default;

    AINLINE BitNumberIterator& operator = ( const BitNumberIterator& ) = default;

    AINLINE constexpr void addBits( platformBitCountType bitCount ) noexcept
    {
        platformBitCountType numBitOff = this->getTotalBitOffset();

        platformBitCountType newBitOff = numBitOff + bitCount;

        this->numOffset = (size_t)( newBitOff / hostBitCount );
        this->bitOffset = (platformLocalBitcountType)( newBitOff % hostBitCount );
    }
    AINLINE constexpr void addBytes( size_t byteCount ) noexcept
    {
        const platformLocalBitcountType hostByteCount = ( hostBitCount / 8u );

        size_t numCountByBytes = byteCount / hostByteCount;
        size_t remainderByBytes = byteCount % hostByteCount;

        this->numOffset += numCountByBytes;
        this->addBits( remainderByBytes * 8u );
    }

    AINLINE constexpr size_t getNumberOffset( void ) const noexcept
    {
        return this->numOffset;
    }
    AINLINE constexpr platformLocalBitcountType getLocalBitOffset( void ) const noexcept
    {
        return this->bitOffset;
    }
    AINLINE constexpr platformBitCountType getTotalBitOffset( void ) const noexcept
    {
        return ( this->numOffset * hostBitCount + this->bitOffset );
    }
    AINLINE constexpr size_t getTotalByteOffset( void ) const noexcept
    {
        if _BITMANAGE_IF_CONSTEXPR ( hostBitCount % 8u == 0 )
        {
            return (size_t)( this->numOffset * ( hostBitCount / 8u ) + this->bitOffset / 8u );
        }
        else
        {
            return (size_t)( getTotalBitOffset() / 8u );
        }
    }

    // Optimized variant.
    AINLINE constexpr BitNumberIterator& operator += ( const BitNumberIterator& right ) noexcept
    {
        this->numOffset += right.numOffset;
        this->addBits( right.bitOffset );
        return *this;
    }
    // Less-optimized variant.
    template <platformLocalBitcountType otherBitCount>
    AINLINE constexpr BitNumberIterator& operator += ( const BitNumberIterator <otherBitCount>& right ) noexcept
    {
        this->addBits( right.getTotalBitOffset() );
        return *this;
    }

    AINLINE constexpr BitNumberIterator& operator -= ( const BitNumberIterator& right ) noexcept
    {
        this->numOffset -= right.numOffset;
        platformLocalBitcountType bitOffset = this->bitOffset;
        platformLocalBitcountType subBitOffset = right.bitOffset;

        if (bitOffset < subBitOffset)
        {
            this->numOffset--;
            this->bitOffset = ( hostBitCount - ( subBitOffset - bitOffset ) );
        }
        else
        {
            this->bitOffset = ( bitOffset - subBitOffset );
        }
        return *this;
    }

private:
    size_t numOffset;
    platformLocalBitcountType bitOffset;
};
template <typename numberType>
using BitNumberIteratorForStruct = BitNumberIterator <sizeof(numberType) * 8u>;

#if defined(_PLATFORM_HAS_FAST_BITREVERSE)
AINLINE constexpr platformNativeWordType reverse_bitorder( platformNativeWordType v ) noexcept
{
    return __platform_bitreverse( v );
}
// Unsigned integer only.
template <typename numberType>
AINLINE constexpr numberType reverse_bitorder( numberType v ) noexcept
{
    // IDEA: implement the other lesser-sized integer versions using the native word variant and then just perform
    // a bit-shift to get the result, resulting in a two-op reverse_bitorder!

    // We assume that all numberType types are sized a power-of-two, meaning that each bigger sized number is
    // also at least double the size of platformNativeWordType.
    // This optional constexpr-if is fine because both code-graphs would compile anyway.
    if _BITMANAGE_IF_CONSTEXPR (sizeof(v) > sizeof(platformNativeWordType))
    {
        const size_t num_units = sizeof(v) / sizeof(platformNativeWordType);

        numberType result = 0u;

        for (size_t n = 0; n < num_units; n++)
        {
            platformNativeWordType buf = (platformNativeWordType)( v >> (n * (sizeof(platformNativeWordType)*8u)) );
            buf = __platform_bitreverse(buf);
            result |= (numberType)buf << ( (num_units-1) - n ) * (sizeof(platformNativeWordType)*8u);
        }
    }
    else
    {
        platformNativeWordType buf = v;
        buf = __platform_bitreverse(buf);
        return (numberType)( buf >> ((sizeof(platformNativeWordType) - sizeof(v)) * 8u) );
    }
}
#else
// Slow but generic variant.
// Unsigned integer only.
template <typename numberType>
constexpr AINLINE numberType reverse_bitorder( numberType v ) noexcept
{
    // MSVC cannot constant evaluate this function. Just wow.
    numberType result = 0u;

    for ( platformLocalBitcountType n = 0; n < (platformLocalBitcountType)(sizeof(numberType) * 8u); n++ )
    {
        result <<= 1;
        result |= v & 0x1;
        v >>= 1;
    }
    return result;
}
#endif

// Useful structure for performing the bit-ordering conversion on whole numbers outside of the BitManager class.
// You don't get the caching optimization of BitManager though so if you risk partial operations it is still preferable to use BitManager over directly this.
struct BitOrderingConverter
{
    AINLINE _ENDIAN_CONSTEXPR BitOrderingConverter( endian::eSpecificEndian storage_endianness = endian::eSpecificEndian::DEFAULT_ENDIAN, bool storage_msbfirst = false ) noexcept
    {
        bool is_diff_byteorder = ( storage_endianness != endian::get_current_endianness() );
        bool is_diff_bitorder = ( storage_msbfirst );

        this->do_reverse_bitorder = ( is_diff_bitorder );
        this->do_reverse_byteorder = ( is_diff_byteorder != is_diff_bitorder );
    }
    AINLINE constexpr BitOrderingConverter( const BitOrderingConverter& ) = default;

    template <typename numberType>
    AINLINE constexpr numberType Identity( const numberType& val ) noexcept
    {
        numberType result = val;
        if (this->do_reverse_byteorder) result = endian::byte_swap_fast( result );
        if (this->do_reverse_bitorder) result = reverse_bitorder( result );
        return result;
    }

    AINLINE constexpr bool DoReverseBitorder( void ) const noexcept
    {
        return this->do_reverse_bitorder;
    }
    AINLINE constexpr bool DoReverseByteorder( void ) const noexcept
    {
        return this->do_reverse_byteorder;
    }

private:
    bool do_reverse_bitorder, do_reverse_byteorder;
};

struct BitVectorizedIdentityManager
{
    AINLINE constexpr BitVectorizedIdentityManager( BitOrderingConverter conv = {} ) noexcept : bitconv( conv )
    {
        return;
    }
    AINLINE constexpr BitVectorizedIdentityManager( endian::eSpecificEndian endianness, bool msbfirst = false ) noexcept : BitVectorizedIdentityManager( BitOrderingConverter( endianness, msbfirst ) )
    {
        return;
    }
    AINLINE BitVectorizedIdentityManager( const BitVectorizedIdentityManager& ) = default;

    template <typename hostNumberType, typename numberType>
    AINLINE constexpr numberType VectorTransform( numberType num ) noexcept
    {
        if constexpr ( is_same_as <hostNumberType, numberType>::value )
        {
            return bitconv.Identity( num );
        }
        return 0u;
    }

    // This method is required to be done using lambdas because in C++ it is currently ONLY possible to specialize DOWN the code-graph
    // but NOT up (in the direction of the root of) the code-graph.
    template <typename hostNumberType, typename callbackType>
    AINLINE constexpr void Select( hostNumberType *vec, size_t num, callbackType&& cb ) noexcept
    {
        typedef typename no_volatile <hostNumberType>::type hostFastNumberType;

        if (num == 0) return;

        // Check for support of FAST vectorized transformations.

        if constexpr ( sizeof(hostNumberType) == 1 )
        {
            
        }

        // By default we just select the first.
        cb( this->VectorTransform <hostFastNumberType> ( (hostFastNumberType)vec[0] ) );
    }

private:
    BitOrderingConverter bitconv;
};

// Storage that is identity-optimized on memory request, taking boundary specification into account, allowing flushing back
// of storage into fetched.from location.
// hostNumberType has to be unsigned integral.
template <typename hostNumberType, template <typename> typename numberSpecificator = identity_number_specificator>
struct BitIdentityStorage
{
    AINLINE BitIdentityStorage( void ) = default;
    AINLINE BitIdentityStorage( const BitIdentityStorage& ) = default;

private:
    AINLINE void FlushIdCache( void ) noexcept
    {
        this->idcache.PerformOnByType(
            [this]( auto& ref ) LAINLINE
            {
                typedef typename plain_type_bitfield <decltype(ref)>::type numType;

                if constexpr ( sizeof(numType) >= sizeof(hostNumberType) )
                {
                    numType data = ref;

                    data = this->vecman.template VectorTransform <typename no_volatile <hostNumberType>::type> ( data );

                    this->memcache.template SelectRef <decltype(data)> (
                        this->idcache_pointer,
                        [&]( auto& ref ) LAINLINE
                        {
                            ref = data;
                        }
                    );
                }
            }
        );
        this->idcache.Invalidate();
    }

public:
    // Modelling assumption conflict: in the optimal BitManager implementation we base all of our memory requests on
    // a fixed host-type that defines that bit-layout for all succeeding memory. Not having a fixed number type across requests
    // does break the bit identity in certain operation orders.
    // => That is why I have decided to introduce the template parameter "hostNumberType".

    template <typename callbackType>
    AINLINE void Select( hostNumberType& num, size_t bytes_left, callbackType&& cb, bool cache_dirty )
    {
        uintptr_t numloc = (uintptr_t)&num;

        if (this->idcache.IsValid())
        {
            // Check whether the request is inside the identity cache.
            uintptr_t cachenumloc = (uintptr_t)this->idcache_pointer;
            
            auto cache_size = this->idcache.GetSize();

            if (numloc >= cachenumloc && numloc + sizeof(hostNumberType) <= cachenumloc + cache_size)
            {
                // Request is still within cache, so return it.
                goto returnResult;
            }

            if ( cache_dirty )
            {
                // Request fell outside of cache, so flush the cache.
                this->FlushIdCache();
            }
            else
            {
                this->idcache.Invalidate();
            }
        }
        
        // Invalidate the memory cache if the request is not inside of it.
        if (this->memcache.IsInitialized() == false || this->memcache.template CanSatisfy <hostNumberType> ( num ) == false)
        {
            if ( cache_dirty )
            {
                this->memcache.Flush();
            }
            else
            {
                this->memcache.Invalidate();
            }
            this->memcache.CacheMemoryLocation( &num, bytes_left, alignof(hostNumberType) );

            if (this->memcache.IsInitialized() == false)
            {
                throw invalid_memory_request_exception();
            }
        }
        
        // Put new stuff into cache.
        {
            this->memcache.ViewDivision( num,
                [&]( typename no_volatile <hostNumberType>::type *items, auto cnt ) LAINLINE
                {
                    // Fill the idcache with as much identity-transformed data as possible, then return a location inside of the idcache.
                    this->vecman.Select( items, cnt,
                        [&]( auto data ) LAINLINE
                        {
                            this->idcache.Store( data );
                        }
                    );
                }
            );
            this->idcache_pointer = &num;
        }
    returnResult:
        uint8_t idbyteoff = (uint8_t)( numloc - (uintptr_t)this->idcache_pointer );

        this->idcache.PerformOn( (callbackType&&)cb, idbyteoff * 8u );
    }

    AINLINE hostNumberType Read( hostNumberType& val, size_t bytes_left, bool cache_dirty )
    {
        hostNumberType result;

        this->Select( val, bytes_left,
            [&]( auto& data ) LAINLINE
            {
                result = (hostNumberType)data;
            }, cache_dirty
        );

        return result;
    }
    AINLINE void Write( hostNumberType& val, hostNumberType write_to, size_t bytes_left, bool cache_dirty )
    {
        this->Select( val, bytes_left,
            [&]( auto& data ) LAINLINE
            {
                data = write_to;
            }, cache_dirty
        );
    }

    AINLINE void Flush( void ) noexcept
    {
        this->FlushIdCache();
        this->memcache.Flush();
    }
    AINLINE void Invalidate( void ) noexcept
    {
        this->idcache.Invalidate();
        this->memcache.Invalidate();
    }

    AINLINE bool IsValid( void ) const noexcept
    {
        return ( this->idcache.IsValid() );
    }

    AINLINE void SetStorageProperty( endian::eSpecificEndian endianness, bool msbfirst ) noexcept
    {
        this->Flush();
        this->vecman = { endianness, msbfirst };
    }

private:
    PlatformBitCache <numberSpecificator> memcache;
    PlatformNumberSolitaryStorage idcache;
    typename numberSpecificator <void>::type *idcache_pointer;
    BitVectorizedIdentityManager vecman;
};

// Performs logical bitwise left-shift but keeps the sign-specification for the result type.
template <typename T>
AINLINE constexpr decltype(auto) LSHIFT( T v, platformLocalBitcountType cnt ) noexcept
{
    if constexpr ( is_unsigned_integral <T>::value )
    {
        typedef typename make_unsigned_integral <decltype(v<<cnt)>::type promoted_T;

        promoted_T promo_v = v;

        return promo_v << cnt;
    }
    else
    {
        return v << cnt;
    }
}
// Performs logical bitwise right-shift but keeps the sign-specification for the result type.
template <typename T>
AINLINE constexpr decltype(auto) RSHIFT( T v, platformLocalBitcountType cnt ) noexcept
{
    if constexpr ( is_unsigned_integral <T>::value )
    {
        typedef typename make_unsigned_integral <decltype(v>>cnt)>::type promoted_T;

        promoted_T promo_v = v;

        return promo_v >> cnt;
    }
    else
    {
        return v >> cnt;
    }
}

// hostNumberType must be unsigned integer only.
template <typename hostNumberType, template <typename numberType> typename numberSpecificator = choose_default_number_specificator <hostNumberType>::template spec>
struct BitManager
{
private:
    typedef typename no_volatile <hostNumberType>::type hostFastNumberType;

public:
    AINLINE BitManager( hostNumberType *buffer, size_t bufSize, size_t numOffset = 0, platformLocalBitcountType bitOffset = 0 ) noexcept
        : buffer( buffer ), bufSize( bufSize ), iterator( numOffset, bitOffset )
    {
        this->cache.SetStorageProperty( this->storage_endianness, this->storage_msbfirst );
    }
    AINLINE BitManager( const BitManager& ) = default;

    AINLINE ~BitManager( void )
    {
        if (dirty_cache)
        {
            this->cache.Flush();
        }
    }

    AINLINE BitManager& operator = ( const BitManager& ) = default;

    AINLINE size_t GetBufferSize( void ) const noexcept { return this->bufSize; }

private:
    typedef BitNumberIteratorForStruct <hostNumberType> iter_t;

    template <typename callbackType>
    AINLINE void SelectByIndex( size_t idx, callbackType&& cb )
    {
        size_t bytes_left = sizeof(hostNumberType) * ( this->bufSize - idx );

        this->cache.Select( this->buffer[ idx ], bytes_left, (callbackType&&)cb, this->dirty_cache );
    }

public:
    // I have decided to implement a true msbfirst parameter instead of reverse-bits because the reversing of entire hostNumberType bits would conflict
    // with the reversing of bit-groups - the so called "byteswap" algorithm - in programming models.
    // Read the algorithm proof inside the methods for further details.

    AINLINE void Reset( void ) noexcept
    {
        this->Flush();
        this->iterator = iter_t();
    }

private:
    template <typename numberType, platformLocalBitcountType valIterBits>
    AINLINE void _PutLoopImpl( const numberType& val, BitNumberIterator <valIterBits>& srcIter, iter_t& iter, iter_t& end_iter )
    {
        while ( true )
        {
            size_t numOff = iter.getNumberOffset();
            platformLocalBitcountType bitOff = iter.getLocalBitOffset();

            iter_t rem_iter = end_iter;
            rem_iter -= iter;

            platformBitCountType avail_bitCnt = rem_iter.getTotalBitOffset();

            if (avail_bitCnt == 0) break;

            this->SelectByIndex( numOff,
                [&]( auto& pnum ) LAINLINE
                {
                    platformLocalBitcountType numBits = get_bitcount( pnum );

                    platformLocalBitcountType bitCnt = (platformLocalBitcountType)( numBits > avail_bitCnt ? avail_bitCnt : numBits );

                    // TODO: may think about implementing a rolling-replace algorithm to save invocations of the
                    // ROR/ROL instructions, improving performance.

                    replace_bits( pnum, RSHIFT( val, srcIter.getLocalBitOffset() ), bitOff, bitCnt );

                    srcIter.addBits( bitCnt );
                    iter.addBits( bitCnt );

                    this->dirty_cache = true;
                }
            );
        }
        this->iterator = end_iter;
    }
public:
    // numberType must be unsigned integer only.
    // If msbfirst == true then the bits of numbers are reversed.
    template <typename numberType, bool byteOptimize = true>
    AINLINE void PutEx( const numberType& val, BitNumberIteratorForStruct <numberType>& srcIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        if _BITMANAGE_IF_CONSTEXPR ( byteOptimize )
        {
            // Use this if you know that operations execute on multiples of bytes (8 bit packets).
            size_t dst_bytes_left = sizeof(hostNumberType) * ( this->bufSize - iter.getNumberOffset() ) - iter.getLocalBitOffset() / 8u;
            size_t src_bytes_left = ( sizeof(numberType) * 8u - srcIter.getLocalBitOffset() ) / 8u;
            size_t advby_bytes = ( dst_bytes_left > src_bytes_left ? src_bytes_left : dst_bytes_left );

            end_iter.addBytes( advby_bytes );
        }
        else
        {
            platformBitCountType dst_bits_left = ( (platformBitCountType)this->bufSize * sizeof(hostNumberType) * 8u - iter.getTotalBitOffset() );
            platformBitCountType src_bits_left = ( (platformBitCountType)sizeof(numberType) * 8u - srcIter.getLocalBitOffset() );
            platformBitCountType advby_bits = ( dst_bits_left > src_bits_left ? src_bits_left : dst_bits_left );

            end_iter.addBits( advby_bits );
        }

        this->_PutLoopImpl( val, srcIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void PutPartialBits( const numberType& val, platformLocalBitcountType putBitcnt, BitNumberIteratorForStruct <numberType>& srcIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        platformBitCountType dst_bits_left = ( (platformBitCountType)this->bufSize * sizeof(hostNumberType) * 8u - iter.getTotalBitOffset() );
        platformBitCountType src_bits_left = ( (platformBitCountType)sizeof(numberType) * 8u - srcIter.getLocalBitOffset() );
        platformBitCountType advby_bits_content = ( dst_bits_left > src_bits_left ? src_bits_left : dst_bits_left );

        platformBitCountType advby_bits = ( advby_bits_content > putBitcnt ? putBitcnt : advby_bits_content );

        end_iter.addBits( advby_bits );

        this->_PutLoopImpl( val, srcIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void PutPartialBytes( const numberType& val, uint8_t putBytecnt, BitNumberIteratorForStruct <numberType>& srcIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        // Use this if you know that operations execute on multiples of bytes (8 bit packets).
        size_t dst_bytes_left = sizeof(hostNumberType) * ( this->bufSize - iter.getNumberOffset() ) - iter.getLocalBitOffset() / 8u;
        size_t src_bytes_left = ( sizeof(numberType) * 8u - srcIter.getLocalBitOffset() ) / 8u;
        size_t advby_bytes_content = ( dst_bytes_left > src_bytes_left ? src_bytes_left : dst_bytes_left );

        size_t advby_bytes = ( advby_bytes_content > putBytecnt ? putBytecnt : advby_bytes_content );

        end_iter.addBytes( advby_bytes );

        this->_PutLoopImpl( val, srcIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void PutEx( const numberType& val, BitNumberIteratorForStruct <numberType>& srcIter, endian::eSpecificEndian target_endianness, bool msbfirst = false )
    {
        endian::eSpecificEndian prev_storage_endianness = this->storage_endianness;
        bool prev_storage_msbfirst = this->storage_msbfirst;

        this->SetDefaultStorageProperty( target_endianness, msbfirst );

        try
        {
            this->PutEx( val, srcIter );
        }
        catch( ... )
        {
            this->storage_endianness = prev_storage_endianness;
            this->storage_msbfirst = prev_storage_msbfirst;
            throw;
        }

        this->storage_endianness = prev_storage_endianness;
        this->storage_msbfirst = prev_storage_msbfirst;
    }
    // Only use this function if you know that the put request will complete 100%.
    // Otherwise you should use PutEx with the iterator, proceeding until the iterator is at it's end.
    template <typename numberType>
    AINLINE void Put( const numberType& val, endian::eSpecificEndian target_endianness = endian::eSpecificEndian::DEFAULT_ENDIAN, bool msbfirst = false )
    {
        BitNumberIteratorForStruct <numberType> srcIter;

        this->PutEx( val, srcIter, target_endianness, msbfirst );
    }
private:
    template <typename numberType, platformLocalBitcountType dstIterBits>
    AINLINE void _FetchLoopImpl( numberType& result, BitNumberIterator <dstIterBits>& dstIter, iter_t& iter, iter_t& end_iter )
    {
        bool exec = true;

        while ( exec )
        {
            size_t numOff = iter.getNumberOffset();
            platformLocalBitcountType bitOff = iter.getLocalBitOffset();

            iter_t rem_iter = end_iter;
            rem_iter -= iter;

            platformBitCountType avail_bitCnt = rem_iter.getTotalBitOffset();

            if (avail_bitCnt == 0) break;

            this->SelectByIndex( numOff,
                [&]( auto& pnum ) LAINLINE
                {
                    platformLocalBitcountType numBits = get_bitcount( pnum );

                    platformLocalBitcountType bitCnt = (platformLocalBitcountType)( numBits > avail_bitCnt ? avail_bitCnt : numBits );

                    auto wbits = extract_bits( pnum, bitOff, bitCnt );

                    result |= LSHIFT( wbits, dstIter.getLocalBitOffset() );

                    dstIter.addBits( bitCnt );
                    iter.addBits( bitCnt );
                }
            );
        }
        this->iterator = end_iter;
    }
public:
    template <typename numberType, bool byteOptimize = true>
    AINLINE void FetchEx( numberType& result, BitNumberIteratorForStruct <numberType>& dstIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        if _BITMANAGE_IF_CONSTEXPR ( byteOptimize )
        {
            // Use this if you know that operations execute on multiples of bytes (8 bit packets).
            size_t src_bytes_left = sizeof(hostNumberType) * ( this->bufSize - iter.getNumberOffset() ) - iter.getLocalBitOffset() / 8u;
            size_t dst_bytes_left = ( sizeof(numberType) * 8u - dstIter.getLocalBitOffset() ) / 8u;
            size_t advby_bytes = ( dst_bytes_left > src_bytes_left ? src_bytes_left : dst_bytes_left );

            end_iter.addBytes( advby_bytes );
        }
        else
        {
            platformBitCountType src_bits_left = ( (platformBitCountType)this->bufSize * sizeof(hostNumberType) * 8u - iter.getTotalBitOffset() );
            platformBitCountType dst_bits_left = ( (platformBitCountType)sizeof(numberType) * 8u - dstIter.getLocalBitOffset() );
            platformBitCountType advby_bits = ( dst_bits_left > src_bits_left ? src_bits_left : dst_bits_left );

            end_iter.addBits( advby_bits );
        }

        this->_FetchLoopImpl( result, dstIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void FetchPartialBits( numberType& result, platformLocalBitcountType fetchBitCnt, BitNumberIteratorForStruct <numberType>& dstIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        platformBitCountType src_bits_left = ( (platformBitCountType)this->bufSize * sizeof(hostNumberType) * 8u - iter.getTotalBitOffset() );
        platformBitCountType dst_bits_left = ( (platformBitCountType)sizeof(numberType) * 8u - dstIter.getLocalBitOffset() );
        platformBitCountType advby_bits_content = ( dst_bits_left > src_bits_left ? src_bits_left : dst_bits_left );

        platformBitCountType advby_bits = ( advby_bits_content > fetchBitCnt ? fetchBitCnt : advby_bits_content );

        end_iter.addBits( advby_bits );

        this->_FetchLoopImpl( result, dstIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void FetchPartialBytes( numberType& result, uint8_t fetchByteCnt, BitNumberIteratorForStruct <numberType>& dstIter )
    {
        auto iter = this->iterator;

        auto end_iter = iter;

        // Use this if you know that operations execute on multiples of bytes (8 bit packets).
        size_t src_bytes_left = sizeof(hostNumberType) * ( this->bufSize - iter.getNumberOffset() ) - iter.getLocalBitOffset() / 8u;
        size_t dst_bytes_left = ( sizeof(numberType) * 8u - dstIter.getLocalBitOffset() ) / 8u;
        size_t advby_bytes_content = ( dst_bytes_left > src_bytes_left ? src_bytes_left : dst_bytes_left );

        size_t advby_bytes = ( advby_bytes_content > fetchByteCnt ? fetchByteCnt : advby_bytes_content );

        end_iter.addBytes( advby_bytes );

        this->_FetchLoopImpl( result, dstIter, iter, end_iter );
    }
    template <typename numberType>
    AINLINE void FetchEx( numberType& result, BitNumberIteratorForStruct <numberType>& dstIter, endian::eSpecificEndian target_endianness, bool msbfirst = false )
    {
        endian::eSpecificEndian prev_storage_endianness = this->storage_endianness;
        bool prev_storage_msbfirst = this->storage_msbfirst;

        this->SetDefaultStorageProperty( target_endianness, msbfirst );

        try
        {
            this->FetchEx( result, dstIter );
        }
        catch( ... )
        {
            this->storage_endianness = prev_storage_endianness;
            this->storage_msbfirst = prev_storage_msbfirst;
            throw;
        }

        this->storage_endianness = prev_storage_endianness;
        this->storage_msbfirst = prev_storage_msbfirst;
    }
    // Only use this function if you know that the fetch request will complete 100%.
    // Otherwise you should use FetchEx with the iterator, proceeding until the iterator is at it's end.
    template <typename numberType>
    AINLINE numberType Fetch( endian::eSpecificEndian target_endianness = endian::eSpecificEndian::DEFAULT_ENDIAN, bool msbfirst = false )
    {
        BitNumberIteratorForStruct <numberType> dstIter;

        numberType result = 0;
        FetchEx( result, dstIter, target_endianness, msbfirst );

        return result;
    }
    inline void Flush( void ) noexcept
    {
        if (this->dirty_cache)
        {
            this->cache.Flush();

            this->dirty_cache = false;
        }
        else
        {
            this->cache.Invalidate();
        }
    }

    AINLINE bool IsAtEnd( void ) const noexcept
    {
        return ( this->iterator.getNumberOffset() == this->bufSize );
    }

    inline void SetDefaultStorageProperty( endian::eSpecificEndian endianness, bool msbfirst ) noexcept
    {
        if (this->storage_endianness == endianness && this->storage_msbfirst == msbfirst) return;

        this->cache.SetStorageProperty( endianness, msbfirst );
        this->storage_endianness = endianness;
        this->storage_msbfirst = msbfirst;
    }

private:
    hostNumberType *buffer;
    size_t bufSize;
    iter_t iterator;

    // Endian properties of storage.
    endian::eSpecificEndian storage_endianness = endian::eSpecificEndian::DEFAULT_ENDIAN;
    bool storage_msbfirst = false;

    BitIdentityStorage <hostNumberType, numberSpecificator> cache;
    bool dirty_cache = false;
};

} // namespace eir

#endif //_BIT_MANAGE_HEADER_
/*****************************************************************************
*
*  PROJECT:     Eir SDK
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        eirrepo/sdk/Variant.h
*  PURPOSE:     Discriminating union of types in one container, using type-id.
*
*  Find the Eir SDK at: https://osdn.net/projects/eirrepo/
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _EIR_VARIANT_HEADER_
#define _EIR_VARIANT_HEADER_

#include "MetaHelpers.h"
#include "Union.h"

namespace eir
{

namespace VariantUtils
{

// For optimizing variants that only contain POD types.
// After all, simplification of code-gen is really important in the wake of torn-apart-by-the-community compilers.
// The choice between equivalent code-graphs is the heaviest computational problem of modern-day compilers.
template <typename... Ts>
struct types_lifetime_desolate : public true_type {};
template <typename T, typename... Ts>
struct types_lifetime_desolate <T, Ts...>
    : public conditional <(is_pod_type <T>::value == false),
        false_type,
        types_lifetime_desolate <Ts...>
    >::type {};

} // namespace VariantUtils
    
struct invalid_value_exception {};

// Default variant type.
template <typename... Ts>
struct Variant
{
    AINLINE Variant( void ) noexcept : typesel()
    {
        return;
    }
    // With C++20 we could make the constructor accept one of the supported types, by limiting
    // the constructor using C++20 concepts!
    AINLINE Variant( const Variant& right )
        : typesel( right.typesel )
    {
        this->typesel.Switch(
            [&] <typename T> () LAINLINE
        {
            placement_constructor <T> ( this->data.template Get <T> (), right.data.template Get <T> () );
        });
    }
    // Using C++20 we could generalize this to allow construction of bigger variants out of
    // smaller variants with fewer types (C++20 concepts!)
    AINLINE Variant( Variant&& right )
        : typesel( castmove( right.typesel ) )
    {
        this->typesel.Switch(
            [&] <typename T> () LAINLINE
        {
            placement_constructor <T> ( this->data.template Get <T> (), castmove( right.data.template Get <T> () ) );
        });
    }

    AINLINE void Clear( void ) noexcept
    {
        if constexpr ( VariantUtils::types_lifetime_desolate <Ts...>::value == false )
        {
            this->typesel.Switch(
                [&] <typename T> () LAINLINE
            {
                this->data.template Get <T> ().~T();
            });
        }
        this->typesel.Unselect();
    }

    AINLINE ~Variant( void )
    {
        this->Clear();
    }

    AINLINE Variant& operator = ( const Variant& right )
    {
        if constexpr ( VariantUtils::types_lifetime_desolate <Ts...>::value )
        {
            this->typesel = right.typesel;
            this->typesel.Switch(
                [&] <typename T> () LAINLINE
            {
                this->data.template Get <T> () = right.data.template Get <T> ();
            });
        }
        else
        {
            if ( this->typesel.GetIndex() == right.typesel.GetIndex() )
            {
                this->typesel.Switch(
                    [&] <typename T> () LAINLINE
                {
                    this->data.template Get <T> () = right.data.template Get <T> ();
                });
            }
            else
            {
                this->Clear();
                this->typesel = right.typesel;
                this->typesel.Switch(
                    [&] <typename T> () LAINLINE
                {
                    placement_constructor <T> ( this->data.template Get <T> (), right.data.template Get <T> () );
                });
            }
        }
        return *this;
    }
    AINLINE Variant& operator = ( Variant&& right )
    {
        if constexpr ( VariantUtils::types_lifetime_desolate <Ts...>::value )
        {
            this->typesel = right.typesel;
            this->typesel.Switch(
                [&] <typename T> () LAINLINE
            {
                this->data.template Get <T> () = castmove( right.data.template Get <T> () );
            });
        }
        else
        {
            if ( this->typesel.GetIndex() == right.typesel.GetIndex() )
            {
                this->typesel.Switch(
                    [&] <typename T> () LAINLINE
                {
                    this->data.template Get <T> () = castmove( right.data.template Get <T> () );
                });
            }
            else
            {
                this->Clear();
                this->typesel = right.typesel;
                this->typesel.Switch(
                    [&] <typename T> () LAINLINE
                {
                    placement_constructor <T> ( this->data.template Get <T> (), castmove( right.data.template Get <T> () ) );
                });
            }
        }
        return *this;
    }

    template <typename T>
    AINLINE void Store( T&& data )
    {
        if constexpr ( decltype(this->typesel)::template Contains <typename base_type <T>::type> () )
        {
            if constexpr ( VariantUtils::types_lifetime_desolate <Ts...>::value )
            {
                this->data.template Get <T> () = forward <T> ( data );
                this->typesel.template Select <T> ();
            }
            else
            {
                if ( this->typesel.IsSelected <T> () )
                {
                    this->data.template Get <T> () = forward <T> ( data );
                }
                else
                {
                    this->Clear();
                    placement_constructor <T> ( this->data.template Get <T> (), forward <T> ( data ) );
                    this->typesel.template Select <T> ();
                }
            }
        }
        else
        {
            this->Clear();
        }
    }

    AINLINE bool IsEmpty( void ) const noexcept
    {
        return ( this->typesel.HasSelection() == false );
    }
    template <typename T>
    AINLINE bool IsSelected( void ) const noexcept
    {
        return this->typesel.template IsSelected <T> ();
    }
    template <typename T>
    AINLINE T& Get( void )
    {
        // TODO: maybe add support to get base types of bigger types if the equal type does not exist.
        // This would still be fast because the search for a compatible type would be done at compile-time!

        if ( this->template IsSelected <T> () == false )
            throw invalid_value_exception();

        return this->data.template Get <T> ();
    }
    template <typename T>
    AINLINE T* GetOptional( void ) noexcept
    {
        if constexpr ( decltype(this->typesel)::template Contains <T> () == false )
        {
            return nullptr;
        }
        else
        {
            if ( this->template IsSelected <T> () == false )
                return nullptr;

            return &this->data.template Get <T> ();
        }
    }

    template <typename callbackType>
    AINLINE void Visit( callbackType&& cb )
    {
        this->typesel.Switch(
            [&] <typename T> () LAINLINE
        {
            cb( this->data.template Get <T> () );
        });
    }
    template <typename callbackType>
    AINLINE void Visit( callbackType&& cb ) const
    {
        this->typesel.Switch(
            [&] <typename T> () LAINLINE
        {
            cb( this->data.template Get <T> () );
        });
    }

    template <typename callbackType>
    AINLINE void Select( callbackType&& cb )
    {
        bool executed = decltype(this->typesel)::ForEach(
            [&] <typename T> () LAINLINE
        {
            T tmp;
            if (cb(tmp))
            {
                this->Store <T> ( castmove( tmp ) );
                return true;
            }
            return false;
        });
        if (executed == false)
        {
            this->typesel.Unselect();
        }
    }

    // Dangerous method! Use with caution.
    AINLINE void* GetStoragePointer( void ) noexcept
    {
        if constexpr ( sizeof... (Ts) == 0 )
        {
            return nullptr;
        }
        else
        {
            return this->data.GetStoragePointer();
        }
    }
    AINLINE const void* GetStoragePointer( void ) const noexcept
    {
        return const_cast <Variant*> ( this )->GetStoragePointer();
    }

private:
    TypeSelector <Ts...> typesel;
    Union <Ts...> data;
};

// TODO: implement a variant which works on unsigned integers and just
// stores the value inside of a common biggest type.
    
} // namespace eir

#endif //_EIR_VARIANT_HEADER_
/*****************************************************************************
*
*  PROJECT:     Eir SDK
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        eirrepo/sdk/Union.h
*  PURPOSE:     Union struct for language-driven helpers.
*
*  Find the Eir SDK at: https://osdn.net/projects/eirrepo/
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _EIR_UNION_HEADER_
#define _EIR_UNION_HEADER_

#include "MetaHelpers.h"

namespace eir
{
    
// Union data storage model.
template <typename... Ts>
struct Union
{
    AINLINE Union( void ) noexcept {}
    AINLINE Union( const Union& ) = delete;

    AINLINE Union& operator = ( const Union& ) = delete;

    template <typename T>
    AINLINE T& Get( void ) noexcept
    {
        static_assert( sequence_contains <T, Ts...>::value == true, "union does not contain type" );

        T *ptr = nullptr;

        auto lamb = [&] <typename TI, typename... UTs> ( auto& lamb, unpacked_union <TI, UTs...>& memb ) LAINLINE
        {
            if constexpr ( is_same_as <T, TI>::value )
            {
                ptr = &memb.value;
            }
            else
            {
                lamb( lamb, memb.next );
            }
        };

        lamb( lamb, this->data );

        return *ptr;
    }
    template <typename T>
    AINLINE const T& Get( void ) const noexcept
    {
        return const_cast <Union*> ( this )->template Get <T> ();
    }

    template <typename T>
    AINLINE static constexpr bool Contains( void ) noexcept
    {
        return sequence_contains <T, Ts...>::value;
    }

    AINLINE void* GetStoragePointer( void ) noexcept
    {
        return &this->data.value;
    }
    AINLINE const void* GetStoragePointer( void ) const noexcept
    {
        return &this->data.value;
    }

private:
    template <typename... UTs>
    struct unpacked_union
    {};
    template <typename UT, typename... UTs>
    struct unpacked_union <UT, UTs...>
    {
        AINLINE unpacked_union( void ) noexcept {}

        union 
        {
            UT value;
            unpacked_union <UTs...> next;
        };
    };

    unpacked_union <Ts...> data;
};

template <eTypeIdentification idschema, typename... Ts>
struct Union <CustomTypeSelector <idschema, Ts...>> : public Union <Ts...> {};
    
} // namespace eir

#endif //_EIR_UNION_HEADER_
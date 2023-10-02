/*****************************************************************************
*
*  PROJECT:     Eir SDK
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        eirrepo/sdk/MetaHelpers.h
*  PURPOSE:     Memory management templates and other utils.
*
*  Find the Eir SDK at: https://osdn.net/projects/eirrepo/
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _COMMON_META_PROGRAMMING_HELPERS_
#define _COMMON_META_PROGRAMMING_HELPERS_

// Thanks to http://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature
#if __has_include(<type_traits>)
#include <type_traits>
#endif

#if __has_include(<limits>)
#include <limits>
#endif

#include "MacroUtils.h"

#include <new>

namespace eir
{

template <typename To, typename From>
concept nothrow_move_assignable_with = requires ( To& a, From b ) { noexcept(a = std::move(b)); };

template <typename To, typename From>
concept copy_assignable_with = requires ( To& a, const From b ) { a = b; };

template <typename T>
struct no_volatile
{
    typedef T type;
};
template <typename T>
struct no_volatile <volatile T> : public no_volatile <T> {};

template <typename T>
struct no_const
{
    typedef T type;
};
template <typename T>
struct no_const <const T> : public no_const <T> {};

#if __has_include(<type_traits>)
template <typename T>
struct is_unsigned_integral
{
    static constexpr bool value = ( std::is_unsigned <T>::value && std::is_integral <T>::value );
};
#else
template <typename T>
struct is_unsigned_integral : public false_type {};
template <>
struct is_unsigned_integral <uint8_t> : public true_type {};
template <>
struct is_unsigned_integral <uint16_t> : public true_type {};
template <>
struct is_unsigned_integral <uint32_t> : public true_type {};
template <>
struct is_unsigned_integral <uint64_t> : public true_type {};
#endif

#if __has_include(<type_traits>)
template <typename T>
struct make_unsigned_integral
{
    typedef typename std::enable_if <std::is_integral <T>::value, typename std::make_unsigned <T>::type>::type type;
};
#else
template <typename T>
struct make_unsigned_integral
{};
template <>
struct make_unsigned_integral <int8_t>
{
    typedef uint8_t type;
};
template <>
struct make_unsigned_integral <uint8_t>
{
    typedef uint8_t type;
};
template <>
struct make_unsigned_integral <int16_t>
{
    typedef uint16_t type;
};
template <>
struct make_unsigned_integral <uint16_t>
{
    typedef uint32_t type;
};
template <>
struct make_unsigned_integral <int32_t>
{
    typedef uint32_t type;
};
template <>
struct make_unsigned_integral <uint32_t>
{
    typedef uint32_t type;
};
template <>
struct make_unsigned_integral <int64_t>
{
    typedef uint64_t type;
};
template <>
struct make_unsigned_integral <uint64_t>
{
    typedef uint64_t type;
};
#endif

// Converts to the underlying type, throwing away any array-spec, pointer-spec, reference, etc.
template <typename T>
struct plain_type
{
    typedef T type;
};
template <typename T>
struct plain_type <volatile T> : public plain_type <T> {};
template <typename T>
struct plain_type <const T> : public plain_type <T> {};
template <typename T>
struct plain_type <T&> : public plain_type <T> {};
template <typename T>
struct plain_type <T&&> : public plain_type <T> {};
template <typename T>
struct plain_type <T*> : public plain_type <T> {};
template <typename T>
struct plain_type <T[]> : public plain_type <T> {};

// Converts to the underlying type of const/volatile/reference types.
template <typename T>
struct base_type
{
    typedef T type;
};
template <typename T>
struct base_type <const T> : public base_type <T> {};
template <typename T>
struct base_type <volatile T> : public base_type <T> {};
template <typename T>
struct base_type <T&> : public base_type <T> {};
template <typename T>
struct base_type <T&&> : public base_type <T> {};

struct true_type
{
    static const constexpr bool value = true;
};
struct false_type
{
    static const constexpr bool value = false;
};

// Checks if a type is POD.
template <typename T>
struct is_pod_type : public false_type {};
template <>
struct is_pod_type <char> : public true_type {};
template <>
struct is_pod_type <short> : public true_type {};
template <>
struct is_pod_type <int> : public true_type {};
template <>
struct is_pod_type <long> : public true_type {};
template <>
struct is_pod_type <long long> : public true_type {};
template <>
struct is_pod_type <unsigned char> : public true_type {};
template <>
struct is_pod_type <unsigned short> : public true_type {};
template <>
struct is_pod_type <unsigned int> : public true_type {};
template <>
struct is_pod_type <unsigned long> : public true_type {};
template <>
struct is_pod_type <unsigned long long> : public true_type {};
template <>
struct is_pod_type <float> : public true_type {};
template <>
struct is_pod_type <double> : public true_type {};
template <>
struct is_pod_type <bool> : public true_type {};
template <typename T>
struct is_pod_type <const T> : public is_pod_type <T> {};
template <typename T>
struct is_pod_type <volatile T> : public is_pod_type <T> {};
template <typename T>
struct is_pod_type <T&> : public is_pod_type <T> {};
template <typename T>
struct is_pod_type <T&&> : public is_pod_type <T> {};
template <typename T>
struct is_pod_type <T*> : public true_type {};

template <bool eval, typename true_type, typename false_type>
struct conditional
{
    typedef false_type type;
};
template <typename true_type, typename false_type>
struct conditional <true, true_type, false_type>
{
    typedef true_type type;
};

// Returns the first biggest type in the list.
template <typename BT, typename... Ts>
struct biggest_type
{
    typedef BT type;
};
template <typename BT, typename T, typename... Ts>
struct biggest_type <BT, T, Ts...>
    : public biggest_type <typename conditional <(sizeof(BT) >= sizeof(T)), BT, T>::type, Ts...> {};

template <typename T1, typename T2>
struct is_same_as : public false_type {};
template <typename T1>
struct is_same_as <T1, T1> : public true_type {};

template <typename T1, typename T2>
struct is_plain_same_as : public is_same_as <typename plain_type <T1>::type, typename plain_type <T2>::type> {};

template <typename T>
struct remove_reference
{
    typedef T type;
};
template <typename T>
struct remove_reference <T&> : public remove_reference <T> {};
template <typename T>
struct remove_reference <T&&> : public remove_reference <T> {};

template <typename T>
AINLINE constexpr typename remove_reference <T>::type&& castmove( T&& v ) noexcept { return (typename remove_reference <T>::type&&)v; }

template <typename T>
AINLINE constexpr T&& forward( typename remove_reference <T>::type& v ) noexcept { return (T&&)v; }
template <typename T>
AINLINE constexpr T&& forward( typename remove_reference <T>::type&& v ) noexcept { return (T&&)v; }

template <typename T, typename... argTypes>
AINLINE T& placement_constructor( T& storage, argTypes&&... args )
{
    // Language bug: if the placement new function is not defined then this placement-constructor invocation will fail.
    return *::new (&storage) T( forward <argTypes> ( args )... );
}

#if __has_include(<limits>)
template <typename T>
struct numeric_limits
{
    static constexpr T MIN = std::numeric_limits <T>::min();
    static constexpr T MAX = std::numeric_limits <T>::max();
};
#else
template <typename T>
struct numeric_limits
{};
template <>
struct numeric_limits <int8_t>
{
    static constexpr int8_t MIN = -128;
    static constexpr int8_t MAX = 127;
};
// Unsigned integral could be generalized if we had C++20 concepts.
template <>
struct numeric_limits <uint8_t>
{
    static constexpr uint8_t MIN = 0u;
    static constexpr uint8_t MAX = 255u;
};
template <>
struct numeric_limits <int16_t>
{
    static constexpr int16_t MIN = -0x8000;
    static constexpr int16_t MAX = 0x7FFF;
};
template <>
struct numeric_limits <uint16_t>
{
    static constexpr uint16_t MIN = 0u;
    static constexpr uint16_t MAX = 0xFFFFu;
};
template <>
struct numeric_limits <int32_t>
{
    static constexpr int32_t MIN = -0x80000000;
    static constexpr int32_t MAX = 0x7FFFFFFF;
};
template <>
struct numeric_limits <uint32_t>
{
    static constexpr uint32_t MIN = 0u;
    static constexpr uint32_t MAX = 0xFFFFFFFFu;
};
template <>
struct numeric_limits <int64_t>
{
    static constexpr int64_t MIN = -0x8000000000000000;
    static constexpr int64_t MAX = 0x7FFFFFFFFFFFFFFF;
};
template <>
struct numeric_limits <uint64_t>
{
    static constexpr uint64_t MIN = 0u;
    static constexpr uint64_t MAX = 0xFFFFFFFFFFFFFFFFu;
};
#endif

template <size_t num, typename FT, typename... Ts>
struct fitting_numeric
{
    typedef FT type;
};
template <size_t num, typename FT, typename T, typename... Ts>
struct fitting_numeric <num, FT, T, Ts...>
    : public fitting_numeric <num, typename conditional <(numeric_limits <T>::MAX >= num && sizeof(FT) > sizeof(T)), T, FT>::type, Ts...> {};

template <size_t num>
struct fitting_unsigned_integral : public fitting_numeric <num, uint8_t, uint16_t, uint32_t, uint64_t> {};

template <size_t num>
struct fitting_signed_integral : public fitting_numeric <num, int8_t, int16_t, int32_t, int64_t> {};

template <typename TC, typename... Ts>
struct sequence_contains : public false_type {};
template <typename TC, typename... Ts>
struct sequence_contains <TC, TC, Ts...> : public true_type {};
template <typename TC, typename T2, typename... Ts>
struct sequence_contains <TC, T2, Ts...> : public sequence_contains <TC, Ts...> {};

enum class eTypeIdentification
{
    INCREMENTAL,
    BY_SIZE
};

// Configurable type selector.
template <eTypeIdentification idschema = eTypeIdentification::INCREMENTAL, typename... Ts>
struct CustomTypeSelector
{
    typedef typename
        conditional <(idschema == eTypeIdentification::INCREMENTAL), typename fitting_unsigned_integral <sizeof...(Ts)>::type,
            typename conditional <(idschema == eTypeIdentification::BY_SIZE),
                typename fitting_unsigned_integral <sizeof(typename biggest_type <Ts..., uint8_t>::type)>::type,
                void
            >::type
        >::type
        typeid_t;
private:
    template <size_t N, typename CT, typename... TTs>
    struct index_in_sequence
    {
        static constexpr size_t id = N;
    };
    template <size_t N, typename CT, typename... TTs>
    struct index_in_sequence <N, CT, CT, TTs...>
    {
        static constexpr size_t id = N;
    };
    template <size_t N, typename CT, typename T, typename... TTs>
    struct index_in_sequence <N, CT, T, TTs...> : public index_in_sequence <N+1, CT, TTs...> {};

    template <size_t N, typename... TTs>
    struct select_type {};
    template <typename T, typename... TTs>
    struct select_type <0, T, TTs...>
    {
        typedef T type;
    };
    template <size_t N, typename T, typename... TTs>
    struct select_type <N, T, TTs...> : public select_type <N-1, TTs...> {};

public:
    AINLINE static constexpr typeid_t GetInvalidIndex( void ) noexcept
    {
        if constexpr ( idschema == eTypeIdentification::INCREMENTAL )
        {
            return sizeof...(Ts);
        }
        else if constexpr ( idschema == eTypeIdentification::BY_SIZE )
        {
            return 0u;
        }
    }

    AINLINE static constexpr bool IsValidIndex( typeid_t tn ) noexcept
    {
        if constexpr ( idschema == eTypeIdentification::INCREMENTAL )
        {
            return ( tn < sizeof...( Ts ) );
        }
        else
        {
            return ( tn != GetInvalidIndex() );
        }
    }

    template <typename T>
    AINLINE static constexpr typeid_t GetTypeIndex( void ) noexcept
    {
        if constexpr ( idschema == eTypeIdentification::INCREMENTAL )
        {
            return index_in_sequence <0, T, Ts...>::id;
        }
        else if constexpr ( idschema == eTypeIdentification::BY_SIZE )
        {
            if constexpr ( index_in_sequence <0, T, Ts...>::id < sizeof...(Ts) )
            {
                return sizeof( T );
            }
            else
            {
                return 0u;
            }
        }
    }

    AINLINE static constexpr size_t GetSizeForIndex( typeid_t tn ) noexcept
    {
        if constexpr ( idschema == eTypeIdentification::INCREMENTAL )
        {
            typeid_t curidx = 0;
            size_t sz = 0;

            auto lamb = [&] <typename T> ( T _ ) LAINLINE
            {
                if ( sz == 0 && curidx == tn )
                {
                    sz = sizeof(T);
                }
                curidx++;
            };

            ( lamb( Ts() ), ... );

            return sz;
        }
        else if constexpr ( idschema == eTypeIdentification::BY_SIZE )
        {
            return tn;
        }
    }

    template <typename T>
    AINLINE static constexpr bool Contains( void ) noexcept
    {
        return ( IsValidIndex( GetTypeIndex <T> () ) );
    }

    AINLINE constexpr CustomTypeSelector( void ) noexcept
    {
        this->tn = GetInvalidIndex();
    }
    AINLINE constexpr CustomTypeSelector( const CustomTypeSelector& ) = default;

    template <typename T>
    AINLINE constexpr void Select( void ) noexcept
    {
        this->tn = GetTypeIndex <T> ();
    }
    AINLINE constexpr void Unselect( void ) noexcept
    {
        this->tn = GetInvalidIndex();
    }

    AINLINE typeid_t GetIndex( void ) const noexcept
    {
        return this->tn;
    }

    AINLINE constexpr void SelectByIndex( typeid_t tid ) noexcept
    {
        // This is okay because if the index is >= sizeof...(Ts) then it is invalid, even if much above.
        // You have to be careful for type ids when the type id is a type's size!
        this->tn = tid;
    }

    template <typename T>
    AINLINE constexpr bool IsSelected( void ) const noexcept
    {
        if constexpr ( sizeof...( Ts ) == 0 )
        {
            return false;
        }
        else
        {
            return ( this->tn == GetTypeIndex <T> () );
        }
    }
    AINLINE constexpr bool HasSelection( void ) const noexcept
    {
        return ( sizeof...( Ts ) > 0 && IsValidIndex( this->tn ) );
    }

    AINLINE static constexpr size_t GetCount( void ) noexcept
    {
        return sizeof... ( Ts );
    }

    template <typename callbackType>
    AINLINE static constexpr bool ForEach( callbackType&& cb )
    {
        bool success = false;

        auto lamb = [&] <typename T> () LAINLINE
        {
            if ( success == false )
            {
                if ( cb.template operator() <T> () ) success = true;
            }
        };

        ( lamb.template operator() <Ts> (), ... );

        return success;
    }

    template <typename callbackType>
    AINLINE constexpr bool Switch( callbackType&& cb ) const
    {
        bool exec = false;

        auto lambda = [&] <typename T> () LAINLINE
        {
            if ( GetTypeIndex <T> () == this->tn )
            {
                cb.template operator() <T> ();
                exec = true;
            }
        };

        ( lambda.template operator() <Ts> (), ... );

        return exec;
    }

private:
    typeid_t tn;
};

template <eTypeIdentification idschema, typename... Ts>
struct biggest_type <CustomTypeSelector <idschema, Ts...>> : public biggest_type <Ts...> {};

// Simple incremental schema type selector.
template <typename... Ts>
using TypeSelector = CustomTypeSelector <eTypeIdentification::INCREMENTAL, Ts...>;

// NOTE: a combination of TypeSelector and Union can be used to implement the std::variant type.

} // namespace eir

#define INSTANCE_METHCHECKEX( checkName, methName ) \
    template<typename, typename T> \
    struct has_##checkName { \
        static_assert( \
            std::integral_constant<T, false>::value, \
            "Second template parameter needs to be of function type."); \
    }; \
    template<typename C, typename Ret, typename... Args> \
    struct has_##checkName <C, Ret(Args...)> { \
    private: \
        template<typename T> \
        static constexpr auto check(T*) \
        -> typename \
            std::is_same< \
                decltype( std::declval<T>().methName( std::declval<Args>()... ) ), \
                Ret \
            >::type; \
        template<typename> \
        static constexpr std::false_type check(...); \
        typedef decltype(check<C>(0)) type; \
    public: \
        static constexpr bool value = type::value; \
    };

#define INSTANCE_METHCHECK( methName ) INSTANCE_METHCHECKEX( methName, methName )

#define PERFORM_METHCHECK( className, methName, funcSig )    ( has_##methName <className, funcSig>::value )

// Check if a class has a specific field.
#define INSTANCE_FIELDCHECK( fieldName ) \
    template <typename T> \
    concept hasField_##fieldName = requires( T t ) { t.fieldName; }
    
#define PERFORM_FIELDCHECK( className, fieldName ) ( hasField_##fieldName <className> )

#define INSTANCE_SUBSTRUCTCHECK( subStructName ) \
    template <typename T> \
    concept hasSubStruct_##subStructName = requires { typename T::subStructName; T::subStructName(); }

#define PERFORM_SUBSTRUCTCHECK( className, subStructName ) ( hasSubStruct_##subStructName <className> )

#define DEFINE_HEAP_REDIR_ALLOC_DEFAULTED_OBJMANIPS( allocTypeName ) \
    AINLINE allocTypeName( void ) = default; \
    AINLINE allocTypeName( allocTypeName&& ) = default; \
    AINLINE allocTypeName( const allocTypeName& ) = default; \
    AINLINE allocTypeName& operator = ( allocTypeName&& ) = default; \
    AINLINE allocTypeName& operator = ( const allocTypeName& ) = default;

// Providing the everything-purpose standard allocator pattern in the Eir SDK!
// We want a common setup where the link to the DynamicTypeSystem (DTS) is fixed to the position of the DTS.
#define DEFINE_HEAP_REDIR_ALLOC_BYSTRUCT_STDLAYOUT( allocTypeName, redirAllocTypeName ) \
    DEFINE_HEAP_REDIR_ALLOC_DEFAULTED_OBJMANIPS( allocTypeName ) \
    static inline void* Allocate( void *refMem, size_t memSize, size_t alignment ) noexcept; \
    static inline bool Resize( void *refMem, void *objMem, size_t reqNewSize ) noexcept requires ( eir::ResizeMemoryAllocator <redirAllocTypeName> ); \
    static inline void* Realloc( void *refMem, void *objMem, size_t reqNewSize, size_t alignment ) noexcept requires ( eir::ReallocMemoryAllocator <redirAllocTypeName> ); \
    static inline void Free( void *refMem, void *memPtr ) noexcept;
#define DEFINE_HEAP_REDIR_ALLOC_BYSTRUCT( allocTypeName, redirAllocTypeName ) \
    struct allocTypeName \
    { \
        DEFINE_HEAP_REDIR_ALLOC_BYSTRUCT_STDLAYOUT( allocTypeName, redirAllocTypeName ) \
    };
// Defines a heap redir allocator with resize-semantics.
#define DEFINE_HEAP_REDIR_ALLOC_IMPL_STDLAYOUT( allocTypeName ) \
    DEFINE_HEAP_REDIR_ALLOC_DEFAULTED_OBJMANIPS( allocTypeName ) \
    static inline void* Allocate( void *refMem, size_t memSize, size_t alignment ) noexcept; \
    static inline bool Resize( void *refMem, void *objMem, size_t reqNewSize ) noexcept; \
    static inline void Free( void *refMem, void *memPtr ) noexcept;
#define DEFINE_HEAP_REDIR_ALLOC_IMPL( allocTypeName ) \
    struct allocTypeName \
    { \
        DEFINE_HEAP_REDIR_ALLOC_IMPL_STDLAYOUT( allocTypeName ) \
    };

// Simple compatibility definitions for redir-allocators.
#define DEFINE_HEAP_REDIR_ALLOC_COMPATMETH( allocTypeName, compatAllocTypeName ) \
    AINLINE allocTypeName( struct compatAllocTypeName&& ) noexcept {} \
    AINLINE allocTypeName& operator = ( struct compatAllocTypeName&& ) noexcept { return *this; }

// Non-inline version of the heap allocator template.
#define DEFINE_HEAP_ALLOC( allocTypeName ) \
    struct allocTypeName \
    { \
        AINLINE allocTypeName( void ) = default; \
        AINLINE allocTypeName( allocTypeName&& ) = default; \
        AINLINE allocTypeName( const allocTypeName& ) = default; \
        AINLINE allocTypeName& operator = ( allocTypeName&& ) = default; \
        AINLINE allocTypeName& operator = ( const allocTypeName& ) = default; \
        static void* Allocate( void *refMem, size_t memSize, size_t alignment ) noexcept; \
        static bool Resize( void *refMem, void *objMem, size_t reqNewSize ) noexcept; \
        static void Free( void *refMem, void *memPtr ) noexcept; \
    };

// This thing assumes that the object pointed at by allocNode is of type "NativeHeapAllocator",
// but you may of course implement your own thing that has the same semantics.
#define IMPL_HEAP_REDIR_ALLOC( allocTypeName, hostStructTypeName, redirNode, allocNode ) \
    void* allocTypeName::Allocate( void *refMem, size_t memSize, size_t alignment ) noexcept \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return hostStruct->allocNode.Allocate( memSize, alignment ); \
    } \
    bool allocTypeName::Resize( void *refMem, void *objMem, size_t reqNewSize ) noexcept \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return hostStruct->allocNode.SetAllocationSize( objMem, reqNewSize ); \
    } \
    void allocTypeName::Free( void *refMem, void *memPtr ) noexcept \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        hostStruct->allocNode.Free( memPtr ); \
    }

// Default macros for the allocator templates.
#define IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS ( void *refMem, size_t memSize, size_t alignment ) noexcept
#define IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN void*

#define IMPL_HEAP_REDIR_METH_RESIZE_ARGS_DIRECT ( void *refMem, void *objMem, size_t reqNewSize ) noexcept
#define IMPL_HEAP_REDIR_METH_RESIZE_ARGS(allocatorType) IMPL_HEAP_REDIR_METH_RESIZE_ARGS_DIRECT requires ( eir::ResizeMemoryAllocator <allocatorType> )
#define IMPL_HEAP_REDIR_METH_RESIZE_RETURN bool

#define IMPL_HEAP_REDIR_METH_REALLOC_ARGS_DIRECT ( void *refMem, void *objMem, size_t reqNewSize, size_t alignment ) noexcept
#define IMPL_HEAP_REDIR_METH_REALLOC_ARGS(allocatorType) IMPL_HEAP_REDIR_METH_REALLOC_ARGS_DIRECT requires ( eir::ReallocMemoryAllocator <allocatorType> )
#define IMPL_HEAP_REDIR_METH_REALLOC_RETURN void*

#define IMPL_HEAP_REDIR_METH_FREE_ARGS ( void *refMem, void *memPtr ) noexcept
#define IMPL_HEAP_REDIR_METH_FREE_RETURN void

// Direct allocation helpers that redirect calls to another static allocator that depends on a parent struct.
#define IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Allocate( hostStruct, memSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Resize( hostStruct, objMem, reqNewSize ); \
    }

#define IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Realloc( hostStruct, objMem, reqNewSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        directAllocTypeName::Free( hostStruct, memPtr ); \
    }

// A simple redirector for allocators.
#define IMPL_HEAP_REDIR_DIRECT_ALLOC( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_FREE_RETURN allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName )

#define IMPL_HEAP_REDIR_DIRECT_ALLOC_TEMPLATEBASE( templateBaseType, tbt_templargs, tbt_templuse, allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN templateBaseType tbt_templuse::allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN templateBaseType tbt_templuse::allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN templateBaseType tbt_templuse::allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_FREE_RETURN templateBaseType tbt_templuse::allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DIRECT_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, directAllocTypeName )

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Allocate( hostStruct->refPtrNode, memSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Resize( hostStruct->refPtrNode, objMem, reqNewSize ); \
    }

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return directAllocTypeName::Realloc( hostStruct->refPtrNode, objMem, reqNewSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        directAllocTypeName::Free( hostStruct->refPtrNode, memPtr ); \
    }

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_FREE_RETURN allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName )

#define IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_TEMPLATEBASE( templateBaseType, tbt_templargs, tbt_templuse, allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN templateBaseType tbt_templuse::allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_ALLOCATE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN templateBaseType tbt_templuse::allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_RESIZE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN templateBaseType tbt_templuse::allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_REALLOC_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_FREE_RETURN templateBaseType tbt_templuse::allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DIRECTRPTR_ALLOC_METH_FREE_BODY( allocTypeName, hostStructTypeName, redirNode, refPtrNode, directAllocTypeName )

// Similar to direct allocation but redirect calls to member allocator template instead.
#define IMPL_HEAP_REDIR_DYN_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return hostStruct->dynAllocNode.Allocate( hostStruct, memSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DYN_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return hostStruct->dynAllocNode.Resize( hostStruct, objMem, reqNewSize ); \
    }

#define IMPL_HEAP_REDIR_DYN_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        return hostStruct->dynAllocNode.Realloc( hostStruct, objMem, reqNewSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DYN_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        hostStruct->dynAllocNode.Free( hostStruct, memPtr ); \
    }

#define IMPL_HEAP_REDIR_DYN_ALLOC( allocTypeName, hostStructTypeName, redirNode, dynAllocNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    IMPL_HEAP_REDIR_METH_FREE_RETURN allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, dynAllocNode )

#define IMPL_HEAP_REDIR_DYN_ALLOC_TEMPLATEBASE( templateBaseType, tbt_templargs, tbt_templuse, allocTypeName, hostStructTypeName, redirNode, dynAllocNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN templateBaseType tbt_templuse::allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN templateBaseType tbt_templuse::allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN templateBaseType tbt_templuse::allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, dynAllocNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_FREE_RETURN templateBaseType tbt_templuse::allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DYN_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, dynAllocNode )

// Similiar to dyn-alloc but allows you to determine a separate refPtr which is on the node-way to
// the allocator itself.
#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        auto *refObj = hostStruct->refPtrNode; \
        return refObj->dynAllocRemNode.Allocate( refObj, memSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        auto *refObj = hostStruct->refPtrNode; \
        return refObj->dynAllocRemNode.Resize( refObj, objMem, reqNewSize ); \
    }

#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        auto *refObj = hostStruct->refPtrNode; \
        return refObj->dynAllocRemNode.Realloc( refObj, objMem, reqNewSize, alignment ); \
    }

#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    { \
        hostStructTypeName *hostStruct = LIST_GETITEM( hostStructTypeName, refMem, redirNode ); \
        auto *refObj = hostStruct->refPtrNode; \
        refObj->dynAllocRemNode.Free( refObj, memPtr ); \
    }

#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC( allocTypeName, hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode, directAllocTypeName ) \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    IMPL_HEAP_REDIR_METH_FREE_RETURN allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode )

#define IMPL_HEAP_REDIR_DYNRPTR_ALLOC_TEMPLATEBASE( templateBaseType, tbt_templargs, tbt_templuse, allocTypeName, hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode, directAllocTypeName ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_ALLOCATE_RETURN templateBaseType tbt_templuse::allocTypeName::Allocate IMPL_HEAP_REDIR_METH_ALLOCATE_ARGS \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_ALLOCATE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_RESIZE_RETURN templateBaseType tbt_templuse::allocTypeName::Resize IMPL_HEAP_REDIR_METH_RESIZE_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_RESIZE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_REALLOC_RETURN templateBaseType tbt_templuse::allocTypeName::Realloc IMPL_HEAP_REDIR_METH_REALLOC_ARGS(directAllocTypeName) \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_REALLOC_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode ) \
    tbt_templargs \
    IMPL_HEAP_REDIR_METH_FREE_RETURN templateBaseType tbt_templuse::allocTypeName::Free IMPL_HEAP_REDIR_METH_FREE_ARGS \
    IMPL_HEAP_REDIR_DYNRPTR_ALLOC_METH_FREE_BODY( hostStructTypeName, redirNode, refPtrNode, dynAllocRemNode )

#endif //_COMMON_META_PROGRAMMING_HELPERS_

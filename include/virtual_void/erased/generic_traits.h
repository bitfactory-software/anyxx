#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"
#include "lifetime/observer.h"

namespace virtual_void::erased
{

template<>
struct trait< mutable_observer >
{
    static const bool is_const = false;
    using type = mutable_observer;
    using param_t = void*;

    template< typename FROM >
    static type erase( FROM&& from ) 
    {
        using from_t = std::remove_cvref_t< FROM >;
        if constexpr( std::is_base_of_v< type, from_t > )
        {
            return from;
        }
        else
        {
            return typed_observer< FROM >( from );
        }
    }
};

template<>
struct trait< const_observer >
{
    static const bool is_const = true;
    using type = const_observer;
    using param_t = void const *;

    template< typename FROM >
    static type erase( FROM&& from )
    {
        using from_t = std::remove_reference_t< FROM >;
        if constexpr( std::is_base_of_v< type, from_t > )
        {
            return from;
        }
        else
        {
            return typed_observer< std::add_const_t< from_t > >( from );
        }
    }
};

template< typename ERASED, bool is_const_, typename MAKE_ERASED >
struct erase_trait
{
    static const bool is_const = is_const_;
    using type = ERASED;
    using param_t = void const *;

    template< typename FROM >
    static type erase( FROM&& from )
    {
        using from_t = std::remove_cvref_t< FROM >;
        if constexpr( std::is_base_of_v< type, from_t > )
        {
            return from;
        }
        else
        {
            return MAKE_ERASED{}( std::forward< FROM >( from ) );
        }
    }
};

}
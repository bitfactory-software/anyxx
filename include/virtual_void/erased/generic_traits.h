#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"
#include "lifetime/observer.h"

namespace virtual_void::erased
{

template< typename ERASED, typename CONSTRUCTED_WITH >
auto unerase( auto from )
{
    using constructed_with_t = std::remove_cvref_t< CONSTRUCTED_WITH >;
    constexpr bool was_already_erased = std::is_base_of_v< ERASED, constructed_with_t >;
    if constexpr( was_already_erased )
    {
        return static_cast< constructed_with_t::conrete_t * >( from );
    }
    else
    {
        using concrete_t = constructed_with_t;
        if constexpr( trait< ERASED >::is_const )
        {
            return static_cast< concrete_t const * >( from );
        }
        else
        {
            return static_cast< concrete_t * >( from );
        }
    }
}

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

template< typename SHARED_CONST, typename MAKE_SHARED >
struct trait_shared_const
{
    static const bool is_const = true;
    using type = SHARED_CONST;
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
            return MAKE_SHARED{}( std::forward< FROM >( from ) );
        }
    }
};

}
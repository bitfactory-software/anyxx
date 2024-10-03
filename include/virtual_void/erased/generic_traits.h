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
    using param_t = const type&;

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
    template< typename CONSTRUCTOR_PARAM >
    static auto unerase( const type& from )
    {
        using constructor_param_t = std::remove_cvref_t< CONSTRUCTOR_PARAM >;
        if constexpr( std::is_base_of_v< type, constructor_param_t > )
        {
            return static_cast< constructor_param_t::conrete_t * >( from.data() );
        }
        else
        {
            return static_cast< constructor_param_t* >( from.data() );
        }
    }
};

template<>
struct trait< const_observer >
{
    static const bool is_const = true;
    using type = const_observer;
    using param_t = const type&;

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
    template< typename CONSTRUCTOR_PARAM >
    static auto unerase( const type& from )
    {
        using constructor_param_t = std::remove_cvref_t< CONSTRUCTOR_PARAM >;
        if constexpr( std::is_base_of_v< type, constructor_param_t > )
        {
            return static_cast< const constructor_param_t::conrete_t * >( from.data() );
        }
        else
        {
            return static_cast< const constructor_param_t* >( from.data() );
        }
    }
};

template< typename SHARED_CONST, typename MAKE_SHARED >
struct trait_shared_const
{
    using type = SHARED_CONST;
        
    using param_t = const type&;

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
    template< typename CONSTRUCTOR_PARAM >
    static auto unerase( const type& from )
    {
        using constructor_param_t = std::remove_cvref_t< CONSTRUCTOR_PARAM >;
        if constexpr( std::is_base_of_v< type, constructor_param_t > )
        {
            return static_cast< const constructor_param_t::conrete_t * >( from.data() );
        }
        else
        {
            return static_cast< const constructor_param_t* >( from.data() );
        }
    }
};

}
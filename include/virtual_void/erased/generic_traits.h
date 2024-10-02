#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"

namespace virtual_void::erased
{

template<>
struct trait< mutable_observer >
{
    static const bool is_const = false;
    using type = mutable_observer;
    using param_t = mutable_observer;

    template< typename FROM >
    static mutable_observer erase( FROM&& from )
    {
        return static_cast< std::remove_cvref_t< FROM > * >( &from ); // if this fails to compile, is param for interface const!
    }
    template< typename TO >
    static auto unerase( mutable_observer from )
    {
        return static_cast< std::remove_cvref_t< TO > * >( from );
    }
};

template<>
struct trait< const_observer >
{
    static const bool is_const = true;
    using type = const_observer;
    using param_t = const_observer;

    template< typename FROM >
    static const_observer erase( FROM&& from )
    {
        return static_cast< std::remove_reference_t< FROM > * >( &from );
    }
    template< typename TO >
    static auto unerase( const_observer from )
    {
        return static_cast< std::remove_reference_t< TO > const * >( from );
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
            return static_cast< const constructor_param_t::wrapped_type * >( from.data() );
        }
        else
        {
            return static_cast< const constructor_param_t* >( from.data() );
        }
    }
};

}
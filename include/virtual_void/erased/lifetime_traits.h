#pragma once

#include <type_traits>
#include <stdexcept>

#include "lifetime.h"

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

}
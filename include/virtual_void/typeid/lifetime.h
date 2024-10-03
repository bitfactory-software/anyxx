#pragma once

#include <type_traits>
#include <stdexcept>

#include "lifetime.h"

namespace virtual_void::typeid_
{

template<>
struct trait< mutable_observer >
{
    static const bool is_const = false;
    using type = void const *;
    using param_t = void const *;

    template< typename FROM >
    static void const * erase( FROM&& from )
    {
        return static_cast< std::remove_cvref_t< FROM > * >( &from ); // if this fails to compile, is param for interface const!
    }
    template< typename TO >
    static auto unerase( void const * from )
    {
        return static_cast< std::remove_cvref_t< TO > * >( from );
    }
};

template<>
struct trait< const_observer >
{
    static const bool is_const = true;
    using type = void const *;
    using param_t = void const *;

    template< typename FROM >
    static void const * erase( FROM&& from )
    {
        return static_cast< std::remove_reference_t< FROM > * >( &from );
    }
    template< typename TO >
    static auto unerase( void const * from )
    {
        return static_cast< std::remove_reference_t< TO > const * >( from );
    }
};


}
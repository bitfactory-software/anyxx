#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"
#include "lifetime/observer.h"

namespace virtual_void::erased
{

template< typename ERASED, typename MAKE_ERASED >
struct erase_trait
{
    static constexpr bool is_const = ERASED::is_const;
    using type = ERASED;
    using param_t = ERASED::void_t;

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
            return MAKE_ERASED{}( std::forward< FROM >( from ) );
        }
    }
};

struct make_mutable_observer
{
    template< typename FROM > auto operator()( FROM&& from )
    {
        return typed_observer< std::remove_const_t< std::remove_reference_t< FROM > > >( std::forward< FROM >( from ) );
    }
};
struct make_const_observer
{
    template< typename FROM > auto operator()( FROM&& from )
    {
        return typed_observer< std::add_const_t< std::remove_reference_t< FROM > > >( from );
    }
};

template<> struct trait< mutable_observer > : erase_trait< mutable_observer, make_mutable_observer >{};
template<> struct trait< const_observer >   : erase_trait< const_observer, make_const_observer >{};

}
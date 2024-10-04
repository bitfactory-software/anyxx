#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"
#include "lifetime/observer.h"

namespace virtual_void::erased
{

template< typename ERASED >
struct erase_trait
{
    using type = ERASED;

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
            using make_erased_t = ERASED::make_erased;
            return make_erased_t{}( std::forward< FROM >( from ) );
        }
    }
};


template<> struct trait< mutable_observer > : erase_trait< mutable_observer >{};
template<> struct trait< const_observer >   : erase_trait< const_observer >{};

}
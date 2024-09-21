#pragma once

#include <memory>
#include <utility>

#include "../forward.h"

namespace virtual_void::proxy
{
    struct meta
    {
        template <class O> constexpr explicit meta( std::in_place_type_t< O > ) 
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< std::shared_ptr< O > > )
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< std::unique_ptr< O > > )
            : type_info( typeid( O ) ) {}
        template <class O> constexpr explicit meta( std::in_place_type_t< O* > )
            : type_info( typeid( O ) ) {}

        const std::type_info& type_info;
    };

    struct meta_facade : pro::facade_builder
        ::support_copy<pro::constraint_level::nontrivial>
        ::add_reflection< meta >
        ::build {};

    PRO_DEF_FREE_DISPATCH( free_data, data_, data );

    template< typename O > const void* data_( const O& o ) { return &o; }
    template< typename O > void* data_( O& o ) { return &o; }

    template< typename O > const void* data_( std::shared_ptr< const O >& o ) { return o.get(); }
    template< typename O > void* data_( std::shared_ptr< O >& o ) { return o.get(); }
    template< typename O > const void* data_( std::unique_ptr< const O >& o ) { return o.get(); }

    struct virtual_const_void_facade : pro::facade_builder
        ::support_copy<pro::constraint_level::nontrivial>
        ::add_facade< meta_facade, true >
        ::add_convention< free_data, const void*() const >
        ::build {};

    struct virtual_void_facade : pro::facade_builder
        ::support_copy<pro::constraint_level::nontrivial>
//        ::add_facade< virtual_const_void_facade, true >
        ::add_convention< free_data, void*() >
        ::build {};
}


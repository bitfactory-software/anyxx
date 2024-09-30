#pragma once

#include <memory>
#include <utility>

#include "../virtual_void.h"

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
struct virtual_const_void_facade : pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_facade< meta_facade, true >
    ::add_convention< free_data, const void*() const >
    ::build {};
struct virtual_void_facade : pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_facade< virtual_const_void_facade, true >
    ::add_convention< free_data, void*() >
    ::build {};
template< typename O > const void* data_( const O& o ) { return &o; }
template< typename O > void* data_( O& o ) { return &o; }


PRO_DEF_FREE_DISPATCH( free_m_table_of, m_table_of_, m_table_of );
struct m_table_facade : pro::facade_builder
    ::support_copy<pro::constraint_level::nontrivial>
    ::add_convention< free_m_table_of, m_table*()const >
    ::build {};
template< typename O > m_table* m_table_of_( const O& o ) { return m_table_of< O >(); }

template< typename TYPED_VOID, typename FACADE >
auto to_typed_void_( const pro::proxy< FACADE >& p )
{
    const std::type_info& type_info = pro::proxy_reflect< meta >( p ).type_info;
    auto cv = data( *p );
    return TYPED_VOID{ type_info, cv };
}
template< typename FACADE >
auto to_typed_const_void( const pro::proxy< FACADE >& p )
{
    return to_typed_void_< typeid_const_void >( p );
}
template< typename FACADE >
auto to_typeid_void( const pro::proxy< FACADE >& p )
{
    return to_typed_void_< typeid_void >( p );
}

template< typename VIRTUAL_VOID, typename FACADE >
auto to_virtual_void_( const pro::proxy< FACADE >& p )
{
    auto vt = m_table_of( *p );
    auto cv = data( *p );
    return VIRTUAL_VOID{ vt, cv };
}
template< typename FACADE >
auto to_virtual_const_void( const pro::proxy< FACADE >& p )
{
    return to_virtual_void_< virtual_const_void >( p );
}
template< typename FACADE >
auto to_virtual_void( const pro::proxy< FACADE >& p )
{
    return to_virtual_void_< typeid_void >( p );
}
}


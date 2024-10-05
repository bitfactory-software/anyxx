#pragma once

#include <typeindex>

namespace virtual_void
{
class m_table_t;
template< typename CLASS > constexpr m_table_t* m_table_of();

using m_table_const_void = std::pair< const m_table_t*, const void* >;
using m_table_void = std::pair< const m_table_t*, void* >;

template< typename P > auto to_m_table_void( const P* p ){ return m_table_const_void{ m_table_of< P >(), p }; }
template< typename P > auto to_m_table_void( P* p ){ return m_table_const_void{ m_table_of< P >(), p }; }

using typeid_const_void = std::pair< const std::type_info&, const void* >;
using typeid_void = std::pair< const std::type_info&, void* >;

template< typename P > auto to_typeid_void( const P* p ){ return typeid_const_void{ typeid( *p ), p }; }
template< typename P > auto to_typeid_void( P* p ){ return typeid_void{ typeid( *p ), p }; }

template< typename >  struct self_pointer;
template<>  struct self_pointer< void * >		{ template< typename CLASS > using type = CLASS*; };
template<>  struct self_pointer< const void * >	{ template< typename CLASS > using type = const CLASS*; };

class type_info_dispatch;

template< typename DISPATCH, typename VOID >
concept MtableDispatchableVoid = requires( const DISPATCH& void_ )
{
    { void_.data() }	-> std::convertible_to< VOID >;
    { void_.m_table() } -> std::convertible_to< const m_table_t* >;
};

}
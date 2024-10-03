#pragma once

#include <type_traits>
#include <stdexcept>

#include "../forward.h"

namespace virtual_void::erased
{

struct abstract_data
{
	void* data_ = nullptr;
	abstract_data( void* data )
		: data_( data )
	{}
};
template< typename M >
struct concrete_data : abstract_data
{
	M m_;
	concrete_data( M&& m )
		: abstract_data( std::addressof( m_ ) )
		, m_( std::move( m ) )
	{}
	template< typename... ARGS >
	concrete_data( std::in_place_t,  ARGS&&... args )
		: abstract_data( std::addressof( m_ ) )
		, m_( std::forward< ARGS >( args )... )
	{}
};

template< typename U > U* reconcrete_cast( abstract_data& a ) { return static_cast< U* >( a.data_ ); }
template< typename U > const U* reconcrete_cast( const abstract_data& a ) { return static_cast< const U* >( a.data_ ); }

}
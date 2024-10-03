#pragma once

#include <type_traits>
#include <stdexcept>

#include "../forward.h"

namespace virtual_void::erased
{

template< typename VOID >
struct observer
{
	using void_t = VOID;
	observer( const observer& ) = default;
	observer( observer& ) = default;
	observer( observer&& ) = default;
	template< typename T >
	observer( T&& v )
		requires( !std::derived_from< T, observer > )
		: data_( &v )
	{}
	VOID data_ = nullptr;
	VOID data() const { return data_; }
};
using const_observer = observer< void const *>;
using mutable_observer = observer< void *>;

template< typename U > auto reconcrete_cast( mutable_observer o )	{ return static_cast< U* >( o.data() ); }
template< typename U > auto reconcrete_cast( const_observer o )		{ return static_cast< const U* >( o.data() ); }

template< typename T > struct select_observer				{ using type = mutable_observer; };
template< typename T > struct select_observer< T const >	{ using type = const_observer; };

template< typename T >
struct typed_observer : public select_observer< std::remove_reference_t< T > >::type
{
	using conrete_t = std::remove_reference_t< T >;
	using observer_t = select_observer< T >::type;
	using observer_t::observer_t;
	typed_observer( const typed_observer& ) = default;
	typed_observer( typed_observer& ) = default;
	typed_observer( typed_observer&& ) = default;
	typed_observer( const observer_t& o )
		: observer_t( o )
	{}
	typed_observer( T&& v )
		: observer_t( std::forward< T >( v ) )
	{}
    conrete_t& operator*() const  { return  *static_cast< conrete_t* >( this->data() ); }
    conrete_t* operator->() const { return  static_cast< conrete_t* >( this->data() ); }
};


}
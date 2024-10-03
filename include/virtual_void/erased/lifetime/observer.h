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
	VOID data_ = nullptr;
	VOID data() const { return data_; }
};
using const_observer = observer< void const *>;
using mutable_observer = observer< void *>;

template< typename U > auto reconcrete_cast( mutable_observer o )	{ return static_cast< U* >( *o.data() ); }
template< typename U > auto reconcrete_cast( const_observer o )		{ return static_cast< const U* >( *o.data() ); }

template< typename T > struct select_observer				{ using type = mutable_observer; };
template< typename T > struct select_observer< T const >	{ using type = const_observer; };

template< typename T >
struct typed_observer : public select_observer< T >::type
{
	using observer_t = select_observer< T >::type;
	typed_observer( const observer_t  o )
		: observer_t( o )
	{}
    T& operator*() const { return  *static_cast< T* >( this->data() ); }
    T* operator->() const { return  static_cast< T* >( this->data() ); }
};

}
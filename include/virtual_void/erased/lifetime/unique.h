#pragma once

#include <type_traits>
#include <stdexcept>

#include "data.h"

namespace virtual_void::erased
{

using unique_abstract_data_ptr = std::unique_ptr< abstract_data >;
class unique
{
	unique_abstract_data_ptr ptr_;
protected:
	unique( unique_abstract_data_ptr&& ptr )
		: ptr_( std::move( ptr ) )
	{}
public:
	using void_t = void *;
	static constexpr bool is_const = false;
	unique( const unique& ) = default;
	unique( unique& ) = default;
	unique( unique&& ) = default;
	unique& operator=( unique&& ptr ) = default;
 	template< typename T >
	unique( T&& v ) noexcept
		: ptr_( std::make_unique< concrete_data< T > >( std::move( v ) ) )
	{}
	template< typename T, typename... ARGS > 
	unique( std::in_place_type_t< T >, ARGS&&... args ) noexcept
		: ptr_( std::make_unique< concrete_data< T > >( std::in_place, std::forward< ARGS >( args )... ) )
	{}
	void* data() const { return ptr_->data_; }
	operator bool() const { return ptr_.operator bool(); } // false only after move!
	template< typename U > friend U* reconcrete_cast( unique& );
	template< typename U > friend const U* reconcrete_cast( const unique& );
};
template< typename U > U* reconcrete_cast( unique& u ) { return reconcrete_cast< U >( *u.ptr_ ); }
template< typename U > const U* reconcrete_cast( const unique& u ) { return reconcrete_cast< U >( *u.ptr_ ); }

template< typename T >
class typed_unique : public unique
{
    template< typename T > friend auto as( unique&& source );
	typed_unique( unique&& u ) noexcept
		: unique( std::move( u ) ) 
	{}
public:
	using conrete_t = T;
	using unique::unique;
	typed_unique( T&& v ) noexcept
		: unique( std::move( v ) )
	{}
	template< typename... ARGS > 
	typed_unique( std::in_place_t, ARGS&&... args ) noexcept
		: unique( std::in_place_type_t< T >, std::forward< ARGS >( args )... )
	{}
    T& operator*() const { return  *static_cast< T* >( data() ); }
    T* operator->() const { return  static_cast< T* >( data() ); }
};

template< typename T > auto as( unique&& source )
{
    return typed_unique< T >{ std::move( source ) };
}
template< typename T, typename... ARGS > typed_unique< T > make_unique( ARGS&&... args )
{
	return { std::in_place, std::forward< ARGS >( args )... };
}

}
#pragma once

#include <type_traits>
#include <stdexcept>

#include "data.h"

namespace virtual_void::erased
{

template< typename T > class typed_shared_const;
using shared_abstract_data_ptr = std::shared_ptr< abstract_data >;

struct make_shared_const_t;

class shared_const
{ 
protected:
	shared_abstract_data_ptr ptr_;
	explicit shared_const( const shared_abstract_data_ptr& ptr )
		: ptr_( ptr )
	{}
public:
	using void_t = void const *;
	static constexpr bool is_const = true;
	using make_erased = make_shared_const_t;

	shared_const( const shared_const& ptr ) = default;
	shared_const( shared_const& ptr ) = default;
	shared_const( shared_const&& ptr ) = default;
	shared_const& operator=( shared_const&& ptr ) = default;
	template< typename T >
	shared_const( T&& v ) noexcept
		requires ( !std::derived_from< T, shared_const > )
		: ptr_( std::make_shared< concrete_data< T > >( std::forward< T >( v ) ) )
	{}
	template< typename T >
	shared_const( const T& v ) noexcept
		requires ( !std::derived_from< T, shared_const > )
		: ptr_( std::make_shared< concrete_data< T > >( v ) )
	{}
	template< typename T, typename... ARGS > 
	shared_const( std::in_place_type_t< T >, ARGS&&... args ) noexcept
		: ptr_( std::make_shared< concrete_data< T > >( std::in_place, std::forward< ARGS >( args )... ) )
	{}
    const void* data() const { return ptr_->data_; }
	operator bool() const { return ptr_.operator bool(); } // false only after move!
	template< typename U > friend U* reconcrete_cast( shared_const& );
	template< typename U > friend const U* reconcrete_cast( const shared_const& );
};
template< typename U > U const * reconcrete_cast( const shared_const& u ) { return reconcrete_cast< U >( *u.ptr_ ); }

template< typename T >
class typed_shared_const : public shared_const
{
private:
   typed_shared_const( shared_const&& ptr ) noexcept
        : shared_const( std::move( ptr ) )
    {}
 public:
	using conrete_t = const T;
	using shared_const::shared_const;
	typed_shared_const( T&& v ) noexcept
		: shared_const( std::forward< T >( v ) )
	{}
	typed_shared_const( const T& v ) noexcept
		: shared_const( std::forward< T >( v ) )
	{}
	template< typename... ARGS > 
	typed_shared_const( std::in_place_t, ARGS&&... args ) noexcept
		: shared_const( std::in_place_type< T >, std::forward< ARGS >( args )... )
	{}
    template< typename T, typename... ARGS > friend typed_shared_const< T > make_shared_const( ARGS&&... args );
    template< typename T > friend typed_shared_const< T > as( shared_const source );
	template< typename DERIVED >
	typed_shared_const( const typed_shared_const< DERIVED >& rhs ) noexcept
		requires ( std::derived_from< DERIVED, T > && !std::same_as< DERIVED, T > )
		: shared_const( rhs )
	{}
	template< typename DERIVED >
	typed_shared_const( typed_shared_const< DERIVED >&& rhs ) noexcept
		requires ( std::derived_from< DERIVED, T > && !std::same_as< DERIVED, T > )
		: shared_const( std::move( rhs ) )
	{}
	template< typename DERIVED >
	typed_shared_const& operator=( const typed_shared_const< DERIVED >& rhs ) noexcept
		requires ( std::derived_from< DERIVED, T > && !std::same_as< DERIVED, T > )
	{
		typed_shared_const clone{ rhs };
		swap( *this, clone );
		return *this;
	}
	template< typename DERIVED >
	typed_shared_const& operator=( typed_shared_const< DERIVED >&& rhs ) noexcept
		requires ( std::derived_from< DERIVED, T > && !std::same_as< DERIVED, T > )
	{
		(*this) = std::move( rhs );
		return *this;
	}
    friend void swap( typed_shared_const& rhs, typed_shared_const& lhs ) noexcept
    {
        using std::swap;
        swap( rhs.ptr_, lhs.ptr_ );
    }
    const T& operator*() const noexcept  { return *static_cast< const T* >( data() ); }
    const T* operator->() const noexcept { return  static_cast< const T* >( data() ); }
};

template< typename T, typename... ARGS > typed_shared_const< T > make_shared_const( ARGS&&... args )
{
	return { std::in_place, std::forward< ARGS >( args )... };
}
template< typename T > typed_shared_const< T > as( shared_const source )
{
    return typed_shared_const< T >{ std::move( source ) };
}

struct make_shared_const_t
{
    template< typename FROM > shared_const operator()( FROM&& from )
    {
        return 	make_shared_const< std::remove_cvref_t< FROM > >( std::forward< FROM >( from ) );
    }
};

static_assert( is_erased_lifetime_holder< shared_const > );
static_assert( is_erased_lifetime_holder< typed_shared_const< int > > );

}
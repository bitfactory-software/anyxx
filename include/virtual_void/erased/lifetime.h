#pragma once

#include <type_traits>
#include <stdexcept>

#include "forward.h"

namespace virtual_void::erased
{

template< typename T > class typed_shared_const;

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

using shared_abstract_data_ptr = std::shared_ptr< abstract_data >;
class shared_const
{ 
protected:
	shared_abstract_data_ptr ptr_;
public:
	shared_const( const shared_abstract_data_ptr& ptr )
		: ptr_( ptr )
	{}
    const void* data() const { return ptr_->data_; }
	operator bool() const { return ptr_.operator bool(); } // false only after move!
};

template< typename T >
class typed_shared_const : public shared_const
{
private:
   typed_shared_const( shared_const&& ptr ) noexcept
        : shared_const( std::move( ptr ) )
    {}
 public:
	using wrapped_type = T;
	using shared_const::shared_const;
	typed_shared_const( T&& v ) noexcept
		: shared_const( std::make_shared< concrete_data< T > >( std::forward< T >( v ) ) )
	{}
	typed_shared_const( const T& v ) noexcept
		: shared_const( std::make_shared< concrete_data< T > >( T{ v } ) )
	{}
	template< typename... ARGS > 
	typed_shared_const( std::in_place_t, ARGS&&... args ) noexcept
		: shared_const( std::make_shared< concrete_data< T > >( std::in_place, std::forward< ARGS >( args )... ) )
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


using unique_abstract_data_ptr = std::unique_ptr< abstract_data >;
class unique
{
	unique_abstract_data_ptr ptr_;
protected:
	unique( unique_abstract_data_ptr&& ptr )
		: ptr_( std::move( ptr ) )
	{}
public:
    void* data() const { return ptr_->data_; }
	operator bool() const { return ptr_.operator bool(); } // false only after move!
};

template< typename T >
class typed_unique : public unique
{
    template< typename T > friend auto as( unique&& source );
	typed_unique( unique&& u ) noexcept
		: unique( std::move( u ) ) 
	{}
public:
	using unique::unique;
	typed_unique( T&& v ) noexcept
		: unique( std::make_unique< concrete_data< T > >( std::move( v ) ) )
	{}
	template< typename... ARGS > 
	typed_unique( std::in_place_t, ARGS&&... args ) noexcept
		: unique( std::make_unique< concrete_data< T > >( std::in_place, std::forward< ARGS >( args )... ) )
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
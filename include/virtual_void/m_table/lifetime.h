#pragma once

#include <typeindex>
#include <stdexcept>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <assert.h>

#include "../virtual_void.h"
#include "../erased/forward.h"

namespace virtual_void::m_table
{

template< typename T > class typed_shared_const;

struct abstract_data
{
	const m_table_t* m_table_ = nullptr;
	void* data_ = nullptr;
	abstract_data( const m_table_t* m_table, void* data )
		: m_table_( m_table )
		, data_( data )
	{}
};
template< typename M >
struct concrete_data : abstract_data
{
	M m_;
	concrete_data( M&& m )
		: abstract_data( m_table_of< M >(), std::addressof( m_ ) )
		, m_( std::move( m ) )
	{}
	template< typename... ARGS >
	concrete_data( std::in_place_t,  ARGS&&... args )
		: abstract_data( m_table_of< M >(), std::addressof( m_ ) )
		, m_( std::forward< ARGS >( args )... )
	{}
};

struct make_shared_const_t;

using shared_abstract_data_ptr = std::shared_ptr< abstract_data >;
class shared_const
{ 
protected:
	shared_abstract_data_ptr ptr_;
public:
	shared_const( const shared_abstract_data_ptr& ptr )
		: ptr_( ptr )
	{}
	using void_t = void const *;
	static constexpr bool is_const = true;
	using make_erased = make_shared_const_t;

	const void* data() const { return ptr_->data_; }
    const std::type_info& type() const { return ptr_->m_table_->type(); }
	const m_table_t* m_table() const { return ptr_->m_table_; };
};
static_assert( MtableDispatchableVoid< const shared_const, const void* > );

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
static_assert( MtableDispatchableVoid< const typed_shared_const< nullptr_t >, const void* > );
template< typename T, typename... ARGS > typed_shared_const< T > make_shared_const( ARGS&&... args )
{
	return { std::in_place, std::forward< ARGS >( args )... };
}
template< typename T > typed_shared_const< T > as( shared_const source )
{
    if( source.type() != typeid( T ) )
        throw error( "source is: " + std::string( source.type().name() ) + "." );
    return typed_shared_const< T >{ std::move( source ) };
}
struct make_shared_const_t
{
    template< typename FROM > auto operator()( FROM&& from )
    {
        return make_shared_const< std::remove_cvref_t< FROM > >( std::forward< FROM >( from ) );
    }
};
static_assert( erased::is_erased< shared_const > );
static_assert( erased::is_erased< typed_shared_const< int > > );


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
    void* data() const { return ptr_->data_; }
    const std::type_info& type() const { return ptr_->m_table_->type(); }
	const m_table_t* m_table() const { return ptr_->m_table_; };
};
static_assert( MtableDispatchableVoid< const unique, void* > );
static_assert( MtableDispatchableVoid< const unique, const void* > );

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
		: unique( std::make_unique< concrete_data< T > >( std::move( v ) ) )
	{}
	template< typename... ARGS > 
	typed_unique( std::in_place_t, ARGS&&... args ) noexcept
		: unique( std::make_unique< concrete_data< T > >( std::in_place, std::forward< ARGS >( args )... ) )
	{}
    T& operator*() const { return  *static_cast< T* >( data() ); }
    T* operator->() const { return  static_cast< T* >( data() ); }
};
static_assert( MtableDispatchableVoid< const typed_unique< nullptr_t >, void* > );
static_assert( MtableDispatchableVoid< const typed_unique< nullptr_t >, const void* > );

template< typename T > auto as( unique&& source )
{
    if( source.type() != typeid( T ) )
        throw error( "source is: " + std::string( source.type().name() ) + "." );
    return typed_unique< T >{ std::move( source ) };
}
template< typename T, typename... ARGS > typed_unique< T > make_unique( ARGS&&... args )
{
	return { std::in_place, std::forward< ARGS >( args )... };
}
//---lifetime 

}
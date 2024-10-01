#pragma once

#include <type_traits>

#include "forward.h"

namespace virtual_void::erased
{

template<>
struct trait< mutable_observer >
{
    static const bool is_const = false;
    using type = mutable_observer;
    using param_t = mutable_observer;

    template< typename FROM >
    static mutable_observer erase( FROM&& from )
    {
        return static_cast< std::remove_cvref_t< FROM > * >( &from ); // if this fails to compile, is param for interface const!
    }
    template< typename TO >
    static auto unerase( mutable_observer from )
    {
        return static_cast< std::remove_cvref_t< TO > * >( from );
    }
};

template<>
struct trait< const_observer >
{
    static const bool is_const = true;
    using type = const_observer;
    using param_t = const_observer;

    template< typename FROM >
    static const_observer erase( FROM&& from )
    {
        return static_cast< std::remove_reference_t< FROM > * >( &from );
    }
    template< typename TO >
    static auto unerase( const_observer from )
    {
        return static_cast< std::remove_reference_t< TO > const * >( from );
    }
};

////+++lifetime 
//template< typename t > class typed_shared_const;
//
//struct abstract_data
//{
//	const m_table_t* m_table_ = nullptr;
//	void* data_ = nullptr;
//	abstract_data( const m_table* m_table, void* data )
//		: m_table_( m_table )
//		, data_( data )
//	{}
//};
//template< typename m >
//struct concrete_data : abstract_data
//{
//	m m_;
//	concrete_data( m&& m )
//		: abstract_data( m_table_of< m >(), std::addressof( m_ ) )
//		, m_( std::move( m ) )
//	{}
//	template< typename... args >
//	concrete_data( std::in_place_t,  args&&... args )
//		: abstract_data( m_table_of< m >(), std::addressof( m_ ) )
//		, m_( std::forward< args >( args )... )
//	{}
//};
//
//using shared_abstract_data_ptr = std::shared_ptr< abstract_data >;
//class shared_const
//{ 
//protected:
//	shared_abstract_data_ptr ptr_;
//public:
//	shared_const( const shared_abstract_data_ptr& ptr )
//		: ptr_( ptr )
//	{}
//    const void* data() const { return ptr_->data_; }
//    const std::type_info& type() const { return ptr_->m_table_->type(); }
//	const m_table* m_table() const { return ptr_->m_table_; };
//};
//static_assert( mtabledispatchablevoid< const shared_const, const void* > );
//
//template< typename t >
//class typed_shared_const : public shared_const
//{
//private:
//   typed_shared_const( shared_const&& ptr ) noexcept
//        : shared_const( std::move( ptr ) )
//    {}
// public:
//	using wrapped_type = t;
//	using shared_const::shared_const;
//	typed_shared_const( t&& v ) noexcept
//		: shared_const( std::make_shared< concrete_data< t > >( std::forward< t >( v ) ) )
//	{}
//	typed_shared_const( const t& v ) noexcept
//		: shared_const( std::make_shared< concrete_data< t > >( t{ v } ) )
//	{}
//	template< typename... args > 
//	typed_shared_const( std::in_place_t, args&&... args ) noexcept
//		: shared_const( std::make_shared< concrete_data< t > >( std::in_place, std::forward< args >( args )... ) )
//	{}
//    template< typename t, typename... args > friend typed_shared_const< t > make_shared_const( args&&... args );
//    template< typename t > friend typed_shared_const< t > as( shared_const source );
//	template< typename derived >
//	typed_shared_const( const typed_shared_const< derived >& rhs ) noexcept
//		requires ( std::derived_from< derived, t > && !std::same_as< derived, t > )
//		: shared_const( rhs )
//	{}
//	template< typename derived >
//	typed_shared_const( typed_shared_const< derived >&& rhs ) noexcept
//		requires ( std::derived_from< derived, t > && !std::same_as< derived, t > )
//		: shared_const( std::move( rhs ) )
//	{}
//	template< typename derived >
//	typed_shared_const& operator=( const typed_shared_const< derived >& rhs ) noexcept
//		requires ( std::derived_from< derived, t > && !std::same_as< derived, t > )
//	{
//		typed_shared_const clone{ rhs };
//		swap( *this, clone );
//		return *this;
//	}
//	template< typename derived >
//	typed_shared_const& operator=( typed_shared_const< derived >&& rhs ) noexcept
//		requires ( std::derived_from< derived, t > && !std::same_as< derived, t > )
//	{
//		(*this) = std::move( rhs );
//		return *this;
//	}
//    friend void swap( typed_shared_const& rhs, typed_shared_const& lhs ) noexcept
//    {
//        using std::swap;
//        swap( rhs.ptr_, lhs.ptr_ );
//    }
//    const t& operator*() const noexcept  { return *static_cast< const t* >( data() ); }
//    const t* operator->() const noexcept { return  static_cast< const t* >( data() ); }
//};
//static_assert( mtabledispatchablevoid< const typed_shared_const< nullptr_t >, const void* > );
//template< typename t, typename... args > typed_shared_const< t > make_shared_const( args&&... args )
//{
//	return { std::in_place, std::forward< args >( args )... };
//}
//template< typename t > typed_shared_const< t > as( shared_const source )
//{
//    if( source.type() != typeid( t ) )
//        throw error( "source is: " + std::string( source.type().name() ) + "." );
//    return typed_shared_const< t >{ std::move( source ) };
//}
//
//using unique_abstract_data_ptr = std::unique_ptr< abstract_data >;
//class unique
//{
//	unique_abstract_data_ptr ptr_;
//protected:
//	unique( unique_abstract_data_ptr&& ptr )
//		: ptr_( std::move( ptr ) )
//	{}
//public:
//    void* data() const { return ptr_->data_; }
//    const std::type_info& type() const { return ptr_->m_table_->type(); }
//	const m_table* m_table() const { return ptr_->m_table_; };
//};
//static_assert( mtabledispatchablevoid< const unique, void* > );
//static_assert( mtabledispatchablevoid< const unique, const void* > );
//
//template< typename t >
//class typed_unique : public unique
//{
//    template< typename t > friend auto as( unique&& source );
//	typed_unique( unique&& u ) noexcept
//		: unique( std::move( u ) ) 
//	{}
//public:
//	using unique::unique;
//	typed_unique( t&& v ) noexcept
//		: unique( std::make_unique< concrete_data< t > >( std::move( v ) ) )
//	{}
//	template< typename... args > 
//	typed_unique( std::in_place_t, args&&... args ) noexcept
//		: unique( std::make_unique< concrete_data< t > >( std::in_place, std::forward< args >( args )... ) )
//	{}
//    t& operator*() const { return  *static_cast< t* >( data() ); }
//    t* operator->() const { return  static_cast< t* >( data() ); }
//};
//static_assert( mtabledispatchablevoid< const typed_unique< nullptr_t >, void* > );
//static_assert( mtabledispatchablevoid< const typed_unique< nullptr_t >, const void* > );
//
//template< typename t > auto as( unique&& source )
//{
//    if( source.type() != typeid( t ) )
//        throw error( "source is: " + std::string( source.type().name() ) + "." );
//    return typed_unique< t >{ std::move( source ) };
//}
//template< typename t, typename... args > typed_unique< t > make_unique( args&&... args )
//{
//	return { std::in_place, std::forward< args >( args )... };
//}
////---lifetime 

}
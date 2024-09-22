#pragma once

#include <memory>

#include "virtual_void.h"

namespace virtual_void
{

class shared_const
{
	v_table* v_table_ = nullptr;
	std::shared_ptr< const void > ptr_;
public:
    template< typename T, typename... ARGS > friend shared_const make_shared_const( ARGS&&... args );
    const void* data() const { return ptr_.get(); }
    const std::type_info& type() const { return v_table_->type(); }
	v_table* v_table() const { return v_table_; };
};
template< typename T, typename... ARGS > shared_const make_shared_const( ARGS&&... args )
{
    using deleter_t = void(*)( const void*);
    deleter_t deleter = +[]( const void* p ){ delete static_cast< const T* >( p ); };
    shared_const s;
    s.ptr_ = std::unique_ptr< const void, deleter_t >( new T( std::forward< ARGS >( args )... ), deleter );
    s.v_table_ = v_table_of< T >();
    return s;
}
static_assert( VtableDispatchableVoid< const shared_const, const void* > );

template< typename T >
class typed_shared_const
{
private:
    shared_const ptr_;
    typed_shared_const( shared_const&& ptr )
        : ptr_( std::move( ptr ) )
    {}
    template< typename T > friend auto as( shared_const source );
public:
    const T* operator->() const { return  static_cast< const T* >( ptr_.data() ); }
    const void* data() const { return ptr_.data(); }
    const std::type_info& type() const { return ptr_.type(); }
	v_table* v_table() const { return ptr_.v_table; };
};
template< typename T > auto as( shared_const source )
{
    if( source.type() != typeid( T ) )
        throw error( "source is: " + std::string( source.type().name() ) + "." );
    return typed_shared_const< T >{ std::move( source ) };
}
static_assert( VtableDispatchableVoid< const typed_shared_const< nullptr_t >, const void* > );

class unique
{
	v_table* v_table_;
    using deleter_t = void(*)( void*);
	std::unique_ptr< void, deleter_t > ptr_;
    unique( std::unique_ptr< void, deleter_t > ptr, v_table* v_table )
        : ptr_( std::move( ptr ) ), v_table_( v_table )
    {}
public:
    template< typename T, typename... ARGS > friend unique make_unique( ARGS&&... args );
    void* data() const { return ptr_.get(); }
    const std::type_info& type() const { return v_table_->type(); }
	v_table* v_table() const { return v_table_; };
};
template< typename T, typename... ARGS > unique make_unique( ARGS&&... args )
{
    unique::deleter_t deleter = +[]( void* p ){ delete static_cast< T* >( p ); };
    return unique{ std::unique_ptr< void, unique::deleter_t >( new T( std::forward< ARGS >( args )... ), deleter ), v_table_of< T >() };
}
static_assert( VtableDispatchableVoid< const unique, void* > );
static_assert( VtableDispatchableVoid< const unique, const void* > );

template< typename T >
class typed_unique
{
private:
    unique ptr_;
    typed_unique( unique&& ptr )
        : ptr_( std::move( ptr ) )
    {}
    template< typename T > friend auto as( unique&& source );
public:
    const T* operator->() const { return  static_cast< const T* >( ptr_.data() ); }
    void* data() const { return ptr_.data(); }
    const std::type_info& type() const { return ptr_.type(); }
	v_table* v_table() const { return ptr_.v_table; };
};
template< typename T > auto as( unique&& source )
{
    if( source.type() != typeid( T ) )
        throw error( "source is: " + std::string( source.type().name() ) + "." );
    return typed_unique< T >{ std::move( source ) };
}
static_assert( VtableDispatchableVoid< const typed_unique< nullptr_t >, void* > );
static_assert( VtableDispatchableVoid< const typed_unique< nullptr_t >, const void* > );

}
#pragma once

#include <memory>
#include <functional>

#include "virtual_void.h"

namespace virtual_void
{

using deleter_t = void(*)( const void*);

class shared_const_void
{
private:
	v_table* v_table_ = nullptr;
	std::shared_ptr< const void > ptr_;
public:
    template< typename T, typename... ARGS > friend shared_const_void make_shared_const_void( ARGS&&... args );
    const void* data() const { return ptr_.get(); }
    const std::type_info& type() const { return v_table_->type(); }
	v_table* v_table() const { return v_table_; };
};
template< typename T, typename... ARGS >
shared_const_void make_shared_const_void( ARGS&&... args )
{
    deleter_t deleter = +[]( const void* p ){ delete static_cast< const T* >( p ); };
    shared_const_void s;
    s.ptr_ = std::unique_ptr< const void, deleter_t >( new T( std::forward< ARGS >( args )... ), deleter );
    s.v_table_ = v_table_of< T >();
    return s;
}

class unique_void
{
private:
	v_table* v_table_;
	std::unique_ptr< const void, deleter_t > ptr_;
    unique_void( std::unique_ptr< const void, deleter_t > ptr, v_table* v_table )
        : ptr_( std::move( ptr ) ), v_table_( v_table )
    {}
public:
    template< typename T, typename... ARGS > friend unique_void make_unique_void( ARGS&&... args );
    const void* data() const { return ptr_.get(); }
    const std::type_info& type() const { return v_table_->type(); }
	v_table* v_table() const { return v_table_; };
};
template< typename T, typename... ARGS >
unique_void make_unique_void( ARGS&&... args )
{
    deleter_t deleter = +[]( const void* p ){ delete static_cast< const T* >( p ); };
    return unique_void{ std::unique_ptr< const void, deleter_t >( new T( std::forward< ARGS >( args )... ), deleter ), v_table_of< T >() };
}

}
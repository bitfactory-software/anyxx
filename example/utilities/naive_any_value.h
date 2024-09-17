#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <type_traits>
#include <concepts>

//class any_value
//{
//private:
//    struct concept_
//    {
//        ~concept_() = default;
//    };
//    template< typename VALUE >
//    struct model : concept_
//    {
//        model( CALLABLE&& callable ) : callable_( std::forward< CALLABLE >( callable ) ){}
//    };
//    std::shared_ptr< concept_ > _;
//public:
//    any_value() = default;
//    ~any_value() = default;
//    any_value( const any_value& ) = default;
//    any_value& operator=( const any_value& ) = default;
//    any_value( any_value&& ) = default;
//    any_value& operator=( any_value&& ) = default;
//    template< typename CALLABLE >
//        any_value( CALLABLE&& callable )
//            requires ( std::invocable< CALLABLE, ARGS... > ) 
//        : callable_( std::make_shared< model< CALLABLE > >( std::forward< CALLABLE >( callable ) ) ) 
//        {}
//    bool has_value() const { return callable_; }
//    explicit operator bool() const { return callable_; }
//    R operator()( ARGS&&... args ) const { return callable_->invoke( std::forward< ARGS >( args )... ); }
//};
//



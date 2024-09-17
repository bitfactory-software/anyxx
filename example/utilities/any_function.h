#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <type_traits>
#include <concepts>

template< typename R, typename... ARGS >
class any_function;
template< typename R, typename... ARGS >
class any_function< R( ARGS...) >
{
private:
    struct concept_
    {
        virtual R invoke( ARGS&&... args ) = 0;
        ~concept_() = default;
    };
    template< typename CALLABLE >
    struct model : concept_
    {
        CALLABLE callable_;
        model( const CALLABLE& callable ) : callable_( callable ){}
        //model( CALLABLE&& callable ) : callable_( std::move( callable ) ){}
        R invoke( ARGS&&... args ) override { return  callable_( std::forward< ARGS >( args )... ); }
    };
    std::shared_ptr< concept_ > callable_;
public:
    any_function() = default;
    //~any_function() = default;
    //any_function( const any_function& ) = default;
    //any_function& operator=( const any_function& ) = default;
    //any_function( any_function&& ) = default;
    //any_function& operator=( any_function&& ) = default;
//    template< typename X > any_function( const std::initializer_list< X >& ) = delete;
    template< typename CALLABLE >
        any_function( const CALLABLE& callable )
            requires ( std::invocable< CALLABLE, ARGS... > ) 
        : callable_( std::make_shared< model< CALLABLE > >( callable ) ) 
        {}
    //template< typename CALLABLE > : requires std::invokeable< CALLABLE, R, ARGS... >
    //    any_function( CALLABLE&& callable ) : callable_( std::make_shared< model< CALLABLE > >( std::move( callable ) ) {}
    bool has_value() const { return callable_; }
    explicit operator bool() const { return callable_; }
    R operator()( ARGS&&... args ){ return callable_->invoke( std::forward< ARGS >( args )... ); }
};




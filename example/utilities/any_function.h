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
    template< typename TARGET >
    struct model : concept_
    {
        TARGET target_;
        model( TARGET&& target ) : target_( std::forward< TARGET >( target ) ){}
        R invoke( ARGS&&... args ) override { return  target_( std::forward< ARGS >( args )... ); }
    };
    std::shared_ptr< concept_ > target_;
public:
    any_function() = default;
    ~any_function() = default;
    any_function( const any_function& ) = default;
    any_function& operator=( const any_function& ) = default;
    any_function( any_function&& ) = default;
    any_function& operator=( any_function&& ) = default;
    template< typename TARGET >
        any_function( TARGET&& target )
            requires ( std::invocable< TARGET, ARGS... > ) 
        : target_( std::make_shared< model< TARGET > >( std::forward< TARGET >( target ) ) ) 
        {}
    bool has_value() const { return target_; }
    explicit operator bool() const { return target_; }
    R operator()( ARGS&&... args ) const { return target_->invoke( std::forward< ARGS >( args )... ); }
};




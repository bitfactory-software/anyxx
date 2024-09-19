#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <string>
#include <type_traits>
#include <concepts>
#include <stdexcept>
#include <sstream>

namespace naive
{

class any_value
{
private:
    struct concept_;
    using model_ptr_t = std::unique_ptr< concept_ >;
    struct concept_
    {
        virtual void* data() = 0;
        virtual model_ptr_t clone() const = 0;
        virtual ~concept_() = default;
    };
    template< typename VALUE >
    struct model final : concept_
    {
        using value_t = VALUE;
        value_t value_;
        model( VALUE&& value ) : value_( std::forward< VALUE >( value ) ){ static_assert( std::same_as< value_t, std::remove_reference_t< VALUE > > ); }
        void* data() override { return &value_; }
        model_ptr_t clone() const override { return std::make_unique< model >( *this ); }
    };
    model_ptr_t model_;
    struct has_no_value{};
    const std::type_info* type_info_ = &typeid( has_no_value );
public:
//    any_value() = default;
    ~any_value() = default;
    any_value( const any_value& rhs )
        : model_( rhs ? rhs.model_->clone() : nullptr )
        , type_info_ ( rhs.type_info_ )
    {}
    any_value& operator=( const any_value& rhs )
    {
        any_value clone = rhs;
        swap( *this, clone );
    }
    any_value( any_value&& ) = default;
    any_value& operator=( any_value&& ) = default;
    template< typename VALUE >
        any_value( VALUE&& value )
            requires ( std::copy_constructible< std::remove_reference_t< VALUE > > )
                && (!std::same_as< any_value, std::remove_reference_t< VALUE > > )
                && (std::same_as< VALUE, std::remove_pointer_t< VALUE > > )
        : model_( std::make_unique< model< std::remove_reference_t< VALUE > > >( std::forward< VALUE >( value ) ) ) 
        , type_info_( &typeid( VALUE ) )
        {}
    friend void swap( any_value& rhs, any_value& lhs )
    {
        using std::swap;
        swap( rhs.model_, lhs.model_ );
        swap( rhs.type_info_, lhs.type_info_ );
    }
    bool has_value() const { return static_cast< bool >( model_ ); }
    explicit operator bool() const { return has_value(); }
    void* data() { return model_->data(); } // via v-table
    const void* data() const { return model_.get(); }
    const std::type_info& type() const { return *type_info_; }
};

class bad_any_value_cast : public std::runtime_error
{
public:
    bad_any_value_cast( const std::type_info& from_, const std::type_info& to_ )
        : runtime_error( ( std::stringstream{} << "bad cast from " << from_.name() << " to " << to_.name() ).str() )
    {}
};

inline bool can_any_value_cast( const std::type_info& from, const std::type_info& to )
{
}
inline void throw_bad_any_value_cast( const std::type_info& from, const std::type_info& to ) { throw bad_any_value_cast( from, to ); }
inline void ignore_any_value_cast_missmatch( const std::type_info&, const std::type_info& ) {}

template< typename TO, typename ANY >
auto any_value_cast( ANY* any, auto handle_missmatch )
{
    const std::type_info& from = any->type();
    const std::type_info& to = typeid( TO );
    using target_pointer_t = std::conditional_t<std::is_const_v< ANY >, const TO*, TO* >;
    if( from == to )
        return static_cast< target_pointer_t >( any->data() );  
    handle_missmatch( from, to );
    return target_pointer_t{ nullptr };
}
template< typename TO > const TO&   any_value_cast( const any_value& any ) { return *any_value_cast< TO > ( &any, throw_bad_any_value_cast ) ; }
template< typename TO > TO&         any_value_cast( any_value& any )       { return *any_value_cast< TO >( &any, throw_bad_any_value_cast ) ; }
template< typename TO > const TO*   any_value_cast( const any_value* any ) { return any_value_cast< TO > ( &any, ignore_any_value_cast_missmatch ) ; }
template< typename TO > TO*         any_value_cast( any_value* any )       { return any_value_cast< TO >( &any, ignore_any_value_cast_missmatch ) ; }

}
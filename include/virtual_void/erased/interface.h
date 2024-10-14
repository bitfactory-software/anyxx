#pragma once

//
// developed from: https://github.com/AlexCodesApps/virtual_void::erased/blob/main/dynamic_interface.hpp
//
// for Microsoft C++, you must enable the C-Preprocessor with this flag: /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <type_traits>
#include <typeinfo>

#include "concept.h"

namespace virtual_void::erased
{

template< typename VOID >
struct interface_base 
{
    using void_t = VOID;
    static bool static_is_derived_from( const std::type_info& from ) { return typeid( interface_base ) == from; } 
    bool (*_is_derived_from)( const std::type_info& ); 
    template < typename UNERASE >
    interface_base( UNERASE )
        : _is_derived_from( []( const std::type_info& from ){ return static_is_derived_from( from ); } )
    {};
};

template< is_erased_lifetime_holder LIFETIME_HOLDER >
class base 
{
public:
    using lifetime_holder_t = LIFETIME_HOLDER;
    using interface_t = interface_base< typename LIFETIME_HOLDER::void_t >;
protected:
    lifetime_holder_t lifetime_holder_ = nullptr;
    interface_t* interface_impementation_ = nullptr;
public:
    base( lifetime_holder_t lifetime_holder, interface_t* v_table )
        : lifetime_holder_( std::move( lifetime_holder ) )
        , interface_impementation_( v_table )
    {}
    template <typename CONSTRUCTED_WITH>
    base(CONSTRUCTED_WITH&& constructed_with) 
        requires ( !std::derived_from< std::remove_cvref_t< CONSTRUCTED_WITH >, base< LIFETIME_HOLDER > > )
        : lifetime_holder_( virtual_void::erased::erase_to< lifetime_holder_t >( std::forward< CONSTRUCTED_WITH >( constructed_with ) ) )
    {
        static interface_t imlpemented_interface{ virtual_void::erased::unerase< LIFETIME_HOLDER, CONSTRUCTED_WITH >() };
        interface_impementation_ = &imlpemented_interface;
    }
    template< typename OTHER >
    base( const OTHER& other )
        requires ( std::derived_from< OTHER, base< LIFETIME_HOLDER > > )
        : lifetime_holder_( other.get_lifetime_holder() )
        , interface_impementation_( other.get_interface() )
    {}
    base(const base&) = default;
    base(base&) = default;
    base(base&&) = default;
    auto& get_lifetime_holder() const { return lifetime_holder_; }
    auto& get_lifetime_holder() { return lifetime_holder_; }
    interface_t* get_interface() const { return interface_impementation_; }
    bool is_derived_from( const std::type_info& from ) const { return interface_impementation_->_is_derived_from( from ); }
    template< typename FROM > bool is_derived_from() const { return is_derived_from( typeid( FROM::interface_t ) );  }
protected:
    base() = default;
};

template< typename V_TABLE > void set_is_derived_from( auto interface )
{
    interface->_is_derived_from = +[]( const std::type_info& from ){ return V_TABLE::static_is_derived_from( from ); };
}

template< typename TO, typename FROM >
TO static_interface_cast( const FROM& from ) requires ( std::derived_from< TO, FROM > ) { return *static_cast< const TO* >( &from ); }
template< typename TO, typename FROM >
std::optional< TO > interface_cast( const FROM& from ) requires ( std::derived_from< TO, FROM > ) 
{ 
    if( from.is_derived_from< TO >() )
        return { *static_cast< const TO* >( &from ) };
    return {};
}

template< typename TO, typename FROM >
TO interface_lifetime_cast( const FROM& from )
{
    return TO
        ( lifetime_cast< typename TO::lifetime_holder_t >( from.get_lifetime_holder() )
        , v_table_cast< TO >( from.get_interface() )
        );
}

template< template< typename, template< typename > typename > typename... >
struct bases_;
template< template< typename, template< typename > typename > typename BASE >
struct bases_< BASE >
{
    template< typename E > using type = BASE< E, virtual_void::erased::base >;
};
template
    < template< typename, template< typename > typename > typename FIRST
    , template< typename, template< typename > typename > typename... MORE
    >
struct bases_< FIRST, MORE... >
{
    template< typename E > using type = FIRST< E, typename bases_< MORE... >::type >;
};
template< template< typename, template< typename > typename > typename... BASES >
using bases = bases_< BASES... >::type;

template
    < typename E
    , template< typename > typename UNUSED 
    > using open_base = base< E >;

template< typename E, template< typename, template< typename > typename > typename... BASES >
struct compose_
{
    using type = bases_< BASES... >:: template type< E >;
};

template
    < typename E
    , template< typename > typename UNUSED 
    > using open_base = base< E >;

template< typename E, template< typename, template< typename > typename > typename... BASES >
using compose = typename bases_< BASES... >::template type< E >;

template
    < typename E
    , template< typename > typename UNUSED 
    > using open_base = base< E >;

}

#define _detail_EXPAND(...) _detail_EXPAND4(_detail_EXPAND4(_detail_EXPAND4(_detail_EXPAND4(__VA_ARGS__))))
#define _detail_EXPAND4(...) _detail_EXPAND3(_detail_EXPAND3(_detail_EXPAND3(_detail_EXPAND3(__VA_ARGS__))))
#define _detail_EXPAND3(...) _detail_EXPAND2(_detail_EXPAND2(_detail_EXPAND2(_detail_EXPAND2(__VA_ARGS__))))
#define _detail_EXPAND2(...) _detail_EXPAND1(_detail_EXPAND1(_detail_EXPAND1(_detail_EXPAND1(__VA_ARGS__))))
#define _detail_EXPAND1(...) __VA_ARGS__

#define _detail_EXPAND_(...) _detail_EXPAND_4(_detail_EXPAND_4(_detail_EXPAND_4(_detail_EXPAND_4(__VA_ARGS__))))
#define _detail_EXPAND_4(...) _detail_EXPAND_3(_detail_EXPAND_3(_detail_EXPAND_3(_detail_EXPAND_3(__VA_ARGS__))))
#define _detail_EXPAND_3(...) _detail_EXPAND_2(_detail_EXPAND_2(_detail_EXPAND_2(_detail_EXPAND_2(__VA_ARGS__))))
#define _detail_EXPAND_2(...) _detail_EXPAND_1(_detail_EXPAND_1(_detail_EXPAND_1(_detail_EXPAND_1(__VA_ARGS__))))
#define _detail_EXPAND_1(...) __VA_ARGS__
#define _detail_RMCVREF(x) typename std::remove_const<typename std::remove_volatile<typename std::remove_reference<x>::type>::type>::type
#define _detail_PARENS ()
#define _detail_foreach_macro_h(macro, a, ...) macro(a) \
__VA_OPT__(_detail_foreach_macro_a _detail_PARENS (macro, __VA_ARGS__))
#define _detail_foreach_macro_a() _detail_foreach_macro_h
#define _detail_foreach_macro(macro, ...) _detail_EXPAND(_detail_foreach_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_h(macro, a, ...) macro(a) \
__VA_OPT__(, _detail_map_macro_a _detail_PARENS (macro, __VA_ARGS__))
#define _detail_map_macro(macro, ...) _detail_EXPAND(_detail_map_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_a() _detail_map_macro_h
#define _detail_CONCAT_H(a, b) a ## b
#define _detail_CONCAT(a, b) _detail_CONCAT_H(a, b)
#define _detail_PARAM_LIST_H(b, c, f, ...) std::forward<decltype(c)>(c) __VA_OPT__(, _detail_PARAM_LIST_A _detail_PARENS (b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_A() _detail_PARAM_LIST_H
#define _detail_PARAM_LIST(...) _detail_EXPAND_(_detail_PARAM_LIST_H(__VA_ARGS__))
#define _detail_PARAM_LIST_2H(b, c, f, ...) f c __VA_OPT__(, _detail_PARAM_LIST_2A _detail_PARENS (b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_2A() _detail_PARAM_LIST_2H
#define _detail_PARAM_LIST2(...) _detail_EXPAND_(_detail_PARAM_LIST_2H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(,)
#define _detail_INTERFACE_FPD_H(l) _detail_INTERFACE_FUNCTION_PTR_DECL l
#define _detail_INTERFACE_MEMEBER_LIMP_H(l) _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_INTERFACE_FREE_LIMP_H(l) _detail_INTERFACE_LAMBDA_TO_FREE_IMPL l
#define _detail_INTERFACE_METHOD_H(l) _detail_INTERFACE_METHOD l
#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _detail_INTERFACE_FUNCTION_PTR_DECL(type, name, ...) \
type (* name)(void_t __VA_OPT__(, __VA_ARGS__));

#define _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL(type, name, ...) \
name ( [](void_t _vp __VA_OPT__(,_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) \
{\
    return  ( UNERASE{}( _vp ) )->name(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));\
})

#define _detail_INTERFACE_LAMBDA_TO_FREE_IMPL(type, name, ...) \
name ( [](void_t _vp __VA_OPT__(,_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) \
{ \
    return name##_( *UNERASE{}( _vp ) __VA_OPT__(,) __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));\
})

#define _detail_INTERFACE_METHOD(type, name, ...) \
type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) requires ( !LIFETIME_HOLDER::is_const ) { \
    return static_cast< interface_t* >(interface_impementation_)->name(base_t::lifetime_holder_.data() __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
} \
type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const { \
    return static_cast< interface_t* >(interface_impementation_)->name(base_t::lifetime_holder_.data() __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
}

        
#define _detail_ERASED_INTERFACE( n, delegate_lampda_limp, l) \
template< typename BASE_V_TABLE > \
struct n##interface : BASE_V_TABLE \
{ \
    using interface_base_t = BASE_V_TABLE; \
    using void_t = interface_base_t::void_t; \
    using v_table_t = n##interface; \
    static bool static_is_derived_from( const std::type_info& from ) \
    {  \
        return typeid( v_table_t ) == from ? true : interface_base_t::static_is_derived_from( from ) ; \
    } \
    _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l)\
    template <typename UNERASE > \
    n##interface( UNERASE unerase ) \
        : interface_base_t( unerase ) \
        , _detail_map_macro(delegate_lampda_limp, _detail_EXPAND_LIST l) \
    { \
        virtual_void::erased::set_is_derived_from< v_table_t >( this ); \
    }; \
}; \
template< virtual_void::erased::is_erased_lifetime_holder LIFETIME_HOLDER, template < typename > typename BASE = virtual_void::erased::base > \
struct n : BASE< LIFETIME_HOLDER > \
{ \
public: \
    using lifetime_holder_t = LIFETIME_HOLDER; \
    using void_t = LIFETIME_HOLDER::void_t; \
    using base_t = BASE< LIFETIME_HOLDER >; \
    using interface_base_t = base_t::interface_t; \
    using interface_t = n##interface< interface_base_t >; \
protected: \
    using base_t::lifetime_holder_; \
    using base_t::interface_impementation_; \
public: \
    n( lifetime_holder_t lifetime_holder, interface_t* v_table ) \
        : base_t( std::move( lifetime_holder ), v_table ) \
    {} \
    template <typename CONSTRUCTED_WITH> \
    n(CONSTRUCTED_WITH&& v) \
        requires ( !std::derived_from< std::remove_cvref_t< CONSTRUCTED_WITH >, base_t > ) \
    : base_t(std::forward<CONSTRUCTED_WITH>(v)) \
    {  \
        static interface_t imlpemented_interface{ virtual_void::erased::unerase< LIFETIME_HOLDER, CONSTRUCTED_WITH >() }; \
        interface_impementation_ = &imlpemented_interface; \
    } \
    template< typename OTHER > \
    n( const OTHER& other ) \
        requires ( std::derived_from< OTHER, base_t > ) \
        : base_t( other ) \
    {} \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l) \
    n(const n&) = default;\
    n(n&) = default;\
    n(n&&) = default;\
protected: \
    n() = default;\
};
#define ERASED_INTERFACE( name, ...) _detail_ERASED_INTERFACE(name, _detail_INTERFACE_MEMEBER_LIMP_H, (__VA_ARGS__))
#define ERASED_FREE_INTERFACE( name, ...) _detail_ERASED_INTERFACE(name, _detail_INTERFACE_FREE_LIMP_H, (__VA_ARGS__))
#define INTERFACE_METHOD(...) (__VA_ARGS__),


namespace virtual_void::erased
{

template< typename BASE_V_TABLE, typename RET, typename... ARGS >
struct call_operator_interface : BASE_V_TABLE
{
    using interface_base_t = BASE_V_TABLE;
    using void_t = interface_base_t::void_t;
    using v_table_t = call_operator_interface;
    static bool static_is_derived_from( const std::type_info& from ) 
    { 
        return typeid( v_table_t ) == from ? true : BASE_V_TABLE::static_is_derived_from( from ) ; 
    } 
    RET (*call_op)(void_t, ARGS&&... );
    template< typename UNERASE >
    call_operator_interface( UNERASE unerase ) 
        : BASE_V_TABLE( unerase )
        , call_op ( []( void_t _vp, ARGS&&... args ) 
            {
                return ( * UNERASE{}( _vp ) )( std::forward< ARGS >(args)...); 
            })
    {
        set_is_derived_from< v_table_t >( this );
    }
};

template< is_erased_lifetime_holder LIFETIME_HOLDER, template < typename > typename BASE, typename RET, typename... ARGS >
struct call_operator_facade;
template< is_erased_lifetime_holder LIFETIME_HOLDER, template < typename > typename BASE, typename RET, typename... ARGS >
struct call_operator_facade< LIFETIME_HOLDER, BASE, RET(ARGS...) >: BASE< LIFETIME_HOLDER >
{
public:
    using base_t = BASE< LIFETIME_HOLDER >;
    using interface_base_t = base_t::interface_t;
    using interface_t = call_operator_interface< interface_base_t, RET, ARGS... >;
protected:
    using base_t::lifetime_holder_;
    using base_t::interface_impementation_;
public:
    using lifetime_holder_t = LIFETIME_HOLDER;
    using void_t = LIFETIME_HOLDER::void_t;
    call_operator_facade( lifetime_holder_t lifetime_holder, interface_t* v_table )
        : base_t( std::move( lifetime_holder ), v_table )
    {}
    template <typename CONSTRUCTED_WITH>
    call_operator_facade(CONSTRUCTED_WITH&& v) 
        requires ( !std::derived_from< std::remove_cvref_t< CONSTRUCTED_WITH >, base_t > )
        : base_t(std::forward<CONSTRUCTED_WITH>(v))
    { 
        static interface_t imlpemented_interface{ virtual_void::erased::unerase< LIFETIME_HOLDER, CONSTRUCTED_WITH >() };
        interface_impementation_ = &imlpemented_interface;
    }
    template< typename OTHER >
    call_operator_facade( const OTHER& other )
        requires ( std::derived_from< OTHER, base_t > )
        : base_t( other )
    {}
    RET operator()( ARGS&&... args ) requires ( !LIFETIME_HOLDER::is_const ) 
    { 
        return static_cast< interface_t* >( interface_impementation_ )->call_op( base_t::lifetime_holder_.data(), std::forward< ARGS >(args)... ); 
    }
    RET operator()( ARGS&&... args ) const 
    { 
        return static_cast< interface_t* >(interface_impementation_)->call_op( base_t::lifetime_holder_.data(), std::forward< ARGS >(args)...); 
    }
    call_operator_facade(const call_operator_facade&) = default;
    call_operator_facade(call_operator_facade&) = default;
    call_operator_facade(call_operator_facade&&) = default;
protected:
    call_operator_facade() = default;
};
template< typename RET, typename... ARGS >
struct call_operator_
{
    template< typename LIFETIME_HOLDER, template < typename > typename BASE >
    using type = call_operator_facade< LIFETIME_HOLDER, BASE, RET, ARGS... >;
};
template< typename RET, typename... ARGS >
using call_operator = call_operator_< RET, ARGS... >::type;

};

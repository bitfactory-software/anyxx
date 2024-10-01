#pragma once

//
// developed from: https://github.com/AlexCodesApps/dynamic_interface/blob/main/dynamic_interface.hpp
//
// for Microsoft C++, you must enable the C-Preprocessor with this flag: /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <memory>
#include <type_traits>


namespace dynamic_interface
{
    template< typename ERASED >
    struct trait; 

    template<>
    struct trait< void* >
    {
        static const bool is_const = false;
        using type = void*;
        using param_t = void*;

        template< typename FROM >
        static void* erase( FROM&& from )
        {
            return static_cast< std::remove_cvref_t< FROM > * >( &from ); // if this fails to compile, is param for interface const!
        }
        template< typename TO >
        static auto unerase( void* from )
        {
            return static_cast< std::remove_cvref_t< TO > * >( from );
        }
    };

    template<>
    struct trait< void const* >
    {
        static const bool is_const = true;
        using type = void const*;
        using param_t = void const*;

        template< typename FROM >
        static void const * erase( FROM&& from )
        {
            return static_cast< std::remove_reference_t< FROM > * >( &from );
        }
        template< typename TO >
        static auto unerase( const void* from )
        {
            return static_cast< std::remove_reference_t< TO > const * >( from );
        }
    };

    template< typename ERASED >
    struct base 
    {
        using erased_t = ERASED;
        erased_t _ref = nullptr;
        struct _v_table_t 
        {
            bool (*_is_derived_from)( const std::type_info& ); 
            template <typename UNUSED>
            _v_table_t(UNUSED&&)
                : _is_derived_from ( []( const std::type_info& from ){ return base::static_is_derived_from( from ); } )
            {};
        } *_v_table;
        template <typename T>
        base(T&& v) 
            requires ( !std::derived_from< std::remove_cvref_t< T >, base< ERASED > > )
            : _ref(dynamic_interface::trait<erased_t>::erase(std::forward<T>(v)))
        {
            static _v_table_t _tp_v_table{ v };
            _v_table = &_tp_v_table;
        }
        template< typename OTHER >
        base( const OTHER& other )
            requires ( std::derived_from< OTHER, base< ERASED > > )
        {
            _ref = other._ref;
            _v_table = other._v_table;
        }
        base(const base&) = default;
        base(base&) = default;
        base(base&&) = default;
        auto* get_erased() const { return &_ref; }
        auto* get_erased() { return &_ref; }
        bool is_derived_from( const std::type_info& from ) const { return _v_table->_is_derived_from( from ); }
        template< typename FROM > bool is_derived_from() const { return is_derived_from( typeid( FROM ) );  } 
        static bool static_is_derived_from( const std::type_info& from ) { return typeid( base ) == from; } 
    protected:
        base() = default;
    };

    template< typename FACADE > void set_is_derived_from( auto v_table )
    {
        v_table->_is_derived_from = +[]( const std::type_info& from ){ return FACADE::static_is_derived_from( from ); };
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

    template< template< typename, template< typename > typename > typename... >
    struct bases_;
    template< template< typename, template< typename > typename > typename BASE >
    struct bases_< BASE >
    {
        template< typename E > using type = BASE< E, dynamic_interface::base >;
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
};

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
type (* name)(erased_param_t __VA_OPT__(, __VA_ARGS__));

#define _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL(type, name, ...) \
name ( [](erased_param_t _vp __VA_OPT__(,_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) \
{\
    return dynamic_interface::trait<erased_t>::unerase<_tp>(_vp)->name(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));\
})

#define _detail_INTERFACE_LAMBDA_TO_FREE_IMPL(type, name, ...) \
name ( [](erased_param_t _vp __VA_OPT__(,_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) \
{ \
    return name##_( *dynamic_interface::trait<erased_t>::unerase<_tp>(_vp) __VA_OPT__(,) __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));\
})

#define _detail_INTERFACE_METHOD(type, name, ...) \
type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const { \
    return static_cast< _v_table_t* >(_v_table)->name(base_t::_ref __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
}

#define _detail_DECLARE_INTERFACE( n, delegate_lampda_limp, l) \
template< typename ERASED, template < typename > typename BASE = dynamic_interface::base > \
struct n : BASE< ERASED > \
{ \
    using interface_t = n; \
    using erased_t = ERASED; \
    using erased_param_t = dynamic_interface::trait<ERASED>::param_t; \
    using base_t = BASE< ERASED >; \
    using base_t::_ref; \
    using base_t::_v_table; \
    using base_v_table_t = base_t::_v_table_t; \
    struct _v_table_t : base_v_table_t \
    { \
        _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l)\
        template <typename _tp> \
        _v_table_t(_tp&& param) \
            : base_v_table_t( std::forward<_tp>(param) ) \
            , _detail_map_macro(delegate_lampda_limp, _detail_EXPAND_LIST l) \
        { \
            dynamic_interface::set_is_derived_from< n >( this ); \
        }; \
    }; \
    template <typename _tp> \
    n(_tp&& v) \
        requires ( !std::derived_from< std::remove_cvref_t< _tp >, BASE< ERASED > > ) \
    : base_t(std::forward<_tp>(v)) \
    {  \
        static _v_table_t _tp_v_table{ v }; \
        _v_table = &_tp_v_table; \
    } \
    template< typename OTHER > \
    n( const OTHER& other ) \
        requires ( std::derived_from< OTHER, BASE< ERASED > > ) \
    { \
        _ref = other._ref; \
        _v_table = other._v_table; \
    } \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l) \
    n(const n&) = default;\
    n(n&) = default;\
    n(n&&) = default;\
    static bool static_is_derived_from( const std::type_info& from ) \
    {  \
        return typeid( n ) == from ? true : BASE< ERASED >::static_is_derived_from( from ) ; \
    } \
protected: \
    n() = default;\
};
#define DECLARE_INTERFACE( name, ...) _detail_DECLARE_INTERFACE(name, _detail_INTERFACE_MEMEBER_LIMP_H, (__VA_ARGS__))
#define DECLARE_FREE_INTERFACE( name, ...) _detail_DECLARE_INTERFACE(name, _detail_INTERFACE_FREE_LIMP_H, (__VA_ARGS__))
#define INTERFACE_METHOD(...) (__VA_ARGS__),

namespace dynamic_interface
{
    template< typename ERASED, template < typename > typename BASE, typename RET, typename... ARGS >
    struct call_operator_facade : BASE< ERASED >
    {
        using erased_t = ERASED;
        using erased_param_t = trait<ERASED>::param_t;
        using base_t = BASE< ERASED >;
        using base_v_table_t = base_t::_v_table_t;
        using base_t::_ref;
        using base_t::_v_table;
        struct _v_table_t : base_v_table_t
        {
            RET (*call_op)(erased_param_t, ARGS&&... );
            template <typename _tp>
            _v_table_t(_tp&& param) : base_v_table_t( std::forward<_tp>(param) )
                , call_op ( [](erased_param_t _vp, ARGS&&... args ) { return ( *trait<erased_t>::unerase<_tp>(_vp) ) ( std::forward< ARGS >(args)...); })
            {
                set_is_derived_from< call_operator_facade >( this );
            }
        };
        template <typename _tp>
        call_operator_facade(_tp&& v) 
            requires ( !std::derived_from< std::remove_cvref_t< _tp >, BASE< ERASED > > )
            : base_t(std::forward<_tp>(v))
        { 
            static _v_table_t _tp_v_table{ v };
            _v_table = &_tp_v_table;
        }
        template< typename OTHER >
        call_operator_facade( const OTHER& other )
            requires ( std::derived_from< OTHER, BASE< ERASED > > )
        {
            _ref = other._ref;
            _v_table = other._v_table;
        }
        RET operator()( ARGS&&... args ) requires ( !trait<ERASED>::is_const ) { return static_cast< _v_table_t* >(_v_table)->call_op( base_t::_ref, std::forward< ARGS >(args)...); }
        RET operator()( ARGS&&... args ) const { return static_cast< _v_table_t* >(_v_table)->call_op( base_t::_ref, std::forward< ARGS >(args)...); }
        call_operator_facade(const call_operator_facade&) = default;
        call_operator_facade(call_operator_facade&) = default;
        call_operator_facade(call_operator_facade&&) = default;
        static bool static_is_derived_from( const std::type_info& from ) 
        { 
            return typeid( call_operator_facade ) == from ? true : BASE< ERASED >::static_is_derived_from( from ) ; 
        } 
    protected:
        call_operator_facade() = default;
    };
    template< typename RET, typename... ARGS >
    struct call_operator_
    {
        template< typename ERASED, template < typename > typename BASE >
        using type = call_operator_facade< ERASED, BASE, RET, ARGS... >;
    };
    template< typename RET, typename... ARGS >
    using call_operator = call_operator_< RET, ARGS... >::type;
};

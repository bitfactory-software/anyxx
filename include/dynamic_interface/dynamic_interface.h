#include <memory>
#include <type_traits>

namespace dynamic_interface
{
    template< typename ERASED >
    struct trait; 

    template<>
    struct trait< void* >
    {
        using type = void*;

        using param_t = void*;

        template< typename FROM >
        static void* erase( FROM&& from )
        {
            return static_cast< std::remove_cvref_t< FROM > * >( &from );
        }
        template< typename TO >
        static auto unerase( void* from )
        {
            return static_cast< std::remove_cvref_t< TO > * >( from );
        }
    };

    template< typename ERASED >
    struct base 
    {
        using erased_t = ERASED;
        erased_t _ref = nullptr;
        struct _v_table_t 
        {
            template <typename UNUSED>
            _v_table_t(UNUSED&&){};
        };
        template <typename T>
        base(T&& v) 
            : _ref(dynamic_interface::trait<erased_t>::erase(std::forward<T>(v)))
        {}
        base(const base&) = default;
        base(base&) = default;
        base(base&&) = default;
        auto* get_erased() const { return &_ref; }
        auto* get_erased() { return &_ref; }
    };

    template< template< typename E, template< typename > typename B > typename BASE_INTERFACE >
    struct basic_
    {
        template< typename ERASED > using type = BASE_INTERFACE< ERASED, base >;
    };
    template< template< typename E, template< typename > typename B > typename BASE >
    using basic = basic_< BASE >::type;
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
type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) { \
    return _v_table->name(base_t::_ref __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
}

#define _detail_DECLARE_INTERFACE( n, delegate_lampda_limp, l) \
template< typename ERASED, template < typename > typename BASE = dynamic_interface::base > \
struct n : BASE< ERASED > \
{ \
    using erased_t = ERASED; \
    using erased_param_t = dynamic_interface::trait<ERASED>::param_t; \
    using base_t = BASE< ERASED >; \
    using base_v_table_t = base_t::_v_table_t; \
    struct _v_table_t : base_v_table_t \
    { \
        _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l)\
        template <typename _tp> \
        _v_table_t(_tp&& param) \
            : base_v_table_t( std::forward<_tp>(param) ) \
            , _detail_map_macro(delegate_lampda_limp, _detail_EXPAND_LIST l) \
         {}; \
    } * _v_table; \
    template <typename _tp> \
    n(_tp&& v)  \
    : base_t(std::forward<_tp>(v)) \
    {  \
        static _v_table_t _tp_v_table{ v }; \
        _v_table = &_tp_v_table; \
    } \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l) \
    n(const n&) = default;\
    n(n&) = default;\
    n(n&&) = default;\
};
#define DECLARE_INTERFACE( name, ...) _detail_DECLARE_INTERFACE(name, _detail_INTERFACE_MEMEBER_LIMP_H, (__VA_ARGS__))
#define DECLARE_FREE_INTERFACE( name, ...) _detail_DECLARE_INTERFACE(name, _detail_INTERFACE_FREE_LIMP_H, (__VA_ARGS__))
#define INTERFACE_METHOD(...) (__VA_ARGS__),

            //static _v_table_t _tp_v_table; \
            //_detail_map_macro(limp, _detail_EXPAND_LIST l) \
            //_v_table = &_tp_v_table; \

/*
THIS INTERFACE:
DECLARE_INTERFACE(example,
    (void, print, const char *)
)
EXPANDS TO:
    class example {
        struct _impl {
            void *_ref = nullptr;
            void (*print)(void *, const char *);
            _impl() = default;
            template <typename _tp>
            _impl(_tp &&v)
                : _ref(const_cast<
                    typename std ::remove_const<typename std ::remove_volatile<
                        typename std ::remove_reference<_tp>::type>::type>::type
                        *>(&v)),
                print([](void *_vp, const char *_sig) {
                    return static_cast<typename std ::remove_const<
                        typename std ::remove_volatile<
                            typename std ::remove_reference<_tp>::type>::type>::
                                            type *>(_vp)
                        ->print(std ::forward<decltype(_sig)>(_sig));
                }) {}
        } _body;

    public:
        example() = default;
        template <typename _tp> example(_tp &&v) : _body(v) {}
        void print(const char *_sig) {
            return print(_ref, std ::forward<decltype(_sig)>(_sig));
        }
        operator bool() { return _ref != nullptr; }
    };
*/

#include <bit_factory/anyxx.hpp>
#include <string>

#ifdef __clang__
#pragma GCC diagnostic ignored "-W#pragma-messages"
#endif

#ifndef ANY_DLL_MODE
#pragma message("ERROR! usage of component_base needs #define ANY_DLL_MODE")
#endif

#ifndef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT
#endif

#ifdef COMPONENT_BASE_SHARED
#ifdef _MSC_VER
#undef COMPONENT_BASE_EXPORT
#define COMPONENT_BASE_EXPORT __declspec(dllexport)
#pragma message("component_base -> dll")
#endif
#pragma message("component_base -> import")
#endif
static_assert(anyxx::is_in_dll_mode);

namespace test::component_base {
struct X;
}

namespace test::component_base {

struct to_string_i;
template <typename T>
struct to_string_i_default_model_map {
  static auto to_string([[maybe_unused]] T const& x)
      -> anyxx::map_return<T, std::string> {
    return x.to_string();
  };
};
template <typename T>
struct to_string_i_model_map : to_string_i_default_model_map<T> {};
template <typename T>
  requires(anyxx::is_variant<T>)
struct to_string_i_model_map<T> {
  template <typename V>
  using x_model_map = to_string_i_model_map<V>;
  static auto to_string([[maybe_unused]] T const& x) -> decltype(auto) {
    return std::visit(
        anyxx::overloads{[&]<typename V>(V&& v) {
                           return x_model_map<std::decay_t<V>>::to_string(
                               std::forward<V>(v));
                         },
                         [&]<anyxx::is_any Any>([[maybe_unused]] Any&& any) {
                           return std::forward<Any>(any).to_string();
                         }},
        x);
  };
};
struct to_string_i_has_open_dispatch;
struct to_string_i_v_table
    : anyxx::base_trait::v_table_t,
      anyxx::dispatch_holder<
          anyxx::is_type_complete<to_string_i_has_open_dispatch>, to_string_i> {
  using v_table_base_t = typename anyxx::base_trait::v_table_t;
  using v_table_t = to_string_i_v_table;
  using any_value_t = anyxx::any<anyxx::val, to_string_i>;
  static constexpr bool open_dispatch_enabeled =
      anyxx::is_type_complete<to_string_i_has_open_dispatch>;
  using own_dispatch_holder_t =
      typename anyxx::dispatch_holder<open_dispatch_enabeled, to_string_i>;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : v_table_base_t::static_is_derived_from(from);
  }
  anyxx::v_table_return<any_value_t, std::string> (*to_string)(void const*);
  ;
  ;
  template <typename Concrete>
  explicit(false) to_string_i_v_table(std::in_place_type_t<Concrete> concrete);
};
struct to_string_i : anyxx::base_trait {
  using any_value_t = anyxx::any<anyxx::val, to_string_i>;
  using base_t = anyxx::base_trait;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = to_string_i_v_table;
  template <typename StaticDispatchType>
  using static_dispatch_map_t = to_string_i_model_map<StaticDispatchType>;
  template <typename Self>
  decltype(auto) to_string(this Self&& self)
    requires(::anyxx::const_correct_call_for_proxy<
             void const*, typename std::decay_t<Self>::proxy_t, false>)
  {
    using self_t = std::decay_t<Self>;
    using T = typename self_t::T;
    using proxy_t = typename self_t::proxy_t;
    if constexpr (!self_t::dyn) {
      using traited_t = typename proxy_t::value_t;
      if constexpr (std::same_as<void, std::string>) {
        return static_dispatch_map_t<T>::to_string(get_proxy_value(self));
      } else {
        return anyxx::jacket_return<std::string>::forward(
            static_dispatch_map_t<T>::to_string(get_proxy_value(self)),
            std::forward<Self>(self));
      }
    } else {
      if constexpr (std::same_as<void, std::string>) {
        return get_v_table(self)->to_string(anyxx::get_proxy_ptr(self));
      } else {
        return anyxx::jacket_return<std::string>::forward(
            get_v_table(self)->to_string(anyxx::get_proxy_ptr(self)),
            std::forward<Self>(self));
      }
    }
  }
};
template <typename Concrete>
to_string_i_v_table::to_string_i_v_table(
    std::in_place_type_t<Concrete> concrete)
    : v_table_base_t(concrete) {
  using concept_map = to_string_i_model_map<Concrete>;
  to_string =
      [](void const* _vp) -> anyxx::v_table_return<any_value_t, std::string> {
    if constexpr (std::same_as<anyxx::self&, std::string>) {
      concept_map{}.to_string(*anyxx::unchecked_unerase_cast<Concrete>(_vp));
      return anyxx::handle_self_ref_return<std::string>{}();
    } else {
      return concept_map{}.to_string(
          *anyxx::unchecked_unerase_cast<Concrete>(_vp));
    }
  };
  ;
  ;
  if constexpr (open_dispatch_enabeled) {
    own_dispatch_holder_t::set_dispatch_table(
        ::anyxx::dispatch_table_instance<to_string_i_v_table, Concrete>());
  }
  ::anyxx::set_is_derived_from<v_table_t>(this);
};
template <typename Proxy = anyxx::default_proxy<>::type>
using any_to_string_i = anyxx::any<Proxy, to_string_i>;
ANY(get_value_i, (ANY_FN(double, get_value, (), const)), )
ANY_(set_value_i, get_value_i, (ANY_FN(void, set_value, (double), )), )

COMPONENT_BASE_EXPORT
any_to_string_i<anyxx::cref> get_to_string_i_co();
COMPONENT_BASE_EXPORT any_to_string_i<anyxx::shared> get_to_string_i_sc(
    double v);
COMPONENT_BASE_EXPORT any_to_string_i<anyxx::unique> get_to_string_i_u(double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::shared> sc_X(double v);
COMPONENT_BASE_EXPORT anyxx::any<anyxx::unique> u_X(double v);

}  // namespace test::component_base


#pragma once

#include <bit_factory/anyxx/v23/any.hpp>
#include <bit_factory/anyxx/v23/v_table_data.hpp>

namespace anyxx {

struct self {};

template <typename Param>
struct jacket_return;

template <typename Param>
  requires(!std::is_reference_v<Param>)
struct jacket_return<Param> {
  template <typename Sig>
  static Param forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
template <typename Param>
  requires std::is_reference_v<Param>
struct jacket_return<Param> {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
static_assert(!is_type_class<val<>>);
template <>
struct jacket_return<self> {
  template <typename Sig, typename Any>
  static decltype(auto) forward(Sig&& sig, Any const&) {
    using sig_t = std::decay_t<Sig>;
    if constexpr (is_type_class<typename std::decay_t<Any>::proxy_t>) {
      using target_t = anyxx::make_trait_as<typename Any::proxy_t::value_t,
                                            typename Any::trait_t>;
      if constexpr (is_any<sig_t>) {
        return target_t{get_proxy_value(std::forward<Sig>(sig))};
      } else {
        return target_t{std::forward<Sig>(sig)};
      }
    } else {
      if constexpr (is_any<sig_t> && !Any::is_dyn) {
        return Any{get_proxy_value(sig)};
      } else if constexpr (is_any<sig_t> && Any::is_dyn) {
        return unchecked_downcast_to<Any>(std::forward<Sig>(sig));
      } else {
        return Any{std::forward<Sig>(sig)};
      }
    }
  }
};
template <>
struct jacket_return<self&> {
  static auto& forward(auto, auto&& any) {
    return any;  // "return *this" semantics!
  }
};

// template <typename Param>
// struct map_return;
//
// template <typename Param>
//     requires(!std::is_reference_v<Param>)
// struct map_return<Param> {
//     template <typename Sig>
//     static Param forward(Sig&& sig) {
//         return std::forward<Sig>(sig);
//     }
// };
// template <typename Param>
//     requires (std::same_as<Param, self&> || std::same_as<Param, self>)
// struct map_return<Param> {
//     template <typename Sig>
//     static decltype(auto) forward(Sig&& sig) {
//         return std::forward<Sig>(sig);
//     }
// };
//
template <typename T>
struct translate_sig_map {
  template <typename AnyValue>
  using v_table_param = T;
  template <typename AnyValue>
  using v_table_return = T;
  template <typename AnyValue>
  using map_return = T;
  template <typename AnyValue>
  using concept_arg = T;
};
template <>
struct translate_sig_map<self> {
  template <typename AnyValue>
  using v_table_param = any<cref, dynamic_copyable>;
  template <typename AnyValue>
  using v_table_return = AnyValue;
  template <typename Model>
  using map_return = Model;
  template <typename Model>
  using concept_arg = Model;
};
template <>
struct translate_sig_map<self const&> {
  template <typename AnyValue>
  using v_table_param = any<cref, dynamic_copyable>;
  template <typename AnyValue>
  using v_table_return = int;  // dummy
  template <typename Model>
  using map_return = Model const&;
  template <typename Model>
  using concept_arg = Model const&;
};
template <>
struct translate_sig_map<self&> {
  template <typename AnyValue>
  using v_table_param = any<mutref, dynamic_copyable>;
  template <typename AnyValue>
  using v_table_return = int;  // dummy
  template <typename Model>
  using map_return = Model&;
  template <typename Model>
  using concept_arg = Model&;
};
template <typename T>
struct is_use_as__impl : std::false_type {};
template <typename T, template <typename...> typename Trait,
          typename... TraitArgs>
struct is_use_as__impl<any<using_<T>, Trait, TraitArgs...>> : std::true_type {};
template <typename T>
inline constexpr bool is_use_as_ = is_use_as__impl<std::decay_t<T>>::value;

template <typename T>
  requires is_use_as_<T>
struct translate_sig_map<T> {
  template <typename AnyValue>
  using v_table_param = any<mutref, dynamic_copyable>;
  template <typename AnyValue>
  using v_table_return = T;  // dummy
  template <typename Model>
  using map_return = T;
  template <typename Model>
  using concept_arg = typename std::decay_t<T>::rep_type;
};

template <typename AnyValue, typename Param>
using v_table_param =
    translate_sig_map<Param>::template v_table_param<AnyValue>;
template <typename AnyValue, typename Return>
using v_table_return =
    translate_sig_map<Return>::template v_table_return<AnyValue>;
template <typename Model, typename Param>
using map_return = translate_sig_map<Param>::template map_return<Model>;
template <typename Model, typename Param>
using concept_arg = translate_sig_map<Param>::template concept_arg<Model>;

template <typename T>
struct handle_self_ref_return {
  static T operator()() {
    static std::remove_reference_t<T> dummy;
    return dummy;
  }
};
template <>
struct handle_self_ref_return<void> {
  static void operator()() {}
};
template <>
struct handle_self_ref_return<self&> {
  static int operator()() { return 0; }
};

template <typename Concrete, typename T>
struct v_table_to_map {
  template <typename Sig>
  static Sig&& forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self&> {
  template <typename Sig>
  static Concrete& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self const&> {
  template <typename Sig>
  static Concrete const& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};

template <typename, typename T>
struct forward_trait_to_map {
  template <typename Sig>
  static Sig&& forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self&> {
  template <typename Sig>
  static Traited& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self const&> {
  template <typename Sig>
  static Traited const& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
template <typename Traited, typename T>
  requires is_use_as_<T>
struct forward_trait_to_map<Traited, T const&> {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
//---

}  // namespace anyxx
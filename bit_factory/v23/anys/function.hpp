#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

struct constness {};
struct const_ : constness {
  using type = const_void;
};
struct mutable_ : constness {
  using type = mutable_void;
};

template <typename Constness, typename Lifetime, typename R, typename... Args>
struct function_v_table : Lifetime::v_table_t {
  R (*f_)(typename Constness::type, Args...);
  template <typename Concrete>
  function_v_table([[maybe_unused]] std::in_place_type_t<Concrete> concrete)
      : Lifetime::v_table_t(concrete) {
    f_ = +[](typename Constness::type self_ptr, Args... args) -> R {
      return std::invoke(*unchecked_unerase_cast<Concrete>(self_ptr),
                         std::forward<Args>(args)...);
    };
  }
};

template <typename, typename>
struct function_proxy_for_lifetime {
  using type = val<>;
};
template <>
struct function_proxy_for_lifetime<observeable<>, const_> {
  using type = cref;
};
template <>
struct function_proxy_for_lifetime<observeable<>, mutable_> {
  using type = mutref;
};

template <typename Constness, typename Lifetime, typename R, typename... Args>
struct function_impl : Lifetime {
  using v_table_t = function_v_table<Constness, Lifetime, R, Args...>;
  template <typename Self>
  auto operator()(this Self&& self, Args... args) -> R
    requires(const_correct_call_for_proxy_and_self<
             typename Constness::type, typename std::decay_t<Self>::proxy_t,
             std::is_const_v<std::remove_reference_t<Self>>,
             false /*exact const*/>)
  {
    using self_t = std::decay_t<Self>;
    if constexpr (self_t::is_dyn) {
      return get_v_table(self)->f_(get_proxy_ptr(self),
                                   std::forward<Args>(args)...);
    } else {
      using T =
          typename proxy_trait<typename self_t::proxy_t>::static_dispatch_t;
      return std::invoke(get_proxy_value(std::forward<Self>(self)),
                         std::forward<Args>(args)...);
    }
  }
  using default_proxy_t =
      typename function_proxy_for_lifetime<Lifetime, Constness>::type;
};

template <typename T0, typename... Ts>
struct function;
template <typename Lifetime, typename R, typename... Args>
struct function<Lifetime, R(Args...)>
    : function_impl<mutable_, Lifetime, R, Args...> {};
template <typename Lifetime, typename R, typename... Args>
struct function<Lifetime, R(Args...) const>
    : function_impl<const_, Lifetime, R, Args...> {};
template <typename R, typename... Args>
struct function<R(Args...)>
    : function_impl<mutable_, copyable<>, R, Args...> {};
template <typename R, typename... Args>
struct function<R(Args...) const>
    : function_impl<const_, copyable<>, R, Args...> {};

namespace self_test {
struct functor {
  int operator()(double) const;
};
using f_cref = any_<function, observeable<>, int(double) const>;
static_assert(std::invocable<f_cref, double>);
using f_mutref = any_<function, observeable<>, int(double)>;
static_assert(std::invocable<f_mutref, double>);

using f_movable_const_val = any_<function, moveable<>, int(double) const>;
static_assert(std::invocable<f_movable_const_val, double>);
using f_movable_mutable_val = any_<function, moveable<>, int(double)>;
static_assert(std::invocable<f_movable_mutable_val, double>);

using f_const_val = any_<function, copyable<>, int(double) const>;
static_assert(std::invocable<f_const_val, double>);
using f_mutable_val = any_<function, copyable<>, int(double)>;
static_assert(std::invocable<f_mutable_val, double>);

using f_const_val_default = any_<function, int(double) const>;
static_assert(std::invocable<f_const_val, double>);
using f_mutable_val_default = any_<function, int(double)>;
static_assert(std::invocable<f_mutable_val, double>);

}  // namespace self_test

}  // namespace anyxx
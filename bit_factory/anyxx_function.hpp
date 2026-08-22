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

template <typename Constness, typename Lifetime, typename R, typename... Args>
struct function;
template <typename Constness, typename Lifetime, typename R, typename... Args>
struct function<R(Args...), Constness, Lifetime> : Lifetime {
  using v_table_t = function_v_table<Constness, Lifetime, R, Args...>;
  template <typename Self>
  auto operator()(this Self&& self, Args... args) -> R
    requires(const_correct_call_for_proxy_and_self<
             typename Constness::type, typename std::decay_t<Self>::proxy_t,
             std::is_const_v<std::remove_reference_t<Self>>,
             false /*exact const*/>)
  {
    using self_t = std::decay_t<Self>;
    if constexpr (self_t::dyn) {
      return get_v_table(self)->f_(get_proxy_ptr(self),
                                   std::forward<Args>(args)...);
    } else {
      using T =
          typename proxy_trait<typename self_t::proxy_t>::static_dispatch_t;
      return std::invoke(get_proxy_value(std::forward<Self>(self)),
                         std::forward<Args>(args)...);
    }
  }
  template <typename, typename>
  struct proxy_for_lifetime {
    using type = val<>;
  };
  template <>
  struct proxy_for_lifetime<observeable, const_> {
    using type = cref;
  };
  template <>
  struct proxy_for_lifetime<observeable, mutable_> {
    using type = mutref;
  };
  using default_proxy_t =
      typename proxy_for_lifetime<Lifetime, Constness>::type;
};

namespace self_test {
struct functor {
  int operator()(double) const;
};
using f_cref = any<function<int(double), const_, observeable>>;
static_assert(std::invocable<f_cref, double>);
using f_mutref = any<function<int(double), mutable_, observeable>>;
static_assert(std::invocable<f_mutref, double>);

using f_movable_const_val =
    any<function<int(double), const_, moveable>>;
static_assert(std::invocable<f_movable_const_val, double>);
using f_movable_mutable_val =
    any<function<int(double), mutable_, moveable>>;
static_assert(std::invocable<f_movable_mutable_val, double>);

using f_const_val = any<function<int(double), const_, copyable>>;
static_assert(std::invocable<f_const_val, double>);
using f_mutable_val = any<function<int(double), mutable_, copyable>>;
static_assert(std::invocable<f_mutable_val, double>);

}  // namespace self_test

}  // namespace anyxx
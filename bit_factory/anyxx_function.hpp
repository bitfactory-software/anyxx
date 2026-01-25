#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

template <typename R, typename... Args>
struct function_v_table : any_v_table {
  R (*f_)(const_void, Args...);
  template <typename Concrete>
  function_v_table([[maybe_unused]] std::in_place_type_t<Concrete> concrete)
      : any_v_table(concrete) {
    f_ = +[](const_void self_ptr, Args... args) -> R {
      auto self = static_cast<Concrete const *>(self_ptr);
      return (*self)(std::forward<Args>(args)...);
    };
  }
};
struct constness {};
struct const_ : constness {
  using type = const_void;
};
struct mutable_ : constness {
  using type = mutable_void;
};

template <typename Constness, typename R, typename... Args>
struct function;
template <typename Constness, typename R, typename... Args>
struct function<R(Args...), Constness> : emtpty_trait {
  using v_table_t = function_v_table<R, Args...>;
  template <typename Self>
  auto operator()(this Self &&self, Args... args) -> R
    requires(const_correct_call_for_proxy<typename Constness::type,
                                          typename std::decay_t<Self>::proxy_t,
                                          false /*exact const*/>)
  {
    using self_t = std::decay_t<Self>;
    if constexpr (self_t::dyn) {
      return get_v_table(self)->f_(get_proxy_ptr(self),
                                   std::forward<Args>(args)...);
    } else {
      return get_proxy_value(std::forward<Self>(self))(
          std::forward<Args>(args)...);
    }
  }
};

namespace self_test {
struct functor {
  int operator()(double) const;
};
using f_const_const = any<cref, function<int(double), const_>>;
static_assert(std::invocable<f_const_const, double>);
using f_const_mutable = any<cref, function<int(double), mutable_>>;
static_assert(!std::invocable<f_const_mutable, double>);
using f_mutable_const = any<mutref, function<int(double), const_>>;
static_assert(std::invocable<f_mutable_const, double>);
using f_mutable_mutable = any<mutref, function<int(double), mutable_>>;
static_assert(std::invocable<f_mutable_mutable, double>);

using f_const_const_val =
    any<by_val<functor const &>, function<int(double), const_>>;
static_assert(std::invocable<f_const_const_val, double>);
using f_const_mutable_val =
    any<by_val<functor const &>, function<int(double), mutable_>>;
static_assert(!std::invocable<f_const_mutable_val, double>);
using f_mutable_const_val =
    any<by_val<functor &>, function<int(double), const_>>;
static_assert(std::invocable<f_mutable_const_val, double>);
using f_mutable_mutable_val =
    any<by_val<functor &>, function<int(double), mutable_>>;
static_assert(std::invocable<f_mutable_mutable_val, double>);
}  // namespace self_test

}  // namespace anyxx
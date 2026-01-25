#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

ANY_TEMPLATE_EX(
    ((ValueType), (Reference)), forward_iterator,
    (ANY_OP(anyxx::self &, ++, (), ),
     ANY_FN_DEF(anyxx::self, post_inc, (), , ([&x]() { return x++; })),
     ANY_OP(Reference, *, (), const),
     ANY_OP_DEF(bool, ==, equal_to, (anyxx::self const &), const,
                ([&x](auto const &r) { return x == r; })),
     ANY_OP_DEF(bool, !=, not_equal_to, (anyxx::self const &), const,
                ([&x](auto const &r) { return x != r; }))),
    anyxx::val,
    (using iterator_category = std::forward_iterator_tag;
     using difference_type = std::ptrdiff_t; using value_type = ValueType;
     using reference = Reference;
     template <typename Self> auto operator++(this Self &&self, int) {
       return std::forward<Self>(self).post_inc();
     }))

TRAIT_TEMPLATE(
    ((ValueType), (Reference)), forward_range,
    (ANY_FN((any_forward_iterator<ValueType, Reference>), begin, (), const),
     ANY_FN((any_forward_iterator<ValueType, Reference>), end, (), const)))

template <typename ValueType, typename Reference, typename Box = anyxx::cref>
using any_forward_range = any<Box, forward_range<ValueType, Reference>>;

template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
//

template <typename AnyValue, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_v_table_param<AnyValue, AnyForwardRange const &> {
  using type =
      anyxx::any_forward_range<AnyValue, AnyValue,
                               //      anyxx::any_forward_range<any<val>,
                               //      any<val>,
                               typename AnyForwardRange::proxy_t> const &;
};
//
template <typename Concrete, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct v_table_to_map<Concrete, AnyForwardRange const &> {
  static auto forward(auto const &any_range) {
    return std::views::transform(any_range, [](auto const &any) {
      return *unerase_cast<Concrete>(any);
    });
  }
};
//
template <typename Traited, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct forward_trait_to_map<Traited, AnyForwardRange const &> {
  static auto forward(auto any_range) {
    return std::views::transform(any_range,
                                 []<typename T>(T const &any) -> Traited {
                                   if constexpr (is_any<T>) {
                                     if constexpr (T::dyn) {
                                       return *unerase_cast<Traited>(any);
                                     } else {
                                       return static_cast<Traited>(any);
                                     }
                                   } else {
                                     return any;
                                   }
                                 });
  }
};

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
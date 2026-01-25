#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

TRAIT_TEMPLATE_EX_(
    ((Base)), incrementable, Base, (),
    (ANY_OP(anyxx::self &, ++, (), ),
     ANY_FN_DEF(anyxx::self, post_inc, (), , ([&x]() { return x++; }))),
    (template <typename Self> auto operator++(this Self &&self, int) {
      return std::forward<Self>(self).post_inc();
    }))

// target for 0.9.1: this works!
// ANY_TEMPLATE_EX_(((ValueType), (Reference)), forward_iterator,
//                  incrementable<anyxx::base_trait>, (),
//                  (ANY_OP(Reference, *, (), const),
//                   ANY_OP_DEF(bool, ==, equal_to, (anyxx::self const &),
//                   const,
//                              ([&x](auto const &r) { return x == r; })),
//                   ANY_OP_DEF(bool, !=, not_equal_to, (anyxx::self const &),
//                              const, ([&x](auto const &r) { return x != r;
//                              }))),
//                  anyxx::val,
//                  (using iterator_category = std::forward_iterator_tag;
//                   using difference_type = std::ptrdiff_t;
//                   using value_type = ValueType; using reference =
//                   Reference;))

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

}  // namespace anyxx
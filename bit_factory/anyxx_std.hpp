#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

ANY_TEMPLATE_EX(
    ((ValueType), (Reference)), any_forward_iterator,
    (ANY_OP(anyxx::self &, ++, (), ),
     ANY_METHOD_DEFAULTED(anyxx::self, post_inc, (), ,
                          ([&x]() { return x++; })),
     ANY_OP(Reference, *, (), const),
     ANY_OP_DEFAULTED(bool, ==, equal_to, (anyxx::self const &), const,
                      ([&x](auto const &r) { return x == r; })),
     ANY_OP_DEFAULTED(bool, !=, not_equal_to, (anyxx::self const &), const,
                      ([&x](auto const &r) { return x != r; }))),
    anyxx::value, ,
    (using iterator_category = std::forward_iterator_tag;
     using difference_type = std::ptrdiff_t; using value_type = ValueType;
     using reference = Reference; auto operator++(int) { return post_inc(); }))

ANY_TEMPLATE(
    ((ValueType), (Reference)), any_forward_range,
    (ANY_METHOD((any_forward_iterator<ValueType, Reference>), begin, (), const),
     ANY_METHOD((any_forward_iterator<ValueType, Reference>), end, (), const)),
    anyxx::const_observer, )

template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
//
template <typename AnyConstObserver, typename AnyMutableObserver,
          typename AnyValue, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_v_table_param<AnyConstObserver, AnyMutableObserver, AnyValue,
                               AnyForwardRange const &> {
  using type =
      anyxx::any_forward_range<AnyValue, AnyValue,
                               typename AnyForwardRange::erased_data_t> const &;
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
    return std::views::transform(
        any_range, [](auto const &any) -> Traited { return any; });
  }
};

}  // namespace anyxx
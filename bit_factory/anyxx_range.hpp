#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

TRAIT_TEMPLATE_EX_(
    ((Base)), incrementable, Base, (),
    (ANY_OP(anyxx::self&, ++, (), ),
     ANY_FN_DEF(public, anyxx::self, post_inc, (), , ([&x]() { return x++; }))),
    , , , (template <typename Self> auto operator++(this Self&& self, int) {
      return std::forward<Self>(self).post_inc();
    }))

// target for 0.9.1: this works!
// ANY_TEMPLATE_EX_(((ValueType), (Reference)), forward_iterator,
//                  incrementable<anyxx::dynamic_value>, (),
//                  (ANY_OP(Reference, *, (), const),
//                   ANY_OP_DEF(bool, ==, equal_to, (anyxx::self const &),
//                   const,
//                              ([&x](auto const &r) { return x == r; })),
//                   ANY_OP_DEF(bool, !=, not_equal_to, (anyxx::self const &),
//                              const, ([&x](auto const &r) { return x != r;
//                              }))),
//                  anyxx::val<>,
//                  (using iterator_category = std::forward_iterator_tag;
//                   using difference_type = std::ptrdiff_t;
//                   using value_type = ValueType; using reference =
//                   Reference;))

struct forward_iterator_is_nullable {};
TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), forward_iterator, dynamic_value, (),
    (ANY_OP(anyxx::self&, ++, (), ),
     ANY_FN_DEF(public, anyxx::self, post_inc, (), , ([&x]() { return x++; })),
     ANY_OP(Reference, *, (), const),
     ANY_OP_DEF(public, bool, ==, equal_to, (anyxx::self const&), const,
                ([&x](auto const& r) { return x == r; })),
     ANY_OP_DEF(public, bool, !=, not_equal_to, (anyxx::self const&), const,
                ([&x](auto const& r) { return x != r; }))),
    , , ,
    (using iterator_category = std::forward_iterator_tag;
     using difference_type = std::ptrdiff_t; using value_type = ValueType;
     using reference = Reference;
     template <typename Self> auto operator++(this Self&& self, int) {
       return std::forward<Self>(self).post_inc();
     }))

template <typename ValueType, typename Reference, typename Proxy = nullable_val>
using any_forward_iterator = any<forward_iterator<ValueType, Reference>, Proxy>;

TRAIT_TEMPLATE_(
    ((ValueType), (Reference)), forward_range, dynamic_value, (),
    (ANY_FN((any_forward_iterator<ValueType, Reference>), begin, (), const),
     ANY_FN((any_forward_iterator<ValueType, Reference>), end, (), const)))

template <typename ValueType, typename Reference, typename Proxy = anyxx::cref>
using any_forward_range = any<forward_range<ValueType, Reference>, Proxy>;

template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
//

template <typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_sig_model_map<AnyForwardRange const&>
    : translate_sig_default_model_map<self> {
  template <typename AnyValue>
  using v_table_param =
      any_forward_range<AnyValue, AnyValue,
                        typename AnyForwardRange::proxy_t> const&;
  template <typename Model>
  using concept_arg = anyxx::any_forward_range<Model, Model, anyxx::cref>;
};

template <typename Concrete, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct v_table_to_map<Concrete, AnyForwardRange const&> {
  static auto forward(auto const& any_range) {
    return std::views::transform(any_range, [](auto const& any) {
      return *unerase_cast<Concrete>(any);
    });
  }
};
//
template <typename Traited, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct forward_trait_to_map<Traited, AnyForwardRange const&> {
  static auto forward(auto any_range) {
    return std::views::transform(any_range,
                                 []<typename T>(T const& any) -> Traited {
                                   if constexpr (is_any<T>) {
                                     if constexpr (T::dyn) {
                                       return *unerase_cast<Traited>(any);
                                     } else {
                                       return get_proxy_value(any);
                                     }
                                   } else {
                                     return any;
                                   }
                                 });
  }
};


static_assert(any_forward_iterator<self, self>::v_table_t::val_nullable::value == true);
static_assert(std::same_as<any_forward_iterator<self, self>::v_table_t::any_value_t, any<forward_iterator<self,self>, nullable_val>>);
static_assert(std::forward_iterator<any_forward_iterator<self, self>>);
static_assert(std::ranges::forward_range<any_forward_range<self, self>>);

}  // namespace anyxx
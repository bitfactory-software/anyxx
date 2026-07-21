#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

inline static constexpr std::size_t iterator_val_proxy_size =
    small_object_size * 2u;

struct forward_iterator_is_nullable {};
struct forward_iterator_val_size {
  static constexpr std::size_t value = iterator_val_proxy_size;
};

struct deduce_type {};

template <typename I, typename IteratorCategory>
struct iterator_category_impl {
  using type = std::conditional_t<std::same_as<IteratorCategory, deduce_type>,
                                  typename std::iterator_traits<I>::value_type,
                                  IteratorCategory>;
};
template <typename IteratorCategory>
struct iterator_category_impl<void*, IteratorCategory> {
  using type = std::forward_iterator_tag;
};
template <typename I>
struct difference_type_impl {
  using type = typename std::iterator_traits<I>::difference_type;
};
template <>
struct difference_type_impl<void*> {
  using type = std::ptrdiff_t;
};
template <typename I, typename ValueType>
struct value_type_impl {
  using type = std::conditional_t<std::same_as<ValueType, deduce_type>,
                                  typename std::iterator_traits<I>::value_type,
                                  ValueType>;
};
template <typename ValueType>
struct value_type_impl<void*, ValueType> {
  using type = ValueType;
};
template <typename I, typename Reference>
struct reference_impl {
  using type = std::conditional_t<std::same_as<Reference, deduce_type>,
                                  typename std::iterator_traits<I>::reference,
                                  Reference>;
};
template <typename Reference>
struct reference_impl<void*, Reference> {
  using type = Reference;
};

template <typename T>
concept has_post_increment = requires(T x) {
  { x++ } -> std::same_as<T>;
};

template <typename T>
auto post_increment_def = [](T& self) -> T {
  if constexpr (has_post_increment<T>) {
    return self++;
  } else {
    auto r = self;
    ++self;
    return r;
  }
};

TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), forward_iterator, dynamic_value, (),
    (ANY_OP_MAP_NAMED(anyxx::self&, ++, op_pre_increment, (), ),
     ANY_FN_DEF(public, anyxx::self, post_increment, (), ,
                std::bind(post_increment_def<T>, x)),
     ANY_OP_DEF(public, typename deduced_type::reference, *, op_dereference, (),
                const,
                ([&x]() -> typename deduced_type::reference { return *x; })),
     ANY_OP_MAP_NAMED_FRIEND(bool, ==, equal, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, !=, inequal, (anyxx::self const&), const)),
    ,
    (ANY_TYPE((), iterator_category, std::forward_iterator_tag,
              (std::forward_iterator_tag)),
     ANY_TYPE((), difference_type, std::ptrdiff_t,
              (difference_type_impl<T>::type)),
     ANY_TYPE((), value_type, ValueType, (value_type_impl<T, ValueType>::type)),
     ANY_TYPE((), reference, Reference, (reference_impl<T, Reference>::type))),
    , (template <typename Self> auto operator++(this Self&& self, int) {
      return std::forward<Self>(self).post_increment();
    }))

template <typename ValueType, typename Reference,
          typename Proxy = val<std::true_type, iterator_val_proxy_size>>
using any_forward_iterator = any<forward_iterator<ValueType, Reference>, Proxy>;

TRAIT_TEMPLATE_(((AnyIterator)), range, dynamic_value, (),
                (ANY_FN(AnyIterator, begin, (), const),
                 ANY_FN(AnyIterator, end, (), const)))

template <typename AnyIterator, typename Proxy = anyxx::cref>
using any_range = any<range<AnyIterator>, Proxy>;

template <typename ValueType, typename Reference = ValueType, typename Proxy = anyxx::cref>
using any_forward_range =
    any_range<any_forward_iterator<ValueType, Reference>, Proxy>;

using any_self_forward_range = any_forward_range<self, self>;
template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
static_assert(is_any_self_forward_range<any_self_forward_range>);

template <typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_sig_map<AnyForwardRange const&> : translate_sig_map<self> {
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

static_assert(
    any_forward_iterator<self, self>::v_table_t::val_nullable::value == true);
static_assert(
    std::same_as<any_forward_iterator<self, self>::v_table_t::any_value_t,
                 any<forward_iterator<self, self>,
                     val<std::true_type, iterator_val_proxy_size>>>);
static_assert(std::forward_iterator<any_forward_iterator<self, self>>);
static_assert(std::ranges::forward_range<any_forward_range<self, self>>);

template <typename Trait, typename R>
auto trait_range_as(R&& r) {
  return r | std::views::transform(
                 [&](auto const& x) { return trait_as<Trait>(x); });
}

}  // namespace anyxx
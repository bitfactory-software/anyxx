#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx {

inline static constexpr std::size_t iterator_val_proxy_size =
    small_object_size * 2u;
using iterator_val = nullable_val<iterator_val_proxy_size>;

struct input_iterator_is_nullable {};
struct input_iterator_val_size {
  static constexpr std::size_t value = iterator_val_proxy_size;
};

struct deduce_type {};

template <typename I, typename IteratorCategory>
struct iterator_category_impl {
  using type =
      std::conditional_t<std::same_as<IteratorCategory, deduce_type>,
                         typename std::iterator_traits<I>::iterator_category,
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

TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), input_iterator, dynamic_copyable, (),
    (ANY_OP_MAP_NAMED(anyxx::self&, ++, op_pre_increment, (), ),
     ANY_OP_MAP_NAMED_UNARY(Reference, *, op_dereference, const),
     ANY_FN_DEF(public, bool, equal, (anyxx::self const&), const,
                [&x](auto const& rhs) { return x == rhs; })),
    ,
    (ANY_TYPE((), iterator_category, std::input_iterator_tag,
              (std::input_iterator_tag)),
     ANY_TYPE((), difference_type, std::ptrdiff_t,
              (difference_type_impl<T>::type)),
     ANY_TYPE((), value_type, ValueType, (value_type_impl<T, ValueType>::type)),
     ANY_TYPE((), reference, Reference, (reference_impl<T, Reference>::type))),
    , (using default_proxy_t = iterator_val;))

template <typename It>
concept has_iterator_category = requires { typename It::iterator_category; };
template <typename ValueType, typename Reference = ValueType&>
using any_input_iterator = any_<input_iterator, ValueType, Reference>;
template <template <typename...> typename It, typename Proxy, typename... Args>
concept is_compareable_iterator =
    (has_iterator_category<any<Proxy, It, Args...>> &&
     is_any_derived_from<any_<It, Args...>,
                         any_<input_iterator, Args...>>);
template <template <typename...> typename It, typename Proxy, typename... Args>
  requires is_compareable_iterator<It, Proxy, Args...>
bool operator==(any<Proxy, It, Args...> const& lhs,
                any<Proxy, It, Args...> const& rhs) {
  return lhs.equal(rhs);
}
template <template <typename...> typename It, typename Proxy, typename... Args>
  requires is_compareable_iterator<It, Proxy, Args...>
bool operator!=(any<Proxy, It, Args...> const& lhs,
                any<Proxy, It, Args...> const& rhs) {
  return !(rhs == lhs);
}

TRAIT_TEMPLATE_EX_(((ValueType), (Reference)), forward_iterator, input_iterator,
                   ((ValueType), (Reference)), , ,
                   (ANY_TYPE((), iterator_category, std::forward_iterator_tag,
                             (std::forward_iterator_tag))),
                   , ())

template <typename ValueType, typename Reference = ValueType&>
using any_forward_iterator = any_<forward_iterator, ValueType, Reference>;

TRAIT_TEMPLATE_EX_(((ValueType), (Reference)), bidirectional_iterator,
                   forward_iterator, ((ValueType), (Reference)),
                   (ANY_OP_MAP_NAMED(anyxx::self&, --, op_pre_decrement, (), )),
                   ,
                   (ANY_TYPE((), iterator_category,
                             std::bidirectional_iterator_tag,
                             (std::bidirectional_iterator_tag))),
                   , ())

template <typename ValueType, typename Reference = ValueType&>
using any_bidirectional_iterator =
    any_<bidirectional_iterator, ValueType, Reference>;

TRAIT_TEMPLATE_EX_(
    ((ValueType), (Reference)), random_access_iterator, bidirectional_iterator,
    ((ValueType), (Reference)),
    (ANY_OP_MAP_NAMED_FRIEND(bool, <, less, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, >, greater, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, <=, less_equal, (anyxx::self const&), const),
     ANY_OP_MAP_NAMED_FRIEND(bool, >=, greater_equal, (anyxx::self const&),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self&, +=, plus_assign, (std::ptrdiff_t),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self&, -=, minus_assign, (std::ptrdiff_t),
                             const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self, +, plus, (std::ptrdiff_t), const),
     ANY_OP_MAP_NAMED_FRIEND(anyxx::self, -, minus, (std::ptrdiff_t), const),
     ANY_FN_DEF(public, std::ptrdiff_t, difference_to, (anyxx::self const&),
                const, [&x](auto const& rhs) { return x - rhs; }),
     ANY_OP_MAP_NAMED(Reference, [], at, (std::ptrdiff_t), const)),
    ,
    (ANY_TYPE((), iterator_category, std::random_access_iterator_tag,
              (std::random_access_iterator_tag))),
    , ())

template <typename ValueType, typename Reference = ValueType&>
using any_random_access_iterator =
    any_<random_access_iterator, ValueType, Reference>;

template <typename Value, typename Ref>
auto operator-(any_random_access_iterator<Value, Ref> const& lhs,
               any_random_access_iterator<Value, Ref> const& rhs) {
  return lhs.difference_to(rhs);
}
template <typename Value, typename Ref>
auto operator+(std::ptrdiff_t n,
               any_random_access_iterator<Value, Ref> const& it) {
  return it + n;
}

}  // namespace anyxx
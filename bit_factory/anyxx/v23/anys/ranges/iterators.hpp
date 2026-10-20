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

TRAIT_TEMPLATE_EX(
    ((ValueType), (Reference)), input_iterator, dynamic_copyable, (),
    (UNARY_OP(anyxx::self&, ++, op_pre_increment, ),
     UNARY_OP(Reference, *, op_dereference, const),
     FN_IMPL(public, bool, equal, (anyxx::self const&), const,
             (auto const& rhs) { return self == rhs; })),
    ,
    (USING((), iterator_category, std::input_iterator_tag,
           (std::input_iterator_tag)),
     USING((), difference_type, std::ptrdiff_t,
           (difference_type_impl<T>::type)),
     USING((), value_type, ValueType, (value_type_impl<T, ValueType>::type)),
     USING((), reference, Reference, (reference_impl<T, Reference>::type))),
    , (using default_proxy_t = iterator_val;))

template <typename It>
concept has_iterator_category = requires { typename It::iterator_category; };
template <typename ValueType, typename Reference = ValueType&>
using any_input_iterator = any<input_iterator, ValueType, Reference>;
template <template <typename...> typename It, typename Proxy, typename... Args>
concept is_compareable_iterator =
    (has_iterator_category<any_<Proxy, It, Args...>> &&
     is_any_derived_from<any<It, Args...>, any<input_iterator, Args...>>);
template <template <typename...> typename It, typename Proxy, typename... Args>
  requires is_compareable_iterator<It, Proxy, Args...>
bool operator==(any_<Proxy, It, Args...> const& lhs,
                any_<Proxy, It, Args...> const& rhs) {
  return lhs.equal(rhs);
}
template <template <typename...> typename It, typename Proxy, typename... Args>
  requires is_compareable_iterator<It, Proxy, Args...>
bool operator!=(any_<Proxy, It, Args...> const& lhs,
                any_<Proxy, It, Args...> const& rhs) {
  return !(rhs == lhs);
}

TRAIT_TEMPLATE_EX(((ValueType), (Reference)), forward_iterator, input_iterator,
                  ((ValueType), (Reference)), , ,
                  (USING((), iterator_category, std::forward_iterator_tag,
                         (std::forward_iterator_tag))),
                  , ())

template <typename ValueType, typename Reference = ValueType&>
using any_forward_iterator = any<forward_iterator, ValueType, Reference>;

TRAIT_TEMPLATE_EX(((ValueType), (Reference)), bidirectional_iterator,
                  forward_iterator, ((ValueType), (Reference)),
                  (UNARY_OP(anyxx::self&, --, op_pre_decrement, )), ,
                  (USING((), iterator_category, std::bidirectional_iterator_tag,
                         (std::bidirectional_iterator_tag))),
                  , ())

template <typename ValueType, typename Reference = ValueType&>
using any_bidirectional_iterator =
    any<bidirectional_iterator, ValueType, Reference>;

TRAIT_TEMPLATE_EX(
    ((ValueType), (Reference)), random_access_iterator, bidirectional_iterator,
    ((ValueType), (Reference)),
    (OP_FREE(bool, <, less, (anyxx::self const&), const),
     OP_FREE(bool, >, greater, (anyxx::self const&), const),
     OP_FREE(bool, <=, less_equal, (anyxx::self const&), const),
     OP_FREE(bool, >=, greater_equal, (anyxx::self const&), const),
     OP(anyxx::self&, +=, plus_assign, (std::ptrdiff_t), ),
     OP(anyxx::self&, -=, minus_assign, (std::ptrdiff_t), ),
     OP(anyxx::self, +, plus, (std::ptrdiff_t), const),
     OP(anyxx::self, -, minus, (std::ptrdiff_t), const),
     FN_IMPL(public, std::ptrdiff_t, difference_to, (anyxx::self const&), const,
             (auto const& rhs) { return self - rhs; }),
     OP(Reference, [], at, (std::ptrdiff_t), const)),
    ,
    (USING((), iterator_category, std::random_access_iterator_tag,
           (std::random_access_iterator_tag))),
    , ())

template <typename ValueType, typename Reference = ValueType&>
using any_random_access_iterator =
    any<random_access_iterator, ValueType, Reference>;

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

TRAIT_TEMPLATE_EX(((ValueType), (Reference)), contiguous_iterator,
                  random_access_iterator, ((ValueType), (Reference)),
                  (OP(ValueType*, ->, op_arrow, (), const)), ,
                  (USING((), iterator_category, std::contiguous_iterator_tag,
                         (std::contiguous_iterator_tag))),
                  , ())

template <typename ValueType, typename Reference = ValueType&>
using any_contiguous_iterator = any<contiguous_iterator, ValueType, Reference>;

template <typename Value, typename Ref>
auto operator-(any_contiguous_iterator<Value, Ref> const& lhs,
    any_contiguous_iterator<Value, Ref> const& rhs) {
    return lhs.difference_to(rhs);
}
template <typename Value, typename Ref>
auto operator+(std::ptrdiff_t n,
               any_contiguous_iterator<Value, Ref> const& it) {
  return it + n;
}

}  // namespace anyxx
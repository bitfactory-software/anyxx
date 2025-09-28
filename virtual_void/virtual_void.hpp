#pragma once

#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#undef interface

namespace virtual_void {

template <typename>
struct self_pointer;
template <>
struct self_pointer<void*> {
  template <typename CLASS>
  using type = CLASS*;
};
template <>
struct self_pointer<const void*> {
  template <typename CLASS>
  using type = const CLASS*;
};

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

template <class BASE, class DERIVED>
concept base_of = std::derived_from<DERIVED, BASE>;

template <typename DATA>
struct erased_data_trait;

template <class E>
concept is_erased_data = requires(E e) {
  typename erased_data_trait<E>::void_t;
  {
    erased_data_trait<E>::is_constructibile_from_const
  } -> std::convertible_to<bool>;
  {
    erased_data_trait<E>::value(e)
  } -> std::convertible_to<typename erased_data_trait<E>::void_t>;
  { erased_data_trait<E>::has_value(e) } -> std::convertible_to<bool>;
};

struct mutable_ {};
struct const_ {};
template <typename CS>
concept is_constness = (std::same_as<CS, mutable_> || std::same_as<CS, const_>);

using const_void = void const*;
using mutable_void = void*;
template <typename V>
concept voidness =
    (std::same_as<V, const_void> || std::same_as<V, mutable_void>);

template <typename X>
concept is_ereasurness = is_constness<X> || voidness<X>;

template <is_ereasurness ERASURENESS>
struct void_;
template <>
struct void_<mutable_> {
  using type = void*;
};
template <>
struct void_<const_> {
  using type = void const*;
};
template <>
struct void_<mutable_void> {
  using type = void*;
};
template <>
struct void_<const_void> {
  using type = void const*;
};
template <is_ereasurness ERASURENESS>
using void_t = void_<ERASURENESS>::type;

template <is_ereasurness ERASURENESS>
struct constness_type;
template <>
struct constness_type<mutable_> {
  using type = mutable_;
};
template <>
struct constness_type<const_> {
  using type = const_;
};
template <>
struct constness_type<mutable_void> {
  using type = mutable_;
};
template <>
struct constness_type<const_void> {
  using type = const_;
};
template <is_ereasurness ERASURENESS>
using const_t = constness_type<ERASURENESS>::type;

template <is_ereasurness ERASURENESS>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};
template <>
struct is_const_void_<mutable_> : std::false_type {};
template <>
struct is_const_void_<const_> : std::true_type {};
template <typename ERASURENESS>
concept is_const_void = is_const_void_<ERASURENESS>::value;

template <typename DATA>
using data_void = erased_data_trait<DATA>::void_t;
template <typename DATA>
using data_const_t = const_t<typename erased_data_trait<DATA>::void_t>;

template <typename ERASED_DATA>
concept is_const_data = is_const_void<data_void<ERASED_DATA>>;

template <bool CALL_IS_CONST, bool ERASED_DATA_IS_CONST>
concept const_correct_call =
    ((CALL_IS_CONST == ERASED_DATA_IS_CONST) || !ERASED_DATA_IS_CONST);

template <typename CALL, typename ERASED_DATA>
concept const_correct_call_for_erased_data =
    is_ereasurness<CALL> && is_erased_data<ERASED_DATA> &&
    (const_correct_call<is_const_void<CALL>, is_const_data<ERASED_DATA>>);

template <is_erased_data ERASED_DATA, typename FROM>
ERASED_DATA erased(FROM&& from) {
  return erased_data_trait<ERASED_DATA>::construct_from(
      std::forward<FROM>(from));
}
template <is_erased_data ERASED_DATA, typename FROM>
ERASED_DATA erase_to(FROM&& from) {
  if constexpr (is_erased_data<std::remove_reference_t<FROM>>) {
    return from;
  } else {
    return erased<ERASED_DATA>(from);
  }
}
template <is_erased_data ERASED_DATA, typename V, typename... ARGS>
ERASED_DATA erased_in_place(ARGS&&... args) {
  return erased_data_trait<ERASED_DATA>::construct_in_place(
      std::in_place_type<V>, std::forward<ARGS>(args)...);
}

struct virtual_void_default_unerase {
  template <typename CONSTRUCTED_WITH>
  using unerased_type = CONSTRUCTED_WITH;
};

template <typename UNERASER>
concept is_uneraser = requires(UNERASER u, mutable_void mv) {
  { u(mv) } -> std::convertible_to<typename UNERASER::type*>;
};
template <typename T>
struct static_cast_uneraser {
  using type = T;
  auto operator()(mutable_void from) { return static_cast<T*>(from); }
  auto operator()(const_void from) { return static_cast<T const*>(from); }
};
template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
struct unerased {
  using constructed_with_t = std::remove_cvref_t<CONSTRUCTED_WITH>;
  using trait_t = erased_data_trait<ERASED_DATA>;
  using type = trait_t::template unerased_type<constructed_with_t>;
};
template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
using unerased_type = unerased<ERASED_DATA, CONSTRUCTED_WITH>::type;

template <is_constness CONSTNESS, typename T>
struct const_qualified_ {
  using type = T;
};
template <typename T>
struct const_qualified_<const_, T> {
  using type = T const;
};
template <is_constness CONSTNESS, typename T>
using const_qualified = typename const_qualified_<CONSTNESS, T>::type;

template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH, bool is_const>
struct make_uneraser;

template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
struct make_uneraser<ERASED_DATA, CONSTRUCTED_WITH, true> {
  using unerased = unerased_type<ERASED_DATA, CONSTRUCTED_WITH>;
  using type = static_cast_uneraser<unerased const>;
};
template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH>
struct make_uneraser<ERASED_DATA, CONSTRUCTED_WITH, false> {
  using unerased = unerased_type<ERASED_DATA, CONSTRUCTED_WITH>;
  using type = static_cast_uneraser<unerased>;
};
template <is_erased_data ERASED_DATA, typename CONSTRUCTED_WITH,
          bool is_const = is_const_data<ERASED_DATA>>
using uneraser = make_uneraser<ERASED_DATA, CONSTRUCTED_WITH, is_const>::type;

template <is_erased_data ERASED_DATA>
bool has_data(ERASED_DATA const& vv) {
  return erased_data_trait<ERASED_DATA>::has_value(vv);
}
template <is_erased_data ERASED_DATA>
void const* get_data(ERASED_DATA const& vv)
  requires std::same_as<void const*,
                        typename erased_data_trait<ERASED_DATA>::void_t>
{
  return erased_data_trait<ERASED_DATA>::value(vv);
}
template <is_erased_data ERASED_DATA>
void* get_data(ERASED_DATA const& vv)
  requires std::same_as<void*, typename erased_data_trait<ERASED_DATA>::void_t>
{
  return erased_data_trait<ERASED_DATA>::value(vv);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o) {
  return unchecked_unerase_cast<U>(get_data(o));
}
template <typename U, is_erased_data ERASED_DATA>
auto unchecked_unerase_cast(ERASED_DATA const& o)
  requires(!is_const_data<ERASED_DATA>)
{
  return unchecked_unerase_cast<U>(get_data(o));
}

namespace runtime {
class meta_data;
}

template <typename U>
bool type_match(runtime::meta_data const& meta);

class type_mismatch_error : error {
  using error::error;
};

template <typename U>
void check_type_match(runtime::meta_data const& meta) {
  if (!type_match<U>(meta)) throw type_mismatch_error("type mismatch");
}
template <typename U, is_erased_data ERASED_DATA>
auto unerase_cast(ERASED_DATA const& o, runtime::meta_data const& meta) {
  check_type_match<U>(meta);
  return unchecked_unerase_cast<U>(o);
}
template <typename U, is_erased_data ERASED_DATA>
U const* unerase_cast(ERASED_DATA const* o, runtime::meta_data const& meta) {
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}
template <typename U, is_erased_data ERASED_DATA>
U* unerase_cast(ERASED_DATA const* o, runtime::meta_data const& meta)
  requires(!is_const_data<ERASED_DATA>)
{
  if (type_match<U>(meta)) return unchecked_unerase_cast<U>(*o);
  return nullptr;
}

template <typename CONSTRUCTED_WITH, typename ERASED_DATA, typename BASE>
concept erased_constructibile_for =
    !std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, BASE> &&
    !is_erased_data<std::remove_cvref_t<CONSTRUCTED_WITH>> &&
    (!std::is_const_v<std::remove_reference_t<CONSTRUCTED_WITH>> ||
     erased_data_trait<ERASED_DATA>::is_constructibile_from_const);

}  // namespace virtual_void
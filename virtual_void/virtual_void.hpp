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

}  // namespace virtual_void
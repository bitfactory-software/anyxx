#pragma once

#include <stdexcept>
#include <type_traits>

#include "../concept.h"
#include "../data/has_no_meta.h"
#include "../data/has_type_info.h"

namespace virtual_void::erased {

template <typename VOID>
struct is_const_void {};
template <>
struct is_const_void<void*> {
  static constexpr bool value = false;
};
template <>
struct is_const_void<void const*> {
  static constexpr bool value = true;
};

template <typename META>
struct make_mutable_observer;
template <typename META>
struct make_const_observer;

template <bool, typename META>
struct select_make_observer;

template <typename META>
struct select_make_observer<false, META> {
  using type = make_mutable_observer<META>;
};
template <typename META>
struct select_make_observer<true, META> {
  using type = make_const_observer<META>;
};
template <bool is_const, typename META>
using select_make_observer_t = select_make_observer<is_const, META>::type;

template <typename VOID, typename META>
struct observer : META {
  using void_t = VOID;
  static constexpr bool is_const = is_const_void<VOID>::value;
  using make_erased = select_make_observer_t<is_const, META>;

  observer(const observer&) = default;
  observer(observer&) = default;
  observer(observer&&) = default;
  template <typename T>
  observer(T&& v)
    requires(!std::derived_from<std::decay_t<T>, observer>)
      : META(std::in_place_type<T>), data_(&v) {}
  observer(void_t v, const META& meta) : data_(v), META(meta) {}
  VOID data_ = nullptr;
  VOID data() const { return data_; }
};
template <typename META = data::has_no_meta>
using const_observer = observer<void const*, META>;
template <typename META = data::has_no_meta>
using mutable_observer = observer<void*, META>;
static_assert(const_observer<>::is_const);
static_assert(!mutable_observer<>::is_const);
static_assert(const_observer<data::has_type_info>::is_const);
static_assert(!mutable_observer<data::has_type_info>::is_const);

template <typename U, typename META>
auto reconcrete_cast(mutable_observer<META> o) {
  return static_cast<U*>(o.data());
}
template <typename U, typename META>
auto reconcrete_cast(const_observer<META> o) {
  return static_cast<const U*>(o.data());
}

template <typename T, typename META = data::has_no_meta>
struct select_observer {
  using type = mutable_observer<META>;
};
template <typename T, typename META>
struct select_observer<T const, META> {
  using type = const_observer<META>;
};
template <typename T, typename META = data::has_no_meta>
using select_observer_t = select_observer<T, META>::type;

template <typename T, typename META = data::has_no_meta>
struct typed_observer
    : public select_observer_t<std::remove_reference_t<T>, META> {
  using conrete_t = std::remove_reference_t<T>;
  using observer_t = select_observer_t<T, META>;
  typed_observer(const typed_observer&) = default;
  typed_observer(typed_observer&) = default;
  typed_observer(typed_observer&&) = default;
  typed_observer(const observer_t& o)
      : select_observer_t<std::remove_reference_t<T>, META>(o) {}
  typed_observer(T&& v)
      : select_observer_t<std::remove_reference_t<T>, META>(std::forward<T>(v)) {}
  conrete_t& operator*() const {
    return *static_cast<conrete_t*>(this->data());
  }
  conrete_t* operator->() const {
    return static_cast<conrete_t*>(this->data());
  }
};

template <typename META>
struct make_mutable_observer {
  template <typename FROM>
  mutable_observer<META> operator()(FROM&& from) {
    return typed_observer<std::remove_const_t<std::remove_reference_t<FROM>>,
                          META>(std::forward<FROM>(from));
  }
};
template <typename META>
struct make_const_observer {
  template <typename FROM>
  const_observer<META> operator()(FROM&& from) {
    return typed_observer<std::add_const_t<std::remove_reference_t<FROM>>,
                          META>(from);
  }
};

static_assert(is_erased_lifetime_holder<mutable_observer<>>);
static_assert(is_erased_lifetime_holder<const_observer<>>);
static_assert(is_erased_lifetime_holder<mutable_observer<data::has_type_info>>);
static_assert(is_erased_lifetime_holder<const_observer<data::has_type_info>>);
static_assert(
    is_erased_lifetime_holder<typed_observer<int const, data::has_type_info>>);
static_assert(
    is_erased_lifetime_holder<typed_observer<int, data::has_type_info>>);

}  // namespace virtual_void::erased
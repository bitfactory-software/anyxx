#pragma once

#include <stdexcept>
#include <type_traits>

#include "../concept.h"

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

struct make_mutable_observer;
struct make_const_observer;

template <bool>
struct select_make_observer {
  using type = make_mutable_observer;
};
template <>
struct select_make_observer<true> {
  using type = make_const_observer;
};
template <bool is_const>
using select_make_observer_t = select_make_observer<is_const>::type;

template <typename VOID>
struct observer {
  using void_t = VOID;
  static constexpr bool is_const = is_const_void<VOID>::value;
  using make_erased = select_make_observer_t<is_const>;

  observer(const observer&) = default;
  observer(observer&) = default;
  observer(observer&&) = default;
  template <typename T>
  observer(T&& v)
    requires(!std::derived_from<T, observer>)
      : data_(&v) {}
  observer(void_t v) : data_(v) {}
  VOID data_ = nullptr;
  VOID data() const { return data_; }
};
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;
static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

template <typename U>
auto reconcrete_cast(mutable_observer o) {
  return static_cast<U*>(o.data());
}
template <typename U>
auto reconcrete_cast(const_observer o) {
  return static_cast<const U*>(o.data());
}

template <typename T>
struct select_observer {
  using type = mutable_observer;
};
template <typename T>
struct select_observer<T const> {
  using type = const_observer;
};
template <typename T>
using select_observer_t = select_observer<T>::type;

template <typename T>
struct typed_observer : public select_observer_t<std::remove_reference_t<T> > {
  using conrete_t = std::remove_reference_t<T>;
  using observer_t = select_observer_t<T>;
  typed_observer(const typed_observer&) = default;
  typed_observer(typed_observer&) = default;
  typed_observer(typed_observer&&) = default;
  typed_observer(const observer_t& o)
      : select_observer_t<std::remove_reference_t<T> >(o) {}
  typed_observer(T&& v)
      : select_observer<std::remove_reference_t<T> >(std::forward<T>(v)) {}
  conrete_t& operator*() const {
    return *static_cast<conrete_t*>(this->data());
  }
  conrete_t* operator->() const {
    return static_cast<conrete_t*>(this->data());
  }
};

struct make_mutable_observer {
  template <typename FROM>
  mutable_observer operator()(FROM&& from) {
    return typed_observer<std::remove_const_t<std::remove_reference_t<FROM> > >(
        std::forward<FROM>(from));
  }
};
struct make_const_observer {
  template <typename FROM>
  const_observer operator()(FROM&& from) {
    return typed_observer<std::add_const_t<std::remove_reference_t<FROM> > >(
        from);
  }
};

static_assert(is_erased_lifetime_holder<mutable_observer>);
static_assert(is_erased_lifetime_holder<const_observer>);
static_assert(is_erased_lifetime_holder<typed_observer<int const> >);
static_assert(is_erased_lifetime_holder<typed_observer<int> >);

}  // namespace virtual_void::erased
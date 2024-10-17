#pragma once

#include <stdexcept>
#include <type_traits>

#include "data.h"

namespace virtual_void::erased {

struct abstract_value_data_cloneable;
using abstract_value_data = abstract_data_t<abstract_value_data_cloneable>;
using value_abstract_data_ptr = std::unique_ptr<abstract_value_data>;
using abstract_value_data_clone_f =
    value_abstract_data_ptr (*)(abstract_value_data const*);
struct abstract_value_data_cloneable {
  abstract_value_data_clone_f clone_;
};
template <typename M>
struct concrete_value_data : concrete_data_t<M, abstract_value_data_cloneable> {
  static value_abstract_data_ptr clone_impl(
      abstract_value_data const* from_abstract) {
    const auto from = static_cast<const concrete_value_data*>(from_abstract);
    auto cloned = std::make_unique<concrete_value_data>(*from);
    cloned->data_ = &cloned->m_;
    return cloned;
  };
  concrete_value_data(M&& m)
      : concrete_data_t<M, abstract_value_data_cloneable>(std::move(m)) {
    this->clone_ = &clone_impl;
  }
  template <typename... ARGS>
  concrete_value_data(std::in_place_t in_place, ARGS&&... args)
      : concrete_data_t<M, abstract_value_data_cloneable>(
            in_place, std::forward<ARGS>(args)...) {
    this->clone_ = &clone_impl;
  }
};

inline value_abstract_data_ptr clone(abstract_value_data const* other) {
  return other->clone_(other);
};

struct make_value_t;

class value {
  value_abstract_data_ptr ptr_;

 protected:
  explicit value(value_abstract_data_ptr&& ptr) : ptr_(std::move(ptr)) {}

 public:
  using void_t = void*;
  static constexpr bool is_const = false;
  using make_erased = make_value_t;

  value() = default;
  value(value&&) = default;
  value& operator=(value&& ptr) = default;
  value(const value& other) : ptr_(clone(other.ptr_.get())) {}
  value& operator=(const value& other) {
    if (this != &other) {
      value tmp(other);
      using std::swap;
      swap(*this, tmp);
    }
    return *this;
  }
  value(value& other) : value(*const_cast<const value*>(&other)) {}
  template <typename T>
  value(T&& v) noexcept
    requires(!std::derived_from<T, value>)
      : ptr_(std::make_unique<concrete_value_data<T> >(std::move(v))) {}
  template <typename T, typename... ARGS>
  value(std::in_place_type_t<T>, ARGS&&... args) noexcept
      : ptr_(std::make_unique<concrete_value_data<T> >(
            std::in_place, std::forward<ARGS>(args)...)) {}
  void* data() const { return ptr_->data_; }
  operator bool() const {
    return ptr_.operator bool();
  }  // false only after move!
  template <typename U>
  friend U* reconcrete_cast(value&);
  template <typename U>
  friend const U* reconcrete_cast(const value&);
};
template <typename U>
U* reconcrete_cast(value& u) {
  return reconcrete_cast<U, abstract_value_data_cloneable>(*u.ptr_);
}
template <typename U>
const U* reconcrete_cast(const value& u) {
  return reconcrete_cast<U, abstract_value_data_cloneable>(*u.ptr_);
}

template <typename T>
class typed_value : public value {
  template <typename T>
  friend auto as(value&& source);
  typed_value(value&& u) noexcept : value(std::move(u)) {}

 public:
  using conrete_t = T;
  using value::value;
  typed_value(T&& v) noexcept : value(std::move(v)) {}
  template <typename... ARGS>
  typed_value(std::in_place_t, ARGS&&... args) noexcept
      : value(std::in_place_type<T>, std::forward<ARGS>(args)...) {}
  T& operator*() const { return *static_cast<T*>(data()); }
  T* operator->() const { return static_cast<T*>(data()); }
};

template <typename T>
auto as(value&& source) {
  return typed_value<T>{std::move(source)};
}
template <typename T, typename... ARGS>
typed_value<T> make_value(ARGS&&... args) {
  return {std::in_place, std::forward<ARGS>(args)...};
}

struct make_value_t {
  template <typename FROM>
  value operator()(FROM&& from) {
    return make_value<std::remove_cvref_t<FROM> >(std::forward<FROM>(from));
  }
};

static_assert(erased::is_erased_lifetime_holder<value>);
static_assert(erased::is_erased_lifetime_holder<typed_value<int> >);

}  // namespace virtual_void::erased
#pragma once

#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>

namespace anypp {

template <is_erased_data TO, is_erased_data FROM>
struct borrow_trait;

template <typename TO, typename FROM>
concept borrowable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM f) {
      { borrow_trait<TO, FROM>{}(f) } -> std::same_as<TO>;
    };

template <is_erased_data FROM>
  requires(!is_const_data<FROM>)
struct borrow_trait<mutable_observer, FROM> {
  auto operator()(const auto& from) {
    return mutable_observer{get_void_data_ptr(from)};
  }
};
template <is_erased_data FROM>
struct borrow_trait<const_observer, FROM> {
  auto operator()(const auto& from) {
    return const_observer{get_void_data_ptr(from)};
  }
};
template <>
struct borrow_trait<shared_const, shared_const> {
  auto operator()(const auto& from) { return from; }
};
template <>
struct borrow_trait<weak, shared_const> {
  auto operator()(const auto& from) { return weak{from}; }
};

template <typename TO, typename FROM>
  requires borrowable_from<TO, FROM>
TO borrow_as(FROM const& from) {
  return borrow_trait<TO, FROM>{}(from);
}

static_assert(!borrowable_from<mutable_observer, const_observer>);
static_assert(borrowable_from<mutable_observer, mutable_observer>);
static_assert(borrowable_from<mutable_observer, unique>);
static_assert(!borrowable_from<mutable_observer, shared_const>);

static_assert(borrowable_from<const_observer, const_observer>);
static_assert(borrowable_from<const_observer, mutable_observer>);
static_assert(borrowable_from<const_observer, unique>);
static_assert(borrowable_from<const_observer, shared_const>);

static_assert(!borrowable_from<shared_const, const_observer>);
static_assert(!borrowable_from<shared_const, mutable_observer>);
static_assert(!borrowable_from<shared_const, unique>);
static_assert(borrowable_from<shared_const, shared_const>);

static_assert(!borrowable_from<unique, const_observer>);
static_assert(!borrowable_from<unique, mutable_observer>);
static_assert(!borrowable_from<unique, unique>);
static_assert(!borrowable_from<unique, shared_const>);

};  // namespace anypp
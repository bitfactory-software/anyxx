#pragma once

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct cast_trait;

template <typename TO, typename FROM>
concept castable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM f) {
      { cast_trait<TO, FROM>{}(f) } -> std::same_as<TO>;
    };

template <is_erased_data FROM>
  requires(!is_const_data<FROM>)
struct cast_trait<mutable_observer, FROM> {
  auto operator()(const auto& from) {
    return mutable_observer{get_void_data_ptr(from)};
  }
};
template <is_erased_data FROM>
struct cast_trait<const_observer, FROM> {
  auto operator()(const auto& from) {
    return const_observer{get_void_data_ptr(from)};
  }
};
template <>
struct cast_trait<shared_const, shared_const> {
  auto operator()(const auto& from) { return from; }
};

template <typename TO, typename FROM>
  requires castable_from<TO, FROM>
TO cast_to(FROM const& from) {
  return cast_trait<TO, FROM>{}(from);
}

static_assert(!castable_from<mutable_observer, const_observer>);
static_assert(castable_from<mutable_observer, mutable_observer>);
static_assert(castable_from<mutable_observer, unique>);
static_assert(!castable_from<mutable_observer, shared_const>);

static_assert(castable_from<const_observer, const_observer>);
static_assert(castable_from<const_observer, mutable_observer>);
static_assert(castable_from<const_observer, unique>);
static_assert(castable_from<const_observer, shared_const>);

static_assert(!castable_from<shared_const, const_observer>);
static_assert(!castable_from<shared_const, mutable_observer>);
static_assert(!castable_from<shared_const, unique>);
static_assert(castable_from<shared_const, shared_const>);

static_assert(!castable_from<unique, const_observer>);
static_assert(!castable_from<unique, mutable_observer>);
static_assert(!castable_from<unique, unique>);
static_assert(!castable_from<unique, shared_const>);

};  // namespace virtual_void::data
#pragma once

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/runtime/meta_data.hpp>

namespace virtual_void::data {

template <is_erased_data TO, is_erased_data FROM>
struct clone_trait;

template <typename TO, typename FROM>
concept cloneable_from =
    is_erased_data<FROM> && is_erased_data<TO> && requires(FROM const& f, runtime::meta_data& meta_data) {
      { clone_trait<TO, FROM>{}(f, meta_data) } -> std::same_as<TO>;
    };

template <is_erased_data FROM>
struct clone_trait<shared_const, FROM> {
  shared_const operator()(FROM const& from, auto const& meta_data) {
    return meta_data.copy_construct(get_void_data_ptr(from));
  }
};
template <is_erased_data FROM>
struct clone_trait<unique, FROM> {
  auto operator()(FROM const& from, auto const& meta_data) {
    return meta_data.copy_construct(get_void_data_ptr(from));
  }
};

template <is_erased_data TO, is_erased_data FROM>
  requires cloneable_from<TO, FROM>
TO clone_to(FROM const& from, auto const& meta_data) {
  return clone_trait<TO, FROM>{}(from, meta_data);
}

static_assert(!cloneable_from<mutable_observer, const_observer>);
static_assert(!cloneable_from<mutable_observer, mutable_observer>);
static_assert(!cloneable_from<mutable_observer, unique>);
static_assert(!cloneable_from<mutable_observer, shared_const>);

static_assert(!cloneable_from<const_observer, const_observer>);
static_assert(!cloneable_from<const_observer, mutable_observer>);
static_assert(!cloneable_from<const_observer, unique>);
static_assert(!cloneable_from<const_observer, shared_const>);

static_assert(cloneable_from<shared_const, const_observer>);
static_assert(cloneable_from<shared_const, mutable_observer>);
static_assert(cloneable_from<shared_const, unique>);
static_assert(cloneable_from<shared_const, shared_const>);

static_assert(cloneable_from<unique, const_observer>);
static_assert(cloneable_from<unique, mutable_observer>);
static_assert(cloneable_from<unique, unique>);
static_assert(cloneable_from<unique, shared_const>);

};  // namespace virtual_void::data
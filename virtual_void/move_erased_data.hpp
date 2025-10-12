#pragma once

#include <virtual_void/observer.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/unique.hpp>

namespace virtual_void {

template <is_erased_data TO, is_erased_data FROM>
bool constexpr can_move_to_from = false;

template <typename TO, typename FROM>
concept moveable_from =
    is_erased_data<FROM> && is_erased_data<TO> && can_move_to_from<TO, FROM>;

template <is_erased_data X>
bool constexpr can_move_to_from<X, X> = true;

template <>
bool constexpr can_move_to_from<shared_const, unique> = true;

template <>
bool constexpr can_move_to_from<weak, shared_const> = true;

template <voidness TO, voidness FROM>
  requires const_correct_call<is_const_void<TO>, is_const_void<FROM>, is_weak_data<FROM>>
bool constexpr can_move_to_from<TO, FROM> = true;

template <typename TO, typename FROM>
  requires moveable_from<TO, std::decay_t<FROM>>
TO move_to(FROM&& from) {
  return std::move(from);
}

static_assert(!moveable_from<mutable_observer, const_observer>);
static_assert(moveable_from<mutable_observer, mutable_observer>);
static_assert(!moveable_from<mutable_observer, unique>);
static_assert(!moveable_from<mutable_observer, shared_const>);

static_assert(moveable_from<const_observer, const_observer>);
static_assert(moveable_from<const_observer, mutable_observer>);
static_assert(!moveable_from<const_observer, unique>);
static_assert(!moveable_from<const_observer, shared_const>);

static_assert(!moveable_from<shared_const, const_observer>);
static_assert(!moveable_from<shared_const, mutable_observer>);
static_assert(moveable_from<shared_const, unique>);
static_assert(moveable_from<shared_const, shared_const>);

static_assert(!moveable_from<unique, const_observer>);
static_assert(!moveable_from<unique, mutable_observer>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared_const>);

};  // namespace virtual_void
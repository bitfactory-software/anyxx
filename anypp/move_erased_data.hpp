#pragma once

#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>

namespace anypp {

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
static_assert(!moveable_from<mutable_observer, weak>);
static_assert(!moveable_from<mutable_observer, value>);

static_assert(moveable_from<const_observer, const_observer>);
static_assert(moveable_from<const_observer, mutable_observer>);
static_assert(!moveable_from<const_observer, unique>);
static_assert(!moveable_from<const_observer, shared_const>);
static_assert(!moveable_from<const_observer, weak>);
static_assert(!moveable_from<const_observer, value>);

static_assert(!moveable_from<shared_const, const_observer>);
static_assert(!moveable_from<shared_const, mutable_observer>);
static_assert(moveable_from<shared_const, unique>);
static_assert(moveable_from<shared_const, shared_const>);
static_assert(!moveable_from<shared_const, weak>);
static_assert(!moveable_from<shared_const, value>);

static_assert(!moveable_from<weak, const_observer>);
static_assert(!moveable_from<weak, mutable_observer>);
static_assert(!moveable_from<weak, unique>);
static_assert(moveable_from<weak, shared_const>);
static_assert(moveable_from<weak, weak>);
static_assert(!moveable_from<weak, value>);

static_assert(!moveable_from<unique, const_observer>);
static_assert(!moveable_from<unique, mutable_observer>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared_const>);
static_assert(!moveable_from<unique, weak>);
static_assert(!moveable_from<unique, value>);

static_assert(!moveable_from<value, const_observer>);
static_assert(!moveable_from<value, mutable_observer>);
static_assert(!moveable_from<value, unique>);
static_assert(!moveable_from<value, shared_const>);
static_assert(!moveable_from<value, weak>);
static_assert(moveable_from<value, value>);

};  // namespace anypp
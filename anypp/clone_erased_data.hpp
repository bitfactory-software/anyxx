#pragma once

#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>
#include <anypp/meta_data.hpp>

namespace virtual_void {

template <is_erased_data TOFROM>
struct can_copy_to;

template <typename TO>
concept cloneable_to =
    is_erased_data<TO> && trait<TO>::is_owner;

template <is_erased_data TO, is_erased_data FROM>
  requires cloneable_to<TO>
TO clone_to(FROM const& from, auto const& meta_data) {
    return meta_data.copy_construct(get_void_data_ptr(from));
}

static_assert(!cloneable_to<mutable_observer>);
static_assert(!cloneable_to<const_observer>);
static_assert(cloneable_to<shared_const>);
static_assert(cloneable_to<unique>);

};  // namespace virtual_void
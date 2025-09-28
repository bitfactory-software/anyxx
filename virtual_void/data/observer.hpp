#pragma once

#include <virtual_void/data/observer_trait.hpp>

namespace virtual_void::data {

template <typename VV_VOID>
using observer = VV_VOID;

using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

}

namespace virtual_void {
template <>
struct erased_data_trait<data::const_observer>
    : observer_trait<data::const_observer, data::const_observer> {};
template <>
struct erased_data_trait<data::mutable_observer>
    : observer_trait<data::mutable_observer, data::mutable_observer> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {

static_assert(erased_data_trait<const_observer>::is_const);
static_assert(!erased_data_trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<const_observer>);

}  // namespace virtual_void::data
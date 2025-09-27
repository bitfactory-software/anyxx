#pragma once

#include <virtual_void/data/decorated_ptr.hpp>
#include <virtual_void/data/observer_trait.hpp>

namespace virtual_void::data {

template <typename VV_VOID>
using observer = decorated_ptr<VV_VOID>;

using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

}

namespace virtual_void {
template <typename VV_VOID>
struct erased_data_trait<data::observer<VV_VOID>>
    : observer_trait<data::observer<VV_VOID>, VV_VOID> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {

static_assert(erased_data_trait<const_observer>::is_const);
static_assert(!erased_data_trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<const_observer>);

}  // namespace virtual_void::data
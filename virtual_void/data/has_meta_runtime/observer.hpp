#pragma once

#include <stdexcept>
#include <type_traits>

#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/observer_trait.hpp>
#include <virtual_void/data/has_meta_runtime/meta.hpp>

namespace virtual_void::data::has_meta_runtime {
template <typename VV_VOID>
using observer = data::observer<VV_VOID, meta>;
}

namespace virtual_void {
template <typename VV_VOID>
struct erased_data_trait<data::has_meta_runtime::observer<VV_VOID>>
    : observer_trait<data::has_meta_runtime::observer<VV_VOID>, VV_VOID,
                     data::has_meta_runtime::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_meta_runtime {

using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;
static_assert(erased_data_trait<const_observer>::is_const);
static_assert(!erased_data_trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
}  // namespace virtual_void::data::has_meta_runtime
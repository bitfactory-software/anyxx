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
template <typename V, typename VV_VOID>
using typed_observer = virtual_typed<V, observer<VV_VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;
static_assert(erased_data_trait<const_observer>::is_const);
static_assert(!erased_data_trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_virtual_typed<typed_const_observer<int>>);
static_assert(is_virtual_typed<typed_mutable_observer<int>>);
}  // namespace virtual_void::data::has_meta_runtime
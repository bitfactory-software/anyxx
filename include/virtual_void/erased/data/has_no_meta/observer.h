#pragma once

#include <stdexcept>
#include <type_traits>

#include "../observer_ptr.h"
#include "../observer_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_no_meta {

template <typename VOID>
using observer_ptr = data::observer_ptr<VOID, has_no_meta>;
}

namespace virtual_void::erased {
template <typename VOID>
struct data_trait<data::has_no_meta::observer_ptr<VOID>>
    : observer_trait<data::has_no_meta::observer_ptr<VOID>> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_no_meta {

template <typename VOID>
using observer = virtual_void<observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = virtual_typed<V, observer_ptr<VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;

static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_void<typed_const_observer<int>>);
static_assert(is_virtual_void<typed_mutable_observer<int>>);

}  // namespace virtual_void::erased::data::has_no_meta

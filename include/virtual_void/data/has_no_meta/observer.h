#pragma once

#include "../observer_ptr.h"
#include "../observer_trait.h"
#include "meta.h"

namespace virtual_void::data::has_no_meta {

template <typename VOID>
using observer_ptr = data::observer_ptr<VOID, meta>;
}

namespace virtual_void {
template <typename VOID>
struct virtual_void_trait<data::has_no_meta::observer_ptr<VOID>>
    : observer_trait<data::has_no_meta::observer_ptr<VOID>> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
template <typename VOID>
using observer = observer_ptr<VOID>;
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
//static_assert(std::is_same_v<const_observer::void_t,void const*>);
//static_assert(std::is_same_v<observer_trait<const_observer::data_t>::void_t,void const*>);
//static_assert(is_const_void<void const*>::value);
//static_assert(is_const_void<observer_trait<const_observer::data_t>::void_t>::value);
static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<mutable_observer>);
static_assert(is_virtual_void<const_observer>);
static_assert(is_virtual_typed<typed_const_observer<int>>);
static_assert(is_virtual_typed<typed_mutable_observer<int>>);
}  // namespace virtual_void::data::has_no_meta

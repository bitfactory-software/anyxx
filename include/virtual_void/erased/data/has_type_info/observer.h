#pragma once

#include <stdexcept>
#include <type_traits>

#include "meta.h"
#include "../observer_trait.h"
#include "../observer_ptr.h"

namespace virtual_void::typeid_ {
template <typename VOID>
using observer_ptr =
    erased::data::observer_ptr<VOID, erased::data::has_type_info>;
}

namespace virtual_void::erased {
template <typename VOID>
struct data_trait<typeid_::observer_ptr<VOID>>
    : observer_trait<typeid_::observer_ptr<VOID>> {};

}  // namespace virtual_void::erased

namespace virtual_void::typeid_ {

template <typename VOID>
using observer = erased::virtual_void<typeid_::observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = erased::virtual_typed<V, typeid_::observer_ptr<VOID>>;
template <typename V>
using typed_const_observer = typed_observer<V, void const*>;
template <typename V>
using typed_mutable_observer = typed_observer<V, void*>;

static_assert(const_observer::is_const);
static_assert(!mutable_observer::is_const);

static_assert(erased::is_virtual_void<const_observer>);
static_assert(erased::is_virtual_void<mutable_observer>);
static_assert(erased::is_virtual_void<typed_const_observer<int>>);
static_assert(erased::is_virtual_void<typed_mutable_observer<int>>);

}  // namespace virtual_void::typeid_

#pragma once

#include <stdexcept>
#include <type_traits>

#include "meta.h"
#include "../observer_trait.h"
#include "../observer_ptr.h"

namespace virtual_void::erased::data::has_m_table {
template <typename VOID>
using observer_ptr =
    data::observer_ptr<VOID, has_m_table>;
}

namespace virtual_void::erased {

template <typename VOID>
struct data_trait<data::has_m_table::observer_ptr<VOID>>
    : observer_trait<data::has_m_table::observer_ptr<VOID>> {};

}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_m_table {

template <typename VOID>
using observer = erased::virtual_void<observer_ptr<VOID>>;
using const_observer = observer<void const*>;
using mutable_observer = observer<void*>;

template <typename V, typename VOID>
using typed_observer = erased::virtual_typed<V, m_table::observer_ptr<VOID>>;
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

}  // namespace virtual_void::m_table
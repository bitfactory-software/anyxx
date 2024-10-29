#pragma once

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_no_meta {
using value_DATA = data::value_ptr<allocation_base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<data::has_no_meta::value_DATA>
    : value_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_no_meta {
using value = virtual_void<value_DATA>;
template <typename T>
using typed_value = virtual_typed<T, value_DATA>;
static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::erased::data::has_no_meta
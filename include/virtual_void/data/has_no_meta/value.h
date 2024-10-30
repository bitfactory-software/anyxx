#pragma once

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::data::has_no_meta {
using value_data = data::value_ptr<allocation_base<meta>>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_no_meta::value_data>
    : value_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
using value = virtual_void<value_data>;
template <typename T>
using typed_value = virtual_typed<T, value_data>;
static_assert(is_virtual_void<value>);
static_assert(is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::data::has_no_meta
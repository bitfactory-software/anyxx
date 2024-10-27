#pragma once

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_no_meta {
using value_data_ptr = data::value_ptr<base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<data::has_no_meta::value_data_ptr>
    : value_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_no_meta {
using value = virtual_void<value_data_ptr>;
template <typename T>
using typed_value = virtual_typed<T, value_data_ptr>;
static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::erased::data::has_no_meta
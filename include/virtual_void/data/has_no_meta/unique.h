#pragma once

#include "../unique_trait.h"
#include "meta.h"

namespace virtual_void::data::has_no_meta {
using unique = erased_unique<decoration_base<meta>>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_no_meta::unique>
    : unique_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
template <typename T>
using typed_unique = virtual_typed<T, unique>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_typed<typed_unique<int>>);
}  // namespace virtual_void::data::has_no_meta
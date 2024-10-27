#pragma once

#include "../unique_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_no_meta {
using unique_DATA = data::unique_ptr<base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<data::has_no_meta::unique_DATA>
    : unique_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_no_meta {
using unique = virtual_void<unique_DATA>;
template <typename T>
using typed_unique = virtual_typed<T, unique_DATA>;
static_assert(erased::is_virtual_void<unique>);
static_assert(erased::is_virtual_void<typed_unique<int> >);
}  // namespace virtual_void::erased::data::has_no_meta
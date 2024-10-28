#pragma once

#include <stdexcept>
#include <type_traits>

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_type_info {
using value_DATA =
    data::value_ptr<base<meta>>;
}

namespace virtual_void::erased {
using namespace virtual_void;
template <>
struct virtual_void_trait<data::has_type_info::value_DATA>
    : value_trait<data::has_type_info::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_type_info {
using value = erased::virtual_void<value_DATA>;
template <typename T>
using typed_value = erased::virtual_typed<T, value_DATA>;
static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::erased::data::has_type_info
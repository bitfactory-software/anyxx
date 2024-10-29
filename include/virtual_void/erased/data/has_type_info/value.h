#pragma once

#include <stdexcept>
#include <type_traits>

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_type_info {
using value_data =
    data::value_ptr<allocation_base<meta>>;
}

namespace virtual_void::erased {
using namespace virtual_void;
template <>
struct virtual_void_trait<data::has_type_info::value_data>
    : value_trait<data::has_type_info::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_type_info {
using value = erased::virtual_void<value_data>;
template <typename T>
using typed_value = erased::virtual_typed<T, value_data>;
static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::erased::data::has_type_info
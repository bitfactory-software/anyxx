#pragma once

#include "../unique_trait.h"
#include "meta.h"

namespace virtual_void::data::has_type_info {
using unique_data =
    data::unique_ptr<allocation_base<meta>>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::unique_data>
    : unique_trait<data::has_type_info::meta> {};

}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
using unique = virtual_void<unique_data>;
template <typename T>
using typed_unique = virtual_typed<T, unique_data>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_void<typed_unique<int> >);
}  // namespace virtual_void::data::has_type_info
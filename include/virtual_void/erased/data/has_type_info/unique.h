#pragma once

#include "../unique_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_type_info {
using unique_data_ptr =
    erased::data::unique_ptr<erased::data::has_type_info::with_type_info>;
}

namespace virtual_void::erased {
template <>
struct data_trait<data::has_type_info::unique_data_ptr>
    : unique_trait<data::has_type_info::has_type_info> {};

}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_type_info {
using unique = erased::virtual_void<unique_data_ptr>;
template <typename T>
using typed_unique = erased::virtual_typed<T, unique_data_ptr>;
static_assert(erased::is_virtual_void<unique>);
static_assert(erased::is_virtual_void<typed_unique<int> >);
}  // namespace virtual_void::erased::data::has_type_info
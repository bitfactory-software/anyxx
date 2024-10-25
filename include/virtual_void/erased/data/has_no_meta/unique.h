#pragma once

#include "../unique_trait.h"

namespace virtual_void::erased {

using unique_data_ptr = data::unique_ptr<data::with_no_meta>;

template <>
struct data_trait<unique_data_ptr> : unique_trait<data::has_no_meta> {
};

using unique = virtual_void<unique_data_ptr>;
template <typename T>
using typed_unique = virtual_typed<T, unique_data_ptr>;

static_assert(erased::is_virtual_void<unique>);
static_assert(erased::is_virtual_void<typed_unique<int> >);

}  // namespace virtual_void::erased
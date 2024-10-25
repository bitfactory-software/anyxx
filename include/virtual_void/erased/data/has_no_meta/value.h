#pragma once

#include <stdexcept>
#include <type_traits>

#include "has_no_meta.h"
#include "../value_trait.h"

namespace virtual_void::erased {

using value_data_ptr = data::value_ptr<data::with_no_meta>;

template <>
struct data_trait<value_data_ptr> : value_trait<data::has_no_meta> {
};

using value = virtual_void<value_data_ptr>;
template <typename T>
using typed_value = virtual_typed<T, value_data_ptr>;

static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);

}  // namespace virtual_void::erased
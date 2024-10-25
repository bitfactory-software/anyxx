#pragma once

#include <stdexcept>
#include <type_traits>

#include "meta.h"
#include "../value_trait.h"

namespace virtual_void::typeid_ {
using value_data_ptr = erased::data::value_ptr<erased::data::with_type_info>;
}

namespace virtual_void::erased {
using namespace virtual_void;
template <>
struct data_trait<typeid_::value_data_ptr> : value_trait<data::has_type_info> {};
}  // namespace virtual_void::erased

namespace virtual_void::typeid_ {

using value = erased::virtual_void<value_data_ptr>;
template <typename T>
using typed_value = erased::virtual_typed<T, value_data_ptr>;

static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);

}  // namespace virtual_void::typeid_
#pragma once

#include "has_type_info.h"
#include "../shared_const_trait.h"

namespace virtual_void::typeid_ {
using shared_const_data_ptr =
    erased::data::shared_const_ptr<erased::data::with_type_info>;
}
namespace virtual_void::erased {
template <>
struct data_trait<typeid_::shared_const_data_ptr>
    : shared_const_trait<data::has_type_info> {};
}  // namespace virtual_void::erased
namespace virtual_void::typeid_ {

static_assert(erased::is_data_pointer<shared_const_data_ptr>);

using shared_const = erased::virtual_void<shared_const_data_ptr>;
template <typename T>
using typed_shared_const =
    erased::virtual_typed<T const, shared_const_data_ptr>;

static_assert(erased::is_virtual_void<shared_const>);
static_assert(erased::is_virtual_void<typed_shared_const<int>>);

}  // namespace virtual_void::typeid_